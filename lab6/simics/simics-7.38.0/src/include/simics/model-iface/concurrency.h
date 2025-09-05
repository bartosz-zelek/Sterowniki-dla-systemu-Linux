/*
  © 2018 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_MODEL_IFACE_CONCURRENCY_H
#define SIMICS_MODEL_IFACE_CONCURRENCY_H

#include <simics/base/types.h>
#include <simics/pywrap.h>
#include <simics/base/attr-value.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add-type id="concurrency_mode_t def"></add-type> */
typedef enum {
        /* Model uses the Standard Device Model */
        Sim_Concurrency_Mode_Serialized = 1,

        /* Model uses the Threaded Device Model.
           Direct memory pages are protected against concurrency. */
        Sim_Concurrency_Mode_Serialized_Memory = 2,

        /* Model uses the Threaded Device Model.
           Concurrency can be observed on direct memory pages */
        Sim_Concurrency_Mode_Full = 4,
} concurrency_mode_t;

/*
   <add id="concurrency_mode_interface_t">
   <ndx>concurrency_mode_t</ndx>

   The <iface>concurrency_mode</iface> interface allows a model to
   support a concurrency mode other than the default
   <const>Sim_Concurrency_Mode_Serialized</const>.

   The <fun>supported_modes</fun> method returns a bitmask
   with all the supported modes.

   The <fun>current_mode</fun> method returns the currently active
   mode. Note that the model should normally checkpoint this setting.

   The <fun>switch_mode</fun> method is invoked by Simics to notify
   a model that a different concurrency mode has been selected by the user.
   This typically happens when the user switches threading mode using
   the <cmd>set-threading-mode</cmd> command. The object will typically
   be placed in a different thread domain shortly after the call to this
   method.

   A model will be placed in the cell thread domain if it uses
   the <const>Sim_Concurrency_Mode_Serialized</const> mode and
   in a separate thread domain otherwise. The <iface>concurrency_group</iface>
   interface can be used to ensure that multiple objects are placed in the
   same thread domain.

   Refer to the chapter about threading in the
   <cite>API Reference Manual</cite> for details about thread domains,
   the Standard Device Model and the Threaded Device Model.

   <insert id="concurrency_mode_t def"/>
   <insert-until text="// ADD INTERFACE concurrency_mode_interface_t"/>
   </add>

   <add id="concurrency_mode_interface_exec_context">
   Global Context for all methods.
   </add>
*/
SIM_INTERFACE(concurrency_mode) {
        concurrency_mode_t (*supported_modes)(conf_object_t *NOTNULL obj);
        concurrency_mode_t (*current_mode)(conf_object_t *NOTNULL obj);
        void (*switch_mode)(conf_object_t *NOTNULL obj,
                            concurrency_mode_t mode);
};
#define CONCURRENCY_MODE_INTERFACE "concurrency_mode"
// ADD INTERFACE concurrency_mode_interface_t

/*
   <add id="concurrency_group_interface_t">

   The <iface>concurrency_group</iface> interface is used to ensure
   that groups of objects are placed in the same thread domain.

   The <fun>serialized_memory_group</fun> method returns a list with objects
   that should be placed in the same thread domain when the models
   run in <const>Sim_Concurrency_Mode_Serialized_Memory</const>.

   The <fun>execution_group</fun> method returns a list with objects that
   should always be placed in the same thread domain.

   Both methods take a <arg>group_index</arg> argument, allowing
   multiple lists to be returned. The index argument is 0-based,
   and a nil attribute for the first unsupported index. Note that
   returned lists do not necessarily contain the object implementing
   the interface; the interface can be used to group other objects.

   When Simics forms the thread domain groups, all objects implementing the
   <iface>concurrency_group</iface> are queried, and the constraints
   are combined. For instance, the groups [A, B] and [A, C]
   are combined into the group [A, B, C]. That is, the objects
   A, B and C will be put in the same thread domain.

   Refer to the threading chapter in the <cite>API Reference Manual</cite>
   for more details.

   <insert-until text="// ADD INTERFACE concurrency_group_interface_t"/>
   </add>

   <add id="concurrency_group_interface_exec_context">
   Global Context for all methods.
   </add>
*/
SIM_INTERFACE(concurrency_group) {
        attr_value_t (*serialized_memory_group)(conf_object_t *NOTNULL obj,
                                                unsigned group_index);
        attr_value_t (*execution_group)(conf_object_t *NOTNULL obj,
                                        unsigned group_index);
};
#define CONCURRENCY_GROUP_INTERFACE "concurrency_group"
// ADD INTERFACE concurrency_group_interface_t

EXPORTED void VT_update_thread_domain_assignments(void);

#if defined(__cplusplus)
}
#endif

#endif
