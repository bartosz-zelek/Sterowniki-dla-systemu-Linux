/*
  © 2015 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_MODEL_IFACE_HAP_LISTEN_H
#define SIMICS_MODEL_IFACE_HAP_LISTEN_H

#include <simics/base/types.h>
#include <simics/pywrap.h>
#include <simics/base/hap-producer.h>

#if defined(__cplusplus)
extern "C" {
#endif

#if !defined(PYWRAP) /* only for C */
/*
  <add id="hap_listen_interface_t">

  With the <iface>hap_listen</iface> interface, objects can pick up
  haps and process then as they wish, including re-raising the haps.

  <insert-until text="// ADD INTERFACE hap_listen_interface_t"/>
  </add>

  <add id="hap_listen_interface_exec_context">
  <table border="false">
  <tr><td><fun>occurred</fun></td><td>Cell Context</td></tr>
  </table>

  </add>
*/

SIM_INTERFACE(hap_listen) {
        void (*occurred)(conf_object_t *obj, conf_object_t *origin,
                         hap_type_t hap, int64 value, va_list ap, bool always);
};
#define HAP_LISTEN_INTERFACE "hap_listen"
// ADD INTERFACE hap_listen_interface_t
#endif

#if defined(__cplusplus)
}
#endif

#endif
