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

#include "kernel-api.h"
#include "cpudefs.h"
#include "vmxmon.h"
#include "vm.h"
#include "vmx.h"
#include "asm-inlines.h"
#include "tlb.h"
#include "pctr.h"
#include "core.h"
#include "dbg.h"
#include "nested-ept.h"
#include "host-instructions.h"
#include "vm-exit-codes.h"

#ifdef __windows__
#define WINDOWS                 1
#else
#define WINDOWS                 0
#endif
#define LINUX !WINDOWS

/* 0 means we handle it in kernel */
static const unsigned char passon_vmexits[] = {
        [VMEXIT_EXC]                    = 0,
        [VMEXIT_IRQ]                    = VMRET_AGAIN /* VMRET_NOP */,
        [VMEXIT_TRIPLE_FAULT]           = VMRET_TRIPLE_FAULT,
        [VMEXIT_INIT]                   = 0,
        [VMEXIT_STARTUP_IPI]            = VMRET_STARTUP_IPI,
        [VMEXIT_IRQ_WINDOW]             = 0,
        [VMEXIT_TASK_SWITCH]            = VMRET_TASK_SWITCH,
        [VMEXIT_CPUID]                  = VMRET_CPUID,
        [VMEXIT_HLT]                    = VMRET_HLT,
        [VMEXIT_INVD]                   = VMRET_INVD,
        [VMEXIT_INVLPG]                 = 0,
        [VMEXIT_RDPMC]                  = VMRET_RDPMC,
        [VMEXIT_RDTSC]                  = VMRET_RDTSC,
        [VMEXIT_VMCALL]                 = VMRET_VMCALL,
        [VMEXIT_VMCLEAR]                = VMRET_VMCLEAR,
        [VMEXIT_VMLAUNCH]               = VMRET_VMLAUNCH,
        [VMEXIT_VMPTRLD]                = VMRET_VMPTRLD,
        [VMEXIT_VMPTRST]                = VMRET_VMPTRST,
        [VMEXIT_VMREAD]                 = 0,
        [VMEXIT_VMRESUME]               = VMRET_VMRESUME,
        [VMEXIT_VMWRITE]                = 0,
        [VMEXIT_VMXOFF]                 = VMRET_VMXOFF,
        [VMEXIT_VMXON]                  = VMRET_VMXON,
        [VMEXIT_CTRL_REG_ACCESS]        = 0,
        [VMEXIT_MOV_DR]                 = VMRET_MOV_DR,
        [VMEXIT_IO]                     = VMRET_IO,
        [VMEXIT_RDMSR]                  = VMRET_RDMSR,
        [VMEXIT_WRMSR]                  = VMRET_WRMSR,
        [VMEXIT_INV_GUEST_STATE]        = VMRET_INV_GUEST_STATE,
        [VMEXIT_MSR_LOAD_FAILURE]       = VMRET_MSR_LOAD_FAILURE,
        [VMEXIT_MWAIT]                  = VMRET_MWAIT,
        [VMEXIT_MONITOR]                = VMRET_MONITOR,
        [VMEXIT_PAUSE]                  = 0,
        [VMEXIT_MACHINE_CHECK]          = VMRET_MACHINE_CHECK,
        [VMEXIT_TPR_BELOW_THRESH]       = 0,
        [VMEXIT_GDTR_IDTR_ACCESS]       = VMRET_GDTR_IDTR_ACCESS,
        [VMEXIT_LDTR_TR_ACCESS]         = VMRET_LDTR_TR_ACCESS,
        [VMEXIT_INVEPT]                 = VMRET_INVEPT,
        [VMEXIT_RDTSCP]                 = VMRET_RDTSCP,
        [VMEXIT_INVVPID]                = VMRET_INVVPID,
        [VMEXIT_WBINVD]                 = VMRET_WBINVD,
        [VMEXIT_XSETBV]                 = VMRET_XSETBV,
        [VMEXIT_RDRAND]                 = VMRET_RDRAND,
        [VMEXIT_INVPCID]                = VMRET_INVPCID,
        [VMEXIT_VMFUNC]                 = VMRET_VMFUNC,
        [VMEXIT_ENCLS]                  = VMRET_ENCLS,
        [VMEXIT_RDSEED]                 = VMRET_RDSEED,
};

#define IA32_DBGCTL_GUEST_BITS          BIT(1)

static bool
bios_bug_prevents_execution(vm_t *vm)
{
        /* Certain BIOSes have problem with a 16-bit TSS:

          - The ThinkPad T400 crashes if the guest task type is 3 (16-bit TSS)
          and SMM mode is entered.

          - The Dell Latitude E6420 crashes if the task type is 3; the guest
          task selector is 0; and SMM mode is entered. The same problem
          has been observed in certain desktop machines too, with manually
          generated SMIs.

          As a workaround, we never run with a 16-bit TSS. If the task
          register selector is 0 (indicating a task register which has
          never been loaded) a 32-bit TSS is used instead. If the task
          register selector is non-NULL, then we refuse to run. */

        vmxmon_t *x = vm->com;
        unsigned type = (x->seg_attr[SEG_TR] & 0xf);
        return (type == 3 && x->seg_sel[SEG_TR] != 0
                && (x->active_vmxmon_features
                    & VMXMON_FEATURE_BIOS_BUG_WORKAROUND));
}

static inline u32
vmcs_get_sim_cr0(vm_t *vm)
{
        vmxmon_t *x = vm->com;
        return (x->cr0 & vm->cr0_host_mask)
                | (vmread(VMCS_GUEST_CR0) & ~vm->cr0_host_mask);
}

static inline u32
vmcs_get_sim_cr4(vm_t *vm)
{
        vmxmon_t *x = vm->com;
        return (x->cr4 & vm->cr4_host_mask)
                | (vmread(VMCS_GUEST_CR4) & ~vm->cr4_host_mask);
}

static void
vmcs_update_guest_cr0(vm_t *vm, u32 sim_cr0)
{
        const u32 ubits = CR0_PG | CR0_PE | CR0_WP | CR0_AM | CR0_TS | CR0_MP
                | CR0_EM;
        u64 host_mask = ~(u64)ubits;
        u32 cr0 = CR0_NE | CR0_ET;
        vmxmon_t *x = vm->com;

        if (!vm->use_ug) {
                cr0 |= CR0_PG | CR0_PE;
                host_mask |= CR0_PG | CR0_PE;
        }

        if (!vm->use_ept) {
                /* need to track MMU sub-mode changes */
                cr0 |= CR0_WP;
                host_mask |= CR0_WP;
        }

        if (!vm->fpu_owned) {
                cr0 |= CR0_TS | CR0_MP;
                host_mask |= CR0_TS | CR0_MP;
        }
        cr0 |= (sim_cr0 & ~host_mask);

        if (x->vmx_non_root_mode)
                host_mask |= x->cr0_shadow_bits;

        if ((x->intercepts & INTERCEPT_TLB_FLUSHES))
                host_mask |= CR0_PE | CR0_PG;

        if ((x->intercepts & INTERCEPT_CR0_WRITE))
                host_mask |= (u32)-1;

        vm->cr0_host_mask = host_mask;
        x->cr0 = sim_cr0;

        vmwrite(VMCS_CR0_GUEST_HOST_MASK, host_mask);
        vmwrite(VMCS_GUEST_CR0, cr0);

        u32 cr0_rs = x->vmx_non_root_mode ?
                ((sim_cr0 & ~x->cr0_shadow_bits)
                 | (x->cr0_read_shadow & x->cr0_shadow_bits))
                : sim_cr0;

        vmwrite(VMCS_CR0_READ_SHADOW, cr0_rs);
}

static inline void
vmcs_update_guest_cr4(vm_t *vm, u32 sim_cr4)
{
        vmxmon_t *x = vm->com;
        const u32 ubits = (CR4_VME | CR4_PVI | CR4_TSD | CR4_DE
                           | CR4_PSE | CR4_PAE | CR4_PGE
                           | CR4_PCE | CR4_OSFXSR | CR4_OSXMMEXCPT
                           | CR4_LA57
                           | CR4_FSGSBASE
                           | (vm->use_xsave ? CR4_OSXSAVE : 0)
                           | CR4_SMEP | CR4_SMAP
                           | (vm->use_pku ? CR4_OSPKE : 0)
                           | CR4_LAM_SUP)
                        & (gi.cr4_fixed_1 & ~gi.cr4_fixed_0)
                        & x->cr4_legal_mask;
        u32 cr4 = CR4_VMXE | CR4_MCE | (sim_cr4 & ubits);
        u64 host_mask = ~(u64)ubits;

        /* CR4_PCIDE are currently wired to 0 */

        if (!vm->use_ept) {
                cr4 |= CR4_PAE | CR4_PSE | CR4_PGE;
                /* CR4.LA57 is determined by target */
                host_mask |= CR4_PAE | CR4_PSE | CR4_PGE | CR4_LA57;
        }

        if (x->vmx_non_root_mode)
                host_mask |= x->cr4_shadow_bits;

        if (x->intercepts & INTERCEPT_TLB_FLUSHES) {
                const u32 tlb_affecting_bits = CR4_PSE | CR4_PAE | CR4_PGE
                        | CR4_PCIDE | CR4_SMEP | CR4_LA57;
                host_mask |= tlb_affecting_bits;
        }

        if (x->intercepts & INTERCEPT_CR4_WRITE)
                host_mask |= 0xffffffffU;

        vm->cr4_host_mask = host_mask;
        x->cr4 = sim_cr4;

        vmwrite(VMCS_CR4_GUEST_HOST_MASK, host_mask);
        vmwrite(VMCS_GUEST_CR4, cr4);

        const u32 target_non_root_read_shadow = (sim_cr4 & ~x->cr4_shadow_bits)
                 | (x->cr4_read_shadow & x->cr4_shadow_bits);
        u32 cr4_rs = x->vmx_non_root_mode? target_non_root_read_shadow:sim_cr4;

        vmwrite(VMCS_CR4_READ_SHADOW, cr4_rs);
}

static void
vmcs_update_pfault_ecode_match(vm_t *vm)
{
        vmxmon_t *x = vm->com;
        bool intercept = !vm->use_ept || x->vmx_non_root_mode
                || (x->exc_bitmap & BIT(14));

        /* Microsoft Hyper-V does not implement "Page-fault error-code mask"
         * and "Page-fault error-code match" VMCS fields. Access to these
         * fields causes "VMREAD/VMWRITE from/to unsupported VMCS component"
         * VM-instruction error. */
        if (gi.is_hyperv) {
                /* we want to intercept RSV since the number of
                   physical address bits could differ */
                intercept |= vm->maxphyaddr > gi.host_max_physaddr;

                u32 exc_bitmap = vmread(VMCS_EXC_BITMAP);
                if (intercept)
                        exc_bitmap |= BIT(14);
                else
                        exc_bitmap &= ~BIT(14);
                vmwrite(VMCS_EXC_BITMAP, exc_bitmap);

                return;
        }

        if (intercept) {
                vmwrite(VMCS_PFAULT_ECODE_MASK, 0);
                vmwrite(VMCS_PFAULT_ECODE_MATCH, 0);
        } else {
                /* we want to intercept RSV since the number of
                   physical address bits could differ */
                vmwrite(VMCS_PFAULT_ECODE_MASK, ECODE_RSV);
                vmwrite(VMCS_PFAULT_ECODE_MATCH, ECODE_RSV);
        }
}

static void
vmcs_update_vmentry_ctrls(vm_t *vm)
{
        vmxmon_t *x = vm->com;
        const cpu_state_t *cs = vm->cs;

        bool load_perf = (!cs->perf_event
                          && gi.has_vmcs_pctr_handling
                          && cs->pmc.perf_global_ctrl != 0);
        bool lma = (x->msr_efer & EFER_LMA);
        u32 entry_ctrls = VMENTRY_RES_1
                | (lma ? VMENTRY_IA32E_MODE_GUEST : 0)
                | (load_perf ? VMENTRY_LOAD_IA32_PERF_GLOBAL_CTRL : 0)
                | (gi.use_vmcs_pat_handling ? VMENTRY_LOAD_IA32_PAT : 0)
                | (gi.has_efer_support ? VMENTRY_LOAD_IA32_EFER : 0)
                | (vm->use_bndcfgs ? VMENTRY_LOAD_IA32_BNDCFGS : 0)
                | 1 * VMENTRY_LOAD_DEBUG_CTRLS;

        vmwrite(VMCS_VMENTRY_CTRLS, entry_ctrls);
}


static void
vmcs_update_proc_ctls(vm_t *vm)
{
        vmxmon_t *x = vm->com;

        bool int_window = (x->intercepts & INTERCEPT_INT_WINDOW);
        bool nmi_window = (x->intercepts & INTERCEPT_NMI_WINDOW);
        bool tpr_shadow = vm->use_tpr_limit;
        bool cr8_load = (x->intercepts & INTERCEPT_CR8_LOAD) || !tpr_shadow;
        bool cr8_store = (x->intercepts & INTERCEPT_CR8_STORE) || !tpr_shadow;
        bool invlpg = (x->intercepts & INTERCEPT_TLB_FLUSHES) || !vm->use_ept;
        bool cr3_store = (x->intercepts & INTERCEPT_CR3_READ)
                || gi.cr3_store_always_exits || !vm->use_ept;
        bool cr3_load = (x->intercepts & INTERCEPT_CR3_WRITE)
                || gi.cr3_load_always_exits || invlpg;

        if (x->vmx_non_root_mode) {
                invlpg = true;
                cr3_store = true;
                cr3_load = true;
        }

        u32 pctrls = PROC_RES_1
                | (int_window ? PROC_INT_WINDOW_EXITING : 0)
                | (nmi_window ? PROC_NMI_WINDOW_EXITING : 0)
                | (cr8_load ? PROC_CR8_LOAD_EXITING : 0)
                | (cr8_store ? PROC_CR8_STORE_EXITING : 0)
                | (vm->use_tpr_limit ? PROC_USE_TPR_SHADOW : 0)
                | (gi.has_msr_bitmap ? PROC_USE_MSR_BITMAPS : 0)
                | (gi.has_proc2 ? PROC_ACTIVATE_SEC_CTRLS : 0)
                | 1 * PROC_HLT_EXITING
                | 1 * PROC_MWAIT_EXITING
                | 1 * PROC_RDPMC_EXITING
                | 1 * PROC_MOV_DR_EXITING
                | 1 * PROC_UNCOND_IO_EXITING
                | 1 * PROC_MONITOR_EXITING
                | (vm->use_direct_rdtsc ? PROC_USE_TSC_OFFSETTING : 0)
                | 0 * PROC_USE_IO_BITMAPS
                | (vm->monitor_trap_flag ? PROC_MONITOR_TRAP_FLAG : 0)
                | (cr3_load ? PROC_CR3_LOAD_EXITING : 0)
                | (cr3_store ? PROC_CR3_STORE_EXITING : 0)
                | (invlpg ? PROC_INVLPG_EXITING : 0)
                | (vm->use_direct_rdtsc ? 0 : PROC_RDTSC_EXITING)
                | 1 * PROC_RDPMC_EXITING
                | 1 * PROC_PAUSE_EXITING;

        vm->proc_ctrls = pctrls;
        vmwrite(VMCS_PROC_BASED_VM_EXEC_CTRLS, pctrls);
}

static void
vmcs_set_pin_ctrls(vm_t *vm)
{
        u32 pin_ctrls = PIN_RES_1
                | (gi.has_virtual_nmi ? PIN_VIRTUAL_NMIS : 0)
                | 1 * PIN_EXT_INT_EXITING
                | 1 * PIN_NMI_EXITING
                | 0 * PIN_PROCESS_POSTED_INTERRUPTS;
        vmwrite(VMCS_PIN_BASED_VM_EXEC_CTRLS, pin_ctrls);
}

static void
vmcs_update_vmexit_ctrls(vm_t *vm)
{
        const cpu_state_t *cs = vm->cs;
        bool load_perf = (!cs->perf_event
                          && gi.has_vmcs_pctr_handling
                          && vm->cs->pmc.perf_global_ctrl != 0);
        u32 vmexit_ctrls = VMEXIT_RES_1
                | (gi.use_vmcs_pat_handling ? VMEXIT_LOAD_IA32_PAT : 0)
                | (gi.has_efer_support ? VMEXIT_LOAD_IA32_EFER : 0)
                | (load_perf ? VMEXIT_LOAD_IA32_PERF_GLOBAL_CTRL : 0)
                | 1 * VMEXIT_HOST_ASPACE_SIZE
                | 1 * VMEXIT_SAVE_DEBUG_CTRLS
                | 0 * VMEXIT_ACK_INT_ON_EXIT
                | 0 * VMEXIT_SAVE_IA32_PAT
                | 0 * VMEXIT_SAVE_IA32_EFER
                | (vm->use_bndcfgs ? VMEXIT_CLEAR_IA32_BNDCFGS : 0)
                | 0 * VMEXIT_SAVE_VMX_PREEMPTION_TIMER;
        vmwrite(VMCS_VMEXIT_CTRLS, vmexit_ctrls);
}

static void
vmcs_update_proc2_ctls(vm_t *vm)
{
        if (!gi.has_proc2)
                return;

        vmxmon_t *x = vm->com;
        u32 proc2_ctrls = 0
                | 0 * PROC2_VIRTUALIZE_APIC
                | (vm->use_ept ? PROC2_ENABLE_EPT : 0)
                | ((x->intercepts & INTERCEPT_DESC_TABLE) ?
                   PROC2_DESC_TABLE_EXITING :
                   ((x->active_vmxmon_features
                    & VMXMON_FEATURE_BIOS_BUG_WORKAROUND) ?
                    (gi.proc_based2_1_allowed & PROC2_DESC_TABLE_EXITING) : 0))
                | (gi.proc_based2_1_allowed & PROC2_ENABLE_RDTSCP)
                | 0 * PROC2_VIRTUALIZE_X2APIC_MODE
                | (vm->use_vpid ? PROC2_ENABLE_VPID : 0)
                | (gi.proc_based2_1_allowed & PROC2_WBINVD_EXITING)
                | (vm->use_ug ? PROC2_UNRESTRICTED_GUEST : 0)
                | 0 * PROC2_APIC_REG_VIRTUALIZATION
                | 0 * PROC2_VIRT_INT_DELIVERY
                | 0 * PROC2_PAUSE_LOOP_EXITING
                | (gi.proc_based2_1_allowed & PROC2_RDRAND_EXITING)
                | 0 * PROC2_ENABLE_INVPCID
                | 0 * PROC2_ENABLE_VMFUNC
                | (vm->use_svmcs ? PROC2_ENABLE_SVMCS: 0)
                | (gi.proc_based2_1_allowed & PROC2_ENCLS_EXITING)
                | (gi.proc_based2_1_allowed & PROC2_RDSEED_EXITING)
                | ((vm->use_ept && vm->use_mbe) ? PROC2_MODE_BASED_EXEC_EPT : 0)
                | (vm->use_direct_rdtsc ?
                   (gi.proc_based2_1_allowed & PROC2_USE_TSC_SCALING) : 0);

        vm->proc2_ctrls = proc2_ctrls;
        vmwrite(VMCS_SEC_PROC_BASED_CTLS, proc2_ctrls);
}

static void
vmcs_update_exc_bitmap(vm_t *vm)
{
        vmxmon_t *x = vm->com;
        unsigned exc_bitmap = (x->exc_bitmap
                               | (1 << 7)  /*#NM*/
                               | (1 << 14) /*#PF*/
                               | (1 << 1)  /*#DB*/);

        /* for now, intercept everything */
        exc_bitmap |= 0xffffffff;
        vmwrite(VMCS_EXC_BITMAP, exc_bitmap);
}

static void
modify_msr_bitmap(vm_t *vm, unsigned msr_num,
                  bool r_intercept, bool w_intercept)
{
        bool use_high = (msr_num >= 0xc0000000);
        unsigned msr_offs = use_high ? msr_num - 0xc0000000 : msr_num;
        unsigned read_offs = (msr_offs / 8) + (use_high ? 1024 : 0);
        unsigned write_offs = read_offs + 2048;
        u8 *p = (u8 *)vm->msr_bitmap_page.p;
        u8 val = 1 << (msr_offs & 7);

        wrap_BUG_ON(read_offs >= 2048);

        p[read_offs] &= ~val;
        p[write_offs] &= ~val;

        if (r_intercept)
                p[read_offs] |= val;
        if (w_intercept)
                p[write_offs] |= val;
}

static void
update_msr_intercepts(vm_t *vm)
{
        if (!gi.has_msr_bitmap)
                return;

        vmxmon_t *x = vm->com;
        u32 new_r = x->msr_intercepts_r;
        u32 new_w = x->msr_intercepts_w;

        /* KERNEL_GS is not saved or loaded in 32-bit mode */
        if (!vm->ia32e_guest) {
                new_r |= MSR_INTERCEPT_KERNEL_GS;
                new_w |= MSR_INTERCEPT_KERNEL_GS;
        }

        /* guest efer does not always match the real efer value */
        if (!vm->may_read_efer)
                new_r |= MSR_INTERCEPT_EFER;

        /* intercept everything in VMX non-root mode */
        if (x->vmx_non_root_mode) {
                new_r |= -1;
                new_w |= -1;
        }

        if (vm->msr_intercepts_r != new_r || vm->msr_intercepts_w != new_w) {
                vm->msr_intercepts_r = new_r;
                vm->msr_intercepts_w = new_w;

                modify_msr_bitmap(vm, MSR_FS_BASE,
                                  (new_r & MSR_INTERCEPT_FS_BASE),
                                  (new_w & MSR_INTERCEPT_FS_BASE));
                modify_msr_bitmap(vm, MSR_GS_BASE,
                                  (new_r & MSR_INTERCEPT_GS_BASE),
                                  (new_w & MSR_INTERCEPT_GS_BASE));
                modify_msr_bitmap(vm, MSR_KERNEL_GS_BASE,
                                  (new_r & MSR_INTERCEPT_KERNEL_GS),
                                  (new_w & MSR_INTERCEPT_KERNEL_GS));
                modify_msr_bitmap(vm, MSR_EFER,
                                  (new_r & MSR_INTERCEPT_EFER),
                                  true);
        }
}

static bool
possible_spurious_nm_due_to_errata(u64 cr0)
{
        /* Certain CPUs have problem with FXSAVE and FXRSTOR instructions.
           They may produce #NM instead of #UD when used with the F3 prefix.
           Experiments shows that #NM is generated only if guest set CR0.TS.

           RDFSBASE encoding is REP + FXSAVE:  F3 0F AE /0
           RDGSBASE encoding is REP + FXRSTOR: F3 0F AE /1

           This issue is observed on E5-2670 and i7-2600 SandyBridge hosts.
           The Intel® microarchitecture code name Westmere CPU X5650 also
           produce same issue.  We assume that all host without FSGSBASE
           support have this problem.

           As a workaround we intercept #NM exceptions if FSGSBASE instructions
           are not supported by host and CR0.TS bit is set. */
        return !gi.has_fsgsbase && (cr0 & CR0_TS);
}

