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

#ifndef HOST_INSTRUCTIONS_H
#define HOST_INSTRUCTIONS_H

#ifdef __cplusplus
extern "C" {
#endif

void cpuid_cnt(unsigned op, unsigned cnt, unsigned *eax, unsigned *ebx,
        unsigned *ecx, unsigned *edx);
void cpuid_cla(unsigned op, unsigned *eax, unsigned *ebx, unsigned *ecx,
        unsigned *edx);
unsigned cpuid_eax(unsigned op);
unsigned cpuid_ebx(unsigned op);
unsigned cpuid_ecx(unsigned op);
unsigned cpuid_edx(unsigned op);

#ifdef __cplusplus
}
#endif
#endif   /* HOST_INSTRUCTIONS_H */
