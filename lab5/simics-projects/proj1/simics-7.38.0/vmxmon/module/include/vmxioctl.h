/*
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

#ifndef VMXIOCTL_H
#define VMXIOCTL_H

#include "ktypes.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct {
        u64             cmd;                    /* request */
        u64             p1;                     /* input parameters */
        u64             p2;
        u64             p3;
        u64             p4;
        u64             ret;                    /* return value */
} vmxmon_req_t;

typedef struct vmxdbg_vmcs {
        /* 16-bit control fields */
        u16     vpid;
        u16     posted_interrupt_notification_vector;
        u16     eptp_index;

        /* 16-bit guest state fields */
        u16     guest_es_sel;
        u16     guest_cs_sel;
        u16     guest_ss_sel;
        u16     guest_ds_sel;
        u16     guest_fs_sel;
        u16     guest_gs_sel;
        u16     guest_ldtr_sel;
        u16     guest_tr_sel;
        u16     guest_interrupt_status;
        u16     pml_index;

        /* 16-bit host state fields */
        u16     host_es_sel;
        u16     host_cs_sel;
        u16     host_ss_sel;
        u16     host_ds_sel;
        u16     host_fs_sel;
        u16     host_gs_sel;
        u16     host_tr_sel;

        /* 64-bit control fields */
        u64     io_bitmap_a;
        u64     io_bitmap_b;
        u64     msr_bitmap_addr;
        u64     vmexit_msr_store_addr;
        u64     vmexit_msr_load_addr;
        u64     vmentry_msr_load_addr;
        u64     executive_vmcs_ptr;
        u64     pml_addr;
        u64     tsc_offset;
        u64     virt_apic_page_addr;
        u64     apic_access_addr;
        u64     posted_interrupt_descriptor_addr;
        u64     vmfunc_ctls;
        u64     ept_ptr;
        u64     eoi_exit_bitmap_0;
        u64     eoi_exit_bitmap_1;
        u64     eoi_exit_bitmap_2;
        u64     eoi_exit_bitmap_3;
        u64     eptp_list_addr;
        u64     vmread_bitmap_addr;
        u64     vmwrite_bitmap_addr;
        u64     virtual_exception_information;
        u64     xss_exiting_bitmap;
        u64     encls_exiting_bitmap;
        u64     subpage_permission_ptr;
        u64     tsc_multiplier;

        /* 64-bit read-only data fields */
        u64     guest_phys_addr;

        /* 64-bit guest state fields */
        u64     link_ptr;
        u64     guest_ia32_dbgctl;
        u64     guest_ia32_pat;
        u64     guest_ia32_efer;
        u64     guest_ia32_perf_global_ctl;
        u64     guest_pdpte0;
        u64     guest_pdpte1;
        u64     guest_pdpte2;
        u64     guest_pdpte3;
        u64     guest_ia32_bndcfgs;
        u64     guest_ia32_rtit_ctl;

        /* 64-bit host state fields */
        u64     host_ia32_pat;
        u64     host_ia32_efer;
        u64     host_ia32_perf_global_ctrl;

        /* 32-bit control fields */
        u32     pin_based_vm_exec_ctrls;
        u32     proc_based_vm_exec_ctrls;
        u32     exc_bitmap;
        u32     pfault_ecode_mask;
        u32     pfault_ecode_match;
        u32     cr3_target_cnt;
        u32     vmexit_ctrls;
        u32     vmexit_msr_store_cnt;
        u32     vmexit_msr_load_cnt;
        u32     vmentry_ctrls;
        u32     vmentry_msr_load_cnt;
        u32     vmentry_int_info_field;
        u32     vmentry_exc_ecode;
        u32     vmentry_inst_len;
        u32     tpr_threshold;
        u32     proc_based2_vm_exec_ctrls;
        u32     ple_gap;
        u32     ple_window;

        /* 32-bit read-only data fields */
        u32     vm_instr_err;
        u32     exit_reason;
        u32     vmexit_int_info;
        u32     vmexit_int_ecode;
        u32     idt_vectoring_info_field;
        u32     idt_vectoring_ecode;
        u32     vmexit_inst_len;
        u32     vmx_inst_info;

        /* 32-bit guest state fields */
        u32     guest_es_limit;
        u32     guest_cs_limit;
        u32     guest_ss_limit;
        u32     guest_ds_limit;
        u32     guest_fs_limit;
        u32     guest_gs_limit;
        u32     guest_ldtr_limit;
        u32     guest_tr_limit;
        u32     guest_gdtr_limit;
        u32     guest_idtr_limit;
        u32     guest_es_access_rights;
        u32     guest_cs_access_rights;
        u32     guest_ss_access_rights;
        u32     guest_ds_access_rights;
        u32     guest_fs_access_rights;
        u32     guest_gs_access_rights;
        u32     guest_ldtr_access_rights;
        u32     guest_tr_access_rights;
        u32     guest_intability_info;
        u32     guest_activity_state;
        u32     guest_smbase;
        u32     guest_ia32_sysenter_cs;
        u32     preemption_timer_value;

        /* 32-bit host state fields */
        u32     host_ia32_sysenter_cs;

        /* natural-width control fields */
        u64     cr0_guest_host_mask;
        u64     cr4_guest_host_mask;
        u64     cr0_read_shadow;
        u64     cr4_read_shadow;
        u64     cr3_target_val_0;
        u64     cr3_target_val_1;
        u64     cr3_target_val_2;
        u64     cr3_target_val_3;

        /* natural-width read-only data fields */
        u64     exit_qual;
        u64     io_rcx;
        u64     io_rsi;
        u64     io_rdi;
        u64     io_rip;
        u64     guest_lin_addr;

        /* natural-width guest state fields */
        u64     guest_cr0;
        u64     guest_cr3;
        u64     guest_cr4;
        u64     guest_es_base;
        u64     guest_cs_base;
        u64     guest_ss_base;
        u64     guest_ds_base;
        u64     guest_fs_base;
        u64     guest_gs_base;
        u64     guest_ldtr_base;
        u64     guest_tr_base;
        u64     guest_gdtr_base;
        u64     guest_idtr_base;
        u64     guest_dr7;
        u64     guest_rsp;
        u64     guest_rip;
        u64     guest_rflags;
        u64     guest_pend_dbg_exc;
        u64     guest_ia32_sysenter_esp;
        u64     guest_ia32_sysenter_eip;

        /* natural-width host state fields */
        u64     host_cr0;
        u64     host_cr3;
        u64     host_cr4;
        u64     host_fs_base;
        u64     host_gs_base;
        u64     host_tr_base;
        u64     host_gdtr_base;
        u64     host_idtr_base;
        u64     host_ia32_sysenter_esp;
        u64     host_ia32_sysenter_eip;
        u64     host_rsp;
        u64     host_rip;

        /* VMX implementation info */
        u32     vmx_rev;
        u32     vmx_mem_type;
        u32     pin_based_0_allowed;
        u32     pin_based_1_allowed;
        u32     proc_based_0_allowed;
        u32     proc_based_1_allowed;
        u32     proc_based2_0_allowed;
        u32     proc_based2_1_allowed;
        u32     vmexit_0_allowed;
        u32     vmexit_1_allowed;
        u32     vmentry_0_allowed;
        u32     vmentry_1_allowed;
        u32     cr0_fixed_0;
        u32     cr0_fixed_1;
        u32     cr4_fixed_0;
        u32     cr4_fixed_1;
        u32     preemption_timer_scale;
        u64     ept_vpid_cap;
} vmxdbg_vmcs_t;

