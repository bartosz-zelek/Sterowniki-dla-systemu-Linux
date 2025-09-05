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

#include "page.h"
#include <linux/types.h>
#include <linux/mm.h>
#include <linux/highmem.h>
#include <linux/gfp.h>
#include <linux/hardirq.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/atomic.h>
#include "kernel-page.h"
#include "kernel-api.h"
#include "dbg-config.h"

static struct {
        atomic_t n_allocs;
        atomic_t n_pages;
        atomic_t n_mpages;
        atomic_t n_upages;
} dbg;

/* Complain if we are not in a context where sleeping is allowed. */
static void
assert_may_sleep(const char *str)
{
        if (DEBUG_ALLOCATION_CONTEXT) {
                unsigned long flags;
                local_save_flags(flags);
                if (preempt_count() || !(flags & 0x200)) {
                        printm("%s: illegal context"
                               " %ld %ld %ld %d %lx\n",
                               str, in_interrupt(), in_softirq(), in_irq(),
                               preempt_count(), flags);
                }
        }
}

void
assert_memory_released(void)
{
        if (DEBUG_PAGE_LEAKS) {
                int c0 = atomic_read(&dbg.n_allocs);
                int c1 = atomic_read(&dbg.n_pages);
                int c2 = atomic_read(&dbg.n_mpages);
                int c3 = atomic_read(&dbg.n_upages);
                if (c0 != 0 || c1 != 0 || c2 != 0 || c3 != 0)
                        printm("memory leak detected (%d %d %d %d)\n",
                               c0, c1, c2, c3);
        }
}

static inline void
dbg_inc(atomic_t *counter)
{
        if (DEBUG_PAGE_LEAKS)
                atomic_inc(counter);
}

static inline void
dbg_dec(atomic_t *counter)
{
        if (DEBUG_PAGE_LEAKS)
                atomic_dec(counter);
}


/************************************************************************/
/*	general allocations                         			*/
/************************************************************************/

void *
x_malloc(int size)
{
        void *ret = kmalloc(size, GFP_USER);
        assert_may_sleep("x_malloc");
        if (ret)
                dbg_inc(&dbg.n_allocs);
        return ret;
}

void
x_free(void *p)
{
        assert_may_sleep("x_free");
        if (p)
                dbg_dec(&dbg.n_allocs);
        kfree(p);
}

char *
x_alloc_page(void)
{
        char *p = (char*)__get_free_page(GFP_USER | __GFP_ZERO);
        if (!p)
                return NULL;
        assert_may_sleep("x_alloc_page");
        dbg_inc(&dbg.n_pages);
        return p;
}

void
x_free_page(char *p)
{
        dbg_dec(&dbg.n_pages);
        assert_may_sleep("x_free_page");
        free_page((unsigned long)p);
}


/************************************************************************/
/*	kernel page allocations						*/
/************************************************************************/

#ifdef GFP_DMA32
#define COMPAT_GFP_LOW32        GFP_DMA32       /* x86-64 */
#else
#define COMPAT_GFP_LOW32        GFP_DMA         /* x86-64 */
#endif

char *
alloc_mpage32(mpage_t *mpage)
{
        struct page *p = alloc_page(GFP_USER | COMPAT_GFP_LOW32 | __GFP_ZERO);
        if (!p)
                return NULL;

        if (page_to_phys(p) & ~0xffffffffULL) {
                __free_page(p);
                return NULL;
        }
        assert_may_sleep("alloc_mpage32");
        mpage->page = p;
        mpage->p = kmap(p);
        dbg_inc(&dbg.n_mpages);

        return mpage->p;
}

char *
alloc_mpage(mpage_t *mpage)
{
        struct page *p = alloc_page(GFP_USER | __GFP_ZERO);
        if (!p)
                return NULL;

        assert_may_sleep("alloc_mpage");
        mpage->page = p;
        mpage->p = kmap(p);
        dbg_inc(&dbg.n_mpages);

        return mpage->p;
}

void
free_mpage(mpage_t *mpage)
{
        assert_may_sleep("free_mpage");
        if (mpage->p)
                kunmap(mpage->page);
        if (mpage->page) {
                __free_page(mpage->page);
                dbg_dec(&dbg.n_mpages);
        }
        mpage->page = NULL;
        mpage->p = NULL;
}

u64
mpage_phys(mpage_t *mpage)
{
        wrap_BUG_ON(!mpage->page);
        return page_to_phys(mpage->page);
}

void
mpage_kunmap(mpage_t *mpage)
{
        assert_may_sleep("mpage_kunmap");
        kunmap(mpage->page);
        mpage->p = NULL;
}

/************************************************************************/
/*        user page access                                              */
/************************************************************************/

#define PAGE_PHYS_SPECIAL_UPAGE         1

static int
do_get_special_upage(upage_t *up, unsigned long addr)
{
        pgd_t pgd = *pgd_offset(current->mm, addr);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,12,0)
        p4d_t p4d;
#endif
        pud_t pud;
        pmd_t pmd;
        pte_t pte;
        unsigned long pfn;

        if (pgd_none(pgd))
                return 1;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,12,0)
        p4d = *p4d_offset(&pgd, addr);
        if (p4d_none(p4d))
                return 1;
        pud = *pud_offset(&p4d, addr);
#else
        pud = *pud_offset(&pgd, addr);
