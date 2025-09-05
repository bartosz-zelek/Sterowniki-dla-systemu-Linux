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

#ifndef MM_H
#define MM_H

#include "tphys.h"

#define MMU_MODE_NO_PAGING     0       /* 32-bit 1-1 mapping */
#define MMU_MODE_LONG          1       /* 64-bit paging, either 4 or 5-level */
#define MMU_MODE_PAE           2       /* 32-bit pae paging */
#define MMU_MODE_CLASSIC       3       /* 32-bit non-pae paging */
#define MMU_MODE_MASK          0x3     /* basic MMU mode */
#define MMU_MODE_QUAL_WP       4       /* cr0.WP is set */
#define MMU_MODE_QUAL_CR4_PSE  8       /* cr4.PSE is set (classic paging) */
#define MMU_MODE_QUAL_NXE      16      /* EFER.NXE is set */
#define MMU_MODE_QUAL_5_LEVEL  32      /* CR4.LA57 is set */
#define MMU_MODE_UNINITIALIZED 64      /* MMU mode not set */

#define PRIMARY_MMU_MODE(mode)  ((mode) & MMU_MODE_MASK)
#define MMU_SUBMODE_WP(mode)    (((mode) & MMU_MODE_QUAL_WP) ? 1 : 0)
#define IS_MMU_MODE_NO_PAGING(mode) \
        (PRIMARY_MMU_MODE(mode) == MMU_MODE_NO_PAGING)
#define IS_MMU_MODE_LONG(mode) \
        (PRIMARY_MMU_MODE(mode) == MMU_MODE_LONG)
#define IS_MMU_MODE_PAE(mode) \
        (PRIMARY_MMU_MODE(mode) == MMU_MODE_PAE)
#define IS_MMU_MODE_CLASSIC(mode) \
        (PRIMARY_MMU_MODE(mode) == MMU_MODE_CLASSIC)

extern int      mmu_init(vm_t *vm);
extern bool     mmu_mode_change(vm_t *vm);
extern int      mmu_reload_pae_pdpte(vm_t *vm, bool *ret_modified);

extern int gpa_to_pa(vm_t *vm, tphys_t *in_out_pa, rwx_t rwx);

extern int gpa_to_pa_perm(vm_t *vm, tphys_t *in_out_pa, rwx_t rwx, rwx_t *ept_perm);

static inline u64
make_amask(vm_t *vm)
{
        return ~BITMASK(51, vm->maxphyaddr) & BITMASK(51, 12);
}

#endif   /* MM_H */
