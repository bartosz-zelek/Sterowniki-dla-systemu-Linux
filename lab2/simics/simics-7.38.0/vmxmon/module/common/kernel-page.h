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

#ifndef INCLUDE_KERNEL_PAGE_H
#define INCLUDE_KERNEL_PAGE_H

#ifndef __linux__
#include <wdm.h>
#endif

#include "ktypes.h"

#ifdef __cplusplus
extern "C" {
#endif

/* page allocation (unpinned) */
char *x_alloc_page(void);
void x_free_page(char *p);


/* page allocation (for pinned pages) */
typedef struct {
        /* page address (public) */
        char *p;
        /* private fields */
#ifdef __linux__
        struct page *page;
#else
        struct _MDL *mdl;
#endif
} mpage_t;

char *alloc_mpage(mpage_t *mpage);
char *alloc_mpage32(mpage_t *mpage);
void free_mpage(mpage_t *mpage);
u64 mpage_phys(mpage_t *mpage);
void mpage_kunmap(mpage_t *mpage);

#define alloc_mpage_native alloc_mpage

static inline bool
mpage_allocated(mpage_t *mpage)
{
#if defined(__linux__)
        return !!mpage->page;
#else
        return !!mpage->mdl;
#endif
}

/* page handling (userspace) */
typedef struct {
        /* kernel access addr */
        char *p;

        /* private fields */
#ifdef __linux__
        union {
                struct page *page;
                unsigned long page_phys;
        };
#else
        MDL mdl;
        /* The space immediately following a MDL must be reserved for
           an array with #pages elements. */
        PFN_NUMBER _mdl_pfn[1];
#endif        
} upage_t;

int get_upage(uintptr_t va, upage_t *upage);
void put_upage(upage_t *upage);
u64 upage_phys(upage_t *upage);
char *map_upage(upage_t *upage);
void unmap_upage(upage_t *upage);

static inline bool
upage_is_mapped(upage_t *upage)
{
        return !!upage->p;
}

#ifdef __cplusplus
}
#endif

#endif
