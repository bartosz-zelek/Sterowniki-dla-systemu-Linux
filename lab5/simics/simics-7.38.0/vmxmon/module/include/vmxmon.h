/* -*- coding: utf-8 -*-

  Copyright 2015-2022 Intel Corporation

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of Intel Corporation nor the names of its contributors
      may be used to endorse or promote products derived from this software
      without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef VMXMON_H
#define VMXMON_H

#include "vmxioctl.h"
#include "version.h"
#include "vmp-exit-codes.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define VMP_BIT(n) (1ULL << (n))

#define SEG_ES          0
#define SEG_CS          1
#define SEG_SS          2
#define SEG_DS          3
#define SEG_FS          4
#define SEG_GS          5
#define SEG_LDT         6
#define SEG_TR          7
#define NSEGS           8

#define SEGATTR_TYPE_LSB VMP_BIT(0)
#define SEGATTR_S       VMP_BIT(4)  /* desc type, 0 = system */
#define SEGATTR_DPL_LSB VMP_BIT(5)
#define SEGATTR_P       VMP_BIT(7)  /* present */
#define SEGATTR_AVL     VMP_BIT(12) /* avail for system software */
#define SEGATTR_L       VMP_BIT(13) /* 64-bit mode (for CS only) */
#define SEGATTR_DB      VMP_BIT(14) /* 0 = 16 bit, 1 = 32-bit */
#define SEGATTR_G       VMP_BIT(15) /* granularity */
#define SEGATTR_U       VMP_BIT(16) /* unusable (0 = usable) */


#define GPR_AX          0
#define GPR_CX          1
#define GPR_DX          2
#define GPR_BX          3
#define GPR_SP          4
#define GPR_BP          5
#define GPR_SI          6
#define GPR_DI          7

#define INT_TYPE_EXTERNAL               0
#define INT_TYPE_RESERVED               1
#define INT_TYPE_NMI                    2
#define INT_TYPE_HARDWARE_EXC           3
#define INT_TYPE_SOFTWARE_INT           4
#define INT_TYPE_PRIV_SOFTWARE_EXC      5
#define INT_TYPE_SOFTWARE_EXC           6
#define INT_TYPE_OTHER_EVENT            7

#define VMXMON_PROTECT_READ             1
#define VMXMON_PROTECT_WRITE            2
#define VMXMON_PROTECT_EXECUTE          4

typedef struct opaque_page_info         *page_handle_t;

typedef struct {
        u8              valid;
        u8              deliver_ec;             /* even has error code */
        u8              vector;
        u8              type;
        u16             instr_len;
        u16             ecode;
} ev_injection_t;

typedef struct {
        u16             start;                  /* index of first entry */
        u16             max_size;               /* maximal size of table */
        u16             n;                      /* current size of table */
        u16             res;
} utable_t;

typedef struct {
        u8              flush_all_write_mappings; /* flush write mappings */
        u8              flush_all_pages;        /* flush all phys pages */
        u8              res[6];
        u64             res2[6];
        utable_t        wprotect_pages;         /* pages to write protect */

        u64             data[];                 /* fills entire page */
} vmxmon_shared_t;

typedef struct vmx_vmcs_field {
        u64 value;
        u32 index;
        u8 valid;
        u8 dirty; /* true for fields modified inside vmxmon */
        u8 pad[2];
} vmx_vmcs_field_t;

typedef struct vmxmon_xsave_area {
        u8 legacy_area[0x200];

        /* xsave header */
        u64 xstate_bv;
        u64 xstate_comp;
        u64 reserved[6];

        u64 data[]; /* fills entire page */
} vmxmon_xsave_area_t;

