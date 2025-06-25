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

#include "kernel-api.h"

#include "cpudefs.h"
#include "vmxmon.h"
#include "vm.h"
#include "vmx.h"
#include "tlb.h"

typedef struct tlb              tlb_t;
typedef struct lin_bp           lin_bp_t;

#define N_SEGMENT_BUCKETS       128
#define SEGMENT_LIMIT           512     /* flush initiated, if exceeded */

enum {
        PT_4_LEVEL_PML4,                 /* [4-level] PML4 */
        PT_4_LEVEL_PDPT,                 /* [4-level] page dir ptr table */
        PT_4_LEVEL_PD,                   /* [4-level] page directory */
        PT_4_LEVEL_PT,                   /* [4-level] page table */

        PT_PAE_PD,                       /* [PAE] page directory */
        PT_PAE_PT,                       /* [PAE] page table */

        PT_CLASSIC_PD,                   /* [CLASSIC] page directory */
        PT_CLASSIC_PT,                   /* [CLASSIC] page table */

        PT_TRIVIAL_PD,                   /* [NOMMU] page directory */
        PT_TRIVIAL_PT,                   /* [NOMMU] page table */

        PT_LM_PDPT,                      /* [32-bit paging] PDPT */

        PT_5_LEVEL_PML5,
        PT_5_LEVEL_PML4,
        PT_5_LEVEL_PDPT,
        PT_5_LEVEL_PD,
        PT_5_LEVEL_PT,
};

enum {
        TLB_MODE_NP,
        TLB_MODE_CLASSIC,
        TLB_MODE_PAE,
        TLB_MODE_LONG,
};

struct lin_bp {
        int             id;             /* user visible breakpoint id */
        int             prot;           /* VMXMON_PROT_xxx */
        uintptr_t       base;           /* base address of breakpoint */
        unsigned long   n_pages;        /* #pages */
        lin_bp_t        *next;
};

struct mlevel {
        tlb_t           *tlb;           /* tlb we belong to */

        mlevel_t        *parent;        /* parent or NULL */
        mlevel_t        *first_child;   /* child node or NULL */
        mlevel_t        *sib_next;      /* list with siblings... */

        void            **children;     /* next level (mask + 1 items) */
        mpage_t         ppage;          /* the page presented to the MMU */

        page_usage_t    *dep;           /* vmxpage dependency */
        char            *dep_page;      /* pointer to dependency page */
        char            *page_copy;     /* used for page table validation */

        unsigned short  mask;           /* 0x1ff or 0x3 (PAE level 3) */
        unsigned short  par_ind;        /* parent index */
        unsigned char   levnum;         /* 1: page table, 2: pd, 3: ... */
        unsigned char   pt_kind;        /* PT_xxx */

        unsigned short  classic_va_offs; /* l2: [0,0x300], l1: [0,0x200] */
};

typedef struct segment segment_t;
struct segment {
        mlevel_t        level;          /* page table or level-3 table */
        u64             segment_base_addr;
        segment_t       *bucket_next;   /* next segment in the hash bucket */
        u64             segid;          /* segment id (unique) */

        unsigned long   bp_validation;  /* set to tlb->bp_validation when
                                           breakpoints ranges are checked */
};

typedef struct {
        segment_t       *buckets[N_SEGMENT_BUCKETS];
        int             n_segments;     /* #entries in the hash */
} segment_hash_t;

#define SEGMENTS_PER_PAGE   ((0x1000 - sizeof(void *)) / sizeof(segment_t))

typedef struct tlb_alloc_page {
        segment_t segments[SEGMENTS_PER_PAGE];
        struct tlb_alloc_page *next;
} tlb_alloc_page_t;

typedef struct {
        segment_t        *free_list;
        tlb_alloc_page_t *pages;
        u64              n_allocked;
} tlb_alloc_t;

struct tlb {
        vm_t            *vm;            /* backlink */
        mlevel_t        *root;          /* == roots[tlb_mode] */

        unsigned        tlb_mode;       /* TLB_MODE_xxx */
        mlevel_t        *roots[4];      /* indexed by tlb_mode */
        unsigned        full_mode[4];   /* full mode (with qualifiers) */

        u64             va_segmask;     /* va bits used to qualify segments */

        segment_hash_t  segment_hash;

        tlb_alloc_t     tlb_alloc;      /* custom allocator for segment_t
                                           and mlevel_t */

        /* linear range breakpoint support */
        unsigned long   last_bp_id;
        lin_bp_t        *lin_bp;        /* first breakpoint */
        unsigned long   bp_validation;  /* increased when new bps are added */
};

static void
alloc_page_with_segments(tlb_alloc_t *m_alloc)
{
        tlb_alloc_page_t *p = (tlb_alloc_page_t *)x_alloc_page();
        wrap_BUG_ON(m_alloc->free_list);
        if (p) {
                unsigned i = 0;
                for (i = 0; i < SEGMENTS_PER_PAGE - 1; i++)
                        p->segments[i].bucket_next = &p->segments[i + 1];

                p->next = m_alloc->pages;
                m_alloc->pages = p;
                m_alloc->free_list = &p->segments[0];
        }
}

static segment_t *
zalloc_segment(tlb_t *tlb)
{
        tlb_alloc_t *m_alloc = &tlb->tlb_alloc;

        if (!m_alloc->free_list) {
                alloc_page_with_segments(m_alloc);
                if (!m_alloc->free_list)
                        return NULL;
        }
        segment_t *s = m_alloc->free_list;
        m_alloc->free_list = s->bucket_next;
        wrap_memset(s, 0, sizeof *s);
        m_alloc->n_allocked++;
        return s;
}

static void
free_segment(tlb_t *tlb, segment_t *s)
{
        tlb_alloc_t *m_alloc = &tlb->tlb_alloc;
        wrap_BUG_ON(m_alloc->n_allocked == 0);
        s->bucket_next = m_alloc->free_list;
        m_alloc->free_list = s;
        m_alloc->n_allocked--;

        if (m_alloc->n_allocked == 0) {
                while (m_alloc->pages) {
                        tlb_alloc_page_t *p = m_alloc->pages;
                        m_alloc->pages = p->next;
                        x_free_page((char *)p);
                }
                m_alloc->free_list = NULL;
        }
}

static inline void
host_tlb_modified(tlb_t *tlb)
{
        tlb->vm->entry_work |= EW_FLUSH_VPID;
}

static inline void
clear_pte_bits(tlb_t *tlb, u64 *pte, u64 mask)
{
        if (*pte & mask)
                host_tlb_modified(tlb);
        *pte &= ~mask;
}

static inline void
set_pte_bits(tlb_t *tlb, u64 *pte, u64 bits)
{
        if (~*pte & bits)
                host_tlb_modified(tlb);
        *pte |= bits;
}

static inline void
clear_pte(tlb_t *tlb, u64 *pte)
{
        if (*pte & PTE_P)
                host_tlb_modified(tlb);
        *pte = 2;
}

static inline void
clear_pde(tlb_t *tlb, u64 *pde)
{
        if (*pde & PDE_P)
                host_tlb_modified(tlb);
        *pde = 0;
}

static inline void
set_pde(tlb_t *tlb, u64 *pde, u64 val)
{
        if (*pde != val)
                host_tlb_modified(tlb);
        *pde = val;
}

