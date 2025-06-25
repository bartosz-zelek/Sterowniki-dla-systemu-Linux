/*
 * Copyright 2015-2024 Intel Corporation.
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

#define PERFCTR_VMX_SWITCH              0
#define PERFCTR_USERSPACE_SWITCH        1
#define PERFCTR_VMEXIT_EXC_IRQ          4
#define PERFCTR_VMEXIT_EXC_NMI          5
#define PERFCTR_VMEXIT_EXC_NM           6
#define PERFCTR_VMEXIT_EXC_PF           7
#define PERFCTR_VMEXIT_EXC_DB           8
#define PERFCTR_VMEXIT_EXC_OTHER        9
#define PERFCTR_FPU_OBTAIN              10
#define PERFCTR_GET_REGS                11
#define PERFCTR_GUEST_EXC_PF            12
#define PERFCTR_TLB_PTE_MISSING         13
#define PERFCTR_TLB_PTE_WRITE_ENABLE    14
#define PERFCTR_TLB_PTE_OTHER           15

#define PERFCTR_SIM_STEPS               19
#define PERFCTR_DBG1                    42
#define PERFCTR_DBG2                    43

#define PERFCTR_N_MAPPED_PAGES          46
#define PERFCTR_N_LOCKED_PAGES          47
#define PERFCTR_N_MEMORY_LIMIT_PRUNE    48
#define PERFCTR_N_MAPPED_LIMIT_PRUNE    41

#define PERFCTR_TLB_NEW_CR3             50
#define PERFCTR_TLB_FLUSH_CR3           51
#define PERFCTR_TLB_NEW_SEGMENT         52
#define PERFCTR_TLB_VERIFY_SEGMENT      53
#define PERFCTR_TLB_REUSE_SEGMENT       54
#define PERFCTR_TLB_FLUSH_SPAGE         55
#define PERFCTR_TLB_VERIFY_SEGPAGE_SEGS 56
#define PERFCTR_TLB_SEGMENT_DIRTY_FLUSH 57
#define PERFCTR_TLB_SEGMENT_UNLINKED    58
#define PERFCTR_TLB_GLOBAL_OPT          59
#define PERFCTR_TLB_FLUSH_ALL           60
#define PERFCTR_TLB_VERIFY_LEVEL        61

#define PERFCTR_INVVPID                 62
#define PERFCTR_INVEPT                  63

#define PERFCTR_TLB_LARGE_USER_PAGE     44
#define PERFCTR_TLB_USER_PAGE           45
#define PERFCTR_TLB_SU_PAGE             49
#define PERFCTR_TLB_PTE_MISSING_CLEARED 37
#define PERFCTR_TLB_ENTRY_PROTECTED     38
#define PERFCTR_TLB_CR3_CHANGE          39
#define PERFCTR_TLB_LARGE_SU_PAGE       40

#define PERFCTR_GUEST_EPT_LOOKUP        64
#define PERFCTR_GUEST_EPT_MISS          65
#define PERFCTR_GUEST_EPT_NOPAGE        66

#define PERFCTR_COUNT                   67
