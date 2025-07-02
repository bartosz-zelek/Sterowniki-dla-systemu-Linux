/*
 * Vmxmon - export the Intel(R) Virtualization Technology (Intel(R) VT) for
 * IA-32, Intel(R) 64 and Intel(R) Architecture (Intel(R) VT-x) to user-space.
 * Copyright 2015-2022 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 */

#include "tphys.h"
#include "kernel-api.h"
#include "cpudefs.h"
#include "vmxmon.h"
#include "vm.h"
#include "asm-inlines.h"
#include "simcall.h"
#include "mm.h"
#include "tlb.h"
#include "nested-ept.h"

#define NPAGES_MAPPED_LIMIT                     30000

static const unsigned undefined_level = 0xbaad;
typedef struct {
        mpage_t         ept;            /* EPT page */
        void            **children;     /* next level */
        unsigned        level;          /* 1 to 5 or undefined_level */
} ept_level_t;

struct page_usage {                     /* page -> usage mapping */
        page_usage_t    **backlink;     /* *backlink == ptr_to_us */
        page_usage_t    *next;          /* next page usage (linked list) */

        mlevel_t        *owner;         /* mlevel owner of mapping */
        u32             qual;           /* offset into segment */
        u32             pflags;         /* PFLAG_xxx */
};

#define PU_STRUCTS_PER_PAGE \
        ((0x1000 - sizeof(void *)) / sizeof(page_usage_t))

typedef struct tphys_alloc_page {
        page_usage_t pu_structs[PU_STRUCTS_PER_PAGE];
        struct tphys_alloc_page *next;
} tphys_alloc_page_t;

typedef struct {
        page_usage_t       *free_list;
        tphys_alloc_page_t *pages;
        u64                n_allocked;
} tphys_alloc_t;

/* guest physical address space */
struct tphys_space {
        /* mutex protecting this tphys space */
        mutex_t         *tphys_mutex;

        u64             eptp;                   /* physical address of EPT */
        bool            five_level_walk;

        ept_level_t     t2p_root;               /* tphys -> vmxpage table */

        /* shared userpage, used for invalidations */
        upage_t         shared_com_page;
        vmxmon_shared_t *shared_com;

        /* this tphys is shared by VMs in the linked list */
        vm_t            *first_vm;              /* next: vm->tphys_next_vm */

        /* custom allocator for page_usage_t */
        tphys_alloc_t   tphys_alloc;

        /* statistics */
        unsigned        n_pages_mapped;         /* #pages mapped */
};

static struct {
        unsigned        n_pages_locked;
} st;


#define LEV1_IND(pa)          (((pa) >> 12) & 0x1ff)
#define LEV2_IND(pa)          (((pa) >> 21) & 0x1ff)
#define LEV3_IND(pa)          (((pa) >> 30) & 0x1ff)
#define LEV4_IND(pa)          (((pa) >> 39) & 0x1ff)
#define LEV5_IND(pa)          (((pa) >> 48) & 0x1ff)

static inline bool
is_target_paddr_supported(tphys_t pa, tphys_space_t *s)
{
        u64 supported_width = s->five_level_walk? 52: 48;
        return (pa >> supported_width) == 0;
}


#define WPROTECT_TABLE_SIZE ((0x1000 - sizeof(vmxmon_shared_t)) / sizeof (u64))

static inline void
set_ept_pte(ept_level_t *lev, unsigned index, u64 val)
{
        u64 *pte = (u64 *)lev->ept.p;
        pte[index] = val;
}

static inline void
clear_ept(ept_level_t *lev, unsigned index)
{
        u64 *pte = (u64 *)lev->ept.p;
        pte[index] = 0;
}

static void
flush_EPT(tphys_space_t *s)
{
        vm_t *vm;
        for (vm = s->first_vm; vm; vm = vm->tphys_next_vm)
                vm->entry_work |= EW_FLUSH_EPT;
}

static void
alloc_pu_structs(tphys_alloc_t *m_alloc)
{
        tphys_alloc_page_t *p = (tphys_alloc_page_t *)x_alloc_page();
        wrap_BUG_ON(m_alloc->free_list);
        if (p) {
                unsigned i = 0;
                for (i = 0; i < PU_STRUCTS_PER_PAGE - 1; i++)
                        p->pu_structs[i].next = &p->pu_structs[i + 1];

                p->next = m_alloc->pages;
                m_alloc->pages = p;
                m_alloc->free_list = &p->pu_structs[0];
        }
}