static inline void
set_pte(tlb_t *tlb, u64 *pte, u64 val)
{
        if (*pte != val)
                host_tlb_modified(tlb);
        *pte = val;
}


/************************************************************************/
/*	simple segment hash implementation      			*/
/************************************************************************/

static inline int
segment_hash_function(u64 segid)
{
        /* the segid is basically a page address (with some extra bits) */
        return (segid >> 12) & (N_SEGMENT_BUCKETS - 1);
}

static segment_t *
segment_hash_get(tlb_t *tlb, u64 va, u64 segid)
{
        segment_hash_t *ht = &tlb->segment_hash;
        int ind = segment_hash_function(segid);
        segment_t **s = &ht->buckets[ind];

        u64 segment_baseaddr = va & tlb->va_segmask;

        while (*s && ((**s).segid != segid
                || (**s).segment_base_addr != segment_baseaddr)) {
                s = &(**s).bucket_next;
        }
        segment_t *ret = (*s) ? *s : NULL;
        if (*s)
                *s = (**s).bucket_next;

        ht->n_segments--;
        TLB_BUMP(PERFCTR_TLB_REUSE_SEGMENT);
        return ret;
}

static void
segment_hash_put(tlb_t *tlb, segment_t *s)
{
        wrap_BUG_ON(s->level.parent);
        int ind = segment_hash_function(s->segid);
        segment_hash_t *ht = &tlb->segment_hash;

        s->bucket_next = ht->buckets[ind];
        ht->buckets[ind] = s;
        ht->n_segments++;
}

/* returns a linked list with all segments and clears the table */
static segment_t *
segment_hash_unlink_all(tlb_t *tlb)
{
        segment_hash_t *ht = &tlb->segment_hash;
        segment_t *first = NULL;
        segment_t **pp = &first;
        int i;

        for (i = 0; i < N_SEGMENT_BUCKETS; i++) {
                *pp = ht->buckets[i];
                ht->buckets[i] = NULL;
                while (*pp) {
                        pp = &(**pp).bucket_next;
                        ht->n_segments--;
                }
        }
        return first;
}


/************************************************************************/
/*	level iteration               					*/
/************************************************************************/

typedef struct {
        mlevel_t        *cur;
        mlevel_t        *root;
} level_iter_t;

static level_iter_t
new_level_iter(mlevel_t *root)
{
        return (level_iter_t) { .root = root, .cur = NULL };
}

static mlevel_t *
iter_level(level_iter_t *iter)
{
        mlevel_t *cur = iter->cur;
        mlevel_t *root = iter->root;

        if (cur == NULL)
                cur = root;
        else if (cur->first_child)
                cur = cur->first_child;
        else {
                /* sibling of either the current node or some parent node */
                mlevel_t *p;
                for (p = cur; p != root && !p->sib_next; p = p->parent)
                        ;
                cur = (p == root) ? NULL : p->sib_next;
        }
        iter->cur = cur;
        return iter->cur;
}

typedef struct {
        mlevel_t        *root;
        u64             va;
        u64             end;
} va_iter_t;

typedef struct {
        mlevel_t *level;                /* level */
        unsigned first;
        unsigned last;
} va_iter_range_t;

static va_iter_t
new_va_iter(mlevel_t *root, u64 start, u64 end)
{
        u64 mask = (((1ULL + root->mask) << (root->levnum * 9 + 3)) - 0x1000);
        end &= mask;
        start &= mask;

        return (va_iter_t) { .root = root, .va = start, .end = end };
}
        
static bool
va_iter_next(va_iter_t *iter, va_iter_range_t *r)
{
        u64 va = iter->va;
        u64 end = iter->end;
        
        mlevel_t *lev = iter->root;
        while (va <= end) {
                unsigned shift = lev->levnum * 9 + 3;
                unsigned ind = (va >> shift) & lev->mask;

                /* drill down */
                if (lev->levnum != 1 && lev->children[ind] != NULL) {
                        lev = lev->children[ind];
                        continue;
                }

                u64 end_ind_64 = (end >> shift);
                unsigned end_ind = end_ind_64 < lev->mask ? 
                        end_ind_64 : lev->mask;
                
                /* let va point to the base of the current level */
                va &= ~(((lev->mask + 1ULL) << shift) - 1);

                /* leaf */
                if (lev->levnum == 1) {
                        iter->va = va + ((end_ind + 1ULL) << shift);
                        *r = (va_iter_range_t) {lev, ind, end_ind};
                        return true;
                }

                /* find index of first child */
                for (; ind <= end_ind && !lev->children[ind]; ind++)
                        ;
                va += (u64)ind << shift;

                /* no child found, start over at new va */
                if (ind == end_ind)
                        lev = iter->root;
        }
        *r = (va_iter_range_t) { 0 };
        return false;
}


/************************************************************************/
/*	ranged bp support             					*/
/************************************************************************/

static void
add_bp_protection(tlb_t *tlb, lin_bp_t *bp)
{
        if (!tlb->root)
                return;

        u64 last_page = bp->base + ((bp->n_pages - 1ULL) << 12);
        va_iter_t iter = new_va_iter(tlb->root, bp->base, last_page);
        va_iter_range_t r;

        while (va_iter_next(&iter, &r)) {
                unsigned i;
                for (i = r.first; i <= r.last; i++) {
                        u64 *pte = (u64*)r.level->ppage.p + i;

                        if (!*pte)
                                continue;
                        if (bp->prot & VMXMON_PROTECT_READ)
                                clear_pte_bits(tlb, pte, PTE_P);
                        if (bp->prot & VMXMON_PROTECT_WRITE)
                                clear_pte_bits(tlb, pte, PTE_W);
                        if (bp->prot & VMXMON_PROTECT_EXECUTE)
                                set_pte_bits(tlb, pte, PTE_NX);
                }
        }
}

static inline void
bp_protect_segment(tlb_t *tlb, segment_t *s)
{
        if (s->bp_validation == tlb->bp_validation)
                return;

        lin_bp_t *bp;
        for (bp = tlb->lin_bp; bp; bp = bp->next)
                add_bp_protection(tlb, bp);

        s->bp_validation = tlb->bp_validation;
}

static inline u64
restrict_pte_bp(tlb_t *tlb, uintptr_t va, u64 pte)
{
        lin_bp_t *bp;

        for (bp = tlb->lin_bp; bp; bp = bp->next) {
                if (((uintptr_t)(va - bp->base) >> 12) >= bp->n_pages)
                        continue;
                if (bp->prot & VMXMON_PROTECT_EXECUTE)
                        pte |= PTE_NX;
                if (bp->prot & VMXMON_PROTECT_WRITE)
                        pte &= ~PTE_W;
                if (bp->prot & VMXMON_PROTECT_READ) {
                        wrap_printk("read prohibited - "
                                    "should have been caught earlier\n");
                        pte = 0;
                }
        }
        return pte;
}

int
tlb_add_lin_bp(vm_t *vm, uintptr_t start,
               unsigned long n_pages, int prot)
{
        tlb_t *tlb = vm->tlb;
        lin_bp_t *bp = x_malloc(sizeof(*bp));

        if (!bp)
                return 0;
        bp->next = tlb->lin_bp;
        bp->prot = prot;
        bp->base = start;
        bp->id = ++tlb->last_bp_id;
        bp->n_pages = n_pages;

        tlb->lin_bp = bp;
        tlb->bp_validation++;

        add_bp_protection(tlb, bp);
        return bp->id;
}

