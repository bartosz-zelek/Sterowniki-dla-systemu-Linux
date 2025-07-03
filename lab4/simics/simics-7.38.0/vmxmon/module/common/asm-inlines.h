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

#ifndef ASM_INLINES_H
#define ASM_INLINES_H

#include "kernel-api.h"

#pragma pack(push, 1)
typedef struct {
        u16             limit;
        uintptr_t       base;
} descriptor_t;
#pragma pack(pop)

#define INVVPID_VPID_ADDR               0
#define INVVPID_VPID_CONTEXT            1
#define INVVPID_ALL                     2
#define INVVPID_VPID_LOCAL_CONTEXT      3

#define INVEPT_CONTEXT                  1
#define INVEPT_ALL_CONTEXTS             2

#define set_dr(n, val) \
        __asm__ volatile("mov %0,%%dr" # n :: "r" ((uintptr_t)(val)) : "cc")

static inline u16
get_cs(void)
{
        u16 val;
        __asm__ volatile("mov %%cs,%0" : "=r" (val));
        return val;
}

static inline u16
get_ss(void)
{
        u16 val;
        __asm__ volatile("mov %%ss,%0" : "=r" (val));
        return val;
}

static inline u16
get_ds(void)
{
        u16 val;
        __asm__ volatile("mov %%ds,%0" : "=r" (val));
        return val;
}

static inline u16
get_es(void)
{
        u16 val;
        __asm__ volatile("mov %%es,%0" : "=r" (val));
        return val;
}

static inline u16
get_gs(void)
{
        u16 val;
        __asm__ volatile("mov %%gs,%0" : "=r" (val));
        return val;
}

static inline u16
get_fs(void)
{
        u16 val;
        __asm__ volatile("mov %%fs,%0" : "=r" (val));
        return val;
}

static inline void
set_xcr0(u64 val)
{
        u32 eax = val & 0xffffffff;
        u32 edx = val >> 32;
        u32 ecx = 0;
        /* xsetbv */
        __asm__ volatile(".byte 0x0f, 0x01, 0xd1" 
                         :: "a" (eax), "d" (edx), "c" (ecx));
}

static inline u64
get_xcr0(void)
{
        u32 eax, edx;
        u32 ecx = 0;
        /* xgetbv */
        __asm__ volatile(".byte 0x0f, 0x01, 0xd0"
                         : "=a" (eax), "=d" (edx) : "c" (ecx));
        return ((u64)edx << 32) | eax;
}

static inline void
set_pkru(u32 val)
{
        u32 eax = val, ecx = 0, edx = 0;
        /* wrpkru */
        __asm__ volatile(".byte 0x0f, 0x01, 0xef"
                         :: "a" (eax), "c"(ecx), "d" (edx));
}

static inline u32
get_pkru(void)
{
        u32 eax, ecx = 0;
        /* rdpkru*/
        __asm__ volatile(".byte 0xf, 0x01, 0xee"
                         : "=a" (eax) : "c" (ecx) : "rdx");
        return eax;
}

static inline uintptr_t
get_cr8(void)
{
        uintptr_t ret;
        __asm__ volatile("mov %%cr8,%0" : "=r" (ret) :: "cc");
        return ret;
}

static inline uintptr_t
cr4_set_bits(uintptr_t mask)
{
#ifdef __linux__
        return wrap_cr4_set_bits(mask);
#else
        uintptr_t v;
        __asm__ volatile("mov %%cr4,%0" : "=r" (v) :: "cc");
        v |= mask;
        __asm__ volatile("mov %0,%%cr4" :: "r" (v) : "cc");
        return v;
#endif
}

static inline void
cr4_clear_bits(uintptr_t old_cr4, uintptr_t mask)
{
#ifdef __linux__
        wrap_cr4_clear_bits(old_cr4, mask);
#else
        old_cr4 &= ~mask;
        __asm__ volatile("mov %0,%%cr4" :: "r" (old_cr4) : "cc");
#endif
}

static inline uintptr_t
get_cr4(void)
{
        uintptr_t ret;
        __asm__ volatile("mov %%cr4,%0" : "=r" (ret) :: "cc");
        return ret;
}

static inline uintptr_t
get_cr3(void)
{
        uintptr_t ret;
        __asm__ volatile("mov %%cr3,%0" : "=r" (ret) :: "cc");
        return ret;
}

static inline uintptr_t
get_cr0(void)
{
        uintptr_t ret;
        __asm__ volatile("mov %%cr0,%0" : "=r" (ret) :: "cc");
        return ret;
}

static inline u64
rdmsr(u32 msr)
{
        u32 eax, edx;
        __asm__ volatile("rdmsr" : "=a" (eax), "=d" (edx) : "c" (msr));
        return ((u64)edx << 32) | eax;
}

static inline u64
rdpmc(u32 pctr_idx)
{
        u32 eax, edx;
        __asm__ volatile("rdpmc" : "=a" (eax), "=d" (edx) : "c" (pctr_idx));
        return ((u64)edx << 32) | eax;
}

static inline u64
readtsc(void)
{
        u32 eax, edx;
        __asm__ volatile("rdtsc" : "=a" (eax), "=d" (edx));
        return ((u64)edx << 32) | eax;
}

static inline void
wrmsr(u32 msr, u64 value)
{
        u32 eax = value & 0xffffffff;
        u32 edx = value >> 32;
        __asm__ volatile("wrmsr" :: "a" (eax), "d" (edx), "c" (msr));
}

static inline void
get_gdt_desc(descriptor_t *di)
{
        __asm__ volatile("sgdt %0" : "=m" (*di));
}

static inline void
get_idt_desc(descriptor_t *di)
{
        __asm__ volatile("sidt %0" : "=m" (*di));
}

static inline u16
get_ldt(void)
{
        u16 ret;
        __asm__ volatile("sldt %0" : "=rm" (ret));
        return ret;
}

static inline void
set_ldt(u16 sel)
{
        __asm__ volatile("lldt %0" :: "rm" (sel));
}

static inline u16
get_tr(void)
{
        u16 seg;
        __asm__ volatile("str %0" : "=rm" (seg));
        return seg;
}

static inline void
set_tr(u16 sel)
{
        __asm__ volatile("ltr %0" :: "rm" (sel));
}

static inline void
fxsave64(void *ptr)
{
        __asm__ volatile("fxsave64 %0" : "=m" (*(char *)ptr));
}

static inline void
fxrstor64(void *ptr)
{
        __asm__ volatile("fxrstor64 %0" :: "m" (*(char *)ptr));
}

static inline void
xsave64(void *ptr, u32 eax, u32 edx)
{
        __asm__ volatile(
                "xsave64 %0"
                : "=m" (*(char *) ptr)
                : "a" (eax), "d" (edx));
}

/************************************************************************/
/*      VMX                                                             */
/************************************************************************/

static inline int
vmxon(u64 phys_page)
{
        uintptr_t ret;
        __asm__ volatile("vmxon %1\n\t"
                         "pushf\n\t"
                         "pop %0" : "=r" (ret) : "m" (phys_page) : "cc");
        return (unsigned)ret & 0x41;
}

static inline void
vmxoff(void)
{
        __asm__ volatile("vmxoff" ::: "cc");
}

static inline void
vmclear(u64 phys_vmcs)
{
        __asm__ volatile("vmclear %0" :: "m" (phys_vmcs) : "cc");
}

static inline void
vmptrld(u64 phys_vmcs)
{
        __asm__ volatile("vmptrld %0" :: "m" (phys_vmcs) : "cc");
}

static inline u64
vmptrst(void)
{
        u64 phys_vmcs;
        __asm__ volatile("vmptrst %0" : "=m" (phys_vmcs) :: "cc");
        return phys_vmcs;
}

static inline void
vmwrite(uintptr_t reg_encoding, uintptr_t val)
{
        __asm__ volatile("vmwrite %1,%0"
                         :: "r" (reg_encoding), "r" (val) : "cc");
}

static inline bool
vmwrite_checked(uintptr_t reg_encoding, uintptr_t val)
{
        u8 error = 0;
        __asm__ volatile("vmwrite %2, %1 \n\t"
                         "setna %0"
                : "=qm" (error)
                : "r" (reg_encoding), "r" (val)
                : "cc");
        return error == 0;
}

static inline uintptr_t
vmread(uintptr_t reg_encoding)
{
        uintptr_t val = 0;
        __asm__ volatile("vmread %1, %0"
                         : "=r" (val) : "r" (reg_encoding) : "cc");
        return val;
}

static inline void
invvpid(u32 type, u16 vpid, u64 va)
{
        struct {
                u64 vpid : 16;
                u64 res  : 48;
                u64 va;
        } desc = { .vpid = vpid, .va = va };

        /* invvpid (%rax), %rcx */
        __asm__ volatile(".byte 0x66, 0x0f, 0x38, 0x81, 0x08"
                     :: "a" (&desc), "c" ((uintptr_t)type), "m" (desc)
                     : "cc");
}

static inline void
invept(u32 type, u64 eptp)
{
        struct {
                u64 eptp;
                u64 reserved;
        } desc = { .eptp = eptp };

        /* invept (%rax), %rcx */
        __asm__ volatile(".byte 0x66, 0x0f, 0x38, 0x80, 0x08"
                     :: "a" (&desc), "c" ((uintptr_t)type), "m" (desc)
                     : "cc");
}

static inline uintptr_t
get_gdt_base(void)
{
        descriptor_t desc;
        get_gdt_desc(&desc);
        return desc.base;
}

static inline u32 *
get_gdt_desc_ptr(u16 seg)
{
        return (u32 *)(get_gdt_base() + (seg & ~7));
}

static inline uintptr_t
get_tr_base(void) 
{
        u16 seg = get_tr() & ~7;
        u32 *p = get_gdt_desc_ptr(seg);
        uintptr_t ret = (p[0] >> 16)
                | ((p[1] & 0xff) << 16)
                | (p[1] & (0xff << 24));
        ret |= ((u64)p[2] << 32);
        return ret;
}

static inline void
atomic_inc(u32 *p) 
{
        __asm__("lock addl %1,%0" : "+m" (*p) : "i" (1) : "cc");
}

static inline void
atomic_dec(u32 *p) 
{
        __asm__("lock subl %1,%0" : "+m" (*p) : "i" (1) : "cc");
}

#endif   /* ASM_INLINES_H */
