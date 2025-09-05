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

#ifndef VMX_H
#define VMX_H

/************************************************************************/
/*	MSR handling							*/
/************************************************************************/

#define BIT(n)                          (1ULL << (n))
#define FIELD(val, e, s)                (((val) >> (s)) & \
                                           ((1ULL << ((e) - (s) + 1)) - 1))
#define BITMASK(e, s)                   (((1ULL << ((e) - (s) + 1)) - 1) << (s))

/* VMX MSRs numbers */
#define MSR_VMX_BASIC                   0x480
#define   VMX_BASIC_REV_ID(x)             ((x) & 0xffff)
#define   VMX_BASIC_VMCS_SIZE(x)          ((unsigned)((x) >> 32) & 0x1fff)
#define   VMX_BASIC_MEM_TYPE(x)           (((x) >> 50) & 0xf)
#define     VMX_MEM_TYPE_UC               0             /* strong uncachable */
#define     VMX_MEM_TYPE_WB               6             /* write back */
#define   VMX_BASIC_INS_OUTS_REPORTING    BIT(54)
#define   VMX_BASIC_HAS_TRUE_CTLS         BIT(55)
#define MSR_VMX_PINBASED_CTLS           0x481
#define MSR_VMX_PROCBASED_CTLS          0x482
#define MSR_VMX_EXIT_CTLS               0x483
#define MSR_VMX_ENTRY_CTLS              0x484
#define MSR_VMX_MISC                    0x485
#define   VMX_MISC_MAX_NUM_CR3(x)         ((unsigned)((x) >> 16) & 0xff)
#define   VMX_MISC_MAX_NUM_MSRS(x)        ((unsigned)((x) >> 25) & 0x7)
#define MSR_VMX_CR0_FIXED0              0x486
#define MSR_VMX_CR0_FIXED1              0x487
#define MSR_VMX_CR4_FIXED0              0x488
#define MSR_VMX_CR4_FIXED1              0x489
#define MSR_VMX_VMCS_ENUM               0x48a

/* the following MSRs are not defined on early VMX revision */
#define MSR_VMX_PROCBASED_CTLS2         0x48b
#define MSR_VMX_EPT_VPID_CAP            0x48c

#define MSR_VMX_TRUE_PINBASED_CTLS      0x48d
#define MSR_VMX_TRUE_PROCBASED_CTLS     0x48e
#define MSR_VMX_TRUE_EXIT_CTLS          0x48f
#define MSR_VMX_TRUE_ENTRY_CTLS         0x490

#define MSR_VMX_VMFUNC                  0x491
#define   VMFUNC_EPTP_SWITCHING           BIT(0)

/************************************************************************/
/*	VMCS register encodings						*/
/************************************************************************/

/* 16-bit control fields */
#define VMCS_VPID                       0x000
#define VMCS_POSTED_INTERRUPT_NOTIFICATION_VECTOR 0x002
#define VMCS_EPTP_INDEX                 0x004

/* 16-bit guest state fields */
#define VMCS_GUEST_ES_SEL               0x800
#define VMCS_GUEST_CS_SEL               0x802
#define VMCS_GUEST_SS_SEL               0x804
#define VMCS_GUEST_DS_SEL               0x806
#define VMCS_GUEST_FS_SEL               0x808
#define VMCS_GUEST_GS_SEL               0x80a
#define VMCS_GUEST_LDTR_SEL             0x80c
#define VMCS_GUEST_TR_SEL               0x80e
#define VMCS_GUEST_INTERRUPT_STATUS     0x810
#define VMCS_PML_INDEX                  0x812

/* 16-bit host state fields */
#define VMCS_HOST_ES_SEL                0xc00
#define VMCS_HOST_CS_SEL                0xc02
#define VMCS_HOST_SS_SEL                0xc04
#define VMCS_HOST_DS_SEL                0xc06
#define VMCS_HOST_FS_SEL                0xc08
#define VMCS_HOST_GS_SEL                0xc0a
#define VMCS_HOST_TR_SEL                0xc0c