static page_usage_t *
zalloc_page_usage(tphys_space_t *s)
{
        tphys_alloc_t *m_alloc = &s->tphys_alloc;

        if (!m_alloc->free_list) {
                alloc_pu_structs(m_alloc);
                if (!m_alloc->free_list)
                        return NULL;
        }
        page_usage_t *pu = m_alloc->free_list;
        m_alloc->free_list = pu->next;
        wrap_memset(pu, 0, sizeof *pu);
        m_alloc->n_allocked++;
        return pu;
}

static void
free_page_usage(tphys_space_t *s, page_usage_t *pu)
{
        tphys_alloc_t *m_alloc = &s->tphys_alloc;
        wrap_BUG_ON(m_alloc->n_allocked == 0);
        pu->next = m_alloc->free_list;
        m_alloc->free_list = pu;
        m_alloc->n_allocked--;

        if (m_alloc->n_allocked == 0) {
                while (m_alloc->pages) {
                        tphys_alloc_page_t *p = m_alloc->pages;
                        m_alloc->pages = p->next;
                        x_free_page((char *)p);
                }
                m_alloc->free_list = NULL;
        }
}


/************************************************************************/
/* Target physical page map and lookup subsystem. It uses a tree structure
   with levels that are compatible with host EPT layout. When host EPT is
   used to map target physical pages, host's EPTP points to this tree and
   it is used when running in VMX non-root mode. When shadow paging is
   used, this tree is only used for lookups by the driver itself */
/************************************************************************/

static ept_level_t *
allocate_level_members(ept_level_t *lev)
{
        wrap_memset(lev, 0, sizeof *lev);
        lev->children = (void *)x_alloc_page();
        alloc_mpage(&lev->ept);

        if (!lev->children || !lev->ept.p) {
                free_mpage(&lev->ept);
                if (lev->children) {
                        x_free_page((char *)lev->children);
                }
                return NULL;
        }
        return lev;
}

static void
link_to_parent_level(ept_level_t *lev, ept_level_t *parent, unsigned par_index)
{
        if (parent->level == undefined_level) {
                printm("undefined top level. Fallback to 4\n");
                parent->level = 4;
        }
        lev->level = parent->level - 1;
        parent->children[par_index] = lev;
        u64 ent = mpage_phys(&lev->ept)
                | EPT_R_ACCESS | EPT_W_ACCESS | EPT_XS_ACCESS;
        set_ept_pte(parent, par_index, ent);
}

static ept_level_t *
init_intermediate_level(ept_level_t *lev, ept_level_t *parent, unsigned par_index)
{
        wrap_BUG_ON(!parent);
        lev = allocate_level_members(lev);
        if (!lev) {
                return NULL;
        }
        link_to_parent_level(lev, parent, par_index);
        return lev;
}

static ept_level_t *
init_root_level(ept_level_t *lev)
{
        lev = allocate_level_members(lev);
        if (!lev) {
                return NULL;
        }
        /* Top level value depends on what the userspace client wants.
           It will be set up by the vmxmon setup phase. */
        lev->level = undefined_level;
        return lev;
}

static ept_level_t *
alloc_intermediate_level(ept_level_t *par, unsigned par_index)
{
        ept_level_t *lev = x_malloc(sizeof *lev);
        if (!lev)
                return NULL;
        if (!init_intermediate_level(lev, par, par_index)) {
                x_free(lev);
                lev = NULL;
        }
        return lev;
}

static void
free_ept_level(ept_level_t *lev)
{
        /* release sublevels */
        if (lev->level != 1) {
                unsigned i;
                for (i = 0; i < 512; i++) {
                        if (lev->children[i]) {
                                free_ept_level(lev->children[i]);
                                x_free(lev->children[i]);
                        }
                }
        }
        x_free_page((char *)lev->children);
        free_mpage(&lev->ept);
}