typedef struct {
        u64             ip_entry;          /* ip on VMX entry */
        u64             ip_exit;           /* ip on VMX exit */

        u64             step_entry;        /* step on entry */
        u32             elapsed;           /* steps executed */
        u32             exit_reason;       /* VMCS_EXIT_REASON */

        u64             debug;
} vmx_trace_entry_t;

enum {
        Num_Vmx_Trace_Entries = (0x1000/sizeof(vmx_trace_entry_t) - 1)
};
typedef struct {
        u32             n;                 /* W: n valid traces */
        u32             pad;               /* reserved */
        vmx_trace_entry_t entries[Num_Vmx_Trace_Entries];
} vmx_trace_t;
typedef struct {
        u64             virt;
        u64             phys;
        u64             host;
        u64             reserved;
} vmx_tlb_entry_t;

#if defined __linux__
#define VMXMON_REQUEST                  _IOWR('X', 1, vmxmon_req_t)
#define VMXMON_GET_VMCS                 _IOWR('X', 2, vmxdbg_vmcs_t)
#define VMXMON_GET_TRACE                _IOWR('X', 3, vmx_trace_t)
#define VMXMON_SIGNAL                   _IO('X', 5)
#define VMXMON_ENTER                    _IO('X', 6)
#define VMXMON_GET_REGS                 _IOW('X', 7, u64)
#else
#define VMXMON_IOCTL(n, p) \
        CTL_CODE(FILE_DEVICE_UNKNOWN, (n), METHOD_NEITHER, (p))
#define VMXMON_REQUEST      VMXMON_IOCTL(1, FILE_READ_DATA | FILE_WRITE_DATA)
#define VMXMON_GET_VMCS     VMXMON_IOCTL(2, FILE_READ_DATA | FILE_WRITE_DATA)
#define VMXMON_GET_TRACE    VMXMON_IOCTL(3, FILE_READ_DATA | FILE_WRITE_DATA)
#define VMXMON_SIGNAL       VMXMON_IOCTL(5, 0)
#define VMXMON_ENTER        VMXMON_IOCTL(6, FILE_READ_DATA | FILE_WRITE_DATA)
#define VMXMON_GET_REGS     VMXMON_IOCTL(7, FILE_READ_DATA | FILE_WRITE_DATA)
#endif

#if defined(__cplusplus)
}
#endif

#endif   /* VMXIOCTL_H */
