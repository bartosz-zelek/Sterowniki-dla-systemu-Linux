/* -*- coding: utf-8 -*-
 *
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

#ifndef VM_H
#define VM_H

#include "kernel-page.h"
#include "asm-inlines.h"
#include "vmxmon.h"
#include "vmx.h"
#include "perfctr.h"
#include "vm-exit-codes.h"

/* early (no VMCS) */
#define EW_CR3_DIRTY              (1 << 0)      /* x->cr3 changed */

/* once-only, VMCS available */
#define EW_TPR_LIMIT_USAGE_SWITCH (1 << 4)
#define EW_EPT_USAGE_SWITCH       (1 << 5)
#define EW_RELOAD_PROC            (1 << 6)
#define EW_RELOAD_PROC2           (1 << 7)
#define EW_CR3_RELOAD             (1 << 8)      /* reload VMCS */
#define EW_RELOAD_CR4             (1 << 9)      /* reload cr4 */
#define EW_RELOAD_DIRTY_CR4       (1 << 10)     /* reload cr4 from x->cr4 */
#define EW_RELOAD_VMENTRY_CTRLS   (1 << 11)
#define EW_RELOAD_APIC_PAGE       (1 << 12)     /* Reload virtual apic page */
#define EW_UPDATE_EPT_PTR         (1 << 13)
#define EW_ONCE_ONLY_MASK         0xffff        /* bits 0-15 */

/* per-entry, VMCS available */
#define EW_FLUSH_VPID             (1 << 16)
#define EW_FLUSH_EPT              (1 << 17)
#define EW_INJECT                 (1 << 18)     /* state: injected exception */
#define EW_RELOAD_CR0             (1 << 20)     /* reload cr0 */
#define EW_RELOAD_DIRTY_CR0       (1 << 21)     /* reload cr0 from x->cr0 */
#define EW_RELOAD_VMEXIT_CTRLS    (1 << 22)

/* indexed MSRs */
#define I_MSR_STEP_CTR          0               /* used by pctr */
#define I_MSR_EFER              1
#define I_MSR_KERNEL_GS_BASE    2
#define I_MSR_LSTAR             3
#define I_MSR_STAR              4
#define I_MSR_FMASK             5
#define I_MSR_CR_PAT            6
#define I_MSR_PERF_GLOBAL_CTRL  7               /* used with apm2 */
#define I_MSR_PMC_CTRL          8
#define I_MSR_PMC_CTRL2         9
#define I_MSR_TSC_AUX           10
#define I_MSR_TSC               11
#define NUM_IMSR                12

/* supported XCR0 bits */
#define XSTATE_SUPPORTED_MASK \
        (XSTATE_LEGACY | XSTATE_AVX | XSTATE_AVX512 | XSTATE_MPX | XSTATE_PKRU)


typedef struct cpu_state cpu_state_t;
typedef struct guest_ept guest_ept_t;

typedef struct {
        u64             mask;                   /* performance counter mask */

        u32             step_msr;               /* msr for step counting */

        u32             ctrl_msr;               /* msr for counter enabling */
        u64             ctrl_val;               /* value to enable counting */

        u32             ctrl2_msr;              /* msr for counter enabling */
        u64             ctrl2_val;              /* value to enable counting */

        u64             perf_global_ctrl;       /* bit in PERF_GLOBAL_CTRL */
        
        bool            counts_injections;
        bool            counts_vmxenter;

        bool            (*get_and_clear_overflow)(const cpu_state_t *cs);
} pctr_t;


/* CPU state shared by multiple vm's */
struct cpu_state {
        int             cpu;                    /* cpu index */
        u8              thread;                 /* thread# (hyperthreading) */
        
        bool            vmx_is_on;              /* set in VMXON state */
        bool            our_vmxon;              /* we did the VMXON */
        bool            org_cr4_vmxe;           /* orig. val of CR4[VMXE] */

        bool            smm_freeze_disabled;    /* SMM_FREEZE disabled */
        bool            use_smm_perf_freeze;    /* SMM_FREEZE available */

        u8              apic_error_vec;         /* spurious interrupt vect */

        bool            claim_nmi;              /* claim NMI on CPU */

        bool            use_apm1;
        pctr_t          pmc;                    /* pctr info */

        mpage_t         vmxon_page;
        void            *lapic_base;

        struct perf_event *perf_event;          /* performance counter event */