/* return pointer to lowest ept level; allocate intermediate levels as needed */
static ept_level_t *
ept4_establish_level1(ept_level_t *lev4_root, u64 pa)
{
        unsigned l4 = LEV4_IND(pa);
        ept_level_t *lev3 = lev4_root->children[l4];
        if (!lev3) {
                lev3 = alloc_intermediate_level(lev4_root, l4);
                if (!lev3)
                        return NULL;
        }

        unsigned l3 = LEV3_IND(pa);
        ept_level_t *lev2 = lev3->children[l3];
        if (!lev2) {
                lev2 = alloc_intermediate_level(lev3, l3);
                if (!lev2)
                        return NULL;
        }

        unsigned l2 = LEV2_IND(pa);
        ept_level_t *lev1 = lev2->children[l2];
        if (!lev1) {
                lev1 = alloc_intermediate_level(lev2, l2);
                if (!lev1)
                        return NULL;
        }
        return lev1;
}

static ept_level_t *
lookup_level4(ept_level_t *t2p_root, tphys_t pa)
{
        if (t2p_root->level == 5 ) {
                unsigned l5 = LEV5_IND(pa);
                return t2p_root->children[l5];
        } else {
                return t2p_root;
        }
}

static ept_level_t *
ept_lookup_lev1(ept_level_t *t2p_root, tphys_t pa)
{
        ept_level_t *lev4 = lookup_level4(t2p_root, pa);
        if (!lev4) {
                return NULL;
        }

        unsigned l4 = LEV4_IND(pa);
        unsigned l3 = LEV3_IND(pa);
        unsigned l2 = LEV2_IND(pa);

        ept_level_t *lev3 = lev4->children[l4];
        if (!lev3)
                return NULL;
        ept_level_t *lev2 = lev3->children[l3];
        if (!lev2)
                return NULL;
        return lev2->children[l2];
}

static vmx_page_t *
t2p_lookup(tphys_space_t *s, tphys_t pa)
{
        ept_level_t *lev1 = ept_lookup_lev1(&s->t2p_root, pa);

        if (!lev1)
                return NULL;
        unsigned l1 = LEV1_IND(pa);
        return (vmx_page_t *)lev1->children[l1];
}


static u64
make_epte(vmx_page_t *xp, rwx_t rwx)
{
        const u64 memory_type_writeback = 6;
        const u64 ignore_pat_memory_type = BIT(6);
        const u64 ept_entry_memory_attributes = (memory_type_writeback << 3)
                | ignore_pat_memory_type;
        u64 epte = ept_entry_memory_attributes | upage_phys(&xp->upage);
        if (rwx & RWX_R)
                epte |= EPT_R_ACCESS;
        if (rwx & RWX_W)
                epte |= EPT_W_ACCESS;
        if (rwx & RWX_XS)
                epte |= EPT_XS_ACCESS;
        if (rwx & RWX_XU)
                epte |= EPT_XU_ACCESS;
        return epte;
}

static inline u8
rwx_to_ecode(rwx_t v)
{
        return (v & RWX_W) | (v & RWX_X ? ECODE_I : 0);
}

static int
get_vmxpage(vm_t *vm, tphys_t pa, vmx_page_t **ret, rwx_t rwx)
{
        if (!is_target_paddr_supported(pa, vm->pspace)) {
                set_vmret(vm, VMRET_GUEST_STATE_UNSUPPORTED);
                return 1;
        }

        vmx_page_t *xp = t2p_lookup(vm->pspace, pa);

        /* miss? */
        if (xp == NULL) {
                vmxmon_t *x = vm->com;
                x->vmret_qual = pa & ~0xfffULL;
                if (x->active_vmxmon_features & VMXMON_FEATURE_REPORT_FETCH)
                        x->ecode = rwx_to_ecode(rwx);
                else
                        x->ecode = (rwx & RWX_W) ? ECODE_W : 0;
                set_vmret(vm, VMRET_PHYSPAGE_MISSING);
                return 1;
        }

        /* check protection */
        if ((xp->rwx & rwx) != rwx) {
                vmxmon_t *x = vm->com;
                x->ecode = rwx_to_ecode(rwx);
                if (x->active_vmxmon_features & VMXMON_FEATURE_REPORT_FETCH)
                        x->page_access_rights = xp->rwx;
                x->vmret_qual = pa & ~0xfffULL;
                set_vmret(vm, VMRET_PROTECTION);
                return 1;
        }
        *ret = xp;
        return 0;
}

