/*
 * Vmxmon - export the Intel(R) Virtualization Technology (Intel(R) VT) for
 * IA-32, Intel(R) 64 and Intel(R) Architecture (Intel(R) VT-x) to user-space.
 * Copyright 2020-2022 Intel Corporation.
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

#include "nested-ept.h"
#include "tphys.h"
#include "kernel-api.h"
#include "vmxmon.h"
#include "vm.h"
#include "mm.h"
#include "ept.h"

#define NUM_NESTED_EPTS         4

typedef struct {
        tphys_t pa;             /* tphys address of this EPT page */
        rwx_t ept_perm;         /* combined EPT permissions */
        unsigned level;         /* level (4,3,2,1) */
        bool large_page;        /* this page is part of a large page */

        mpage_t ept;            /* the EPT page presented to HW */

        /* elev_t pointers (guest_page_t pointers for level 1) */
        void **children;        /* links to next level */
} elev_t;

struct guest_ept {
        vm_t *vm;
        u64 guest_eptp;
        u64 eptp;               /* physical address of ept_root page */
        elev_t ept_root;

        bool invept_needed;     /* invept needed (EW_FLUSH_GUEST_EPT) */
};

typedef struct nested_epts {
        unsigned next_reuse;
        guest_ept_t *ept[NUM_NESTED_EPTS];
} nested_epts_t;

struct guest_page {
        /* these fields are constant */
        guest_ept_t *guest_ept;
        u64 gpa;                        /* guest physical address */

        /* these fields are dynamic */
        rwx_t ept_perm;                 /* permissions found in guest EPT */
        u64 pa;                         /* same as vmx_page->tphys */
        struct guest_page *next;
        struct vmx_page *vmx_page;      /* can be NULL */
};


#define LEV1_IND(pa)    (((pa) >> 12) & 0x1ff)
#define LEV2_IND(pa)    (((pa) >> 21) & 0x1ff)
#define LEV3_IND(pa)    (((pa) >> 30) & 0x1ff)
#define LEV4_IND(pa)    (((pa) >> 39) & 0x1ff)
#define LEV5_IND(pa)    (((pa) >> 48) & 0x1ff)

/* EPTE values used for invalidation.
        recursive:      the entire subtree must be verified
        single:         only a single entry needs to be verified
   Low order bits must be 0 to mark the entry as not-present
*/
#define EPTE_INVALID_SINGLE      0x08
#define EPTE_INVALID_RECURSIVE   0x18


static inline u64
get_epte(elev_t *lev, unsigned index)
{
        return ((u64 *)lev->ept.p)[index];
}

static inline void
set_epte(elev_t *lev, unsigned index, u64 val)
{
        ((u64 *)lev->ept.p)[index] = val;
}

static inline void
set_epte_child(elev_t *lev, unsigned index, elev_t *child)
{
        u64 epte = mpage_phys(&child->ept) | RWX_ALL;
        set_epte(lev, index, epte);
}

static void
set_epte_page(elev_t *lev, unsigned index, guest_page_t *gp)
{
        vmx_page_t *xp = gp->vmx_page;
        rwx_t rwx_page = gp->ept_perm & xp->rwx;
        u64 epte = (6 << 3) | BIT(6) | upage_phys(&xp->upage) | rwx_page;
        set_epte(lev, index, epte);
}

static inline void
clear_epte(elev_t *lev, unsigned index)
{
        set_epte(lev, index, 0);
}

static void
set_epte_invalid_recursive(elev_t *lev, unsigned index)
{
        set_epte(lev, index, EPTE_INVALID_RECURSIVE);
}

static void
make_epte_invalid_single(guest_ept_t *g, elev_t *lev, unsigned index)
{
        u64 *p = &((u64 *)lev->ept.p)[index];
        /* do not overwrite RECURSIVE with SINGLE */
        *p = (*p & EPTE_INVALID_RECURSIVE) | EPTE_INVALID_SINGLE;

        g->invept_needed = true;
}

/* assumption: level1 is known to exist, usually because a guest page exist. */
static elev_t *
lev1_lookup(elev_t *root, tphys_t pa)
{
        unsigned l4 = LEV4_IND(pa);
        unsigned l3 = LEV3_IND(pa);
        unsigned l2 = LEV2_IND(pa);

        elev_t *lev3 = root->children[l4];
        elev_t *lev2 = lev3->children[l3];
        elev_t *lev1 = lev2->children[l2];
        return lev1;
}