void
tlb_remove_lin_bp(vm_t *vm, int id)
{
        tlb_t *tlb = vm->tlb;
        lin_bp_t **pp;

        for (pp = &tlb->lin_bp; *pp && (**pp).id != id; pp = &(**pp).next)
                ;
        if (*pp) {
                lin_bp_t *bp = *pp;
                *pp = bp->next;
                x_free(bp);
        }
}

static void
tlb_cleanup_lin_bp(tlb_t *tlb)
{
        lin_bp_t *bp = tlb->lin_bp;

        while (bp) {
                lin_bp_t *nbp = bp->next;
                x_free(bp);
                bp = nbp;
        }
        tlb->lin_bp = NULL;
}

int
tlb_lin_bp_check(vm_t *vm, u64 va, u8 ecode)
{
        tlb_t *tlb = vm->tlb;
        lin_bp_t *bp = tlb->lin_bp;
        vmxmon_t *x = vm->com;

        for (; bp; bp = bp->next) {
                int hit;

                if (((va - bp->base) >> 12) >= bp->n_pages)
                        continue;

                hit = (bp->prot & VMXMON_PROTECT_READ)
                        || ((bp->prot & VMXMON_PROTECT_WRITE)
                            && (ecode & ECODE_W))
                        || ((bp->prot & VMXMON_PROTECT_EXECUTE)
                            && (ecode & ECODE_I));

                if (!hit)
                        continue;

                x->vmret_qual = va;
                x->ecode = ecode;
                set_vmret(tlb->vm, VMRET_RANGE_BREAKPOINT);
                return 1;
        }
        return 0;
}

bool
tlb_has_lin_bps(vm_t *vm)
{
        return vm->tlb->lin_bp != NULL;
}


/************************************************************************/
/*	common segment and segment page handling        		*/
/************************************************************************/

#define LEV_INDEX(lev, va) \
        (((va) >> ((9 * (lev)->levnum) + 3)) & (lev)->mask)

/* faster variants */
#define LEV_INDEX_PML4(va)      (((va) >> (9 + 9 + 9 + 12)) & 0x1ff)
#define LEV_INDEX_PDPT_64(va)   (((va) >> (9 + 9 + 12)) & 0x1ff)
#define LEV_INDEX_PDPT_32(va)   (((va) >> (9 + 9 + 12)) & 0x3)
#define LEV_INDEX_PD(va)        (((va) >> (9 + 12)) & 0x1ff)
#define LEV_INDEX_PT(va)        (((va) >> (12)) & 0x1ff)


static inline unsigned
get_pt_kind(unsigned full_mmu_mode, unsigned levnum)
{
        unsigned primary_mode = PRIMARY_MMU_MODE(full_mmu_mode);
        bool is_five_level_requested = full_mmu_mode & MMU_MODE_QUAL_5_LEVEL;

        switch (primary_mode) {
        case MMU_MODE_NO_PAGING:
                return (levnum == 1) ? PT_TRIVIAL_PT    /* segment */
                        : (levnum == 2) ? PT_TRIVIAL_PD
                        : PT_LM_PDPT;

        case MMU_MODE_CLASSIC:
                return (levnum == 1) ? PT_CLASSIC_PT    /* segment */
                        : (levnum == 2) ? PT_CLASSIC_PD
                        : PT_LM_PDPT;

        case MMU_MODE_PAE:
                return (levnum == 1) ? PT_PAE_PT        /* segment */
                        : (levnum == 2) ? PT_PAE_PD
                        : PT_LM_PDPT;

        case MMU_MODE_LONG:
                if (is_five_level_requested) {
                        return PT_5_LEVEL_PT + 1 - levnum;
                } else {
                        return (levnum == 1) ? PT_4_LEVEL_PT
                        : (levnum == 2) ? PT_4_LEVEL_PD
                        : (levnum == 3) ? PT_4_LEVEL_PDPT  /* segment */
                        : PT_4_LEVEL_PML4;
                }
        default:
                wrap_BUG_ON(1);
                return 0;
        }
}

static inline unsigned
get_classic_va_offs(unsigned mmu_mode, unsigned levnum, u64 va)
{
        return (mmu_mode != MMU_MODE_CLASSIC) ? 0
                : (levnum == 1) ? ((va >> 12) & 0x200)
                : (levnum == 2) ? ((va >> 22) & 0x300)
                : 0;
}

/* update level dependency */
static int
update_level_dep(mlevel_t *lev, vmx_page_t *xp)
{
        vm_t *vm = lev->tlb->vm;
        char *xp_p = xp ? xp->upage.p : NULL;
        if (lev->dep_page == xp_p)
                return 0;
        if (lev->dep)
                vmxpage_drop_usage(vm->pspace, lev->dep);

        if (xp) {
                lev->dep = vmxpage_add_usage(vm, xp, lev, -1 /* n/a */,
                                             PFLAG_PTABLE);
                lev->dep_page = lev->dep ? xp->upage.p : NULL;
                if (!lev->dep) {
                        set_vmret(vm, VMRET_OUT_OF_MEMORY);
                        return 1;
                }
        } else {
                lev->dep = NULL;
                lev->dep_page = NULL;

                /* Should not get here as TLB is always flushed when switching
                   from paged to non-paged mode. */
                wrap_BUG_ON(1);
        }

        return 0;
}

static int
clean_up_oom_at_mlevel_creation(vm_t *vm, char *p1, char *p2, mlevel_t *lev) {
        if (p1)
                x_free_page(p1);
        if (p2)
                x_free_page(p2);
        free_mpage(&lev->ppage);
        set_vmret(vm, VMRET_OUT_OF_MEMORY);
        return 1;
}

static int
allocate_mlevel_members(mlevel_t *lev, vm_t *vm)
{
        bool long_mode = IS_MMU_MODE_LONG(vm->mmu_mode);
        wrap_memset(lev, 0, sizeof *lev);
        char *p1 = x_alloc_page();
        char *p2 = x_alloc_page();
        if (!p1 || !p2)
                return clean_up_oom_at_mlevel_creation(vm, p1, p2, lev);
        bool success = long_mode? alloc_mpage(&lev->ppage)
                                : alloc_mpage32(&lev->ppage);
        if (!success) {
                return clean_up_oom_at_mlevel_creation(vm, p1, p2, lev);
        }
        lev->children = (void *)p1;
        lev->page_copy = p2;
        return 0;
}