/* lookup and map vmxpage */
int
vmxpage_lookup(vm_t *vm, tphys_t tphys, vmx_page_t **ret, rwx_t rwx)
{
        if (get_vmxpage(vm, tphys, ret, rwx))
                return 1;

        /* map (if necessary) */
        vmx_page_t *xp = *ret;
        if (!upage_is_mapped(&xp->upage)) {
                map_upage(&xp->upage);
                vm->pspace->n_pages_mapped++;
        }
        return 0;
}

int
vmxpage_lookup_nomap(vm_t *vm, tphys_t pa, vmx_page_t **ret, rwx_t rwx)
{
        return get_vmxpage(vm, pa, ret, rwx);
}

static void
update_ept_perm(tphys_space_t *s, vmx_page_t *xp)
{
        u64 pa = xp->tphys;
        ept_level_t *lev1 = ept_lookup_lev1(&s->t2p_root, pa);
        unsigned l1 = LEV1_IND(pa);

        if (lev1 && lev1->children[l1] == xp) {
                set_ept_pte(lev1, l1, make_epte(xp, xp->rwx));
        } else {
                printm("update_ept_perm: xp not mapped!\n");
        }
}

static rwx_t
prot_to_rwx(unsigned prot)
{
        rwx_t ret = RWX_ALL;
        if (prot & VMXMON_PROTECT_READ)
                ret &= ~(RWX_R | RWX_W);
        if (prot & VMXMON_PROTECT_WRITE)
                ret &= ~RWX_W;
        if (prot & VMXMON_PROTECT_EXECUTE)
                ret &= ~RWX_X;

        return ret;
}

static int
establish_4_lower_levels(tphys_space_t *s, ept_level_t *lev4, tphys_t tphys,
        uintptr_t va, unsigned prot)
{
        ept_level_t *lev1 = ept4_establish_level1(lev4, tphys);
        unsigned l1 = LEV1_IND(tphys);

        if (lev1 == NULL)
                return 1;

        vmx_page_t *xp = lev1->children[l1];
        if (xp)
                return tphys_protect_page(s, tphys, prot);

        xp = x_malloc(sizeof *xp);
        if (xp == NULL)
                return 1;

        wrap_memset(xp, 0, sizeof *xp);
        xp->tphys = tphys;
        xp->rwx = prot_to_rwx(prot);

        if (get_upage(va, &xp->upage)) {
                printm("vmxmon: get_upage failed\n");
                x_free(xp);
                return 1;
        }
        atomic_inc(&st.n_pages_locked);

        lev1->children[l1] = xp;

        update_ept_perm(s, xp);
        return 0;
}

static int
add_4_level_mapping(tphys_space_t *s, tphys_t tphys, uintptr_t va, unsigned prot)
{
        return establish_4_lower_levels(s, &s->t2p_root, tphys, va, prot);
}

static ept_level_t *
establish_level4(ept_level_t *lev5_root, tphys_t tphys)
{
        unsigned l5 = LEV5_IND(tphys);
        ept_level_t *lev4 = lev5_root->children[l5];
        if (!lev4) {
                lev4 = alloc_intermediate_level(lev5_root, l5);
        }
        return lev4;
}

static int
add_5_level_mapping(tphys_space_t *s, tphys_t tphys, uintptr_t va, unsigned prot)
{
        ept_level_t *lev5_root = &s->t2p_root;
        ept_level_t *lev4 = establish_level4(lev5_root, tphys);
        if (!lev4) {
                return 1;
        }
        return establish_4_lower_levels(s, lev4, tphys, va, prot);
}

static bool
is_tphys_space_configured(tphys_space_t *s)
{
        /* Setup must have been called and the target should have indicated
           which page walk length it wants */
        return s->t2p_root.level != undefined_level;
}

int
tphys_add_mapping(tphys_space_t *s, tphys_t tphys, uintptr_t va, unsigned prot)
{
        if (!is_tphys_space_configured(s)) {
                return 1;
        }
        if (!is_target_paddr_supported(tphys, s))
                return 1;

        /* Ensure that queued up invalidations are performed first. */
        tphys_perform_lazy_flushing(s);

        if ((va | tphys) & 0xfff) {
                wrap_printk("tphys_add_mapping: misaligned addresses\n");
                return 1;
        }

        if (s->five_level_walk) {
                return add_5_level_mapping(s, tphys, va, prot);
        } else {
                return add_4_level_mapping(s, tphys, va, prot);
        }
}

