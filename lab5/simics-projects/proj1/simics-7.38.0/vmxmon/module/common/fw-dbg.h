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

#ifndef FW_DBG_H
#define FW_DBG_H

#include "dbg-config.h"

#if FIREWIRE_DEBUG
void fw_dbg_init(void);
void fw_dbg_cleanup(void);
void fw_dbg_vm_created(struct virtual_machine *vm);
void fw_dbg_vm_destroyed(struct virtual_machine *vm);
#else
static inline void fw_dbg_init(void) {}
static inline void fw_dbg_cleanup(void) {}
static inline void fw_dbg_vm_created(struct virtual_machine *vm) {}
static inline void fw_dbg_vm_destroyed(struct virtual_machine *vm) {}
#endif

#endif
