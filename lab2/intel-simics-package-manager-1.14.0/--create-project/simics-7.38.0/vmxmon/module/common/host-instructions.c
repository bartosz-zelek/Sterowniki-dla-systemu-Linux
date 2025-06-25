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

#include "host-instructions.h"

void
cpuid_cnt(unsigned op, unsigned cnt, unsigned *eax,
            unsigned *ebx, unsigned *ecx, unsigned *edx)
{
        __asm__("cpuid"
            : "=a" (*eax), "=b" (*ebx), "=c" (*ecx), "=d" (*edx)
            : "0" (op), "c" (cnt));
}

void
cpuid_cla(unsigned op, unsigned *eax, unsigned *ebx,
           unsigned *ecx, unsigned *edx)
{
        cpuid_cnt(op, 0, eax, ebx, ecx, edx);
}

unsigned
cpuid_eax(unsigned op)
{
        unsigned eax = 0;
        unsigned ebx = 0;
        unsigned ecx = 0;
        unsigned edx = 0;
        cpuid_cla(op, &eax, &ebx, &ecx, &edx);
        return eax;
}

unsigned
cpuid_ebx(unsigned op)
{
        unsigned eax = 0;
        unsigned ebx = 0;
        unsigned ecx = 0;
        unsigned edx = 0;
        cpuid_cla(op, &eax, &ebx, &ecx, &edx);
        return ebx;
}

unsigned
cpuid_ecx(unsigned op)
{
        unsigned eax = 0;
        unsigned ebx = 0;
        unsigned ecx = 0;
        unsigned edx = 0;
        cpuid_cla(op, &eax, &ebx, &ecx, &edx);
        return ecx;
}

unsigned
cpuid_edx(unsigned op)
{
        unsigned eax = 0;
        unsigned ebx = 0;
        unsigned ecx = 0;
        unsigned edx = 0;
        cpuid_cla(op, &eax, &ebx, &ecx, &edx);
        return edx;
}
