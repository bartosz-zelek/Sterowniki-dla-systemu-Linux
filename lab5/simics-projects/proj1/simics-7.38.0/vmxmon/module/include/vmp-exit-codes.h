/*
  Copyright 2015-2025 Intel Corporation

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

#ifndef VMP_EXIT_CODES_H
#define VMP_EXIT_CODES_H

/* ioctl return reasons */
#define VMRET_NOP 1

#define VMRET_TRIPLE_FAULT 2
#define VMRET_INIT 3
#define VMRET_STARTUP_IPI 4
#define VMRET_IRQ_WINDOW 5
#define VMRET_TASK_SWITCH 6
#define VMRET_CPUID 7
#define VMRET_HLT 8
#define VMRET_INVD 9
#define VMRET_INVLPG 10
#define VMRET_RDPMC 11
#define VMRET_RDTSC 12
#define VMRET_VMCALL 13
#define VMRET_VMCLEAR 14
#define VMRET_VMLAUNCH 15
#define VMRET_VMPTRLD 16
#define VMRET_VMPTRST 17
#define VMRET_VMREAD 18
#define VMRET_VMRESUME 19
#define VMRET_VMWRITE 20
#define VMRET_VMXOFF 21
#define VMRET_VMXON 22
#define VMRET_CTRL_REG_ACCESS 23
#define VMRET_MOV_DR 24
#define VMRET_IO 25
#define VMRET_RDMSR 26
#define VMRET_WRMSR 27
#define VMRET_INV_GUEST_STATE 28 /* entry failure */
#define VMRET_MSR_LOAD_FAILURE 29 /* entry failure */
#define VMRET_MWAIT 30
#define VMRET_MONITOR 31
#define VMRET_PAUSE 32
#define VMRET_MACHINE_CHECK 33 /* entry failure */
#define VMRET_TPR_BELOW_THRESH 34
#define VMRET_EXC 35

#define VMRET_INTERNAL_ERROR 36 /* should not occur */
#define VMRET_ENTRY_ERROR 37
#define VMRET_PHYSPAGE_MISSING 38 /* q: paddr, ecode */
#define VMRET_PAGE_FAULT 39 /* q: paddr, ecode */
#define VMRET_CR0_VALUE_UNSUPPORTED 40
#define VMRET_STEP_TIMER 41 /* step timer expiry */
#define VMRET_AGAIN 42 /* INTERNAL */
#define VMRET_EFER_VALUE_UNSUPPORTED 43 /* MSR_EFER invalid */
#define VMRET_VMXON_FAILURE 44 /* VMXON failed */
#define VMRET_OUT_OF_MEMORY 45
#define VMRET_PROTECTION 46 /* q: paddr, ecode,
 page_access_rights */
#define VMRET_GUEST_STATE_UNSUPPORTED 47
#define VMRET_ICEBP 48
#define VMRET_RANGE_BREAKPOINT 49 /* q: va, ecode */
#define VMRET_TICK_TIMER 50

#define VMRET_DBG_SWITCH_CPU 51 /* run on other cpu */
#define VMRET_TRACE_BUF_FULL 52

#define VMRET_NMI_WINDOW 53

#define VMRET_INVEPT 54
#define VMRET_RDTSCP 55
#define VMRET_INVVPID 56
#define VMRET_WBINVD 57
#define VMRET_XSETBV 58
#define VMRET_GDTR_IDTR_ACCESS 59
#define VMRET_LDTR_TR_ACCESS 60
#define VMRET_RDRAND 61
#define VMRET_INVPCID 62
#define VMRET_VMFUNC 63
#define VMRET_RDSEED 64
#define VMRET_ENCLS 65
#define VMRET_ENCLV 66

#define VMRET_EMULATE 78
#define VMRET_NM_ERRATA_EMULATION 79

/* VMRET instruction exit range (for future expansion) */
#define VMRET_INST_EXIT_RANGE_START 54
#define VMRET_INST_EXIT_RANGE_END 79

#define VMRET_GUEST_EPT_VIOLATION 80
#define VMRET_GUEST_EPT_MISCONFIG 81
#define VMRET_FPU_DISABLED 82
#define VMRET_SIGNALED 83
#define VMRET_PREEMPTION_TIMER 84

#define VMRET_PKU_VIOLATION 94

#define VMRET_COUNT 95

#endif // VMP_EXIT_CODES_H
