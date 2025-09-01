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
#include "asm-inlines.h"
#include "simcall.h"
#include "tlb.h"
#include "ept.h"

static inline u64
combine_perm(u64 pte_a, u64 pte_b)
{
        return ((pte_a & pte_b) & (PTE_U | PTE_W)) | ((pte_a | pte_b) & PTE_NX);
}

static bool
pku_update_perm(vm_t *vm, u64 pte, u64 *perm)
{
        vmxmon_t *x = vm->com;
        bool pku_active = (x->cr4 & CR4_OSPKE) && (x->pkru != 0);

        bool is_user = *perm & PTE_U;
        
        unsigned pkey = FIELD(pte, 62, 59);
        bool pkey_ad = (x->pkru >> ((pkey * 2) + 0)) & 1;
        bool pkey_wd = (x->pkru >> ((pkey * 2) + 1)) & 1;

        if (pku_active && is_user && pkey_ad)
                return true;

        if (pku_active && is_user && pkey_wd)
                *perm &= ~PTE_W;

        return false;
}

static inline u64
update_accessed_dirty(u64 pte, u8 ecode)
{
        return pte | ((ecode & ECODE_W) ? PTE_A | PTE_D : PTE_A);
}

static inline u64
update_pte_bits(u64 pte, u64 perm, u8 ecode)
{
        pte = update_accessed_dirty(pte, ecode);
        return (pte & ~(PTE_U | PTE_W | PTE_NX)) | perm;
}

static void
raise_pf(vm_t *vm, u64 va, u8 ecode)
{
        vmxmon_t *x = vm->com;
        ecode &= ECODE_U | ECODE_I | ECODE_W | ECODE_P | ECODE_RSV;

        if (!(x->exc_bitmap & BIT(14)) && !x->vmx_non_root_mode) {
		/* The I/D-bit is reserved if NXE is disabled. */
		if (!(x->msr_efer & EFER_NXE))
			ecode &= ~ECODE_I;

                int vmret = vmx_inject_pf(vm, va, ecode);
                set_vmret(vm, vmret);
        } else {
                x->vmret_qual = va;
                x->ecode = ecode;
                set_vmret(vm, VMRET_PAGE_FAULT);
        }
}

static void
raise_rsvd(vm_t *vm, u64 va, u8 ecode)
{
        raise_pf(vm, va,
                 (ecode & (ECODE_I | ECODE_U | ECODE_W)) | ECODE_RSV | ECODE_P);
}

static inline bool
handle_missing_rsvd(vm_t *vm, u64 va, u64 pte, u8 ecode, u64 rsv) 
{
        if ((pte & PTE_P) && (pte & rsv) == 0)
                return false;

        ecode &= ECODE_I | ECODE_U | ECODE_W;
        if ((pte & PTE_P))
                ecode |= ECODE_P | ECODE_RSV;
        raise_pf(vm, va, ecode);
        return true;
}

static inline bool
handle_perm_violation(vm_t *vm, u64 va, u64 perm, u8 ecode)
{
        bool is_user = ecode & ECODE_U;
        bool is_exec = ecode & ECODE_I;
        bool is_write = ecode & ECODE_W;

        vmxmon_t *x = vm->com;
        bool smep_enabled = x->cr4 & CR4_SMEP;
        bool smap_enabled = (x->cr4 & CR4_SMAP) && !(x->rflags & RFLAGS_AC);
        bool check_wp = MMU_SUBMODE_WP(vm->mmu_mode) || is_user;

        if ((is_write && check_wp && !(perm & PTE_W))
            || (is_user && !(perm & PTE_U))
            || (is_exec && (perm & PTE_NX))
            || (!is_user && (perm & PTE_U)
                && ((smep_enabled && is_exec) || (smap_enabled && !is_exec)))) {
                /* raise PF */
                ecode &= ECODE_I | ECODE_U | ECODE_W; 
                raise_pf(vm, va, ecode | ECODE_P);
                return true;
        }
        return false;
}

