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

#ifndef NESTED_EPT_H
#define NESTED_EPT_H

#include "ktypes.h"
#include "kernel-api.h"
#include "vm.h"
#include "tphys.h"

typedef struct guest_page  guest_page_t;

/* init */
int nested_epts_init(vm_t *vm);
void nested_epts_cleanup(vm_t *vm);

/* vmx.c interface */
u64 guest_ept_get_eptp(vm_t *vm);
int handle_nested_ept_fault(vm_t *vm, u64 gpa, rwx_t rwx);
void change_guest_ept(vm_t *vm);
void handle_invept_flush(vm_t *vm, u64 eptp);
void handle_ept_viol_flush(vm_t *vm, u64 gpa, u32 access, u64 eptp);

bool need_guest_ept_flush(vm_t *vm);
void mark_all_guest_eptps_for_flush(vm_t *vm);


/* tphys.c interface */
void free_vmxpage_guest_pages(vmx_page_t *xp);
void update_vmxpage_guest_perm(vmx_page_t *xp);
void drop_all_guest_pages(vm_t *vm);

#endif