static guest_page_t *
alloc_guest_page(guest_ept_t *g, u64 gpa)
{
        guest_page_t *gp = x_malloc(sizeof *gp);
        if (!gp) {
                set_vmret(g->vm, VMRET_OUT_OF_MEMORY);
                return NULL;
        }
        wrap_memset(gp, 0, sizeof *gp);
        /* this fields will never change */
        gp->guest_ept = g;
        gp->gpa = gpa;
        return gp;
}

static void
unlink_guest_page(guest_page_t *gp)
{
        vmx_page_t *xp = gp->vmx_page;
        if (xp) {
                guest_page_t **pp = &xp->guest_page;
                for (; *pp != gp; pp = &(*pp)->next)
                        ;
                *pp = gp->next;
        }
        gp->vmx_page = NULL;
}

static void
free_guest_page(guest_page_t *gp)
{
        guest_ept_t *g = gp->guest_ept;

        g->invept_needed = true;

        elev_t *lev1 = lev1_lookup(&g->ept_root, gp->gpa);
        unsigned l1 = LEV1_IND(gp->gpa);
        clear_epte(lev1, l1);
        lev1->children[l1] = NULL;

        unlink_guest_page(gp);
        x_free(gp);
}

static elev_t *
init_level(elev_t *lev, int level)
{
        wrap_memset(lev, 0, sizeof *lev);
        lev->children = (void *)x_alloc_page();
        alloc_mpage(&lev->ept);

        if (!lev->children || !lev->ept.p) {
                free_mpage(&lev->ept);
                if (lev->children)
                        x_free_page((char *)lev->children);
                return NULL;
        }
        lev->level = level;
        return lev;
}

static elev_t *
alloc_level(int level)
{
        elev_t *lev = x_malloc(sizeof *lev);
        if (lev && init_level(lev, level))
                return lev;
        if (lev)
                x_free(lev);
        return NULL;
}

/* Free level and contents. The root level pointer is not release since
   it is co-allocated. */
static void
free_level(elev_t *lev)
{
        if (lev->level != 1) {
                unsigned i;
                for (i = 0; i < 512; i++) {
                        if (lev->children[i])
                                free_level(lev->children[i]);
                }
        } else {
                unsigned i;
                for (i = 0; i < 512; i++) {
                        if (lev->children[i])
                                free_guest_page(lev->children[i]);
                }
        }
        x_free_page((char *)lev->children);
        free_mpage(&lev->ept);

        /* Level 4 is co-allocated; do not release. */
        if (lev->level != 4)
                x_free(lev);
}

/* Mark entire guest EPT as invalid (revalidation needed for every entry) */
static void
invalidate_level(guest_ept_t *g, elev_t *lev)
{
        g->invept_needed = true;
        unsigned i;
        for (i = 0; i < 512; i++)
                set_epte_invalid_recursive(lev, i);
}

static void
invalidate_gpa(guest_ept_t *g, elev_t *lev, u64 gpa, rwx_t rwx)
{
        const unsigned ind = (gpa >> (12 + 9 * (lev->level - 1))) & 0x1ff;

        if (lev->level != 1) {
                elev_t *child = lev->children[ind];
                if (child) {
                        if (rwx & ~child->ept_perm)
                                make_epte_invalid_single(g, lev, ind);
                        invalidate_gpa(g, child, gpa, rwx);
                }
        } else {
                make_epte_invalid_single(g, lev, ind);
        }
}

