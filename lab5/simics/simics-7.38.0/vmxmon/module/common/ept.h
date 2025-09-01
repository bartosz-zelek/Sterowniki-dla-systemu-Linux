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

#ifndef VMXMON_EPT_H
#define VMXMON_EPT_H

#include "vm.h"
#include "tphys.h"

static inline int
guest_ept_violation(vm_t *vm)
{
        set_vmret(vm, VMRET_GUEST_EPT_VIOLATION);
        return 1;
}

static inline int
guest_ept_misconfig(vm_t *vm)
{
        set_vmret(vm, VMRET_GUEST_EPT_MISCONFIG);
        return 1;
}

static inline rwx_t
epte_perm(vm_t *vm, u64 epte)
{
        rwx_t perm = epte & (RWX_R | RWX_W | RWX_XS);

        if (vm->use_mbe)
                perm |= epte & RWX_XU;
        else
                perm |= (perm & RWX_XS) ? RWX_XU : 0;

        return perm;
}

/* Get EPT entry and check for the entry for violation/misconfiguration.
   Returns 0 on success and the entry in ret_epte.
   Returns 1 on failure and sets vmret. */
extern int get_ept_entry(vm_t *vm, int level, tphys_t page_pa, unsigned index,
                         rwx_t rwx, u64 *ret_epte);

#endif  /* VMXMON_EPT_H */