/* 64-bit control fields */
#define VMCS_IO_BITMAP_A                0x2000
#define VMCS_IO_BITMAP_B                0x2002
#define VMCS_MSR_BITMAP_ADDR            0x2004
#define VMCS_VMEXIT_MSR_STORE_ADDR      0x2006
#define VMCS_VMEXIT_MSR_LOAD_ADDR       0x2008
#define VMCS_VMENTRY_MSR_LOAD_ADDR      0x200a
#define VMCS_EXECUTIVE_VMCS_PTR         0x200c
#define VMCS_PML_ADDR                   0x200e
#define VMCS_TSC_OFFSET                 0x2010
#define VMCS_VIRT_APIC_PAGE_ADDR        0x2012
#define VMCS_APIC_ACCESS_ADDR           0x2014
#define VMCS_POSTED_INTERRUPT_DESC_ADDR 0x2016
#define VMCS_VMFUNC_CTLS                0x2018
#define VMCS_EPT_PTR                    0x201a
#define VMCS_EOI_EXIT_BITMAP_0          0x201c
#define VMCS_EOI_EXIT_BITMAP_1          0x201e
#define VMCS_EOI_EXIT_BITMAP_2          0x2020
#define VMCS_EOI_EXIT_BITMAP_3          0x2022
#define VMCS_EPTP_LIST_ADDR             0x2024
#define VMCS_VMREAD_BITMAP_ADDR         0x2026
#define VMCS_VMWRITE_BITMAP_ADDR        0x2028
#define VMCS_VIRTUAL_EXC_INFO           0x202a
#define VMCS_XSS_EXITING_BITMAP         0x202c
#define VMCS_ENCLS_EXITING_BITMAP       0x202e
#define VMCS_SUBPAGE_PERMISSION_PTR     0x2030
#define VMCS_TSC_MULTIPLIER             0x2032

/* 64-bit read-only data fields */
#define VMCS_GUEST_PHYS_ADDR            0x2400

/* 64-bit guest state fields */
#define VMCS_VMCS_LINK_PTR              0x2800
#define VMCS_GUEST_IA32_DBGCTL          0x2802
#define VMCS_GUEST_IA32_PAT             0x2804
#define VMCS_GUEST_IA32_EFER            0x2806
#define VMCS_GUEST_IA32_PERF_GLOBAL_CTRL 0x2808
#define VMCS_GUEST_PDPTE0               0x280a
#define VMCS_GUEST_PDPTE1               0x280c
#define VMCS_GUEST_PDPTE2               0x280e
#define VMCS_GUEST_PDPTE3               0x2810
#define VMCS_GUEST_IA32_BNDCFGS         0x2812
#define VMCS_GUEST_IA32_RTIT_CTL        0x2814

/* 64-bit host state fields */
#define VMCS_HOST_IA32_PAT              0x2c00
#define VMCS_HOST_IA32_EFER             0x2c02
#define VMCS_HOST_IA32_PERF_GLOBAL_CTRL 0x2c04