        /* Virtual machine currently running on this CPU. Use for debugging. */
        struct virtual_machine *vm;
};

typedef struct {
        u32             dummy;                  /* win asm-offset workaround */
        descriptor_t    gdt_desc;               /* base, limit */
        descriptor_t    idt_desc;               /* base, limit */

        u64             rbp;
        u64             rbx;
        u64             rdi;
        u64             rsi;
        u64             r10;
        u64             r11;
        u64             r12;
        u64             r13;
        u64             r14;
        u64             r15;

        uintptr_t       entry_failure;          /* set if VMRESUME returns */
        u16             tss_sel;                /* TSS segment selector */

        u64             xcr0;
        u64             ia32_bndcfgs;
        u32             pkru;

        uintptr_t       cr4;
} host_state_t;

typedef struct virtual_machine  vm_t;
typedef struct tphys_space      tphys_space_t;

typedef struct {
        int             (*page_fault)(vm_t *vm, u8 ecode, u64 va);
} mmu_ops_t;

#define MAX_AUX_ENTRIES                 (0x1000 / sizeof(u64))

/* virtual machine state */
struct virtual_machine {
        vm_t            *next;             /* linked list, protected by
                                              [global] initmutex */
        mutex_t         *vm_lock;          /* mutex protecting this vm */

        int             session_id;        /* unique identifier of this VM */
        fdesc_t         process_id;
        u32             entry_block;       /* block vmx_enter if != 0 */

        /* userspace communication */
        upage_t         com_page;          /* user communication page */
        upage_t         aux_page;          /* user page for aux info */
        vmxmon_t        *com;
        u64             *aux_table;        /* used to pass userspace arrays */

        u32             entry_work;        /* EW_xxx bitmask */
        u32             dirty;             /* copy of x->dirty */

        /* cached guest registers */
        u64             guest_xcr0;        /* [safe] copy of x->xcr0 */
        
        /* MMU */
        u64             guest_cr3;         /* used to detect cr3 changes */
        u64             guest_pdpte[4];    /* used to detect pdpte changes */
        unsigned        mmu_mode;          /* MMU_MODE_xxx | MMU_MODE_QUAL_xx */
        mmu_ops_t       mmu_ops;           /* mode specific MMU operations */
        struct tlb      *tlb;              /* page translation realization */
        u16             vpid;              /* vpid we use */
        u8              maxphyaddr;        /* copy of x->maxphyaddr */

        /* flags */
        unsigned        flags;             /* All flags passed during setup, or
                                              undefined_flags */
        bool            ia32e_guest;       /* guest is IA32E, set by setup */
        bool            use_5_level_pages;
        bool            non_root_guest;    /* guest in VMX non-root mode */
        bool            use_nested_ept;    /* use the nested guest EPT */
        bool            use_mbe;           /* use mode-based execution */

        bool            use_tpr_limit;     /* TPR limit mechanism in use */
        bool            use_ept;           /* EPT in use */
        bool            use_xsave;         /* Instructions that uses state
                                              managed by XSAVE are in use */
        bool            use_ug;            /* unrestricted guest in use */
        bool            use_vpid;          /* VPID in use */
        bool            use_lazy_fpu;      /* use lazy FPU save/restore */
        bool            use_direct_rdtsc;  /* direct rdtsc execution enabled */
        u8              tsc_shift;
        bool            use_svmcs;         /* shadow VMCS can be used to
                                              serve guest accesses */
        bool            use_perf_counters; /* use performance counters */
        bool            vmcs_pinned;       /* VMCS can be accessed */
        bool            always_do_vmxoff;
        bool            fpu_owned;         /* FPU is enabled */
        bool            tss_type_optimization; /* optimization for 16 bit TSS
                                                  in IA32e-mode */
        bool            monitor_trap_flag; /* single-stepping */
        bool            large_xsave_area;  /* XSAVE area is page sized */
        bool            fpu_disabled;      /* run with FPU off, e.g. to hide
                                              instruction set differences */
        bool            use_bndcfgs;       /* load/restore IA32_BNDCFGS */
        bool            use_pku;           /* load/restore PKRU and enable
                                              CR4.OSPKE */
        bool            pkru_owned;        /* Guest PKRU is loaded */
        
        /* guest physical address space */
        tphys_space_t   *pspace;           /* target physical address space */
        vm_t            *tphys_next_vm;    /* next vm sharing pspace */
        struct nested_epts *nested_epts;   /* EPTs for nested virt. */
        guest_ept_t     *cur_guest_ept;    /* active guest EPT */

