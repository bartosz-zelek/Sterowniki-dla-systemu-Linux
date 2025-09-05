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

#include "connection.h"
#include "instrumentation-tracer-tool.h"

#include <simics/device-api.h>

conf_class_t *connection_class;

static const char *segstr[] = { "es", "cs", "ss", "ds", "fs", "gs" };

static const char *
get_access_type_str(connection_t *con, int at)
{
        if (!con->x86_at_iface)
                return "N/A";
        const char *str = con->x86_at_iface->get_short_name(con->cpu, at);
        if (!str)
                return "N/A";
        return str;
}

static const char *
get_memory_type_str(int mt)
{
        switch (mt) {
        case X86_Strong_Uncacheable:    /* UC */
        case X86_Uncacheable:           /* UC- */
                return "UC";
        case X86_Write_Combining:
                return "WC";
        case X86_Write_Through:
                return "WT";
        case X86_Write_Back:
                return "WB";
        case X86_Write_Protected:
                return "WP";
        default:
                return "er";
        }
}

static void
trace_a_changed_register(connection_t *con, strbuf_t *out,
                         reg_state_t *ptr, uint64 new_value)
{
        const char *reg_name =
                con->ir_iface->get_name(con->cpu, ptr->reg_id);

        sb_addfmt(out, "  %s <- 0x%llx", reg_name, new_value);
        if (con->print_old_value)
                sb_addfmt(out, " (0x%llx)", ptr->last_value);
        sb_addstr(out, "\n");

        if (con->tracer->fh) {
                sb_write(out, con->tracer->fh);
        } else {
                SIM_printf("%s", sb_str(out));
        }
}

static void
setup_proc_for_changed_regs(connection_t *con, strbuf_t *proc,
                            unsigned long long cnt)
{
        if (con->use_cpu_number)
                sb_addfmt(proc, " reg: [%9llu] CPU  %u",
                          cnt, con->id);
        else
                sb_addfmt(proc, " reg: [%9llu] %s",
                          cnt, con->short_name);
}

static void
register_changes_forall(connection_t *con, unsigned long long cnt)
{
        strbuf_t proc = SB_INIT;
        strbuf_t out = SB_INIT;

        for (unsigned i = 0; i < con->nr_regs; ++i) {
                reg_state_t *ptr = &con->regs[i];
                uint64 v = con->ir_iface->read(con->cpu, ptr->reg_id);
                if (v == ptr->last_value)
                        continue;

                if (!sb_len(&proc)) {
                        setup_proc_for_changed_regs(con, &proc, cnt);
                }

                sb_clear(&out);
                sb_extend(&out, 40);
                sb_copy(&out, &proc);

                trace_a_changed_register(con, &out, ptr, v);

                ptr->last_value = v;
        }
        sb_free(&out);
        sb_free(&proc);
}

static void
raised_register_changes_forall(conf_object_t *obj, conf_object_t *cpu,
                               exception_handle_t *handle, void *cb_user_data)
{
        connection_t *con = obj_to_con(obj);
        unsigned long long cnt;
        if (con->in_instruction) {
                cnt = con->instr_cnt;
                con->in_instruction = false;
        } else if (con->trace_exceptions) {
                cnt = con->exc_cnt;
        } else {
                // Skip register changes for exceptions
                // if exceptions are not traced.
                return;
        }
        register_changes_forall(con, cnt);
}

static void
raised_forall(conf_object_t *obj, conf_object_t *cpu,
              exception_handle_t *handle, void *cb_user_data)
{
        connection_t *con = obj_to_con(obj);
        int exc_num;
        if (con->x86_eq_iface) {
                /* x86 specific */
                uint8 vector = con->x86_eq_iface->vector(cpu, handle);
                x86_exception_source_t source = con->x86_eq_iface->source(
                        cpu, handle);
                
                exc_num = vector | (source << 8);
        } else {
                /* Generic */
                exc_num = con->eq_iface->exception_number(cpu, handle);
        }
        const char *str = con->ex_iface->get_name(cpu, exc_num);        
        con->exc_cnt++;
        strbuf_t proc = SB_INIT;
        if (con->use_cpu_number)
                sb_fmt(&proc, "CPU  %u", con->id);
        else
                sb_fmt(&proc, "%s", con->short_name);

        strbuf_t out = SB_INIT;
        sb_addfmt(&out, "exce: [%9llu] %s exception %d (%s)\n",
                  con->exc_cnt,
                  sb_str(&proc),
                  exc_num,
                  str);
        
        if (con->tracer->fh) {
                sb_write(&out, con->tracer->fh);
        } else {
                SIM_printf("%s", sb_str(&out));
        }
        sb_free(&proc);
        sb_free(&out);
}

