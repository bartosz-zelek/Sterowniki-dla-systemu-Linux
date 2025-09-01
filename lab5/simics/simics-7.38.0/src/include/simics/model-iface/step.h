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

#ifndef SIMICS_MODEL_IFACE_STEP_H
#define SIMICS_MODEL_IFACE_STEP_H

#include <simics/base/types.h>
#include <simics/base/attr-value.h>
#include <simics/base/event.h>
#include <simics/processor/time.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/*
  <add id="step_interface_t">

  The <iface>step</iface> interface is typically implemented by
  processors, but can be implemented by other objects as well. Its
  purpose is to handle step events using a queue.

  The current number of steps for the <param>queue</param> is returned
  when calling <fun>get_step_count</fun>.

  The <fun>post_step</fun> function will schedule an event that will
  occur after <arg>steps</arg> (which must be nonnegative)
  counted from local current step at
  <arg>queue</arg>. An event previously posted can be removed by
  calling <fun>cancel_step</fun>. The <fun>cancel_step</fun> function takes a
  function <arg>pred</arg> as argument which is called when a matching
  event is found. The event is only removed if <arg>pred</arg> returns
  <tt>1</tt>. The <fun>find_next_step</fun> takes the same arguments
  as <fun>cancel_step</fun> but only returns the number of cycles before
  the event will occur. The <arg>evclass</arg> is the event class,
  <arg>obj</arg> is the object posting the event, and
  <arg>user_data</arg> is pointer to data used as a parameter when
  calling the callback function defined in the <arg>evclass</arg>.
  If no matching event was found, <fun>find_next_step</fun> returns
  <math>-1</math>.

  The <fun>events</fun> method returns a list of all pending events in
  expiration order. Each element is a four-element list containing the event
  object, the event class name, the expiration time counted in steps as an
  integer and the event description as given by the event class
  <fun>describe</fun> method, or <em>nil</em> for events whose event class do
  not define that method.

  The <fun>advance</fun> function will increment the number of steps
  for the queue, decrementing the number of steps to the first event
  to the value defined by <arg>steps</arg>. The number of steps remaining
  to the next event is returned. It is an error to advance beyond the
  next pending event, so the return value is never negative.

  The implementor of the <iface>step</iface> interface can use any
  checkpoint representation. The <param>name</param> field in the
  event class data structure is unique, and the attribute setter
  function for checkpoint restore can use
  <fun>SIM_get_event_class</fun> to get the event class structure
  corresponding to an event class name.

  <insert-until text="// ADD INTERFACE step_interface_t"/>
  </add>

   <add id="step_interface_exec_context">
   Cell Context for all methods.
   </add>
*/

SIM_INTERFACE(step) {
        pc_step_t (*get_step_count)(conf_object_t *NOTNULL queue);
        void (*post_step)(
                conf_object_t *NOTNULL queue,
                event_class_t *NOTNULL evclass,
                conf_object_t *NOTNULL obj,
                pc_step_t steps,
                lang_void *user_data);
        void (*cancel_step)(
                conf_object_t *NOTNULL queue,
                event_class_t *NOTNULL evclass,
                conf_object_t *NOTNULL obj,
                int (*pred)(lang_void *data, lang_void *match_data),
                lang_void *match_data);
        pc_step_t (*find_next_step)(
                conf_object_t *NOTNULL queue,
                event_class_t *NOTNULL evclass,
                conf_object_t *NOTNULL obj,
                int (*pred)(lang_void *data, lang_void *match_data),
                lang_void *match_data);

        attr_value_t (*events)(conf_object_t *NOTNULL obj);

        pc_step_t (*advance)(conf_object_t *queue, pc_step_t steps);
};

#define STEP_INTERFACE "step"
// ADD INTERFACE step_interface_t