        /* host state */
        host_state_t    host_state;        /* CPU state */
        u64             host_tsc_aux;      /* host IA32_TSC_AUX value */
        u64             host_ia32_star;    /* host IA32_STAR value */
        u64             host_ia32_fmask;   /* host IA32_FMASK value */

        /* VMCS */
        int             cpu;               /* CPU# with up-to-date state */
        cpu_state_t     *cs;               /* for easy reference */
        mpage_t         vmcs;              /* VMCS page */
        u64             vmcs_phys;         /* VMCS page phys */
        u32             last_vmcs_vm_instr_err;
        bool            launched;          /* VMCS has been launched */
        bool            initial_vmclear_done; /* VMCS has been cleared */
        bool            may_read_efer;     /* sim and guest efer matches */

        mpage_t         virt_apic_page;    /* virtual apic page */
        mpage_t         msr_bitmap_page;   /* MSR bitmaps */

        /* MSR handling.
           Below we describe the layout of msr_page. The page should not
           be accessed directly though: accessor functions should be used. */
        /* indices of entries on VMCS MSR page (msr_page) */
        #define FIRST_GMSR_SLOT         0x80
        #define FIRST_HMSR_SLOT         0x0
        #define UNUSED_MSR_SLOT         0xff

        mpage_t         msr_page;
        struct msr_entry *g_msr[NUM_IMSR];  /* guest MSR on VMCS page pointer */
        struct msr_entry *h_msr[NUM_IMSR];  /* host MSR on VMCS page pointer */
        u32             imsr2msr[NUM_IMSR]; /* iMSR -> MSR table */
        u8              hslot2imsr[NUM_IMSR]; /* host MSR slot -> iMSR */
        u8              n_g_msrs_l;        /* #guest MSRS loaded at vmentry */
        u8              n_g_msrs_so;      /* #guest MSRS store at vmexit only */
        /* #guest MSRS loaded at vmentry and stored at vmexit */
        u8              n_g_msrs_ls;
        u8              n_h_msrs;          /* #host MSRS loaded at vmexit */

        u32             cr0_host_mask;     /* VMCS_CR0_GUEST_HOST_MASK */
        u32             cr4_host_mask;     /* VMCS_CR4_GUEST_HOST_MASK */

        /* cached MSR bitmap state */
        u32             msr_intercepts_r;  /* MSR_INTERCEPT_xx */
        u32             msr_intercepts_w;

        /* privileged register contents */
        u64             shadow_page_table_pa;
        struct host_fpu_state *fpu_state;
        unsigned        vmret;             /* userspace return code */

        /* lazy FPU backoff mechanism */
        unsigned        fpu_recheck_cnt;   /* decreased at each entry */
        unsigned        fpu_used_cnt;      /* increased at each #FPU exit */

        /* VMX exit state */
        u32             vmexit_reason;
        u32             vmexit_int_info;

        /* cached VMCS registers */
        u32             proc_ctrls;        /* VMCS_PROC_BASED_VM_EXEC_CTRLS */
        u32             proc2_ctrls;       /* VMCS_SEC_PROC_BASED */

        /* shadow VMCS  */
        mpage_t         linked_vmcs;       /* Linked VMCS page */
        u64             linked_vmcs_phys;  /* Linked VMCS page phys */

        /* userspace VMCS communication table */
        upage_t         guest_vmcs;
        vmx_vmcs_field_t *guest_vmcs_table;
        u64             guest_eptp;        /* used for "nested EPT" only -
                                              not always in sync! */

        mpage_t         vmread_bitmap;     /* VMREAD bitmap page */
        u64             vmread_bitmap_phys;
        mpage_t         vmwrite_bitmap;    /* VMWRITE bitmap page */
        u64             vmwrite_bitmap_phys;
        bool            vmwrite_happened;

        /* xsave area */
        upage_t         xsave_page;        /* user page for xsave area */
        vmxmon_xsave_area_t *xsave_area;

        /* misc */
        s32             steps;             /* #steps we try to run */
        s32             elapsed;           /* #steps actually run */

        /* initialization */
        bool            vmx_initialized;   /* vmx_init called successfully */

        /* debug */
        vmx_trace_t     *vmx_trace;        /* allocated on demand */
        struct vmx_dbg  *vmx_dbg;          /* extra debug info */