static void
instruction_register_changes_forall(conf_object_t *obj, conf_object_t *cpu,
                                    instruction_handle_t *instr,
                                    void *connection)
{
        connection_t *con = obj_to_con(obj);
        register_changes_forall(con, con->instr_cnt);
        con->in_instruction = false;
}

static void
instruction_forall(conf_object_t *obj, conf_object_t *cpu,
                   instruction_handle_t *instr,
                   void *connection)
{
        connection_t *con = obj_to_con(obj);
        const cpu_instruction_query_interface_t *iq = con->iq_iface;
        tracer_tool_t *tt = con->tracer;
        uint64 la = iq->logical_address(cpu, instr);
        uint64 pa = iq->physical_address(cpu, instr);
        cpu_bytes_t opcode = iq->get_instruction_bytes(cpu, instr);
        strbuf_t sb = SB_INIT;

        con->instr_cnt++;
        
        if (con->print_linear_address && con->x86_iq_iface) {
                uint64 lina = con->x86_iq_iface->linear_address(cpu, instr);
                sb_addfmt(&sb, " <l:0x%0*llx>", con->va_digits, lina);
        }
        if (con->print_virtual_address) {               
                sb_addfmt(&sb, " <%s:0x%0*llx>",
                          con->x86_iq_iface ? "cs" : "v",
                          con->va_digits, la);
        }
        if (con->print_physical_address)
                sb_addfmt(&sb, " <p:0x%0*llx>", con->pa_digits, pa);

        sb_addfmt(&sb, " ");
        if (con->print_opcode) {
                for (uint8 i = 0; i < opcode.size; i++)
                        sb_addfmt(&sb, "%02x ", opcode.data[i]);
                if (opcode.size < 6) {
                        for (uint8 i = opcode.size; i < 6; i++)
                                sb_addfmt(&sb, "   ");
                }
        }

        attr_value_t val = SIM_make_attr_data(opcode.size, opcode.data);
        tuple_int_string_t t = con->pi_iface->disassemble(cpu, la, val, 0);
        SIM_attr_free(&val);
        if (t.integer > 0)
                sb_addfmt(&sb, "%s\n", t.string);
        else
                sb_addfmt(&sb, "illegal instruction\n");

        MM_FREE(t.string);
        
        if (!con->remove_duplicates
            || strcmp(sb_str(&con->last_line), sb_str(&sb)) != 0) {
                strbuf_t proc = SB_INIT;
                if (con->use_cpu_number)
                        sb_fmt(&proc, "CPU  %u", con->id);
                else
                        sb_fmt(&proc, "%s", con->short_name);
                strbuf_t out = SB_INIT;

                sb_addfmt(&out, "inst: [%9llu] %s%s",
                          con->instr_cnt,
                          sb_str(&proc),
                          sb_str(&sb));
        
                if (tt->fh) {
                        sb_write(&out, tt->fh);
                } else {
                        SIM_printf("%s", sb_str(&out));
                }
                sb_free(&proc);
                sb_free(&out);
        }
        sb_free(&con->last_line);
        con->last_line = sb;
        con->in_instruction = true;
}

