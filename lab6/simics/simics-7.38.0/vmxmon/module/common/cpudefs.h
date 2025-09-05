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

#ifndef CPUDEFS_H
#define CPUDEFS_H

#include "vmx.h"

#define RFLAGS_AC               BIT(18)     /* alignment check/access control */
#define RFLAGS_RF               BIT(16)     /* resume flag */
#define RFLAGS_TF               BIT(8)      /* trace flag */

/* CPUID defs */
#define CPUID_1_ECX_VMX                 BIT(5)
#define CPUID_1_ECX_PERF_CAPABILITIES   BIT(15)
#define CPUID_1_ECX_XSAVE               BIT(26)
#define CPUID_1_ECX_OSXSAVE             BIT(27)
#define CPUID_1_ECX_AVX                 BIT(28)
#define CPUID_1_EDX_PAE                 BIT(6)
#define CPUID_7_EBX_FSGSBASE            BIT(0)
#define CPUID_7_EBX_BMI1                BIT(3)
#define CPUID_7_EBX_FPU_CSDS_DEPRECATION BIT(13)
#define CPUID_7_ECX_PKU                 BIT(3)
#define CPUID_7_ECX_OSPKE               BIT(4)
#define CPUID_80000001_ECX_LZCNT        BIT(5)
#define CPUID_80000001_EDX_NX           BIT(20)
#define CPUID_80000001_EDX_1GB_PAGES    BIT(26)
#define CPUID_80000001_EDX_RDTSCP       BIT(27)

#define MSR_IA32_TIME_STAMP_COUNTER  0x10

#define MSR_IA32_FEATURE_CTRL   0x3a
#define MSR_IA32_MISC_ENABLE    0x1a0
#define MSR_IA32_CR_PAT         0x277

#define MSR_IA32_SYSENTER_CS	0x174
#define MSR_IA32_SYSENTER_ESP	0x175
#define MSR_IA32_SYSENTER_EIP	0x176

#define MSR_EFER                0xc0000080
#define MSR_STAR                0xc0000081
#define MSR_LSTAR               0xc0000082
#define MSR_CSTAR               0xc0000083      /* amd64 only */
#define MSR_IA32_FMASK          0xc0000084
#define MSR_FS_BASE             0xc0000100
#define MSR_GS_BASE             0xc0000101
#define MSR_KERNEL_GS_BASE      0xc0000102
#define MSR_IA32_TSC_AUX        0xc0000103

/* performance counters */
#define MSR_CRU_ESCR0           0x3b8
#define MSR_CRU_ESCR1           0x3b9
#define MSR_CRU_ESCR2           0x3cc
#define MSR_CRU_ESCR3           0x3cd
#define MSR_FSB_ESCR0           0x3a2
#define MSR_FSB_ESCR1           0x3a3

#define MSR_BPU_CCCR0           0x360
#define MSR_BPU_CCCR1           0x361
#define MSR_BPU_CCCR2           0x362
#define MSR_BPU_CCCR3           0x363
#define MSR_IQ_CCCR0            0x36c
#define MSR_IQ_CCCR1            0x36d
#define MSR_IQ_CCCR2            0x36e
#define MSR_IQ_CCCR3            0x36f
#define MSR_IQ_CCCR4            0x370
#define MSR_IQ_CCCR5            0x371

#define MSR_BPU_COUNTER0        0x300
#define MSR_BPU_COUNTER1        0x301
#define MSR_BPU_COUNTER2        0x302
#define MSR_BPU_COUNTER3        0x303
#define MSR_IQ_COUNTER0         0x30c
#define MSR_IQ_COUNTER1         0x30d
#define MSR_IQ_COUNTER2         0x30e
#define MSR_IQ_COUNTER3         0x30f
#define MSR_IQ_COUNTER4         0x310
#define MSR_IQ_COUNTER5         0x311