typedef struct vmxmon {                         /* must be smaller than 4k */
        /* state synchronization */
        u32             dirty;                  /* modified registers */
        u32             insync;                 /* up to date registers */

        /* generic CPU state */
        u64             ip;
        u64             rflags;
        u64             gprs[16];

        u64             seg_base[NSEGS];        /* segment state */
        u32             seg_limit[NSEGS];
        u32             seg_attr[NSEGS];
        u16             seg_sel[NSEGS];

        u64             gdt_base;
        u64             idt_base;
        u32             gdt_limit;
        u32             idt_limit;

        u64             cr0;
        u64             cr2;
        u64             cr3;
        u64             cr4;
        u8              tpr;                    /* cr8.TPR */
        u8              tpr_threshold;
        u8              pad0[6];
        u64             xcr0;

        u64             dr[4];
        u64             dr6;
        u64             dr7;

        u64             pdpte[4];               /* PDPTE entries (PAE paging) */

        u64             deprecated_ssp;         /* shadow stack pointer */

        u64             cr4_legal_mask;         /* CR4 bits that are
                                                   supported by guest */

        u32             pkru;
        u32             reserved0;
        u64             vlapic_page;      /* Guest virtual APIC page address */
        u64             invept_ent;             /* INVEPT entry */

        /* Fields used for flushing EPT cache due to EPT violation */
        u64             ept_viol_gpa;           /* GPA causing EPT violation */
        u64             ept_viol_eptp;          /* EPTP pointer for violation */
        u32             ept_viol_access;        /* Access bits for violation */
        u32             reserved1;

        /* this block must reside at offset 512 for backward compatibility */
        u32             version;                /* VMXMON_VERSION */
        u32             vmxmon_size;            /* size of vmxmon struct */
        u32             api_version;
        u32             api_compat_version;

        /* vmxmon capabilities */
        u32             vmxmon_capabilities;    /* VMXMON_CAPABILITY_xxx */
        u32             vmxmon_features;        /* VMXMON_FEATURES_xxx */
        u64             xcr0_capabilities;      /* supported XCR0 bits */
        u64             reserved2[2];

        /* vmxmon statistics capabilities */
        u32             vmexit_count;
        u32             vmret_count;
        u32             perfctr_count;
        u32             reserved7;

        /* Intel® FPU/SSE/AVX state; must be 64-byte aligned */
        char            fxsave_area[0x340];     /* first 0x200 correspond to
                                                   legacy region and can be used
                                                   by FXSAVE instruction for
                                                   context switch. */

        u8              fpu_dirty;              /* fxsave_area modified
                                                   by vmxmon */
        u8              pad1[7];

        /* misc CPU state */
        u64             pending_debug_exc;
        u32             intability_state;
        u32             pad6;
        
        /* MSR state */
        u64             ia32_dbgctl;
        u64             ia32_sysenter_esp;
        u64             ia32_sysenter_eip;
        u16             ia32_sysenter_cs;
        u8              pad7[6];
        u64             msr_star;
        u64             msr_lstar;              /* x86-64 specific */
        u64             msr_cstar;              /* AMD only */
        u64             msr_syscall_flag_mask;
        u64             msr_kernel_gs_base;
        u64             msr_efer;
        u64             ia32_tsc_aux;
        u64             ia32_bndcfgs;
        u64             reserved3[1];
        u64             ia32_vmx_misc;

        /* VMX root and non-root modes */
        u8              vmx_non_root_mode;
        u8              vmx_non_root_guest_ept;
        u8              vmx_root_mode;
        u8              vmcs_loaded;
        u8              vmcs_dirty_userspace;   /* field written by userspace */
        u8              vmcs_dirty_vmxmon;      /* field written by vmxmon */
        u8              pad8[2];
        u64             cr0_read_shadow;
        u64             cr0_shadow_bits;
        u64             cr4_read_shadow;
        u64             cr4_shadow_bits;
        u64             vmx_eptp;
        u64             vmx_msr_ept_vpid_cap;

        /* guest configuration */
        u8              maxphyaddr;             /* guest MAXPHYADDR (32-52) */
        u8              has_1gb_mmu_page;       /* CPU supports 1GB pages */
        u8              has_rdtscp;             /* CPU supports RDTSCP */
        u8              pad4[5];

        /* simulation control */
        u32             active_vmxmon_features; /* set by user,
                                                   VMXMON_FEATURES_xxx */
        u32             intercepts;             /* events to intercept */
        u32             exc_bitmap;             /* exceptions to intercept */
        u32             msr_intercepts_r;       /* MSR_INTERCEPT_xx */
        u32             msr_intercepts_w;        
        u8              return_on_pause;
        u8              tsc_shift;
        u8              pad3[2];

        /* step timing */
        s64             step_limit;             /* max steps to run */
        s64             steps;                  /* steps executed */
        s64             abs_time;               /* total #steps executed */
        u32             step_threshold;         /* step safety margin */
        u32             rdtsc_threshold;        /* deprecated */

        /* entry information */
        ev_injection_t  injection;              /* event to inject (if any) */

        /* entry actions */
        utable_t        invlpg_table;           /* INVLPG invalidations */
        u8              clear_tlb;              /* clear TLB */
        u8              cr3_flush;              /* perform a cr3 flush */
        u8              cr3_flush_keep_global;  /* keep g entries on flush */
        u8              step_over_on_entry;     /* step over on entry */
        u8              pad5[3];
        volatile u8     running;                /* currently running guest */
        u64             reserved4;

        /* exit information */
        u64             vmret_qual;             /* return qualifier */
        u32             _vmret;                 /* return reason */
        u32             ecode;                  /* error code */
        u32             vmret_inst_info;        /* (current unused) */
        u8              int_vec;                /* exception number */
        u8              instruction_length;
        u8              page_access_rights;     /* current page access rights */
        u8              pad2;

        /* cycle timing (obsolete) */
        s64             cycle_limit;            /* >= steps + stall_cycles */
        s64             stall_cycles;
        u64             unused;
        u32             pause_slow_cycles;
        u32             pad10;

        /* RDTSC direct simulation */
        s64             tsc_cycles_limit;       /* deprecated */
        s64             tsc_cycles_spent;       /* <= tsc_cycle_limit */
        s64             tsc_start_value;        /* simulated time at start */

        /* reserved space */
        u64             reserved5[2];

        /* reserved space */
        volatile u8     signaled;               /* set by vmxmon_signal */
        u8              pad12[7];
        u64             reserved6[29];

        /* guest VMCS */
        u32             pad13;
        u32             vmcs_proc2_ctls;

        /* debug */
        u8              dbg_use_stepi;          /* allow tracing */
        u8              debug_stop;             /* used to stop simulation */
        u8              debug_trace;            /* debug trace flags */
        u8              dbg_cpu_migration;      /* force cpu bouncing */
        u8              pad11[4];

        /* statistics */
        u64             vmexit_stats[64];       /* vmexit statistics */
        u64             vmret_stats[96];        /* vmret statistics */
        u64             vmxmon_stats[96];       /* performance counters */
} vmxmon_t;

