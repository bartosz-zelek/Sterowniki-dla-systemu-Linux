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

#include "ept.h"
#include "mm.h"

static int
check_ept_entry(vm_t *vm, int level, u64 entry)
{
        vmxmon_t *x = vm->com;
        bool mbe = x->vmcs_proc2_ctls & PROC2_MODE_BASED_EXEC_EPT;
        u64 exec_mask = EPT_XS_ACCESS | (mbe ? EPT_XU_ACCESS : 0);
        u64 access_mask = EPT_R_ACCESS | EPT_W_ACCESS | exec_mask;

        if ((entry & access_mask) == 0 /* not present */)
                return guest_ept_violation(vm);

        if ((entry & (EPT_R_ACCESS | EPT_W_ACCESS)) == EPT_W_ACCESS)
                return guest_ept_misconfig(vm);

        bool exec_only_allowed = x->vmx_msr_ept_vpid_cap & 1;
        if (!exec_only_allowed && !(entry & EPT_R_ACCESS)
            && (entry & exec_mask))
                return guest_ept_misconfig(vm);

        if (entry & BITMASK(51, vm->maxphyaddr))
                return guest_ept_misconfig(vm);

        /* large page? */
        if (level > 1 && (entry & 0xf8)) {
                /* 512 GB pages and larger are not supported for now */
                if (level >= 4)
                        return guest_ept_misconfig(vm);
                if (!(entry & 0x80))
                        return guest_ept_misconfig(vm);

                bool has_1GB_pages = x->vmx_msr_ept_vpid_cap & (1 << 17);
                if (level == 3 && !has_1GB_pages)
                        return guest_ept_misconfig(vm);

                bool has_2MB_pages = x->vmx_msr_ept_vpid_cap & (1 << 16);
                if (level == 2 && !has_2MB_pages)
                        return guest_ept_misconfig(vm);

                u64 page_mask = (1ULL << (12 + 9 * (level - 1))) - 1;
                if (entry & page_mask & make_amask(vm))
                        return guest_ept_misconfig(vm);
        }
        if (level == 1 || entry & 0x80) {
                /* TODO: validate memory type */
        }

        return 0;
}

static int
handle_ept_ad_flags(vm_t *vm, int level, rwx_t rwx, u64 epte)
{
        if (!(vm->com->vmx_eptp & BIT(6)))
                return 0;

        if (!(epte & BIT(8))) {
                set_vmret(vm, VMRET_EMULATE);
                return 1;
        }

        if (level == 1 || epte & BIT(7)) {
                if ((rwx & RWX_W) && !(epte & BIT(9))) {
                        set_vmret(vm, VMRET_EMULATE);
                        return 1;
                }
        }

        return 0;
}

int
get_ept_entry(vm_t *vm, int level, tphys_t page_pa, unsigned index, rwx_t rwx,
              u64 *ret_epte)
{
        vmx_page_t *xp;
        if (vmxpage_lookup(vm, page_pa, &xp, RWX_R))
                return 1;

        u64 epte = *((u64 *)xp->upage.p + index);

        if (check_ept_entry(vm, level, epte))
                return 1;

        if (handle_ept_ad_flags(vm, level, rwx, epte))
                return 1;

        *ret_epte = epte;
        return 0;
}