static bool
xcr0_illegal(u64 xcr0)
{
        /* X87 bit is clear */
        if (!(xcr0 & XSTATE_X87))
                return true;
        /* Intel® AVX set while SSE cleared */
        if ((xcr0 & (XSTATE_SSE | XSTATE_AVX)) == XSTATE_AVX)
                return true;
        /* Intel® MPX state must be all set or all cleared */
        if ((xcr0 & XSTATE_MPX) != 0 && (xcr0 & XSTATE_MPX) != XSTATE_MPX)
                return true;
        /* Intel® AVX512 state must be all set or all cleared */
        if ((xcr0 & XSTATE_AVX512) != 0
            && (xcr0 & XSTATE_AVX512) != XSTATE_AVX512)
                return true;
        return false;
}

static bool
mpx_enabled(vm_t *vm)
{
        if (vm->guest_xcr0 & XSTATE_MPX) {
                u64 bndcfgu = 0;
                /* (guest_xcr0 & XSTATE_MPX) implies large XSAVE area */
                if (vm->xsave_area->xstate_bv & XSTATE_MPX) {
                        /* BNDCFGU is bytes 7:0 in XSAVE.BNDCSR */
                        u64 *bndcsr_p = (u64 *)((char *)vm->xsave_area
                                                + gi.xsave_bndcsr_offs);
                        bndcfgu = bndcsr_p[0];
                }
                vmxmon_t *x = vm->com;
                return (bndcfgu | x->ia32_bndcfgs) & BNDCFG_EN;
        }
        return 0;
}

/************************************************************************/
/*	MSR handling            					*/
/************************************************************************/
static u32
imsr2msr(vm_t *vm, unsigned char imsr)
{
        switch (imsr) {
        case I_MSR_EFER:
                return MSR_EFER;
        case I_MSR_KERNEL_GS_BASE:
                return MSR_KERNEL_GS_BASE;
        case I_MSR_LSTAR:
                return MSR_LSTAR;
        case I_MSR_STAR:
                return MSR_STAR;
        case I_MSR_FMASK:
                return MSR_IA32_FMASK;
        case I_MSR_PERF_GLOBAL_CTRL:
                return MSR_IA32_PERF_GLOBAL_CTRL;
        case I_MSR_STEP_CTR:
                return vm->cs->pmc.step_msr;
        case I_MSR_PMC_CTRL:
                return vm->cs->pmc.ctrl_msr;
        case I_MSR_PMC_CTRL2:
                return vm->cs->pmc.ctrl2_msr;
        case I_MSR_CR_PAT:
                return MSR_IA32_CR_PAT;
        case I_MSR_TSC_AUX:
                return MSR_IA32_TSC_AUX;
        case I_MSR_TSC:
                return MSR_IA32_TIME_STAMP_COUNTER;
        default:
                wrap_BUG_ON(1);
                return 0;
        }
}

static inline u8
msr2imsr(vm_t *vm, u64 msr)
{
        u8 imsr;
        for (imsr = 0; imsr < NUM_IMSR; imsr++) {
                if (vm->imsr2msr[imsr] == msr)
                        return imsr;
        }

        wrap_BUG_ON(1);
        return 0;
}

/* auto-load/store of MSRs */
typedef struct msr_entry {
        u32     msr;
        u32     reserved;
        u64     value;
} msr_entry_t;

static inline msr_entry_t *
msr_slot(vm_t *vm, u8 slot)
{
        return (msr_entry_t*)vm->msr_page.p + slot;
}

static inline bool
hmsr_active(vm_t *vm, unsigned char imsr)
{
        return vm->h_msr[imsr] != msr_slot(vm, UNUSED_MSR_SLOT);
}

static inline bool
gmsr_active(vm_t *vm, unsigned char imsr)
{
        return vm->g_msr[imsr] != msr_slot(vm, UNUSED_MSR_SLOT);
}


/* VM-exit MSR-load area starts at FIRST_HMSR_SLOT */
static inline u64
vmexit_msr_load_addr(u64 msr_page_paddr)
{
        return msr_page_paddr + sizeof(msr_entry_t)*FIRST_HMSR_SLOT;
}

static inline u8
vmexit_msr_load_count(vm_t *vm) 
{
        return vm->n_h_msrs;
}

/* VM-exit MSR-store area is stacked under FIRST_GMSR_SLOT */
static inline u64
vmexit_msr_store_addr(vm_t *vm, u64 msr_page_paddr)
{
        return msr_page_paddr + sizeof(msr_entry_t) 
                * (FIRST_GMSR_SLOT - vm->n_g_msrs_so);
}

static inline u8
vmexit_msr_store_count(vm_t *vm) 
{
        return vm->n_g_msrs_so + vm->n_g_msrs_ls;
}

/* VM-exit MSR-load area starts at FIRST_GMSR_SLOT */
static inline u64
vmentry_msr_load_addr(u64 msr_page_paddr)
{
        return msr_page_paddr + sizeof(msr_entry_t)*FIRST_GMSR_SLOT;
}

static inline u8
vmentry_msr_load_count(vm_t *vm) 
{
        return vm->n_g_msrs_l;
}

/* imsr: make VM-exit to load 'value' into it */
static void
activate_host_msr(vm_t *vm, unsigned char imsr, u64 value)
{
        msr_entry_t *unused = msr_slot(vm, UNUSED_MSR_SLOT);
        wrap_BUG_ON(vm->h_msr[imsr] != unused);

        u8 slot = vm->n_h_msrs++;
        msr_entry_t *mt = msr_slot(vm, FIRST_HMSR_SLOT + slot);
        mt->msr = vm->imsr2msr[imsr];
        mt->value = value;
        mt->reserved = 0;

        vm->h_msr[imsr] = mt;
        vm->hslot2imsr[slot] = imsr;
        vmwrite(VMCS_VMEXIT_MSR_LOAD_CNT, vmexit_msr_load_count(vm));
}

/* imsr: make VM-exit to store its value */
static void
activate_guest_store_only_msr(vm_t *vm, unsigned char imsr)
{
        msr_entry_t *unused = msr_slot(vm, UNUSED_MSR_SLOT);
        wrap_BUG_ON(vm->g_msr[imsr] != unused);

        u8 slot = ++vm->n_g_msrs_so;
        msr_entry_t *mt = msr_slot(vm, FIRST_GMSR_SLOT - slot);
        mt->msr = vm->imsr2msr[imsr];
        mt->reserved = 0;

        vm->g_msr[imsr] = mt;
        vmwrite(VMCS_VMEXIT_MSR_STORE_CNT, vmexit_msr_store_count(vm));

        u64 pa = mpage_phys(&vm->msr_page);
        vmwrite(VMCS_VMEXIT_MSR_STORE_ADDR, vmexit_msr_store_addr(vm, pa));
}

/* imsr: make VM-entry to load 'value' into it */
static void
activate_guest_load_only_msr(vm_t *vm, unsigned char imsr, u64 value)
{
        msr_entry_t *unused = msr_slot(vm, UNUSED_MSR_SLOT);
        wrap_BUG_ON(vm->g_msr[imsr] != unused);

        u8 slot = vm->n_g_msrs_l++;
        msr_entry_t *mt = msr_slot(vm, FIRST_GMSR_SLOT + slot);
        mt->msr = vm->imsr2msr[imsr];
        mt->value = value;
        mt->reserved = 0;

        vm->g_msr[imsr] = mt;
        vmwrite(VMCS_VMENTRY_MSR_LOAD_CNT, vmentry_msr_load_count(vm));
}

/* imsr: make VM-entry to load it, VM-exit to store it
   NB: it isn't allowed to call this function after activate_guest_load_only_msr
       was called (the load/store imsrs are to be contiguous in the host
       memory: we don't do any reordering if load-only MSR was added between).
 */
static void
activate_guest_load_store_msr(vm_t *vm, unsigned char imsr)
{
        /* Load only MSRs should be added after load and store MSRs, see "NB:"
           section in the comment above. */
        wrap_BUG_ON(vm->n_g_msrs_l != vm->n_g_msrs_ls);
        activate_guest_load_only_msr(vm, imsr, 0);
        vm->n_g_msrs_ls++;
}

/* imsr (convenience function combining two):
       make VM-entry to load 'value' to it
       and VM-exit to restore its current value */
static void
activate_guest_and_host_msr(vm_t *vm, unsigned char imsr, u64 value)
{
        activate_guest_load_only_msr(vm, imsr, value);
        activate_host_msr(vm, imsr, rdmsr(vm->imsr2msr[imsr]));
}

static void
deactivate_host_msr(vm_t *vm, unsigned char imsr)
{
        msr_entry_t *unused = msr_slot(vm, UNUSED_MSR_SLOT);
        msr_entry_t *base = msr_slot(vm, FIRST_HMSR_SLOT);
        wrap_BUG_ON(vm->h_msr[imsr] == unused);

        u8 slot = vm->h_msr[imsr] - base;
        u8 last_slot = vm->n_h_msrs - 1;
        u8 last_imsr = vm->hslot2imsr[last_slot];

        /* relocate the last host msr to the deactivated msr slot */
        base[slot] = base[last_slot];
        vm->hslot2imsr[slot] = last_imsr;
        vm->h_msr[last_imsr] = &base[slot];

        /* host imsr is now unused */
        vm->h_msr[imsr] = unused;

        /* load one less host MSR at VMEXIT */
        vm->n_h_msrs--;
        vmwrite(VMCS_VMEXIT_MSR_LOAD_CNT, vmexit_msr_load_count(vm));
}

static void
deactivate_guest_load_only_msr(vm_t *vm, unsigned char imsr)
{
        msr_entry_t *unused = msr_slot(vm, UNUSED_MSR_SLOT);
        msr_entry_t *base = msr_slot(vm, FIRST_GMSR_SLOT);
        wrap_BUG_ON(vm->g_msr[imsr] == unused);

        u8 slot = vm->g_msr[imsr] - base;
        u8 last_slot = vm->n_g_msrs_l - 1;
        msr_entry_t *last_entry = msr_slot(vm, FIRST_GMSR_SLOT + last_slot);
        u8 last_imsr = msr2imsr(vm, last_entry->msr);

        /* relocate the last guest msr to the deactivated msr slot */
        base[slot] = base[last_slot];
        vm->g_msr[last_imsr] = &base[slot];

        /* guest imsr is now unused */
        vm->g_msr[imsr] = unused;

        /* load one less guest MSR at VMENTRY */
        vm->n_g_msrs_l--;
        vmwrite(VMCS_VMENTRY_MSR_LOAD_CNT, vmentry_msr_load_count(vm));
}

static void
deactivate_guest_store_only_msr(vm_t *vm, unsigned char imsr)
{
        msr_entry_t *unused = msr_slot(vm, UNUSED_MSR_SLOT);
        msr_entry_t *base = msr_slot(vm, FIRST_GMSR_SLOT - vm->n_g_msrs_so);
        wrap_BUG_ON(vm->g_msr[imsr] == unused);

        u8 slot = vm->g_msr[imsr] - base;
        msr_entry_t *last_entry = msr_slot(vm, FIRST_GMSR_SLOT - 1);
        u8 last_imsr = msr2imsr(vm, last_entry->msr);

        /* relocate the first guest msr to the deactivated msr slot */
        base[slot] = base[0];
        vm->g_msr[last_imsr] = &base[slot];

        /* guest imsr is now unused */
        vm->g_msr[imsr] = unused;

        /* store one less guest MSR at VMEXIT */
        vm->n_g_msrs_so--;
        vmwrite(VMCS_VMEXIT_MSR_STORE_CNT, vmexit_msr_store_count(vm));

        u64 pa = mpage_phys(&vm->msr_page);
        vmwrite(VMCS_VMEXIT_MSR_STORE_ADDR, vmexit_msr_store_addr(vm, pa));
}

static void
reset_imsr_handling(vm_t *vm)
{
        int i;
        for (i = 0; i < NUM_IMSR; i++) {
                vm->imsr2msr[i] = imsr2msr(vm, i);
                vm->g_msr[i] = msr_slot(vm, UNUSED_MSR_SLOT);
                vm->h_msr[i] = msr_slot(vm, UNUSED_MSR_SLOT);
                vm->hslot2imsr[i] = 0;
        }
        vm->n_h_msrs = 0;
        vm->n_g_msrs_l = 0;
        vm->n_g_msrs_ls = 0;
        vm->n_g_msrs_so = 0;
}

static void
set_vmcs_msr_load_store_info(vm_t *vm)
{
        u64 pa = mpage_phys(&vm->msr_page);

        vmwrite(VMCS_VMENTRY_MSR_LOAD_ADDR, vmentry_msr_load_addr(pa));
        vmwrite(VMCS_VMEXIT_MSR_STORE_ADDR, vmexit_msr_store_addr(vm, pa));
        vmwrite(VMCS_VMEXIT_MSR_LOAD_ADDR, vmexit_msr_load_addr(pa));

        vmwrite(VMCS_VMENTRY_MSR_LOAD_CNT, vmentry_msr_load_count(vm));
        vmwrite(VMCS_VMEXIT_MSR_STORE_CNT, vmexit_msr_store_count(vm));
        vmwrite(VMCS_VMEXIT_MSR_LOAD_CNT, vmexit_msr_load_count(vm));
}

static inline void
set_hmsr(vm_t *vm, unsigned char imsr, u64 val)
{
        vm->h_msr[imsr]->value = val;
}

static inline void
set_gmsr(vm_t *vm, unsigned char imsr, u64 val)
{
        vm->g_msr[imsr]->value = val;
}

static inline u64
get_gmsr(vm_t *vm, unsigned char imsr)
{
        return vm->g_msr[imsr]->value;
}

static void
set_guest_load_and_host_msr(vm_t *vm, unsigned char imsr,
                       u64 guest_val, u64 host_val)
{
        bool new_active = (guest_val != host_val);
        bool old_active = hmsr_active(vm, imsr);

        if (likely(new_active == old_active)) {
                if (new_active) {
                        set_gmsr(vm, imsr, guest_val);
                        set_hmsr(vm, imsr, host_val);
                }
        } else {
                if (new_active) {
                        activate_guest_load_only_msr(vm, imsr, guest_val);
                        activate_host_msr(vm, imsr, host_val);
                } else {
                        deactivate_guest_load_only_msr(vm, imsr);
                        deactivate_host_msr(vm, imsr);                        
                }
        }
}

static void
clear_guest_and_host_msr(vm_t *vm, unsigned char imsr)
{
        if (hmsr_active(vm, imsr)) {
                deactivate_guest_load_only_msr(vm, imsr);
                deactivate_host_msr(vm, imsr);                
        }
}

static void
setup_msr_handling(vm_t *vm)
{
        const cpu_state_t *cs = vm->cs;

        reset_imsr_handling(vm);

        /* setup IA32_KERNEL_GS_BASE */
        if (vm->ia32e_guest) {
                activate_guest_load_store_msr(vm, I_MSR_KERNEL_GS_BASE);
                activate_host_msr(vm, I_MSR_KERNEL_GS_BASE,
                                  rdmsr(MSR_KERNEL_GS_BASE));
        }

        /* guest MSRs which are modified must go first */
        if (vm->use_perf_counters) {
                activate_guest_load_store_msr(vm, I_MSR_STEP_CTR);
                if (cs->perf_event) {
                        activate_host_msr(vm, I_MSR_STEP_CTR, 0x0);
                } else {
                        /* update_host_perf_counter_state sets hosts values */
                        activate_guest_and_host_msr(
                                vm, I_MSR_PMC_CTRL, cs->pmc.ctrl_val);

                        if (cs->pmc.ctrl2_msr != 0) {
                                activate_guest_and_host_msr(
                                        vm, I_MSR_PMC_CTRL2, cs->pmc.ctrl2_val);
                        }
                        /* setup IA32_PERF_GLOBAL_CTRL */
                        u64 gval = cs->pmc.perf_global_ctrl;
                        if (gval != 0) {
                                gi.has_vmcs_pctr_handling
                                ? vmwrite(
                                        VMCS_GUEST_IA32_PERF_GLOBAL_CTRL, gval)
                                : activate_guest_and_host_msr(
                                        vm, I_MSR_PERF_GLOBAL_CTRL, gval);
                        }
                }
        }

        /* setup IA32_EFER */
        if (!gi.has_efer_support)
                activate_guest_and_host_msr(vm, I_MSR_EFER, 0);
        else
                vmwrite(VMCS_HOST_IA32_EFER, rdmsr(MSR_EFER));

        /* setup IA32_LSTAR (IA32_STAR and IA32_FMASK are handled on-demand) */
        if (vm->ia32e_guest)
                activate_guest_and_host_msr(vm, I_MSR_LSTAR, 0);

        /* we only use PAT0 but we set all entries as cachable (0x6) */
        if (gi.pat_handling_needed) {
                gi.use_vmcs_pat_handling
                ? (vmwrite(VMCS_GUEST_IA32_PAT, 0x0606060606060606ULL),
                   vmwrite(VMCS_HOST_IA32_PAT, rdmsr(MSR_IA32_CR_PAT)))
                : activate_guest_and_host_msr(
                        vm, I_MSR_CR_PAT, 0x0606060606060606ULL);
        }
        set_vmcs_msr_load_store_info(vm);
}

static void
set_guest_ia32_tsc_aux(vm_t *vm)
{
        vmxmon_t *x = vm->com;
        if (!vm->use_direct_rdtsc || !x->has_rdtscp)
                clear_guest_and_host_msr(vm, I_MSR_TSC_AUX);
        else
                set_guest_load_and_host_msr(vm, I_MSR_TSC_AUX,
                                       x->ia32_tsc_aux, vm->host_tsc_aux);
}

static inline u8
get_virt_apic_tpr(vm_t *vm)
{
        return *((u8 *)vm->virt_apic_page.p + 0x80) >> 4;
}

static inline void
set_virt_apic_tpr(vm_t *vm, u8 tpr)
{
        *((u8 *)vm->virt_apic_page.p + 0x80) = tpr << 4;
}

static inline void
update_tpr(vm_t *vm)
{
        if (vm->use_tpr_limit) {
                vmxmon_t *x = vm->com;
                set_virt_apic_tpr(vm, x->tpr);
                vmwrite(VMCS_TPR_THRESHOLD, x->tpr_threshold);
        }
}

static void
update_guest_efer(vm_t *vm)
{
        vmxmon_t *x = vm->com;
        u64 efer = (x->msr_efer & (EFER_SCE | EFER_LMA | EFER_NXE))
                | ((x->msr_efer & EFER_LMA) ? EFER_LME : 0);

        if (!vm->use_ept)
                efer |= EFER_NXE;

        if (!gi.has_efer_support)
                set_gmsr(vm, I_MSR_EFER, efer);
        else
                vmwrite(VMCS_GUEST_IA32_EFER, efer);

        /* update EFER intercept */
        bool may_read_efer = (efer == x->msr_efer);
        if (may_read_efer != vm->may_read_efer) {
                vm->may_read_efer = may_read_efer;
                update_msr_intercepts(vm);
        }
}

/* update host performance register state */
static void
update_host_perf_counter_state(vm_t *vm)
{
        cpu_state_t *cs = vm->cs;
        if (cs->perf_event)
                return;
        
        set_hmsr(vm, I_MSR_PMC_CTRL, rdmsr(cs->pmc.ctrl_msr));
        if (cs->pmc.ctrl2_msr != 0)
                set_hmsr(vm, I_MSR_PMC_CTRL2, rdmsr(cs->pmc.ctrl2_msr));

        u64 v = cs->pmc.perf_global_ctrl;
        if (v != 0) {
                u64 val = rdmsr(MSR_IA32_PERF_GLOBAL_CTRL) & ~v;
                if (gi.has_vmcs_pctr_handling)
                        vmwrite(VMCS_HOST_IA32_PERF_GLOBAL_CTRL, val);
                else
                        set_hmsr(vm, I_MSR_PERF_GLOBAL_CTRL, val);
        }
}

/************************************************************************/
/*	state transfer (VMCS -> vmxmon)					*/
/************************************************************************/

static inline u64
activate_vmcs(vm_t *vm)
{
        u64 old_vmcs = vmptrst();
        if (old_vmcs != vm->vmcs_phys)
                vmptrld(vm->vmcs_phys);
        return old_vmcs;
}

static void
update_32kb_bitmap(char *p, u64 field, bool val)
{
        if (!p || field >= 0x8000)
                return;
        int byte_off = field >> 3;
        int bit_off  = field & 0x7;
        if (val)
                p[byte_off] |= (1 << bit_off);
        else
                p[byte_off] &= ~(1 << bit_off);
}

static bool
read_32kb_bitmap(const char *p, u64 field)
{
        if (!p || field >= 0x8000)
                return true;
        int byte_off = field >> 3;
        int bit_off = field & 0x7;
        return p[byte_off] & (1 << bit_off);
}

static bool
guest_supports_writing_vmcs_field(vmxmon_t *x, u64 field)
{
        /* Guest might not support writing to VM-exit fields
           (bits [11:10] == 1). Check this by inspecting guest
           IA32_VMX_MISC[29] */
        if ((((field >> 10) & 3) == 1) && !(x->ia32_vmx_misc & BIT(29)))
                return false;
        return true;
}

static bool
probe_write_vmcs_field_from_guest(vm_t *vm, u64 field)
{
        /* Verify that we can write to VMCS field. Do not actually write data */
        vmxmon_t *x = vm->com;
        if (field & ~0x7fffll)
                return false;
        /* Do not allow to write to fields that we cannot read */
        if (read_32kb_bitmap(vm->vmread_bitmap.p, field))
                return false;
        if (!guest_supports_writing_vmcs_field(x, field))
                return false;
        return true;
}

