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

#ifndef SIMICS_MODEL_IFACE_BREAKPOINTS_H
#define SIMICS_MODEL_IFACE_BREAKPOINTS_H

#include <simics/base/breakpoints.h>
#include <simics/base/cbdata.h>
#include <simics/base/memory-transaction.h>
#include <simics/pywrap.h>
#include <simics/util/int128.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add id="breakpoint_interface_t">

   The breakpoint interface is implemented by any object that supports breaking
   on an address range.

   <insert-until text="// ADD INTERFACE breakpoint_interface"/>

   The <fun>insert_breakpoint</fun> function is called when adding a
   breakpoint on <arg>object</arg>. The <arg>handle</arg> identified the
   breakpoint and is to be used when triggering the breakpoint and when
   breakpoints are removed. The <arg>access</arg> parameter specifies the types
   of accesses that the breakpoint is set for. The breakpoint range is from
   <arg>start</arg> to <arg>end</arg> and includes both ends.

   The implementer of this interface should call <arg>caller</arg> through the
   <iface>breakpoint_trigger</iface> interface to trigger the breakpoint.

   <b>remove_breakpoint</b> should remove the breakpoint and further accesses
   to the address range should not trigger that breakpoint.

   This interface is only to be used by the Simics core. Other uses of
   breakpoints should go through the available breakpoint API calls such as
   <fun>SIM_breakpoint</fun>.
   </add>
   <add id="breakpoint_interface_exec_context">
   Cell Context for all methods.
   </add>
*/

SIM_INTERFACE(breakpoint) {
        void (*insert_breakpoint)(conf_object_t *object,
                                  conf_object_t *caller,
                                  breakpoint_handle_t handle,
                                  access_t access,
                                  generic_address_t start,
                                  generic_address_t end);
        void (*remove_breakpoint)(conf_object_t *object,
                                  breakpoint_handle_t handle);
        breakpoint_info_t (*get_breakpoint)(conf_object_t *obj,
                                            breakpoint_handle_t handle);
};

#define BREAKPOINT_INTERFACE "breakpoint"
// ADD INTERFACE breakpoint_interface

/* <add id="breakpoint_change_interface_t">

   The <iface>breakpoint_change</iface> interface is implemented by
   objects that wish to get notified when breakpoints are added,
   removed, or changed in some way.

   The object that wants to get notified of changes to breakpoints in
   a another object would use the <iface>simple_dispatcher</iface> in
   the breakpoint_change port of that other object. The other object
   will then lookup the <iface>breakpoint_change</iface> interface in
   the object to notify and use that when breakpoint updates occur.

   The breakpoint_change port is implemented by objects of the
   <class>cell</class> class, and a listener that registers on a cell with get
   notified on any breakpoint changes in that cell.

   <insert-until text="// ADD INTERFACE breakpoint_change_interface"/>
   </add>
   <add id="breakpoint_change_interface_exec_context">
   Cell Context for all methods.
   </add>
*/

SIM_INTERFACE(breakpoint_change) {
        void (*breakpoint_added)(conf_object_t *obj,
                                 conf_object_t *bp_obj,
                                 breakpoint_handle_t handle);
        void (*breakpoint_removed)(conf_object_t *obj,
                                   conf_object_t *bp_obj,
                                   breakpoint_handle_t handle);
};

#define BREAKPOINT_CHANGE_INTERFACE "breakpoint_change"
// ADD INTERFACE breakpoint_change_interface

/* <add id="breakpoint_query_v2_interface_t">

   Objects of the <class>context</class> and <class>memory-space</class>
   classes implement the <iface>breakpoint_query_v2</iface> interface. It is
   used by processors to check for breakpoints.

   Implementors of this interface should use
   <fun>SIM_register_compatible_interfaces</fun> after
   <fun>SIM_register_interface</fun> to also register earlier versions of the
   interface.

   The <fun>get_breakpoints</fun> function returns a set of
   breakpoints that intersect the range given in <arg>start</arg> and
   <arg>end</arg>, including both start and end in the range. Only
   breakpoints set on access types with bits set in
   <arg>read_write_execute</arg> will be returned.

   When information from <fun>get_breakpoints</fun> has been processed, the
   breakpoints array in the breakpoint_set_t structure should be freed with
   MM_FREE.

   <insert-until text="// ADD INTERFACE breakpoint_query_v2_interface"/>
   </add>
   <add id="breakpoint_query_v2_interface_exec_context">
   Cell Context for all methods.
   </add>
*/

