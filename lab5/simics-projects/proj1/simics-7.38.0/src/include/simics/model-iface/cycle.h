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

#ifndef SIMICS_MODEL_IFACE_CYCLE_H
#define SIMICS_MODEL_IFACE_CYCLE_H

#include <simics/base/types.h>
#include <simics/base/attr-value.h>
#include <simics/base/event.h>
#include <simics/base/local-time.h>
#include <simics/base/time.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/*
  <add id="cycle_interface_t">

  <dl>

  <dt>Interface Methods</dt>

  <dd>The <iface>cycle</iface> interface is typically implemented by
  processors, but can be implemented by other objects as well. Its purpose is
  to handle events based on time. The cycle queue has a cycle as the smallest
  time unit. The cycle queue also has an associated frequency which makes it
  possible to define events based on seconds or picoseconds.

  The <fun>get_frequency</fun> function returns the frequency in Hertz for
  the <param>queue</param>. Most objects implementing <iface>cycle</iface> also
  have a notification mechanism for frequency changes through the
  <iface>simple_dispatcher</iface> interface in the <attr>cpu_frequency</attr>
  port. It is recommended that such a notification mechanism is used to get
  updates rather than polling with <fun>get_frequency</fun>.

  The current number of cycles executed by the <param>queue</param> is returned
  by <fun>get_cycle_count</fun>. Time elapsed since the queue was created is
  returned by <fun>get_time</fun> (in seconds) and <fun>get_time_in_ps</fun>
  (in picoseconds); this will be equal to the value returned
  by <fun>get_cycle_count</fun> divided by the value returned
  by <fun>get_frequency</fun> if the frequency has been constant since time
  zero.

  The <fun>cycles_delta</fun> function returns the highest number of cycles
  <param>obj</param> can run <em>before</em> it passes the absolute local time
  <param>when</param>, assuming no frequency change occurs in the
  meantime. Note that <fun>cycles_delta</fun> can raise an exception if
  <param>when</param> was too far ahead in the future. The
  <fun>cycles_delta_from_ps</fun> function performs the same function, for an
  absolute local time expressed in picoseconds.

  The <fun>post_cycle</fun> function will schedule an event that will occur
  after <param>cycles</param> counted from local current time at
  <param>queue</param>. The <fun>post_time</fun> function is similar but takes
  <param>seconds</param> as argument, while <fun>post_time_in_ps</fun> takes
  a number of <param>picoseconds</param>. The arguments
  <param>cycles</param>, <param>seconds</param> and
  <param>picoseconds</param> must be nonnegative.

  An event previously posted can be removed by calling <fun>cancel</fun>. The
  <fun>cancel</fun> function takes a function <param>pred</param> as argument
  which is called when a matching event is found. The event is only removed if
  <param>pred</param> returns <tt>1</tt>. 

  The <fun>find_next_cycle</fun>, <fun>find_next_time</fun> and
  <fun>find_next_time_as_ps</fun> functions take the same arguments as
  <fun>cancel</fun> but only return the number of cycles, seconds or
  picoseconds before the event occur. The <param>evclass</param> is the event
  class, <param>obj</param> is the object posting the event, and
  <param>user_data</param> is pointer to data used as a parameter when calling
  the callback function defined in the <param>evclass</param>.
  If no matching event was found, <fun>find_next_cycle</fun> and
  <fun>find_next_time</fun> return <math>-1</math>;
  <fun>find_next_time_as_ps</fun> returns <tt>ILLEGAL_DURATION</tt>.

  The <fun>events</fun> method returns a list of all pending events in
  expiration order. Each element is a four-element list containing the event
  object, the event class name, the expiration time counted in cycles as an
  integer and the event description as given by the event class
  <fun>describe</fun> method, or NIL for events whose event class does not
  define that method.

  What happens to already posted events when a frequency change occurs is
  implementation dependent. Simics processors will scale the cycle queue to
  keep the time left before triggering events equal across the frequency
  change. Note that the new times will be rounded to entire cycles during the
  scaling, so approximations may occur when switching between high and low
  frequencies.</dd>

  <dt>Implementation</dt>

  <dd>It is implementation dependent how the queue is implemented, whether
  cycles or seconds are used as underlying time unit, and what happens when the
  frequency is changed.

  Objects implementing the cycle interface are usually meant to be scheduled by
  Simics itself. For this to happen, a number of conditions must be fulfilled:

  <ul>

  <li>Each schedulable object implementing the <iface>cycle</iface> interface
  must be controlled by an object implementing the <iface>execute</iface>
  interface. It can be the same object that implements the
  <iface>execute</iface> interface. The object implementing the
  <iface>execute</iface> interface points to the object implementing the
  <iface>cycle</iface> interface via its <attr>queue</attr> attribute.</li>

  <li>Any schedulable object implementing the <iface>cycle</iface> interface
  must inform Simics about changes in frequency by calling the
  <fun>VT_clock_frequency_change</fun> function. That also applies to the
  initial frequency set when the object is created.</li>

  <li>For schedulable objects, the <iface>cycle</iface> interface must be
  registered with <fun>SIM_register_clock</fun>, which will also add some
  Simics specific attributes to the corresponding class. Beyond those, the
  implementor of the <iface>cycle</iface> can use any checkpoint
  representation. The <param>name</param> field in the event class data
  structure is unique, and the attribute setter function for checkpoint restore
  can use <fun>SIM_get_event_class</fun> to get the event class structure
  corresponding to an event class name.</li>

  </ul></dd>
  </dl>

  <insert-until text="// ADD INTERFACE cycle_interface_t"/>
  </add>

  <add id="cycle_interface_exec_context">
  Cell Context for all methods.
  </add>
