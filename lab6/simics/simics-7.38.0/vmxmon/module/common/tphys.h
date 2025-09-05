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

#ifndef TPHYS_H
#define TPHYS_H

#include "ktypes.h"
#include "kernel-api.h"
#include "cpudefs.h"
#include "vm.h"

#define PFLAG_RAM                0x1    /* RAM mapping */
#define PFLAG_WRITABLE           0x2    /* RAM r/w mapping */
#define PFLAG_PTABLE             0x4    /* page table */

#define RWX_R   1       /* readable */
#define RWX_W   2       /* writable */
#define RWX_XS  4       /* supervisor executable */
#define RWX_XU  BIT(10) /* user executable */

#define RWX_X   (RWX_XS | RWX_XU)
#define RWX_ALL (RWX_R | RWX_W | RWX_XS | RWX_XU)

typedef unsigned rwx_t;

typedef u64                tphys_t;     /* target physical address */
typedef struct page_usage  page_usage_t;
typedef struct vmx_page    vmx_page_t;
typedef struct page_group  page_group_t;

struct vmx_page {                       /* target physical page */
        upage_t         upage;          /* userspace page */
        tphys_t         tphys;          /* target physical address of page */

        rwx_t           rwx;            /* access rights */

        struct guest_page *guest_page;    /* for nested EPT virtualization */
        page_usage_t    *users;         /* all usages of this page */
};

extern int      tphys_init(vm_t *vm);
extern void     tphys_update_walk_length(tphys_space_t *s,
                                         bool use_5_level_pages);
extern void     tphys_share(vm_t *vm, vm_t *pspace_vm);
extern void     tphys_unlink_and_unlock(vm_t *vm);
extern int      tphys_setup_shared_vmxmon(tphys_space_t *s,
                                          uintptr_t user_page);
extern void     tphys_unmap_shared_vmxmon(tphys_space_t *s);

extern void     tphys_perform_lazy_flushing(tphys_space_t *s);
extern int      tphys_protect_page(tphys_space_t *s, tphys_t tphys,
                                   unsigned prot);
extern int      tphys_add_mapping(tphys_space_t *s, tphys_t tphys,
                                  uintptr_t va, unsigned prot);
extern u64      tphys_get_eptp(const tphys_space_t *s);
extern u64      encoded_eptp_walk_length(tphys_space_t *s);

extern void     tphys_lock(tphys_space_t *s);
extern void     tphys_unlock(tphys_space_t *s);

extern void     vmxpage_drop_usage(tphys_space_t *s, page_usage_t *m);
struct mlevel;
extern page_usage_t *vmxpage_add_usage(vm_t *vm, vmx_page_t *xp,
                                       struct mlevel *level, unsigned qual,
                                       unsigned pflags);

extern void     vmxpage_memory_limit_prune(vm_t *vm);

extern int      vmxpage_lookup(vm_t *vm, tphys_t pa, vmx_page_t **ret,
                               rwx_t rwx);
extern int      vmxpage_lookup_nomap(vm_t *vm, tphys_t pa, vmx_page_t **ret,
                                     rwx_t rwx);


#endif   /* TPHYS_H */