/* vmxpage must already be unlinked from group */
static void
do_free_vmxpage(tphys_space_t *s, vmx_page_t *xp)
{
        ept_level_t *lev1 = ept_lookup_lev1(&s->t2p_root, xp->tphys);
        unsigned l1 = LEV1_IND(xp->tphys);

        wrap_BUG_ON((lev1 == NULL
                     || lev1->children[l1] == NULL
                     || xp->guest_page
                     || xp->users != NULL));

        lev1->children[l1] = NULL;
        clear_ept(lev1, l1);

        if (upage_is_mapped(&xp->upage)) {
                unmap_upage(&xp->upage);
                s->n_pages_mapped--;
        }
        atomic_dec(&st.n_pages_locked);
        put_upage(&xp->upage);

        x_free(xp);
}


/************************************************************************/
/*	user protection handling      					*/
/************************************************************************/

static void
_flush_write_mappings(tphys_space_t *s, vmx_page_t *xp)
{
        const unsigned key = PFLAG_RAM | PFLAG_WRITABLE;

        page_usage_t *m, *next;
        for (m = xp->users; m ; m = next) {
                next = m->next;
                if ((m->pflags & key) != key)
                        continue;
                m->pflags &= ~PFLAG_WRITABLE;
                _tlb_write_protect_page(m->owner, m->qual);
        }
        xp->rwx &= ~RWX_W;

        update_ept_perm(s, xp);
}

static void
clear_all_tlbs(tphys_space_t *s)
{
        vm_t *vm;
        for (vm = s->first_vm; vm; vm = vm->tphys_next_vm) {
                if (vm->tlb)
                        tlb_clear(vm);
        }
}

/* remove all usage of the vmx page */
static void
remove_page_users(tphys_space_t *s, vmx_page_t *xp)
{
        /* iterate until all page usages have been released */
        while (xp->users) {
                page_usage_t *m = xp->users;
                if (!(m->pflags & PFLAG_RAM)) {
                        /* this page is used for paging (PFLAG_PTABLE);
                           we need to clear all tlbs to ensure all
                           references have been dropped */
                        clear_all_tlbs(s);
                        wrap_BUG_ON(xp->users);
                        break;
                }
                _tlb_unmap_page(m->owner, m->qual);
                vmxpage_drop_usage(s, m);
                /* m is released at this point */
        }
        if (xp->guest_page)
                free_vmxpage_guest_pages(xp);
}

/* ensure that all page users respect the 'rwx' access setting for the page */
static void
update_page_user_perm(tphys_space_t *s, vmx_page_t *xp)
{
        const rwx_t rwx = xp->rwx;

        /* drop ram mappings (if permissions have been removed) */
        page_usage_t *m;
        for (m = xp->users; m ; m = m->next) {
                if (!(m->pflags & PFLAG_RAM))
                        continue;

                if (!(rwx & RWX_X))
                        _tlb_make_page_noexec(m->owner, m->qual);

                if (!(rwx & RWX_W) && (m->pflags & PFLAG_WRITABLE)) {
                        m->pflags &= ~PFLAG_WRITABLE;
                        _tlb_write_protect_page(m->owner, m->qual);
                }
        }
        if (xp->guest_page)
                update_vmxpage_guest_perm(xp);
}


/* Set access permissions for the page 'xp' to 'rwx'.
   The page is released if neither RWX_R or RWX_X is set. */
static void
_tphys_protect_page(tphys_space_t *s, vmx_page_t *xp, rwx_t rwx)
{
        bool flush = xp->rwx & ~rwx;
        xp->rwx = rwx;

        update_ept_perm(s, xp);

        if (!flush)
                return;

        if (!(rwx & (RWX_R | RWX_X))) {
                /* handle page removal */
                remove_page_users(s, xp);
                do_free_vmxpage(s, xp);
        } else {
                /* permissions have been dropped */
                update_page_user_perm(s, xp);
        }
        flush_EPT(s);
}