static void
get_regs_from_vmcs(vm_t *vm, u32 regbits)
{
        vmxmon_t *x = vm->com;

        x->insync |= vm->dirty;
        regbits &= ~x->insync;

        if (regbits & REGBIT_IP)
                x->ip = vmread(VMCS_GUEST_RIP);

        if (regbits & REGBIT_IP_INFO) {
                x->seg_sel[SEG_CS] = vmread(VMCS_GUEST_CS_SEL);
                x->seg_limit[SEG_CS] = vmread(VMCS_GUEST_CS_LIMIT);
                x->seg_attr[SEG_CS] = vmread(VMCS_GUEST_CS_ACCESS_RIGHTS);
                x->seg_base[SEG_CS] = vmread(VMCS_GUEST_CS_BASE);
        }

        if (regbits & REGBIT_SP)
                x->gprs[GPR_SP] = vmread(VMCS_GUEST_RSP);

        if (regbits & REGBIT_EFLAGS)
                x->rflags = vmread(VMCS_GUEST_RFLAGS);

        if (regbits & REGBIT_SEGS) {
                /* BIOS bug workaround; we check for tr_sel == 0 to handle
                   hardware which lacks support for VMX descriptor exits */
                u16 tr_sel = vmread(VMCS_GUEST_TR_SEL);
                u32 tr_attr = vmread(VMCS_GUEST_TR_ACCESS_RIGHTS);
                if (vm->tss_type_optimization && tr_sel == 0)
                        tr_attr = (tr_attr & ~0xf) | 3;

                x->seg_sel[SEG_TR] = tr_sel;
                if (tr_attr & SEGATTR_U) {
                        x->seg_attr[SEG_TR] = SEGATTR_U;
                        x->seg_limit[SEG_TR] = 0;
                        x->seg_base[SEG_TR] = 0;
                } else {
                        x->seg_attr[SEG_TR] = tr_attr;
                        x->seg_limit[SEG_TR] = vmread(VMCS_GUEST_TR_LIMIT);
                        x->seg_base[SEG_TR] = vmread(VMCS_GUEST_TR_BASE);
                }

                x->seg_sel[SEG_ES] = vmread(VMCS_GUEST_ES_SEL);
                u32 es_attr = vmread(VMCS_GUEST_ES_ACCESS_RIGHTS);
                if (es_attr & SEGATTR_U) {
                        x->seg_attr[SEG_ES] = SEGATTR_U;
                        x->seg_limit[SEG_ES] = 0;
                        x->seg_base[SEG_ES] = 0;
                } else {
                        x->seg_attr[SEG_ES] = es_attr;
                        x->seg_limit[SEG_ES] = vmread(VMCS_GUEST_ES_LIMIT);
                        x->seg_base[SEG_ES] = vmread(VMCS_GUEST_ES_BASE);
                }

                x->seg_sel[SEG_SS] = vmread(VMCS_GUEST_SS_SEL);
                u32 ss_attr = vmread(VMCS_GUEST_SS_ACCESS_RIGHTS);
                if (ss_attr & SEGATTR_U) {
                        /* SS.DPL is always valid */
                        x->seg_attr[SEG_SS] = (SEGATTR_U | BITMASK(6, 5))
                                            & ss_attr;
                        x->seg_limit[SEG_SS] = 0;
                        x->seg_base[SEG_SS] = 0;
                } else {
                        x->seg_attr[SEG_SS] = ss_attr;
                        x->seg_limit[SEG_SS] = vmread(VMCS_GUEST_SS_LIMIT);
                        x->seg_base[SEG_SS] = vmread(VMCS_GUEST_SS_BASE);
                }

                x->seg_sel[SEG_DS] = vmread(VMCS_GUEST_DS_SEL);
                u32 ds_attr = vmread(VMCS_GUEST_DS_ACCESS_RIGHTS);
                if (ds_attr & SEGATTR_U) {
                        x->seg_attr[SEG_DS] = SEGATTR_U;
                        x->seg_limit[SEG_DS] = 0;
                        x->seg_base[SEG_DS] = 0;
                } else {
                        x->seg_attr[SEG_DS] = ds_attr;
                        x->seg_limit[SEG_DS] = vmread(VMCS_GUEST_DS_LIMIT);
                        x->seg_base[SEG_DS] = vmread(VMCS_GUEST_DS_BASE);
                }

                x->seg_sel[SEG_FS] = vmread(VMCS_GUEST_FS_SEL);
                u32 fs_attr = vmread(VMCS_GUEST_FS_ACCESS_RIGHTS);
                if (fs_attr & SEGATTR_U) {
                        x->seg_attr[SEG_FS] = SEGATTR_U;
                        x->seg_limit[SEG_FS] = 0;
                } else {
                        x->seg_attr[SEG_FS] = fs_attr;
                        x->seg_limit[SEG_FS] = vmread(VMCS_GUEST_FS_LIMIT);
                }
                x->seg_base[SEG_FS] = vmread(VMCS_GUEST_FS_BASE);

                x->seg_sel[SEG_GS] = vmread(VMCS_GUEST_GS_SEL);
                u32 gs_attr = vmread(VMCS_GUEST_GS_ACCESS_RIGHTS);
                if (gs_attr & SEGATTR_U) {
                        x->seg_attr[SEG_GS] = SEGATTR_U;
                        x->seg_limit[SEG_GS] = 0;
                } else {
                        x->seg_attr[SEG_GS] = gs_attr;
                        x->seg_limit[SEG_GS] = vmread(VMCS_GUEST_GS_LIMIT);
                }
                x->seg_base[SEG_GS] = vmread(VMCS_GUEST_GS_BASE);

                x->seg_sel[SEG_LDT] = vmread(VMCS_GUEST_LDTR_SEL);
                u32 ldt_attr = vmread(VMCS_GUEST_LDTR_ACCESS_RIGHTS);
                if (ldt_attr & SEGATTR_U) {
                        x->seg_attr[SEG_LDT] = SEGATTR_U;
                        x->seg_limit[SEG_LDT] = 0;
                        x->seg_base[SEG_LDT] = 0;
                } else {
                        x->seg_attr[SEG_LDT] = ldt_attr;
                        x->seg_limit[SEG_LDT] = vmread(VMCS_GUEST_LDTR_LIMIT);
                        x->seg_base[SEG_LDT] = vmread(VMCS_GUEST_LDTR_BASE);
                }
        }

        if ((regbits & REGBIT_CR3) && vm->use_ept
            && !(x->intercepts & INTERCEPT_TLB_FLUSHES)) {
                x->cr3 = vm->guest_cr3 = vmread(VMCS_GUEST_CR3);
                if (IS_MMU_MODE_PAE(vm->mmu_mode) && x->pdpte[0] != -1) {
                        x->pdpte[0] = vm->guest_pdpte[0] =
                                vmread(VMCS_GUEST_PDPTE0);
                        x->pdpte[1] = vm->guest_pdpte[1] =
                                vmread(VMCS_GUEST_PDPTE1);
                        x->pdpte[2] = vm->guest_pdpte[2] =
                                vmread(VMCS_GUEST_PDPTE2);
                        x->pdpte[3] = vm->guest_pdpte[3] =
                                vmread(VMCS_GUEST_PDPTE3);
                }
        }

        if (regbits & REGBIT_CR) {
                x->cr0 = vmcs_get_sim_cr0(vm);
                x->cr4 = vmcs_get_sim_cr4(vm);

                if (vm->use_tpr_limit)
                        x->tpr = get_virt_apic_tpr(vm);
        }

        if (regbits & REGBIT_MSRS) {
                /* system linkage and debug registers */
                x->ia32_sysenter_cs = vmread(VMCS_GUEST_IA32_SYSENTER_CS);
                x->ia32_sysenter_esp = vmread(VMCS_GUEST_IA32_SYSENTER_ESP);
                x->ia32_sysenter_eip = vmread(VMCS_GUEST_IA32_SYSENTER_EIP);

                /* debug registers */
                x->dr7 = vmread(VMCS_GUEST_DR7);
                x->ia32_dbgctl = (x->ia32_dbgctl & ~IA32_DBGCTL_GUEST_BITS)
                        | (vmread(VMCS_GUEST_IA32_DBGCTL)
                           & IA32_DBGCTL_GUEST_BITS);

                /* KERNEL_GS_BASE can change */
                if (vm->ia32e_guest) {
                        x->msr_kernel_gs_base = get_gmsr(
                                vm, I_MSR_KERNEL_GS_BASE);
                }
        }

        if (regbits & REGBIT_DESC_TABLES) {
                /* descriptor tables */
                x->gdt_base = vmread(VMCS_GUEST_GDTR_BASE);
                x->idt_base = vmread(VMCS_GUEST_IDTR_BASE);
                x->gdt_limit = vmread(VMCS_GUEST_GDTR_LIMIT);
                x->idt_limit = vmread(VMCS_GUEST_IDTR_LIMIT);
        }

        if (regbits & REGBIT_INTABILITY) {
                x->intability_state =
                        vmread(VMCS_GUEST_INTERRUPTIBILITY_STATE);
        }

        if (regbits & REGBIT_INTERCEPTS) {
                /* nothing */
        }

        if (regbits & REGBIT_OTHER) {
                /* misc [FIXME] */
                x->pending_debug_exc = vmread(VMCS_GUEST_PEND_DBG_EXC);
        }

        x->insync |= regbits;
}

static void
x_fetch_state_from_vm_cpu(void *the_vm, int cpu)
{
        vm_t *vm = the_vm;
        if (vm->cpu == -1)
                return;

        u64 old_vmcs = activate_vmcs(vm);
        get_regs_from_vmcs(vm, REGBIT_ALL);
        vmclear(vm->vmcs_phys);

        if (gi.has_svmcs) {
                vmclear(vm->linked_vmcs_phys);
        }
        vm->launched = false;
        if (old_vmcs != -1 && old_vmcs != vm->vmcs_phys
            && old_vmcs != vm->linked_vmcs_phys)
                vmptrld(old_vmcs);

        /* perform a full VMCS reload on the new CPU */
        vm->dirty = REGBIT_ALL;
        vm->cpu = -1;
        vm->cs = NULL;
}

static void
fetch_state_from_vm_cpu(vm_t *vm)
{
        int err = run_on_cpu(vm->cpu, x_fetch_state_from_vm_cpu, (void *)vm);
        if (err)
                printm("run_on_cpu failed with error %d\n", err);
}

static inline void
fetch_state_from_current_cpu(vm_t *vm)
{
        x_fetch_state_from_vm_cpu(vm, vm->cpu);
}

static void
get_regs(vm_t *vm, u32 regbits)
{
        if (vm->cpu == -1)
                return;

        unsigned int ipl;
        int cpu = wrap_get_cpu(&ipl);

        if (cpu != vm->cpu) {
                wrap_put_cpu(ipl);
                fetch_state_from_vm_cpu(vm);
        } else {
                activate_vmcs(vm);
                get_regs_from_vmcs(vm, regbits);
                wrap_put_cpu(ipl);
        }
}

/* update VMCS settings related to the direct RDTSC mode */
static void
vmcs_update_direct_rdtsc_mode(vm_t *vm)
{
        vmcs_update_proc_ctls(vm);   /* RDTSC exiting and use TSC offsetting */
        vmcs_update_proc2_ctls(vm);  /* enable RDTSCP and use TSC scaling */

        if (gi.has_tsc_scaling)
                vmwrite(VMCS_TSC_MULTIPLIER, BIT(48 - vm->tsc_shift));

        /* grant/deny IA32_TSC direct read */
        if (gi.has_msr_bitmap) {
                bool r_intercept = !vm->use_direct_rdtsc;
                bool w_intercept = true;
                modify_msr_bitmap(vm, MSR_IA32_TIME_STAMP_COUNTER,
                                  r_intercept, w_intercept);
        }
}

static void
setup_direct_rdtsc(vm_t *vm)
{
        vmxmon_t *x = vm->com;

        vm->use_direct_rdtsc =
                (x->active_vmxmon_features & VMXMON_FEATURE_DIRECT_RDTSC)
                && !x->vmx_non_root_mode && gi.has_tsc_scaling;

        if (x->active_vmxmon_features & VMXMON_FEATURE_TSC_SHIFT)
                vm->tsc_shift = x->tsc_shift;
        else
                vm->tsc_shift = mp.tsc_shift;

        vmcs_update_direct_rdtsc_mode(vm);

        /* save/restore IA32_TSC_AUX when necessary */
        set_guest_ia32_tsc_aux(vm);

        /* Save TSC at VMEXIT if necessary */
        if (vm->use_direct_rdtsc) {
                if (!gmsr_active(vm, I_MSR_TSC))
                        activate_guest_store_only_msr(vm, I_MSR_TSC);
        } else {
                if (gmsr_active(vm, I_MSR_TSC))
                        deactivate_guest_store_only_msr(vm, I_MSR_TSC);
        }
}

/* Called as close as possible to entering non-root.
   Returns host TSC value at entry or 0 if direct RDTSC is not in use. */
static u64
load_guest_tsc(vm_t *vm)
{
        vmxmon_t *x = vm->com;

        if (!vm->use_direct_rdtsc)
                return 0;

        /* set guest TSC to match tsc_start_value + tsc_cycles_spent */
        u64 tsc = readtsc();
        u64 tsc_offset = x->tsc_start_value + x->tsc_cycles_spent;
        tsc_offset -= tsc >> vm->tsc_shift;

        vmwrite(VMCS_TSC_OFFSET, tsc_offset);
        return tsc;
}

/* Called at vmexit to account for elapsed TSC ticks.
   Supposed to be called after calculation of elapsed steps. */
static void
update_elapsed_tsc_ticks(vm_t *vm, u64 tsc_start)
{
        if (!vm->use_direct_rdtsc)
                return;

        /* Update spent TSC cycles only if at least one instruction executed
           to filter out early exits. */
       if (vm->use_perf_counters && (vm->elapsed == 0))
                return;

        u64 tsc_end = get_gmsr(vm, I_MSR_TSC);

        /* Scale TSC start and TSC end. Note: scaling must be applied before
         * TSC spent calculation to avoid potential lose of a cycle. */
        tsc_start >>= vm->tsc_shift;
        tsc_end >>= vm->tsc_shift;

        u64 tsc_spent = tsc_end - tsc_start;
        vm->com->tsc_cycles_spent += tsc_spent;
}

/************************************************************************/
/*      performance counter handling                                    */
/************************************************************************/

/* returns #elapsed steps */
static inline unsigned
get_elapsed_steps(vm_t *vm)
{
        /* we truncate to 32-bits to avoid a semi-complex sign extension */
        s32 res = get_gmsr(vm, I_MSR_STEP_CTR);
        u32 steps = vm->steps + res;

        /* vmxenter is never counted if there was an entry error */
        if (steps != 0 && vm->cs->pmc.counts_vmxenter)
                steps--;
        return steps;
}

static inline int
timer_setup(vm_t *vm)
{
        if (!vm->use_perf_counters)
                return 0;

        vmxmon_t *x = vm->com;
        s64 n = x->step_limit - x->steps;
        bool can_run = n > x->step_threshold;
        bool can_trace = x->dbg_use_stepi && n != 0 && gi.has_monitor_trap_flag;

        if (!can_run && !can_trace) {
                set_vmret(vm, VMRET_STEP_TIMER);
                get_regs_from_vmcs(vm, REGBIT_ALL);
                return 1;
        }

        bool do_trace = !can_run;

        /* update MONITOR_TRAP_FLAG bit in PROC */
        if (do_trace != vm->monitor_trap_flag) {
                vm->monitor_trap_flag = do_trace;
                vmcs_update_proc_ctls(vm);
        }

        if (do_trace) {
                /* needed to work around performance counter bug */
                x->ip = vmread(VMCS_GUEST_RIP);
        } else {
                /* subtract NMI skid from performance counter */
                n -= x->step_threshold;
        }

        vm->steps = n;
        u64 val = (-n & vm->cs->pmc.mask);
        set_gmsr(vm, I_MSR_STEP_CTR, val);
        return 0;
}

static inline void
handle_step_counter(vm_t *vm)
{
        if (!vm->use_perf_counters)
                return;

        vmxmon_t *x = vm->com;
        unsigned elapsed = get_elapsed_steps(vm);

        BUMP_N(PERFCTR_SIM_STEPS, elapsed);
        vm->elapsed = elapsed;
        x->steps += elapsed;
}


/************************************************************************/
/*	VMX setup                   					*/
/************************************************************************/

static inline u8
get_thread_num(const cpu_state_t *cs)
{
        u32 eax, ebx, ecx, edx, max_cores;
        u32 max_log_cpus = (cpuid_ebx(1) >> 16) & 0xff;
        u8 thread = 0;

        cpuid_cnt(4,0, &eax, &ebx, &ecx, &edx);
        max_cores = (eax >> 26) + 1;

        if (max_log_cpus > max_cores)
                thread = (wrap_apic_read(cs->lapic_base, APIC_ID) >> 24) & 1;
        return thread;
}

static int
make_vmcs_page(mpage_t *mpage, bool shadow)
{
        char *p = alloc_mpage(mpage);
        if (!p)
                return 1;

        *(u32*)p = gi.vmx_rev | (shadow ? 1 << 31 : 0);
#ifndef VMCS_DEBUG
        /* Do not unmap vmcs page when VMCS_DEBUG is active. It is required
           to make sure that the page will be saved in kernel memory dump. */
        mpage_kunmap(mpage);
#endif
        return 0;
}


static bool
cpu_has_vmx(void)
{
        u32 val = cpuid_ecx(1);
        return (val & CPUID_1_ECX_VMX);
}

static bool
cpu_has_nx(void)
{
        u32 val = cpuid_eax(0x80000000ul);
        if (val < 0x80000001ul)
                return false;
        val = cpuid_edx(0x80000001ul);
        return (val & CPUID_80000001_EDX_NX);
}

static bool
cpu_has_1gb_pages(void)
{
        u32 val = cpuid_eax(0x80000000ul);
        if (val < 0x80000001ul)
                return false;
        val = cpuid_edx(0x80000001ul);
        return (val & CPUID_80000001_EDX_1GB_PAGES);
}

static bool
cpu_has_rdtscp(void)
{
        u32 val = cpuid_eax(0x80000000ul);
        if (val < 0x80000001ul)
                return false;
        val = cpuid_edx(0x80000001ul);
        return (val & CPUID_80000001_EDX_RDTSCP);
}

static int
cpu_max_physaddr(void)
{
        u32 val = cpuid_eax(0x80000000ul);
        if (val < 0x80000008ul) {
                if (cpuid_edx(1) & CPUID_1_EDX_PAE)
                        return 36;
                else
                        return 32;
        }
        return cpuid_eax(0x80000008ul) & 0xff;
}

/* Returns true if the host is affected by "A Page Fault May Not be
   Generated When the PS bit is set to "1" in a PML4E or PDPTE" erratum.
   For reference please see "Intel® Xeon® Processor 5500 Series" and
   "Intel® Core™ i7-600, i5-500, i5-400 and i3-300 Mobile Processor Series"
   specification updates */
static bool
cpu_has_1gb_pages_bug(void)
{
        u32 val = cpuid_eax(1);
        /* core i7-600, i5-500, i5-400, i3-300 */
        bool is_core = ((FIELD(val, 27, 20) == 0)
                        && (FIELD(val, 19, 16) == 2)   /* 0b0010 */
                        && (FIELD(val, 13, 12) == 0)
                        && (FIELD(val, 11, 8) == 6)    /* 0b0110 */
                        && (FIELD(val, 7, 4) == 5));   /* 0b0101 */
        /* xeon 5500 series */
        bool is_xeon = ((FIELD(val, 27, 20) == 0)
                        && (FIELD(val, 19, 16) == 1)   /* 0b0001 */
                        && (FIELD(val, 13, 12) == 0)
                        && (FIELD(val, 11, 8) == 6)    /* 0b0110 */
                        && (FIELD(val, 7, 4) == 10));  /* 0b1010 */
        return !cpu_has_1gb_pages() && (is_core || is_xeon);
}

static bool
cpu_has_avx(void)
{
        u32 val = cpuid_ecx(1);
        return (val & CPUID_1_ECX_AVX);
}

static bool
cpu_has_xsave(void)
{
        u32 val = cpuid_ecx(1);
        return (val & CPUID_1_ECX_XSAVE);
}

static u64
cpu_get_xcr0_mask(void)
{
        unsigned eax, ebx, ecx, edx;
        cpuid_cnt(0xd, 0x0, &eax, &ebx, &ecx, &edx);
        return (u64)edx << 32 | eax;
}

/* retrieve Intel® MPX BNDCSR XSAVE offset */
static u64
cpu_get_xsave_bndcsr_offs(void)
{
        unsigned eax, ebx, ecx, edx;
        cpuid_cnt(0xd, 4 /* BNDCSR */, &eax, &ebx, &ecx, &edx);
        return ebx;
}

static bool
cpu_has_fsgsbase(void)
{
        u32 val = cpuid_eax(0);
        if (val < 0x7)
                return false;
        val = cpuid_ebx(0x7);
        return (val & CPUID_7_EBX_FSGSBASE);
}

static bool
cpu_is_hyperv(void)
{
        if (!(cpuid_ecx(1) & BIT(31)))
                return false;

        u32 eax, ebx, ecx, edx;
        cpuid_cla(0x40000000ul, &eax, &ebx, &ecx, &edx);
        if (ebx != 0x7263694dul || ecx != 0x666f736ful || edx != 0x76482074ul)
                return false;

        return cpuid_eax(0x40000001ul) == 0x31237648ul;
}

/* Returns true if the host is affected by 16 bit TSS BIOS bug.
   Review comments of bios_bug_prevents_execution function for more details.
   Most likely hosts with SandyBridge and newer CPUs are not affected by the bug
   so disabling workaround for such systems. */
static bool
host_has_16_bit_tss_bios_bug(void)
{
        /* It is hard to detect CPU microarchitecture based on Model and Family.
           Intel® AVX feature is used to detect the generation of the host CPU
           because it was introduced in SandyBridge processor. */
        return !cpu_has_avx();
}

static bool
cpu_has_lzcnt(void)
{
        u32 val = cpuid_eax(0x80000000ul);
        if (val < 0x80000001ul)
                return false;
        val = cpuid_ecx(0x80000001ul);
        return (val & CPUID_80000001_ECX_LZCNT);
}

static bool
cpu_has_bmi1(void)
{
        u32 val = cpuid_eax(0);
        if (val < 0x7)
                return false;
        val = cpuid_ebx(0x7);
        return (val & CPUID_7_EBX_BMI1);
}

static bool
cpu_has_fpu_csds_deprecation(void)
{
        u32 val = cpuid_eax(0);
        if (val < 0x7)
                return false;
        val = cpuid_ebx(0x7);
        return (val & CPUID_7_EBX_FPU_CSDS_DEPRECATION);
}

/* PEBS - precise-event-based sampling */
static bool
cpu_has_pebs(void)
{
        /* DS - Debug Store feature? */
        u32 eax, ebx, ecx, edx;
        cpuid_cla(1, &eax, &ebx, &ecx, &edx);
        if (!(edx & BIT(21)))
                return false;

        u64 v = rdmsr(MSR_IA32_MISC_ENABLE);
        return !(v & BIT(12));
}

static bool
cpu_has_pku(void)
{
        u32 val = cpuid_eax(0);
        if (val < 0x7)
                return false;
        val = cpuid_ecx(0x7);
        return (val & CPUID_7_ECX_PKU);
}

static bool
ospke_enabled(void)
{
        u32 val = cpuid_eax(0);
        if (val < 0x7)
                return false;
        val = cpuid_ecx(0x7);
        return (val & CPUID_7_ECX_OSPKE);
}

static bool
osxsave_enabled(void)
{
        u32 val = cpuid_ecx(1);
        return (val & CPUID_1_ECX_OSXSAVE);
}

static bool
vmx_feature_available(void)
{
        /* make sure VMX is present */
        if (!cpu_has_vmx()) {
                printm("This machine does not support VMX\n");
                return false;
        }
        u64 fctrl = rdmsr(MSR_IA32_FEATURE_CTRL);
        bool locked = fctrl & BIT(0);
        bool vmx_enabled = fctrl & BIT(2);

        if (locked && !vmx_enabled) {
                printm("VMX support has not been enabled by the BIOS.\n");
                return false;
        }
        return true;
}

