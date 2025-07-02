/*
  © 2020 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_MODEL_IFACE_FREERUN_H
#define SIMICS_MODEL_IFACE_FREERUN_H

#include <simics/base/conf-object.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add id="freerun_interface_t">

   The <iface>freerun</iface> interface is provided by
   the <class>freerun-extension</class> extension class. The extension
   class augments CPU models with support for freerunning mode. The
   <iface>freerun</iface> interface is used by CPU models to
   interface with the extension class.

   The <fun>enabled</fun> member returns <const>true</const> if
   freerunning mode is enabled and <const>false</const> otherwise.
   The notifier <const>Sim_Notifier_Freerunning_Mode_Change</const> is
   triggered for the object whenever freerunning mode is enabled or disabled.

   The <fun>advance_clock</fun> is used by the CPU model to
   calculate how much its virtual time should be advanced in freerunning mode.
   The <arg>ps_limit</arg> argument is the maximal number of pico-seconds
   virtual time can be advanced. The next CPU event typically occurs
   at this time. The <arg>steps</arg> argument should be set to the number
   of elapsed instructions since the last call to <arg>advance_clock</arg>.
   The <arg>idle</arg> parameter should be set to <const>true</const> if
   the CPU is idle. The function returns the number of pico-seconds
   the virtual time of the CPU should be advanced. The returned value
   is proportional to the time spent simulating the model, but is also
   subject to configurable freerunning restrictions which ensures that
   the instruction rate is kept in an acceptable range.

   The <fun>start_clock</fun> function should be called when
   the CPU model starts instruction simulation. It is used to measure
   the amount of time used to simulate the model.

   The <fun>stop_clock</fun> function should be called when
   the CPU model stops instruction simulation.

   The <fun>current_itime</fun> function returns a prediction of the amount
   of time needed to simulate an instruction, in pico-seconds. The
   estimate is based on historic data and will always be in an interval
   which does not conflict with configured freerunning restrictions.
   The value can be used to estimate how many instructions
   can be executed until the next time event.

   <note>The <iface>freerun</iface> interface is experimental and may
   change without notice.</note>

   <insert-until text="// ADD INTERFACE freerun_interface"/>

   </add>
   <add id="freerun_interface_exec_context">
   Cell Context for all methods.
   </add> */
SIM_INTERFACE(freerun) {
        bool (*enabled)(conf_object_t *NOTNULL obj);

        int64 (*advance_clock)(conf_object_t *NOTNULL obj,
                               int64 ps_limit, int64 steps, bool idle);
        void (*start_clock)(conf_object_t *NOTNULL obj);
        void (*stop_clock)(conf_object_t *NOTNULL obj);

        uint64 (*current_itime)(conf_object_t *NOTNULL obj);
};
#define FREERUN_INTERFACE "freerun"
// ADD INTERFACE freerun_interface

#if defined(__cplusplus)
}
#endif

#endif