        /* instrumentation */
        u64             *vmexit_stats;
        u64             *vmret_stats;
        u64             *perfctr_stats;
};

/* entry block definitions */
#define ENTRY_BLOCK_USER_MASK           0xffff
#define ENTRY_BLOCK_SUSPENDED          0x10000


/* global information (shared by all instances) */
typedef struct global {
        cpu_state_t     **cpu_state;            /* per CPU state (shared) */
        struct wrap_nmi_handle *nmi_handle;
        
        /* Information about the host and the os. */
        bool            has_efer_support;       /* VMX can save/load EFER */
        bool            has_bndcfgs_support;    /* VMX can load/clear BNDCFGS */
        bool            has_proc2;              /* has secondary controls */
        bool            has_vpid;               /* VPID support */
        bool            has_ept;                /* EPT support */
        bool            has_ept_execute_only;   /* Execute only supported */
        bool            has_ept_mbe;            /* Mode-based execution */
        bool            has_desc_table_exiting; /* Descriptor-table exiting */
        bool            has_avx;                /* Intel® AVX support */
        bool            has_xsave;              /* XSAVE support */
        bool            osxsave_enabled;        /* OS set OSXSAVE */
        bool            has_lzcnt;              /* LZCNT support */
        bool            has_bmi1;               /* BMI1 (including TZCNT) */
        bool            has_fpu_csds_deprecation; /* FPU CS/DS deprecation */
        bool            has_svmcs;              /* Shadow VMCS */
        bool            has_fsgsbase;           /* FSGSBASE support */
        bool            has_pku;                /* PKU support */
        bool            ospke_enabled;          /* OS set OSPKE */
        bool            has_unrestricted_guest; /* unrestricted guest support */
        bool            has_virtual_nmi;        /* NMI window exiting */
        bool            has_msr_bitmap;         /* MSR bitmap support */
        bool            has_vmcs_pctr_handling; /* IA32_PERF_GLOBAL_CTRL */
        bool            has_monitor_trap_flag;  /* single-stepping support */
        bool            has_1gb_pages;          /* supports 1GB pages */
        bool            has_1gb_pages_cpu_bug;   /* has 1gb-related erratum */
        bool            has_tsc_scaling;
        bool            has_rdtscp;
        bool            has_ins_outs_reporting; /* ins/outs inst info */
        bool            has_pebs;               /* PEBS monitoring facility */
        bool            pat_handling_needed;    /* PAT0 != 0x6 */
        bool            use_vmcs_pat_handling;  /* use VMCS PAT save/load */
        bool            cr3_store_always_exits;
        bool            cr3_load_always_exits;

        bool            has_16_bit_tss_bios_bug; /* has 16 bit TSS BIOS bug */
        bool            has_5_level_paging;

        u32             dbgctl_smm_perf_freeze; /* FREEZE_WHILE_SMM bit or 0 */

        int             apm_version;            /* performance counters */
        int             host_max_physaddr;

        u64             os_enabled_xsave_mask;  /* used only for Windows host */
        u64             host_xcr0_mask;         /* host XCR0 supported mask */
        u64             xsave_bndcsr_offs;      /* XSAVE.BNDCSR offset */

        int             vmx_rev;
        int             vmx_mem_type;      /* VMX_MEM_TYPE_xxx */

        u32             pin_based_0_allowed;    /* (0_allowed, 1_allowed): */
        u32             pin_based_1_allowed;    /*    (0,0) = fixed-0 */
        u32             proc_based_0_allowed;   /*    (0,1) = flexible */
        u32             proc_based_1_allowed;   /*    (1,1) = fixed-1 */
        u32             proc_based2_0_allowed;  /* available on newer revs */
        u32             proc_based2_1_allowed;
        u32             vmexit_0_allowed;
        u32             vmexit_1_allowed;
        u32             vmentry_0_allowed;
        u32             vmentry_1_allowed;
        u32             cr0_fixed_0;
        u32             cr0_fixed_1;
        u32             cr4_fixed_0;
        u32             cr4_fixed_1;

        u64             ept_vpid_cap;      /* VMX_EPT_VPID_CAP MSR */
        u64             vm_functions;      /* VMX_VMFUNC MSR */

        bool            is_hyperv;
} global_t;


extern global_t gi;

extern vm_t     *vm_create(void);
extern void     vm_destroy(vm_t *vm);
extern int      vm_request(vm_t *vm, vmxmon_req_t *req);
extern void     vm_suspend(vm_t *vm);
extern void     vm_resume(vm_t *vm);

