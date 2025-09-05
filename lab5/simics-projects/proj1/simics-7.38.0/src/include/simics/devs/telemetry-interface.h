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

#ifndef SIMICS_DEVS_TELEMETRY_INTERFACE_H
#define SIMICS_DEVS_TELEMETRY_INTERFACE_H

#include <simics/base/conf-object.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add-type id="telemetry_class_id_t def"> </add-type> */
typedef int telemetry_class_id_t;
/* <add-type id="telemetry_id_t def"> </add-type> */
typedef int telemetry_id_t;

/* <add id="telemetry_interface_t">

  The <iface>telemetry</iface> interface is used to fetch telemetry
  data.

  A model implements one or more telemetry-classes.
  These are identified by an identifier (class ID) and a name.
  The telemetries also has a name and an identifier (telemetry ID).

  This layout forms a two level hierarchy with classes and their telemetries.
  The class and telemetry ID's are represented as integers and must be part of a
  contiguous range starting from zero, i.e., the hierarchy can be viewed as a table
  with telemetries, where the row numbers (>= 0), represents the class ID's, and the
  column numbers (>=0) represents the telemetry ID's.
  For unknown ID's NULL should be returned.

  Both telemetry_class_id and telemetry_id can be enumerated using the functions
  get_telemetry_class_name and get_telemetry_name by starting with an ID
  of zero and go upp until NULL is returned.
  A model should not log an error when an invalid ID is presented (but logging
  info on level 3 makes sense for debug purposes).
  Error handling for missing expected telemetry IDs should be handled in the model
  requesting and retrieving the telemetry value.

  The <fun>get_telemetry_class_id</fun> function returns the telemetry_class_id
  for the telemetry class with name <arg>telemetry_class_name</arg>.
  If no telemetry class with that name is found or that telemetry class is not enabled
  a negative number is returned.

  The <fun>get_telemetry_class_name</fun> returns the name of the corresponding
  <arg>telemetry_class_id</arg>. If no telemetry class is found with that id the value
  NULL is returned.

  The <fun>get_telemetry_class_description</fun> can return a description string for the
  <arg>telemetry_class_id</arg>. If no telemetry class is found with that id the value
  NULL is returned.

  The <fun>get_telemetry_id</fun> function returns the telemetry_id for the telemetry
  with name <arg>telemetry_name</arg> belonging to <arg>telemetry_class_id</arg>.
  If no telemetry with that name is found in the telemetry class a negative number is
  returned.

  The <fun>get_telemetry_name</fun> returns the name of the corresponding
  <arg>telemetry_id</arg> belonging to <arg>telemetry_class_id</arg>. If no telemetry
  with that id is found in the telemetry class the value NULL is returned.

  The <fun>get_telemetry_description</fun> can return a description string for the
  <arg>telemetry_id</arg> belonging to <arg>telemetry_class_id</arg>. If no telemetry
  with that id the value is found NULL is returned.

  The <fun>get_value</fun> function returns the value for the <arg>telemetry_id</arg>
  within <arg>telemetry_class_id</arg>.

  Note all known telemetry_classes are not always enabled in all models,
  i.e. <fun>get_telemetry_class_name</fun> can return a valid name for some
  <arg>telemetry_class_id</arg> but <fun>get_telemetry_id</fun> on that name may still
  return an invalid identifier if that telemetry_class is not enabled in the model.

  Note that the identifiers may vary between invocations of the model and must not be
  stored between runs.

  The command <cmd>list-telemetry-classes</cmd> can be helpful while developing with
  the telemetry-interface.

  <insert id="telemetry_class_id_t def"/>
  <insert id="telemetry_id_t def"/>
  <insert-until text="// ADD INTERFACE telemetry_interface"/>

  </add>
  <add id="telemetry_interface_exec_context">
  Global Context for all methods.
  </add>
*/
SIM_INTERFACE(telemetry) {
        telemetry_class_id_t (*get_telemetry_class_id)(conf_object_t *obj,
                                                       const char *telemetry_class_name);
        const char *(*get_telemetry_class_name)(conf_object_t *obj,
                                                telemetry_class_id_t telemetry_class_id);
        const char *(*get_telemetry_class_description)(conf_object_t *obj,
                                                      telemetry_class_id_t telemetry_class_id);
        telemetry_id_t (*get_telemetry_id)(conf_object_t *obj,
                                           telemetry_class_id_t telemetry_class_id,
                                           const char *telemetry_name);
        const char *(*get_telemetry_name)(conf_object_t *obj,
                                          telemetry_class_id_t telemetry_class_id,
                                          telemetry_id_t telemetry_id);
        const char *(*get_telemetry_description)(conf_object_t *obj,
                                                 telemetry_class_id_t telemetry_class_id,
                                                 telemetry_id_t telemetry_id);
        attr_value_t (*get_value)(conf_object_t *obj,
                                  telemetry_class_id_t telemetry_class_id,
                                  telemetry_id_t telemetry_id);
};

#define TELEMETRY_INTERFACE "telemetry"

// ADD INTERFACE telemetry_interface

#if defined(__cplusplus)
}
#endif

#endif	/* !SIMICS_DEVS_TELEMETRY_INTERFACE_H */
