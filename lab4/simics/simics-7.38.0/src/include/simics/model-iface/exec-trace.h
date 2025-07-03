/*
  © 2010 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_MODEL_IFACE_EXEC_TRACE_H
#define SIMICS_MODEL_IFACE_EXEC_TRACE_H

#include <simics/base/types.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add-type id="instruction_trace_callback_t def"></add-type> */
typedef void (*instruction_trace_callback_t)(lang_void *tracer_data,
                                             conf_object_t *cpu,
                                             linear_address_t la,
                                             logical_address_t va,
                                             physical_address_t pa,
                                             byte_string_t opcode);

/*
  <add id="exec_trace_interface_t">

  The <iface>exec_trace</iface> interface is implemented by processor models
  that support tracing. A trace listener registers itself with the
  <fun>register_tracer</fun> call. The <arg>tracer</arg> callback will be
  called by the processor model
  when each instruction is just about to be executed, passing the
  <arg>tracer_data</arg> as passed to the <fun>register_tracer</fun> function
  in addition to information about the instruction that is executed.
  Invoke <fun>unregister_tracer</fun> with the same two pointers to deregister
  the listener.

  <insert id="instruction_trace_callback_t def"/>

  The <arg>pa</arg> parameter to the callback will always be valid, but some
  CPU architectures may not support <arg>la</arg> or <arg>va</arg>. The
  <arg>la</arg> argument is typically only valid for x86 CPUs. Lastly, the
  opcode of the instruction is passed in <arg>opcode</arg>. The
  <arg>opcode</arg> is passed without endian conversion, meaning that byte X in
  <arg>opcode</arg> corresponds to the byte at <arg>pa</arg> + X.

  <insert-until text="// ADD INTERFACE exec_trace_interface_t"/>
  </add>
  <add id="exec_trace_interface_exec_context">
  Global Context for both methods.
  Cell Context for the callback.
  </add>
*/
SIM_INTERFACE(exec_trace) {
        void (*register_tracer)(conf_object_t *NOTNULL cpu_obj,
                                instruction_trace_callback_t tracer,
                                lang_void *tracer_data);
        void (*unregister_tracer)(conf_object_t *NOTNULL cpu_obj,
                                  instruction_trace_callback_t tracer,
                                  lang_void *tracer_data);
};

#define EXEC_TRACE_INTERFACE "exec_trace"
// ADD INTERFACE exec_trace_interface_t

#if defined(__cplusplus)
}
#endif

#endif