#define VMXMON_FEATURE_NMI_WINDOW  VMP_BIT(0) /* NMI window intercept */
#define VMXMON_FEATURE_TPR_LIMIT   VMP_BIT(1) /* TPR (cr8) handling */
#define VMXMON_FEATURE_EPT         VMP_BIT(2) /* EPT paging */
/* bit 3 used to be virtual box mode */
#define VMXMON_FEATURE_VPID        VMP_BIT(4) /* use VPID */
#define VMXMON_FEATURE_UNRESTRICTED_GUEST VMP_BIT(5) /* unrestricted guest */
#define VMXMON_FEATURE_BIOS_BUG_WORKAROUND VMP_BIT(6) /* BIOS bug workaround */
#define RESERVED_VMXMON_FEATURE_AVX        VMP_BIT(7)
#define VMXMON_FEATURE_REPORT_FETCH VMP_BIT(8)     /* Report code fetches */
#define VMXMON_FEATURE_1GB_NOFAULT_ERRATA VMP_BIT(9) /* 1GB pages bug */
#define VMXMON_FEATURE_SVMCS       VMP_BIT(10) /* Use Shadow VMCS */
#define VMXMON_FEATURE_FPU_DISABLE VMP_BIT(11) /* run with FPU disabled */
#define VMXMON_FEATURE_DIRECT_RDTSC VMP_BIT(12) /* direct RDTSC execution */
#define VMXMON_FEATURE_PKU          VMP_BIT(13) /* use PKU */
#define VMXMON_FEATURE_NESTED_EPT   VMP_BIT(14) /* use EPT for nested guests */
#define VMXMON_FEATURE_TSC_SHIFT    VMP_BIT(15) /* use user mode TSC shift */
#define VMXMON_FEATURE_5_LEVEL_PAGING VMP_BIT(16) /* LA57/PA52, EPT5 paging */

#define VMXMON_CAPABILITY_1GB_PAGES VMP_BIT(0) /* host supports 1GB pages */
#define VMXMON_CAPABILITY_XSAVE     VMP_BIT(1) /* host supports XSAVE */
#define VMXMON_CAPABILITY_AVX       VMXMON_CAPABILITY_XSAVE
#define VMXMON_CAPABILITY_LZCNT     VMP_BIT(2) /* LZCNT support */
#define VMXMON_CAPABILITY_BMI1      VMP_BIT(3) /* BMI1 (including TZCNT) */
#define VMXMON_CAPABILITY_FPU_CSDS_DEPRECATION VMP_BIT(4) /* Host supports FPU
                                                          CS/DS deprecation */
#define VMXMON_CAPABILITY_SIGNAL    VMP_BIT(5) /* vmxmon_signal supported */
#define VMXMON_CAPABILITY_INST_INFO VMP_BIT(6) /* vmret_inst_info support */
#define VMXMON_CAPABILITY_EXECUTE_ONLY VMP_BIT(7) /* execute-only support */
#define VMXMON_CAPABILITY_DESC_TABLE VMP_BIT(8) /* descriptor-table exiting */
#define VMXMON_CAPABILITY_PKU       VMP_BIT(9) /* support protection keys */
#define VMXMON_CAPABILITY_NON_ROOT_TPR VMP_BIT(10) /* support TPR (cr8)
                                                   handling in VMX non-root */