static void
mem_forall(conf_object_t *obj, conf_object_t *cpu,
           memory_handle_t *mem, const char *rw,
           void *connection)          
{
        connection_t *con = obj_to_con(obj);
        tracer_tool_t *tt = con->tracer;
        const cpu_memory_query_interface_t *mq = con->mq_iface;
        
        uint64 la = mq->logical_address(cpu, mem);
        uint64 pa = mq->physical_address(cpu, mem);
        cpu_bytes_t bytes = mq->get_bytes(cpu, mem);
        bool atomic = mq->atomic(cpu, mem);

        con->data_cnt++;
                
        strbuf_t sb = SB_INIT;
        if (con->print_linear_address && con->x86_mq_iface) {
                uint64 lina = con->x86_mq_iface->linear_address(cpu, mem);
                sb_addfmt(&sb, " <l:0x%0*llx>", con->va_digits, lina);
        }
        if (con->print_virtual_address) {
                if (con->x86_mq_iface) {
                        int seg = con->x86_mq_iface->segment(cpu, mem);
                        if (seg == -1) {
                                sb_addfmt(&sb, " <linear access>");
                        } else {
                                sb_addfmt(&sb, " <%s:0x%0*llx>",
                                          segstr[seg],
                                          con->va_digits, la);
                        }
                } else {
                        sb_addfmt(&sb, " <v:0x%0*llx>", con->va_digits, la);
                }
        }
        if (con->print_physical_address)
                sb_addfmt(&sb, " <p:0x%0*llx>", con->pa_digits, pa);
        if (con->print_access_type && con->x86_mq_iface) {
                int at = con->x86_mq_iface->access_type(cpu, mem);
                sb_addfmt(&sb, " %-4s%s",
                          get_access_type_str(con, at),
                          atomic ? " atomic" : "");
        }
        if (con->print_memory_type && con->x86_mq_iface) {
                int mt = con->x86_mq_iface->memory_type(cpu, mem);
                sb_addfmt(&sb, " %s", get_memory_type_str(mt));
        }
        sb_addfmt(&sb, " %s %2zu bytes  ", rw, bytes.size);

        if (bytes.size <= 8) {
                uint64 val = 0;
                for (int i = bytes.size - 1; i >= 0; i--)
                        val = val << 8 | bytes.data[i];
                sb_addfmt(&sb, "0x%llx", val);
        } else {
                for (int i = bytes.size - 1; i >= 0; i--)
                        sb_addfmt(&sb, "%02x", bytes.data[i]);
        }
        sb_addfmt(&sb, "\n");

        if (!con->remove_duplicates
            || strcmp(sb_str(&con->last_line), sb_str(&sb)) != 0) {
                strbuf_t proc = SB_INIT;
                if (con->use_cpu_number)
                        sb_fmt(&proc, "CPU  %u", con->id);
                else
                        sb_fmt(&proc, "%s", con->short_name);

                strbuf_t out = SB_INIT;
                sb_addfmt(&out, "data: [%9llu] %s%s",
                          con->data_cnt,
                          sb_str(&proc),
                          sb_str(&sb));

                if (tt->fh) {
                        sb_write(&out, tt->fh);
                } else {
                        SIM_printf("%s", sb_str(&out));
                }
                sb_free(&proc);
                sb_free(&out);
        }
        sb_free(&con->last_line);
        con->last_line = sb;
}

static void
read_forall(conf_object_t *obj, conf_object_t *cpu,
            memory_handle_t *mem, void *connection_user_data)
{
        mem_forall(obj, cpu, mem, "Read ", connection_user_data);
}

static void
write_forall(conf_object_t *obj, conf_object_t *cpu,
             memory_handle_t *mem, void *connection_user_data)
{
        mem_forall(obj, cpu, mem, "Write", connection_user_data);
}

static void
setup_int_registers(connection_t *con)
{
        const processor_cli_interface_t *pcli_iface =
                SIM_c_get_interface(con->cpu, PROCESSOR_CLI_INTERFACE);

        if (pcli_iface) {
                attr_value_t diff_regs = pcli_iface->get_diff_regs(con->cpu);
                unsigned nr_regs = SIM_attr_list_size(diff_regs);

                con->regs = MM_ZALLOC(nr_regs, reg_state_t);
                con->nr_regs = nr_regs;

                for (unsigned i = 0; i < nr_regs; ++i) {

                        const char *rname =
                                SIM_attr_string(SIM_attr_list_item(diff_regs, i));
                        if (rname == 0)
                                continue;

                        int reg_id = con->ir_iface->get_number(con->cpu, rname);
                        if (reg_id == -1)
                                continue;

                        uint64 value =
                                con->ir_iface->read(con->cpu, reg_id);

                        con->regs[i].last_value = value;
                        con->regs[i].reg_id = reg_id;
                }
                SIM_attr_free(&diff_regs);
        }
}

