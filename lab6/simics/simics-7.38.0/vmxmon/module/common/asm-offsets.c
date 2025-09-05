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

/*
 * constant extraction from C header files
 */

#include "kernel-api.h"

#include "vmxmon.h"
#include "vm.h"
#include "vmx.h"

#ifdef _MSC_VER

#include <stddef.h>

#define DEFINE(sym, val) \
        extern volatile size_t sym = val

#define HS_DEF(sym, val ) \
        DEFINE(sym, offsetof(host_state_t, val ))

#define VM_DEF(sym, val ) \
        DEFINE(sym, offsetof(vm_t, val ))

#define xDEF(sym, val ) \
        DEFINE(sym, offsetof(vmxmon_t, val ))

VM_DEF(VM_HOST_STATE, host_state);         /* fixme */
VM_DEF(VM_VMXMON, com);

HS_DEF(HS_GDT_DESC, gdt_desc);
HS_DEF(HS_IDT_DESC, idt_desc);
HS_DEF(HS_TSS_SEL, tss_sel);

HS_DEF(HS_RBP, rbp);
HS_DEF(HS_RBX, rbx);
HS_DEF(HS_RDI, rdi);
HS_DEF(HS_RSI, rsi);
HS_DEF(HS_R12, r12);
HS_DEF(HS_R13, r13);
HS_DEF(HS_R14, r14);
HS_DEF(HS_R15, r15);

HS_DEF(HS_ENTRY_FAILURE, entry_failure);

xDEF(xAX, gprs[GPR_AX]);
xDEF(xBX, gprs[GPR_BX]);
xDEF(xCX, gprs[GPR_CX]);
xDEF(xDX, gprs[GPR_DX]);
xDEF(xSI, gprs[GPR_SI]);
xDEF(xDI, gprs[GPR_DI]);
xDEF(xBP, gprs[GPR_BP]);
xDEF(xR8, gprs[8]);
xDEF(xR9, gprs[9]);
xDEF(xR10, gprs[10]);
xDEF(xR11, gprs[11]);
xDEF(xR12, gprs[12]);
xDEF(xR13, gprs[13]);
xDEF(xR14, gprs[14]);
xDEF(xR15, gprs[15]);

//xDEF(xSP, gprs[GPR_SP]);

xDEF(xCR2, cr2);

DEFINE(ADDR_SIZE, sizeof(void*));
DEFINE(HOST_RSP, VMCS_HOST_RSP);

#else

/* This is an awful abuse of GCC inline asm to generate a definition of a
   struct offset as a symbol that can be used in asm code. The whole concept is
   highly dubious. */
#define DEFINE(sym, val) \
        asm volatile(".set " #sym ", %c0" : : "i" (val))

#define HS_DEF(sym, val ) \
        DEFINE(sym, __builtin_offsetof(host_state_t, val ))

#define VM_DEF(sym, val ) \
        DEFINE(sym, __builtin_offsetof(vm_t, val ))

#define xDEF(sym, val ) \
        DEFINE(sym, __builtin_offsetof(vmxmon_t, val ))

int main(void);

int
main( void )
{
        VM_DEF(VM_HOST_STATE, host_state);
        VM_DEF(VM_VMXMON, com);

        HS_DEF(HS_GDT_DESC, gdt_desc);
        HS_DEF(HS_IDT_DESC, idt_desc);
        HS_DEF(HS_TSS_SEL, tss_sel);

        HS_DEF(HS_RBP, rbp);
        HS_DEF(HS_RBX, rbx);
        HS_DEF(HS_R12, r12);
        HS_DEF(HS_R13, r13);
        HS_DEF(HS_R14, r14);
        HS_DEF(HS_R15, r15);

        HS_DEF(HS_ENTRY_FAILURE, entry_failure);

        xDEF(xAX, gprs[GPR_AX]);
        xDEF(xBX, gprs[GPR_BX]);
        xDEF(xCX, gprs[GPR_CX]);
        xDEF(xDX, gprs[GPR_DX]);
        xDEF(xSI, gprs[GPR_SI]);
        xDEF(xDI, gprs[GPR_DI]);
        xDEF(xBP, gprs[GPR_BP]);
        xDEF(xR8, gprs[8]);
        xDEF(xR9, gprs[9]);
        xDEF(xR10, gprs[10]);
        xDEF(xR11, gprs[11]);
        xDEF(xR12, gprs[12]);
        xDEF(xR13, gprs[13]);
        xDEF(xR14, gprs[14]);
        xDEF(xR15, gprs[15]);

        //xDEF(xSP, gprs[GPR_SP]);

        xDEF(xCR2, cr2);

        DEFINE(ADDR_SIZE, sizeof(void*));

        return 0;
}

#endif
