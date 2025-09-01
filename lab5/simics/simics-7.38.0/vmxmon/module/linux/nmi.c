/*
 * nmi.c - subscribe to NMI such that leaking NMIs do not upset the system.
 * Copyright 2012-2022 Intel Corporation.
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

#include <linux/module.h>
#include <linux/version.h>
#include <linux/perf_event.h>
#include <linux/kdebug.h>
#include <asm/nmi.h>
#include <asm/apic.h>
#include "core.h"
#include "kernel-api.h"

#if defined(CONFIG_PERF_EVENTS)

static struct perf_event_attr stepctr_attr = {
        .type           = PERF_TYPE_HARDWARE,
        .config         = PERF_COUNT_HW_INSTRUCTIONS,
        .size           = sizeof(struct perf_event_attr),
        .pinned         = 1,
};

static void
perf_overflow(struct perf_event *event, struct perf_sample_data *data,
              struct pt_regs *regs)
{       
	/* ensure that our NMI handler never gets throttled */
	event->hw.interrupts = 0;
}

struct perf_event *
create_cpu_perf_event(int cpu)
{
        struct perf_event *ev;

        /* old GCC versions cannot use designated initializers
           for anonymous unions; set the field here instead */
        stepctr_attr.sample_period = 0xffffffff;

        ev = perf_event_create_kernel_counter(
                &stepctr_attr, cpu, NULL, perf_overflow, NULL);

        if (IS_ERR(ev)) {
                printm("Failed to create perf_event on cpu %d: %#lx\n",
                       cpu, PTR_ERR(ev));
                return NULL;
        }

        return ev;
}

void
release_cpu_perf_event(struct perf_event *ev)
{
        if (ev)
                perf_event_release_kernel(ev);
}
#else
struct perf_event *create_cpu_perf_event(int cpu) { return NULL; }
void release_cpu_perf_event(struct perf_event *ev) {}
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 2, 0)

static int
nmi_handler(unsigned int cmd, struct pt_regs *regs)
{       
        switch (cmd) {
        case NMI_LOCAL: 
                return handle_nmi() ? NMI_HANDLED : NMI_DONE;
        case NMI_UNKNOWN:
        default:
                return NMI_DONE;
        }
}

static void
setup_nmi(void)
{
        register_nmi_handler(NMI_LOCAL, nmi_handler, NMI_FLAG_FIRST, "vmxmon");
}

static void
release_nmi(void)
{
        unregister_nmi_handler(NMI_LOCAL, "vmxmon");
}

#else

static int
nmi_callback(struct notifier_block *self,
             unsigned long val, void *data)
{
        switch (val) {
        case DIE_NMI:
                return handle_nmi() ? NOTIFY_STOP : NOTIFY_DONE;
        default:
                return NOTIFY_DONE;
        }
}

static struct notifier_block nmi_nb = {
        .notifier_call  = nmi_callback,
        .priority = NMI_LOCAL_HIGH_PRIOR,
};

static void
setup_nmi(void)
{
        register_die_notifier(&nmi_nb);
}

static inline void
release_nmi(void)
{
        unregister_die_notifier(&nmi_nb);
}

#endif


wrap_nmi_handle_t *
wrap_register_nmi(void)
{
        /* not supported */
        setup_nmi();
        return (void *)(uintptr_t)1;
}

void
wrap_unregister_nmi(wrap_nmi_handle_t *h)
{
        if (h)
                release_nmi();
}
