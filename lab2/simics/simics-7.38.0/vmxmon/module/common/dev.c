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

#include "vmxmon.h"
#include "vm.h"
#include "simcall.h"
#include "core.h"
#include "tlb.h"
#include "fw-dbg.h"

global_t        gi;             /* globals */

static mutex_t  *initmutex;
static int      opencnt;          /* protected by initmutex */
static bool     is_suspending;    /* protected by initmutex */
static int      next_session_id;  /* protected by initmutex */
static vm_t     *all_vms;         /* protected by initmutex */

static vm_t *
lookup_vm(int session_id)
{
        vm_t *vm;

        for (vm = all_vms; vm && vm->session_id != session_id; vm = vm->next)
                ;

        return vm;
}

static inline bool
cannot_share_between_vms(vm_t *vm, vm_t *vm2)
{
        if (!vm2) {
                return true;
        }
        bool same_vm = vm == vm2;
        bool already_shared_pspace = vm->pspace == vm2->pspace;
        bool same_process = vm->process_id == vm2->process_id;
        bool same_walk_len = vm->flags == vm2->flags;
        bool cannot_share = same_vm || already_shared_pspace
                || !same_process || !same_walk_len;
        return cannot_share;
}

/* use the pspace associated with session_id */
int
share_pspace(vm_t *vm, int session_id)
{
        int ret = -EINVAL;
        wrap_mutex_lock_nested(initmutex, INIT_LEVEL);
        vm_t *vm2 = lookup_vm(session_id);

        if (!is_vm_set_up(vm)) {
                ret = -EINVAL;
        } else if (cannot_share_between_vms(vm, vm2)) {
                ret = -EINVAL;
        } else {
                wrap_mutex_lock_nested(vm->vm_lock, VM_LEVEL);

                /* free the old pspace */
                tphys_lock(vm->pspace);
                tphys_unlink_and_unlock(vm);

                tphys_lock(vm2->pspace);

                tphys_share(vm, vm2);  /* share the pspace from vm2 */

                tphys_unlock(vm2->pspace);
                wrap_mutex_unlock(vm->vm_lock);
                ret = 0;
        }
        wrap_mutex_unlock(initmutex);
        return ret;
}

static u16
get_free_vpid(void) 
{
        u16 vpid = 1;
        for (;;) {
                vm_t *vm = all_vms;
                while (vm != NULL && vm->vpid != vpid)
                        vm = vm->next;
                if (vm == NULL)
                        return vpid;
                vpid++;
        }
        /* not reached */
}

int
dbg_get_cpu_with_vmcs(void)
{
        wrap_mutex_lock_nested(initmutex, INIT_LEVEL);
        vm_t *vm;
        for (vm = all_vms; vm && vm->cpu == -1; vm = vm->next)
                ;
        int cpu = vm ? vm->cpu : -1;
        wrap_mutex_unlock(initmutex);
        return cpu;
}

void
dev_release(vm_t *vm)
{
        if (vm) {
                wrap_mutex_lock_nested(initmutex, INIT_LEVEL);
                vm_t **pp = &all_vms;
                for (; *pp != vm; pp = &(**pp).next)
                        ;
                *pp = vm->next;
                wrap_mutex_unlock(initmutex);
                vm_destroy(vm);
                wrap_mutex_lock_nested(initmutex, INIT_LEVEL);
                opencnt--;
                if (opencnt == 0)
                        vmx_last_vm_closed();
                wrap_mutex_unlock(initmutex);
        }
}

vm_t *
dev_open(void)
{
        vm_t *vm = vm_create();
        if (vm) {
                wrap_mutex_lock_nested(initmutex, INIT_LEVEL);
                opencnt++;
                vm->next = all_vms;
                all_vms = vm;
                vm->session_id = ++next_session_id;
                vm->vpid = get_free_vpid();
                if (is_suspending)
                        vm_suspend(vm);
                wrap_mutex_unlock(initmutex);
        }
        return vm;
}

int
dev_register(void)
{
        int err;

        initmutex = wrap_alloc_mutex();
        if (!initmutex)
                return -ENOMEM;

        err = vmx_mod_init();
        if (err)
                return err;
        fw_dbg_init();
        
        return 0;
}

void
dev_unregister(void)
{
        vmx_mod_cleanup();
        fw_dbg_cleanup();

        wrap_free_mutex(initmutex);
}

static void
dev_suspend_locked(void)
{
        vm_t *vm;
        for (vm = all_vms; vm; vm = vm->next)
                vm_suspend(vm);
        vmx_all_vms_suspended();
}

static void
dev_resume_locked(void)
{
        vm_t *vm;
        for (vm = all_vms; vm; vm = vm->next)
                vm_resume(vm);
}

void
dev_suspend(void)
{
        wrap_mutex_lock_nested(initmutex, INIT_LEVEL);
        is_suspending = true;
        dev_suspend_locked();
        wrap_mutex_unlock(initmutex);
}

void
dev_resume(void)
{
        wrap_mutex_lock_nested(initmutex, INIT_LEVEL);
        is_suspending = false;
        dev_resume_locked();
        wrap_mutex_unlock(initmutex);
}

void
dev_cpu_up(int cpu)
{
        vmx_cpu_up(cpu);
}

void
dev_cpu_down(int cpu)
{
        /* We reuse the suspend/resume mechanism to ensure that the CPU
           going down can be removed without races. */
        wrap_mutex_lock_nested(initmutex, INIT_LEVEL);
        if (!is_suspending)
                dev_suspend_locked();
        vmx_cpu_down(cpu);
        if (!is_suspending)
                dev_resume_locked();
        wrap_mutex_unlock(initmutex);
}
