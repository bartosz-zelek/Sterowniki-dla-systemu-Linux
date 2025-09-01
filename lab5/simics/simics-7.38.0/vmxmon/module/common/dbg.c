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

#include "dbg.h"
#include "kernel-api.h"

#include "cpudefs.h"
#include "vmxmon.h"
#include "vm.h"
#include "vmx.h"
#include "asm-inlines.h"
#include "tlb.h"
#include "core.h"

static void
do_fetch_state(void *the_p, int cpu)
{
        vmxdbg_vmcs_t *d = the_p;
        cpu_state_t *cs = gi.cpu_state[cpu];

        if (cs == NULL || !cs->vmx_is_on)
                return;

        /* 16-bit control fields */
        if (gi.proc_based2_1_allowed & PROC2_ENABLE_VPID)
                d->vpid = vmread(VMCS_VPID);
        if (gi.pin_based_1_allowed & PIN_PROCESS_POSTED_INTERRUPTS) {
                d->posted_interrupt_notification_vector
                        = vmread(VMCS_POSTED_INTERRUPT_NOTIFICATION_VECTOR);
        }
        if (gi.proc_based2_1_allowed & PROC2_EPT_VIOLATION_VE)
                d->eptp_index = vmread(VMCS_EPTP_INDEX);

        /* 16-bit guest-state fields */
        d->guest_es_sel = vmread(VMCS_GUEST_ES_SEL);
        d->guest_cs_sel = vmread(VMCS_GUEST_CS_SEL);
        d->guest_ss_sel = vmread(VMCS_GUEST_SS_SEL);
        d->guest_ds_sel = vmread(VMCS_GUEST_DS_SEL);
        d->guest_fs_sel = vmread(VMCS_GUEST_FS_SEL);
        d->guest_gs_sel = vmread(VMCS_GUEST_GS_SEL);
        d->guest_ldtr_sel = vmread(VMCS_GUEST_LDTR_SEL);
        d->guest_tr_sel = vmread(VMCS_GUEST_TR_SEL);
        if (gi.proc_based2_1_allowed & PROC2_VIRT_INT_DELIVERY) {
                d->guest_interrupt_status
                        = vmread(VMCS_GUEST_INTERRUPT_STATUS);
        }
        if (gi.proc_based2_1_allowed & PROC2_ENABLE_PML)
                d->pml_index = vmread(VMCS_PML_INDEX);

        /* 16-bit host state fields */
        d->host_es_sel = vmread(VMCS_HOST_ES_SEL);
        d->host_cs_sel = vmread(VMCS_HOST_CS_SEL);
        d->host_ss_sel = vmread(VMCS_HOST_SS_SEL);
        d->host_ds_sel = vmread(VMCS_HOST_DS_SEL);
        d->host_fs_sel = vmread(VMCS_HOST_FS_SEL);
        d->host_gs_sel = vmread(VMCS_HOST_GS_SEL);
        d->host_tr_sel = vmread(VMCS_HOST_TR_SEL);

        /* 64-bit control fields */
        d->io_bitmap_a = vmread(VMCS_IO_BITMAP_A);
        d->io_bitmap_b = vmread(VMCS_IO_BITMAP_B);
        if (gi.proc_based_1_allowed & PROC_USE_MSR_BITMAPS)
                d->msr_bitmap_addr = vmread(VMCS_MSR_BITMAP_ADDR);
        d->vmexit_msr_store_addr = vmread(VMCS_VMEXIT_MSR_STORE_ADDR);
        d->vmexit_msr_load_addr = vmread(VMCS_VMEXIT_MSR_LOAD_ADDR);
        d->vmentry_msr_load_addr = vmread(VMCS_VMENTRY_MSR_LOAD_ADDR);
        d->executive_vmcs_ptr = vmread(VMCS_EXECUTIVE_VMCS_PTR);
        if (gi.proc_based2_1_allowed & PROC2_ENABLE_PML)
                d->pml_addr = vmread(VMCS_PML_ADDR);
        d->tsc_offset = vmread(VMCS_TSC_OFFSET);
        if (gi.proc_based_1_allowed & PROC_USE_TPR_SHADOW)
                d->virt_apic_page_addr = vmread(VMCS_VIRT_APIC_PAGE_ADDR);
        if (gi.proc_based2_1_allowed & PROC2_VIRTUALIZE_APIC)
                d->apic_access_addr = vmread(VMCS_APIC_ACCESS_ADDR);
        if (gi.proc_based2_1_allowed & PROC2_ENABLE_VMFUNC)
                d->vmfunc_ctls = vmread(VMCS_VMFUNC_CTLS);
        if (gi.proc_based2_1_allowed & PROC2_ENABLE_EPT)
                d->ept_ptr = vmread(VMCS_EPT_PTR);
        if (gi.proc_based2_1_allowed & PROC2_VIRT_INT_DELIVERY) {
                d->eoi_exit_bitmap_0 = vmread(VMCS_EOI_EXIT_BITMAP_0);
                d->eoi_exit_bitmap_1 = vmread(VMCS_EOI_EXIT_BITMAP_1);
                d->eoi_exit_bitmap_2 = vmread(VMCS_EOI_EXIT_BITMAP_2);
                d->eoi_exit_bitmap_3 = vmread(VMCS_EOI_EXIT_BITMAP_3);
        }
        if (gi.vm_functions & VMFUNC_EPTP_SWITCHING)
                d->eptp_list_addr = vmread(VMCS_EPTP_LIST_ADDR);
        if (gi.proc_based2_1_allowed & PROC2_ENABLE_SVMCS) {
                d->vmread_bitmap_addr = vmread(VMCS_VMREAD_BITMAP_ADDR);
                d->vmwrite_bitmap_addr = vmread(VMCS_VMWRITE_BITMAP_ADDR);
        }
        if (gi.proc_based2_1_allowed & PROC2_EPT_VIOLATION_VE) {
                d->virtual_exception_information
                        = vmread(VMCS_VIRTUAL_EXC_INFO);
        }
        if (gi.proc_based2_1_allowed & PROC2_ENABLE_XSAVES)
                d->xss_exiting_bitmap = vmread(VMCS_XSS_EXITING_BITMAP);
        if (gi.proc_based2_1_allowed & PROC2_ENCLS_EXITING)
                d->encls_exiting_bitmap = vmread(VMCS_ENCLS_EXITING_BITMAP);
        if (gi.proc_based2_1_allowed & PROC2_SPP_WRITE_FOR_EPT) {
                d->subpage_permission_ptr
                        = vmread(VMCS_SUBPAGE_PERMISSION_PTR);
        }
        if (gi.proc_based2_1_allowed & PROC2_USE_TSC_SCALING)
                d->tsc_multiplier = vmread(VMCS_TSC_MULTIPLIER);

        /* 64-bit read-onyl data fields */
        if (gi.proc_based2_1_allowed & PROC2_ENABLE_EPT)
                d->guest_phys_addr = vmread(VMCS_GUEST_PHYS_ADDR);

        /* 64-bit guest state fields */
        if (gi.proc_based2_1_allowed & PROC2_ENABLE_SVMCS)
                d->link_ptr = vmread(VMCS_VMCS_LINK_PTR);
        d->guest_ia32_dbgctl = vmread(VMCS_GUEST_IA32_DBGCTL);
        if (gi.vmentry_1_allowed & VMENTRY_LOAD_IA32_PAT
            || gi.vmexit_1_allowed & VMEXIT_SAVE_IA32_PAT)
                d->guest_ia32_pat = vmread(VMCS_GUEST_IA32_PAT);
        if (gi.vmentry_1_allowed & VMENTRY_LOAD_IA32_EFER
            || gi.vmexit_1_allowed & VMEXIT_SAVE_IA32_EFER)
                d->guest_ia32_efer = vmread(VMCS_GUEST_IA32_EFER);
        if (gi.vmentry_1_allowed & VMENTRY_LOAD_IA32_PERF_GLOBAL_CTRL) {
                d->guest_ia32_perf_global_ctl
                        = vmread(VMCS_GUEST_IA32_PERF_GLOBAL_CTRL);
        }
        if (gi.proc_based2_1_allowed & PROC2_ENABLE_EPT) {
                d->guest_pdpte0 = vmread(VMCS_GUEST_PDPTE0);
                d->guest_pdpte1 = vmread(VMCS_GUEST_PDPTE1);
                d->guest_pdpte2 = vmread(VMCS_GUEST_PDPTE2);
                d->guest_pdpte3 = vmread(VMCS_GUEST_PDPTE3);
        }
        if (gi.vmentry_1_allowed & VMENTRY_LOAD_IA32_BNDCFGS
            || gi.vmexit_1_allowed & VMEXIT_CLEAR_IA32_BNDCFGS)
                d->guest_ia32_bndcfgs = vmread(VMCS_GUEST_IA32_BNDCFGS);
        if (gi.vmentry_1_allowed & VMENTRY_LOAD_IA32_RTIT_CTL
            || gi.vmexit_1_allowed & VMEXIT_CLEAR_IA32_RTIT_CTL)
                d->guest_ia32_rtit_ctl = vmread(VMCS_GUEST_IA32_RTIT_CTL);

        /* 64-bit host state fields */
        if (gi.vmexit_1_allowed & VMEXIT_LOAD_IA32_PAT)
                d->host_ia32_pat = vmread(VMCS_HOST_IA32_PAT);
        if (gi.vmexit_1_allowed & VMEXIT_LOAD_IA32_EFER)
                d->host_ia32_efer = vmread(VMCS_HOST_IA32_EFER);
        if (gi.vmexit_1_allowed & VMEXIT_LOAD_IA32_PERF_GLOBAL_CTRL) {
                d->host_ia32_perf_global_ctrl
                        = vmread(VMCS_HOST_IA32_PERF_GLOBAL_CTRL);
        }

        /* 32-bit control fields */
        d->pin_based_vm_exec_ctrls = vmread(VMCS_PIN_BASED_VM_EXEC_CTRLS);
        d->proc_based_vm_exec_ctrls = vmread(VMCS_PROC_BASED_VM_EXEC_CTRLS);
        d->exc_bitmap = vmread(VMCS_EXC_BITMAP);
        d->pfault_ecode_mask = vmread(VMCS_PFAULT_ECODE_MASK);
        d->pfault_ecode_match = vmread(VMCS_PFAULT_ECODE_MATCH);
        d->cr3_target_cnt = vmread(VMCS_CR3_TARGET_CNT);
        d->vmexit_ctrls = vmread(VMCS_VMEXIT_CTRLS);
        d->vmexit_msr_store_cnt = vmread(VMCS_VMEXIT_MSR_STORE_CNT);
        d->vmexit_msr_load_cnt = vmread(VMCS_VMEXIT_MSR_LOAD_CNT);
        d->vmentry_ctrls = vmread(VMCS_VMENTRY_CTRLS);
        d->vmentry_msr_load_cnt = vmread(VMCS_VMENTRY_MSR_LOAD_CNT);
        d->vmentry_int_info_field = vmread(VMCS_VMENTRY_INT_INFO_FIELD);
        d->vmentry_exc_ecode = vmread(VMCS_VMENTRY_EXC_ECODE);
        d->vmentry_inst_len = vmread(VMCS_VMENTRY_INST_LEN);
        if (gi.proc_based_1_allowed & PROC_USE_TPR_SHADOW)
                d->tpr_threshold = vmread(VMCS_TPR_THRESHOLD);
        if (gi.proc_based_1_allowed & PROC_ACTIVATE_SEC_CTRLS) {
                d->proc_based2_vm_exec_ctrls
                        = vmread(VMCS_SEC_PROC_BASED_CTLS);
        }
        if (gi.proc_based2_1_allowed & PROC2_PAUSE_LOOP_EXITING) {
                d->ple_gap = vmread(VMCS_PLE_GAP);
                d->ple_window = vmread(VMCS_PLE_WINDOW);
        }

        /* 32-bit read-only data fields */
        d->vm_instr_err = vmread(VMCS_VM_INSTR_ERR);
        d->exit_reason = vmread(VMCS_EXIT_REASON);
        d->vmexit_int_info = vmread(VMCS_VMEXIT_INT_INFO);
        d->vmexit_int_ecode = vmread(VMCS_VMEXIT_INT_ECODE);
        d->idt_vectoring_info_field = vmread(VMCS_IDT_VECTORING_INFO_FIELD);
        d->idt_vectoring_ecode = vmread(VMCS_IDT_VECTORING_ECODE);
        d->vmexit_inst_len = vmread(VMCS_VMEXIT_INST_LEN);
        d->vmx_inst_info = vmread(VMCS_VMX_INST_INFO);

        /* 32-bit guest state fields */
        d->guest_es_limit = vmread(VMCS_GUEST_ES_LIMIT);
        d->guest_cs_limit = vmread(VMCS_GUEST_CS_LIMIT);
        d->guest_ds_limit = vmread(VMCS_GUEST_DS_LIMIT);
        d->guest_ss_limit = vmread(VMCS_GUEST_SS_LIMIT);
        d->guest_fs_limit = vmread(VMCS_GUEST_FS_LIMIT);
        d->guest_gs_limit = vmread(VMCS_GUEST_GS_LIMIT);
        d->guest_ldtr_limit = vmread(VMCS_GUEST_LDTR_LIMIT);
        d->guest_tr_limit = vmread(VMCS_GUEST_TR_LIMIT);
        d->guest_gdtr_limit = vmread(VMCS_GUEST_GDTR_LIMIT);
        d->guest_idtr_limit = vmread(VMCS_GUEST_IDTR_LIMIT);
        d->guest_es_access_rights = vmread(VMCS_GUEST_ES_ACCESS_RIGHTS);
        d->guest_cs_access_rights = vmread(VMCS_GUEST_CS_ACCESS_RIGHTS);
        d->guest_ds_access_rights = vmread(VMCS_GUEST_DS_ACCESS_RIGHTS);
        d->guest_fs_access_rights = vmread(VMCS_GUEST_FS_ACCESS_RIGHTS);
        d->guest_gs_access_rights = vmread(VMCS_GUEST_GS_ACCESS_RIGHTS);
        d->guest_ss_access_rights = vmread(VMCS_GUEST_SS_ACCESS_RIGHTS);
        d->guest_ldtr_access_rights = vmread(VMCS_GUEST_LDTR_ACCESS_RIGHTS);
        d->guest_tr_access_rights = vmread(VMCS_GUEST_TR_ACCESS_RIGHTS);
        d->guest_intability_info = vmread(VMCS_GUEST_INTERRUPTIBILITY_STATE);
        d->guest_smbase = vmread(VMCS_GUEST_SMBASE);
        d->guest_activity_state = vmread(VMCS_GUEST_ACTIVITY_STATE);
        d->guest_ia32_sysenter_cs = vmread(VMCS_GUEST_IA32_SYSENTER_CS);
        if (gi.pin_based_1_allowed & PIN_ACTIVATE_VMX_PREEMPTION_TIMER) {
                d->preemption_timer_value
                        = vmread(VMCS_PREEMPTION_TIMER_VALUE);
        }

        /* 32-bit host state fields */
        d->host_ia32_sysenter_cs = vmread(VMCS_HOST_IA32_SYSENTER_CS);

        /* natural-width control fields */
        d->cr0_guest_host_mask = vmread(VMCS_CR0_GUEST_HOST_MASK);
        d->cr4_guest_host_mask = vmread(VMCS_CR4_GUEST_HOST_MASK);
        d->cr0_read_shadow = vmread(VMCS_CR0_READ_SHADOW);
        d->cr4_read_shadow = vmread(VMCS_CR4_READ_SHADOW);
        d->cr3_target_val_0 = vmread(VMCS_CR3_TARGET_VAL_0);
        d->cr3_target_val_1 = vmread(VMCS_CR3_TARGET_VAL_1);
        d->cr3_target_val_2 = vmread(VMCS_CR3_TARGET_VAL_2);
        d->cr3_target_val_3 = vmread(VMCS_CR3_TARGET_VAL_3);

        /* natural-width read-only data fields */
        d->exit_qual = vmread(VMCS_EXIT_QUAL);
        d->io_rcx = vmread(VMCS_IO_RCX);
        d->io_rsi = vmread(VMCS_IO_RSI);
        d->io_rdi = vmread(VMCS_IO_RDI);
        d->io_rip = vmread(VMCS_IO_RIP);
        d->guest_lin_addr = vmread(VMCS_GUEST_LIN_ADDR);

        /* natural-width guest state fields */
        d->guest_cr0 = vmread(VMCS_GUEST_CR0);
        d->guest_cr3 = vmread(VMCS_GUEST_CR3);
        d->guest_cr4 = vmread(VMCS_GUEST_CR4);
        d->guest_es_base = vmread(VMCS_GUEST_ES_BASE);
        d->guest_cs_base = vmread(VMCS_GUEST_CS_BASE);
        d->guest_ss_base = vmread(VMCS_GUEST_SS_BASE);
        d->guest_ds_base = vmread(VMCS_GUEST_DS_BASE);
        d->guest_fs_base = vmread(VMCS_GUEST_FS_BASE);
        d->guest_gs_base = vmread(VMCS_GUEST_GS_BASE);
        d->guest_ldtr_base = vmread(VMCS_GUEST_LDTR_BASE);
        d->guest_tr_base = vmread(VMCS_GUEST_TR_BASE);
        d->guest_gdtr_base = vmread(VMCS_GUEST_GDTR_BASE);
        d->guest_idtr_base = vmread(VMCS_GUEST_IDTR_BASE);
        d->guest_dr7 = vmread(VMCS_GUEST_DR7);
        d->guest_rsp = vmread(VMCS_GUEST_RSP);
        d->guest_rip = vmread(VMCS_GUEST_RIP);
        d->guest_rflags = vmread(VMCS_GUEST_RFLAGS);
        d->guest_pend_dbg_exc = vmread(VMCS_GUEST_PEND_DBG_EXC);
        d->guest_ia32_sysenter_esp = vmread(VMCS_GUEST_IA32_SYSENTER_ESP);
        d->guest_ia32_sysenter_eip = vmread(VMCS_GUEST_IA32_SYSENTER_EIP);

        /* natural-width host state fields */
        d->host_cr0 = vmread(VMCS_HOST_CR0);
        d->host_cr3 = vmread(VMCS_HOST_CR3);
        d->host_cr4 = vmread(VMCS_HOST_CR4);
        d->host_fs_base = vmread(VMCS_HOST_FS_BASE);
        d->host_gs_base = vmread(VMCS_HOST_GS_BASE);
        d->host_tr_base = vmread(VMCS_HOST_TR_BASE);
        d->host_gdtr_base = vmread(VMCS_HOST_GDTR_BASE);
        d->host_idtr_base = vmread(VMCS_HOST_IDTR_BASE);
        d->host_ia32_sysenter_esp = vmread(VMCS_HOST_IA32_SYSENTER_ESP);
        d->host_ia32_sysenter_eip = vmread(VMCS_HOST_IA32_SYSENTER_EIP);
        d->host_rsp = vmread(VMCS_HOST_RSP);
        d->host_rip = vmread(VMCS_HOST_RIP);

        d->vmx_rev = gi.vmx_rev;
        d->vmx_mem_type = gi.vmx_mem_type;
        d->pin_based_0_allowed = gi.pin_based_0_allowed;
        d->pin_based_1_allowed = gi.pin_based_1_allowed;
        d->proc_based_0_allowed = gi.proc_based_0_allowed;
        d->proc_based_1_allowed = gi.proc_based_1_allowed;
        d->proc_based2_0_allowed = gi.proc_based2_0_allowed;
        d->proc_based2_1_allowed = gi.proc_based2_1_allowed;
        d->vmexit_0_allowed = gi.vmexit_0_allowed;
        d->vmexit_1_allowed = gi.vmexit_1_allowed;
        d->vmentry_0_allowed = gi.vmentry_0_allowed;
        d->vmentry_1_allowed = gi.vmentry_1_allowed;
        d->cr0_fixed_0 = gi.cr0_fixed_0;
        d->cr0_fixed_1 = gi.cr0_fixed_1;
        d->cr4_fixed_0 = gi.cr4_fixed_0;
        d->cr4_fixed_1 = gi.cr4_fixed_1;
        d->preemption_timer_scale = rdmsr(MSR_VMX_MISC) & 0x1f;
        d->ept_vpid_cap = gi.ept_vpid_cap;
}

