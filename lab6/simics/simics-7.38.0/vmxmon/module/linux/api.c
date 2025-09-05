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

#include "api.h"
#include "vmxioctl.h"
#include "kernel-api.h"

#include <linux/slab.h>
#include <linux/types.h>
#include <linux/spinlock.h>
#include <linux/mm.h>
#include <linux/highmem.h>
#include <linux/version.h>
#include <linux/smp.h>
#include <linux/moduleparam.h>
#include <linux/mutex.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 11, 0)
#include <linux/sched/signal.h>
#endif
#include <asm/io.h>
#include <asm/apic.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 2, 0)
#include <asm/fpu/api.h>
#else
#include <asm/i387.h>
#endif
#include <asm/nmi.h>
#include <asm/uaccess.h>
#include <asm/msr.h>
#include <asm/processor.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 0, 0)
#include <asm/tlbflush.h>
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 11, 0)
#include <asm/desc.h>
#endif
#if ((LINUX_VERSION_CODE >= KERNEL_VERSION(6, 3, 0))  \
     || RHEL_9_GREATER_EQUAL(434))
#include <asm/gsseg.h>
#endif


#define MSR_IA32_APIC_BASE      0x1b

module_param_named(enable_ept, mp.enable_ept, bool, 0664);
module_param_named(enable_ug, mp.enable_ug, bool, 0664);
module_param_named(enable_vpid, mp.enable_vpid, bool, 0664);
module_param_named(enable_svmcs, mp.enable_svmcs, bool, 0664);
module_param_named(enable_xsave, mp.enable_xsave, bool, 0664);
module_param_named(vmm_exclusive, mp.vmm_exclusive, bool, 0664);
module_param_named(update_all_regs, mp.update_all_regs, bool, 0664);
module_param_named(reload_all_regs, mp.reload_all_regs, bool, 0664);
module_param_named(use_nmi, mp.use_nmi, bool, 0664);
module_param_named(nmi_optimization, mp.nmi_optimization, bool, 0664);
module_param_named(use_apm1, mp.use_apm1, bool, 0664);

module_param_named(bios_bug_workaround, mp.bios_bug_workaround, int, 0664);
module_param_named(mem_limit_mb, mp.mem_limit_mb, int, 0664);
module_param_named(tsc_shift, mp.tsc_shift, int, 0664);

#ifdef CONFIG_X86_X2APIC
static struct {
        bool x2apic_mode;
} ss;
#endif

void
wrap_trigger_BUG_ON(const char *filename, int line)
{
	printk("[vmxmon] BUG on %s:%d\n", filename, line);
        BUG_ON(1);
}

void *
wrap_memset(void *s, int c, int n)
{
        return memset(s, c, n);
}

void *
wrap_memcpy(void *d, const void *s, int n)
{
        return memcpy(d, s, n);
}

int
wrap_strcmp(const char *cs, const char *ct)
{
        return strcmp(cs, ct);
}

void
_wrap_printk(const char *fmt, ...)
{
        va_list va;
        va_start(va, fmt);
        vprintk(fmt, va);
        va_end(va);
}

uintptr_t
wrap_cr4_set_bits(uintptr_t mask)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 0, 0)
        cr4_set_bits(mask);
        return cr4_read_shadow();
#else
        unsigned long v;
        __asm__ volatile("mov %%cr4,%0" : "=r" (v) :: "cc");
        v |= mask;
        __asm__ volatile("mov %0,%%cr4" :: "r" (v) : "cc");
        return v;
#endif
}

void
wrap_cr4_clear_bits(uintptr_t old_cr4, uintptr_t mask)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 0, 0)
        cr4_clear_bits(mask);
#else
        old_cr4 &= ~mask;
        __asm__ volatile("mov %0,%%cr4" :: "r" (old_cr4) : "cc");
#endif
}

int
wrap_get_cpu_num(void)
{
        return smp_processor_id();
}

int
wrap_NR_CPUS(void)
{
        return NR_CPUS;
}

int
wrap_get_cpu(unsigned int *ipl)
{
        return get_cpu();
}

void
wrap_put_cpu(unsigned int ipl)
{
        put_cpu();
}

typedef struct {
        void    (*func)(void *, int);
        void            *info;
} callback_t;

static void
do_callback(void *info)
{
        callback_t *q = info;
        (*q->func)(q->info, smp_processor_id());
}

/* run on all CPUs (including self) */
int
run_on_each_cpu(void (*func)(void *, int), void *info)
{
        callback_t q = { .func = func, .info = info, };
#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 3, 0)
        return on_each_cpu(do_callback, &q, 1);
#else
        on_each_cpu(do_callback, &q, 1);
        return 0;
#endif
}

static void
ping_callback(void *info)
{
        /* nothing */
}

void
ping_cpu(int cpu)
{
        smp_call_function_single(cpu, ping_callback, NULL, 0);
}

int
run_on_cpu(int cpu, void (*func)(void *, int), void *info)
{
        callback_t q = { .func = func, .info = info, };
        return smp_call_function_single(cpu, do_callback, &q, 1);
}

