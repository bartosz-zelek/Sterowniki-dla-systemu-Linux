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

#ifndef CONNECTION_H
#define CONNECTION_H

#include <simics/model-iface/cpu-instrumentation.h>
#include <simics/model-iface/exception.h>
#include <simics/model-iface/processor-info.h>
#include <simics/model-iface/int-register.h>

#include <simics/arch/x86-instrumentation.h>

#define FOR_OPTIONS(op)                                                 \
        op(trace_data,            "tracing of data")                    \
        op(trace_instructions,    "tracing of instructions")            \
        op(trace_exceptions,      "tracing of exceptions")              \
        op(print_register_changes,"printing of changed registers")      \
        op(print_old_value,       "printing of previous register value")\
        op(remove_duplicates,     "removal of duplicated lines")        \
        op(print_virtual_address, "printing of virtual addresses")      \
        op(print_physical_address,"printing of physical addresses")     \
        op(print_linear_address,  "printing of linear addresses")       \
        op(print_opcode,          "printing of opcode")                 \
        op(print_access_type,     "printing of access types")           \
        op(print_memory_type,     "printing of memory types")           \
        op(use_cpu_number,        "use of cpu number instead of name")  \

#define DECLARE(option, desc)                        \
        bool option;

#if defined(__cplusplus)
extern "C" {
#endif

struct tracer_tool_t;

typedef struct {
        uint64 last_value;
        int    reg_id;
} reg_state_t;

typedef struct {
        conf_object_t obj;
        conf_object_t *cpu;
        char *short_name;
        struct tracer_tool *tracer;

        int pa_digits;
        int va_digits;
        unsigned id;
        const processor_info_interface_t *pi_iface;
        const cpu_instrumentation_subscribe_interface_t *ci_iface;
        const cpu_instruction_query_interface_t *iq_iface;
        const cpu_memory_query_interface_t *mq_iface;
        const cpu_exception_query_interface_t *eq_iface;
        const x86_instruction_query_interface_t *x86_iq_iface;
        const x86_memory_query_interface_t *x86_mq_iface;
        const x86_exception_query_interface_t *x86_eq_iface;
        const x86_access_type_interface_t *x86_at_iface;
        const exception_interface_t *ex_iface;
        const int_register_interface_t *ir_iface;

        FOR_OPTIONS(DECLARE)

        strbuf_t last_line;
        uint64 data_cnt;
        uint64 instr_cnt;
        uint64 exc_cnt;

        reg_state_t *regs;
        unsigned nr_regs;
        bool in_instruction;
} connection_t;

FORCE_INLINE connection_t *
obj_to_con(conf_object_t *obj)
{
        return (connection_t *)obj;
}

void init_connection();
void configure_connection(connection_t *con);

extern conf_class_t *connection_class;

#if defined(__cplusplus)
}
#endif
#endif        
