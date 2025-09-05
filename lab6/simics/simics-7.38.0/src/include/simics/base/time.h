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

#ifndef SIMICS_BASE_TIME_H
#define SIMICS_BASE_TIME_H

#include <simics/base/types.h>
#include <simics/base/conf-object.h>

#if defined(__cplusplus)
extern "C" {
#endif

#ifndef TURBO_SIMICS

/* <add id="device api types">
   <name>simtime_t, cycles_t, pc_step_t, nano_secs_t</name>
   <insert id="simtime_t DOC"/>
   </add> */

/* <add id="simtime_t DOC">
   <ndx>simtime_t</ndx>
   <ndx>cycles_t</ndx>
   <ndx>pc_step_t</ndx>
   <ndx>nano_secs_t</ndx>
   <name>simtime_t, cycles_t, pc_step_t, nano_secs_t</name>
   <doc>
   <doc-item name="NAME">
   simtime_t, cycles_t, pc_step_t, nano_secs_t
   </doc-item>
   <doc-item name="SYNOPSIS">
   <insert id="simtime_t def"/>
   <insert id="cycles_t def"/>
   <insert id="pc_step_t def"/>
   <insert id="nano_secs_t def"/>
   </doc-item>
   <doc-item name="DESCRIPTION">
   These are the types used for keeping track of time in Simics.

   <type>cycles_t</type> is used when the time is specified in cycles,
   <type>pc_step_t</type> is used when the time is specified in steps, and
   <type>simtime_t</type> is used in places where it is unknown whether the
   time is in steps or cycles. See the <cite>Understanding Simics Timing</cite>
   application note for a discussion about the difference between steps and
   cycles.

   <type>nano_secs_t</type> is used to express a number of nanoseconds
   (<math>10<sup>-9</sup></math> seconds).
   </doc-item>
   </doc></add>
*/

/* <add-type id="simtime_t def"></add-type> */
typedef int64 simtime_t;
/* <add-type id="cycles_t def"></add-type> */
typedef simtime_t cycles_t;
/* <add-type id="nano_secs_t def"></add-type> */
typedef int64 nano_secs_t;

EXPORTED cycles_t SIM_cycle_count(conf_object_t *NOTNULL obj);
EXPORTED double SIM_time(conf_object_t *NOTNULL obj);

EXPORTED void SIM_stall_cycle(conf_object_t *NOTNULL obj, cycles_t cycles);
EXPORTED void SIM_stall(conf_object_t *NOTNULL obj, double seconds);
EXPORTED cycles_t SIM_stalled_until(conf_object_t *NOTNULL obj);
EXPORTED cycles_t SIM_stall_count(conf_object_t *NOTNULL obj);

EXPORTED conf_object_t *SIM_object_clock(const conf_object_t *NOTNULL obj);
EXPORTED conf_object_t *SIM_picosecond_clock(conf_object_t *NOTNULL obj);
EXPORTED void VT_set_object_clock(
        conf_object_t *NOTNULL obj, conf_object_t *clock);

EXPORTED conf_object_t *VT_object_ps_clock(conf_object_t *NOTNULL obj);

EXPORTED cycles_t VT_cycles_to_quantum_end(conf_object_t *NOTNULL obj);

#endif /*  TURBO_SIMICS */

#if defined(__cplusplus)
}
#endif

#endif