static int
init_intermediate_mlevel(tlb_t *tlb, mlevel_t *lev, mlevel_t *par, u64 va,
                  vmx_page_t *xp)
{
        /* Parent level must be present. For root levels without parents,
           init_root_mlevel() must be called instead to construct them */
        wrap_BUG_ON(!par);

        vm_t *vm = tlb->vm;
        if (allocate_mlevel_members(lev, vm)) {
                return 1;
        }

        lev->tlb = tlb;

        /* calculate levnum */
        unsigned char levnum = par->levnum - 1;
        lev->levnum = levnum;
        lev->mask = 0x1ff;

        unsigned full_mmu_mode = vm->mmu_mode;
        lev->pt_kind = get_pt_kind(full_mmu_mode, levnum);
        unsigned primary_mode = PRIMARY_MMU_MODE(full_mmu_mode);
        lev->classic_va_offs = get_classic_va_offs(primary_mode, levnum, va);

        if (update_level_dep(lev, xp) != 0) {
                return clean_up_oom_at_mlevel_creation(vm,
                        (char *)lev->children, lev->page_copy, lev);
        }

        /* link new level back to parent level*/
        lev->parent = par;
        lev->sib_next = par->first_child;
        par->first_child = lev;
        unsigned par_ind = LEV_INDEX(par, va);
        lev->par_ind = par_ind;
        par->children[par_ind] = lev;

        /* PDE entry */
        u64 pde = mpage_phys(&lev->ppage) | PDE_P;
        if (par->pt_kind != PT_LM_PDPT)
                pde |= PDE_A | PDE_U | PDE_W;   /* reserved for PAE PDPTE */
        u64 *_pde = (u64*)par->ppage.p;
        set_pde(tlb, &_pde[par_ind], pde);

        /* copy relevant mapping entry */
        if (par->dep) {
                char *p_page = par->dep_page;
                if (IS_MMU_MODE_CLASSIC(vm->mmu_mode)) {
                        u32 *guest = (u32 *)p_page + par->classic_va_offs;
                        u32 *copy = (u32 *)par->page_copy;
                        unsigned ind = par_ind;
                        if (par->levnum == 2)
                                ind = ind >> 1;  /* 2 -> 1 mapping */
                        copy[par_ind] = guest[ind];
                } else {
                        u64 *guest = (u64 *)p_page;
                        u64 *copy = (u64 *)par->page_copy;
                        copy[par_ind] = guest[par_ind];
                }
        }
        return 0;
}

static int
init_root_mlevel(tlb_t *tlb, mlevel_t *lev)
{
        vm_t *vm = tlb->vm;
        bool long_mode = IS_MMU_MODE_LONG(vm->mmu_mode);
        bool is_5_level = vm->mmu_mode & MMU_MODE_QUAL_5_LEVEL;
        bool is_pae_top_level = !long_mode;

        if (allocate_mlevel_members(lev, vm)) {
                return 1;
        }

        lev->tlb = tlb;

        /* Target PAE, classic and no paging use 3-level PAE shadow pages.
         * Target 4-level paging uses 4-level shadow pages.
         * Target 5-level paging uses 5-level shadow pages.
         * For 3-level PAE shadow pages, a unique mask is used. Otherwise it is
         * a regular 9 bit-wide one. */

        unsigned char top_shadow_level = 0;
        unsigned short top_level_mask = 0;
        if (is_pae_top_level) {
                top_shadow_level = 3;
                top_level_mask = 0x3;
        } else if (!is_5_level) {
                top_shadow_level = 4;
                top_level_mask = 0x1ff;
        } else {
                top_shadow_level = 5;
                top_level_mask = 0x1ff;
        }

        lev->levnum = top_shadow_level;
        lev->mask = top_level_mask;

        unsigned full_mmu_mode = vm->mmu_mode;
        lev->pt_kind = get_pt_kind(full_mmu_mode, top_shadow_level);
        lev->classic_va_offs = 0; /* Not used for top mlevel */
        return 0;
}

/* assumptions:
        i) removed from parent's linked list
       ii) any PDE mapping us has been cleared
      iii) segments are not in the segment hash
*/
static void
free_level(tlb_t *tlb, mlevel_t *p)
{
        vm_t *vm = tlb->vm;
        if (p->levnum == 1) {
                /* page table: release vmxpage references */
                for (int i = 0; i < 512; i++) {
                        page_usage_t *m = p->children[i];
                        if (m)
                                vmxpage_drop_usage(vm->pspace, m);
                }
        } else {
                /* release lower levels */
                while (p->first_child) {
                        mlevel_t *child = p->first_child;
                        p->first_child = child->sib_next;
                        free_level(tlb, child);
                }
        }
        x_free_page((char*)p->children);
        x_free_page(p->page_copy);

        /* drop MMU page */
        free_mpage(&p->ppage);

        /* drop vmxpage dependency */
        if (p->dep)
                vmxpage_drop_usage(vm->pspace, p->dep);

        free_segment(tlb, (segment_t *)p);
}

static mlevel_t *
clean_up_oom_at_new_level(tlb_t *tlb, segment_t *s)
{
        if (s) {
                free_segment(tlb, s);
        }
        set_vmret(tlb->vm, VMRET_OUT_OF_MEMORY);
        return NULL;
}

static mlevel_t *
new_level(tlb_t *tlb, mlevel_t *par, u64 va, vmx_page_t *xp_dep)
{
        segment_t *s = zalloc_segment(tlb);

        if (!s || init_intermediate_mlevel(tlb, &s->level, par, va, xp_dep)) {
                return clean_up_oom_at_new_level(tlb, s);
        }
        return &s->level;
}

static mlevel_t *
new_root_tlb_level(tlb_t *tlb)
{
        segment_t *s = zalloc_segment(tlb);

        if (!s || init_root_mlevel(tlb, &s->level)) {
                return clean_up_oom_at_new_level(tlb, s);
        }
        return &s->level;
}


static segment_t *
new_segment(tlb_t *tlb, mlevel_t *par, u64 va, u64 segid, vmx_page_t *xp_dep)
{
        segment_t *s = zalloc_segment(tlb);

        if (!s || init_intermediate_mlevel(tlb, &s->level, par, va, xp_dep))
                goto oom;

        s->segment_base_addr = va & tlb->va_segmask;
        s->segid = segid;
        s->bp_validation = tlb->bp_validation;
        s->bucket_next = NULL;

        TLB_BUMP(PERFCTR_TLB_NEW_SEGMENT);
        return s;
 oom:
        if (s)
                free_segment(tlb, s);
        set_vmret(tlb->vm, VMRET_OUT_OF_MEMORY);
        return NULL;
}


/************************************************************************/
/*	segment id                    					*/
/************************************************************************/

static inline u64
no_mmu_segid(u32 va)
{
        /* top 11 (2 + 9) bits of the address is used as segid;
           the value is shifted down to suit the hash function */
        return (((u32)va & 0xffe00000) >> 9) | 1;
}

static inline u64
classic_segid(u32 pde)
{
        u32 mask = 0xfffff0a7;
        u32 mask_ps = (0xfffff0a7 | PDE_G | PDE_D) & ~PDE_PAT;
        return pde & ((pde & PDE_PS) ? mask_ps : mask);
}

static inline u64
pae_segid(u64 pde)
{
        u64 mask = 0x800ffffffffff0a7ULL;
        u64 mask_ps = (0x800ffffffffff0a7ULL | PDE_G | PDE_D) & ~PDE_PAT;
        return pde & ((pde & PDE_PS) ? mask_ps : mask);
}

static inline u64
long_segid(u64 pde)
{
        return pde & 0x800ffffffffff0a7ULL;
}

static inline bool
_pte_ok_64(tlb_t *tlb, u64 guest_pte, u64 *copy, u64 *_pte)
{
        if (guest_pte == *copy)
                return true;

        u64 diff = guest_pte ^ *copy;
        if (diff & ~(PTE_D | PTE_W | PTE_AVL_MASK_64))
                return false;
        if ((guest_pte & (PTE_W | PTE_D)) != (PTE_W | PTE_D))
                clear_pte_bits(tlb, _pte, PTE_W);

        *copy = guest_pte;
        return true;
}