static bool
cpu_enable_vmx_feature(void)
{
        u64 fctrl = rdmsr(MSR_IA32_FEATURE_CTRL);
        if (!(fctrl & BIT(0))) {
                fctrl |= BIT(2) | BIT(0);
                printm("VMX feature enabled"
                       " (the BIOS should have done this!)\n");
                wrmsr(MSR_IA32_FEATURE_CTRL, fctrl);
                fctrl = rdmsr(MSR_IA32_FEATURE_CTRL);
        }
        if (!(fctrl & BIT(2))) {
                printm("VT-x support has not been enabled by the BIOS.\n");
                return false;
        }
        return true;
}

/* Ensure VMX is enabled on the CPU. */
static bool
vmx_on(cpu_state_t *cs)
{
        /* Fast path: VMX already enabled. */
        if (likely(cs->vmx_is_on))
                return true;

        /* turn on VMX feature (if firmware hasn't bothered) */
        if (!cpu_enable_vmx_feature())
                return false;

        u64 vmxon_pa = mpage_phys(&cs->vmxon_page);

        /* turn on CR4[VMX] */
        unsigned long cr4 = get_cr4();
        bool cr4_vmxe = (cr4 & CR4_VMXE);
        if (!cr4_vmxe)
                cr4_set_bits(CR4_VMXE);

        /* enter VMX root operation */
        cs->our_vmxon = true;
        if (vmxon(vmxon_pa)) {
                printm("CPU already in VMX root operation\n");
                cs->our_vmxon = false;
        }
        cs->org_cr4_vmxe = cr4_vmxe;
        cs->vmx_is_on = true;
        return true;
}

static void
vmx_off(int cpu)
{
        cpu_state_t *cs = gi.cpu_state[cpu];
        if (cs && cs->vmx_is_on) {
                cs->vmx_is_on = false;

                unsigned long cr4 = get_cr4();
                if (!(cr4 & CR4_VMXE)) {
                        printm("CR4[VMXE] already cleared!\n");
                        return;
                }
                if (cs->our_vmxon)
                        vmxoff();
                if (!cs->org_cr4_vmxe)
                        cr4_clear_bits(get_cr4(), CR4_VMXE);
        }
}

static void
x_enable_perf_counting(void *unused, int cpu) 
{
        cpu_state_t *cs = gi.cpu_state[cpu];

        /* Clear any stale PMC overflow indication. */
        cs->pmc.get_and_clear_overflow(cs);

        /* On Windows, the vector is saved/restored
         * at vmentry/vmexit */
        if (LINUX && mp.use_nmi) {
                /* The NMI handler is installed at this point; further
                   unblocking is the responsibility of the NMI handler. */
                wrap_apic_write(cs->lapic_base, APIC_LVTPC, APIC_DM_NMI);
        }
}

static void
enable_perf_counting(cpu_state_t *cs)
{
        /* On hardware with IA32_FIXED_CTR0 (i.e. APM version >= 2),
           try to use an OS-provided performance counter. */
        if (gi.apm_version >= 2 && !cs->use_apm1) {
                /* This call is expected to fail on old
                   kernels and on Windows. */
                cs->perf_event = create_cpu_perf_event(cs->cpu);
        }
        if (!cs->perf_event)
                run_on_cpu(cs->cpu, x_enable_perf_counting, NULL);
}

static void
reenable_perf_counting(cpu_state_t *cs)
{
        if (likely(cs->use_apm1 == mp.use_apm1))
                return;

        pctr_setup_cpu_info(cs);

        if (cs->perf_event) {
                release_cpu_perf_event(cs->perf_event);
                cs->perf_event = NULL;
        }

        enable_perf_counting(cs);
}

static int
prepare_cpu(vm_t *vm, int cpu, unsigned *ipl)
{
        cpu_state_t *cs = gi.cpu_state[cpu];
        if (!cs) {
                /* This can occur while the CPU is being suspended. */
                set_vmret_(vm, VMRET_NOP);
                wrap_put_cpu(*ipl);
                return 1;
        }
        if (!vmx_on(cs)) {
                set_vmret_(vm, VMRET_VMXON_FAILURE);
                wrap_put_cpu(*ipl);
                return 1;
        }
        reenable_perf_counting(cs);
        return 0;
}

static inline int
check_vm_instr_err(vm_t *vm)
{
        u32 err = vmread(VMCS_VM_INSTR_ERR);

        /* detect and report any detected VM instruction errors
           (e.g. usage of invalid VMCS register) */
        if (err != 0 && err != vm->last_vmcs_vm_instr_err) {
                vm->last_vmcs_vm_instr_err = err;

                /* VMXON in vmx-root operation can be caused by another agent */
                if (err == 15)
                        return 0;

                printm("VMCS_VM_INSTR_ERR: %d\n", err);
                set_vmret(vm, VMRET_INTERNAL_ERROR);
                return 1;
        }
        return 0;
}


/* Returns NULL if ok, and an error description if not ok. */
static inline const char *
_vmcs_assert_static_host_state(void)
{
#define ASSERT_EQUAL(a, b, desc) if ((a) != (b)) return desc
        u16 seg, tss_sel;
        descriptor_t gdt_desc;
        descriptor_t idt_desc;

        /* save GDT, IDT and LDT selector */
        get_gdt_desc(&gdt_desc);
        ASSERT_EQUAL(vmread(VMCS_HOST_GDTR_BASE), gdt_desc.base, "GDTR base");
        get_idt_desc(&idt_desc);
        ASSERT_EQUAL(vmread(VMCS_HOST_IDTR_BASE), idt_desc.base, "IDTR base");

        /* CR registers */
        ASSERT_EQUAL(vmread(VMCS_HOST_CR3), get_cr3(), "CR3");
        ASSERT_EQUAL(vmread(VMCS_HOST_CR4), get_cr4(), "CR4");

        /* task register */
        tss_sel = get_tr();
        ASSERT_EQUAL(vmread(VMCS_HOST_TR_SEL), tss_sel, "TR selector");
        ASSERT_EQUAL(vmread(VMCS_HOST_TR_BASE), get_tr_base(), "TR base");

        /* segment registers */
        seg = get_cs();
        ASSERT_EQUAL(vmread(VMCS_HOST_CS_SEL), seg, "CS selector");
        seg = get_ss();
        ASSERT_EQUAL(vmread(VMCS_HOST_SS_SEL), seg, "SS selector");
        seg = get_ds();
        ASSERT_EQUAL(vmread(VMCS_HOST_DS_SEL), seg & ~7, "DS selector");
        seg = get_es();
        ASSERT_EQUAL(vmread(VMCS_HOST_ES_SEL), seg & ~7, "ES selector");

        /* Segment stuff is always reloaded */

        /* misc */
        ASSERT_EQUAL(vmread(VMCS_HOST_IA32_SYSENTER_CS),
                     rdmsr(MSR_IA32_SYSENTER_CS), "IA32_SYSENTER_CS");
        ASSERT_EQUAL(vmread(VMCS_HOST_IA32_SYSENTER_ESP),
                     rdmsr(MSR_IA32_SYSENTER_ESP), "IA32_SYSENTER_ESP");
        ASSERT_EQUAL(vmread(VMCS_HOST_IA32_SYSENTER_EIP),
                     rdmsr(MSR_IA32_SYSENTER_EIP), "IA32_SYSENTER_EIP");
        return NULL;
}

static inline int
vmcs_assert_static_host_state(vm_t *vm)
{
        if (ENABLE_STATIC_HOST_STATE_ASSERT) {
                const char *str = _vmcs_assert_static_host_state();
                if (!str)
                        return 0;
                printm("Static state check failure: %s\n", str);
                set_vmret(vm, VMRET_INTERNAL_ERROR);
                return 1;
        }
        return 0;
}

static void
save_mutable_host_state(vm_t *vm)
{
        update_host_perf_counter_state(vm);

        vmwrite(VMCS_HOST_CR0, get_cr0());
        vmwrite(VMCS_HOST_CR3, get_cr3());

        /* swapgs is invalid in 32-bit mode - KERNEL_GS_BASE is safe */
        if (vm->ia32e_guest)
                set_hmsr(vm, I_MSR_KERNEL_GS_BASE, rdmsr(MSR_KERNEL_GS_BASE));
}

static void
vmcs_save_static_host_state(vm_t *vm)
{
        host_state_t *h = &vm->host_state;

        /* save GDT, IDT and LDT selector */
        get_gdt_desc(&h->gdt_desc);
        get_idt_desc(&h->idt_desc);
        vmwrite(VMCS_HOST_GDTR_BASE, h->gdt_desc.base);
        vmwrite(VMCS_HOST_IDTR_BASE, h->idt_desc.base);

        /* CR registers */
        uintptr_t cr4 = get_cr4();
        vm->host_state.cr4 = cr4;
        vmwrite(VMCS_HOST_CR4, cr4);

        /* task register */
        h->tss_sel = get_tr();
        vmwrite(VMCS_HOST_TR_SEL, h->tss_sel);
        vmwrite(VMCS_HOST_TR_BASE, get_tr_base());

        /* segment registers */
        vmwrite(VMCS_HOST_CS_SEL, get_cs());
        vmwrite(VMCS_HOST_SS_SEL, get_ss());
        vmwrite(VMCS_HOST_DS_SEL, 0);
        vmwrite(VMCS_HOST_ES_SEL, 0);
        vmwrite(VMCS_HOST_FS_SEL, 0);
        vmwrite(VMCS_HOST_GS_SEL, 0);
        vmwrite(VMCS_HOST_FS_BASE, rdmsr(MSR_FS_BASE));
        vmwrite(VMCS_HOST_GS_BASE, rdmsr(MSR_GS_BASE));

        /* misc */
        vmwrite(VMCS_HOST_IA32_SYSENTER_CS, rdmsr(MSR_IA32_SYSENTER_CS));
        vmwrite(VMCS_HOST_IA32_SYSENTER_ESP, rdmsr(MSR_IA32_SYSENTER_ESP));
        vmwrite(VMCS_HOST_IA32_SYSENTER_EIP, rdmsr(MSR_IA32_SYSENTER_EIP));

        if (gi.has_bndcfgs_support && (h->xcr0 & XSTATE_MPX))
                h->ia32_bndcfgs = rdmsr(MSR_IA32_BNDCFGS);

        /* vm->use_xsave cannot be used here since it might change later on */
        if (gi.has_xsave)
                h->xcr0 = get_xcr0();

        /* return trampoline */
        extern char vmx_return[];   /* assembly export */
        vmwrite(VMCS_HOST_RIP, (uintptr_t)vmx_return);

        if (gi.has_rdtscp)
                vm->host_tsc_aux = rdmsr(MSR_IA32_TSC_AUX);

        vm->host_ia32_star = rdmsr(MSR_STAR);
        vm->host_ia32_fmask = rdmsr(MSR_IA32_FMASK);
}

static void
vmcs_setup_ctrl_state(vm_t *vm)
{
        vmcs_set_pin_ctrls(vm);

        /* should this be done somewhere else too? */
        vmwrite(VMCS_VMENTRY_INT_INFO_FIELD, 0);
        vmwrite(VMCS_VMENTRY_EXC_ECODE, 0);
        vmwrite(VMCS_VMENTRY_INST_LEN, 0);

        vmwrite(VMCS_GUEST_ACTIVITY_STATE, 0);

        if (gi.has_vpid)
                vmwrite(VMCS_VPID, vm->vpid);
        if (gi.has_msr_bitmap)
                vmwrite(VMCS_MSR_BITMAP_ADDR,
                          mpage_phys(&vm->msr_bitmap_page));

        if (gi.proc_based2_1_allowed & PROC2_ENCLS_EXITING)
                vmwrite(VMCS_ENCLS_EXITING_BITMAP, -1ULL);
}
static void
dbg_vmcs_save_host_msrs_on_exit(vm_t *vm)
{
#ifdef VMCS_DEBUG
        if (!gmsr_active(vm, I_MSR_TSC))
                activate_guest_store_only_msr(vm, I_MSR_TSC);
#endif
}

static void
prepare_vmcs(vm_t *vm)
{
        if (!vm->initial_vmclear_done) {
                vmclear(vm->vmcs_phys);
                if (gi.has_svmcs)
                        vmclear(vm->linked_vmcs_phys);
                vm->initial_vmclear_done = true;
        }
        vmptrld(vm->vmcs_phys);

        vmcs_save_static_host_state(vm);
        vmcs_setup_ctrl_state(vm);

        setup_msr_handling(vm);

        dbg_vmcs_save_host_msrs_on_exit(vm);

        vm->dirty = REGBIT_ALL;
        vm->entry_work |= 0
                | EW_RELOAD_VMEXIT_CTRLS
                | EW_CR3_RELOAD
                | EW_EPT_USAGE_SWITCH
                | EW_FLUSH_VPID
                | EW_FLUSH_EPT
                | EW_RELOAD_APIC_PAGE;

        mark_all_guest_eptps_for_flush(vm);
}

/************************************************************************/
/*	exception injection   						*/
/************************************************************************/

static inline void
vmx_inject_hw_exception(vm_t *vm, u8 num)
{
        vmxmon_t *x = vm->com;

        if (vm->entry_work & EW_INJECT) {
                printm("Double injection detected!\n");
                x->debug_stop = 1;
        }
        vm->entry_work |= EW_INJECT;
        x->injection.valid = 1;
        x->injection.type = 3;            /* hw exception */
        x->injection.vector = num;        /* #NM */
        x->injection.deliver_ec = 0;
        x->injection.instr_len = 0;
}

/* #PF injection (only callable from the inner loop) */
int
vmx_inject_pf(vm_t *vm, u64 cr2, u16 ecode)
{
        vmxmon_t *x = vm->com;

        BUMP(PERFCTR_GUEST_EXC_PF);

        if (vm->entry_work & EW_INJECT) {
                /* Could happen if event injection caused #PF.
                   For example, if stack is not writable. */
                return VMRET_EMULATE;
        }

        vm->entry_work |= EW_INJECT;
        x->cr2 = cr2;
        x->injection.valid = 1;
        x->injection.type = 3;             /* hardware exception */
        x->injection.vector = 14;          /* #PF */
        x->injection.deliver_ec = 1;
        x->injection.ecode = ecode;
        x->injection.instr_len = 0;
        return VMRET_AGAIN;
}


/************************************************************************/
/*	VMX execution   						*/
/************************************************************************/

static inline void
assert_vmcs(vm_t *vm)
{
        if (vmptrst() != vm->vmcs_phys)
                printm("VMCS assertion failed "
                       "(interference with foreign hypervisor?)\n");
}

static const u64 memory_type_writeback = 6;
static const u64 four_level_walk_length = (4-1) << 3;

static inline u64
get_eptp(vm_t *vm)
{
        u64 eptp = tphys_get_eptp(vm->pspace);
        u64 walk_length = encoded_eptp_walk_length(vm->pspace);
        return eptp | memory_type_writeback | walk_length;
}

static inline u64
get_nested_eptp(vm_t *vm)
{
        u64 eptp = guest_ept_get_eptp(vm);
        return eptp | memory_type_writeback | four_level_walk_length;
}

static inline void
handle_tlb_flushes_no_ept(vm_t *vm)
{
        vmxmon_t *x = vm->com;

        if (vm->use_ept)
                return;

        /* handle tlb flushes */
        if (x->cr3_flush) {
                if (!x->cr3_flush_keep_global)
                        clear_utable(&x->invlpg_table);
                tlb_flush(vm, x->cr3_flush_keep_global);
                x->cr3_flush = false;
        }

        /* handle page invalidations */
        if (!utable_empty(&x->invlpg_table)) {
                unsigned i, end;
                get_utable_range_and_clear(&x->invlpg_table, &i, &end,
                                           MAX_AUX_ENTRIES);
                for (; i < end; i++)
                        tlb_invlpg(vm, vm->aux_table[i]);
        }
}

static inline void
handle_tlb_flushes_ept(vm_t *vm)
{
        vmxmon_t *x = vm->com;
        if (!vm->use_ept)
                return;

        if (!vm->use_vpid) {
                /* nothing to do, just clear flush requests */
                x->cr3_flush = false;
                clear_utable(&x->invlpg_table);
                return;
        }

        if (x->cr3_flush) {
                if (!x->cr3_flush_keep_global)
                        clear_utable(&x->invlpg_table);
                unsigned type = x->cr3_flush_keep_global ?
                        INVVPID_VPID_LOCAL_CONTEXT : INVVPID_VPID_CONTEXT;
                invvpid(type, vm->vpid, 0);
                x->cr3_flush = false;
        }

        if (vm->use_ept && !utable_empty(&x->invlpg_table)) {
                unsigned i, end;
                get_utable_range_and_clear(&x->invlpg_table, &i, &end,
                                           MAX_AUX_ENTRIES);
                for (; i < end; i++)
                        invvpid(INVVPID_VPID_ADDR,
                                  vm->vpid, vm->aux_table[i]);
        }
}

static inline void
handle_dirty_regs(vm_t *vm)
{
        unsigned dirty = vm->dirty;     /* secure variant of x->dirty */
        vmxmon_t *x = vm->com;

        if (!dirty)
                return;

        if (dirty & REGBIT_IP) {
                vmwrite(VMCS_GUEST_RIP, x->ip);
        }

        if (dirty & REGBIT_IP_INFO) {
                vmwrite(VMCS_GUEST_CS_SEL, x->seg_sel[SEG_CS]);
                vmwrite(VMCS_GUEST_CS_LIMIT, x->seg_limit[SEG_CS]);
                vmwrite(VMCS_GUEST_CS_BASE, x->seg_base[SEG_CS]);
                vmwrite(VMCS_GUEST_CS_ACCESS_RIGHTS, x->seg_attr[SEG_CS]);
        }

        if (dirty & REGBIT_SP) {
                vmwrite(VMCS_GUEST_RSP, x->gprs[GPR_SP]);
        }

        if (dirty & REGBIT_EFLAGS) {
                vmwrite(VMCS_GUEST_RFLAGS, x->rflags);
        }

        if (dirty & REGBIT_SEGS) {
                /* BIOS bug workaround */
                u16 tr_sel = x->seg_sel[SEG_TR];
                u32 tr_attr = x->seg_attr[SEG_TR];
                vm->tss_type_optimization = (tr_sel == 0 
                                             && (tr_attr & 0xf) == 3);
                if (vm->tss_type_optimization)
                        tr_attr = (tr_attr & ~0xf) | 11;

                /* segment selectors */
                vmwrite(VMCS_GUEST_ES_SEL, x->seg_sel[SEG_ES]);
                vmwrite(VMCS_GUEST_SS_SEL, x->seg_sel[SEG_SS]);
                vmwrite(VMCS_GUEST_DS_SEL, x->seg_sel[SEG_DS]);
                vmwrite(VMCS_GUEST_FS_SEL, x->seg_sel[SEG_FS]);
                vmwrite(VMCS_GUEST_GS_SEL, x->seg_sel[SEG_GS]);
                vmwrite(VMCS_GUEST_LDTR_SEL, x->seg_sel[SEG_LDT]);
                vmwrite(VMCS_GUEST_TR_SEL, tr_sel);

                /* segment limits */
                vmwrite(VMCS_GUEST_ES_LIMIT, x->seg_limit[SEG_ES]);
                vmwrite(VMCS_GUEST_SS_LIMIT, x->seg_limit[SEG_SS]);
                vmwrite(VMCS_GUEST_DS_LIMIT, x->seg_limit[SEG_DS]);
                vmwrite(VMCS_GUEST_FS_LIMIT, x->seg_limit[SEG_FS]);
                vmwrite(VMCS_GUEST_GS_LIMIT, x->seg_limit[SEG_GS]);
                vmwrite(VMCS_GUEST_LDTR_LIMIT, x->seg_limit[SEG_LDT]);
                vmwrite(VMCS_GUEST_TR_LIMIT, x->seg_limit[SEG_TR]);

                /* segment access rights */
                vmwrite(VMCS_GUEST_ES_ACCESS_RIGHTS, x->seg_attr[SEG_ES]);
                vmwrite(VMCS_GUEST_SS_ACCESS_RIGHTS, x->seg_attr[SEG_SS]);
                vmwrite(VMCS_GUEST_DS_ACCESS_RIGHTS, x->seg_attr[SEG_DS]);
                vmwrite(VMCS_GUEST_FS_ACCESS_RIGHTS, x->seg_attr[SEG_FS]);
                vmwrite(VMCS_GUEST_GS_ACCESS_RIGHTS, x->seg_attr[SEG_GS]);
                vmwrite(VMCS_GUEST_LDTR_ACCESS_RIGHTS, x->seg_attr[SEG_LDT]);
                vmwrite(VMCS_GUEST_TR_ACCESS_RIGHTS, tr_attr);

                /* base values */
                vmwrite(VMCS_GUEST_ES_BASE, x->seg_base[SEG_ES]);
                vmwrite(VMCS_GUEST_SS_BASE, x->seg_base[SEG_SS]);
                vmwrite(VMCS_GUEST_DS_BASE, x->seg_base[SEG_DS]);
                vmwrite(VMCS_GUEST_FS_BASE, x->seg_base[SEG_FS]);
                vmwrite(VMCS_GUEST_GS_BASE, x->seg_base[SEG_GS]);
                vmwrite(VMCS_GUEST_LDTR_BASE, x->seg_base[SEG_LDT]);
                vmwrite(VMCS_GUEST_TR_BASE, x->seg_base[SEG_TR]);
        }

        if (dirty & REGBIT_CR) {
                vm->entry_work |= EW_RELOAD_DIRTY_CR0 | EW_RELOAD_DIRTY_CR4;
                update_tpr(vm);
                /* cr2 is always saved, cr3 is handled separately */
        }

        if (dirty & REGBIT_MSRS) {
                /* system linkage */
                vmwrite(VMCS_GUEST_IA32_SYSENTER_CS, x->ia32_sysenter_cs);
                vmwrite(VMCS_GUEST_IA32_SYSENTER_ESP, x->ia32_sysenter_esp);
                vmwrite(VMCS_GUEST_IA32_SYSENTER_EIP, x->ia32_sysenter_eip);

                /* debug registers */
                u64 dr7 = (x->dr7 & 0xffff23ff) | 0x0400;

                /* Certain BIOSes can't handle DR7.GD bit correctly.
                 * As a workaround vmxmon never sets this bit.
                 *
                 * It is safe to disable debug-register protection in DR7
                 * because all MOV_DR instructions are intercepted:
                 * PROC_MOV_DR_EXITING is always set. */
                dr7 &= ~BIT(13);

                vmwrite(VMCS_GUEST_DR7, dr7);
                vmwrite(VMCS_GUEST_IA32_DBGCTL,
                          (vm->cs->use_smm_perf_freeze ? BIT(14) : 0)
                          | (x->ia32_dbgctl & IA32_DBGCTL_GUEST_BITS));

                /* Intel® MPX control register */
                if (vm->use_bndcfgs)
                        vmwrite(VMCS_GUEST_IA32_BNDCFGS, x->ia32_bndcfgs);

                /* handle MSR_EFER (long mode selection and other stuff) */
                update_guest_efer(vm);

                if (vm->ia32e_guest) {
                        vmxmon_t *x = vm->com;
                        set_guest_load_and_host_msr(vm, I_MSR_STAR,
                                                    x->msr_star,
                                                    vm->host_ia32_star);
                        set_guest_load_and_host_msr(vm, I_MSR_FMASK,
                                                    x->msr_syscall_flag_mask,
                                                    vm->host_ia32_fmask);
                        set_gmsr(vm, I_MSR_KERNEL_GS_BASE,
                                 x->msr_kernel_gs_base);
                        set_gmsr(vm, I_MSR_LSTAR, x->msr_lstar);
                }
                set_guest_ia32_tsc_aux(vm);

                /* vmentry_ctrls depends on EFER.LMA */
                vm->entry_work |= EW_RELOAD_VMENTRY_CTRLS;
        }

        if (dirty & REGBIT_DESC_TABLES) {
                /* descriptor tables */
                vmwrite(VMCS_GUEST_GDTR_BASE, x->gdt_base);
                vmwrite(VMCS_GUEST_IDTR_BASE, x->idt_base);
                vmwrite(VMCS_GUEST_GDTR_LIMIT, x->gdt_limit);
                vmwrite(VMCS_GUEST_IDTR_LIMIT, x->idt_limit);
        }

        if (dirty & REGBIT_INTABILITY) {
                vmwrite(VMCS_GUEST_INTERRUPTIBILITY_STATE,
                          x->intability_state);
        }

        if (dirty & REGBIT_OTHER) {
                /* misc [FIXME] */
                vmwrite(VMCS_GUEST_PEND_DBG_EXC, x->pending_debug_exc);
        }

        if (dirty & REGBIT_CR3) {
                /* handled from update_mmu_state */
        }

        /* trap intercepts and interrupt window handling */
        if (dirty & REGBIT_INTERCEPTS) {
                vm->entry_work |= EW_RELOAD_PROC | EW_RELOAD_PROC2;
                vmcs_update_exc_bitmap(vm);
                vmcs_update_pfault_ecode_match(vm);
                update_msr_intercepts(vm);
        }

        /* injection */
        if (dirty & REGBIT_FLOW_CNTRL) {
                vm->entry_work &= ~EW_INJECT;
                if (x->injection.valid)
                        vm->entry_work |= EW_INJECT;
        }

        /* CPU state is up-to-date except control registers.
         * Control registers will be updated later. */
        vm->dirty &= (u32)REGBIT_CR;
}