/* 32-bit control fields */
#define VMCS_PIN_BASED_VM_EXEC_CTRLS    0x4000
#define   PIN_EXT_INT_EXITING               BIT(0)
#define   PIN_NMI_EXITING                   BIT(3)
#define   PIN_VIRTUAL_NMIS                  BIT(5)
#define   PIN_ACTIVATE_VMX_PREEMPTION_TIMER BIT(6)
#define   PIN_PROCESS_POSTED_INTERRUPTS     BIT(7)                    
#define   PIN_RES_1                         (BIT(1) | BIT(2) | BIT(4))
#define VMCS_PROC_BASED_VM_EXEC_CTRLS   0x4002
#define   PROC_INT_WINDOW_EXITING           BIT(2)
#define   PROC_USE_TSC_OFFSETTING           BIT(3)
#define   PROC_HLT_EXITING                  BIT(7)
#define   PROC_INVLPG_EXITING               BIT(9)
#define   PROC_MWAIT_EXITING                BIT(10)
#define   PROC_RDPMC_EXITING                BIT(11)
#define   PROC_RDTSC_EXITING                BIT(12)
#define   PROC_CR3_LOAD_EXITING             BIT(15)
#define   PROC_CR3_STORE_EXITING            BIT(16)
#define   PROC_CR8_LOAD_EXITING             BIT(19)
#define   PROC_CR8_STORE_EXITING            BIT(20)
#define   PROC_USE_TPR_SHADOW               BIT(21)
#define   PROC_NMI_WINDOW_EXITING           BIT(22)
#define   PROC_MOV_DR_EXITING               BIT(23)
#define   PROC_UNCOND_IO_EXITING            BIT(24)
#define   PROC_USE_IO_BITMAPS               BIT(25)
#define   PROC_MONITOR_TRAP_FLAG            BIT(27)
#define   PROC_USE_MSR_BITMAPS              BIT(28)
#define   PROC_MONITOR_EXITING              BIT(29)
#define   PROC_PAUSE_EXITING                BIT(30)
#define   PROC_ACTIVATE_SEC_CTRLS           BIT(31)
#define   PROC_RES_1                        0x04006172
#define VMCS_EXC_BITMAP                 0x4004
#define VMCS_PFAULT_ECODE_MASK          0x4006
#define VMCS_PFAULT_ECODE_MATCH         0x4008
#define VMCS_CR3_TARGET_CNT             0x400a
#define VMCS_VMEXIT_CTRLS               0x400c
#define   VMEXIT_SAVE_DEBUG_CTRLS           BIT(2)
#define   VMEXIT_HOST_ASPACE_SIZE           BIT(9)
#define   VMEXIT_LOAD_IA32_PERF_GLOBAL_CTRL BIT(12)
#define   VMEXIT_ACK_INT_ON_EXIT            BIT(15)
#define   VMEXIT_SAVE_IA32_PAT              BIT(18)
#define   VMEXIT_LOAD_IA32_PAT              BIT(19)
#define   VMEXIT_SAVE_IA32_EFER             BIT(20)
#define   VMEXIT_LOAD_IA32_EFER             BIT(21)
#define   VMEXIT_SAVE_VMX_PREEMPTION_TIMER  BIT(22)
#define   VMEXIT_CLEAR_IA32_BNDCFGS         BIT(23)
#define   VMEXIT_CONCEAL_VMX_FROM_PT        BIT(24)
#define   VMEXIT_CLEAR_IA32_RTIT_CTL        BIT(25)
#define   VMEXIT_RES_1                      0x00036dfb
#define VMCS_VMEXIT_MSR_STORE_CNT       0x400e
#define VMCS_VMEXIT_MSR_LOAD_CNT        0x4010
#define VMCS_VMENTRY_CTRLS              0x4012
#define   VMENTRY_LOAD_DEBUG_CTRLS          BIT(2)
#define   VMENTRY_IA32E_MODE_GUEST          BIT(9)
#define   VMENTRY_ENTRY_TO_SMM              BIT(10)
#define   VMENTRY_DEFAULT_DUAL_MON          BIT(11)
#define   VMENTRY_LOAD_IA32_PERF_GLOBAL_CTRL BIT(13)
#define   VMENTRY_LOAD_IA32_PAT             BIT(14)
#define   VMENTRY_LOAD_IA32_EFER            BIT(15)
#define   VMENTRY_LOAD_IA32_BNDCFGS         BIT(16)
#define   VMENTRY_CONCEAL_VMX_FROM_PT       BIT(17)
#define   VMENTRY_LOAD_IA32_RTIT_CTL        BIT(18)
#define   VMENTRY_RES_1                     0x000011fb
#define VMCS_VMENTRY_MSR_LOAD_CNT       0x4014
#define VMCS_VMENTRY_INT_INFO_FIELD     0x4016
#define VMCS_VMENTRY_EXC_ECODE          0x4018
#define VMCS_VMENTRY_INST_LEN           0x401a
#define VMCS_TPR_THRESHOLD              0x401c
#define VMCS_SEC_PROC_BASED_CTLS        0x401e
#define   PROC2_VIRTUALIZE_APIC             BIT(0)
#define   PROC2_ENABLE_EPT                  BIT(1)
#define   PROC2_DESC_TABLE_EXITING          BIT(2)
#define   PROC2_ENABLE_RDTSCP               BIT(3)
#define   PROC2_VIRTUALIZE_X2APIC_MODE      BIT(4)
#define   PROC2_ENABLE_VPID                 BIT(5)
#define   PROC2_WBINVD_EXITING              BIT(6)
#define   PROC2_UNRESTRICTED_GUEST          BIT(7)
#define   PROC2_APIC_REG_VIRTUALIZATION     BIT(8)
#define   PROC2_VIRT_INT_DELIVERY           BIT(9)
#define   PROC2_PAUSE_LOOP_EXITING          BIT(10)
#define   PROC2_RDRAND_EXITING              BIT(11)
#define   PROC2_ENABLE_INVPCID              BIT(12)
#define   PROC2_ENABLE_VMFUNC               BIT(13)
#define   PROC2_ENABLE_SVMCS                BIT(14)
#define   PROC2_ENCLS_EXITING               BIT(15)
#define   PROC2_RDSEED_EXITING              BIT(16)
#define   PROC2_ENABLE_PML                  BIT(17)
#define   PROC2_EPT_VIOLATION_VE            BIT(18)
#define   PROC2_CONCEAL_VMX_FROM_PT         BIT(19)
#define   PROC2_ENABLE_XSAVES               BIT(20)
#define   PROC2_MODE_BASED_EXEC_EPT         BIT(22)
#define   PROC2_SPP_WRITE_FOR_EPT           BIT(23)
#define   PROC2_PT_USES_GUEST_PA            BIT(24)
#define   PROC2_USE_TSC_SCALING             BIT(25)
#define   PROC2_ENABLE_UWAIT                BIT(26)
#define   PROC2_ENCLV_EXITING               BIT(28)
#define VMCS_PLE_GAP                    0x4020
#define VMCS_PLE_WINDOW                 0x4022