static inline elev_t *
establish_level(guest_ept_t *g, u64 gpa, int level, elev_t *lev, rwx_t rwx)
{
        const unsigned ind = (gpa >> (12 + 9 * (level - 1))) & 0x1ff;

        /* next level cached? */
        elev_t *child = lev->children[ind];
        if (child) {
                rwx_t perm = epte_perm(g->vm, get_epte(lev, ind));
                if (perm && !(rwx & ~perm)) {
                        /* valid entry */
                        return child;
                }
        }

        u64 ent;
        if (lev->large_page) {
                ent = lev->pa | ((u64)ind << (12  + 9 * (level - 1))) | RWX_ALL;
        } else {
                if (get_ept_entry(g->vm, level, lev->pa, ind, rwx, &ent))
                        return NULL;
        }
        rwx_t ept_perm = lev->ept_perm & epte_perm(g->vm, ent);

        /* insufficient permissions? */
        if (rwx & ~ept_perm) {
                set_vmret(g->vm, VMRET_GUEST_EPT_VIOLATION);
                /* flush PTE entry from cache */
                if (epte_perm(g->vm, get_epte(lev, ind)))
                        make_epte_invalid_single(g, lev, ind);
                return NULL;
        }

        u64 pa = ent & make_amask(g->vm);
        if (!child) {
                child = alloc_level(level - 1);
                if (!child) {
                        set_vmret(g->vm, VMRET_OUT_OF_MEMORY);
                        return NULL;
                }
                lev->children[ind] = child;
        } else {
                u64 epte = get_epte(lev, ind);
                if (epte == EPTE_INVALID_RECURSIVE) {
                        /* recursive invalidation */
                        invalidate_level(g, child);
                } else if (child->pa != pa || child->ept_perm & ~ept_perm) {
                        /* bad pa, or restricting permissions. */
                        invalidate_level(g, child);
                }
        }
        child->ept_perm = ept_perm;
        child->pa = pa;
        child->large_page = lev->large_page || (ent & 0x80);
        set_epte_child(lev, ind, child);
        return child;
}

static guest_page_t *
establish_gpage(guest_ept_t *g, u64 gpa, elev_t *lev, rwx_t rwx)
{
        const unsigned ind = (gpa >> 12) & 0x1ff;

        /* page cached? */
        guest_page_t *gp = lev->children[ind];
        if (gp) {
                rwx_t perm = epte_perm(g->vm, get_epte(lev, ind));
                if (!(rwx & ~perm)) {
                        /* this can happen if we are revalidating, or
                           a more restricted permission was used previously. */

                        /* NOTE: establish_page will ensure that the
                           vmxpage has not changed. */
                        return gp;
                }
        }

        u64 ent;
        if (lev->large_page) {
                ent = lev->pa | ((u64)ind << 12) | RWX_ALL;
        } else {
                if (get_ept_entry(g->vm, 1, lev->pa, ind, rwx, &ent))
                        return NULL;
        }

        rwx_t ept_perm = lev->ept_perm & epte_perm(g->vm, ent);

        /* insufficient permissions? */
        if (rwx & ~ept_perm) {
                set_vmret(g->vm, VMRET_GUEST_EPT_VIOLATION);
                if (epte_perm(g->vm, get_epte(lev, ind)))
                        make_epte_invalid_single(g, lev, ind);
                return NULL;
        }

        /* make sure a guest_page struct is allocated */
        if (!gp) {
                gp = alloc_guest_page(g, gpa);
                if (!gp)
                        return NULL;
                lev->children[ind] = gp;
        } else {
                /* no need for recursive invalidation */
        }
        /* update pa and permissions */
        gp->ept_perm = ept_perm;
        gp->pa = ent & make_amask(g->vm);

        /* note: it could be that we have linked an incorrect vmxpage,
           or a vmxpage with incorrect permissions. This is addressed
           from establish_page. */
        return gp;
}

static int
establish_page(guest_ept_t *g, guest_page_t *gp, elev_t *lev1, rwx_t rwx)
{
        const unsigned ind = (gp->gpa >> 12) & 0x1ff;

        /* verify that we have the correct vmx page linked */
        vmx_page_t *xp = gp->vmx_page;
        if (xp && xp->tphys != gp->pa) {
                /* bad page, unlink */
                unlink_guest_page(gp);
                set_epte(lev1, ind, 0);
                g->invept_needed = true;
                xp = NULL;
        }

        /* establish the vmx_page */
        if (!xp) {
                if (vmxpage_lookup_nomap(g->vm, gp->pa, &xp, rwx))
                        return 1;
                /* add to xp->guest_page linked list */
                gp->next = xp->guest_page;
                xp->guest_page = gp;
                gp->vmx_page = xp;
        } else {
                if (rwx & ~xp->rwx) {
                        if (vmxpage_lookup_nomap(g->vm, gp->pa, &xp, rwx))
                                return 1;
                }
                rwx_t old_rwx = epte_perm(g->vm, get_epte(lev1, ind));
                rwx_t new_rwx = xp->rwx & gp->ept_perm;
                /* this should normally not occur, but just in case */
                if (old_rwx & ~new_rwx)
                        g->invept_needed = true;
        }
        set_epte_page(lev1, ind, gp);
        return 0;
}