static void
set_linked_vmcs(vm_t *vm)
{
        if (!vm->com->vmcs_dirty_userspace)
                return;
        vmx_vmcs_field_t *table = vm->guest_vmcs_table;
        const unsigned table_limit = 0x1000 / sizeof(*table);

        vmptrld(vm->linked_vmcs_phys);

        unsigned i = 0;
        while (table->valid && (i < table_limit)) {
                if (table->index > 0x8000)
                        printm("bad VMCS field %#x\n", table->index);
                if (vmwrite_checked(table->index, table->value)) {

                        update_32kb_bitmap(vm->vmread_bitmap.p,
                                           table->index, false);
                        update_32kb_bitmap(vm->vmwrite_bitmap.p,
                                           table->index, true);
                } else {
                        update_32kb_bitmap(vm->vmread_bitmap.p,
                                           table->index, true);
                        update_32kb_bitmap(vm->vmwrite_bitmap.p,
                                           table->index, true);
                }
                table++;
                i++;
        }
        vm->com->vmcs_dirty_userspace = 0;
        vm->com->vmcs_dirty_vmxmon = 0;
        vm->vmwrite_happened = false;
        vmptrld(vm->vmcs_phys);
}

static void
get_linked_vmcs(vm_t *vm)
{
        if (!vm->vmwrite_happened)
                return;

        vmx_vmcs_field_t *table = vm->guest_vmcs_table;
        const unsigned table_limit = 0x1000 / sizeof(*table);

        vmptrld(vm->linked_vmcs_phys);
        unsigned i = 0;
        while (table->valid && (i < table_limit)) {
                if (table->index > 0x8000)
                        printm("bad VMCS field %#x\n", table->index);
                if (!read_32kb_bitmap(vm->vmwrite_bitmap.p, table->index)) {
                        u64 val = vmread(table->index);
                        update_32kb_bitmap(vm->vmwrite_bitmap.p,
                                           table->index, true);
                        table->value = val;
                        table->dirty = 1;
                }
                table++;
                i++;
        }
        vm->com->vmcs_dirty_vmxmon = 1;
}

static void
flush_guest_ept(vm_t *vm)
{
        if (vm->use_nested_ept && need_guest_ept_flush(vm)) {
                BUMP(PERFCTR_INVEPT);
                invept(INVEPT_CONTEXT, get_nested_eptp(vm));
        }
}


static inline int
sync_state(vm_t *vm)
{
        vmxmon_t *x = vm->com;

        /* handle page invalidations */
        handle_tlb_flushes_ept(vm);

        /* copy register to VMCS */
        handle_dirty_regs(vm);

        u32 ew = vm->entry_work;

        if (vm->use_tpr_limit && (vm->non_root_guest
                                  || (vm->entry_work & EW_RELOAD_APIC_PAGE))) {
                u64 virt_apic_page;
                if (vm->non_root_guest) {
                        vmx_page_t *p;
                        if (vmxpage_lookup_nomap(vm, x->vlapic_page, &p,
                                                 RWX_R | RWX_W))
                                return 1;

                        virt_apic_page = upage_phys(&p->upage);
                } else {
                        virt_apic_page = mpage_phys(&vm->virt_apic_page);
                }
                vmwrite(VMCS_VIRT_APIC_PAGE_ADDR, virt_apic_page);
        }

        /* handle TPR limit usage switch */
        if (ew & EW_TPR_LIMIT_USAGE_SWITCH) {
                ew |= EW_RELOAD_PROC;
                update_tpr(vm);
                vm->entry_work &= ~EW_TPR_LIMIT_USAGE_SWITCH;
        }

        /* handle EPT usage switch */
        if (ew & EW_EPT_USAGE_SWITCH) {
                vmcs_update_pfault_ecode_match(vm);
                update_guest_efer(vm);
                ew |= EW_CR3_RELOAD | EW_RELOAD_PROC | EW_RELOAD_PROC2
                        | EW_RELOAD_CR0 | EW_RELOAD_CR4
                        | EW_UPDATE_EPT_PTR;
        }
        if ((ew & EW_UPDATE_EPT_PTR) && vm->use_ept) {
                u64 eptp;
                if (vm->use_nested_ept)
                        eptp = get_nested_eptp(vm);
                else
                        eptp = get_eptp(vm);
                vmwrite(VMCS_EPT_PTR, eptp);
        }

        /* reload cr4 (guest value in VMCS or x->cr4) */
        if (ew & (EW_RELOAD_CR4 | EW_RELOAD_DIRTY_CR4)) {
                /* obtain guest value from VMCS? */
                if (!(ew & EW_RELOAD_DIRTY_CR4))
                        x->cr4 = vmcs_get_sim_cr4(vm);
                vmcs_update_guest_cr4(vm, x->cr4);
        }

        /* update vmentry_ctrls if needed */
        if (ew & EW_RELOAD_VMENTRY_CTRLS)
                vmcs_update_vmentry_ctrls(vm);

        /* reload PROC */
        if (ew & EW_RELOAD_PROC)
                vmcs_update_proc_ctls(vm);

        /* reload PROC2 */
        if (ew & EW_RELOAD_PROC2) {
                vmcs_update_proc2_ctls(vm);
                if (vm->use_svmcs) {
                        vmwrite(VMCS_VMREAD_BITMAP_ADDR,
                                        vm->vmread_bitmap_phys);
                        vmwrite(VMCS_VMWRITE_BITMAP_ADDR,
                                        vm->vmwrite_bitmap_phys);
                        vmwrite(VMCS_VMCS_LINK_PTR, vm->linked_vmcs_phys);
                } else {
                        vmwrite(VMCS_VMCS_LINK_PTR, -1ULL);
                }
        }

        /* reload the real cr3 */
        if (ew & EW_CR3_RELOAD) {
                ew |= EW_FLUSH_VPID;
                if (vm->use_ept) {
                        vmwrite(VMCS_GUEST_CR3, vm->guest_cr3);
                        if (IS_MMU_MODE_PAE(vm->mmu_mode)) {
                                u64 *pdpte = vm->guest_pdpte;
                                vmwrite(VMCS_GUEST_PDPTE0, pdpte[0]);
                                vmwrite(VMCS_GUEST_PDPTE1, pdpte[1]);
                                vmwrite(VMCS_GUEST_PDPTE2, pdpte[2]);
                                vmwrite(VMCS_GUEST_PDPTE3, pdpte[3]);
                        }
                } else {
                        /* We copy CR3 flags like CR3.LAM_U48 and CR3.LAM_U57
                           from guest CR3 as they are. On hosts with no
                           respective support VM-entry will fail and vmxmon
                           will report VMRET_GUEST_STATE_UNSUPPORTED. That is
                           OK. NB: alternatively, we could leave the
                           CR3.LAM_U* flags cleared on hosts with no LAM. Then
                           vmxmon will report GP# faults if non-canonical
                           pointers are really used in the guest code. This may
                           give better performance but depends a lot on how
                           often non-canonical pointers are used in guest. */
                        u64 target_cr3_flags = x->cr3 & BITMASK(63, 52);
                        vmwrite(VMCS_GUEST_CR3, vm->shadow_page_table_pa
                                  | target_cr3_flags);
                }
        }

        if (vm->use_svmcs)
                set_linked_vmcs(vm);

        vm->entry_work = ew & ~EW_ONCE_ONLY_MASK;
        return 0;
}

static inline void
load_debug_regs(vm_t *vm)
{
        vmxmon_t *x = vm->com;
        u32 dr7 = x->dr7;
        if (unlikely((dr7 & 0xff) != 0)) {
                set_dr(7, 0);
                set_dr(6, 0xffff0000);
                if (dr7 & 0x3)
                        set_dr(0, x->dr[0]);
                if (dr7 & 0xc)
                        set_dr(1, x->dr[1]);
                if (dr7 & 0x30)
                        set_dr(2, x->dr[2]);
                if (dr7 & 0xc0)
                        set_dr(3, x->dr[3]);
        }
}

static inline void
guest_xcr0_switch_in(vm_t *vm)
{
        if (vm->use_xsave)
                set_xcr0(vm->guest_xcr0);
}

static inline void
guest_xcr0_switch_out(vm_t *vm)
{
        /* NB: it's necessary to restore host XCR0 rather soon: instruction
           extensions like AVX signal #UD if the respective bits are not set
           in XCR0 which may happen if we leave guest XCR0. To give an example,
           we saw a crash in IRQ handlers on Linux (which can be invoked
           immediately once wrap_local_irq_restore is called) when they used
           AVX512 and AVX512 state bits were not set in guest_xcr0. */
        if (vm->use_xsave)
                set_xcr0(vm->host_state.xcr0);
}

static inline void
guest_pkru_switch_in(vm_t *vm)
{
        if (vm->use_pku) {
                vm->host_state.pkru = get_pkru();
                set_pkru(vm->com->pkru);
                vm->pkru_owned = true;
        }
}

static inline void
guest_pkru_switch_out(vm_t *vm)
{
        if (vm->pkru_owned) {
                vm->com->pkru = get_pkru();
                set_pkru(vm->host_state.pkru);
                vm->pkru_owned = false;
        }
}

static void
fpu_yield(vm_t *vm)
{
        vmxmon_t *x = vm->com;

        if (!vm->fpu_owned)
                return;

        x->fpu_dirty = true;

        vm->fpu_owned = false;

        u64 xstate = XSTATE_LEGACY;
        if (vm->use_xsave) {
                xstate |= vm->guest_xcr0;
                set_xcr0(xstate);
                xsave64(vm->xsave_area, (u32)xstate, (u32)(xstate >> 32));

                set_xcr0(vm->host_state.xcr0); /* restore host XCR0 */
        } else {
                fxsave64(vm->xsave_area);
        }
        wrap_fpu_release(vm->fpu_state, xstate & gi.os_enabled_xsave_mask);

        vm->entry_work |= EW_RELOAD_CR0;
        vmwrite(VMCS_HOST_CR0, get_cr0()); /* host CR0.TS set */
}

static int
fpu_obtain(vm_t *vm)
{
        vm->fpu_owned = true;
        u64 xstate = XSTATE_LEGACY | (vm->use_xsave ? vm->guest_xcr0 : 0);
        wrap_fpu_obtain(vm->fpu_state, xstate & gi.os_enabled_xsave_mask);
        if (vm->use_xsave) {
                set_xcr0(xstate);
                u32 eax = (u32)xstate;
                u32 edx = (u32)(xstate >> 32);
                if (unlikely(wrap_xrstor64(vm->xsave_area, eax, edx))) {
                        printm("XSAVE load failed (XCR0: %#llx)\n", xstate);
                        goto err;
                }
                set_xcr0(vm->host_state.xcr0);
        } else {
                if (unlikely(wrap_fxrstor64(vm->xsave_area))) {
                        printm("Can't load FXSAVE area\n");
                        goto err;
                }
        }

        vm->entry_work |= EW_RELOAD_CR0;
        vmwrite(VMCS_HOST_CR0, get_cr0()); /* host CR0.TS cleared */
        return 0;

  err:
        if (vm->use_xsave)
                set_xcr0(vm->host_state.xcr0);
        vm->fpu_owned = false;
        wrap_fpu_release(vm->fpu_state, xstate & gi.os_enabled_xsave_mask);
        return 1;
}

static int
conditional_fpu_obtain(vm_t *vm)
{
        /* running with FPU disabled? */
        if (vm->fpu_disabled)
                return 0;

        /* if Intel® MPX is currently enabled then we should obtain Intel® MPX
           config register otherwise Intel® MPX instructions become NOPs and
           will not trigger #NM */
        if (mpx_enabled(vm)) {
                if (unlikely(fpu_obtain(vm)))
                        goto err;
                return 0;
        }

        /* disable lazy FPU if fpu_used_cnt reaches threshold;
           fpu_used_cnt is reset after test_interval entries */
        const unsigned test_interval = 500;
        const unsigned threshold = 25;
        /* when lazy FPU is disabled, keep it disabled for backoff_interval
           entries... */
        const unsigned backoff_interval = 10000;
        /* ...then reenable lazy FPU. If the FPU is used during the next
           recheck_interval entries, immediately disable lazy FPU again. */
        const unsigned recheck_interval = 100;   

        if (vm->use_lazy_fpu) {
                if (vm->fpu_used_cnt < threshold) {
                        /* threshold is not reached yet */
                        if (vm->fpu_recheck_cnt-- == 0) {
                                /* but we reached end of sample interval:
                                   reset statistics and begin new interval */
                                vm->fpu_recheck_cnt = test_interval;
                                vm->fpu_used_cnt = 0;
                        }
                        return 0;
                }
                /* threshold was reached: disable lazy_fpu mode and
                   set up recheck after backoff_interval */
                vm->use_lazy_fpu = false;
                vm->fpu_recheck_cnt = backoff_interval;
                vm->fpu_used_cnt = 0;
        } else if (vm->fpu_recheck_cnt-- == 0) {
                /* recheck count reached; enable lazy FPU for a short while */
                vm->use_lazy_fpu = true;
                vm->fpu_used_cnt = threshold - 1;
                vm->fpu_recheck_cnt = recheck_interval;
        }

        if (unlikely(fpu_obtain(vm)))
                goto err;

        return 0;

  err:
        set_vmret(vm, VMRET_INTERNAL_ERROR);
        return 1;
}

static uintptr_t
cr4_update_mask(void)
{
        uintptr_t mask = 0;
        if (gi.has_xsave && !gi.osxsave_enabled)
                mask |= CR4_OSXSAVE;
        if (gi.has_pku && !gi.ospke_enabled)
                mask |= CR4_OSPKE;
        return mask;
}

static void
update_host_cr4(vm_t *vm)
{
        uintptr_t mask = cr4_update_mask();
        if (mask) {
                uintptr_t cr4 = cr4_set_bits(mask);
                vm->host_state.cr4 = cr4;
                vmwrite(VMCS_HOST_CR4, cr4);
        }
}

static void
restore_host_cr4(vm_t *vm)
{
        uintptr_t mask = cr4_update_mask();
        if (mask)
                cr4_clear_bits(vm->host_state.cr4, mask);
}

static void
unpin_vmcs(vm_t *vm, unsigned ipl)
{
        host_state_t *h = &vm->host_state;

        if (!vm->vmcs_pinned)
                return;

        if (mp.update_all_regs)
                get_regs_from_vmcs(vm, REGBIT_ALL);
        else {
                /* always read out the program counter, as an optimization */
                get_regs_from_vmcs(vm, REGBIT_IP | REGBIT_IP_INFO);
        }

        guest_pkru_switch_out(vm);
        fpu_yield(vm);
        restore_host_cr4(vm);

        /* restore IA32_BNDCFGS if necessary */
        if (vm->use_bndcfgs && h->ia32_bndcfgs != 0)
                wrmsr(MSR_IA32_BNDCFGS, h->ia32_bndcfgs);

        if (vm->use_svmcs)
                get_linked_vmcs(vm);

        int cpu = vm->cpu;
        gi.cpu_state[cpu]->vm = NULL;

        if (WINDOWS)
                vm->cs->claim_nmi = false;
        if (!mp.vmm_exclusive) {
                fetch_state_from_current_cpu(vm);
                vmx_off(cpu);
        }

        vm->vmcs_pinned = false;
        wrap_put_cpu(ipl);
}

static int
pin_vmcs(vm_t *vm, unsigned int *ipl)
{
        /* do not activate the VMCS if we are going to return anyway */
        unsigned vmret = get_vmret(vm);
        if (vmret != VMRET_AGAIN && vmret != 0)
                return 1;

        int cpu = wrap_get_cpu(ipl);

        if (cpu == vm->cpu) {
                activate_vmcs(vm);
        } else {
                if (vm->cpu != -1) {
                        wrap_put_cpu(*ipl);
                        fetch_state_from_vm_cpu(vm);
                        cpu = wrap_get_cpu(ipl);
                }
                if (prepare_cpu(vm, cpu, ipl))
                        return 1;

                cpu_state_t *cs = gi.cpu_state[cpu];
                if (unlikely(!cs)) {
                        if (vmx_cpu_up(cpu)) {
                                set_vmret(vm, VMRET_OUT_OF_MEMORY);
                                return 1;
                        }
                }

                /* Bind VM to CPU. */
                vm->cpu = cpu;
                vm->cs = gi.cpu_state[cpu];
                prepare_vmcs(vm);
        }

        /* things that need cleanup must go after this line */
        vm->vmcs_pinned = true;
        gi.cpu_state[vm->cpu]->vm = vm;

        if (sync_state(vm))
                return 1;

        update_host_cr4(vm);
        if (conditional_fpu_obtain(vm))
                return 1;
        guest_pkru_switch_in(vm);

        load_debug_regs(vm);

        save_mutable_host_state(vm);
        return 0;
}

static void
restore_nmi_blocking(vm_t *vm)
{
        unsigned idt_info = vmread(VMCS_IDT_VECTORING_INFO_FIELD);
        bool idt_valid = (idt_info & BIT(31));
        if (likely(!idt_valid)) {
                vmwrite(VMCS_GUEST_INTERRUPTIBILITY_STATE,
                          vmread(VMCS_GUEST_INTERRUPTIBILITY_STATE) | BIT(3));
        }
}

static inline bool
ept_user_bit_valid(u64 qual)
{
        const u64 advanced_ept_vmexit_info = BIT(22);
        const u64 user_bit_valid = BIT(7) | BIT(8);

        return (gi.ept_vpid_cap & advanced_ept_vmexit_info)
                && ((qual & user_bit_valid) == user_bit_valid);
}

static inline rwx_t
ept_qual_to_rwx(vm_t *vm, u64 qual)
{
        rwx_t rwx = qual & (RWX_R | RWX_W);

        if (qual & BIT(2)) {
                if (vm->use_mbe && ept_user_bit_valid(qual)) {
                        rwx |= (qual & BIT(9)) ? RWX_XU : RWX_XS;
                } else {
                        rwx |= RWX_X;
                }
        }

        return rwx;
}

static inline void
handle_ept_fault(vm_t *vm, u64 qual, unsigned int *ipl)
{
        u64 pa = vmread(VMCS_GUEST_PHYS_ADDR);
        vmx_page_t *p;
        rwx_t rwx = ept_qual_to_rwx(vm, qual);

        if (unlikely(qual & INT_INFO_NMI_UNBLOCKING_DUE_TO_IRET))
                restore_nmi_blocking(vm);

        if (vm->use_nested_ept) {
                unpin_vmcs(vm, *ipl);
                if (!handle_nested_ept_fault(vm, pa, rwx))
                        set_vmret(vm, VMRET_AGAIN);
                pin_vmcs(vm, ipl);
        } else {
                if (!vmxpage_lookup_nomap(vm, pa, &p, rwx)) {
                        /* page already present;
                           flush ept to make it visible */
                        vm->entry_work |= EW_FLUSH_EPT;
                        set_vmret(vm, VMRET_AGAIN);
                }
        }
}