static inline int
_pte_ok_classic(tlb_t *tlb, u32 guest_pte, u32 *copy, u64 *_pte)
{
        if (guest_pte == *copy)
                return true;

        u32 diff = guest_pte ^ *copy;
        if (diff & ~(PTE_D | PTE_W | PTE_AVL_MASK_CLASSIC))
                return false;
        if ((guest_pte & (PTE_W | PTE_D)) != (PTE_W | PTE_D))
                clear_pte_bits(tlb, _pte, PTE_W);

        *copy = guest_pte;
        return true;
}

static inline int
_only_avl_differs_64(u64 guest_pte, u64 *copy)
{
        /* XXX: PTE_AVL_MASK_64 is not general enough */
        if (guest_pte == *copy)
                return true;
        if ((guest_pte ^ *copy) & ~PTE_AVL_MASK_64)
                return false;
        *copy = guest_pte;
        return true;
}


/************************************************************************/
/*	validation                    					*/
/************************************************************************/

static void
unlink_from_par(tlb_t *tlb, mlevel_t *lev)
{
        mlevel_t *par = lev->parent;
        unsigned ind = lev->par_ind;
        u64 *_pde = (u64*)par->ppage.p;

        wrap_BUG_ON(par->children[ind] != lev);

        /* remove PDE entry */
        clear_pde(tlb, &_pde[ind]);

        /* remove direct pointer */
        par->children[ind] = NULL;

        /* unlink from child chain */
        mlevel_t **p = &par->first_child;
        for (; *p && *p != lev; p = &(**p).sib_next)
                ;
        wrap_BUG_ON(!*p);
        *p = lev->sib_next;
        lev->parent = NULL;
}

static inline void
_va_4_level_pml4(tlb_t *tlb, mlevel_t *lev, unsigned i, unsigned end)
{
        if (!lev->dep)
                return;

        u64 *guest = (u64 *)lev->dep_page;
        for (; i <= end; i++) {
                segment_t *seg = lev->children[i];
                if (!seg || long_segid(guest[i]) == seg->segid)
                        continue;
                unlink_from_par(tlb, &seg->level);
                segment_hash_put(tlb, seg);
        }
}

static inline void
_va_intermediate_long(tlb_t *tlb, mlevel_t *lev, unsigned i, unsigned end)
{
        if (!lev->dep)
                return;

        u64 *guest = (u64 *)lev->dep_page;
        u64 *copy = (u64 *)lev->page_copy;
        for (; i <= end; i++) {
                mlevel_t *clev = lev->children[i];
                if (!clev || _only_avl_differs_64(guest[i], &copy[i]))
                        continue;

                unlink_from_par(tlb, clev);
                free_level(tlb, clev);
        }
}

static inline void
_va_pt_64(tlb_t *tlb, mlevel_t *lev, unsigned i, unsigned end)
{
        if (!lev->dep)
                return;

        u64 *guest = (u64 *)lev->dep_page;
        u64 *copy = (u64 *)lev->page_copy;
        u64 *_pte = (u64 *)lev->ppage.p;

        vm_t *vm = tlb->vm;
        for (; i <= end; i++) {
                page_usage_t *m = lev->children[i];
                if (!m || _pte_ok_64(tlb, guest[i], &copy[i], &_pte[i]))
                        continue;

                lev->children[i] = NULL;
                clear_pte(tlb, &_pte[i]);
                vmxpage_drop_usage(vm->pspace, m);
        }
}

static inline void
_va_pae_pd(tlb_t *tlb, mlevel_t *lev, unsigned i, unsigned end)
{
        if (!lev->dep)
                return;

        u64 *guest = (u64 *)lev->dep_page;
        for (; i <= end; i++) {
                segment_t *seg = lev->children[i];
                if (!seg || pae_segid(guest[i]) == seg->segid)
                        continue;

                unlink_from_par(tlb, &seg->level);
                segment_hash_put(tlb, seg);
        }
}

static inline void
_va_classic_pd(tlb_t *tlb, mlevel_t *lev, unsigned i, unsigned end)
{
        if (!lev->dep)
                return;

        u32 *guest = (u32 *)lev->dep_page + lev->classic_va_offs;
        for (; i <= end; i++) {
                /* 256 classic entries corresponds to 512 PAE entries */
                unsigned j = i / 2;

                segment_t *seg = lev->children[i];
                if (!seg || classic_segid(guest[j]) == seg->segid)
                        continue;
                unlink_from_par(tlb, &seg->level);
                segment_hash_put(tlb, seg);
        }
}


static inline void
_va_classic_pt(tlb_t *tlb, mlevel_t *lev, unsigned i, unsigned end)
{
        if (!lev->dep)
                return;

        u32 *guest = (u32 *)lev->dep_page + lev->classic_va_offs;
        u32 *copy = (u32 *)lev->page_copy;
        u64 *_pte = (u64 *)lev->ppage.p;

        for (; i <= end; i++) {
                page_usage_t *m = lev->children[i];
                if (!m || _pte_ok_classic(tlb, guest[i], &copy[i], &_pte[i]))
                        continue;

                lev->children[i] = NULL;
                clear_pte(tlb, &_pte[i]);
                vmxpage_drop_usage(tlb->vm->pspace, m);
        }
}

static void
va_4_level_pml4(tlb_t *tlb, mlevel_t *lev, u64 va)
{
        unsigned ind = LEV_INDEX_PML4(va);
        _va_4_level_pml4(tlb, lev, ind, ind);
}

static inline void
va_intermediate_long(tlb_t *tlb, mlevel_t *lev, u64 va)
{
        unsigned ind = LEV_INDEX(lev, va);
        _va_intermediate_long(tlb, lev, ind, ind);
}

static void
va_pt_64(tlb_t *tlb, mlevel_t *lev, u64 va)
{
        unsigned ind = LEV_INDEX_PT(va);
        _va_pt_64(tlb, lev, ind, ind);
}

static void
va_pae_pd(tlb_t *tlb, mlevel_t *lev, u64 va)
{
        unsigned ind = LEV_INDEX_PD(va);
        _va_pae_pd(tlb, lev, ind, ind);
}

static void
va_classic_pd(tlb_t *tlb, mlevel_t *lev, u64 va)
{
        unsigned ind = LEV_INDEX_PD(va) & ~1;
        _va_classic_pd(tlb, lev, ind, ind + 1);
}

static void
va_classic_pt(tlb_t *tlb, mlevel_t *lev, u64 va)
{
        unsigned ind = LEV_INDEX_PT(va);
        _va_classic_pt(tlb, lev, ind, ind);
}

static void
va_nop(tlb_t *tlb, mlevel_t *lev, u64 va)
{
}

typedef void (*validate_va_t)(tlb_t *tlb, mlevel_t *lev, u64 va);

static const validate_va_t va_funcs[] = {
        [PT_4_LEVEL_PML4]  = &va_4_level_pml4,
        [PT_4_LEVEL_PDPT]  = &va_intermediate_long,
        [PT_4_LEVEL_PD]    = &va_intermediate_long,
        [PT_4_LEVEL_PT]    = &va_pt_64,

        [PT_PAE_PD]     = &va_pae_pd,
        [PT_PAE_PT]     = &va_pt_64,

        [PT_CLASSIC_PD] = &va_classic_pd,
        [PT_CLASSIC_PT] = &va_classic_pt,

        [PT_TRIVIAL_PD] = &va_nop,
        [PT_TRIVIAL_PT] = &va_nop,

        [PT_LM_PDPT]    = &va_nop,

        [PT_5_LEVEL_PML5] = va_intermediate_long,
        [PT_5_LEVEL_PML4] = va_intermediate_long,
        [PT_5_LEVEL_PDPT] = va_intermediate_long,
        [PT_5_LEVEL_PD] = va_intermediate_long,
        [PT_5_LEVEL_PT] = va_pt_64,
};