void
vmx_dbg_get_current_vmcs(vmxdbg_vmcs_t *p, int cpu)
{
        wrap_memset(p, 0, sizeof *p);
        do_fetch_state(p, cpu);
}

void
dev_ioctl_get_vmcs(vm_t *vm, vmxdbg_vmcs_t *d)
{
        wrap_memset(d, 0xde, sizeof *d);

        /* use current VM if active, otherwise go for anyone */
        int cpu = vm->cpu;
        if (cpu == -1)
                cpu = dbg_get_cpu_with_vmcs();

        if (cpu != -1)
                run_on_cpu(cpu, do_fetch_state, d);

}

#if VMCS_DEBUG
int
dbg_vmcs_init(vm_t *vm)
{
        vmx_dbg_t *vmxd = x_malloc(sizeof *vmxd);
        if (!vmxd)
                return 1;

        wrap_memset(vmxd, 0, sizeof *vmxd);

        vm->vmx_dbg = vmxd;
        vmxd->magic = 0x1234567887654321;

        return 0;
}

void
dbg_vmcs_destroy(vm_t *vm)
{
        x_free(vm->vmx_dbg);
}

/* this code sends extra SMIs; used to discover BIOS bugs */
static inline void
dbg_send_smi(cpu_state_t *cs)
{
        if (!DEBUG_SEND_SMI)
                return;

        static unsigned long cnt;
        if ((cnt++ % 4096) != 0)
                return;

        while (wrap_apic_read(cs->lapic_base, APIC_ICR_LOW) & BIT(12))
                ;
        wrap_apic_write(cs->lapic_base, APIC_ICR_HI, 0);
        wrap_apic_write(cs->lapic_base, APIC_ICR_LOW,
                        (2 << 8) | BIT(14) | (0 << 18));
        while (wrap_apic_read(cs->lapic_base, APIC_ICR_LOW) & BIT(12))
                ;
}