static inline void
handle_vmexit_exception(vm_t *vm, unsigned int *ipl, u64 qual)
{
        u32 info = vm->vmexit_int_info;
        u8 ecode = (info & BIT(11)) ? vmread(VMCS_VMEXIT_INT_ECODE) : 0;
        u8 vec = info & 0xff;
        int itype = FIELD(info, 10, 8);
        vmxmon_t *x = vm->com;
        unsigned vmret;

        if (!(info & BIT(31))) {
                printm("bit 31 in VMCS_VMEXIT_INT_INFO not set\n");
                set_vmret(vm, VMRET_INTERNAL_ERROR);
                return;
        }

        /* external interrupt or NMI */
        if (itype <= 2) {
                if (itype == 2 /* NMI */) {
                        BUMP(PERFCTR_VMEXIT_EXC_NMI);
                        set_vmret(vm, VMRET_AGAIN);
                } else {
                        BUMP(PERFCTR_VMEXIT_EXC_IRQ);
                        set_vmret(vm, VMRET_NOP);
                }
                return;
        }

        if (unlikely(info & INT_INFO_NMI_UNBLOCKING_DUE_TO_IRET)) {
                if (likely(vec != 8))
                        restore_nmi_blocking(vm);
        }

        switch (vec) {
        case 1: /* #DB */
                BUMP(PERFCTR_VMEXIT_EXC_OTHER);
                if (itype == 5) {
                        set_vmret(vm, VMRET_ICEBP);
                } else {
                        x->int_vec = vec;
                        set_vmret(vm, VMRET_EXC);
                }
                break;

        case 7: /* #NM (device not available) */
                /* FPU usage disabled by userspace? */
                if (vm->fpu_disabled) {
                        set_vmret(vm, VMRET_FPU_DISABLED);
                        break;
                }
                x->cr0 = vmcs_get_sim_cr0(vm);
                if (!vm->fpu_owned && !(x->cr0 & CR0_TS)) {
                        BUMP(PERFCTR_FPU_OBTAIN);
                        vm->fpu_used_cnt++;
                        vmret = (fpu_obtain(vm) == 0) ? VMRET_AGAIN
                                                      : VMRET_INTERNAL_ERROR;
                        set_vmret(vm, vmret);
                        break;
                }
                if (possible_spurious_nm_due_to_errata(x->cr0)) {
                        set_vmret(vm, VMRET_NM_ERRATA_EMULATION);
                        break;
                }
                BUMP(PERFCTR_VMEXIT_EXC_NM);
                if (x->vmx_non_root_mode || (x->exc_bitmap & BIT(7))) {
                        x->int_vec = vec;
                        set_vmret(vm, VMRET_EXC);
                        break;
                }
                vmx_inject_hw_exception(vm, vec);
                set_vmret(vm, VMRET_AGAIN);
                break;

        case 14: /* #PF */
                BUMP(PERFCTR_VMEXIT_EXC_PF);

                if (vm->use_ept) {
                        /* we only see #PF intercepts */
                        x->vmret_qual = qual;
                        x->ecode = ecode;
                        set_vmret(vm, VMRET_PAGE_FAULT);
                        break;
                }

                /* EFLAGS.AC is required for SMAP checks. */
                x->rflags = vmread(VMCS_GUEST_RFLAGS);

                unpin_vmcs(vm, *ipl);
                if (!vm->mmu_ops.page_fault(vm, ecode, qual))
                        set_vmret(vm, VMRET_AGAIN);
                pin_vmcs(vm, ipl);
                break;

        case 18: /* #MC */
                printm("vmxmon: Guest Machine Check Exception");
                set_vmret(vm, VMRET_MACHINE_CHECK);
                break;

        default:
                BUMP(PERFCTR_VMEXIT_EXC_OTHER);
                x->int_vec = vec;
                set_vmret(vm, VMRET_EXC);
                break;
        }
}

static inline void
clear_injections(vm_t *vm)
{
        if (!(vm->entry_work & EW_INJECT))
                return;

        vmxmon_t *x = vm->com;
        vm->entry_work &= ~EW_INJECT;
        x->injection.valid = false;

        /* old hw does not count injection steps */
        if (!vm->cs->pmc.counts_injections) {
                unsigned idt_info = vmread(VMCS_IDT_VECTORING_INFO_FIELD);
                bool idt_valid = (idt_info & BIT(31));
                unsigned vec = (idt_info & 0xff);
                if (vm->elapsed || !idt_valid || vec != x->injection.vector)
                        x->steps++;
                /* Question: If a benign exception was injected and it causes
                   a secondary exception - should we count that step? */
        }
}


static inline void
handle_idt_vectoring(vm_t *vm)
{
        unsigned idt_info = vmread(VMCS_IDT_VECTORING_INFO_FIELD);
        bool idt_valid = (idt_info & BIT(31));

        if (!idt_valid)
                return;

        unsigned vector = idt_info & 0xff;
        unsigned type = (idt_info >> 8) & 0x7;
        bool has_ecode = (idt_info & BIT(11));
        unsigned inst_len = (type >= 4 && type <= 6) ?
                vmread(VMCS_VMEXIT_INST_LEN) : 0;
        unsigned ecode = has_ecode ? vmread(VMCS_IDT_VECTORING_ECODE) : 0;

        /*
           Some examples of software exceptions:

           a) INTn + vmexit(task_switch)
           b) INTn + vmexit(#GP)
           c) INTO + vmexit(#GP...) (if INTO is not intercepted)
           d) INT3 + vmexit(#GP...) (if INT3 is not intercepted)

           Some examples of hardware exceptions:

           e) injected(#PF) + vmexit(#PF) (stack not writable)
           f) injected(#NM) + vmexit(#PF)
           g) injection + vmexit(NMI/IRQ)
           h) BOUND/UD2 + vmexit (if vector is not intercepted)
           i) #PF + vmexit(EPT_viol)
        */

        vmxmon_t *x = vm->com;
        x->injection.valid = 1;
        x->injection.vector = vector;
        x->injection.deliver_ec = has_ecode;
        x->injection.ecode = ecode;
        x->injection.type = type;
        x->injection.instr_len = inst_len;
        vm->entry_work |= EW_INJECT;

        /*
           Subtle point 1: if an IRET faults, NMI-blocking will be
           cleared. Thus rerunning the iret instruction could
           cause an immediate NMI before the IRET is taken.

           Subtle point 2: blocking by STI is cleared, even though
           the instruction has not completed. That is, if we rerun
           the faulting instruction then, an IRQ could incorrectly
           be taken.
        */
        if (0) {
                printm("idt_vectoring: #%d, type %d, ecode %d:0x%02x, ",
                       vector, type, has_ecode, ecode);
                printm("vmexit %zd %08zx:%02zx (qual %08zx), ",
                       vmread(VMCS_EXIT_REASON),
                       vmread(VMCS_VMEXIT_INT_INFO),
                       vmread(VMCS_VMEXIT_INT_ECODE),
                       vmread(VMCS_EXIT_QUAL));
                printm("rip %08zx\n", vmread(VMCS_GUEST_RIP));
        }
}

/* set dgctl[14] according to use_smm_perf_freeze bit */
static inline void
reload_dbgctl_smm_freeze(cpu_state_t *cs)
{
        u64 v = vmread(VMCS_GUEST_IA32_DBGCTL) & ~BIT(14);
        vmwrite(VMCS_GUEST_IA32_DBGCTL,
                  v | (cs->use_smm_perf_freeze ? BIT(14) : 0));
}

static inline int
handle_freeze_smm_failure(vm_t *vm)
{
        cpu_state_t *cs = vm->cs;

        /* if the VMCS has launched successfully, we are good */
        if (vm->launched)
                return 0;

        if (cs->use_smm_perf_freeze) {
                /* PERF_FREEZE bit might prevent vmentry */
                cs->use_smm_perf_freeze = false;
                cs->smm_freeze_disabled = true;
                reload_dbgctl_smm_freeze(cs);
                set_vmret(vm, VMRET_NOP);
                return 1;
        } else if (cs->smm_freeze_disabled) {
                /* didn't help, re-enable */
                cs->use_smm_perf_freeze = true;
                cs->smm_freeze_disabled = false;
                reload_dbgctl_smm_freeze(cs);
        }
        return 0;
}

static void
check_one(const char *str, u32 v, u32 allow0, u32 allow1)
{
        u32 sup = (v | allow0) & allow1;
        if (sup != v)
                printm("Unsupported %s CTRL %08x (%08x)\n", str, v, v ^ sup);
}

static void
check_vmcs_ctrl_fields(vm_t *vm)
{
        u32 vmentry_ctrls = vmread(VMCS_VMENTRY_CTRLS);
        u32 vmexit_ctrls = vmread(VMCS_VMEXIT_CTRLS);
        u32 proc_ctrls = vmread(VMCS_PROC_BASED_VM_EXEC_CTRLS);
        u32 pin_ctrls = vmread(VMCS_PIN_BASED_VM_EXEC_CTRLS);
        u32 proc2_ctrls = 0;
        if (gi.has_proc2)
                proc2_ctrls = vmread(VMCS_SEC_PROC_BASED_CTLS);

        check_one("VMENTRY", vmentry_ctrls,
                  gi.vmentry_0_allowed, gi.vmentry_1_allowed);
        check_one("VMEXIT", vmexit_ctrls,
                  gi.vmexit_0_allowed, gi.vmexit_1_allowed);
        check_one("PROC", proc_ctrls,
                  gi.proc_based_0_allowed, gi.proc_based_1_allowed);
        check_one("PROC2", proc2_ctrls,
                  gi.proc_based2_0_allowed, gi.proc_based2_1_allowed);
        check_one("PIN", pin_ctrls,
                  gi.pin_based_0_allowed, gi.pin_based_1_allowed);
}

static inline int
handle_entry_failures(vm_t *vm)
{
        /* VMLAUNCH/VMRESUME flags stored in entry_failure */
        unsigned flags = vm->host_state.entry_failure;
        if (likely(flags == 0))
                return 0;

        vmxmon_t *x = vm->com;
        vm->host_state.entry_failure = 0;

        /* basic vm-entry checks */
        if ((flags & 0x01 /* C */)) {
                /* invalid VMCS */
                printm("vmentry: invalid VMCS\n");
                set_vmret(vm, VMRET_INTERNAL_ERROR);
                return 1;
        }

        /* checks on vmx controls and host-state area */
        if ((flags & 0x40 /* Z */)) {
                /* error code reported in VMCS_VM_INSTR_ERR */
                u32 inst_err = vmread(VMCS_VM_INSTR_ERR);
                vm->last_vmcs_vm_instr_err = inst_err;

                /* Targets VTPR might be less than targets TPR threshold. This
                 * will lead to "VM-entry with invalid control fields" error.
                 * Disable TPR shadowing and try to do VM-entry again. */
                if ((inst_err == 7) && vm->non_root_guest && vm->use_tpr_limit
                    && (x->vlapic_page != 0)) {
                        vm->use_tpr_limit = false;
                        vm->entry_work |= EW_TPR_LIMIT_USAGE_SWITCH;
                        set_vmret(vm, VMRET_NOP);
                        return 1;
                }

                check_vmcs_ctrl_fields(vm);
                printm("vmentry: error %d\n", inst_err);
                x->vmret_qual = inst_err;
                /* FIXME: We could probably throw a hard failure */
                set_vmret(vm, VMRET_INTERNAL_ERROR);
                return 1;
        }
        printm("CR.[CZ] not set (%x)\n", flags);
        set_vmret(vm, VMRET_INTERNAL_ERROR);
        return 1;
}


static inline int
handle_guest_state_failures(vm_t *vm)
{
        u32 exit_reason = vm->vmexit_reason;
       
        /* 22.7: guest state entry failures */
        if (likely(!(exit_reason & BIT(31))))
                return 0;

        unsigned long qual = vmread(VMCS_EXIT_QUAL);
        exit_reason &= 0xffff;

        msr_entry_t *failed_msr;

        /* VMLAUNCH instruction isn't counted (no MSRs are written) */
        switch (exit_reason) {
        case VMEXIT_INV_GUEST_STATE:
                if (qual == 4) {
                        /* Invalid VMCS link pointer, the driver's bug,
                           not an unsupported guest state. */
                        break;
                }
                if (handle_freeze_smm_failure(vm)) {
                        return 1;
                } else {
                        vmxmon_t *x = vm->com;
                        x->vmret_qual = qual;
                        set_vmret(vm, VMRET_GUEST_STATE_UNSUPPORTED);
                        return 1;
                }
        case VMEXIT_MSR_LOAD_FAILURE:
                /* The exit qualification is loaded to indicate which entry in
                   the VM-entry MSR-load area caused the problem (1 for the
                   first entry, 2 for the second, etc.). */
                failed_msr = msr_slot(vm, FIRST_GMSR_SLOT + qual - 1);

                if (failed_msr->msr == MSR_KERNEL_GS_BASE
                    || failed_msr->msr == MSR_LSTAR) {
                        /* An address can be 57-bit canonical but not 48-bit
                           canonical if a processor supported 5-level paging
                           even if 4-level paging is active */
                        s64 addr_high = (s64)failed_msr->value >> (57 - 1);
                        if (addr_high == 0 || addr_high == -1) {
                                set_vmret(vm, VMRET_GUEST_STATE_UNSUPPORTED);
                                return 1;
                        }
                }

                printm("failed to load MSR %#x, value = %#llx\n",
                       failed_msr->msr, failed_msr->value);
        case VMEXIT_MACHINE_CHECK:
        default:
                break;
        }
        printm("guest entry failure (reason/qual = %d/0x%lx)\n",
               exit_reason, qual);
        set_vmret(vm, VMRET_INTERNAL_ERROR);
        return 1;
}


static inline void
handle_vmexit(vm_t *vm, unsigned *ipl)
{
        vmxmon_t *x = vm->com;

        u32 reason = vm->vmexit_reason;
        bump_vmexit(vm, reason);

        u64 qual = vmread(VMCS_EXIT_QUAL);
        x->vmret_qual = qual;
        x->instruction_length = vmread(VMCS_VMEXIT_INST_LEN);
        x->vmret_inst_info = vmread(VMCS_VMX_INST_INFO);

        if (reason < sizeof(passon_vmexits)/sizeof(passon_vmexits[0])) {
                if (passon_vmexits[reason]) {
                        set_vmret(vm, passon_vmexits[reason]);
                        return;
                }
        }

        switch (reason) {
        case VMEXIT_IRQ_WINDOW:
                vm->proc_ctrls &= ~PROC_INT_WINDOW_EXITING;
                vmwrite(VMCS_PROC_BASED_VM_EXEC_CTRLS, vm->proc_ctrls);
                x->intercepts &= ~INTERCEPT_INT_WINDOW;
                set_vmret(vm, VMRET_IRQ_WINDOW);
                break;

        case VMEXIT_NMI_WINDOW:
                vm->proc_ctrls &= ~PROC_NMI_WINDOW_EXITING;
                vmwrite(VMCS_PROC_BASED_VM_EXEC_CTRLS, vm->proc_ctrls);
                x->intercepts &= ~INTERCEPT_NMI_WINDOW;
                set_vmret(vm, VMRET_NMI_WINDOW);
                break;

        case VMEXIT_EXC:
                handle_vmexit_exception(vm, ipl, qual);
                break;

        case VMEXIT_TPR_BELOW_THRESH:
                x->steps++;
                set_vmret(vm, VMRET_TPR_BELOW_THRESH);
                break;

        case VMEXIT_PAUSE:
		if (x->return_on_pause) {
			set_vmret(vm, VMRET_PAUSE);
		} else {
			x->steps++;
			x->stall_cycles += x->pause_slow_cycles;
			x->step_over_on_entry = true;
			set_vmret(vm, VMRET_AGAIN);
		}
                break;

        case VMEXIT_INIT:
                printm("INIT received (CPU: %d)\n", vm->cpu);
                set_vmret(vm, VMRET_INTERNAL_ERROR);
                break;

        case VMEXIT_INVLPG:
                /* userspace notifies us about the flushing */
                x->vmret_qual = qual;
                set_vmret(vm, VMRET_INVLPG);
                break;

        case VMEXIT_CTRL_REG_ACCESS:
                if (!x->vmx_non_root_mode
                    && !(x->intercepts & INTERCEPT_CR3_READ) &&
                    (qual & 0x3f) == 0x13) {
                        /* mov from cr3 */
                        int gpr = (qual >> 8) & 0xf;
                        x->gprs[gpr] = x->cr3;
                        if (gpr == GPR_SP) {
                                /* RSP is loaded from VMCS.
                                   Other GPRs are loaded by vmentry_prepare. */
                                vmwrite(VMCS_GUEST_RSP, x->cr3);
                        }
                        x->steps++;
                        x->step_over_on_entry = true;
                        set_vmret(vm, VMRET_AGAIN);
                } else if ((qual & 0x3f) == 3) {
                        /* force a flush */
                        x->cr3_flush_keep_global = 1;
                        x->cr3_flush = 1;
                        set_vmret(vm, VMRET_CTRL_REG_ACCESS);
                } else {
                        set_vmret(vm, VMRET_CTRL_REG_ACCESS);
                }
                break;

        case VMEXIT_MONITOR_TRAP_FLAG:
                if (vm->elapsed == 0) {
                        /* work around buggy performance counter */
                        if (vmread(VMCS_GUEST_RIP) != x->ip)
                                x->steps++;
                }
                set_vmret(vm, VMRET_AGAIN);
                break;

        case VMEXIT_EPT_VIOLATION:
                handle_ept_fault(vm, qual, ipl);
                break;
        case VMEXIT_VMREAD:
                if (vm->use_svmcs) {
                        /* A masked field for VMREAD means that host or guest
                           do not support it */
                        set_vmret(vm, VMRET_VMREAD);
                } else
                        set_vmret(vm, VMRET_VMREAD);
                break;
        case VMEXIT_VMWRITE:
                if (vm->use_svmcs) {
                        unsigned gpr = (x->vmret_inst_info >> 28) & 0xf;
                        if (gpr == GPR_SP)
                                x->gprs[gpr] = vmread(VMCS_GUEST_RSP);
                        u64 field = x->gprs[gpr];
                        if (probe_write_vmcs_field_from_guest(vm, field)) {
                                /* Relax VMWRITE bitmap, so that this and
                                   future references will not cause VM-exits */
                                vm->vmwrite_happened = true;
                                update_32kb_bitmap(vm->vmwrite_bitmap.p, field, false);
                                set_vmret(vm, VMRET_AGAIN);
                        } else { /* Unsupported field */
                                set_vmret(vm, VMRET_VMWRITE);
                        }
                } else {
                        set_vmret(vm, VMRET_VMWRITE);
                }
                break;
        default:
                printm("unexpected exit reason %d:%llx\n", reason, qual);
                set_vmret(vm, VMRET_INTERNAL_ERROR);
                break;
        }
}

static void
handle_vmexit_early(vm_t *vm)
{
        if (vm->host_state.entry_failure != 0)
                return;
        u32 vmexit = vmread(VMCS_EXIT_REASON);
        vm->vmexit_reason = vmexit;
        if (vmexit != VMEXIT_EXC)
                return;
        u32 int_info = vmread(VMCS_VMEXIT_INT_INFO);
        vm->vmexit_int_info = int_info;
        u32 itype = (int_info & INT_INFO_TYPE_MASK);
        if (itype == INT_INFO_TYPE_NMI) {
                /* if we do not have a performance event, then there are
                   likely set bits in the status register - we consider
                   all events to be ours */
                bool our_nmi = (!vm->cs->perf_event
                                || (rdmsr(MSR_IA32_PERF_GLOBAL_STATUS)
                                    == BIT(32)));
                bool fold = mp.nmi_optimization && our_nmi;
                if (fold && handle_nmi()) {
                        /* NOTE: we might miss a non-performance monitoring NMI
                           occurring simultaneously with our NMI when we fold */
                        unblock_nmi();
                } else {
                        /* service NMI */
                        __asm__ __volatile__("int $2");
                }
        }
}

static inline void
do_entry_work(vm_t *vm)
{
        unsigned bits = vm->entry_work;
        vmxmon_t *x = vm->com;

        if (x->step_over_on_entry) {
                x->step_over_on_entry = false;
                x->ip = vmread(VMCS_GUEST_RIP) + x->instruction_length;
                x->intability_state =
                        vmread(VMCS_GUEST_INTERRUPTIBILITY_STATE) & ~3;
                vmwrite(VMCS_GUEST_RIP, x->ip);
                vmwrite(VMCS_GUEST_INTERRUPTIBILITY_STATE,
                          x->intability_state);
        }

        flush_guest_ept(vm);

        if (!bits)
                return;

        if (bits & EW_RELOAD_VMEXIT_CTRLS)
                vmcs_update_vmexit_ctrls(vm);

        if (bits & (EW_RELOAD_CR0 | EW_RELOAD_DIRTY_CR0)) {
                if (!(bits & EW_RELOAD_DIRTY_CR0))
                        x->cr0 = vmcs_get_sim_cr0(vm);
                vmcs_update_guest_cr0(vm, x->cr0);
        }
        if (bits & EW_INJECT) {
                /* possibly somewhat unsafe to read from the shared page here */
                ev_injection_t *p = &x->injection;

                vmwrite(VMCS_VMENTRY_INT_INFO_FIELD,
                          0x80000000 | (p->deliver_ec << 11)
                                     | (p->type << 8) | p->vector);
                if (p->deliver_ec)
                        vmwrite(VMCS_VMENTRY_EXC_ECODE, p->ecode);
                if (p->type == 4 || p->type == 6 || p->type == 5)
                        vmwrite(VMCS_VMENTRY_INST_LEN, p->instr_len);

                /* RFLAGS_RF must be set if this is an external interrupt */
                if (p->type == 0)
                        vmwrite(VMCS_GUEST_RFLAGS,
                                  vmread(VMCS_GUEST_RFLAGS) | RFLAGS_RF);
        }

        if ((bits & EW_FLUSH_VPID) && vm->use_vpid) {
                BUMP(PERFCTR_INVVPID);
                invvpid(INVVPID_VPID_CONTEXT, vm->vpid, 0);
        }

        if (vm->use_ept) {
                if (bits & EW_FLUSH_EPT) {
                        BUMP(PERFCTR_INVEPT);
                        invept(INVEPT_CONTEXT, get_eptp(vm));
                }
        }

        /* injection is sticky until it succeeds */
        vm->entry_work &= EW_INJECT;

        /* CPU state is now completely up-to-date */
        vm->dirty = 0;
}

typedef struct {
        u16 ldt;
        u16 fs, gs;
        u16 ds, es;
        u64 fs_base;
} user_segs_t;

static inline void
save_user_segments(user_segs_t *s)
{
        s->ldt = get_ldt();
        s->fs = get_fs();
        s->gs = get_gs();
        s->ds = get_ds();
        s->es = get_es();
        s->fs_base = rdmsr(MSR_FS_BASE);
}

#if defined __windows__
static inline void
wrap_loadsegment_gs_index(u16 sel)
{
        u64 gs_base = rdmsr(MSR_GS_BASE);
        wrap_loadsegment_gs(sel);
        wrmsr(MSR_GS_BASE, gs_base);
}
#endif

static inline void
restore_user_segments(user_segs_t *s)
{
        if (s->ldt)
                set_ldt(s->ldt);

        /* assumption: %gs is used for kernel TLS */
        if (s->ds)
                wrap_loadsegment_ds(s->ds);
        if (s->es)
                wrap_loadsegment_es(s->es);
        if (s->gs) {
                /* may change KERNEL_GS_BASE but not GS_BASE */
                wrap_loadsegment_gs_index(s->gs);
        }
        if (s->fs)
                wrap_loadsegment_fs(s->fs);
        wrmsr(MSR_FS_BASE, s->fs_base);
}

static inline void
reload_task_register(vm_t *vm)
{
        if (!wrap_invalidate_tss_limit()) {
                host_state_t *h = &vm->host_state;

                /* Reload TR to get correct TSS limit. */
                u32 *p = (u32 *)(h->gdt_desc.base + (h->tss_sel & ~0x7));
                p[1] &= 0xfffffdff; /* Clear descriptor busy bit */
                set_tr(h->tss_sel);
        }
}