/*
  <add id="step_cycle_ratio_interface_t">

  The <iface>step_cycle_ratio</iface> interface is implemented by
  processors that support a changeable ratio between steps and
  cycles. The <cmd>set-step-rate</cmd> command uses this interface to
  set the ratio between steps and cycles.

  The <fun>set_ratio</fun> sets the ratio between <arg>steps</arg> and
  <arg>cycles</arg>. Note that the introduction of stall cycles can
  skew the ratio. The <fun>get_ratio</fun> simply returns the current
  ratio.

  The <arg>cycles</arg> and <arg>step</arg> arguments must be in the range
  [1..128] and <arg>cycles</arg> must be a power of two. Implementers of this
  interface may choose to ignore other values of <arg>cycles</arg> and
  <arg>step</arg> and may log an error.

  <insert-until text="// ADD INTERFACE step_cycle_ratio_interface_t"/>
  </add>
  <add id="step_cycle_ratio_interface_exec_context">
  Cell Context for all methods.
  </add>
*/

typedef struct {
        uint32 steps;
        uint32 cycles;
} step_cycle_ratio_t;

SIM_INTERFACE(step_cycle_ratio) {
        step_cycle_ratio_t (*get_ratio)(conf_object_t *obj);
        void (*set_ratio)(conf_object_t *obj, uint32 steps, uint32 cycles);
};

#define STEP_CYCLE_RATIO_INTERFACE "step_cycle_ratio"
// ADD INTERFACE step_cycle_ratio_interface_t

/*
  <add id="stall_interface_t">

  The <iface>stall</iface> interface can be implemented by objects that also
  implement the <iface>cycle</iface> and <iface>step</iface> interfaces. The
  <iface>stall</iface> interface controls the addition of extra cycles between
  steps.

  The <fun>get_stall_cycles</fun> function returns the remaining number of
  stall cycles. The object will advance that number of cycles before starting
  with the next step.

  The <fun>set_stall_cycles</fun> function is used to change the number of
  stall cycles before the next step. It is legal to first call this function
  with a large value for <arg>cycles</arg> and then at a later point reduce the
  cycle count is resume execution earlier than indicated by the first call.

  The <fun>get_total_stall_cycles</fun> returns the total accumulated number of
  stall cycles.

  <insert-until text="// ADD INTERFACE stall_interface_t"/>
  </add>
  <add id="stall_interface_exec_context">
  Cell Context for all methods.
  </add>
*/
SIM_INTERFACE(stall) {
        cycles_t (*get_stall_cycles)(conf_object_t *obj);
        void (*set_stall_cycles)(conf_object_t *obj, cycles_t cycles);
        cycles_t (*get_total_stall_cycles)(conf_object_t *obj);
};
#define STALL_INTERFACE "stall"
// ADD INTERFACE stall_interface_t

/*
   <add id="step_info_interface_t">

   The <iface>step_info</iface> interface can be implemented by
   processors that optimize the execution by advancing the step count
   using special instructions or processor modes.

   The <fun>get_halt_steps</fun> and <fun>set_halt_steps</fun>
   functions are used to get and set the number of steps that have been
   advanced using special features in the architecture. Examples; X86
   processor it is the number of halt instructions executed, PPC
   processors it is the number of steps spent in sleep mode, ARM
   processors it is the number of steps spent in the "wait for
   interrupt" state.

   The <fun>get_ffwd</fun> and <fun>set_ffwd_steps</fun> functions are
   used to get and set the number of steps that the processor have
   optimized the execution by advancing time that is not
   architectural. This can for instance be execution loops that does
   not affect the processor state.

   <insert-until text="// ADD INTERFACE step_info_interface_t"/>

   </add>
   <add id="step_info_interface_exec_context">
   Cell Context for all methods.
   </add>
*/
SIM_INTERFACE(step_info) {
        pc_step_t (*get_halt_steps)(conf_object_t *obj);
        void (*set_halt_steps)(conf_object_t *obj, pc_step_t steps);
        pc_step_t (*get_ffwd_steps)(conf_object_t *obj);
        void (*set_ffwd_steps)(conf_object_t *obj, pc_step_t steps);
        pc_step_t (*get_ma_steps)(conf_object_t *obj);
        void (*set_ma_steps)(conf_object_t *obj, pc_step_t steps);
};
#define STEP_INFO_INTERFACE "step_info"
// ADD INTERFACE step_info_interface_t

#if defined(__cplusplus)
}
#endif

#endif