SIM_INTERFACE(breakpoint_query_v2) {
        breakpoint_set_t (*get_breakpoints)(conf_object_t *obj,
                                            access_t read_write_execute,
                                            generic_address_t start,
                                            generic_address_t end);
};

#define BREAKPOINT_QUERY_V2_INTERFACE "breakpoint_query_v2"
// ADD INTERFACE breakpoint_query_v2_interface

/* <add id="breakpoint_trigger_interface_t">

   Objects implementing the <iface>breakpoint</iface> interface
   typically also implement the <iface>breakpoint_trigger</iface>
   interface. Processors call the <fun>trigger_breakpoint</fun>
   function to signal that a memory access intersects a
   breakpoint. The <fun>trigger_breakpoint</fun> function will raise
   the appropriate haps and the currently scheduled execute object may
   receive a <fun>stop</fun> call during the call to
   <fun>trigger_breakpoint</fun>.

   The <arg>obj</arg> argument is the object that holds the
   breakpoint, typically a memory space or a context
   object. <arg>initiator_obj</arg> is the source of the memory
   operation that triggers the breakpoint. The <arg>handle</arg>
   argument identifies the breakpoint to trigger. A value of
   <arg>BREAKPOINT_HANDLE_ANY</arg> can be passed as
   <arg>handle</arg>, in which case all the breakpoints that match the
   given <arg>address</arg>, <arg>size</arg>, and <arg>access</arg>
   will be triggered. The <arg>address</arg>, <arg>size</arg>, and
   <arg>access</arg> arguments specify information about the access
   that triggers the breakpoint. The <arg>data</arg> argument points
   to a buffer where the data for the access is kept.

   If a <fun>stop</fun> is received during a call to
   <fun>trigger_breakpoint</fun>, then it is recommended that any
   software visible actions carried out after the breakpoint are
   logged. That could for example be to notify the user if the entire
   instruction that triggers a breakpoint will complete, and that the
   instruction will then not be re-executed when the simulation
   restarts.

   <insert-until text="// ADD INTERFACE breakpoint_trigger_interface"/>
   </add>
   <add id="breakpoint_trigger_interface_exec_context">
   Cell Context for all methods.
   </add>
*/
SIM_INTERFACE(breakpoint_trigger) {
        void (*trigger_breakpoint)(conf_object_t *obj,
                                   conf_object_t *initiator_obj,
                                   breakpoint_handle_t handle,
                                   generic_address_t address,
                                   generic_address_t size,
                                   access_t access,
                                   uint8 *data);
};

#define BREAKPOINT_TRIGGER_INTERFACE "breakpoint_trigger"
// ADD INTERFACE breakpoint_trigger_interface

/* <add-type id="virtual_breakpoint_flags_t"><ndx>virtual_breakpoint_flags_t</ndx></add-type> */
typedef enum {
        Virtual_Breakpoint_Flag_Linear = 1
} virtual_breakpoint_flags_t;

/* <add-type id="virtual_data_bp_handle_t"><ndx>virtual_data_bp_handle_t</ndx></add-type> */
typedef struct virtual_data_bp_handle virtual_data_bp_handle_t;

/* <add id="virtual_data_breakpoint_interface_t">

     Add and remove virtual-address (and, on x86, linear-address) read and
     write breakpoints. On every read access that intersects a read
     breakpoint's interval, the registered callback function is called with the
     object that initiated the read, and the address and size of the read. (The
     interval includes both endpoints; <param>first</param> must be less than
     or equal to <param>last</param>.) Write breakpoints work exactly the same,
     except that the callback is given the actual value being written, not just
     its size.

     The callback is called before the read or write has taken place, but may
     not intervene. If one or more breakpoint callbacks stop the simulation,
     the current instruction is completed before the stop takes effect. If more
     than one breakpoint is triggered by the same read or write, the
     implementation may call their callbacks in any order.

     On x86, the <tt>Virtual_Breakpoint_Flag_Linear</tt> flag causes the
     breakpoint to use linear rather than virtual addresses. (Adding a
     breakpoint with unsupported flags is illegal.)

     <note>
       This interface is preliminary and may change without prior notice.
     </note>

     <insert id="virtual_breakpoint_flags_t"/>

     <insert-until text="// ADD INTERFACE virtual_data_breakpoint"/>
   </add>
   <add id="virtual_data_breakpoint_interface_exec_context">
     Cell Context for all methods.
   </add> */
