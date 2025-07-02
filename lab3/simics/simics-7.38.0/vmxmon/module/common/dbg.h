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

#ifndef DBG_H
#define DBG_H

#include "dbg-config.h"
#include "vm.h"
#include "fw-dbg.h"

typedef struct vmxdbg_state {
        u64 time_stamp;
        u64 cr8;
        vmxdbg_vmcs_t vmcs_dbg;
} vmxdbg_state_t;

typedef struct vmx_dbg {
        u64             magic;
        vmxdbg_state_t  entry_state;
        vmxdbg_state_t  exit_state;
        vmxdbg_state_t  nmi_state;
} vmx_dbg_t;

#if VMCS_DEBUG
int dbg_vmcs_init(vm_t *vm);
void dbg_vmcs_destroy(vm_t *vm);
void dbg_vmcs_entry(vm_t *vm);
void dbg_vmcs_exit(vm_t *vm);
void dbg_vmcs_nmi(vm_t *vm);
#else
static inline int dbg_vmcs_init(vm_t *vm) { return 0; }
static inline void dbg_vmcs_destroy(vm_t *vm) {}
static inline void dbg_vmcs_exit(vm_t *vm) {}
static inline void dbg_vmcs_entry(vm_t *vm) {}
static inline void dbg_vmcs_nmi(vm_t *vm) {}
#endif

void dbg_print_banner(void);

#endif  /* DBG_H */
