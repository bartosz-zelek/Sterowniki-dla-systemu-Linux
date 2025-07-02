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

#ifndef COMMON_DEV_IOCTL_H
#define COMMON_DEV_IOCTL_H

#include "user-copy.h"

static inline long
dev_ioctl(vm_t *vm, unsigned int cmd, user_buffer_t *arg)
{
        if (likely(cmd == VMXMON_ENTER))
                return dev_ioctl_enter(vm);
        else if (cmd == VMXMON_GET_REGS) {
                u64 regbits = 0;
                if (x_copy_from_user(&regbits, arg, sizeof regbits))
                        return -EFAULT;
                return dev_ioctl_get_regs(vm, regbits);
        } else if (cmd == VMXMON_SIGNAL)
                return dev_ioctl_signal(vm);
        else if (cmd == VMXMON_REQUEST) {
                vmxmon_req_t req = {0};
                long ret = 0;
                if (x_copy_from_user(&req, arg, sizeof req))
                        return -EFAULT;
                ret = dev_ioctl_request(vm, &req);
                if (ret < 0)
                        return ret;
                if (ret > 0) {
                        if (x_copy_to_user(arg, &req, sizeof req))
                                return -EFAULT;
                }
                return ret;
        } else if (cmd == VMXMON_GET_TRACE) {
                vmx_trace_t *t = x_malloc(sizeof *t);
                long ret = 0;
                if (!t)
                        return -ENOMEM;
                dev_ioctl_get_trace(vm, t);
                if (x_copy_to_user(arg, t, sizeof *t))
                        ret = -EFAULT;
                x_free(t);
                return ret;
        } else if (cmd == VMXMON_GET_VMCS) {
                vmxdbg_vmcs_t *d = x_malloc(sizeof *d);
                long ret = 0;
                if (!d)
                        return -ENOMEM;
                dev_ioctl_get_vmcs(vm, d);
                if (x_copy_to_user(arg, d, sizeof *d))
                        ret = -EFAULT;
                x_free(d);
                return ret;
        } else {
                return -EINVAL;
        }
}

#endif  /* COMMON_DEV_IOCTL_H */