#define ADDR_1GB_MASK           ((1 << 30) - 1)
#define ADDR_2MB_MASK           ((1 << 21) - 1)
#define ADDR_4KB_MASK           ((1 << 12) - 1)

int
gpa_to_pa_perm(vm_t *vm, tphys_t *in_out_pa, rwx_t rwx, rwx_t *ept_perm)
{
        vmxmon_t *x = vm->com;

        if (!(x->vmx_non_root_mode && x->vmx_non_root_guest_ept)) {
                *ept_perm = RWX_ALL;
                return 0;
        }

        const tphys_t amask = make_amask(vm);
        tphys_t gpa = *in_out_pa;

        rwx_t perm = RWX_ALL;
        u64 pml4e_base;

        int levels = ((x->vmx_eptp & 0x38) >> 3) + 1;
        if (levels == 5) {
                /* PML5E */
                unsigned pml5e_ind = ((gpa >> 48) & 0x1ff);
                u64 pml5e_e;
                if (get_ept_entry(vm, 5, (x->vmx_eptp & ~0xfff), pml5e_ind,
                                  rwx, &pml5e_e))
                        return 1;
                perm &= epte_perm(vm, pml5e_e);
                pml4e_base = pml5e_e & amask;
        } else {
                pml4e_base = x->vmx_eptp & ~0xfff;
        }

        /* PML4E */
        unsigned pml4e_ind = ((gpa >> 39) & 0x1ff);
        u64 pml4e_e;
        if (get_ept_entry(vm, 4, pml4e_base, pml4e_ind, rwx, &pml4e_e))
                return 1;
        perm &= epte_perm(vm, pml4e_e);

        /* PDPTE */
        unsigned pdpte_ind = ((gpa >> 30) & 0x1ff);
        u64 pdpte_e;
        if (get_ept_entry(vm, 3, (pml4e_e & amask), pdpte_ind, rwx, &pdpte_e))
                return 1;
        perm &= epte_perm(vm, pdpte_e);

        tphys_t page_pa;
        tphys_t page_mask;

        if (pdpte_e & (1 << 7)) {
                page_pa = pdpte_e & amask;
                page_mask = ADDR_1GB_MASK;
        } else {
                /* PDE */
                unsigned pde_ind = ((gpa >> 21) & 0x1ff);
                u64 pde_e;
                if (get_ept_entry(vm, 2, (pdpte_e & amask), pde_ind,
                                  rwx, &pde_e))
                        return 1;
                perm &= epte_perm(vm, pde_e);

                if (pde_e & (1 << 7)) {
                        page_pa = pde_e & amask;
                        page_mask = ADDR_2MB_MASK;
                } else {
                        /* PTE */
                        unsigned pte_ind = ((gpa >> 12) & 0x1ff);
                        u64 pte_e;
                        if (get_ept_entry(vm, 1, (pde_e & amask), pte_ind,
                                          rwx, &pte_e))
                                return 1;
                        perm &= epte_perm(vm, pte_e);

                        page_pa = pte_e & amask;
                        page_mask = ADDR_4KB_MASK;
                }
        }

        if (~perm & rwx)
                return guest_ept_violation(vm);

        *in_out_pa = page_pa | (gpa & page_mask);
        *ept_perm = perm;
        return 0;        
}

int
gpa_to_pa(vm_t *vm, tphys_t *in_out_pa, rwx_t rwx)
{
        rwx_t ept_perm;
        return gpa_to_pa_perm(vm, in_out_pa, rwx, &ept_perm);
}

/* For simplicity we call both gpa_to_pa and vmxpage_lookup with 'r/w' access
   (RWX_GUEST_PAGE_STRUCT) even though we could go with just 'r' access if:
   a. no any updates are needed to accessed and dirty bits
      for paging structures pointed to by CR3;
   and
   b. accessed and dirty flags for guest EPT aren't in use (see bit 6 of EPTP:
      setting this bit causes processor accesses to guest paging structure
      entries to be treated as writes; i.e. if EPTP bit 6 is set gpa_to_pa
      SHOULD be called with 'r/w' access). */