static void
validate_va(tlb_t *tlb, u64 va)
{
        mlevel_t *lev = tlb->root;
        while (lev) {
                va_funcs[lev->pt_kind](tlb, lev, va);
                if (lev->levnum == 1)
                        break;
                unsigned ind = LEV_INDEX(lev, va);
                lev = lev->children[ind];
        }
}

typedef void (*validate_one_t)(tlb_t *tlb, mlevel_t *lev);

static void
vl_4_level_pml4(tlb_t *tlb, mlevel_t *lev)
{
        _va_4_level_pml4(tlb, lev, 0, 511);
}

static void
vl_intermediate_long(tlb_t *tlb, mlevel_t *lev)
{
        _va_intermediate_long(tlb, lev, 0, 511);
}

static void
vl_pt_64(tlb_t *tlb, mlevel_t *lev)
{
        _va_pt_64(tlb, lev, 0, 511);
}

static void
vl_pae_pd(tlb_t *tlb, mlevel_t *lev)
{
        _va_pae_pd(tlb, lev, 0, 511);
}

static void
vl_classic_pd(tlb_t *tlb, mlevel_t *lev)
{
        _va_classic_pd(tlb, lev, 0, 511);
}

static void
vl_classic_pt(tlb_t *tlb, mlevel_t *lev)
{
        _va_classic_pt(tlb, lev, 0, 511);
}

static void
vl_nop(tlb_t *tlb, mlevel_t *lev)
{
}

static const validate_one_t vfuncs[] = {
        [PT_4_LEVEL_PML4]  = &vl_4_level_pml4,
        [PT_4_LEVEL_PDPT]  = &vl_intermediate_long,
        [PT_4_LEVEL_PD]    = &vl_intermediate_long,
        [PT_4_LEVEL_PT]    = &vl_pt_64,

        [PT_PAE_PD]     = &vl_pae_pd,
        [PT_PAE_PT]     = &vl_pt_64,

        [PT_CLASSIC_PD] = &vl_classic_pd,
        [PT_CLASSIC_PT] = &vl_classic_pt,

        [PT_TRIVIAL_PD] = &vl_nop,
        [PT_TRIVIAL_PT] = &vl_nop,

        [PT_LM_PDPT]    = &vl_nop,

        [PT_5_LEVEL_PML5] = vl_intermediate_long,
        [PT_5_LEVEL_PML4] = vl_intermediate_long,
        [PT_5_LEVEL_PDPT] = vl_intermediate_long,
        [PT_5_LEVEL_PD] = vl_intermediate_long,
        [PT_5_LEVEL_PT] = vl_pt_64,
};

static void
validate_level(tlb_t *tlb, mlevel_t *root)
{
        if (!root)
                return;

        level_iter_t iter = new_level_iter(root);
        mlevel_t *lev;
        while ((lev = iter_level(&iter)))
                vfuncs[lev->pt_kind](tlb, lev);
}

static void
clear_pd_level(tlb_t *tlb, mlevel_t *lev) 
{
        u64 *_pde = (u64 *)lev->ppage.p;

        segment_t *next = NULL;
        for (segment_t *s = (segment_t *)lev->first_child; s; s = next) {
                next = (segment_t *)s->level.sib_next;

                unsigned ind = s->level.par_ind;
                lev->children[ind] = NULL;
                s->level.parent = NULL;
                segment_hash_put(tlb, s);
                clear_pde(tlb, &_pde[ind]);
        }
        lev->first_child = NULL;

        for (unsigned i = 0; i < 512; i++) {
                segment_t *s = lev->children[i];
                wrap_BUG_ON(s);
        }
        if (lev->dep)
                vmxpage_drop_usage(tlb->vm->pspace, lev->dep);
        lev->dep = NULL;
        lev->dep_page = NULL;
}


/* set proper root dependencies (or make sure all segments
   are flushed if the vmxpage dependency is unavailable) */
static void
set_root_deps(tlb_t *tlb)
{
        mlevel_t *lev = tlb->root;
        vm_t *vm = tlb->vm;
        vmxmon_t *x = vm->com;
        rwx_t rwx = RWX_R;

        if (IS_MMU_MODE_NO_PAGING(vm->mmu_mode))
                return;

        if (lev->pt_kind == PT_LM_PDPT)
                lev = lev->first_child;
        for (; lev; lev = lev->sib_next) {
                u64 ent = IS_MMU_MODE_PAE(vm->mmu_mode) ?
                        vm->guest_pdpte[lev->par_ind] : (x->cr3 | PDE_P);

                if (!(ent & PDE_P))
                        goto reset;

                tphys_t tmp_pa = ent;
                if (gpa_to_pa(vm, &tmp_pa, rwx))
                        goto reset;
                vmx_page_t *xp;
                if (vmxpage_lookup(vm, tmp_pa, &xp, rwx)) {
                        goto reset;
                }
                if (update_level_dep(lev, xp) != 0) {
                        goto reset;
                }
                continue;
        reset:
                clear_pd_level(tlb, lev);
                clear_vmret(vm);
        }
}


/************************************************************************/
/*	tlb map         						*/
/************************************************************************/

static void
reuse_segment(tlb_t *tlb, mlevel_t *par, unsigned par_ind, segment_t *s)
{
        s->level.parent = par;
        s->level.par_ind = par_ind;
        s->level.sib_next = par->first_child;

        par->first_child = &s->level;
        par->children[par_ind] = s;

        u64 *_pde = (u64 *)par->ppage.p;
        u64 pde = mpage_phys(&s->level.ppage) | PDE_A | PDE_P | PDE_U | PDE_W;
        set_pde(tlb, &_pde[par_ind], pde);

        validate_level(tlb, &s->level);
        bp_protect_segment(tlb, s);
}

static inline u64
calc_segid32(vm_t *vm, vmx_page_t *p2, u32 va)
{
        if (IS_MMU_MODE_CLASSIC(vm->mmu_mode)) {
                u32 *p = (u32 *)p2->upage.p + ((va >> (10 + 12)) & 0x3ff);
                return classic_segid(*p);
        }
        if (IS_MMU_MODE_PAE(vm->mmu_mode)) {
                u64 *p = (u64 *)p2->upage.p + LEV_INDEX_PD(va);
                return pae_segid(*p);
        }
        return no_mmu_segid(va);
}

static inline u64
calc_segid64(vm_t *vm, vmx_page_t *p4, u64 va)
{
        unsigned ind = LEV_INDEX_PML4(va);
        u64 *pml4 = (u64 *)p4->upage.p;
        return long_segid(pml4[ind]);
}

static inline segment_t *
instantiate_segment(tlb_t *tlb, u64 va, mlevel_t *par, u64 segid,
                    vmx_page_t *dep)
{
        unsigned par_ind = LEV_INDEX(par, va);

        /* make sure the current segment has a correct segid */
        segment_t *seg = par->children[par_ind];
        if (seg) {
                if (seg->segid == segid)
                        return seg;
                unlink_from_par(tlb, &seg->level);
                segment_hash_put(tlb, seg);
        }

        seg = segment_hash_get(tlb, va, segid);
        if (seg) {
                reuse_segment(tlb, par, par_ind, seg);
                return seg;
        }
        return new_segment(tlb, par, va, segid, dep);
}

