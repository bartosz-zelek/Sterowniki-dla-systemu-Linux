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

#ifndef SIMICS_SIMULATOR_IFACE_OSA_PARAMETERS_INTERFACE_H
#define SIMICS_SIMULATOR_IFACE_OSA_PARAMETERS_INTERFACE_H

#include <simics/device-api.h>
#include <simics/base/cbdata.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add id="osa_parameters_interface_t">
   <name>osa_parameters_interface_t</name>
   <ndx>osa_parameters_interface_t</ndx>

   Interface implemented by tracker components that support getting and setting
   their parameters.

   If either method fails it returns a (false, error-string) pair. If
   get_parameters succeed it returns (true, parameters). If set_parameters
   succeed it returns (true, nil).

   The parameters you pass to set_parameters and thare are returned by
   get_parameters should be a pair (tracker-kind, parameters-value), where
   tracker-kind is a string identifying the kind of tracker and parameters-value
   is the parameters for that kind of tracker. The parameters for an
   unconfigured tracker are (tracker-kind, nil).

   The <arg>include_children</arg> argument of <fun>get_parameters</fun> is
   only used for stacked tracker and specifies if parameters for guest trackers
   should be included or filtered out.

   The is_kind_supported method returns true if the tracker-kind in a parameter
   set is of the right kind for the tracker. This is no guarantee that setting
   these parameters will succeed. If the method returns false the parameters
   will not work.

   <insert-until text="// ADD INTERFACE osa_parameters_interface"/>
   </add>

   <add id="osa_parameters_interface_exec_context">
   Global Context for all methods.
   </add>

*/
SIM_INTERFACE(osa_parameters) {
        attr_value_t (*get_parameters)(conf_object_t *NOTNULL obj,
                                       bool include_children);
        attr_value_t (*set_parameters)(conf_object_t *NOTNULL obj,
                                       attr_value_t parameters);
        bool (*is_kind_supported)(conf_object_t *NOTNULL obj,
                                  const char *kind);
};

#define OSA_PARAMETERS_INTERFACE "osa_parameters"
// ADD INTERFACE osa_parameters_interface

#if defined(__cplusplus)
}
#endif

#endif /* SIMICS_SIMULATOR_IFACE_OSA_PARAMETERS_INTERFACE_H */
