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

#ifndef VM_EXIT_CODES_H
#define VM_EXIT_CODES_H

/* VM-exit reasons with which host CPU ends non-root mode executions */
#define VMEXIT_EXC 0
#define VMEXIT_IRQ 1
#define VMEXIT_TRIPLE_FAULT 2
#define VMEXIT_INIT 3
#define VMEXIT_STARTUP_IPI 4
#define VMEXIT_IRQ_WINDOW 7
#define VMEXIT_NMI_WINDOW 8
#define VMEXIT_TASK_SWITCH 9
#define VMEXIT_CPUID 10
#define VMEXIT_GETSEC 11
#define VMEXIT_HLT 12
#define VMEXIT_INVD 13
#define VMEXIT_INVLPG 14
#define VMEXIT_RDPMC 15
#define VMEXIT_RDTSC 16
#define VMEXIT_RSM 17
#define VMEXIT_VMCALL 18
#define VMEXIT_VMCLEAR 19
#define VMEXIT_VMLAUNCH 20
#define VMEXIT_VMPTRLD 21
#define VMEXIT_VMPTRST 22
#define VMEXIT_VMREAD 23
#define VMEXIT_VMRESUME 24
#define VMEXIT_VMWRITE 25
#define VMEXIT_VMXOFF 26
#define VMEXIT_VMXON 27
#define VMEXIT_CTRL_REG_ACCESS 28
#define VMEXIT_MOV_DR 29
#define VMEXIT_IO 30
#define VMEXIT_RDMSR 31
#define VMEXIT_WRMSR 32
#define VMEXIT_INV_GUEST_STATE 33 /* entry failure */
#define VMEXIT_MSR_LOAD_FAILURE 34 /* entry failure */
#define VMEXIT_MWAIT 36
#define VMEXIT_MONITOR_TRAP_FLAG 37
#define VMEXIT_MONITOR 39
#define VMEXIT_PAUSE 40
#define VMEXIT_MACHINE_CHECK 41 /* entry failure */
#define VMEXIT_TPR_BELOW_THRESH 43

#define VMEXIT_APIC_ACCESS 44
#define VMEXIT_VIRTUALIZED_EOI 45
#define VMEXIT_GDTR_IDTR_ACCESS 46
#define VMEXIT_LDTR_TR_ACCESS 47
#define VMEXIT_EPT_VIOLATION 48
#define VMEXIT_EPT_MISCONFIG 49
#define VMEXIT_INVEPT 50
#define VMEXIT_RDTSCP 51
#define VMEXIT_PREEMPT_TIMER_EXPIRED 52
#define VMEXIT_INVVPID 53
#define VMEXIT_WBINVD 54
#define VMEXIT_XSETBV 55
#define VMEXIT_APIC_WRITE 56
#define VMEXIT_RDRAND 57
#define VMEXIT_INVPCID 58
#define VMEXIT_VMFUNC 59
#define VMEXIT_ENCLS 60
#define VMEXIT_RDSEED 61
#define VMEXIT_PML_FULL 62
#define VMEXIT_XSAVES 63

#define VMEXIT_XRSTORS 64
#define VMEXIT_SPP_EVENT 66
#define VMEXIT_UMWAIT 67
#define VMEXIT_TPAUSE 68

#define VMEXIT_COUNT 69

#endif // VM_EXIT_CODES_H
