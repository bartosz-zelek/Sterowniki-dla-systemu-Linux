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

#ifndef DBG_CONFIG_H
#define DBG_CONFIG_H

// Enable fireware debug.
#define FIREWIRE_DEBUG 0
// Send extra SMIs to discover BIOS bugs. Requires VMCS_DEBUG.
#define DEBUG_SEND_SMI 0
// Save VMCS context to memory before VM-enter and after VM-exit.
#define VMCS_DEBUG 0
/* set to 1 to enable (somewhat expensive) runtime checks */
#define ENABLE_STATIC_HOST_STATE_ASSERT 0
// Debug stack on VMENTRY/VMEXIT
#define VMX_DEBUG_STACK_GUARD 0
/* Set to 1 to enable debug checks of the allocation context. */
#define DEBUG_ALLOCATION_CONTEXT 0
/* Set to 1 to enable checks for page leaks. */
#define DEBUG_PAGE_LEAKS 0

#endif