/* Architectural Performance Monitoring MSRs */
#define MSR_IA32_PMC0		0x0c1
#define MSR_IA32_PMC1		0x0c2
#define MSR_IA32_PMC2		0x0c3
#define MSR_IA32_PMC3		0x0c4
#define MSR_IA32_PMC4		0x0c5
#define MSR_IA32_PMC5		0x0c6
#define MSR_IA32_PMC6		0x0c7
#define MSR_IA32_PMC7		0x0c8

#define MSR_IA32_PERFEVTSEL0	0x186
#define MSR_IA32_PERFEVTSEL1	0x187
#define MSR_IA32_PERFEVTSEL2	0x188
#define MSR_IA32_PERFEVTSEL3	0x189
#define MSR_IA32_PERFEVTSEL4	0x18a
#define MSR_IA32_PERFEVTSEL5	0x18b
#define MSR_IA32_PERFEVTSEL6	0x18c
#define MSR_IA32_PERFEVTSEL7	0x18d

/* Architectural Performance Monitoring MSRs v2 */
#define MSR_IA32_PERF_GLOBAL_STATUS     0x38e
#define MSR_IA32_PERF_GLOBAL_CTRL       0x38f
#define MSR_IA32_PERF_GLOBAL_OVF_CTRL   0x390
#define MSR_IA32_FIXED_CTR0             0x309
#define MSR_IA32_FIXED_CTR1             0x30a
#define MSR_IA32_FIXED_CTR2             0x30b

/* PEBS (Precise Event Based Sampling), NetBurst */
#define MSR_IA32_PEBS_ENABLE            0x3f1

/* Freeze while SMM is supported (bit 12) */
#define MSR_IA32_PERF_CAPABILITIES      0x345

#define MSR_IA32_FIXED_CTR_CTRL         0x38d

/* Intel® MPX control register */
#define MSR_IA32_BNDCFGS                0xd90
#define BNDCFG_EN                       1               /* enable bit */

/* APIC registers and fields */
#define	APIC_ID	                0x20
#define	APIC_ICR_LOW            0x300
#define	APIC_ICR_HI             0x310
#define	APIC_LVTPC              0x340
#define APIC_LVERR              0x370
#define	APIC_DM_NMI             0x400

#define PDE_P                   BIT(0)
#define PDE_W                   BIT(1)
#define PDE_U                   BIT(2)
#define PDE_PTW                 BIT(3)
#define PDE_PCD                 BIT(4)
#define PDE_A                   BIT(5)
#define PDE_D                   BIT(6)          /* large pages only */
#define PDE_PS                  BIT(7)
#define PDE_G                   BIT(8)          /* large pages only */
#define PDE_PAT                 BIT(12)

#define PTE_P                   BIT(0)
#define PTE_W                   BIT(1)
#define PTE_U                   BIT(2)
#define PTE_PWT                 BIT(3)
#define PTE_PCD                 BIT(4)
#define PTE_A                   BIT(5)
#define PTE_D                   BIT(6)
#define PTE_PAT                 BIT(7)
#define PTE_G                   BIT(8)          /* global */
#define PTE_NX                  BIT(63)

#define PTE_AVL_MASK_64         0x7ff0000000000e00ULL
#define PTE_AVL_MASK_CLASSIC    0xe00

#define EPT_R_ACCESS            BIT(0)
#define EPT_W_ACCESS            BIT(1)
#define EPT_XS_ACCESS           BIT(2)
#define EPT_XU_ACCESS           BIT(10)

#define ECODE_P                 BIT(0)
#define ECODE_W                 BIT(1)
#define ECODE_U                 BIT(2)
#define ECODE_RSV               BIT(3)
#define ECODE_I                 BIT(4)

/* CR4 bits */

