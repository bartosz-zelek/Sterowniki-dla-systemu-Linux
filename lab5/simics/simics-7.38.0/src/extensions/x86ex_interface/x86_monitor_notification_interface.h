/* x86_monitor_notification_interface.h - notify about monitored memory write

  © 2010 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef X86_MONITOR_NOTIFICATION_INTERFACE_H
#define X86_MONITOR_NOTIFICATION_INTERFACE_H

#include <simics/device-api.h>
#include <simics/pywrap.h>

#ifdef __cplusplus
extern "C" {
#endif

/* <add id="x86_monitor_notification_interface_t">
    This interface is meant to be implemented by a processor to support
    complex cases of MONITOR/MWAIT instruction pair. A processor will be
    notified about writes to the monitored write-back memory range through the
    <b>notify</b> method. The return value indicates whether to unsubscribe from
    notifications or not. See <b>x86_monitor_interface_t</b> to find out how to
    subscribe for write notifications to a write-back memory range.
<insert-until text="// ADD INTERFACE x86_monitor_notification_interface"/>
    </add>
    <add id="x86_monitor_notification_interface_exec_context">
    Cell Context.
    </add> */

SIM_INTERFACE(x86_monitor_notification) {
        bool (*notify)(conf_object_t *obj);
};

#define X86_MONITOR_NOTIFICATION_INTERFACE "x86_monitor_notification"
// ADD INTERFACE x86_monitor_notification_interface

#ifdef __cplusplus
}
#endif

#endif /* ! X86_MONITOR_NOTIFICATION_INTERFACE_H */
