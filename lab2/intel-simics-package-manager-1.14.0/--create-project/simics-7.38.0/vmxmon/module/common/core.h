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

#ifndef INCLUDE_CORE_H
#define INCLUDE_CORE_H

#include "ktypes.h"
#include "vmxioctl.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct virtual_machine vm_t;

int      dev_register(void);
void     dev_unregister(void);

void     dev_release(vm_t *vm);
vm_t     *dev_open(void);

long     dev_ioctl_enter(vm_t *vm);
long     dev_ioctl_get_regs(vm_t *vmm, u32 regbits);
long     dev_ioctl_signal(vm_t *vm);

void     dev_ioctl_get_trace(vm_t *vm, vmx_trace_t *t);
void     dev_ioctl_get_vmcs(vm_t *vm, vmxdbg_vmcs_t *d);
long     dev_ioctl_request(vm_t *vm, vmxmon_req_t *req);

void     dev_suspend(void);
void     dev_resume(void);

void     dev_cpu_down(int cpu);
void     dev_cpu_up(int cpu);

bool     handle_nmi(void);

#if defined(__cplusplus)
}
#endif

#endif
