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

#include "asmdefs.h"
#include "vmx.h"
#include "asm-offsets.h"
#include "dbg-config.h"

.section .noinstr.text, "ax"


/* do_vmlaunch(host_state_t *h, vmxmon_t *x, bool is_launched) */
.balign 0x1000
SYM_FUNC_START(do_vmlaunch)
        push    %rsi                         // arg2 (vmxmon_t)
        push    %rdi                         // arg1 (host_state_t)
        pushf                                // save rflags on stack

        mov     %rbp,HS_RBP(%rdi)
        mov     %rbx,HS_RBX(%rdi)
        mov     %r12,HS_R12(%rdi)
        mov     %r13,HS_R13(%rdi)
        mov     %r14,HS_R14(%rdi)
        mov     %r15,HS_R15(%rdi)

        mov     $VMCS_HOST_RSP,%rbx          // save stack pointer
        vmwrite %rsp,%rbx

        test    %dl,%dl                     // set rflags per arg3 (is_launched)
        mov     xCR2(%rsi),%rax             // load guest cr2
        mov     %rax,%cr2
        mov     xAX(%rsi),%rax              // load guest gprs
        mov     xBX(%rsi),%rbx
        mov     xDX(%rsi),%rdx
        mov     xBP(%rsi),%rbp
        mov     xCX(%rsi),%rcx
        mov     xR8(%rsi),%r8
        mov     xR9(%rsi),%r9
        mov     xR10(%rsi),%r10
        mov     xR11(%rsi),%r11
        mov     xR12(%rsi),%r12
        mov     xR13(%rsi),%r13
        mov     xR14(%rsi),%r14
        mov     xR15(%rsi),%r15
        mov     xDI(%rsi),%rdi
        mov     xSI(%rsi),%rsi
        /* esp is loaded from VMCS (we can't safely touch it anyway
           since current is accessed from page fault handlers) */
#if VMX_DEBUG_STACK_GUARD
        movq    $0x12345678, -8(%rsp)
        movq    $0x1eaddead, -16(%rsp)
#endif

        jnz .Luse_vmresume
        vmlaunch
        jmp .Lentry_error
.Luse_vmresume:
        vmresume
        /* fall through */

.Lentry_error:
        /* stack: retaddr vmxmon_t host_state_t rflags */
        push    %rcx
        push    %rax

        mov     3 * ADDR_SIZE(%rsp),%rcx      // host_state_t
        pushf
        pop     %rax                          // error info in flags
        mov     %rax,HS_ENTRY_FAILURE(%rcx)   // save error code
        pop     %rax
        pop     %rcx
        /* fall through */

        /* we rely on not getting a page fault below */
SYM_INNER_LABEL_ALIGN(vmx_return, SYM_L_GLOBAL)
        ENDBR
#if VMX_DEBUG_STACK_GUARD
        movq    $0x11111111, -8(%rsp)
        movq    $0x22222222, -16(%rsp)
#endif
        /* stack: retaddr vmxmon_t host_state_t rflags target_ecx */
        push    %rcx

        mov     3 * ADDR_SIZE(%rsp),%rcx      // vmxmon_t
        mov     %rax,xAX(%rcx)                // save eax
        mov     %rbx,xBX(%rcx)                // save ebx
        mov     %rdx,xDX(%rcx)                // save edx
        mov     %rsi,xSI(%rcx)                // save esi
        mov     %rdi,xDI(%rcx)                // save edi
        mov     %rbp,xBP(%rcx)                // save ebp
        mov     %r8,xR8(%rcx)                 // save r8
        mov     %r9,xR9(%rcx)                 // save r9
        mov     %r10,xR10(%rcx)               // save r10
        mov     %r11,xR11(%rcx)               // save r11
        mov     %r12,xR12(%rcx)               // save r12
        mov     %r13,xR13(%rcx)               // save r13
        mov     %r14,xR14(%rcx)               // save r14
        mov     %r15,xR15(%rcx)               // save r15
        pop     %rax
        mov     %rax,xCX(%rcx)
        mov     %cr2,%rax
        mov     %rax,xCR2(%rcx)

        mov     1 * ADDR_SIZE(%rsp),%rax      // host_state_t

        mov     HS_RBX(%rax),%rbx
        mov     HS_RBP(%rax),%rbp
        mov     HS_R12(%rax),%r12
        mov     HS_R13(%rax),%r13
        mov     HS_R14(%rax),%r14
        mov     HS_R15(%rax),%r15

        lgdt    HS_GDT_DESC(%rax)
        lidt    HS_IDT_DESC(%rax)
        popf
        add     $(2 * ADDR_SIZE), %rsp          // pop vmxmon_t, host_state_t
        RET
SYM_FUNC_END(do_vmlaunch)

.section .text, "ax"        
SYM_FUNC_START(unblock_nmi)
        mov     %rsp,%rdi
        mov     %ss,%rax
        push    %rax                            // SS
        push    %rdi                            // RSP
        pushfq                                  // RFLAGS
        mov     %cs,%rax
        push    %rax                            // CS
        lea     1f(%rip), %rax
        push    %rax                            // RIP
        iretq
1:      /* resumes here after iretq */
        RET
SYM_FUNC_END(unblock_nmi)