mutex_t *
wrap_alloc_mutex(void)
{
        mutex_t *mu = kmalloc(sizeof *mu, GFP_KERNEL);
        if (!mu)
                return NULL;
        memset(mu, 0, sizeof *mu);
        mutex_init(mu);
        return mu;
}

void
wrap_free_mutex(mutex_t *mu)
{
        if (mu) {
                mutex_destroy(mu);
                kfree(mu);
        }
}

void
wrap_mutex_unlock(mutex_t *mu)
{
        mutex_unlock(mu);
}

void
wrap_mutex_lock(mutex_t *mu)
{
        mutex_lock(mu);
}

void
wrap_mutex_lock_nested(mutex_t *mu, int depth)
{
        mutex_lock_nested(mu, depth);
}

#define xx_apic_read(a)      native_apic_mem_read(a)
#define xx_apic_write(a,b)   native_apic_mem_write(a, b)

void *
wrap_apic_map(void)
{
        return (void *)-1ULL;
}

void
wrap_apic_unmap(void *va)
{
}

u32
wrap_apic_read(void *va, u64 reg)
{
#ifdef CONFIG_X86_X2APIC
        if (ss.x2apic_mode)
                return native_apic_msr_read(reg);
#endif
        return xx_apic_read(reg);
}

void
wrap_apic_write(void *va, u64 reg, u32 v)
{
#ifdef CONFIG_X86_X2APIC
        if (ss.x2apic_mode) {
                native_apic_msr_write(reg, v);
                return;
        }
#endif
        xx_apic_write(reg, v);
}

int
wrap_need_resched(s32 dummy_elapsed)
{
        return need_resched();
}

void
wrap_schedule(void)
{
        schedule();
}

void
wrap_loadsegment_ds(u16 value)
{
        loadsegment(ds, value);
}

void
wrap_loadsegment_es(u16 value)
{
        loadsegment(es, value);
}

void
wrap_loadsegment_fs(u16 value)
{
        loadsegment(fs, value);
}

void
wrap_loadsegment_gs_index(u16 value)
{
        load_gs_index(value);
}

void
wrap_local_irq_save(unsigned long *irq_flags)
{
        local_irq_save(*irq_flags);
}

void
wrap_local_irq_restore(unsigned long irq_flags)
{
        local_irq_restore(irq_flags);
}

host_fpu_state_t *
wrap_fpu_alloc_state(void)
{
        /* Fake allocation */
        return (void *)1;
}

void
wrap_fpu_free_state(host_fpu_state_t *state)
{
}

int
wrap_signal_pending(void)
{
        return signal_pending(current);
}

void *
wrap_ioremap_page(u64 pa)
{
        return ioremap(pa, 0x1000);
}

void
wrap_iounmap_page(void *p)
{
        iounmap(p);
}

void
wrap_fpu_obtain(host_fpu_state_t *state, u64 unused)
{
        kernel_fpu_begin();
}

void
wrap_fpu_release(host_fpu_state_t *state, u64 unused)
{
        kernel_fpu_end();
}

u64
wrap_get_os_enabled_xsave_mask(void)
{
        /* All the state is allowed even if kernel does not save it */
        return 0xffffffffffffffffull;
}

int
wrap_rdmsr_safe(u32 msr, u64 *v)
{
        return rdmsrl_safe(msr, v);
}

int
wrap_fxrstor64(void *ptr)
{
        int err = 0;
        __asm__ volatile(
                "1: fxrstor64 %2\n"
                "2:\n"
                ".section .fixup,\"ax\"\n"
                "3: movl $-1,%[err]\n"
                "jmp 2b\n"
                ".previous\n"
                _ASM_EXTABLE(1b, 3b)
                : [err] "=r" (err)
                : "0" (0), "m" (*(const char *)ptr));
        return err;
}

int
wrap_xrstor64(void *ptr, u32 eax, u32 edx)
{
        int err = 0;
        __asm__ volatile(
                "1: xrstor64 %2 \n"
                "2:\n"
                ".section .fixup,\"ax\"\n"
                "3: movl $-1,%[err]\n"
                "jmp 2b\n"
                ".previous\n"
                _ASM_EXTABLE(1b, 3b)
                : [err] "=r" (err)
                : "0" (0), "m" (*(const char *)ptr),
                  "a" (eax), "d" (edx));
        return err;
}

fdesc_t
wrap_get_process_id(void)
{
        return current->pid;
}

int
wrap_invalidate_tss_limit(void)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 11, 0)
        invalidate_tss_limit();
        return 1;
#else
        return 0;
#endif
}

int __init
init_api(void)
{
        u64 apic_base;
        rdmsrl(MSR_IA32_APIC_BASE, apic_base);
        if (apic_base & (1 << 10)) {
#ifdef CONFIG_X86_X2APIC
                printm("X2APIC mode.\n");
                ss.x2apic_mode = true;
#else
                printm("X2APIC mode enabled,"
                       " but no CONFIG_X86_X2APIC support.\n");
                return -ENODEV;
#endif
        }
        return 0;
}
