/*
  © 2021 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef TSC_UPDATE_INTERFACE_H
#define TSC_UPDATE_INTERFACE_H

#include <simics/device-api.h>
#include <simics/pywrap.h>

#ifdef __cplusplus
extern "C" {
#endif

/* <add id="tsc_update_interface_t">

   The methods allow to update/overwrite Time Stamp Counter with values coming
   from PUnit and measured in different units.

   <fun>load_tsc_from_xtal</fun> sets TSC to an absolute value scaled from
   the <arg>xtal_value</arg>. The latter value is measured in units of always
   running timer clocks.

   <insert-until text="// ADD INTERFACE tsc_update_interface"/>
   </add>
   <add id="tsc_update_interface_exec_context">
      Cell Context for all methods.
   </add> */
SIM_INTERFACE(tsc_update) {
        void (*load_tsc_from_xtal)(conf_object_t *obj, uint64 xtal_value);
};
// ADD INTERFACE tsc_update_interface

#define TSC_UPDATE_INTERFACE "tsc_update"

#ifdef __cplusplus
}
#endif

#endif /* ! TSC_UPDATE_INTERFACE_H */