mlevel_t *
tlb_map32(vm_t *vm, u32 va, vmx_page_t *p2, vmx_page_t *p1)
{
        tlb_t *tlb = vm->tlb;
        mlevel_t *l3 = tlb->root;

        unsigned ind = LEV_INDEX_PDPT_32(va);
        mlevel_t *l2 = l3->children[ind];

        /* add segment table (PDPTE entry) */
        if (!l2 && !(l2 = new_level(tlb, l3, va, p2)))
                return NULL;

        /* add missing root dependency */
        if (update_level_dep(l2, p2))
                return NULL;
        
        u64 segid = calc_segid32(vm, p2, va);
        segment_t *seg = instantiate_segment(tlb, va, l2, segid, p1);
        return seg ? &seg->level : NULL;
}

static mlevel_t *
instantiate_sublevel(tlb_t *tlb, mlevel_t *par, u64 va, vmx_page_t *xp_dep)
{
        unsigned par_ind = LEV_INDEX(par, va);
        mlevel_t *lev = par->children[par_ind];

        /* make sure the level dependency is correct */
        if (lev) {
                char *xp_p = xp_dep ? xp_dep->upage.p : NULL;
                if (lev->dep_page == xp_p)
                        return lev;
                unlink_from_par(tlb, lev);
                free_level(tlb, lev);
        }
        return new_level(tlb, par, va, xp_dep);
}

mlevel_t *
tlb_map_4_levels(vm_t *vm, u64 va, vmx_page_t *p[4])
{
        tlb_t *tlb = vm->tlb;
        vmx_page_t *p4 = p[0];
        vmx_page_t *p3 = p[1];
        vmx_page_t *p2 = p[2];
        vmx_page_t *p1 = p[3];

        /* add missing root dependency */
        if (update_level_dep(tlb->root, p4))
                return NULL;

        u64 segid = calc_segid64(vm, p4, va);
        segment_t *seg = instantiate_segment(tlb, va, tlb->root, segid, p3);
        if (!seg)
                return NULL;
        mlevel_t *l2 = instantiate_sublevel(tlb, &seg->level, va, p2);
        mlevel_t *l1 = l2 ? instantiate_sublevel(tlb, l2, va, p1) : NULL;
        return l1;
}

mlevel_t *
tlb_map_5_levels(vm_t *vm, u64 va, vmx_page_t *p[5])
{
        tlb_t *tlb = vm->tlb;
        vmx_page_t *p5 = p[0];
        vmx_page_t *p4 = p[1];
        vmx_page_t *p3 = p[2];
        vmx_page_t *p2 = p[3];
        vmx_page_t *p1 = p[4];

        if (update_level_dep(tlb->root, p5))
                return NULL;
        mlevel_t *l4 = instantiate_sublevel(tlb, tlb->root, va, p4);
        if (!l4) {
                return NULL;
        }

        mlevel_t *l3 = instantiate_sublevel(tlb, l4, va, p3);
        if (!l3) {
                return NULL;
        }

        mlevel_t *l2 = instantiate_sublevel(tlb, l3, va, p2);
        if (!l2) {
                return NULL;
        }

        mlevel_t *l1 = instantiate_sublevel(tlb, l2, va, p1);
        return l1;
}


void
_tlb_unmap_page(mlevel_t *p, unsigned qual)
{
        u64 *_pte = (u64*)p->ppage.p;
        wrap_BUG_ON(p->levnum != 1);

        clear_pte(p->tlb, &_pte[qual]);
        p->children[qual] = NULL;
}

void
_tlb_write_protect_page(mlevel_t *p, unsigned qual)
{
        u64 *_pte = (u64*)p->ppage.p;
        wrap_BUG_ON(p->levnum != 1);

        clear_pte_bits(p->tlb, &_pte[qual], PTE_W);
}

void
_tlb_make_page_noexec(mlevel_t *p, unsigned qual)
{
        u64 *_pte = (u64*)p->ppage.p;
        wrap_BUG_ON(p->levnum != 1);

        set_pte_bits(p->tlb, &_pte[qual], PTE_NX);
}



/************************************************************************/
/*	common          						*/
/************************************************************************/

static inline int
restrict_pte_ept(vm_t *vm, u64 *pte, rwx_t ept_perm)
{
        vmxmon_t *x = vm->com;
        if (!(x->vmx_non_root_mode && x->vmx_non_root_guest_ept))
                return 0;

        if (!(ept_perm & RWX_R)) {
                /* Shadow paging that relies on traditional paging
                 * does not allow unreadable (i.e. execute only) pages.
                 * It is potentially possible to use protection keys or
                 * SMAP (for supervisor mode only) to model unreadable pages
                 * with shadow paging. VMP currently does not support that. */
                set_vmret(vm, VMRET_EMULATE);
                return 1;
        }

        if (!(ept_perm & RWX_W))
                *pte &= ~PTE_W;
        if (!(ept_perm & RWX_X))
                *pte |= PTE_NX;
        return 0;
}


/* restrict pte bits according to vmxpage privileges; if we aren't
   allowed to access the vmxpage with the requested privileges,
   return VMRET_PROTECTION */
static inline int
restrict_pte_vmxpage(vm_t *vm, vmx_page_t *xp, u64 pa, u64 *pte_ptr, u8 ecode)
{
        u64 pte = *pte_ptr;

        bool viol = !(xp->rwx & RWX_R);
        if (!(xp->rwx & RWX_X)) {
                viol |= ecode & ECODE_I;
                pte |= PTE_NX;
        }
        if (!(xp->rwx & RWX_W)) {
                viol |= ecode & ECODE_W;
                pte &= ~PTE_W;
        }

        if (viol) {
                vmxmon_t *x = vm->com;
                x->ecode = ecode;
                x->vmret_qual = pa;
                set_vmret(vm, VMRET_PROTECTION);
                return 1;
        }
        *pte_ptr = pte;
        return 0;
}

static inline int
add_guest_page_usage(vm_t *vm, vmx_page_t *xp, u64 pte, mlevel_t *lev, int ind)
{
        /* remove any old page reference */
        page_usage_t *m = lev->children[ind];
        if (m)
                vmxpage_drop_usage(vm->pspace, m);

        /* and create a new one */
        unsigned pflag = (pte & PTE_W)
                ? (PFLAG_RAM | PFLAG_WRITABLE) : PFLAG_RAM;
        lev->children[ind] = vmxpage_add_usage(vm, xp, lev, ind, pflag);
        if (!lev->children[ind]) {
                set_vmret(vm, VMRET_OUT_OF_MEMORY);
                return 1;
        }
        return 0;
}

/* insert the host pte; the physical address is taken from the
   xp and protection bits from the pte */