/* used for VMENTRY_INT_INFO, VMEXIT_INT_INFO, and IDT_VECTORING_INT_INFO */
#define INT_INFO_VALID                       BIT(31)
#define INT_INFO_NMI_UNBLOCKING_DUE_TO_IRET  BIT(12)    /* exit */
#define INT_INFO_ECODE_VALID                 BIT(11)    /* exit, vectoring */
#define INT_INFO_DELIVER_ECODE               BIT(11)    /* entry */
#define INT_INFO_TYPE_MASK                   0x700
#define   INT_INFO_TYPE_EXTERNAL_INT           0x000
#define   INT_INFO_TYPE_RESERVED               0x100
#define   INT_INFO_TYPE_NMI                    0x200
#define   INT_INFO_TYPE_HW_EXC                 0x300
#define   INT_INFO_TYPE_SOFTWARE_INT           0x400    /* entry, vectoring */
#define   INT_INFO_TYPE_PRIV_SOFTWARE_INT      0x500    /* entry, vectoring */
#define   INT_INFO_TYPE_SOFTWARE_EXC           0x600
#define   INT_INFO_TYPE_OTHER                  0x700    /* entry */
#define INT_INFO_VECTOR_MASK                 0xff

/* 32-bit read-only data fields */
#define VMCS_VM_INSTR_ERR               0x4400
#define VMCS_EXIT_REASON                0x4402
#define VMCS_VMEXIT_INT_INFO            0x4404
#define VMCS_VMEXIT_INT_ECODE           0x4406
#define VMCS_IDT_VECTORING_INFO_FIELD   0x4408
#define VMCS_IDT_VECTORING_ECODE        0x440a
#define VMCS_VMEXIT_INST_LEN            0x440c
#define VMCS_VMX_INST_INFO              0x440e