static void
update_use_tpr_limit(vm_t *vm)
{
        vmxmon_t *x = vm->com;
        bool use_tpr_limit =
                vm->ia32e_guest
                && (x->active_vmxmon_features & VMXMON_FEATURE_TPR_LIMIT)
                && (!x->vmx_non_root_mode || x->vlapic_page != 0);

        if (vm->use_tpr_limit == use_tpr_limit)
                return;

        /* read out the TPR value before we disable TPR acceleration */
        if (!use_tpr_limit)
                get_regs(vm, REGBIT_CR);

        vm->entry_work |= EW_TPR_LIMIT_USAGE_SWITCH;
        vm->entry_work |= use_tpr_limit ? EW_RELOAD_APIC_PAGE : 0;
        vm->use_tpr_limit = use_tpr_limit;
}

static int
update_svmcs_setting(vm_t *vm)
{
        vmxmon_t *x = vm->com;
        /* Use host shadow VMCS for guest VMREAD/VMWRITE only if all
           of the following holds:
           - host supports Shadow VMCS
           - guest is in root mode (no guest shadow VMCS at the moment)
           - guest has loaded a VMCS 
        */
        bool use_svmcs = gi.has_svmcs && x->vmx_root_mode && x->vmcs_loaded;
        if (use_svmcs != vm->use_svmcs)
                vm->entry_work |= EW_RELOAD_PROC2;
        vm->use_svmcs = use_svmcs;
        return 0;
}

/* updates XSAVE usage and state (guest xcr0) */
static int
update_use_xsave(vm_t *vm, u64 guest_xcr0)
{
        /* refuse to run with unknown xcr0 bits set since we cannot validate
           that xcr0 is valid and we do not know if simulation breaks
           if we continue with XSAVE disabled */
        if (guest_xcr0 & ~XSTATE_SUPPORTED_MASK) {
                set_vmret(vm, VMRET_GUEST_STATE_UNSUPPORTED);
                return 1;
        }
        bool use_xsave = (guest_xcr0 != 0)
                && mp.enable_xsave && gi.has_xsave
                && !(guest_xcr0 & ~gi.host_xcr0_mask);

        /* require large xsave area for everything beyond Intel® AVX */
        if (!vm->large_xsave_area
            && (guest_xcr0 & ~(XSTATE_AVX | XSTATE_LEGACY))) {
                printm("Large XSAVE area needed for xcr0 %llx!\n", guest_xcr0);
                set_vmret(vm, VMRET_INTERNAL_ERROR);
                return 1;
        }
        /* complain if the guest xcr0 value is illegal. The check is also
           used to ensure that guest_xcr0 can be loaded into hardware xcr0
           without triggering a #GP. */
        if (use_xsave && xcr0_illegal(guest_xcr0)) {
                printm("Illegal xcr0 value (%llx)\n", guest_xcr0);
                set_vmret(vm, VMRET_INTERNAL_ERROR);
                return 1;
        }
        
        if (use_xsave != vm->use_xsave) {
                vm->use_xsave = use_xsave;
                /* update handling of CR4.OSXSAVE (depends on vm->use_xsave) */
                vm->entry_work |= EW_RELOAD_CR4;
        }
        vm->guest_xcr0 = guest_xcr0;
        return 0;
}

/* update IA32_BNDCFGS handling */
static int
update_use_bndcfgs(vm_t *vm)
{
        vmxmon_t *x = vm->com;
        bool use_bndcfgs = ((vm->guest_xcr0 & XSTATE_MPX)
                            && (x->ia32_bndcfgs & BNDCFG_EN));
        if (vm->use_bndcfgs != use_bndcfgs) {
                /* check that VMCS supports bndcfgs */
                if (use_bndcfgs && !gi.has_bndcfgs_support) {
                        set_vmret(vm, VMRET_GUEST_STATE_UNSUPPORTED);
                        return 1;
                }
                vm->use_bndcfgs = use_bndcfgs;
                vm->entry_work |= (EW_RELOAD_VMEXIT_CTRLS
                                   | EW_RELOAD_VMENTRY_CTRLS);
        }
        return 0;
}

static void
update_fpu_disabled(vm_t *vm)
{
        vmxmon_t *x = vm->com;
        vm->fpu_disabled = (x->active_vmxmon_features
                            & VMXMON_FEATURE_FPU_DISABLE);
}

static int
update_extended_state(vm_t *vm)
{
        vmxmon_t *x = vm->com;

        if (vm->dirty & (REGBIT_FEATURES | REGBIT_MSRS)) {
                if (update_use_xsave(vm, x->xcr0))
                        return 1;
                if (update_use_bndcfgs(vm))
                        return 1;
        }

        if (!vm->use_xsave && mpx_enabled(vm)) {
                /* if Intel® MPX is currently enabled then we cannot run without
                   XSAVE support since Intel® MPX instructions become NOPs */
                set_vmret(vm, VMRET_GUEST_STATE_UNSUPPORTED);
                return 1;
        }

        if (vm->dirty & REGBIT_FEATURES)
                update_fpu_disabled(vm);

        return 0;
}

static void
update_use_vpid(vm_t *vm)
{
        vmxmon_t *x = vm->com;
        bool use_vpid = (x->active_vmxmon_features & VMXMON_FEATURE_VPID)
                && mp.enable_vpid && gi.has_vpid;

        if (use_vpid == vm->use_vpid)
                return;

        vm->entry_work |= EW_RELOAD_PROC2 | (use_vpid ? EW_FLUSH_VPID : 0);
        vm->use_vpid = use_vpid;
}

static void
update_use_pku(vm_t *vm)
{
        vmxmon_t *x = vm->com;
        bool use_pku = gi.has_pku
                && (x->active_vmxmon_features & VMXMON_FEATURE_PKU);
        if (vm->use_pku != use_pku) {
                vm->use_pku = use_pku;
                /* update handling of CR4.OSPKE (depends on vm->use_pku) */
                vm->entry_work |= EW_RELOAD_CR4;
        }
}

static inline int
update_use_mbe(vm_t *vm)
{
        vmxmon_t *x = vm->com;

        bool use_mbe = x->vmx_non_root_mode
                && (x->vmcs_proc2_ctls & PROC2_MODE_BASED_EXEC_EPT);

        if (use_mbe != vm->use_mbe) {
                vm->use_mbe = use_mbe;
                vm->entry_work |= EW_RELOAD_PROC2;
        }

        return 0;
}

static bool
can_use_nested_ept(vm_t *vm)
{
        vmxmon_t *x = vm->com;

        /* Nested EPT with paging tables prepared for 5-level tablewalk is not
           tested to work. The case of mismatching walk lengths will not work
           correctly. For now, prevent nested EPT from being used if this vm
           was set up with TARGET_WANTS_5_LEVEL_PAGES */
        bool vm_is_setup_in_incompatible_manner = vm->use_5_level_pages;

        return x->vmx_non_root_mode && x->vmx_non_root_guest_ept
                && !vm_is_setup_in_incompatible_manner;
}

static void
update_ept_setting(vm_t *vm)
{
        vmxmon_t *x = vm->com;
        bool ept_blocked_by_errata = 
                (x->active_vmxmon_features & VMXMON_FEATURE_1GB_NOFAULT_ERRATA)
                && (x->has_1gb_mmu_page && IS_MMU_MODE_LONG(vm->mmu_mode));
        bool ug_feature = gi.has_unrestricted_guest
                && (x->active_vmxmon_features
                    & VMXMON_FEATURE_UNRESTRICTED_GUEST)
                && mp.enable_ept && mp.enable_ug;
        bool pku_active = (x->cr4 & CR4_OSPKE) && (x->pkru != 0);
        bool use_nested_ept = can_use_nested_ept(vm);
        bool use_ept = (x->active_vmxmon_features & VMXMON_FEATURE_EPT)
                && !tlb_has_lin_bps(vm)
                && (ug_feature || !IS_MMU_MODE_NO_PAGING(vm->mmu_mode))
                && mp.enable_ept && gi.has_ept
                && (!use_nested_ept
                    || (x->active_vmxmon_features & VMXMON_FEATURE_NESTED_EPT))
                && (!vm->use_mbe || (use_nested_ept && gi.has_ept_mbe))
                && !ept_blocked_by_errata
                && !(pku_active && !vm->use_pku)
                && !(x->cr3 & BITMASK(51, gi.host_max_physaddr));

        if (use_ept) {
                if (vm->use_nested_ept != use_nested_ept) {
                        vm->use_nested_ept = use_nested_ept;
                        vm->entry_work |= EW_UPDATE_EPT_PTR;
                }
                if (use_nested_ept && vm->guest_eptp != x->vmx_eptp) {
                        vm->guest_eptp = x->vmx_eptp;
                        change_guest_ept(vm);
                        vm->entry_work |= EW_UPDATE_EPT_PTR;
                }
        }
        if (use_ept == vm->use_ept)
                return;

        if (!use_ept) {
                /* we need to read out cr3 if we are turning off EPT */
                get_regs(vm, REGBIT_CR3);
        }

        /* update unrestricted guest setting */
        vm->use_ug = use_ept && ug_feature;

        if (use_ept) {
                /* tlb is not kept in sync while in EPT mode */
                tlb_clear(vm);
                vm->entry_work |= EW_CR3_RELOAD;
        } else {
                /* need to present cr3 to the tlb layer */
                vm->entry_work |= EW_CR3_DIRTY;
        }
        vm->entry_work |= EW_EPT_USAGE_SWITCH;
        vm->use_ept = use_ept;
}

/* called before the cpu is locked down - don't touch the VMCS */
static inline int
update_mmu_state(vm_t *vm)
{
        vmxmon_t *x = vm->com;

        if (x->clear_tlb) {
                tlb_clear(vm);
                x->clear_tlb = false;
        }

        /* CR0.PG, CR0.PE, CR0.WP, CR4.PSE, CR4.PAE, CR4.LA57, EFER
           affect MMU mode */
        if (vm->dirty & (REGBIT_CR | REGBIT_MSRS)) {
                if (mmu_mode_change(vm)) {
                        vm->entry_work |= EW_CR3_DIRTY;
                        update_ept_setting(vm);
                }
        }

        /* update vm->guest_cr3 */
        if (vm->dirty & REGBIT_CR3) {
                if (x->cr3 & BITMASK(51, vm->maxphyaddr)) {
                        set_vmret(vm, VMRET_INV_GUEST_STATE);
                        return 1;
                }
                if (vm->guest_cr3 != x->cr3) {
                        vm->guest_cr3 = x->cr3;
                        vm->entry_work |= EW_CR3_DIRTY;
                }
        }

        /* handle PAE PDPTE reload */
        if (IS_MMU_MODE_PAE(vm->mmu_mode)
            && ((vm->dirty & REGBIT_CR3) || (vm->entry_work & EW_CR3_DIRTY))) {
                bool modified;
                if (mmu_reload_pae_pdpte(vm, &modified))
                        return 1;
                if (modified)
                        vm->entry_work |= EW_CR3_DIRTY;
        }

        /* cr3 or PDPTE entries has changed */
        if (vm->entry_work & EW_CR3_DIRTY) {
                if (!vm->use_ept && tlb_activate_cr3(vm))
                        return 1;
                vm->entry_work &= ~EW_CR3_DIRTY;
                vm->entry_work |= EW_CR3_RELOAD;
        }

        handle_tlb_flushes_no_ept(vm);
        return 0;
}

static inline int
update_paging_settings(vm_t *vm)
{
        if (vm->dirty & REGBIT_FEATURES) {
                update_use_vpid(vm);
                update_use_pku(vm);
        }

        if (update_use_mbe(vm))
                return 1;

        if (vm->dirty & (REGBIT_BREAKPOINTS | REGBIT_FEATURES))
                update_ept_setting(vm);

        if (update_mmu_state(vm))
                return 1;

        return 0;
}

/* called before the cpu is locked down - VMCS not available */
static inline int
early_handle_vm_dirty(vm_t *vm)
{
        vmxmon_t *x = vm->com;
        if ((vm->dirty & REGBIT_SEGS) && bios_bug_prevents_execution(vm)) {
                set_vmret(vm, VMRET_GUEST_STATE_UNSUPPORTED);
                return 1;
        }

        if (update_extended_state(vm))
                return 1;

        if (update_paging_settings(vm))
                return 1;

        if (vm->dirty & (REGBIT_FEATURES | REGBIT_INTERCEPTS))
                update_use_tpr_limit(vm);

        if (update_svmcs_setting(vm))
                return 1;

        if (x->vmx_non_root_mode != vm->non_root_guest) {
                vm->non_root_guest = x->vmx_non_root_mode;
                vm->entry_work |= EW_RELOAD_APIC_PAGE;
        }

        if (x->ept_viol_eptp) {
                handle_ept_viol_flush(vm, x->ept_viol_gpa, x->ept_viol_access,
                                      x->ept_viol_eptp);
                x->ept_viol_eptp = 0;
        }
        if (x->invept_ent != 0) {
                handle_invept_flush(vm, x->invept_ent);
                x->invept_ent = 0;
        }
        return 0;
}

static inline int
check_target_control_registers(vm_t *vm)
{
        vmxmon_t *x = vm->com;

        if (!(x->cr0 & CR0_PE) && !vm->use_ug) {
                set_vmret(vm, VMRET_CR0_VALUE_UNSUPPORTED);
                return 1;
        }

        bool wants_5_level_paging = x->cr4 & CR4_LA57;
        bool requested_5_level_paging = x->active_vmxmon_features
                & VMXMON_FEATURE_5_LEVEL_PAGING;
        bool allows_5_level_paging = requested_5_level_paging &&
                gi.has_5_level_paging;

        if (wants_5_level_paging && !allows_5_level_paging) {
                set_vmret(vm, VMRET_GUEST_STATE_UNSUPPORTED);
                return 1;
        }

        return 0;
}

static inline void
trace_buffer_allocation(vm_t *vm)
{
        vmxmon_t *x = vm->com;
        if (x->debug_trace && !vm->vmx_trace) {
                vm->vmx_trace = x_malloc(sizeof *vm->vmx_trace);
                if (vm->vmx_trace)
                        wrap_memset(vm->vmx_trace, 0, sizeof *vm->vmx_trace);
        } else if (!x->debug_trace && vm->vmx_trace) {
                x_free(vm->vmx_trace);
                vm->vmx_trace = NULL;
        }
}

static int
dbg_trace(vm_t *vm, int is_entry)
{
        vmxmon_t *x = vm->com;
        if (!x->debug_trace || !vm->vmx_trace)
                return 0;

        vmx_trace_t *t = vm->vmx_trace;
        const unsigned n_entries = sizeof t->entries/sizeof t->entries[0];
        if ((x->debug_trace & 2) && t->n == n_entries) {
                set_vmret(vm, VMRET_TRACE_BUF_FULL);
                return 1;
        }

        vmx_trace_entry_t *te = &t->entries[t->n % n_entries];
        if (is_entry) {
                te->ip_entry = vmread(VMCS_GUEST_RIP);
                te->step_entry = x->abs_time + x->steps;
        } else {
                te->ip_exit = vmread(VMCS_GUEST_RIP);
                te->elapsed = x->abs_time + x->steps - te->step_entry;
                te->exit_reason = vmread(VMCS_EXIT_REASON);
                if (te->exit_reason == 0) {
                        unsigned info = vmread(VMCS_VMEXIT_INT_INFO);
                        u32 qual = vmread(VMCS_EXIT_QUAL);
                        int vec = info & 0xff;
                        te->debug = ((u64)vec << 32) | qual;
                } else {
                        te->debug = 0;
                }
                t->n++;
        }

        if (!(x->debug_trace & 4))
                return 0;

        printm("%s %4lld @ %08zx:", is_entry ? "" : " ---> ",
               x->abs_time + x->steps, vmread(VMCS_GUEST_RIP));

        if (is_entry) {
                printm(" run %5d ", vm->steps);
                if (vm->entry_work & EW_INJECT) {
                        printm(" INJECT %d:0x%x ",
                               (int)x->injection.type,
                               (int)x->injection.vector);
                }
        } else {
                int elapsed = te->elapsed;
                printm(" + %-5d", elapsed);
                printm(" [%2zx:%08zx/0x%zx/%08zx]\n",
                       vmread(VMCS_EXIT_REASON),
                       vmread(VMCS_VMEXIT_INT_INFO),
                       vmread(VMCS_VMEXIT_INT_ECODE),
                       vmread(VMCS_EXIT_QUAL));
        }
        return 0;
}

void
dev_ioctl_get_trace(vm_t *vm, vmx_trace_t *t)
{
        wrap_memset(t, 0, sizeof *t);

        wrap_mutex_lock_nested(vm->vm_lock, VM_LEVEL);
        if (vm->vmx_trace) {
                vmx_trace_t *st = vm->vmx_trace;
                const int vsize = sizeof st->entries / sizeof st->entries[0];
                int cur = st->n - vsize;
                int end = st->n;

                if (cur < 0)
                        cur = 0;
                while (cur < end)
                        t->entries[t->n++] = st->entries[cur++ % vsize];
                st->n = 0;
        }
        wrap_mutex_unlock(vm->vm_lock);
}


static inline void
check_memory_limits(vm_t *vm)
{
        /* this limit is mostly here to reclaim unused segments */
        tlb_memory_limit_prune(vm);

        /* make sure we don't lock down too much memory */
        vmxpage_memory_limit_prune(vm);
}

bool
handle_nmi(void)
{
        cpu_state_t *cs = gi.cpu_state[wrap_get_cpu_num()];
        dbg_vmcs_nmi(cs->vm);
        if (unlikely(!cs))
                return false;
        if (unlikely(!mp.use_nmi))
                return false;
        if (!cs->perf_event && cs->pmc.get_and_clear_overflow(cs)) {
                if (LINUX)
                        wrap_apic_write(cs->lapic_base,
                                        APIC_LVTPC, APIC_DM_NMI);
                return true;
        } else if (WINDOWS && cs->claim_nmi) {
                /* Claim one NMI occurring while the CPU is pinned.
                   This is a workaround for "leaking" NMIs which would
                   otherwise bring down the system. */
                cs->claim_nmi = false;
                return true;
        }
        return false;
}

static u32
load_apic_lvtpc_at_entry(vm_t *vm)
{
        cpu_state_t *cs = vm->cs;
        if (unlikely(!vm->use_perf_counters))
                return 0;

        if (WINDOWS)
                cs->claim_nmi = true;

        if (!LINUX) {
                u32 ret = wrap_apic_read(cs->lapic_base, APIC_LVTPC);
                unsigned vec = mp.use_nmi ? APIC_DM_NMI : cs->apic_error_vec;
                wrap_apic_write(cs->lapic_base, APIC_LVTPC, vec);
                return ret;
        }

        if (LINUX && unlikely(!mp.use_nmi)) {
                u32 ret = wrap_apic_read(cs->lapic_base, APIC_LVTPC);
                wrap_apic_write(cs->lapic_base, APIC_LVTPC, cs->apic_error_vec);
                return ret;
        }
        return 0;
}

static void
restore_apic_lvtpc(vm_t *vm, unsigned old_lvtpc)
{
        const cpu_state_t *cs = vm->cs;
        if (unlikely(!vm->use_perf_counters))
                return;
        if (!LINUX || unlikely(!mp.use_nmi))
                wrap_apic_write(cs->lapic_base, APIC_LVTPC, old_lvtpc);
}

/* returns false if execution is blocked by vmxmon_signal */
static inline bool
set_running_state(vmxmon_t *x)
{
        if (x->signaled)
                return false;
        x->running = true;
        return true;
}

static inline void
clear_running_state(vmxmon_t *x)
{
        x->running = false;
}

static inline bool
is_signalled(vmxmon_t *x)
{
        return x->signaled;
}

static inline int
update_maxphyaddr(vm_t *vm)
{
        vmxmon_t *x = vm->com;
        /* Should we flush page mappings if maxphyaddr changed? */
        vm->maxphyaddr = x->maxphyaddr;
        if (vm->maxphyaddr > 52) {
                set_vmret(vm, VMRET_GUEST_STATE_UNSUPPORTED);
                return 1;
        }
        return 0;
}

static void
vmx_enter(vm_t *vm)
{
        vmxmon_t *x = vm->com;
        clear_vmret(vm);

        /* Update vm->dirty early (used from the early_out exit path). */
        vm->dirty |= x->dirty | (mp.reload_all_regs ? x->insync : 0);

        if (update_maxphyaddr(vm))
                goto early_out;

        if (vm->entry_block != 0) {
                set_vmret_(vm, VMRET_NOP);
                goto early_out;
        }
        trace_buffer_allocation(vm);
        check_memory_limits(vm);

        tphys_perform_lazy_flushing(vm->pspace);

        if (is_signalled(x)) {
                set_vmret_(vm, VMRET_SIGNALED);
                goto early_out;
        }
        if (early_handle_vm_dirty(vm))
                goto early_out;

        if (check_target_control_registers(vm))
                goto early_out;

        unsigned ipl = 0;
        if (pin_vmcs(vm, &ipl))
                goto out;

        /* save host state */
        user_segs_t usegs;
        save_user_segments(&usegs);

        setup_direct_rdtsc(vm);

        for (int cnt = 0;; cnt++) {
                /* avoid infinite loops */
                if (cnt > 1000 || x->debug_stop) {
                        set_vmret(vm, VMRET_NOP);
                        break;
                }

                if (timer_setup(vm))
                        break;

                do_entry_work(vm);

                if (dbg_trace(vm, 1))
                        goto out_no_entry;

                if (vmcs_assert_static_host_state(vm))
                        goto out_no_entry;

                if (check_vm_instr_err(vm))
                        goto out_no_entry;

                BUMP(PERFCTR_VMX_SWITCH);

                if (!set_running_state(x)) {
                        set_vmret_(vm, VMRET_SIGNALED);
                        goto out_no_entry;
                }

                x->insync = 0;

                /* Interrupts have to be disabled before vmxmon swaps
                 * guest/host CR2. Otherwise interrupt handler could
                 * potentially cause #PF and change CR2. */
                unsigned long irq_flags;
                wrap_local_irq_save(&irq_flags);
                dbg_vmcs_entry(vm);
                unsigned save_lvtpc = load_apic_lvtpc_at_entry(vm);
                guest_xcr0_switch_in(vm);

                u64 tsc_start = load_guest_tsc(vm);

                do_vmlaunch(&vm->host_state, x, vm->launched);

                guest_xcr0_switch_out(vm);
                clear_running_state(x);

                restore_apic_lvtpc(vm, save_lvtpc);
                dbg_vmcs_exit(vm);
                reload_task_register(vm);
                restore_user_segments(&usegs);

                handle_vmexit_early(vm);
                wrap_local_irq_restore(irq_flags);

                assert_vmcs(vm);

                handle_step_counter(vm);
                update_elapsed_tsc_ticks(vm, tsc_start);

                dbg_trace(vm, 0);

                if (handle_entry_failures(vm))
                        goto out_no_entry;

                if (handle_guest_state_failures(vm))
                        goto out_no_entry;

                /* entry succeeded */
                vm->launched = true;

                clear_injections(vm);
                handle_idt_vectoring(vm);
                handle_vmexit(vm, &ipl);

                if (get_vmret(vm) != VMRET_AGAIN)
                        break;
                /* the VMCS could be unpinned at this point */
                clear_vmret(vm);

                /* asynchronous interrupt? */ 
                if (is_signalled(x)) {
                        set_vmret_(vm, VMRET_SIGNALED);
                        break;
                }

                /* play nice */
                if (wrap_need_resched(vm->elapsed)) {
                        unpin_vmcs(vm, ipl);
                        wrap_schedule();
                        if (pin_vmcs(vm, &ipl))
                                break;
                }
                if (wrap_signal_pending()) {
                        set_vmret_(vm, VMRET_NOP);
                        break;
                }
                continue;
out_no_entry:
                /* VMCS injection field does not carry state, and
                   is assumed to be 0 on entry. We need to clear it
                   if the vmentry fails. */
                vmwrite(VMCS_VMENTRY_INT_INFO_FIELD, 0);
                break;
        }
out:
        unpin_vmcs(vm, ipl);

early_out:
        x->_vmret = get_vmret(vm);
        x->dirty = vm->dirty;
        x->insync |= x->dirty;
        bump_vmret(vm, x->_vmret);
        BUMP(PERFCTR_USERSPACE_SWITCH);
}