static inline void
add_host_pte(vm_t *vm, vmx_page_t *xp, u64 pte, mlevel_t *lev, unsigned ind)
{
        u64 *_pte = (u64*)lev->ppage.p;
        u64 host_pte = upage_phys(&xp->upage)
                | PTE_P | PTE_A | PTE_D
                | (pte & (/*PTE_G |*/ PTE_W | PTE_U | PTE_NX));

        /* tlb insertion statistics */
        if (!(_pte[ind] & PTE_P)) {
                if (_pte[ind] == 0)
                        BUMP(PERFCTR_TLB_PTE_MISSING);
                else
                        BUMP(PERFCTR_TLB_PTE_MISSING_CLEARED);
        } else if ((_pte[ind] ^ host_pte) & PTE_W)
                BUMP(PERFCTR_TLB_PTE_WRITE_ENABLE);
        else {
                /* most likely segment instantiation (PTE already present) */
                BUMP(PERFCTR_TLB_PTE_OTHER);
        }

        set_pte(vm->tlb, &_pte[ind], host_pte);
}

static inline void
save_pte_copy(tlb_t *tlb, mlevel_t *lev, unsigned ind, u64 pte)
{
        if (!lev->dep)
                return;
        if (IS_MMU_MODE_CLASSIC(tlb->vm->mmu_mode)) {
                u32 *guest = (u32 *)lev->dep_page;
                u32 *copy = (u32 *)lev->page_copy;
                copy[ind] = guest[ind + lev->classic_va_offs];
        } else {
                u64 *guest = (u64 *)lev->dep_page;
                u64 *copy = (u64 *)lev->page_copy;
                copy[ind] = guest[ind];
        }
}

static inline rwx_t
ecode_to_rwx(vm_t *vm, u8 ecode, u64 pte)
{
        if (ecode & ECODE_I) {
                if (!vm->use_mbe)
                        return RWX_X;

                return (pte & PTE_U) ? RWX_XU : RWX_XS;
        }

        return RWX_R | ((ecode & ECODE_W) ? RWX_W : 0);
}

/* realize a guest PTE. The ecode parameter is used to determine when we
   can restrict the PTE (e.g. clear PTE_W for a page which isn't dirty). */
int
tlb_realize_pte(vm_t *vm, u64 va, mlevel_t *lev, u64 pte, u8 ecode)
{
        u64 physmask = 0x000ffffffffff000ULL;
        tphys_t pa = pte & physmask;
        tlb_t *tlb = vm->tlb;
        u64 org_pte = pte;
        rwx_t rwx = ecode_to_rwx(vm, ecode, pte);

        rwx_t ept_perm;
        if (gpa_to_pa_perm(vm, &pa, rwx, &ept_perm))
                return 1;
        if (restrict_pte_ept(vm, &pte, ept_perm))
                return 1;

        vmx_page_t *xp = NULL;
        if (vmxpage_lookup_nomap(vm, pa, &xp, rwx))
                return 1;

        /* if CR0.WP == 0, then the combination (!PTE.W && ECODE.W && !ECODE.U)
           can occur here. We resolve the situation by dropping any
           user access privileges and mapping the page r/w. */
        if (!(pte & PTE_W) && (ecode & ECODE_W) && !(ecode & ECODE_U)) {
                pte &= ~PTE_U;
                pte |= PTE_W;
        }

        /* make PTE bits compatible with vmxpage privileges */
        if (restrict_pte_vmxpage(vm, xp, pa, &pte, ecode))
                return 1;

        /* we want to intercept writes if the pte isn't dirty. */
        if (!(pte & PTE_D))
                pte &= ~PTE_W;

        /* restrict pte according to range breakpoints */
        pte = restrict_pte_bp(tlb, va, pte);

        unsigned ind = (va >> 12) & 0x1ff;
        /* guest page used by lev[ind] */
        if (add_guest_page_usage(vm, xp, pte, lev, ind))
                return 1;
        save_pte_copy(tlb, lev, ind, org_pte);

        add_host_pte(vm, xp, pte, lev, ind);
        return 0;
}

static void
clear_segment_hash(tlb_t *tlb)
{
        segment_t *s = segment_hash_unlink_all(tlb);
        while (s) {
                segment_t *seg = s;
                s = s->bucket_next;
                free_level(tlb, &seg->level);
        }
}

static void
do_free_all(tlb_t *tlb)
{
        unsigned i;
        for (i = 0; i < 4; i++) {
                if (tlb->roots[i])
                        free_level(tlb, tlb->roots[i]);
                tlb->roots[i] = NULL;
        }
        tlb->root = NULL;

        clear_segment_hash(tlb);
        tlb->vm->entry_work |= EW_CR3_DIRTY;
}


/* this function is also invoked for qualifier mmu mode switches */
void
tlb_mmu_mode_change(vm_t *vm)
{
        tlb_t *tlb = vm->tlb;

        /* at the moment segments are not qualified by mode */
        clear_segment_hash(tlb);

        unsigned mode =
                IS_MMU_MODE_NO_PAGING(vm->mmu_mode) ? TLB_MODE_NP :
                IS_MMU_MODE_CLASSIC(vm->mmu_mode) ? TLB_MODE_CLASSIC :
                IS_MMU_MODE_PAE(vm->mmu_mode) ? TLB_MODE_PAE :
                TLB_MODE_LONG;

        tlb->tlb_mode = mode;
        tlb->root = tlb->roots[mode];

        /* submode change? */
        if (mode != tlb->full_mode[mode]) {
                if (tlb->root) {
                        free_level(tlb, tlb->root);
                        tlb->roots[mode] = NULL;
                        tlb->root = NULL;
                }
                tlb->full_mode[mode] = vm->mmu_mode;
        }
        tlb->va_segmask = IS_MMU_MODE_LONG(vm->mmu_mode) ?
                (0x1ffULL << (9 * 3 + 12)) : (0x7ffULL << (9 + 12));
}

int
tlb_activate_cr3(vm_t *vm)
{
        tlb_t *tlb = vm->tlb;

        TLB_BUMP(PERFCTR_TLB_CR3_CHANGE);

        if (tlb->root) {
                set_root_deps(tlb);
                validate_level(tlb, tlb->root);
        } else {
                if (!(tlb->root = new_root_tlb_level(tlb)))
                        return 1;
                tlb->roots[tlb->tlb_mode] = tlb->root;
        }
        vm->shadow_page_table_pa = mpage_phys(&tlb->root->ppage);
        return 0;
}

void
tlb_flush(vm_t *vm, bool keep_global)
{
        tlb_t *tlb = vm->tlb;

        TLB_BUMP(PERFCTR_TLB_VERIFY_SEGPAGE_SEGS);
        validate_level(tlb, tlb->root);
}

void
tlb_invlpg(vm_t *vm, u64 addr)
{
        tlb_t *tlb = vm->tlb;

        validate_va(tlb, addr);
}

void
tlb_clear(vm_t *vm)
{
        tlb_t *tlb = vm->tlb;
        if (!tlb)
                return;

        do_free_all(tlb);
        BUMP(PERFCTR_TLB_FLUSH_ALL);
}

void
tlb_memory_limit_prune(vm_t *vm)
{
        if (vm->tlb->segment_hash.n_segments > SEGMENT_LIMIT)
                tlb_clear(vm);
}

int
tlb_init(vm_t *vm)
{
        tlb_t *tlb = x_malloc(sizeof *tlb);
        if (!tlb)
                return 1;

        wrap_memset(tlb, 0, sizeof *tlb);
        vm->tlb = tlb;
        tlb->vm = vm;
        return 0;
}

void
tlb_cleanup(vm_t *vm)
{
        tlb_t *tlb = vm->tlb;
        if (!tlb)
                return;

        do_free_all(tlb);
        tlb_cleanup_lin_bp(tlb);
        x_free(tlb);
        vm->tlb = NULL;
}