extern int      share_pspace(vm_t *vm, int session_id);
extern int      dbg_get_cpu_with_vmcs(void);

/* VMX operations */
extern int      vmx_mod_init(void);
extern void     vmx_mod_cleanup(void);

extern int      vmx_cpu_up(int cpu);
extern void     vmx_cpu_down(int cpu);

extern int      vmx_init(vm_t *vm);
extern void     vmx_cleanup(vm_t *vm);
extern void     vmx_get_regs(vm_t *vm, u32 regbits);
extern int      vmx_inject_pf(vm_t *vm, u64 va, u16 ecode);

struct vmxdbg_vmcs;
extern int      vmx_dbg_get_vmcs(vm_t *vm, vmxdbg_vmcs_t __user *d);
extern void     vmx_dbg_get_current_vmcs(vmxdbg_vmcs_t *p, int cpu);
extern void     vmx_suspend(vm_t *vm);
extern void     vmx_last_vm_closed(void);
extern void     vmx_all_vms_suspended(void);
extern bool     is_vm_set_up(vm_t *vm);

/* assembly exports */
extern void do_vmlaunch(host_state_t *hs, vmxmon_t *m, bool is_launched);
extern void unblock_nmi(void);

#define TLB_BUMP(ctr)        do { bump_perfctr(tlb->vm, ctr, 1); } while(0)
#define BUMP(ctr)            do { bump_perfctr(vm, ctr, 1); } while(0)
#define BUMP_N(ctr, val)     do { bump_perfctr(vm, ctr, val); } while(0)
#define BUMP_SET(ctr, val)   do { set_perfctr(vm, ctr, val); } while(0)

static inline void
bump_perfctr(vm_t *vm, unsigned perfctr, unsigned n)
{
        vmxmon_t *x = vm->com;
        if (perfctr < sizeof x->vmxmon_stats / sizeof x->vmxmon_stats[0])
                x->vmxmon_stats[perfctr] += n;
        if (perfctr < PERFCTR_COUNT)
                vm->perfctr_stats[perfctr] += n;
}

static inline void
set_perfctr(vm_t *vm, unsigned perfctr, unsigned n)
{
        vmxmon_t *x = vm->com;
        if (perfctr < sizeof x->vmxmon_stats / sizeof x->vmxmon_stats[0])
                x->vmxmon_stats[perfctr] = n;
        if (perfctr < PERFCTR_COUNT)
                vm->perfctr_stats[perfctr] = n;
}

static inline void
bump_vmret(vm_t *vm, unsigned vmret)
{
        vmxmon_t *x = vm->com;
        if (vmret < sizeof x->vmret_stats / sizeof x->vmret_stats[0])
                x->vmret_stats[vmret]++;
        if (vmret < VMRET_COUNT)
                vm->vmret_stats[vmret]++;
}

static inline void
bump_vmexit(vm_t *vm, unsigned vmexit)
{
        vmxmon_t *x = vm->com;
        if (vmexit < sizeof x->vmexit_stats / sizeof x->vmexit_stats[0])
                x->vmexit_stats[vmexit]++;
        if (vmexit < VMEXIT_COUNT)
                vm->vmexit_stats[vmexit]++;
}


static inline void
get_utable_range_and_clear(utable_t *t, unsigned *ret_start, unsigned *ret_end,
                           unsigned max)
{
        *ret_start = t->start;
        unsigned e = t->start + t->n;
        if (e > max)
                e = max;
        *ret_end = e;
        t->n = 0;
}

static inline bool
utable_empty(utable_t *t)
{
        return t->n == 0;
}

static inline void
clear_utable(utable_t *t)
{
        t->n = 0;
}

static inline unsigned
get_vmret(vm_t *vm)
{
        return vm->vmret;
}

static inline void
set_vmret(vm_t *vm, unsigned vmret)
{
        if (vm->vmret != 0) {
                wrap_printk("XXX: vmret value overwritten: %d -> %d\n",
                            vm->vmret, vmret);
                vm->vmret = VMRET_INTERNAL_ERROR;
                return;
        }
        vm->vmret = vmret;
}

static inline void
set_vmret_(vm_t *vm, unsigned vmret)
{
        vm->vmret = vmret;
}

static inline void
clear_vmret(vm_t *vm)
{
        vm->vmret = 0;
}

#endif   /* VM_H */