void
configure_connection(connection_t *con)
{
        /* Register a callback that will be called for each instruction */
        bool reg_change_registered = false;
        if (con->trace_instructions) {
                con->ci_iface->register_instruction_before_cb(
                        con->cpu, &con->obj, instruction_forall, NULL);
                if (con->print_register_changes) {
                        con->ci_iface->register_instruction_after_cb(
                                con->cpu, &con->obj,
                                instruction_register_changes_forall, NULL);
                        con->ci_iface->register_exception_after_cb(
                                con->cpu, &con->obj, CPU_Exception_All,
                                raised_register_changes_forall, NULL);
                        reg_change_registered = true;
                }
        }
        if (con->trace_data) {
                con->ci_iface->register_read_after_cb(
                        con->cpu, &con->obj, CPU_Access_Scope_Explicit,
                        read_forall, NULL);
                con->ci_iface->register_read_after_cb(
                        con->cpu, &con->obj, CPU_Access_Scope_Implicit,
                        read_forall, NULL);
                con->ci_iface->register_write_after_cb(
                        con->cpu, &con->obj, CPU_Access_Scope_Explicit,
                        write_forall, NULL);
                con->ci_iface->register_write_after_cb(
                        con->cpu, &con->obj, CPU_Access_Scope_Implicit,
                        write_forall, NULL);
        }
        if (con->trace_exceptions) {
                con->ci_iface->register_exception_before_cb(
                        con->cpu, &con->obj, CPU_Exception_All, raised_forall,
                        NULL);
                if (con->print_register_changes && !reg_change_registered) {
                        con->ci_iface->register_exception_after_cb(
                                con->cpu, &con->obj, CPU_Exception_All,
                                raised_register_changes_forall, NULL);
                        reg_change_registered = true;
                }
        }

        if (reg_change_registered)
                setup_int_registers(con);
}

static conf_object_t *
ic_alloc(void *arg)
{
        connection_t *con = MM_ZALLOC(1, connection_t);
        return &con->obj;
}

// instrumentation_connection::pre_delete_instance
static void
ic_pre_delete_connection(conf_object_t *obj)
{
        connection_t *con = obj_to_con(obj);
        con->ci_iface->remove_connection_callbacks(con->cpu, &con->obj);
}

static int
ic_delete_connection(conf_object_t *obj)
{
        connection_t *con = obj_to_con(obj);
        if (con->nr_regs)
                MM_FREE(con->regs);
        MM_FREE(con->short_name);
        MM_FREE(obj);
        return 0;
}

// instrumentation_connection::enable
static void
ic_enable(conf_object_t *obj)
{
        connection_t *c = obj_to_con(obj);
        c->ci_iface->enable_connection_callbacks(c->cpu, obj);
}

static void
ic_disable(conf_object_t *obj)
{
        connection_t *c = obj_to_con(obj);
        c->ci_iface->disable_connection_callbacks(c->cpu, obj);
}

#define MAKE_ATTR_SET_GET_BOOL(option, desc)                            \
static set_error_t                                                      \
set_ ## option (void *arg, conf_object_t *obj, attr_value_t *val,       \
              attr_value_t *idx)                                        \
{                                                                       \
        connection_t *con = obj_to_con(obj);                            \
        con-> option = SIM_attr_boolean(*val);                          \
        return Sim_Set_Ok;                                              \
}                                                                       \
                                                                        \
static attr_value_t                                                     \
get_ ## option (void *arg, conf_object_t *obj, attr_value_t *idx)       \
{                                                                       \
        connection_t *con = obj_to_con(obj);                            \
        return SIM_make_attr_boolean(con-> option );                    \
}

FOR_OPTIONS(MAKE_ATTR_SET_GET_BOOL);

#define ATTR_REGISTER(option, desc)                     \
        SIM_register_typed_attribute(                   \
                connection_class, #option,              \
                get_ ## option, NULL,                   \
                set_ ## option, NULL,                   \
                Sim_Attr_Optional | Sim_Attr_Read_Only, \
                "b", NULL,                              \
                "Enables " desc ". Default off.");

static set_error_t
set_short_name(conf_object_t *obj, attr_value_t *val)
{
        connection_t *con = obj_to_con(obj);
        con->short_name = MM_STRDUP(SIM_attr_string(*val));
        return Sim_Set_Ok;
}

static attr_value_t
get_short_name(conf_object_t *obj)
{
        connection_t *con = obj_to_con(obj);
        return SIM_make_attr_string(con->short_name);
}

void
init_connection()
{
        const class_data_t ic_funcs = {
                .alloc_object = ic_alloc,
                .description = "Instrumentation connection.",
                .class_desc = "instrumentation connection",
                .pre_delete_instance = ic_pre_delete_connection,
                .delete_instance = ic_delete_connection,
                .kind = Sim_Class_Kind_Pseudo
        };
        connection_class = SIM_register_class("tracer_tool_connection",
                                              &ic_funcs);
        
        static const instrumentation_connection_interface_t ic_iface = {
                .enable = ic_enable,
                .disable = ic_disable,
        };
        SIM_REGISTER_INTERFACE(connection_class,
                               instrumentation_connection, &ic_iface);
        
        FOR_OPTIONS(ATTR_REGISTER);

        SIM_register_attribute(connection_class, "short_name",
                               get_short_name, set_short_name,
                               Sim_Attr_Pseudo,
                               "s", "Use this short name for the processor.");
}