#define RWX_GUEST_PAGE_STRUCT (RWX_R | RWX_W)


/************************************************************************/
/*	PAE paging (legacy 32-bit)					*/
/************************************************************************/

static int
pae_page_fault(vm_t *vm, u8 ecode, u64 va)
{
        const u64 physmask = BITMASK(51, 12);
        vmxmon_t *x = vm->com;
        u64 rsv = BITMASK(63, vm->maxphyaddr);

        /* check for virtual breakpoints */
        if (tlb_lin_bp_check(vm, va, ecode))
                return 1;

        /* --- LEVEL 3 (PDPTE) --- */
        u64 pdpte = vm->guest_pdpte[(va >> 30) & 3];
        u64 pdpte_rsv = rsv | BITMASK(8, 5) | BITMASK(2, 1);
        if (handle_missing_rsvd(vm, va, pdpte, ecode, pdpte_rsv))
                return 1;

        /* bit64 is not reserved if EFER.NXE is set */
        if ((x->msr_efer & EFER_NXE))
                rsv &= ~BIT(63);

        /* --- LEVEL 2 (PDE) --- */
        vmx_page_t *p2;
        tphys_t tmp_pa = pdpte & physmask;
        if (gpa_to_pa(vm, &tmp_pa, RWX_GUEST_PAGE_STRUCT))
                return 1;
        if (vmxpage_lookup(vm, tmp_pa, &p2, RWX_GUEST_PAGE_STRUCT))
                return 1;
        u64 *pde_ptr = (u64*)p2->upage.p + ((va >> 21) & 0x1ff);
        u64 pde = *pde_ptr;
        u64 pde_rsv = (pde & PDE_PS) ? rsv | BITMASK(20, 13) : rsv;
        if (handle_missing_rsvd(vm, va, pde, ecode, pde_rsv))
                return 1;
        u64 perm = combine_perm(pde, PTE_U | PTE_W);

        /* --- LEVEL 1 (PTE) --- */
        vmx_page_t *p1;
        u64 pte, *pte_ptr;
        if (pde & PDE_PS) {
                /* collapsed, 2MB page mapping from PDE */
                p1 = NULL;
                pte_ptr = NULL;
                pte = (pde & ~0x1ff000) | (va & 0x1ff000)
                        | ((pde >> (12 - 7)) & PTE_PAT);

                if ((pte & PTE_U))
                        BUMP(PERFCTR_TLB_LARGE_USER_PAGE);
                else
                        BUMP(PERFCTR_TLB_LARGE_SU_PAGE);
        } else {
                tmp_pa = pde & physmask;
                if (gpa_to_pa(vm, &tmp_pa, RWX_GUEST_PAGE_STRUCT))
                        return 1;
                if (vmxpage_lookup(vm, tmp_pa, &p1, RWX_GUEST_PAGE_STRUCT))
                        return 1;
                pte_ptr = (u64*)p1->upage.p + ((va >> 12) & 0x1ff);
                pte = *pte_ptr;
                if (handle_missing_rsvd(vm, va, pte, ecode, rsv))
                        return 1;
                perm = combine_perm(pte, perm);
                
                if ((pte & PTE_U))
                        BUMP(PERFCTR_TLB_USER_PAGE);
                else
                        BUMP(PERFCTR_TLB_SU_PAGE);
        }

        /* check permissions */
        if (handle_perm_violation(vm, va, perm, ecode))
                return 1;

        /* update accessed bits */
        if (pde & PDE_PS) {
                *pde_ptr = update_accessed_dirty(pde, ecode);
        } else {
                *pde_ptr |= PDE_A;
                *pte_ptr = update_accessed_dirty(pte, ecode);
        }
        pte = update_pte_bits(pte, perm, ecode);
        mlevel_t *lev = tlb_map32(vm, va, p2, p1);
        return !lev || tlb_realize_pte(vm, va, lev, pte, ecode);
}