*/

SIM_INTERFACE(cycle) {

        cycles_t (*get_cycle_count)(conf_object_t *queue);
        double (*get_time)(conf_object_t *queue);
        cycles_t (*cycles_delta)(conf_object_t *NOTNULL clock,
                                 double when);

        uint64 (*get_frequency)(conf_object_t *queue);

        void (*post_cycle)(
                conf_object_t *NOTNULL queue,
                event_class_t *NOTNULL evclass,
                conf_object_t *NOTNULL obj,
                cycles_t cycles,
                lang_void *user_data);
        void (*post_time)(
                conf_object_t *NOTNULL queue,
                event_class_t *NOTNULL evclass,
                conf_object_t *NOTNULL obj,
                double seconds,
                lang_void *user_data);

        void (*cancel)(
                conf_object_t *NOTNULL queue,
                event_class_t *NOTNULL evclass,
                conf_object_t *NOTNULL obj,
                int (*pred)(lang_void *data, lang_void *match_data),
                lang_void *match_data);

        cycles_t (*find_next_cycle)(
                conf_object_t *NOTNULL queue,
                event_class_t *NOTNULL evclass,
                conf_object_t *NOTNULL obj,
                int (*pred)(lang_void *data, lang_void *match_data),
                lang_void *match_data);

        double (*find_next_time)(
                conf_object_t *NOTNULL queue,
                event_class_t *NOTNULL evclass,
                conf_object_t *NOTNULL obj,
                int (*pred)(lang_void *data, lang_void *match_data),
                lang_void *match_data);

        attr_value_t (*events)(conf_object_t *NOTNULL obj);

        /* new picoseconds based functions */
        local_time_t (*get_time_in_ps)(conf_object_t *queue);
        cycles_t (*cycles_delta_from_ps)(conf_object_t *NOTNULL clock,
                                         local_time_t when);
        void (*post_time_in_ps)(
                conf_object_t *NOTNULL queue,
                event_class_t *NOTNULL evclass,
                conf_object_t *NOTNULL obj,
                duration_t picoseconds,
                lang_void *user_data);

        duration_t (*find_next_time_in_ps)(
                conf_object_t *NOTNULL queue,
                event_class_t *NOTNULL evclass,
                conf_object_t *NOTNULL obj,
                int (*pred)(lang_void *data, lang_void *match_data),
                lang_void *match_data);
};

#define CYCLE_INTERFACE "cycle"
// ADD INTERFACE cycle_interface_t

#if defined(__cplusplus)
}
#endif

#endif