long
dev_ioctl_enter(vm_t *vm)
{
        wrap_mutex_lock_nested(vm->vm_lock, VM_LEVEL);
        tphys_lock(vm->pspace);
        if (vm->com)
                vmx_enter(vm);
        tphys_unlock(vm->pspace);
        wrap_mutex_unlock(vm->vm_lock);
        return 0;
}

void
vmx_get_regs(vm_t *vm, u32 regbits)
{
        vmxmon_t *x = vm->com;

        BUMP(PERFCTR_GET_REGS);
        vm->dirty |= x->dirty;

        get_regs(vm, regbits);

        x->dirty = vm->dirty;
        x->insync |= x->dirty;
}

long
dev_ioctl_get_regs(vm_t *vm, u32 regbits)
{
        wrap_mutex_lock_nested(vm->vm_lock, VM_LEVEL);
        tphys_lock(vm->pspace);
        vmx_get_regs(vm, regbits);
        tphys_unlock(vm->pspace);
        wrap_mutex_unlock(vm->vm_lock);
        return 0;
}

long
dev_ioctl_signal(vm_t *vm)
{
        vmxmon_t *x = vm->com;
        if (x && x->running) {
                int vm_cpu = vm->cpu;
                if (vm_cpu != -1)
                        ping_cpu(vm_cpu);
        }
        return 0;
}

/************************************************************************/
/*	VM initialization						*/
/************************************************************************/


static int
alloc_vmcs_structs(vm_t *vm)
{
        if (!alloc_mpage_native(&vm->msr_page))
                return 1;
        if (!alloc_mpage_native(&vm->virt_apic_page))
                return 1;

        if (gi.has_msr_bitmap) {
                if (!alloc_mpage_native(&vm->msr_bitmap_page))
                        return 1;
                /* initially set to VM-exit on all MSRs */
                wrap_memset(vm->msr_bitmap_page.p, 0xff, 0x1000);
        }
        if (make_vmcs_page(&vm->vmcs, false))
                return 1;
        vm->vmcs_phys = mpage_phys(&vm->vmcs);

        if (gi.has_svmcs) {
                if (make_vmcs_page(&vm->linked_vmcs, true))
                        return 1;
                vm->linked_vmcs_phys = mpage_phys(&vm->linked_vmcs);

                if (!alloc_mpage(&vm->vmread_bitmap))
                        return 1;
                if (!alloc_mpage(&vm->vmwrite_bitmap))
                        return 1;

                vm->vmread_bitmap_phys = mpage_phys(&vm->vmread_bitmap);
                vm->vmwrite_bitmap_phys = mpage_phys(&vm->vmwrite_bitmap);
                wrap_memset(vm->vmread_bitmap.p, 0xff, 0x1000);
                wrap_memset(vm->vmwrite_bitmap.p, 0xff, 0x1000);
        }
        return 0;
}

static void
free_vmcs_structs(vm_t *vm)
{
        if (mpage_allocated(&vm->vmcs))
                free_mpage(&vm->vmcs);
        if (mpage_allocated(&vm->msr_page))
                free_mpage(&vm->msr_page);
        if (mpage_allocated(&vm->virt_apic_page))
                free_mpage(&vm->virt_apic_page);
        if (mpage_allocated(&vm->msr_bitmap_page))
                free_mpage(&vm->msr_bitmap_page);
        if (mpage_allocated(&vm->linked_vmcs))
                free_mpage(&vm->linked_vmcs);
        if (mpage_allocated(&vm->vmread_bitmap))
                free_mpage(&vm->vmread_bitmap);
        if (mpage_allocated(&vm->vmwrite_bitmap))
                free_mpage(&vm->vmwrite_bitmap);
}

int
vmx_init(vm_t *vm)
{
        vm->cpu = -1;
        vm->dirty = REGBIT_ALL;

        if (alloc_vmcs_structs(vm)) {
                free_vmcs_structs(vm);
                return 1;
        }

        if (dbg_vmcs_init(vm)) {
                free_vmcs_structs(vm);
                return 1;
        }

        fw_dbg_vm_created(vm);
        vm->vmx_initialized = true;
        return 0;
}

void
vmx_suspend(vm_t *vm)
{
        if (vm->cpu != -1)
                fetch_state_from_vm_cpu(vm);
}

void
vmx_cleanup(vm_t *vm)
{
        if (!vm->vmx_initialized)
                return;

        fw_dbg_vm_destroyed(vm);

        vmx_suspend(vm);

        dbg_vmcs_destroy(vm);
        if (vm->vmx_trace) {
                x_free(vm->vmx_trace);
                vm->vmx_trace = NULL;
        }
        free_vmcs_structs(vm);
        vm->vmx_initialized = false;
}


/************************************************************************/
/*	global initialization / cleanup					*/
/************************************************************************/

static void
x_release_cpuinfo(void *the_cs, int cpu)
{
        cpu_state_t *cs = the_cs;
        if (cs->lapic_base)
                wrap_apic_unmap(cs->lapic_base);
}

static void
free_cpuinfo(cpu_state_t *cs)
{
        wrap_BUG_ON(cs->vmx_is_on);

        run_on_cpu(cs->cpu, x_release_cpuinfo, cs);
        free_mpage(&cs->vmxon_page);
        x_free(cs);
}

static void
x_init_cpuinfo(void *the_cs, int cpu)
{
        cpu_state_t *cs = the_cs;
        wrap_BUG_ON(cpu != cs->cpu);

        if (!(cs->lapic_base = wrap_apic_map()))
                return;
        cs->thread = get_thread_num(cs);

        /* check if SMM_PERF_FREEZE if available (SMM can prevent accurate
           step counting without this feature, bug #6646). On levante,
           enabling this flag leads to VMX entry failures despite the feature
           reported as available - probably a hardware bug. As a workaround,
           we turn off the feature if it appears it causes entry failures. */
        cs->use_smm_perf_freeze =
                (cpuid_ecx(0x1) & CPUID_1_ECX_PERF_CAPABILITIES)
                && (rdmsr(MSR_IA32_PERF_CAPABILITIES) & BIT(12));

        unsigned reg = APIC_LVERR;
        cs->apic_error_vec = wrap_apic_read(cs->lapic_base, reg) & 0xff;

        pctr_setup_cpu_info(cs);
}

static bool
valid_cpuinfo(cpu_state_t *cs)
{
        return cs->lapic_base;
}

static cpu_state_t *
alloc_cpuinfo(int cpu)
{
        cpu_state_t *cs = x_malloc(sizeof *cs);
        if (!cs)
                return NULL;
        wrap_memset(cs, 0, sizeof *cs);
        cs->cpu = cpu;
        if (make_vmcs_page(&cs->vmxon_page, false)) {
                x_free(cs);
                return NULL;
        }
        run_on_cpu(cpu, x_init_cpuinfo, cs);
        if (!valid_cpuinfo(cs)) {
                free_cpuinfo(cs);
                return NULL;
        }
        return cs;
}

int
vmx_cpu_up(int cpu)
{
        cpu_state_t *cs = alloc_cpuinfo(cpu);
        if (!cs)
                return 1;
        wrap_BUG_ON(gi.cpu_state[cpu]);
        gi.cpu_state[cpu] = cs;
        enable_perf_counting(cs);
        return 0;
}

void
vmx_cpu_down(int cpu)
{
        cpu_state_t *cs = gi.cpu_state[cpu];
        if (!cs)
                return;
        gi.cpu_state[cpu] = NULL;
        release_cpu_perf_event(cs->perf_event);
        free_cpuinfo(cs);
}

/* architectural performance monitoring version */
static int
get_apm_version(void)
{
        unsigned eax, ebx, ecx, edx, evbits, vers;

        if (cpuid_eax(0) < 0xa)
                return 0;
        cpuid_cla(0x0a, &eax, &ebx, &ecx, &edx);
        vers = (eax & 0xff);
        evbits = (eax >> 24);

        if (vers == 0)
                return 0;

        /* INST_RETIRED event available? */
        if (evbits < 2 || (ebx & BIT(1)))
                return 0;

        /* edx == 0 on early rev 2 CPUs */
        if (vers > 2 && edx != 0) {
                unsigned n_fctrs = (edx & 0x1f);
                if (n_fctrs == 0)
                        return 1;  /* fallback to version 1 facilities */
        }
        return vers;
}

static bool
host_has_5_level_paging(void)
{
        u32 eax = 0;
        u32 ebx = 0;
        u32 ecx = 0;
        u32 edx = 0;
        cpuid_cnt(7, 0, &eax, &ebx, &ecx, &edx);
        bool has_la57 = ecx & BIT(16);
        bool has_ept5 = gi.ept_vpid_cap & BIT(7);
        return has_la57 && has_ept5;
}

static void
init_vmx_global_info(void)
{
        /* collect information about the VMX implementation */
        u64 v = rdmsr(MSR_VMX_BASIC);
        gi.vmx_mem_type = VMX_BASIC_MEM_TYPE(v);
        gi.vmx_rev = VMX_BASIC_REV_ID(v);
        bool has_true_ctls = (v & VMX_BASIC_HAS_TRUE_CTLS);

        gi.has_ins_outs_reporting = (v & VMX_BASIC_INS_OUTS_REPORTING);

        v = rdmsr(has_true_ctls ?
                    MSR_VMX_TRUE_PINBASED_CTLS : MSR_VMX_PINBASED_CTLS);
        gi.pin_based_0_allowed = (u32)v;
        gi.pin_based_1_allowed = v >> 32;

        v = rdmsr(has_true_ctls ?
                    MSR_VMX_TRUE_PROCBASED_CTLS : MSR_VMX_PROCBASED_CTLS);
        gi.proc_based_0_allowed = (u32)v;
        gi.proc_based_1_allowed = v >> 32;

        gi.cr3_load_always_exits =
                (gi.proc_based_0_allowed & PROC_CR3_LOAD_EXITING);
        gi.cr3_store_always_exits =
                (gi.proc_based_0_allowed & PROC_CR3_STORE_EXITING);

        gi.has_proc2 = (gi.proc_based_1_allowed & PROC_ACTIVATE_SEC_CTRLS);

        /* does this processor support secondary controls? */
        if (gi.has_proc2) {
                v = rdmsr(MSR_VMX_PROCBASED_CTLS2);
                gi.proc_based2_0_allowed = (u32)v;
                gi.proc_based2_1_allowed = v >> 32;
        } else {
                /* no.. don't access the MSR */
                gi.proc_based2_0_allowed = 0;
                gi.proc_based2_1_allowed = 0;
        }

        /* get EPT_VPID_CAP, available on newer revs */
        gi.ept_vpid_cap = 0;
        if (gi.proc_based2_1_allowed & (BIT(1) | BIT(5)))
                gi.ept_vpid_cap = rdmsr(MSR_VMX_EPT_VPID_CAP);

        v = rdmsr(has_true_ctls ?
                    MSR_VMX_TRUE_EXIT_CTLS : MSR_VMX_EXIT_CTLS);
        gi.vmexit_0_allowed = (u32)v;
        gi.vmexit_1_allowed = v >> 32;

        v = rdmsr(has_true_ctls ?
                    MSR_VMX_TRUE_ENTRY_CTLS : MSR_VMX_ENTRY_CTLS);
        gi.vmentry_0_allowed = (u32)v;
        gi.vmentry_1_allowed = v >> 32;

        /* (FIXED0, FIXED1): (0,0) = 0, (1,1) = 1, (0, 1) = flexible */
        gi.cr0_fixed_0 = rdmsr(MSR_VMX_CR0_FIXED0);
        gi.cr0_fixed_1 = rdmsr(MSR_VMX_CR0_FIXED1);
        gi.cr4_fixed_0 = rdmsr(MSR_VMX_CR4_FIXED0);
        gi.cr4_fixed_1 = rdmsr(MSR_VMX_CR4_FIXED1);

        if (gi.proc_based2_1_allowed & PROC2_ENABLE_VMFUNC)
                gi.vm_functions = rdmsr(MSR_VMX_VMFUNC);

        gi.has_efer_support = (gi.vmentry_1_allowed & VMENTRY_LOAD_IA32_EFER)
                && (gi.vmexit_1_allowed & VMEXIT_LOAD_IA32_EFER);

        gi.has_bndcfgs_support
                = (gi.vmentry_1_allowed & VMENTRY_LOAD_IA32_BNDCFGS)
                && (gi.vmexit_1_allowed & VMEXIT_CLEAR_IA32_BNDCFGS);

        const u64 vpid_cap = (BIT(32) | BIT(40)
                              | BIT(41) | BIT(42) | BIT(43));
        gi.has_vpid = (gi.proc_based2_1_allowed & PROC2_ENABLE_VPID)
                && ((gi.ept_vpid_cap & vpid_cap) == vpid_cap);

        const u64 ept_cap = BIT(20) | BIT(25) | BIT(26);
        gi.has_ept = gi.has_vpid
                && (gi.proc_based2_1_allowed & PROC2_ENABLE_EPT)
                && ((gi.ept_vpid_cap & ept_cap) == ept_cap);

        const u64 advanced_ept_vmexit_info = BIT(22);
        gi.has_ept_mbe = (gi.proc_based2_1_allowed & PROC2_MODE_BASED_EXEC_EPT)
                && (gi.ept_vpid_cap & advanced_ept_vmexit_info);

        gi.has_desc_table_exiting = (gi.proc_based2_1_allowed
                                     & PROC2_DESC_TABLE_EXITING);

        gi.has_unrestricted_guest = gi.has_ept
                && (gi.proc_based2_1_allowed & PROC2_UNRESTRICTED_GUEST);

        /* determine if we need to save/restore PAT */
        gi.pat_handling_needed = (rdmsr(MSR_IA32_CR_PAT) & 0xff) != 0x6;
        gi.use_vmcs_pat_handling = gi.pat_handling_needed
                && (gi.vmentry_1_allowed & VMENTRY_LOAD_IA32_PAT)
                && (gi.vmexit_1_allowed & VMEXIT_LOAD_IA32_PAT);

        /* check if we support NMI window exiting */
        gi.has_virtual_nmi = (gi.vmexit_1_allowed & VMEXIT_NMI_WINDOW)
                && (gi.pin_based_1_allowed & PIN_VIRTUAL_NMIS);

        gi.has_msr_bitmap = (gi.proc_based_1_allowed & PROC_USE_MSR_BITMAPS);

        /* check if we support vmcs IA32_PERF_GLOBAL_CTRL handling */
        gi.has_vmcs_pctr_handling =
                (gi.vmexit_1_allowed & VMEXIT_LOAD_IA32_PERF_GLOBAL_CTRL)
                && (gi.vmentry_1_allowed & VMENTRY_LOAD_IA32_PERF_GLOBAL_CTRL);

        gi.has_monitor_trap_flag =
                (gi.proc_based_1_allowed & PROC_MONITOR_TRAP_FLAG);

        gi.has_1gb_pages = cpu_has_1gb_pages();
        gi.has_avx = cpu_has_avx();
        gi.has_xsave = cpu_has_xsave();
        gi.osxsave_enabled = osxsave_enabled();
        gi.has_lzcnt = cpu_has_lzcnt();
        gi.has_bmi1 = cpu_has_bmi1();
        gi.has_fsgsbase = cpu_has_fsgsbase();
        gi.has_pku = cpu_has_pku();
        gi.ospke_enabled = ospke_enabled();
        gi.has_1gb_pages_cpu_bug = cpu_has_1gb_pages_bug();
        gi.has_fpu_csds_deprecation = cpu_has_fpu_csds_deprecation();
        gi.has_svmcs = gi.proc_based2_1_allowed & PROC2_ENABLE_SVMCS;
        gi.has_ept_execute_only = gi.ept_vpid_cap & BIT(0);
        gi.has_tsc_scaling = gi.proc_based2_1_allowed & PROC2_USE_TSC_SCALING;
        gi.has_rdtscp = cpu_has_rdtscp();
        gi.has_pebs = cpu_has_pebs();
        gi.has_16_bit_tss_bios_bug = host_has_16_bit_tss_bios_bug(); 

        gi.apm_version = get_apm_version();
        gi.host_max_physaddr = cpu_max_physaddr();

        gi.os_enabled_xsave_mask = wrap_get_os_enabled_xsave_mask();
        gi.host_xcr0_mask = gi.has_xsave ? cpu_get_xcr0_mask() : 0;
        if (gi.host_xcr0_mask & XSTATE_MPX)
                gi.xsave_bndcsr_offs = cpu_get_xsave_bndcsr_offs();

        gi.is_hyperv = cpu_is_hyperv();
        gi.has_5_level_paging = host_has_5_level_paging();
}


static void
report_if_inside_hypervisor(void)
{
        bool is_hv_bit_set = cpuid_ecx(1) & BIT(31);

        u32 h_a = 0, h_b = 0, h_c = 0, h_d = 0;
        cpuid_cla(0x40000000, &h_a, &h_b, &h_c, &h_d);

        bool is_hv_leaf_present;
        if (is_hv_bit_set) {
                is_hv_leaf_present = (
                        h_a != 0 || h_b != 0 || h_c != 0 || h_d != 0);
        } else {
                /* Get previous leaf to check that hypervisor CPUID leave is
                   indeed reports something: not just holds the value of
                   the largest CPUID leave that is supported by CPU. */
                u32 a = 0, b = 0, c = 0, d = 0;
                cpuid_cla(0x40000000 - 1, &a, &b, &c, &d);
                is_hv_leaf_present = !(a == h_a && b == h_b
                                       && c == h_c && d == h_d);
        }

        if (!is_hv_bit_set && !is_hv_leaf_present)
                return;

        char hv_id[13] = {0};
        const char *hv_vendor = NULL;
        wrap_memcpy(hv_id + 0, &h_b, 4);
        wrap_memcpy(hv_id + 4, &h_c, 4);
        wrap_memcpy(hv_id + 8, &h_d, 4);
        hv_id[12] = '\0';
        if (wrap_strcmp(hv_id, "VMwareVMware") == 0) {
                hv_vendor = "VMware";
        } else if (wrap_strcmp(hv_id, "Microsoft Hv") == 0) {
                hv_vendor = "Microsoft Hypervisor";
        } else if (wrap_strcmp(hv_id, "KVMKVMKVM") == 0) {
                hv_vendor = "KVM";
        }

        printm("Running inside a hypervisor (hypervisor present bit: %s;"
               " vendor: %s)\n",
               is_hv_bit_set ? "set" : "not set",
               hv_vendor != NULL ? hv_vendor : "unknown");
}

static void
vmxmon_banner(void)
{
        printm("v%d.%d.%d [APM-%d%s%s%s%s%s%s%s%s%s%s],"
               " memlimit: %d MB\n",
               VMXMON_MAJOR_VERSION, VMXMON_API_VERSION, VMXMON_RELEASE,
               gi.apm_version,
               gi.has_unrestricted_guest ? " UR" : "",
               gi.has_ept ? " EPT" : "",
               gi.has_ept_execute_only ? " XO" : "",
               gi.has_vpid ? " VPID" : "",
               gi.has_xsave ? " XSAVE" : "",
               gi.has_avx ? " AVX" : "",
               gi.has_1gb_pages_cpu_bug ? " 1GB_BUG" : "",
               gi.has_svmcs ? " SVMCS" : "",
               gi.is_hyperv ? " Hyper-V" : "",
               gi.has_5_level_paging ? " 5-Level paging" : "",
               mp.mem_limit_mb);

        printm("VMX v%d, pin %04x, proc %08x_%08x, xcr0 %llx\n",
               gi.vmx_rev,
               (gi.pin_based_1_allowed ^ gi.pin_based_0_allowed),
               (gi.proc_based_1_allowed ^ gi.proc_based_0_allowed),
               (gi.proc_based2_1_allowed ^ gi.proc_based2_0_allowed),
                gi.host_xcr0_mask);

        report_if_inside_hypervisor();  /* useful to know for debugging */
        dbg_print_banner();
}

int
vmx_mod_init(void)
{
        /* make sure firmware has enabled VMX */
        if (!vmx_feature_available())
                return -ENODEV;

        init_vmx_global_info();
        vmxmon_banner();

        /* make sure we have NX/XD */
        if (!cpu_has_nx()) {
                printm("Turn on support for the no-execute bit in the BIOS!\n");
                return -ENODEV;
        }

        if (!pctrs_supported()) {
                printm("No usable performance counters found.\n");
                return -ENODEV;
        }

        /* only write back memory for VMCS is supported for now */
        if (gi.vmx_mem_type != 6) {
                printm("VMCS memory type %d not supported\n", gi.vmx_mem_type);
                return -ENODEV;
        }

        /* Allocate gi.cpu_state pointer array.
         * The cpu_state is used by NMI handler. */
        unsigned size = wrap_NR_CPUS() * sizeof gi.cpu_state[0];
        gi.cpu_state = x_malloc(size);
        if (!gi.cpu_state)
                goto error;                
        wrap_memset(gi.cpu_state, 0, size);

        /* The NMI handler needs the cpu_state array to be initialized.
         * The NMI handler needs to be registered even if mp.use_nmi is false
         * because the mp.use_nmi flag can be changed on run time. */
        gi.nmi_handle = wrap_register_nmi();
        if (!gi.nmi_handle)
                goto error;

        return 0;

 error:
        x_free(gi.cpu_state);
        return -ENOMEM;
}

static void
x_vmx_off(void *unused, int cpu)
{
        vmx_off(cpu);
}

void
vmx_last_vm_closed(void)
{
        run_on_each_cpu(x_vmx_off, NULL);
}

void
vmx_all_vms_suspended(void)
{
        run_on_each_cpu(x_vmx_off, NULL);
}

void
vmx_mod_cleanup(void)
{
        wrap_unregister_nmi(gi.nmi_handle);

        if (gi.cpu_state) {
                int cpu;
                for (cpu = 0; cpu < wrap_NR_CPUS(); cpu++) {
                        vmx_cpu_down(cpu);
                }
                x_free(gi.cpu_state);
        }
}
