/*
  x86-reset-bus.c

  © 2010 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <simics/device-api.h>
#include <simics/arch/x86.h>
#include <simics/model-iface/cpu-group.h>
#include <simics/devs/signal.h>

#include "x86-reset-bus.h"

#define DEVICE_NAME "x86-reset-bus"

typedef struct irq_device {
        conf_object_t *obj;
        cpu_list_t reset_tgts;
        VECT(const x86_interface_t *) x86_iface;
        const a20_interface_t *a20_iface;
} irq_device_t;

static void *
init_object(conf_object_t *obj, void *data)
{
        irq_device_t *irq = MM_ZALLOC(1, irq_device_t);
        irq->obj = obj;
        VINIT(irq->reset_tgts);
        VINIT(irq->x86_iface);
        return irq;
}

static int
delete_instance(conf_object_t *obj)
{
        irq_device_t *irq = SIM_object_data(obj);
        VFREE(irq->reset_tgts);
        VFREE(irq->x86_iface);
        MM_FREE(irq);
        return 0;
}

/* Exported through the cpu-group interface */
static const cpu_list_t *
get_cpu_list(conf_object_t *obj)
{
        irq_device_t *irq = SIM_object_data(obj);
        return &irq->reset_tgts;
}

static void
set_a20_line(conf_object_t *obj, int value)
{
	irq_device_t *irq = SIM_object_data(obj);

        if (VLEN(irq->reset_tgts) > 0)
                irq->a20_iface->set_a20_line(VGET(irq->reset_tgts, 0), value);
}

static int
get_a20_line(conf_object_t *obj)
{
	irq_device_t *irq = SIM_object_data(obj);

        if (VLEN(irq->reset_tgts) > 0)
                return irq->a20_iface->get_a20_line(VGET(irq->reset_tgts, 0));
        return 0;
}

static void
reset_all(conf_object_t *obj)
{
        irq_device_t *irq = SIM_object_data(obj);

        VFORI(irq->reset_tgts, i) {
                VGET(irq->x86_iface, i)->set_pin_status(
                        VGET(irq->reset_tgts, i), Pin_Init, 1);
        }
}

static set_error_t
set_reset_tgts(void *arg, conf_object_t *obj, attr_value_t *val,
               attr_value_t *idx)
{
	irq_device_t *irq = SIM_object_data(obj);

        for (int i = 0; i < SIM_attr_list_size(*val); i++) {
                if (!SIM_c_get_interface(
                            SIM_attr_object(SIM_attr_list_item(*val, i)),
                            X86_INTERFACE)) {
                        return Sim_Set_Interface_Not_Found;
                }
                if (i == 0) {
                        if (!SIM_c_get_interface(
                                    SIM_attr_object(SIM_attr_list_item(*val, i)),
                                    A20_INTERFACE)) {
                                return Sim_Set_Interface_Not_Found;
                        }
                }
        }

        VFREE(irq->reset_tgts);
        VFREE(irq->x86_iface);

        for (int i = 0; i < SIM_attr_list_size(*val); i++) {
                conf_object_t *c = SIM_attr_object(SIM_attr_list_item(*val, i));
                VADD(irq->reset_tgts, c);
                VADD(irq->x86_iface, SIM_c_get_interface(c, X86_INTERFACE));
        }
        irq->a20_iface = NULL;
        if (!VEMPTY(irq->reset_tgts)) {
                irq->a20_iface = SIM_C_GET_INTERFACE(
                        VGET(irq->reset_tgts, 0), a20);
        }
        return Sim_Set_Ok;
}

static attr_value_t
get_reset_tgts(void *dont_care, conf_object_t *obj, attr_value_t *idx)
{
	irq_device_t *irq = SIM_object_data(obj);
        attr_value_t ret = SIM_alloc_attr_list(VLEN(irq->reset_tgts));
        int i;
        for (i = 0; i < VLEN(irq->reset_tgts); i++)
                SIM_attr_list_set_item(
                        &ret, i, SIM_make_attr_object(VGET(irq->reset_tgts, i)));
        return ret;
}

static void
reset_all_signal_raise(conf_object_t *obj)
{
	reset_all(obj);
}

static void
reset_all_signal_lower(conf_object_t *obj)
{
	/* do nothing */
}

static void
port_reset_all_signal_raise(conf_object_t *pobj)
{
	reset_all_signal_raise(SIM_port_object_parent(pobj));
}

static void
port_reset_all_signal_lower(conf_object_t *pobj)
{
	reset_all_signal_lower(SIM_port_object_parent(pobj));
}

void
init_local()
{
        class_data_t funcs = {
                .init_object = init_object,
                .delete_instance = delete_instance,
                .class_desc = "forwards resets to processors",
                .description =
		"The " DEVICE_NAME " device forwards resets to connected "
                "x86 processors."
        };
        conf_class_t *class = SIM_register_class(DEVICE_NAME, &funcs);

        static const x86_reset_bus_interface_t xrbi = {
                .set_a20_line = set_a20_line,
                .get_a20_line = get_a20_line,
                .reset_all = reset_all,
        };
        SIM_register_interface(class, X86_RESET_BUS_INTERFACE, &xrbi);

        static const cpu_group_interface_t cgi = {
                .get_cpu_list = get_cpu_list,
        };
        SIM_register_interface(class, CPU_GROUP_INTERFACE, &cgi);

        SIM_register_typed_attribute(
                class, "reset_targets",
                get_reset_tgts, NULL,
                set_reset_tgts, NULL,
                Sim_Attr_Optional, "[o*]", NULL,
                "A list of objects implementing the <tt>" X86_INTERFACE
                "</tt> and <tt>" A20_INTERFACE "</tt> interfaces.");

        SIM_register_typed_attribute(
                class, "cpu_list",
                get_reset_tgts, NULL,
                NULL, NULL,
                Sim_Attr_Pseudo, "[o*]", NULL,
                "List of all connected processors. This attribute is "
                "available in all classes implementing the \""
                CPU_GROUP_INTERFACE "\" interface.");

        conf_class_t *signal_cls = SIM_register_simple_port(
                class, "port.reset_all", "Resets all connected processors");
        static const signal_interface_t port_sigifc = {
                .signal_raise = port_reset_all_signal_raise,
                .signal_lower = port_reset_all_signal_lower
        };
        SIM_REGISTER_INTERFACE(signal_cls, signal, &port_sigifc);

        static const signal_interface_t sigifc = {
                .signal_raise = reset_all_signal_raise,
                .signal_lower = reset_all_signal_lower
        };
        SIM_REGISTER_PORT_INTERFACE(
                class, signal, &sigifc,
                "reset_all", "Resets all connected processors");
}
