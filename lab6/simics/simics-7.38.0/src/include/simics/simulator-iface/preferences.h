/*
  © 2011 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SIMULATOR_IFACE_PREFERENCES_H
#define SIMICS_SIMULATOR_IFACE_PREFERENCES_H

#include <simics/base/attr-value.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* Internal use only */
/* <add id="preference_interface_t">

   <ndx>preference_interface_t</ndx>
   <ndx>get_preference_for_module_key</ndx>
   <ndx>set_preference_for_module_key</ndx>

   The preference interface is implemented by objects that store preferences
   on the behalf of other modules. Preferences are settings that are persistent
   between sessions. Typically there is a single object implementing the
   preference interface.

   <insert-until text="// ADD INTERFACE preference_interface"/>

   <b>get_preference_for_module_key</b> is called to retrieve a preference for
   a specified <var>module</var> and <var>key</var>. If no value has been set
   an <i>invalid</i> attribute is returned.

   <b>set_preference_for_module_key</b> is called to store a preference for
   a specified <var>module</var> and <var>key</var>. Any attribute type is
   allowed for the value, including nested types. However, the value may no
   contain any invalid attributes.

   </add>
   <add id="preference_interface_exec_context">
   Global Context for all methods.
   </add> */

SIM_INTERFACE(preference) {
        attr_value_t
        (*get_preference_for_module_key)(conf_object_t *NOTNULL prefs,
                                         const char *NOTNULL module,
                                         const char *NOTNULL key);

        void (*set_preference_for_module_key)(conf_object_t *NOTNULL prefs,
                                              attr_value_t value,
                                              const char *NOTNULL module,
                                              const char *NOTNULL key);
};

#define PREFERENCE_INTERFACE "preference"
// ADD INTERFACE preference_interface

#if defined(__cplusplus)
}
#endif

#endif // SIMICS_SIMULATOR_IFACE_PREFERENCES_H
