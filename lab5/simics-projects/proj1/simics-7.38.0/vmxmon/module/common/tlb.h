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

#ifndef TLB_H
#define TLB_H

#include "cpudefs.h"
#include "mm.h"

typedef struct mlevel   mlevel_t;

int     tlb_init(vm_t *vm);
void    tlb_cleanup(vm_t *vm);

void    tlb_clear(vm_t *vm);
void    tlb_memory_limit_prune(vm_t *vm);

/* MMU interface */
void    tlb_mmu_mode_change(vm_t *vm);
int     tlb_activate_cr3(vm_t *vm);
void    tlb_flush(vm_t *vm, bool keep_global);
void    tlb_invlpg(vm_t *vm, u64 va);

/* fault-in interface */
int     tlb_realize_pte(vm_t *vm, u64 va, mlevel_t *lev, u64 pte, u8 ecode);
mlevel_t *tlb_map32(vm_t *vm, u32 va, vmx_page_t *p2, vmx_page_t *p1);
mlevel_t *tlb_map_4_levels(vm_t *vm, u64 va, vmx_page_t *p[4]);
mlevel_t *tlb_map_5_levels(vm_t *vm, u64 va, vmx_page_t *p[5]);


/* breakpoint interface */
int     tlb_lin_bp_check(vm_t *vm, u64 va, u8 ecode);
void    tlb_remove_lin_bp(vm_t *vm, int id);
int     tlb_add_lin_bp(vm_t *vm, uintptr_t start,
                                         unsigned long n_pages, int prot);
bool    tlb_has_lin_bps(vm_t *vm);

/* tphys callback interface */
void    _tlb_unmap_page(mlevel_t *lev, unsigned qual);
void    _tlb_write_protect_page(mlevel_t *lev, unsigned qual);
void    _tlb_make_page_noexec(mlevel_t *lev, unsigned qual);

#endif   /* TLB_H */