int
handle_nested_ept_fault(vm_t *vm, u64 gpa, rwx_t rwx)
{
        gpa &= ~0xfff;

        BUMP(PERFCTR_GUEST_EPT_LOOKUP);

        guest_ept_t *g = vm->cur_guest_ept;

        const tphys_t amask = make_amask(vm);
        const int num_levels = ((g->guest_eptp & 0x38) >> 3) + 1;

        elev_t *lev4 = &g->ept_root;
        if (num_levels == 5) {
                unsigned ind = LEV5_IND(gpa);
                u64 ent;
                if (get_ept_entry(vm, 5, g->eptp & amask, ind, rwx, &ent))
                        goto out;
                if (lev4->pa != (ent & amask)) {
                        invalidate_level(g, &g->ept_root);
                        lev4->pa = ent & amask;
                        lev4->ept_perm = epte_perm(vm, ent);
                }
        }
        elev_t *lev3 = establish_level(g, gpa, 4, lev4, rwx);
        if (!lev3)
                goto out;
        elev_t *lev2 = establish_level(g, gpa, 3, lev3, rwx);
        if (!lev2)
                goto out;
        elev_t *lev1 = establish_level(g, gpa, 2, lev2, rwx);
        if (!lev1)
                goto out;
        guest_page_t *gp = establish_gpage(g, gpa, lev1, rwx);
        if (!gp)
                goto out;
        if (establish_page(g, gp, lev1, rwx)) {
                BUMP(PERFCTR_GUEST_EPT_NOPAGE);
                return 1;
        }
        return 0;
out:
        BUMP(PERFCTR_GUEST_EPT_MISS);
        return 1;
}


static void
for_all_guest_pages(elev_t *lev, void (*callback)(guest_page_t *))
{
        if (lev->level == 1) {
                unsigned i;
                for (i = 0; i < 512; i++) {
                        if (lev->children[i])
                                callback(lev->children[i]);
                }
        } else {
                unsigned i;
                for (i = 0; i < 512; i++) {
                        if (lev->children[i])
                                for_all_guest_pages(lev->children[i], callback);
                }
        }
}

u64
guest_ept_get_eptp(vm_t *vm)
{
        return vm->cur_guest_ept->eptp;
}

/* permission for vmx_page has changed... update guest page usage. */
void
update_vmxpage_guest_perm(vmx_page_t *xp)
{
        rwx_t rwx = xp->rwx;
        guest_page_t *gp;
        for (gp = xp->guest_page; gp; gp = gp->next) {
                rwx_t page_rwx = gp->ept_perm & rwx;
                guest_ept_t *g = gp->guest_ept;

                elev_t *lev1 = lev1_lookup(&g->ept_root, gp->gpa);
                unsigned l1 = LEV1_IND(gp->gpa);

                rwx_t old_perm = epte_perm(g->vm, get_epte(lev1, l1));

                /* more restrictive permissions? */
                if (old_perm & ~page_rwx)
                        g->invept_needed = true;

                /* Note: only update permissions if the page is mapped
                   - it could be in invalidated state in which case
                   an EPT fault is needed to revalidate it. */
                if (old_perm != 0) {
                        set_epte_page(lev1, l1, gp);
                }
        }
}

/* vmxpage is removed, removed associated guest pages */
void
free_vmxpage_guest_pages(vmx_page_t *xp)
{
        /* iterate until all guest pages have been released */
        while (xp->guest_page) {
                guest_page_t *gp = xp->guest_page;
                free_guest_page(gp);
                /* gp is released at this point */
        }
}

/* drop all guest pages (done before all vmx_pages are released) */
void
drop_all_guest_pages(vm_t *vm)
{
        nested_epts_t *epts = vm->nested_epts;
        if (epts) {
                unsigned i;
                for (i = 0; i < NUM_NESTED_EPTS; i++) {
                        guest_ept_t *g = epts->ept[i];
                        for_all_guest_pages(&g->ept_root, free_guest_page);
                        g->invept_needed = true;
                }
        }
}