#endif
        if (pud_none(pud))
                return 1;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6,9,0)
        if (unlikely(pud_leaf(pud)))
#else
        if (unlikely(pud_large(pud)))
#endif
                return 1;
        pmd = *pmd_offset(&pud, addr);
        if (pmd_none(pmd))
                return 1;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6,9,0)
        if (pmd_leaf(pmd))
#else
        if (pmd_large(pmd))
#endif
                return 1;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(6,5,0)  \
     || RHEL_9_GREATER_EQUAL(380))
        pte = *pte_offset_kernel(&pmd, addr);
#else
        pte = *pte_offset_map(&pmd, addr);
#endif
        if (pte_none(pte))
                return 1;
        pfn = pte_pfn(pte);
        /* We require the "special" bit to be set, i.e. the PTE does
           not have a corresponding struct page. */
        if (!pte_special(pte))
                return 1;
        /* Ensure that this is not an uncached mapping. */
        if (pte.pte & (_PAGE_PWT | _PAGE_PCD | _PAGE_PAT))
                return 1;
        /* The page must be writable and user accessible. */
        if (!(pte.pte & _PAGE_RW) || !(pte.pte & _PAGE_USER))
                return 1;
        up->page_phys = (pfn << 12) | PAGE_PHYS_SPECIAL_UPAGE;
        up->p = NULL;
        return 0;
}

static int
get_special_upage(unsigned long addr, upage_t *up)
{
        unsigned long flags;
        int ret;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5,18,0)  \
     || RHEL_9_GREATER_EQUAL(179))
        if (!access_ok((void __user *)addr, 0x1000))
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(5,0,0)
        if (!access_ok(addr, 0x1000))
#elif (LINUX_VERSION_CODE == KERNEL_VERSION(4,18,0)) && defined(RHEL_RELEASE_CODE) && (RHEL_RELEASE_CODE >= 0x808)
        /* Rocky Linux 8.8 has access_ok() backported from 5.0 with fewer arguments */
        if (!access_ok(addr, 0x1000))
#else
        if (!access_ok(VERIFY_WRITE, addr, 0x1000))
#endif
                return -EFAULT;
        local_irq_save(flags);
        ret = do_get_special_upage(up, addr);
        local_irq_restore(flags);
        return ret;
}

static inline int
is_special_upage(const upage_t *up)
{
        return (up->page_phys & PAGE_PHYS_SPECIAL_UPAGE);
}

int
get_upage(uintptr_t va, upage_t *up)
{
        struct page *page;
        int cnt;

        assert_may_sleep("get_upage");

        va &= ~0xfff;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,8,0)
        down_read(&current->mm->mmap_lock);
#else
        down_read(&current->mm->mmap_sem);
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6,5,0)
        cnt = get_user_pages(va, 1 /* #pages */,
                             FOLL_WRITE | FOLL_FORCE /* gup_flags */,
                             &page);
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(4,9,0)
        cnt = get_user_pages(va, 1 /* #pages */,
                             FOLL_WRITE | FOLL_FORCE /* gup_flags */,
                             &page, NULL /* vmas */);
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(4,6,0)
        cnt = get_user_pages(va, 1 /* #pages */,
                             1 /* write */, 1 /* force */,
                             &page, NULL /* vmas */);
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,168)) \
   && (LINUX_VERSION_CODE < KERNEL_VERSION(4,5,0))
        cnt = get_user_pages(current, current->mm, va, 1 /* #pages */,
                             FOLL_WRITE | FOLL_FORCE /* gup_flags */,
                             &page, NULL /* vmas */);
#else
        cnt = get_user_pages(current, current->mm, va, 1 /* #pages */,
                             1 /* write */, 1 /* force */,
                             &page, NULL /* vmas */);
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,8,0)
        up_read(&current->mm->mmap_lock);
#else
        up_read(&current->mm->mmap_sem);
#endif

        if (cnt == 1) {
                up->page = page;
                /* The page start out as unmapped. */
                up->p = NULL;
        } else if (get_special_upage(va, up) != 0) {
                up->page = NULL;
                up->p = NULL;
                return 1;
        }

        dbg_inc(&dbg.n_upages);
        return 0;
}

void
put_upage(upage_t *up)
{
        if (!up)
                return;
        if (!up->page)
                return;

        if (is_special_upage(up)) {
                unmap_upage(up);
        } else {
                set_page_dirty_lock(up->page);
                unmap_upage(up);
                put_page(up->page);
        }
        up->page = NULL;
        dbg_dec(&dbg.n_upages);
}

u64
upage_phys(upage_t *upage)
{
        if (is_special_upage(upage))
                return upage->page_phys & ~0xfff;
        else
                return page_to_phys(upage->page);
}

char *
map_upage(upage_t *upage)
{
        assert_may_sleep("map_upage");
        if (!upage->p) {
                if (is_special_upage(upage)) {
                        upage->p = ioremap_cache(upage->page_phys & ~0xfff,
                                                 0x1000);
                } else
                        upage->p = kmap(upage->page);
        }
        return upage->p;
}

void
unmap_upage(upage_t *upage)
{
        if (upage->p) {
                if (is_special_upage(upage))
                        iounmap(upage->p);
                else
                        kunmap(upage->page);
                upage->p = NULL;
        }
}