static inline void
dbg_state_current(vm_t *vm, vmxdbg_state_t *d)
{
        d->time_stamp = readtsc();
        d->cr8 = get_cr8();

        vmx_dbg_get_current_vmcs(&d->vmcs_dbg, vm->cpu);
}

void
dbg_vmcs_entry(vm_t *vm)
{
        vmx_dbg_t *vmxd = vm->vmx_dbg;
        dbg_state_current(vm, &vmxd->entry_state);

        /* Sync current VMCS to memory */
        u64 curr_vmcs = vmptrst();
        vmclear(curr_vmcs);
        vmptrld(curr_vmcs);
        vm->launched = false;

        dbg_send_smi(vm->cs);
}

void
dbg_vmcs_exit(vm_t *vm)
{
        vmx_dbg_t *vmxd = vm->vmx_dbg;
        dbg_state_current(vm, &vmxd->exit_state);
        dbg_send_smi(vm->cs);
}

void
dbg_vmcs_nmi(vm_t *vm)
{
        if (!vm)
                return;
        vmx_dbg_t *vmxd = vm->vmx_dbg;
        dbg_state_current(vm, &vmxd->nmi_state);
        dbg_send_smi(vm->cs);
}
#endif // VMCS_DEBUG

void
dbg_print_banner(void)
{
        const char *debug_flags =
#if FIREWIRE_DEBUG
                " FIREWIRE_DEBUG"
#endif
#if DEBUG_SEND_SMI
                " DEBUG_SEND_SMI"
#endif
#if VMCS_DEBUG
                " VMCS_DEBUG"
#endif
#if ENABLE_STATIC_HOST_STATE_ASSERT
                " STATIC_HOST_STATE_ASSERT"
#endif
#if VMX_DEBUG_STACK_GUARD
                " VMX_DEBUG_STACK_GUARD"
#endif
#if DEBUG_ALLOCATION_CONTEXT
                " DEBUG_ALLOCATION_CONTEXT"
#endif
#if DEBUG_PAGE_LEAKS
                " DEBUG_PAGE_LEAKS"
#endif
                "";

        if (debug_flags[0] != 0) {
                printm("WARNING: this is a DEBUG version of the VMXMON"
                       " driver.\n");
                printm("Enabled DEBUG flags:%s.\n", debug_flags);
        }
}
