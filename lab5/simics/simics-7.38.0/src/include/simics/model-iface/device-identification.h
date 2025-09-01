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

#ifndef SIMICS_MODEL_IFACE_DEVICE_IDENTIFICATION_H
#define SIMICS_MODEL_IFACE_DEVICE_IDENTIFICATION_H

#include <simics/base/types.h>
#include <simics/base/attr-value.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/*
   <add id="device_identification_interface_t"> The
   <iface>device_identification</iface> interface is used to identify device
   type such as product, stepping etc.

   <fun>get_id</fun> returns the identifier for the given <arg>key</arg>.
   Existing keys can be retrieved using the <fun>get_key</fun> function. All
   existing keys should return a valid identifier string. For unknown keys the
   returned value is NULL.

   <fun>get_key</fun> returns the key at the specified <arg>index</arg> or NULL
   starting at 0, to get the key at that index. At the <arg>index</arg> where
   NULL is returned there are no more keys.

   <fun>get_key</fun> returns the key at the specified <arg>index</arg>, or
   NULL if the <arg>index</arg> does not correspond to a key. To get the
   complete list of keys, call <fun>get_key</fun> using an <arg>index</arg>
   starting at zero, and increase the <arg>index</arg> for each call until
   <fun>get_key</fun> returns NULL.

   <insert-until text="// ADD INTERFACE device_identification_interface"/>
   </add>
   <add id="device_identification_interface_exec_context">
   Cell Context for all methods.
   </add>
*/
SIM_INTERFACE(device_identification) {
        const char *(*get_id)(conf_object_t *NOTNULL obj, const char *key);
        const char *(*get_key)(conf_object_t *NOTNULL obj, unsigned int index);
};

#define DEVICE_IDENTIFICATION_INTERFACE "device_identification"
// ADD INTERFACE device_identification_interface

#if defined(__cplusplus)
}
#endif

#endif
