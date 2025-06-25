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

#ifndef SIMICS_SIMULATOR_IFACE_RECORDER_H
#define SIMICS_SIMULATOR_IFACE_RECORDER_H

#include <simics/base/types.h>
#include <simics/util/dbuffer.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add id="recorder_v2_interface_t">
   The <iface>recorder_v2</iface> interface is implemented by the recorder, and
   can be used by any object interacting with the outside world in order to
   make re-runs of the same simulation behave identically. This is a
   requirement for reverse execution to work. Objects using this interface must
   implement the <iface>recorded</iface> interface themselves.

   An object uses it by calling the <fun>record</fun> method with itself and
   the data it wishes to record as parameters. The recorder will then save
   the data and call the <fun>input</fun> method in the <iface>recorded</iface>
   interface on the object.

   The <fun>playback</fun> method returns whether the recorder is currently
   playing back recorded data. It may be used by the object to determine if
   output to the outside world should be dropped or not.

   <insert-until text="// ADD INTERFACE recorder_v2_interface"/>
   </add>
   <add id="recorder_v2_interface_exec_context">
     Cell Context for all methods.
   </add> */
SIM_INTERFACE(recorder_v2) {
        void (*record)(conf_object_t *NOTNULL obj,
                       conf_object_t *NOTNULL sender, bytes_t data);
        bool (*playback)(conf_object_t *NOTNULL obj);
};
#define RECORDER_V2_INTERFACE "recorder_v2"
// ADD INTERFACE recorder_v2_interface

/* <append id="recorder_v2_interface_t">
   The <iface>recorded</iface> interface is implemented by objects that wish to
   use the <iface>recorder_v2</iface> interface.

   The <fun>input</fun> method is called with data that has been recorded.
   The <param>playback</param> parameter is set if the data came from a
   previous recording, and clear if the data came directly from a call to
   <fun>record</fun> in <iface>recorder_v2</iface> with live data.

   <insert-until text="// ADD INTERFACE recorded_interface"/>
   </append>
   <add id="recorded_interface_exec_context">
     Cell Context for all methods.
   </add> */
SIM_INTERFACE(recorded) {
        void (*input)(conf_object_t *NOTNULL obj, bytes_t data, bool playback);
};
#define RECORDED_INTERFACE "recorded"
// ADD INTERFACE recorded_interface

#if defined(__cplusplus)
}
#endif

#endif