int
mmu_reload_pae_pdpte(vm_t *vm, bool *ret_modified)
{
        vmxmon_t *x = vm->com;

        u64 *pdpte;
        if (x->pdpte[0] == -1) {
                /* Old userland Simics which doesn't provide PDPTEs to us.
                   We preserve here old vmxmon behavior which may not work
                   for non-root EPT guest simulation. */
                vmx_page_t *p;
                rwx_t rwx = RWX_R;
                
                tphys_t tmp_pa = (u32)x->cr3;
                if (gpa_to_pa(vm, &tmp_pa, rwx))
                        return 1;
                if (vmxpage_lookup(vm, tmp_pa, &p, rwx))
                        return 1;
                
                pdpte = (u64*)p->upage.p + ((x->cr3 & 0xfe0) >> 3);
        } else {
                pdpte = x->pdpte;
        }

        unsigned i;
        *ret_modified = false;
        for (i = 0; i < 4; i++) {
                *ret_modified |= (vm->guest_pdpte[i] != pdpte[i]);
                vm->guest_pdpte[i] = pdpte[i];
        }
        return 0;
}

static mmu_ops_t pae_mmu_ops = {
        .page_fault     = pae_page_fault,
};


/************************************************************************/
/*	legacy 32-bit paging    					*/
/************************************************************************/

static int
classic_page_fault(vm_t *vm, u8 ecode, u64 va)
{
        vmxmon_t *x = vm->com;

        /* virtual breakpoints check */
        if (tlb_lin_bp_check(vm, va, ecode))
                return 1;

        /* --- LEVEL 2 (PDE) --- */
        vmx_page_t *p2;
        tphys_t tmp_pa = x->cr3;
        if (gpa_to_pa(vm, &tmp_pa, RWX_GUEST_PAGE_STRUCT))
                return 1;
        if (vmxpage_lookup(vm, tmp_pa, &p2, RWX_GUEST_PAGE_STRUCT))
                return 1;
        u32 *pde_ptr = (u32*)p2->upage.p + ((va >> 22) & 0x3ff);
        u32 pde = *pde_ptr;
        bool page_4mb = (pde & PDE_PS) && (x->cr4 & CR4_PSE);

        u32 p_rsv = (((BITMASK(63, vm->maxphyaddr)) >> 32) & 0xff) << 13;
        u32 rsv = page_4mb ? p_rsv | BIT(21) : 0;
        if (handle_missing_rsvd(vm, va, pde, ecode, rsv))
                return 1;
        u64 perm = combine_perm(pde, PTE_U | PTE_W);

        /* --- LEVEL 1 (PTE) --- */
        u64 pte;
        u32 *pte_ptr;
        vmx_page_t *p1;
        if (page_4mb) {
                /* 4MB mapping, PAT cleared */
                pte = (pde & 0xffc00fff) | ((u64)(pde & 0x001fe000) << 19)
                        | (va & 0x3ff000);
                p1 = NULL;
                pte_ptr = NULL;
                
                if ((pte & PTE_U))
                        BUMP(PERFCTR_TLB_LARGE_USER_PAGE);
                else
                        BUMP(PERFCTR_TLB_LARGE_SU_PAGE);
        } else {
                tmp_pa = pde;
                if (gpa_to_pa(vm, &tmp_pa, RWX_GUEST_PAGE_STRUCT))
                        return 1;
                if (vmxpage_lookup(vm, tmp_pa, &p1, RWX_GUEST_PAGE_STRUCT))
                        return 1;
                pte_ptr = (u32*)p1->upage.p + ((va >> 12) & 0x3ff);
                pte = (u64)*pte_ptr;
                if (handle_missing_rsvd(vm, va, pte, ecode, 0))
                        return 1;
                perm = combine_perm(pte, perm);

                if ((pte & PTE_U))
                        BUMP(PERFCTR_TLB_USER_PAGE);
                else
                        BUMP(PERFCTR_TLB_SU_PAGE);
        }
        
        /* check permissions */
        if (handle_perm_violation(vm, va, perm, ecode))
                return 1;

        /* update accessed bits */
        if (page_4mb) {
                *pde_ptr = update_accessed_dirty(pde, ecode);
        } else {
                *pde_ptr |= PDE_A;
                *pte_ptr = update_accessed_dirty(pte, ecode);
        }

        pte = update_pte_bits(pte, perm, ecode);
        mlevel_t *lev = tlb_map32(vm, va, p2, p1);
        return !lev || tlb_realize_pte(vm, va, lev, pte, ecode);
}