#define INTERCEPT_INT_WINDOW    VMP_BIT(0) /* intercept int window */
#define INTERCEPT_CR3_READ      VMP_BIT(1) /* intercept reads from cr3 */
#define INTERCEPT_NMI_WINDOW    VMP_BIT(2) /* intercept NMI window */
#define INTERCEPT_CR8_LOAD      VMP_BIT(3)
#define INTERCEPT_CR8_STORE     VMP_BIT(4)
#define INTERCEPT_CR0_WRITE     VMP_BIT(5)
#define INTERCEPT_CR3_WRITE     VMP_BIT(6)
#define INTERCEPT_CR4_WRITE     VMP_BIT(7)
#define INTERCEPT_TLB_FLUSHES   VMP_BIT(8)
#define INTERCEPT_DESC_TABLE    VMP_BIT(9)

#define MSR_INTERCEPT_FS_BASE   VMP_BIT(0) /* IA32_FS_BASE */
#define MSR_INTERCEPT_GS_BASE   VMP_BIT(1) /* IA32_GS_BASE */
#define MSR_INTERCEPT_KERNEL_GS VMP_BIT(2) /* IA32_KERNEL_GS_BASE */
#define MSR_INTERCEPT_EFER      VMP_BIT(3) /* IA32_EFER */

#define REGBIT_CR3              VMP_BIT(0)
#define REGBIT_CR               VMP_BIT(1) /* cr0, cr1, cr2, cr4 */
#define REGBIT_SEGS             VMP_BIT(2) /* all segments except CS */
#define REGBIT_IP               VMP_BIT(3) /* ip */
#define REGBIT_IP_INFO          VMP_BIT(4) /* CS and ilen */
#define REGBIT_SP               VMP_BIT(5) /* stack pointer (GPR_SP) */
#define REGBIT_MSRS             VMP_BIT(6) /* MSRs and debug registers */
#define REGBIT_DESC_TABLES      VMP_BIT(7) /* GDT, IDT */
#define REGBIT_INTABILITY       VMP_BIT(8) /* Interruptibility state */
#define REGBIT_FLOW_CNTRL       VMP_BIT(9) /* event injection etc */
#define REGBIT_INTERCEPTS       VMP_BIT(10) /* intercepts, exc_bitmask */
#define REGBIT_EFLAGS           VMP_BIT(11) /* eflags */
#define REGBIT_BREAKPOINTS      VMP_BIT(12) /* breakpoints */

#define REGBIT_FEATURES         VMP_BIT(16) /* vmxmon features */

#define REGBIT_OTHER            VMP_BIT(31) /* all other registers */
#define REGBIT_ALL              ((u32)-1)
#define REGBIT_ALL_STATE        (VMP_BIT(31) | 0xffff)

/* ioctl interface */

#define REQ_MAP_COMPAGE                 1
#define REQ_VM_ENTER                    2
#define REQ_ADD_TPHYS_MAPPING           3
#define REQ_SETUP                       4
#define TARGET_TYPE_32E                 1
#define TARGET_TYPE_32                  2
#define TARGET_WANTS_5_LEVEL_PAGES     (TARGET_TYPE_32E | 4)
#define REQ_GET_REGS                    5               /* regbits */
#define REQ_HANDLE_DIRTY_PAGES          6
#define REQ_DESTROY_TLB                 7
#define REQ_CREATE_GROUP                9
#define REQ_PROTECT_GROUP               10              /* gid, prot */
#define REQ_PROTECT_PAGE                11              /* tphys, prot */
#define REQ_GET_TLB_MAPPINGS            12
#define REQ_ADD_RANGED_BREAKPOINT       13              /* start, n_pages, prot */
#define REQ_REMOVE_RANGED_BREAKPOINT    14              /* id */
#define REQ_SHARE_PSPACE                16              /* fd2 */
#define REQ_GET_SESSION_ID              17              /* */
#define REQ_BLOCK_ENTRY                 18
#define REQ_UNBLOCK_ENTRY               19
#define REQ_MAP_VMCS_TABLE              20
#define REQ_PERFORM_LAZY_FLUSHES        21
#define REQ_MAP_XSAVE_AREA              22
#define REQ_CLEAR_STATS                 23
#define REQ_GET_VMEXIT_STATS            24              /* index */
#define REQ_GET_VMRET_STATS             25              /* index */
#define REQ_GET_PERFCTR_STATS           26              /* index */

#if defined(__cplusplus)
}
#endif

#endif   /* VMXMON_H */
