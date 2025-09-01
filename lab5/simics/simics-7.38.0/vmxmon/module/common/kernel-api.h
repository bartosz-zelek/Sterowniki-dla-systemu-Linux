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

#ifndef HOST_API_H
#define HOST_API_H

#include "ktypes.h"

#if defined(__cplusplus)
extern "C" {
#endif

/* error numbers shouldn't change between kernel versions */
#define ENOMEM                  12
#define EFAULT                  14
#define ENODEV                  19
#define EINVAL                  22


#if defined __linux__
typedef bool mp_bool;
#else
typedef ULONG mp_bool;
#endif

typedef struct {
        mp_bool         enable_xsave;
        mp_bool         enable_ept;
        mp_bool         enable_ug;
        mp_bool         enable_vpid;
        mp_bool         enable_svmcs;
        mp_bool         nmi_optimization;
        mp_bool         vmm_exclusive;
        mp_bool         update_all_regs;
        mp_bool         reload_all_regs;
        mp_bool         use_nmi;
        int             bios_bug_workaround; /* <0: automatic, 0: off, >0: on */
        int             mem_limit_mb;
        int             tsc_shift;
        mp_bool         use_apm1;
} module_parameters_t;

extern module_parameters_t mp;

void wrap_trigger_BUG_ON(const char *filename, int line)
#if defined __GNUC__
     __attribute__ ((noreturn))
#endif
;

#define wrap_BUG_ON(x) do {                                  \
        if (unlikely(x))                                     \
                wrap_trigger_BUG_ON(__FILE__, __LINE__);     \
} while(0)

/* malloc/free */
void *x_malloc(int size);
void x_free(void *p);

/* string functions */
void *wrap_memset(void *s, int c, int n);
void *wrap_memcpy(void *d, const void *s, int n);
int wrap_strcmp(const char *cs, const char *ct);

/* wrap printk */
void _wrap_printk(const char *fmt, ...)
#if defined __GNUC__
     __attribute__ ((format (printf, 1, 2)))
#endif
;
#define printm(format, ...)  wrap_printk(format, ## __VA_ARGS__)
#define wrap_printk(fmt, ...) _wrap_printk("VMXMON: " fmt, ## __VA_ARGS__)

/* SMP related macros and functions */
int wrap_get_cpu_num(void);
int wrap_NR_CPUS(void);

int wrap_get_cpu(unsigned int *ipl) __acquires(lock);
void wrap_put_cpu(unsigned int ipl) __releases(lock);

int run_on_each_cpu(void (*func)(void *info, int cpu),
                           void *info);
int run_on_cpu(int cpu, void (*func)(void *info, int cpu),
                      void *info);
void ping_cpu(int cpu);

/* mutexes */

typedef struct mutex mutex_t;

#define INIT_LEVEL      0
#define VM_LEVEL        1
#define PSPACE_LEVEL    2

mutex_t *wrap_alloc_mutex(void);
void wrap_free_mutex(mutex_t *mu);
void wrap_mutex_unlock(mutex_t *mu) __releases(mu);
void wrap_mutex_lock(mutex_t *mu) __acquires(mu);
void wrap_mutex_lock_nested(mutex_t *mu, int depth) __acquires(mu);

/* APIC functions */

void *wrap_apic_map(void);
void wrap_apic_unmap(void *va);
u32 wrap_apic_read(void *va, u64 reg);
void wrap_apic_write(void *va, u64 reg, u32 v);

/* scheduling functions */

int wrap_need_resched(s32 elapsed);
void wrap_schedule(void);
int wrap_signal_pending(void);

/* segment access functions */

void wrap_loadsegment_ds(u16 value);
void wrap_loadsegment_es(u16 value);
void wrap_loadsegment_fs(u16 value);
void wrap_loadsegment_gs(u16 value);
#ifdef __linux__
void wrap_loadsegment_gs_index(u16 value);
int wrap_invalidate_tss_limit(void);
#else
static inline int wrap_invalidate_tss_limit(void) { return 0; }
#endif        

/* control registers */

uintptr_t wrap_cr4_set_bits(uintptr_t mask);
void wrap_cr4_clear_bits(uintptr_t old_cr4, uintptr_t mask);

/* interrupt functions */

void wrap_local_irq_save(unsigned long *irq_flags) __acquires(cpu);
void wrap_local_irq_restore(unsigned long irq_flags) __releases(cpu);

/* FPU functions */

typedef struct host_fpu_state host_fpu_state_t;
host_fpu_state_t *wrap_fpu_alloc_state(void);
void wrap_fpu_free_state(host_fpu_state_t *state);
void wrap_fpu_obtain(host_fpu_state_t *state, u64 mask);
void wrap_fpu_release(host_fpu_state_t *state, u64 mask);
u64 wrap_get_os_enabled_xsave_mask(void);
int wrap_fxrstor64(void *ptr);
int wrap_xrstor64(void *ptr, u32 eax, u32 edx);

/* ioremap (for firewire debugging) */
void *wrap_ioremap_page(u64 pa);
void wrap_iounmap_page(void *p);

/* performance event */
struct perf_event;
struct perf_event *create_cpu_perf_event(int cpu);
void release_cpu_perf_event(struct perf_event *ev);
        
/* NMI handling */
typedef struct wrap_nmi_handle wrap_nmi_handle_t;
wrap_nmi_handle_t *wrap_register_nmi(void);
void wrap_unregister_nmi(wrap_nmi_handle_t *h);

/* model specific registers */
int wrap_rdmsr_safe(u32 msr, u64 *v);

fdesc_t wrap_get_process_id(void);

#if defined(__cplusplus)
}
#endif

#endif  /* HOST_API_H */