static mmu_ops_t classic_mmu_ops = {
        .page_fault     = classic_page_fault,
};


/************************************************************************/
/*	32-bit no-mmu paging (cr0.PG = 0)				*/
/************************************************************************/

static int
no_mmu_page_fault(vm_t *vm, u8 ecode, u64 va)
{
        /* virtual breakpoints check */
        if (tlb_lin_bp_check(vm, va, ecode))
                return 1;

        /* fake a 1-1 PTE */
        u64 pte = (va & ~0xfff) | PTE_A | PTE_D | PTE_U | PTE_W | PTE_P;

        mlevel_t *lev = tlb_map32(vm, va, NULL, NULL);
        return !lev || tlb_realize_pte(vm, va, lev, pte, ecode);
}

static mmu_ops_t no_paging_mmu_ops = {
        .page_fault     = no_mmu_page_fault,
};



/************************************************************************/
/*	long mode paging            					*/
/************************************************************************/

typedef struct {
        vmx_page_t *page;
        u64 *entry;
} walk_step_t;

enum {
        Max_Supported_Walk_Depth = 5
};

typedef struct {
        bool success;
        unsigned depth;
        walk_step_t walk[Max_Supported_Walk_Depth];
        u64 entry;
        u64 perm;
} walk_result_t;

static inline
u64 paddr_reserved_mask(vm_t *vm)
{
        vmxmon_t *x = vm->com;
        return BITMASK(51, vm->maxphyaddr) 
                | ((x->msr_efer & EFER_NXE) ? 0 : BIT(63));
}

static inline unsigned
shift(unsigned depth, unsigned max_walk)
{
        const unsigned level_width = 9;
        const unsigned page_offset = 12;
        return page_offset + level_width * (max_walk - depth - 1);
}

static inline u64 *
entry_on_page(vmx_page_t *page, u64 va, unsigned depth, unsigned max_walk)
{
        const unsigned level_mask = 0x1ff;
        unsigned index = (va >> shift(depth, max_walk)) & level_mask;
        u64 *table = (u64 *)page->upage.p;
        return &table[index];
}

static vmx_page_t *
lookup_target_paging_table(vm_t *vm, tphys_t gpa)
{
        tphys_t tmp_pa = gpa;
        if (gpa_to_pa(vm, &tmp_pa, RWX_GUEST_PAGE_STRUCT) != 0) {
                return NULL;
        }
        vmx_page_t *res = NULL;
        if (vmxpage_lookup(vm, tmp_pa, &res, RWX_GUEST_PAGE_STRUCT) != 0) {
                return NULL;
        }
        return res;
}

static inline bool
is_entry_terminating(const walk_result_t *wr, unsigned max_walk)
{
        return wr->depth == max_walk || (wr->entry & PDE_PS);
}

static walk_result_t
walk(vm_t *vm, u64 va, u8 ecode, const unsigned max_walk)
{
        const u64 physmask = BITMASK(51, 12);
        const u64 rsv = paddr_reserved_mask(vm);

        walk_result_t res = {
                .success = true,
                .depth = 0,
                .entry = vm->com->cr3,
                .perm = PTE_U | PTE_W,
                .walk = {}
        };

        do {
                tphys_t gpa = res.entry & physmask;
                vmx_page_t *page = lookup_target_paging_table(vm, gpa);
                if (!page) {
                        return (walk_result_t){.success = false};
                }
                res.walk[res.depth].page = page;

                u64 *entry = entry_on_page(page, va, res.depth, max_walk);
                res.walk[res.depth].entry = entry;
                res.entry = *entry;

                if (handle_missing_rsvd(vm, va, res.entry, ecode, rsv)) {
                        return (walk_result_t){.success = false};
                }
                res.perm = combine_perm(res.entry, res.perm);
                res.depth++;
        } while (!is_entry_terminating(&res, max_walk));
        return res;
}