#define CR4_LAM_SUP             BIT(28)         /* LAM for supervisor ptrs */
#define CR4_OSPKE               BIT(22)         /* Protection key enable bit */
#define CR4_SMAP                BIT(21)         /* SMAP enable bit */
#define CR4_SMEP                BIT(20)         /* SMEP enable bit */
#define CR4_OSXSAVE             BIT(18)         /* XSAVE enable bit */
#define CR4_PCIDE               BIT(17)         /* PCID enable bit */
#define CR4_FSGSBASE            BIT(16)         /* RDFSBASE etc. enable bit */
#define CR4_SMXE                BIT(14)         /* SMX enable bit */
#define CR4_VMXE                BIT(13)         /* VMX enable bit */
#define CR4_LA57                BIT(12)         /* LA57/PA52 */
#define CR4_OSXMMEXCPT          BIT(10)         /* OS unmasked exc support */
#define CR4_OSFXSR              BIT(9)          /* OS FXSAVE/FXRESTORE sup */
#define CR4_PCE                 BIT(8)          /* perf cntr enable */
#define CR4_PGE                 BIT(7)          /* page global enable */
#define CR4_MCE                 BIT(6)          /* machine check enable */
#define CR4_PAE                 BIT(5)          /* physical address extension */
#define CR4_PSE                 BIT(4)          /* page size extension */
#define CR4_DE                  BIT(3)          /* debugging extensions */
#define CR4_TSD                 BIT(2)          /* time stamp disable */
#define CR4_PVI                 BIT(1)          /* protected mode virtual int */
#define CR4_VME                 BIT(0)          /* V86 extension */


/* CR0 bits */
#define CR0_PE                  BIT(0)          /* protection enabled */
#define CR0_MP                  BIT(1)          /* monitor coprocessor */
#define CR0_EM                  BIT(2)          /* emulation */
#define CR0_TS                  BIT(3)          /* task switched */
#define CR0_ET                  BIT(4)          /* extension type (always 1) */
#define CR0_NE                  BIT(5)          /* numeric error */
#define CR0_WP                  BIT(16)         /* (supervisor) write protect */
#define CR0_AM                  BIT(18)         /* alignment mask */
#define CR0_PG                  BIT(31)         /* paging enable */
#define CR0_CD                  BIT(30)         /* cache disable */
#define CR0_NW                  BIT(29)         /* not write through */
#define CR0_RSVD                (BIT(6) | BIT(7) | BIT(8) | BIT(9) | \
                                BIT(10) | BIT(11) | BIT(12) | BIT(13) | \
                                BIT(14) | BIT(15) | BIT(17) | BIT(19) | \
                                BIT(20) | BIT(21) | BIT(22) | BIT(23) | \
                                BIT(24) | BIT(25) | BIT(26) | BIT(27) | \
                                BIT(28)) /* reserved bits (read/write) */


/* IA32_EFER bits */
#define EFER_SCE                BIT(0)          /* system call extension */
#define EFER_LME                BIT(8)          /* R/W long mode enable */
#define EFER_LMA                BIT(10)         /* R   long mode active */
#define EFER_NXE                BIT(11)         /* no execute enable */
#define EFER_LMA_LME            (EFER_LMA | EFER_LME)

/* XCR0 bits */
#define XSTATE_X87              BIT(0)
#define XSTATE_SSE              BIT(1)
#define XSTATE_AVX              BIT(2)
#define XSTATE_BNDREG           BIT(3)
#define XSTATE_BNDCSR           BIT(4)
#define XSTATE_OPMASK           BIT(5)
#define XSTATE_ZMM_HI256        BIT(6)
#define XSTATE_HI16_ZMM         BIT(7)
#define XSTATE_PT               BIT(8)
#define XSTATE_PKRU             BIT(9)

#define XSTATE_LEGACY           (XSTATE_X87 | XSTATE_SSE)
#define XSTATE_MPX              (XSTATE_BNDREG | XSTATE_BNDCSR)
#define XSTATE_AVX512           (XSTATE_OPMASK | XSTATE_ZMM_HI256 \
                                 | XSTATE_HI16_ZMM)

#endif   /* CPUDEFS_H */
