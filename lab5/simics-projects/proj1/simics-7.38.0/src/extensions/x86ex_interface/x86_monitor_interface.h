/* x86_monitor_interface.h - interface for complex MONITOR/MWAIT simulation

  © 2010 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef X86_MONITOR_INTERFACE_H
#define X86_MONITOR_INTERFACE_H

#include <simics/device-api.h>
#include <simics/pywrap.h>

#ifdef __cplusplus
extern "C" {
#endif

/* <add id="x86_monitor_interface_t">
    This interface is meant to be implemented by an uncore device to support
    implementation of MONITOR/MWAIT instruction pair. A listener (e.g. a cpu)
    uses this interface to setup monitored write-back memory range.
    All listeners subscribed to a particular write-back memory range will be
    notified via <b>x86_monitor_notification_interface</b> when a memory write
    transaction hits the monitored memory range.

    This interface is internal and may change without notice.

    The <b>arm</b> method is to subscribe for notifications about writes to
    a write-back memory range starting from <b>start_address</b> up to
    <b>start_address</b> + <b>length</b> - 1 , returns true on success.
    The <b>disarm</b> unsubscribes <b>listener</b>, so the latter won't be
    notified about writes to a monitored memory range, returns true on success.

<insert-until text="// ADD INTERFACE x86_monitor_interface"/>
    </add>
    <add id="x86_monitor_interface_exec_context">
    Cell Context for all methods.
    </add> */
SIM_INTERFACE(x86_monitor) {
    bool (*arm)(conf_object_t *obj, conf_object_t *listener,
                physical_address_t start_address, physical_address_t length);
    bool (*disarm)(conf_object_t *obj, conf_object_t *listener);
};

#define X86_MONITOR_INTERFACE "x86_monitor"
// ADD INTERFACE x86_monitor_interface

#ifdef __cplusplus
}
#endif

#endif /* ! X86_MONITOR_INTERFACE_H */
