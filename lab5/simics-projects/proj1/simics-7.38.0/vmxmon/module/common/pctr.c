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

#include "kernel-api.h"
#include "cpudefs.h"
#include "vmxmon.h"
#include "vm.h"
#include "vmx.h"
#include "asm-inlines.h"
#include "tlb.h"
#include "pctr.h"
#include "host-instructions.h"

#define P4_PMC_MASK           (BIT(40) - 1)


/* Table A-1. (event << 8) | umask */
#define APM_UNHALTED_CORE_CYCLES       0x3c00
#define APM_UNHALTED_REF_CYCLES        0x3c01
#define APM_INST_RETIRED               0xc000

static inline u64
make_perfevtsel_val(unsigned event) 
{
        return BIT(22)                          /* counter enable */
                | BIT(20)                       /* interrupt enable */
                | BIT(16)                       /* USR */
                | BIT(17)                       /* OS */
                | ((event & 0xff) << 8)         /* umask */
                | ((event >> 8) & 0xff);        /* event# */
}

static inline u64
make_iq_val(int thread) 
{
        return BIT(12) | (3 << 16) | (5 << 13)
                | ((thread == 0) ? BIT(26) : BIT(27));
}

static inline u64
get_fixed_mask(void)
{
        unsigned eax, ebx, ecx, edx;
        cpuid_cla(0xa, &eax, &ebx, &ecx, &edx);

        unsigned bits = (edx >> 5) & 0xff;

        /* early v2 reports incorrect information; assume same width as PMC */
        if (edx == 0)
                bits = (eax >> 16) & 0xff;
        return (bits == 64) ? ~0ULL : (1ULL << bits) - 1;
}

static int
p4_available(void)
{
        u32 eax = cpuid_eax(1);
        u32 family = (eax >> 8) & 0xf;
        family += (family == 0xf) ? ((eax >> 20) & 0xff) : 0;

        return (family == 0xf);
}

static int
xeon_phi_available(void)
{
        u32 eax = cpuid_eax(1);

        u32 family = (eax >> 8) & 0xf;
        family += (family == 0xf) ? ((eax >> 20) & 0xff) : 0;
        u32 model = eax >> 4 & 0xf;
        model |= ((eax >> 16) & 0xf) << 4;

        return ((family == 0x6) && (model == 0x57));
}

static bool
get_and_clear_overflow_apm2(const cpu_state_t *cs)
{
        u64 v = cs->pmc.perf_global_ctrl;

        if (rdmsr(MSR_IA32_PERF_GLOBAL_STATUS) & v) {
                wrmsr(MSR_IA32_PERF_GLOBAL_OVF_CTRL, v);
                return true;
        }
        return false;
}

static inline pctr_t
pctr_apm2(void) 
{
        return (pctr_t) {
                .mask = get_fixed_mask(),

                .step_msr = MSR_IA32_FIXED_CTR0,
                .ctrl_msr = MSR_IA32_FIXED_CTR_CTRL,
                .ctrl_val = 0x8 | 0x3,
                .perf_global_ctrl = BIT(32),

                .counts_injections = true,
                .counts_vmxenter = false,

                .get_and_clear_overflow = get_and_clear_overflow_apm2,
        };
}

static inline pctr_t
pctr_xeon_phi(void)
{
        pctr_t pmc = pctr_apm2();
        pmc.counts_vmxenter = true;
        return pmc;
}

static bool
get_and_clear_overflow_apm1(const cpu_state_t *cs)
{
        /* counter overflows when msb goes from 1 -> 0 */
        bool msb = rdmsr(MSR_IA32_PMC0) & BIT(31);
        if (!msb) {
                /* set MSB bit to avoid claiming the next NMI */
                wrmsr(MSR_IA32_PMC0, BIT(31));
        }
        return !msb;
}

static inline pctr_t
pctr_apm1(void)
{
        return (pctr_t) {
                .mask = 0xffffffff,     /* write is 32-bit sign extend */

                .step_msr = MSR_IA32_PMC0,
                .ctrl_msr = MSR_IA32_PERFEVTSEL0,
                .ctrl_val = make_perfevtsel_val(APM_INST_RETIRED),

                .counts_injections = true,
                .counts_vmxenter = false,

                 /* APM1 lacks overflow bits for performance counters */
                .get_and_clear_overflow = get_and_clear_overflow_apm1,
        };
}

static bool
get_and_clear_overflow_p4(const cpu_state_t *cs)
{
        const u64 cccr_ovf = BIT(31);
        u64 cccr = rdmsr(cs->pmc.ctrl_msr);

        if (cccr & cccr_ovf) {
                wrmsr(cs->pmc.ctrl_msr, cccr & ~cccr_ovf);
                return true;
        }
        return false;
}

static inline pctr_t
pctr_p4(int thread)
{
        return (pctr_t) {
                .mask = P4_PMC_MASK,

                .step_msr = (thread == 0) ? MSR_IQ_COUNTER0 : MSR_IQ_COUNTER2,
                .ctrl_msr = (thread == 0) ? MSR_IQ_CCCR0 : MSR_IQ_CCCR2,
                .ctrl_val = make_iq_val(thread),
                .ctrl2_msr = (thread == 0) ? MSR_CRU_ESCR2 : MSR_CRU_ESCR3,
                .ctrl2_val = (7 << 25) | (1 << 9) | BIT(2) | BIT(3),

                .counts_injections = false,
                .counts_vmxenter = true,

                .get_and_clear_overflow = get_and_clear_overflow_p4,
        };
}

void
pctr_setup_cpu_info(cpu_state_t *cs)
{
        cs->use_apm1 = mp.use_apm1;
        if (gi.apm_version && cs->use_apm1)
                cs->pmc = pctr_apm1();
        else if (xeon_phi_available())
                cs->pmc = pctr_xeon_phi();
        else if (gi.apm_version >= 2)
                cs->pmc = pctr_apm2();
        else if (gi.apm_version == 1)
                cs->pmc = pctr_apm1();
        else if (p4_available())
                cs->pmc = pctr_p4(cs->thread);
        else {
                wrap_BUG_ON(1);
        }
}

bool
pctrs_supported(void)       
{
        return (gi.apm_version >= 1) || p4_available();
}
