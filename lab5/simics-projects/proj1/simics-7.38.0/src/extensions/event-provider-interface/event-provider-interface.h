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

#ifndef EVENT_PROVIDER_INTERFACE_H
#define EVENT_PROVIDER_INTERFACE_H

#include <simics/device-api.h>
#include <simics/pywrap.h>
#include <simics/base/time.h>

#ifdef __cplusplus
extern "C" {
#endif


/*

<add id="event_provider_interface_t">

  =============================================

                TECH-PREVIEW

    This interface may change without notice.

  =============================================

  The <iface>event_provider</iface> interface is used internally in between
  ISIM modules.

  The method <fun>event_name</fun> returns name of event with index
  <arg>n</arg> or <tt>NULL</tt> if there is no event with that index.

  The method <fun>event_value</fun> returns accumulated value for event
  with index <arg>n</arg>. Output is undefined if there is no event with
  that index.

  Events must de indexed from <tt>0</tt> to <tt>last</tt> without gaps.

  To use the <iface>event-provider</iface> add the following
  EXTRA_MODULE_VPATH := event-provider-interface
  to the modules Makefile.

  <insert-until text="// ADD INTERFACE event_provider_interface"/>

  </add>
  <add id="event_provider_interface_exec_context">
  Called from performance models.
  </add>
 */



SIM_INTERFACE(event_provider) {
        const char *(*event_name)(conf_object_t *obj, unsigned n);
        uint64 (*event_value)(conf_object_t *obj, unsigned n);
};

#define EVENT_PROVIDER_INTERFACE "event_provider"

// ADD INTERFACE event_provider_interface


#ifdef __cplusplus
}
#endif

#endif /* ! EVENT_PROVIDER_INTERFACE_H */