typedef enum {
        Pte_4K = 1,
        Pte_2M = 2,
        Pte_1G = 3,
        Pte_Invalid_4 = 4,
        Pte_Invalid_5 = 5,
} pte_level_t;

static bool
is_leaf_entry_indeed_pte(u64 entry, pte_level_t pte_level, bool has_1gb_page)
{
        if (pte_level == Pte_4K) {
                return true;
        } else if (pte_level == Pte_2M) {
                if (entry & BITMASK(20, 13)) {
                        return false;
                }
        } else if (pte_level == Pte_1G) {
                if (!has_1gb_page || (entry & BITMASK(29, 13))) {
                        return false;
                }
        } else {
                /* Leaf entry on levels 4 and 5 cannot be PTE */
                return false;
        }
        return true;
}

static void
bump_tlb_page_counter(vm_t *vm, pte_level_t pte_level, u64 pte)
{
        bool is_user = pte & PTE_U;
        unsigned counter = (pte_level == Pte_4K)?
            (is_user? PERFCTR_TLB_USER_PAGE : PERFCTR_TLB_SU_PAGE)
            :(is_user? PERFCTR_TLB_LARGE_USER_PAGE: PERFCTR_TLB_LARGE_SU_PAGE);
        BUMP(counter);
}

static u64
shadow_4kb_pte_from_target_pte(u64 leaf_entry, u64 va, pte_level_t pte_level)
{
        if (pte_level == Pte_1G) {
                return (leaf_entry & ~0x3ffff000) | (va & 0x3ffff000);
        } else if (pte_level == Pte_2M) {
                return (leaf_entry & ~0x1ff000) | (va & 0x1ff000);
        } else { /* 4kB page */
                return leaf_entry;
        }
}

static u64
mark_walk_accessed_dirty(const walk_result_t *wr, u64 pte, u8 ecode)
{
        for (unsigned i = 0; i < wr->depth - 1; i++) {
                *wr->walk[i].entry |= PDE_A;
        }
        *wr->walk[wr->depth - 1].entry = update_accessed_dirty(wr->entry, ecode);

        pte = update_pte_bits(pte, wr->perm, ecode);
        return pte;
}

static bool
check_pte_permissions(vm_t *vm, u64 va, u64 pte, u8 ecode, u64 *perm)
{
        if (pku_update_perm(vm, pte, perm)) {
                set_vmret(vm, VMRET_PKU_VIOLATION);
                return false;
        }

        if (handle_perm_violation(vm, va, *perm, ecode)) {
                return false;
        }
        return true;
}

static void
extract_walked_pages(const walk_step_t walk[], vmx_page_t* walked_pages[])
{
        for (int i = 0; i < Max_Supported_Walk_Depth; i++) {
                walked_pages[i] = walk[i].page;
        }
}

typedef mlevel_t * (map_function_t)(vm_t *vm, u64 va, vmx_page_t *p[]);

static int
long_page_fault(vm_t *vm, u8 ecode, u64 va, unsigned max_walk,
        map_function_t tlb_map)
{
        vmxmon_t *x = vm->com;
        /* virtual breakpoints check */
        if (tlb_lin_bp_check(vm, va, ecode))
                return 1;

        walk_result_t wr = walk(vm, va, ecode, max_walk);
        if (!wr.success) {
                return 1;
        }

        const pte_level_t pte_level = max_walk - wr.depth + 1;

        if (!is_leaf_entry_indeed_pte(wr.entry, pte_level, x->has_1gb_mmu_page)) {
                raise_rsvd(vm, va, ecode);
                return 1;
        }

        u64 pte = shadow_4kb_pte_from_target_pte(wr.entry, va, pte_level);

        bump_tlb_page_counter(vm, pte_level, pte);

        if (!check_pte_permissions(vm, va, pte, ecode, &wr.perm)) {
                return 1;
        }

        pte = mark_walk_accessed_dirty(&wr, pte, ecode);

        vmx_page_t *walked_pages[Max_Supported_Walk_Depth] = {0};
        extract_walked_pages(wr.walk, walked_pages);

        mlevel_t *lev = tlb_map(vm, va, walked_pages);
        if (!lev) {
	        return 1;
	}
        int res = tlb_realize_pte(vm, va, lev, pte, ecode);
        return res;
}