SIM_INTERFACE(virtual_data_breakpoint) {
        virtual_data_bp_handle_t *NOTNULL (*add_read)(
                conf_object_t *NOTNULL obj,
                generic_address_t first, generic_address_t last,
                void (*NOTNULL callback)(
                        cbdata_call_t data, conf_object_t *NOTNULL initiator,
                        generic_address_t address, unsigned size),
                cbdata_register_t data, uint32 flags);
        virtual_data_bp_handle_t *NOTNULL (*add_write)(
                conf_object_t *NOTNULL obj,
                generic_address_t first, generic_address_t last,
                void (*NOTNULL callback)(
                        cbdata_call_t data, conf_object_t *NOTNULL initiator,
                        generic_address_t address, bytes_t value),
                cbdata_register_t data, uint32 flags);
        void (*remove)(conf_object_t *NOTNULL obj,
                       virtual_data_bp_handle_t *NOTNULL bp_handle);
};
#define VIRTUAL_DATA_BREAKPOINT_INTERFACE "virtual_data_breakpoint"
// ADD INTERFACE virtual_data_breakpoint
        
/* <add-type id="virtual_instr_bp_handle_t"><ndx>virtual_instr_bp_handle_t</ndx></add-type> */
typedef struct virtual_instr_bp_handle virtual_instr_bp_handle_t;

/* <add id="virtual_instruction_breakpoint_interface_t">

     Add and remove virtual-address (and, on x86, linear-address) instruction
     breakpoints. Every time the processor executes an instruction that
     intersects the breakpoint's interval, the callback function is called with
     the processor, and the address and size of the instruction. (The interval
     includes both endpoints; <param>first</param> must be less than or equal
     to <param>last</param>.)

     The callback is called before the instruction is executed. If one or more
     breakpoint callbacks stop the simulation, the stop takes effect before
     the instruction is run. (This means that once the simulation starts
     again, the same breakpoints will trigger immediately again. The callback
     can use <fun>VT_step_stamp</fun> to detect re-triggering.) If more than
     one breakpoint is triggered by the same instruction, the implementation
     may call their callbacks in any order.

     If the filter function is non-null and returns false, the callback is not
     called. The filter function is supplied with the instruction opcode (the
     raw bytes of the instruction) and a processor (which may not be the same
     processor that the breakpoint is set on, but is guaranteed to be of the
     same class). The filter may base its decision only on the opcode bytes and
     the string obtained by asking the processor to disassemble the
     instruction; this allows the implementation to cache the result and omit
     future calls to the filter function where the opcode and disassembly
     string would be the same.

     On x86, the <tt>Virtual_Breakpoint_Flag_Linear</tt> flag causes the
     breakpoint to use linear rather than virtual addresses. Calling with
     unsupported flags is illegal.

     <note>
       This interface is preliminary and may change without prior notice.
     </note>

     <insert id="virtual_breakpoint_flags_t"/>

     <insert-until text="// ADD INTERFACE virtual_instruction_breakpoint"/>
   </add>
   <add id="virtual_instruction_breakpoint_interface_exec_context">
     Cell Context for all methods.
   </add> */
SIM_INTERFACE(virtual_instruction_breakpoint) {
        virtual_instr_bp_handle_t *NOTNULL (*add)(
                conf_object_t *NOTNULL obj,
                generic_address_t first, generic_address_t last,
                bool (*filter)(cbdata_call_t filter_data,
                               conf_object_t *NOTNULL cpu, bytes_t opcode),
                cbdata_register_t filter_data,
                void (*NOTNULL callback)(
                        cbdata_call_t callback_data, conf_object_t *NOTNULL cpu,
                        generic_address_t address, unsigned size),
                cbdata_register_t callback_data, uint32 flags);
        void (*remove)(conf_object_t *NOTNULL obj,
                       virtual_instr_bp_handle_t *NOTNULL bp_handle);
};
#define VIRTUAL_INSTRUCTION_BREAKPOINT_INTERFACE \
        "virtual_instruction_breakpoint"
// ADD INTERFACE virtual_instruction_breakpoint

EXPORTED int128 VT_step_stamp(conf_object_t *NOTNULL step_obj);

#if defined(__cplusplus)
}
#endif

#endif
