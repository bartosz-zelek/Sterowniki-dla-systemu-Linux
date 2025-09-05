/*
  © 2016 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include "generic-view.h"
#include <simics/base/log.h>
#include <simics/simulator/conf-object.h>
#include <simics/simulator/output.h>

/* histogram-views */
#include "mnemonic-view.h"
#include "size-view.h"
#include "x86-normalized-view.h"
#include "xed-iform-view.h"

FORCE_INLINE view_connection_t *
view_connection_of_obj(conf_object_t *obj)
{
        return (view_connection_t *)obj;
}


// Callback when an instruction is about to be cached
static void
cached_instruction_cb(
        conf_object_t *obj, conf_object_t *cpu,
        cached_instruction_handle_t *handle,
        instruction_handle_t *iq_handle,
        void *connection_user_data)
{
        view_connection_t *vc = view_connection_of_obj(obj);
        
        /* Get hold of the instruction sequence Simics just saw */
        cpu_bytes_t opcode = vc->iq_iface->get_instruction_bytes(
                cpu, iq_handle);

        /* Let the decoder decide what instruction this is and return
           a unique pointer for the corresponding instruction class. */
        uint64 *counter = vc->get_counter(vc, opcode.data, opcode.size);

        /* Add a counter which automatically increments each time this
           instruction gets executed later. */
        if (counter)
                vc->ci_iface->add_counter(cpu, handle, counter, false);
}


conf_object_t *
new_connection(conf_object_t *parent,
               conf_object_t *provider,
               int num,
               view_type_t view,
               attr_value_t args)
{
        view_connection_t *new = NULL;
        switch (view) {
        case View_Mnemonic:
                new = new_mnemonic_view(parent, provider, num, args);
                break;
        case View_Size:
                new = new_size_view(parent, provider, num, args);
                break;
        case View_X86_Normalized:
                new = new_x86_normalized_view(parent, provider, num, args);
                break;
        case View_Xed_Iform:
                new = new_xed_iform_view(parent, provider, num, args);
                break;
        }
        if (!new)
                return NULL;                 /* Missing interfaces? */
                
        /* Cache up generic interfaces */
        new->cis_iface = SIM_C_GET_INTERFACE(provider,
                                             cpu_instrumentation_subscribe);
        new->iq_iface = SIM_C_GET_INTERFACE(provider, cpu_instruction_query);
        new->ci_iface =  SIM_C_GET_INTERFACE(provider, cpu_cached_instruction);

        /* Request a callback each time an instruction is cached.
           The generic-view code receives the callback. */
        new->cis_iface->register_cached_instruction_cb(
                provider, &new->obj, cached_instruction_cb, NULL);

        return &new->obj;
}


/* Sub-object attributes */
static attr_value_t
get_histogram(void *param, conf_object_t *obj, attr_value_t *idx)
{
        view_connection_t *vc = view_connection_of_obj(obj);
        if (!vc->get_histogram)
                return SIM_make_attr_nil();
        return vc->get_histogram(vc);
}

static attr_value_t
get_cpu(void *param, conf_object_t *obj, attr_value_t *idx)
{
        view_connection_t *vc = view_connection_of_obj(obj);
        return SIM_make_attr_object(vc->cpu);
}

static set_error_t
set_cpu(void *param, conf_object_t *obj, attr_value_t *val,
        attr_value_t *idx)
{
        view_connection_t *vc = view_connection_of_obj(obj);
        vc->cpu = SIM_attr_object(*val);
        return Sim_Set_Ok;
}

static attr_value_t
get_parent(void *param, conf_object_t *obj, attr_value_t *idx)
{
        view_connection_t *vc = view_connection_of_obj(obj);
        return SIM_make_attr_object(vc->parent);
}

static set_error_t
set_parent(void *param, conf_object_t *obj, attr_value_t *val,
           attr_value_t *idx)
{
        view_connection_t *vc = view_connection_of_obj(obj);
        vc->parent = SIM_attr_object(*val);
        return Sim_Set_Ok;
}

static set_error_t
set_clear(void *param, conf_object_t *obj, attr_value_t *val,
          attr_value_t *idx)
{
        view_connection_t *vc = view_connection_of_obj(obj);
        if (vc->clear_histogram)
                vc->clear_histogram(vc);
        return Sim_Set_Ok;        
}

// instrumentation_connection::enable()
static void
ic_enable(conf_object_t *obj)
{
        view_connection_t *vc = view_connection_of_obj(obj);
        vc->cis_iface->enable_connection_callbacks(vc->cpu, obj);
}

// instrumentation_connection::disable()
static void
ic_disable(conf_object_t *obj)
{
        view_connection_t *vc = view_connection_of_obj(obj);
        vc->cis_iface->disable_connection_callbacks(vc->cpu, obj);
}

void
add_generic_view_attributes(conf_class_t *cl)
{
        SIM_register_typed_attribute(
                cl, "histogram",
                get_histogram, NULL,
                NULL, NULL,
                Sim_Attr_Pseudo,
                "[[s|ii]*]", NULL,
                "((instruction, counter)*)* instruction histogram");

        SIM_register_typed_attribute(
                cl, "cpu",
                get_cpu, NULL,
                set_cpu, NULL,
                Sim_Attr_Pseudo,
                "o", NULL,
                "the processor being monitored");
        
        SIM_register_typed_attribute(
                cl, "clear",
                NULL, NULL,
                set_clear, NULL,
                Sim_Attr_Pseudo,
                "b", NULL,
                "When this attribute is set, the current histogram statistics"
                " is removed.");

        SIM_register_typed_attribute(
                cl, "parent",
                get_parent, NULL,
                set_parent, NULL,
                Sim_Attr_Pseudo,
                "o", NULL,
                "Parent object for this connection.");
        
        static const instrumentation_connection_interface_t ic_iface = {
                .enable = ic_enable,
                .disable = ic_disable
        };
        SIM_REGISTER_INTERFACE(cl, instrumentation_connection, &ic_iface);
}

void
init_generic_view()
{
        init_mnemonic_view();
        init_size_view();
        init_x86_normalized_view();
        init_xed_iform_view();
}