static int
four_level_page_fault(vm_t *vm, u8 ecode, u64 va)
{
        return long_page_fault(vm, ecode, va, 4, tlb_map_4_levels);
}

static int
five_level_page_fault(vm_t *vm, u8 ecode, u64 va)
{
        return long_page_fault(vm, ecode, va, 5, tlb_map_5_levels);
}

static mmu_ops_t four_level_mmu_ops = {
        .page_fault     = four_level_page_fault,
};

static mmu_ops_t five_level_mmu_ops = {
        .page_fault = five_level_page_fault,
};

static unsigned
mmu_qualifier(vmxmon_t *x, unsigned mode)
{
        bool is_five_level_requested = (x->cr4 & CR4_LA57)
                && (mode == MMU_MODE_LONG);
        bool has_classic_page_extension = (mode == MMU_MODE_CLASSIC)
                && (x->cr4 & CR4_PSE);
        bool has_no_execute = (mode == MMU_MODE_PAE || mode == MMU_MODE_LONG)
                && (x->msr_efer & EFER_NXE);
        bool has_write_protect = (x->cr0 & CR0_WP)
                || mode == MMU_MODE_NO_PAGING;
        unsigned mode_qual = 0;
        if (has_classic_page_extension) {
                mode_qual |= MMU_MODE_QUAL_CR4_PSE;
        }
        if (has_no_execute) {
                mode_qual |= MMU_MODE_QUAL_NXE;
        }
        if (has_write_protect) {
                mode_qual |= MMU_MODE_QUAL_WP;
        }
        if (is_five_level_requested) {
                mode_qual |= MMU_MODE_QUAL_5_LEVEL;
        }
        return mode_qual;
}

static unsigned
mmu_mode(vmxmon_t *x)
{
        const unsigned mode = (x->msr_efer & EFER_LMA) ? MMU_MODE_LONG :
                (!(x->cr0 & CR0_PG)) ? MMU_MODE_NO_PAGING :
                (x->cr4 & CR4_PAE) ? MMU_MODE_PAE :
                MMU_MODE_CLASSIC;

        unsigned mode_qual = mmu_qualifier(x, mode);
        return mode | mode_qual;
}

static void
update_shadow_page_fault_handler(vm_t *vm, unsigned mmu_mode)
{
        bool is_five_level_requested = mmu_mode & MMU_MODE_QUAL_5_LEVEL;
        switch (PRIMARY_MMU_MODE(mmu_mode)) {
        case MMU_MODE_PAE:
                vm->mmu_ops = pae_mmu_ops;
                break;
        case MMU_MODE_LONG:
                if (is_five_level_requested) {
                        vm->mmu_ops = five_level_mmu_ops;
                } else {
                        vm->mmu_ops = four_level_mmu_ops;
                }
                break;
        case MMU_MODE_CLASSIC:
                vm->mmu_ops = classic_mmu_ops;
                break;
        case MMU_MODE_NO_PAGING:
                vm->mmu_ops = no_paging_mmu_ops;
                break;
        default:
                wrap_BUG_ON(1);
        }

}

bool
mmu_mode_change(vm_t *vm)
{
        vmxmon_t *x = vm->com;
        unsigned mode = mmu_mode(x);

        if (mode == vm->mmu_mode)
                return false;
        update_shadow_page_fault_handler(vm, mode);
        vm->mmu_mode = mode;
        tlb_mmu_mode_change(vm);
        return true;
}

int
mmu_init(vm_t *vm)
{
        vm->mmu_mode = MMU_MODE_UNINITIALIZED;
        return 0;
}