int
tphys_protect_page(tphys_space_t *s, tphys_t pa, unsigned prot)
{
        vmx_page_t *xp = t2p_lookup(s, pa);
        rwx_t rwx = prot_to_rwx(prot);

        /* we might not have been told about the page yet */
        if (!xp)
                return 1;

        _tphys_protect_page(s, xp, rwx);
        return 0;
}



/************************************************************************/
/*	dependency tracking                     			*/
/************************************************************************/

page_usage_t *
vmxpage_add_usage(vm_t *vm, vmx_page_t *xp, mlevel_t *owner,
                  unsigned qual, unsigned pflags)
{
        page_usage_t *p = zalloc_page_usage(vm->pspace);
        if (!p)
                return NULL;

        p->owner = owner;
        p->qual = qual;
        p->pflags = pflags;

        /* add first in linked list of vmxpage users */
        p->backlink = &xp->users;
        p->next = xp->users;
        if (xp->users)
                xp->users->backlink = &p->next;
        xp->users = p;
        return p;
}

void
vmxpage_drop_usage(tphys_space_t *s, page_usage_t *m)
{
        *m->backlink = m->next;
        if (m->next)
                m->next->backlink = m->backlink;
        free_page_usage(s, m);
}


/************************************************************************/
/*	page unmapping          					*/
/************************************************************************/

static void
for_all_pages(tphys_space_t *s, ept_level_t *lev,
              void (*callback)(tphys_space_t *, vmx_page_t *))
{
        if (lev->level == 1) {
                unsigned i;
                for (i = 0; i < 512; i++) {
                        if (lev->children[i])
                                callback(s, lev->children[i]);
                }
        } else {
                unsigned i;
                for (i = 0; i < 512; i++) {
                        if (lev->children[i])
                                for_all_pages(s, lev->children[i], callback);
                }
        }
}

static void
tphys_flush_write_mappings(tphys_space_t *s)
{
        for_all_pages(s, &s->t2p_root, _flush_write_mappings);
        flush_EPT(s);
}

static void
tphys_unmap_all(tphys_space_t *s)
{
        vm_t *vm;
        for (vm = s->first_vm; vm; vm = vm->tphys_next_vm)
                drop_all_guest_pages(vm);

        clear_all_tlbs(s);
        flush_EPT(s);

        for_all_pages(s, &s->t2p_root, do_free_vmxpage);
}

void
vmxpage_memory_limit_prune(vm_t *vm)
{
        tphys_space_t *s = vm->pspace;

        /* total memory exceeded */
        if (st.n_pages_locked / 256 > mp.mem_limit_mb) {
                tphys_unmap_all(s);
                BUMP(PERFCTR_N_MEMORY_LIMIT_PRUNE);
        }
        
        /* too many pages mapped into kernel memory? */
        if (s->n_pages_mapped > NPAGES_MAPPED_LIMIT) {
                tphys_unmap_all(s);
                BUMP(PERFCTR_N_MAPPED_LIMIT_PRUNE);
        }

        BUMP_SET(PERFCTR_N_MAPPED_PAGES, s->n_pages_mapped);
        BUMP_SET(PERFCTR_N_LOCKED_PAGES, st.n_pages_locked);
}

static void
drop_permissions(tphys_space_t *s, u64 pa, rwx_t lost_access)
{
        vmx_page_t *xp = t2p_lookup(s, pa);
        if (!xp || !(xp->rwx & lost_access))
                return;
        _tphys_protect_page(s, xp, xp->rwx & ~lost_access);
}

static inline rwx_t
invalidation_entry_to_rwx(u64 entry)
{
        rwx_t access = entry & 7;
        /* API backward compatibility */
        if (access == 0)
                access = RWX_W;
        if (access & RWX_XS)
                access |= RWX_XU;
        return access;
}

static void
drop_page_permissions(tphys_space_t *s)
{
        vmxmon_shared_t *sx = s->shared_com;
        if (!utable_empty(&sx->wprotect_pages)) {
                unsigned i, end;
                get_utable_range_and_clear(&sx->wprotect_pages, &i, &end,
                                           WPROTECT_TABLE_SIZE);
                for (; i < end; i++) {
                        u64 entry = s->shared_com->data[i];
                        u64 pa = entry & ~0xfff;
                        rwx_t lost_access = invalidation_entry_to_rwx(entry);
                        drop_permissions(s, pa, lost_access);
                }
        }
}

