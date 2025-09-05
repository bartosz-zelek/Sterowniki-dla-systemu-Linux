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

#ifndef SIMCALL_H
#define SIMCALL_H

#define SIMCALL_BREAK                   0x1
#define SIMCALL_LOGPOINT                0x2

static inline void simcall2(unsigned val, uintptr_t val2) {
        unsigned eax = 0x4711;
        __asm__ volatile("cpuid"
                         : "=a" (eax), "=b" (val), "=c" (val2)
                         : "0" (eax), "1" (val), "2" (val2)
                         : "dx");

}
static inline void simcall1(unsigned val) { simcall2(val, 0); }

static inline void
simcall_break(void)
{
        simcall1(SIMCALL_BREAK);
}

static inline void
simcall_logpoint(u64 val)
{
        simcall2(SIMCALL_LOGPOINT, val);
}


#endif   /* SIMCALL_H */