/* 32-bit guest state fields */
#define VMCS_GUEST_ES_LIMIT             0x4800
#define VMCS_GUEST_CS_LIMIT             0x4802
#define VMCS_GUEST_SS_LIMIT             0x4804
#define VMCS_GUEST_DS_LIMIT             0x4806
#define VMCS_GUEST_FS_LIMIT             0x4808
#define VMCS_GUEST_GS_LIMIT             0x480a
#define VMCS_GUEST_LDTR_LIMIT           0x480c
#define VMCS_GUEST_TR_LIMIT             0x480e
#define VMCS_GUEST_GDTR_LIMIT           0x4810
#define VMCS_GUEST_IDTR_LIMIT           0x4812
#define VMCS_GUEST_ES_ACCESS_RIGHTS     0x4814
#define VMCS_GUEST_CS_ACCESS_RIGHTS     0x4816
#define VMCS_GUEST_SS_ACCESS_RIGHTS     0x4818
#define VMCS_GUEST_DS_ACCESS_RIGHTS     0x481a
#define VMCS_GUEST_FS_ACCESS_RIGHTS     0x481c
#define VMCS_GUEST_GS_ACCESS_RIGHTS     0x481e
#define VMCS_GUEST_LDTR_ACCESS_RIGHTS   0x4820
#define VMCS_GUEST_TR_ACCESS_RIGHTS     0x4822
#define VMCS_GUEST_INTERRUPTIBILITY_STATE 0x4824
#define VMCS_GUEST_ACTIVITY_STATE       0x4826
#define VMCS_GUEST_SMBASE               0x4828
#define VMCS_GUEST_IA32_SYSENTER_CS     0x482a
#define VMCS_PREEMPTION_TIMER_VALUE     0x482e

/* 32-bit host state fields */
#define VMCS_HOST_IA32_SYSENTER_CS      0x4c00

/* natural-width control fields */
#define VMCS_CR0_GUEST_HOST_MASK        0x6000
#define VMCS_CR4_GUEST_HOST_MASK        0x6002
#define VMCS_CR0_READ_SHADOW            0x6004
#define VMCS_CR4_READ_SHADOW            0x6006
#define VMCS_CR3_TARGET_VAL_0           0x6008
#define VMCS_CR3_TARGET_VAL_1           0x600a
#define VMCS_CR3_TARGET_VAL_2           0x600c
#define VMCS_CR3_TARGET_VAL_3           0x600e

/* natural-width read-only data fields */
#define VMCS_EXIT_QUAL                  0x6400
#define VMCS_IO_RCX                     0x6402
#define VMCS_IO_RSI                     0x6404
#define VMCS_IO_RDI                     0x6406
#define VMCS_IO_RIP                     0x6408
#define VMCS_GUEST_LIN_ADDR             0x640a

/* natural-width guest state fields */
#define VMCS_GUEST_CR0                  0x6800
#define VMCS_GUEST_CR3                  0x6802
#define VMCS_GUEST_CR4                  0x6804
#define VMCS_GUEST_ES_BASE              0x6806
#define VMCS_GUEST_CS_BASE              0x6808
#define VMCS_GUEST_SS_BASE              0x680a
#define VMCS_GUEST_DS_BASE              0x680c
#define VMCS_GUEST_FS_BASE              0x680e
#define VMCS_GUEST_GS_BASE              0x6810
#define VMCS_GUEST_LDTR_BASE            0x6812
#define VMCS_GUEST_TR_BASE              0x6814
#define VMCS_GUEST_GDTR_BASE            0x6816
#define VMCS_GUEST_IDTR_BASE            0x6818
#define VMCS_GUEST_DR7                  0x681a
#define VMCS_GUEST_RSP                  0x681c
#define VMCS_GUEST_RIP                  0x681e
#define VMCS_GUEST_RFLAGS               0x6820
#define VMCS_GUEST_PEND_DBG_EXC         0x6822
#define VMCS_GUEST_IA32_SYSENTER_ESP    0x6824
#define VMCS_GUEST_IA32_SYSENTER_EIP    0x6826

/* natural-width host state fields */
#define VMCS_HOST_CR0                   0x6c00
#define VMCS_HOST_CR3                   0x6c02
#define VMCS_HOST_CR4                   0x6c04
#define VMCS_HOST_FS_BASE               0x6c06
#define VMCS_HOST_GS_BASE               0x6c08
#define VMCS_HOST_TR_BASE               0x6c0a
#define VMCS_HOST_GDTR_BASE             0x6c0c
#define VMCS_HOST_IDTR_BASE             0x6c0e
#define VMCS_HOST_IA32_SYSENTER_ESP     0x6c10
#define VMCS_HOST_IA32_SYSENTER_EIP     0x6c12
#define VMCS_HOST_RSP                   0x6c14
#define VMCS_HOST_RIP                   0x6c16

#endif   /* VMX_H */