bool
need_guest_ept_flush(vm_t *vm)
{
        guest_ept_t *g = vm->cur_guest_ept;
        bool ret = g->invept_needed;
        g->invept_needed = false;
        return ret;
}

/* Ensure that all guest EPTPs are marked for invalidation. This is used
   when the vm is migrated to a new CPU. */
void
mark_all_guest_eptps_for_flush(vm_t *vm)
{
        nested_epts_t *epts = vm->nested_epts;
        unsigned i;
        for (i = 0; i < NUM_NESTED_EPTS; i++) {
                guest_ept_t *g = epts->ept[i];
                g->invept_needed = true;
        }
}

static bool
eptp_match(u64 eptp1, u64 eptp2)
{
        return ((eptp1 ^ eptp2) & ~0xfff) == 0;
}

void
handle_invept_flush(vm_t *vm, u64 eptp)
{
        nested_epts_t *epts = vm->nested_epts;

        unsigned i;
        for (i = 0; i < NUM_NESTED_EPTS; i++) {
                guest_ept_t *g = epts->ept[i];
                if (eptp == -1 || eptp_match(eptp, g->guest_eptp))
                        invalidate_level(g, &g->ept_root);
        }
}

void
handle_ept_viol_flush(vm_t *vm, u64 gpa, u32 access, u64 eptp)
{
        nested_epts_t *epts = vm->nested_epts;

        unsigned i;
        for (i = 0; i < NUM_NESTED_EPTS; i++) {
                guest_ept_t *g = epts->ept[i];
                if (eptp_match(eptp, g->guest_eptp)) {
                        invalidate_gpa(g, &g->ept_root, gpa, access);
                        break;
                }
        }
}

void
change_guest_ept(vm_t *vm)
{
        nested_epts_t *epts = vm->nested_epts;
        unsigned i = 0;
        for (; i < NUM_NESTED_EPTS; i++) {
                if (epts->ept[i]->guest_eptp == vm->guest_eptp)
                        break;
        }
        if (i == NUM_NESTED_EPTS) {
                i = epts->next_reuse++;
                if (epts->next_reuse >= NUM_NESTED_EPTS)
                        epts->next_reuse = 0;

                guest_ept_t *g = epts->ept[i];
                elev_t *root = &g->ept_root;
                invalidate_level(g, root);
                g->guest_eptp = vm->guest_eptp;
                root->pa = g->guest_eptp & make_amask(vm);
                root->ept_perm = RWX_ALL;
                //wrap_printk("USING EPT %llx\n", vm->guest_eptp);
        }
        vm->cur_guest_ept = epts->ept[i];
}

static guest_ept_t *
new_guest_ept(vm_t *vm)
{
        guest_ept_t *g = x_malloc(sizeof *g);
        if (!g)
                return NULL;
        wrap_memset(g, 0, sizeof *g);

        /* init root paging level */
        elev_t *root = init_level(&g->ept_root, 4);
        if (!root) {
                x_free(g);
                return NULL;
        }
        g->vm = vm;
        g->eptp = mpage_phys(&root->ept);
        return g;
}

static void
free_guest_ept(guest_ept_t *g)
{
        if (g) {
                elev_t *root = &g->ept_root;
                wrap_BUG_ON(root->level != 4);
                free_level(root);
                x_free(g);
        }
}

int
nested_epts_init(vm_t *vm)
{
        nested_epts_t *epts = x_malloc(sizeof *epts);
        if (!epts)
                return 1;
        vm->nested_epts = epts;
        wrap_memset(epts, 0, sizeof *epts);

        unsigned i;
        for (i = 0; i < NUM_NESTED_EPTS; i++) {
                epts->ept[i] = new_guest_ept(vm);
                if (!epts->ept[i]) {
                        nested_epts_cleanup(vm);
                        return 1;
                }
        }
        vm->cur_guest_ept = epts->ept[0];
        return 0;
}

void
nested_epts_cleanup(vm_t *vm)
{
        nested_epts_t *epts = vm->nested_epts;
        if (epts) {
                unsigned i;
                for (i = 0; i < NUM_NESTED_EPTS; i++)
                        free_guest_ept(epts->ept[i]);
                x_free(epts);
                vm->nested_epts = NULL;
        }
}
