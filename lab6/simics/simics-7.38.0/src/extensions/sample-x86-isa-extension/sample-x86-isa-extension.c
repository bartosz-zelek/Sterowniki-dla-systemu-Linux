/*
  © 2019 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <simics/simulator/conf-object.h>
#include <simics/simulator-iface/instrumentation-tool.h>
#include <simics/model-iface/cpu-instrumentation.h>
#include <simics/device-api.h>
#include <simics/arch/x86-instrumentation.h>
#include <simics/arch/x86.h>

typedef struct {
        conf_object_t obj;
        conf_object_t *processor;

        /* Cached interfaces */
        const x86_reg_access_interface_t *x86_reg_access_iface;

        uint64 my_register;
} sample_x86_isa_extension_t;

FORCE_INLINE sample_x86_isa_extension_t *
tool_of_obj(conf_object_t *obj) { return (sample_x86_isa_extension_t *)obj; }

static conf_object_t *
it_alloc(void *arg)
{
        sample_x86_isa_extension_t *tool = MM_ZALLOC(1, sample_x86_isa_extension_t);
        return &tool->obj;
}

static int
it_delete_connection(conf_object_t *obj)
{
        sample_x86_isa_extension_t *tool = tool_of_obj(obj);
        MM_FREE(tool);
        return 0;
}

static cpu_emulation_t
replaced_instruction(conf_object_t *obj, conf_object_t *cpu, void *user_data)
{
        sample_x86_isa_extension_t *tool = tool_of_obj(obj);
        tool->my_register = tool->x86_reg_access_iface->get_gpr(tool->processor,
                                                                0);
        return CPU_Emulation_Fall_Through;
}

static cpu_emulation_t
new_instruction(conf_object_t *obj, conf_object_t *cpu, void *user_data)
{
        sample_x86_isa_extension_t *tool = tool_of_obj(obj);
        tool->x86_reg_access_iface->set_gpr(tool->processor, 0,
                                            tool->my_register);
        return CPU_Emulation_Fall_Through;
}

static int
replace_decoder(conf_object_t *obj, conf_object_t *cpu,
                decoder_handle_t *handle, instruction_handle_t *iq_handle,
                void *cb_data)
{
        const cpu_instruction_decoder_interface_t *id_iface =
                SIM_C_GET_INTERFACE(cpu, cpu_instruction_decoder);
        ASSERT(id_iface);
        const cpu_instruction_query_interface_t *iq_iface =
                SIM_C_GET_INTERFACE(cpu, cpu_instruction_query);
        ASSERT(iq_iface);

        cpu_bytes_t bytes = iq_iface->get_instruction_bytes(cpu, iq_handle);

        if (bytes.size == 0)
                return -1;

        if (bytes.data[0] == 0xF3 /* REP prefix */) {
                if (bytes.size < 2)
                        return -2;

                if (bytes.data[1] == 0xF8 /* CLC instruction */) {
                        id_iface->register_emulation_cb(
                                cpu, replaced_instruction, handle, NULL, NULL);
                        return 2;
                }
        }

        return 0;
}

static int
new_decoder(conf_object_t *obj, conf_object_t *cpu, decoder_handle_t *handle,
            instruction_handle_t *iq_handle, void *cb_data)
{
        const cpu_instruction_decoder_interface_t *id_iface =
                SIM_C_GET_INTERFACE(cpu, cpu_instruction_decoder);
        ASSERT(id_iface);
        const cpu_instruction_query_interface_t *iq_iface =
                SIM_C_GET_INTERFACE(cpu, cpu_instruction_query);
        ASSERT(iq_iface);

        cpu_bytes_t bytes = iq_iface->get_instruction_bytes(cpu, iq_handle);

        if (bytes.size == 0)
                return -1;

        if (bytes.data[0] == 0xF0 /* LOCK prefix */) {
                if (bytes.size < 2)
                        return -2;

                if (bytes.data[1] == 0xF8 /* CLC instruction */) {
                        id_iface->register_emulation_cb(cpu, new_instruction,
                                                        handle, NULL, NULL);
                        return 2;
                }
        }

        return 0;
}

static tuple_int_string_t
replace_disassemble(conf_object_t *obj, conf_object_t *cpu,
                    generic_address_t addr, cpu_bytes_t bytes)
{
        if (bytes.size < 1)
                return (tuple_int_string_t) { -1, NULL };

        if (bytes.data[0] == 0xF3 /* REP prefix */) {
                if (bytes.size < 2)
                        return (tuple_int_string_t) { -2, NULL };

                if (bytes.data[1] == 0xF8 /* CLC instruction */)
                        return (tuple_int_string_t)
                                { 2, MM_STRDUP("mov my_register, rax") };
        }
        return (tuple_int_string_t) { 0, NULL };
}

static tuple_int_string_t
new_disassemble(conf_object_t *obj, conf_object_t *cpu, generic_address_t addr,
                cpu_bytes_t bytes)
{
        if (bytes.size < 1)
                return (tuple_int_string_t) { -1, NULL };

        if (bytes.data[0] == 0xF0 /* LOCK prefix */) {
                if (bytes.size < 2)
                        return (tuple_int_string_t) { -2, NULL };

                if (bytes.data[1] == 0xF8 /* CLC instruction */)
                        return (tuple_int_string_t)
                                { 2, MM_STRDUP("mov rax, my_register") };
        }
        return (tuple_int_string_t) { 0, NULL };
}