void
tphys_perform_lazy_flushing(tphys_space_t *s)
{
        vmxmon_shared_t *sx = s->shared_com;
        if (sx->flush_all_pages) {
                tphys_unmap_all(s);
                sx->flush_all_pages = false;
                sx->flush_all_write_mappings = false;
                clear_utable(&sx->wprotect_pages);
                return;
        }
        if (sx->flush_all_write_mappings) {
                tphys_flush_write_mappings(s);
                sx->flush_all_write_mappings = false;
        }
        drop_page_permissions(s);
}


u64
tphys_get_eptp(const tphys_space_t *s)
{
        return s->eptp;
}

/************************************************************************/
/*	init / cleanup  						*/
/************************************************************************/

int
tphys_setup_shared_vmxmon(tphys_space_t *s, uintptr_t user_page)
{
        if (s->shared_com != NULL)
                return 0;

        if (get_upage(user_page, &s->shared_com_page))
                return -EFAULT;
        s->shared_com = (vmxmon_shared_t *)map_upage(&s->shared_com_page);
        if (s->shared_com == NULL)
                return -ENOMEM;

        vmxmon_shared_t *sx = s->shared_com;
        sx->wprotect_pages.start = 0;
        sx->wprotect_pages.max_size = WPROTECT_TABLE_SIZE;
        return 0;
}

static void
unmap_shared_vmxmon(tphys_space_t *s)
{
        put_upage(&s->shared_com_page);
        s->shared_com = NULL;
}

void
tphys_unmap_shared_vmxmon(tphys_space_t *s)
{
        unmap_shared_vmxmon(s);
}

u64
encoded_eptp_walk_length(tphys_space_t *s)
{
        static const u64 four_level_walk_length = (4-1) << 3;
        static const u64 five_level_walk_length = (5-1) << 3;
        return s->five_level_walk? five_level_walk_length
                :four_level_walk_length;
}

int
tphys_init(vm_t *vm)
{
        tphys_space_t *s = x_malloc(sizeof *s);
        if (!s)
                return 1;
        wrap_memset(s, 0, sizeof *s);

        /* init root paging level */
        if (!init_root_level(&s->t2p_root)) {
                x_free(s);
                return 1;
        }
        s->eptp = mpage_phys(&s->t2p_root.ept);

        /* init tphys_mutex */
        s->tphys_mutex = wrap_alloc_mutex();
        if (!s->tphys_mutex) {
                free_ept_level(&s->t2p_root);
                x_free(s);
                return 1;
        }
        /* s->five_level_walk is still not configured. Only after REQ_SETUP is
           done and tphys_update_walk_length() is called it becomes known what
           length of table walk the client has chosen */

        s->first_vm = vm;
        vm->pspace = s;
        return 0;
}

void
tphys_update_walk_length(tphys_space_t *s, bool use_5_level_pages)
{
        s->five_level_walk = use_5_level_pages;
        if (use_5_level_pages) {
                s->t2p_root.level = 5;
        } else {
                s->t2p_root.level = 4;
        }
}

void
tphys_share(vm_t *vm, vm_t *pspace_vm)
{
        tphys_space_t *s = pspace_vm->pspace;

        vm->tphys_next_vm = s->first_vm;
        s->first_vm = vm;

        vm->pspace = s;
}

/* Must be called only if tphys is locked */
void
tphys_unlink_and_unlock(vm_t *vm)
{
        tphys_space_t *s = vm->pspace;
        tphys_unmap_all(s);

        /* unlink */
        vm->pspace = NULL;
        vm_t **pp = &s->first_vm;
        for (; *pp != vm; pp = &(**pp).tphys_next_vm)
                ;
        *pp = vm->tphys_next_vm;

        /* this pspace might still be shared with other VMs */
        bool pspace_used = s->first_vm != NULL;
        tphys_unlock(s);

        if (pspace_used)
                return;

        unmap_shared_vmxmon(s);

        wrap_free_mutex(s->tphys_mutex);
        free_ept_level(&s->t2p_root);
        x_free(s);
}

void
tphys_lock(tphys_space_t *s)
{
        wrap_mutex_lock_nested(s->tphys_mutex, PSPACE_LEVEL);
}

void
tphys_unlock(tphys_space_t *s)
{
        wrap_mutex_unlock(s->tphys_mutex);
}
