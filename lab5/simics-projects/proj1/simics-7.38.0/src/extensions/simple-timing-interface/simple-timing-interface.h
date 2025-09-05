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

#ifndef SIMPLE_TIMING_INTERFACE_H
#define SIMPLE_TIMING_INTERFACE_H

#include <simics/device-api.h>
#include <simics/pywrap.h>
#include <simics/base/time.h>

#ifdef __cplusplus
extern "C" {
#endif

/*

<add id="simple_timing_v2_interface_t">

  =============================================

                TECH-PREVIEW

    This interface may change without notice.

  =============================================

  The <iface>simple_timing_v2</iface> interface is used to send information
  about timing, instructions executed and activity factor from timing models
  to power and temperature models. It should be implemented by the consumer
  of the data.

  The period that is covered by a call is called a heartbeat. The length of
  that period may vary between calls.

  When a core has completed a heartbeat <fun>new_active_data</fun> is called
  and when a period of idleness has passed <fun>new_idle_data</fun> is called.

  The <arg>core</arg> argument is the core the performance data comes from.

  The <arg>time</arg> argument is the virtual time of the core at the end of
  the heartbeat.

  The <arg>frequency</arg> argument is what the core frequency was set to at
  the time of the call.

  The <arg>cycles</arg> argument is the number of cycles in the heartbeat.

  The <arg>ipc</arg> argument is average ipc during this heartbeat.

  The <arg>cdyn</arg> argument is average activity factor during the heartbeat.

  The <arg>version_data</arg> argument can be a <tt>NULL</tt> or a pointer to
  simple json-string that can contain information about the performance model.

  <note>Temporal decoupling may cause calls regarding different cores to come
  out of order with regards to virtual time.

  Time during heartbeat may not add up with time passed since last call on a
  particular core, especially when cores goes in and out of idle-mode.</note>

  To use the <iface>simple_timing_v2</iface> add the following
  EXTRA_MODULE_VPATH := simple-timing-interface
  to the modules Makefile.

  <insert-until text="// ADD INTERFACE simple_timing_v2_interface"/>

  </add>
  <add id="simple_timing_v2_interface_exec_context">
  Called from performance models.
  </add>
 */

#define SIMPLE_TIMING_V2_MAX_NR_EVENTS  120

SIM_INTERFACE(simple_timing_v2) {
        bool (*new_active_data)
                (conf_object_t *obj,
                 double time,
                 conf_object_t *core,
                 uint64 frequency,

                 cycles_t cycles,
                 double ipc,
                 double cdyn,

                 char const *version_data);

        bool (*new_idle_data)
                (conf_object_t *obj,
                 double time,
                 conf_object_t *core,
                 uint64 frequency,
                 cycles_t cycles);
};

#define SIMPLE_TIMING_V2_INTERFACE "simple_timing_v2"

// ADD INTERFACE simple_timing_v2_interface

#ifdef __cplusplus
}
#endif

#endif /* ! SIMPLE_TIMING_INTERFACE_H */