static void
setup_connection(conf_object_t *obj, conf_object_t *provider)
{
        sample_x86_isa_extension_t *tool = tool_of_obj(obj);

        const x86_instrumentation_subscribe_interface_t *x86_iface =
                SIM_C_GET_INTERFACE(provider, x86_instrumentation_subscribe);
        if (!x86_iface) {
                SIM_LOG_ERROR(obj, 0, "%s does not implement"
                              " x86_instrumentation_subscribe interface",
                              SIM_object_name(provider));
                return;
        }

        const cpu_instrumentation_subscribe_interface_t *cpu_iface =
                SIM_C_GET_INTERFACE(provider, cpu_instrumentation_subscribe);
        if (!cpu_iface) {
                SIM_LOG_ERROR(obj, 0, "%s does not implement"
                              " cpu_instrumentation_subscribe interface",
                              SIM_object_name(provider));
                return;
        }

        /* Use 'LOCK + CLC' as new 'mov rax, my_register' instruction.
         * 'LOCK + CLC' is illegal instruction in current hardware, thus
         * illegal_instruction callback can be used. This type of callbacks
         * does not prevent execution in VMP mode.'
         *
         * NOTE: instruction_decoder callback may also be used here.
         * It is reasonable to use illegal_instruction callback if an
         * instruction being added is the only modification preventing
         * execution in VMP mode. */
        x86_iface->register_illegal_instruction_cb(provider, obj, new_decoder,
                                                   new_disassemble, NULL);

        /* Replace 'REP + CLC' by a new 'mov my_register, rax' instruction.
         * 'REP + CLC' is a valid encoding in current hardware, thus
         * instruction_decoder callback must be used. This type of callback
         * prevents execution in VMP mode to guarantee correctness. */
        cpu_iface->register_instruction_decoder_cb(provider, obj,
                                                   replace_decoder,
                                                   replace_disassemble, NULL);

        tool->processor = provider;

        /* Cache interfaces */
        tool->x86_reg_access_iface
                = SIM_C_GET_INTERFACE(provider, x86_reg_access);
}

static conf_object_t *
it_connect(conf_object_t *obj, conf_object_t *provider, attr_value_t args)
{
        sample_x86_isa_extension_t *tool = tool_of_obj(obj);
        if (tool->processor) {
                SIM_LOG_ERROR(obj, 0, "Only single connection is allowed.");
                return NULL;
        }

        setup_connection(obj, provider);
        return NULL;
}

static void
it_disconnect(conf_object_t *obj, conf_object_t *conn_obj)
{
        SIM_LOG_ERROR(obj, 0, "Disconnect is not allowed.");
}

static attr_value_t
get_processor(void *param, conf_object_t *obj, attr_value_t *idx)
{
        sample_x86_isa_extension_t *tool = tool_of_obj(obj);
        return SIM_make_attr_object(tool->processor);
}

static set_error_t
set_processor(void *param, conf_object_t *obj, attr_value_t *val,
               attr_value_t *idx)
{
        sample_x86_isa_extension_t *tool = tool_of_obj(obj);

        conf_object_t *provider = SIM_attr_object(*val);
        if (provider == tool->processor)
                return Sim_Set_Ok;

        if (tool->processor != NULL)
                return Sim_Set_Illegal_Value;

        setup_connection(obj, provider);
        return Sim_Set_Ok;
}

static attr_value_t
get_my_register(void *param, conf_object_t *obj, attr_value_t *idx)
{
        sample_x86_isa_extension_t *tool = tool_of_obj(obj);
        return SIM_make_attr_int64(tool->my_register);
}

static set_error_t
set_my_register(void *param, conf_object_t *obj, attr_value_t *val,
               attr_value_t *idx)
{
        sample_x86_isa_extension_t *tool = tool_of_obj(obj);
        tool->my_register = SIM_attr_integer(*val);
        return Sim_Set_Ok;
}

void
init_local()
{
        const class_data_t funcs = {
                .alloc_object = it_alloc,
                .delete_instance = it_delete_connection,
                .description = "Sample instruction set architecture extension"
                               " for x86",
                .class_desc = "sample instr set arch extension",
        };

        conf_class_t *cl = SIM_register_class("sample-x86-isa-extension",
                                              &funcs);

        static const instrumentation_tool_interface_t it_iface = {
                .connect = it_connect,
                .disconnect = it_disconnect
        };
        SIM_REGISTER_INTERFACE(cl, instrumentation_tool, &it_iface);        

        SIM_register_typed_attribute(
               cl, "processor",
               get_processor, NULL,
               set_processor, NULL,
               Sim_Attr_Optional,
               "o", NULL,
               "Connected processor.");

        SIM_register_typed_attribute(
               cl, "my_register",
               get_my_register, NULL,
               set_my_register, NULL,
               Sim_Attr_Optional,
               "i", NULL,
               "My new 64-bit wide register.");
}
