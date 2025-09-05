/*
  © 2014 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SIMULATOR_IFACE_OSA_NODE_TREE_ADMIN_INTERFACE_H
#define SIMICS_SIMULATOR_IFACE_OSA_NODE_TREE_ADMIN_INTERFACE_H

#include <simics/device-api.h>
#include "osa-types.h"

#if defined(__cplusplus)
extern "C" {
#endif

/* <add id="osa_node_tree_admin_interface_t">
   <name>osa_node_tree_admin_interface_t</name>
   <ndx>osa_node_tree_admin_interface_t</ndx>

   <ndx>begin!osa_node_tree_admin interface method</ndx> <fun>begin</fun> is
   called from the mapper to start a transaction when adding, updating or
   removing nodes in the node tree. The <arg>initiator</arg> argument specifies
   the initiator processor, this can be nil if the transaction is not initiated
   by a processor. The <arg>initiator</arg> will be passed as <arg>cpu</arg>
   argument to callback functions in the
   <iface>osa_node_tree_notification</iface> interface. The function returns an
   ID to be used when calling <fun>end</fun>.

   <ndx>end!osa_node_tree_admin interface method</ndx> <fun>end</fun> is called
   from the mapper to end a transaction. This should be called at the end of a
   transaction when all modifications to the node tree are complete. The
   <arg>transaction_id</arg> argument should be the value returned from the
   <fun>begin</fun> method that started the transaction.  Stacked calls to
   <fun>begin</fun> and <fun>end</fun> are possible, then the transaction will
   be ended when the first <fun>begin</fun> is ended. The <fun>begin</fun>
   methods must be ended in the opposite order that they were called. For
   stacked calls to <fun>begin</fun> only the initiator of the first call to
   begin will be used.

   <ndx>create!osa_node_tree_admin interface method</ndx>
   <fun>create</fun> creates a new node tree and associates <arg>mapper</arg>
   with it. The properties of the root node for the new tree are set through
   <arg>props</arg> which is a dictionary. The returned value is the node ID
   of the created root node.

   <ndx>add!osa_node_tree_admin interface method</ndx>
   <fun>add</fun> adds a new node rooted at <arg>parent_id</arg>. The new
   node gets the properties specified by <arg>props</arg> which is a
   dictionary. The returned value is the node ID of the newly added node.

   <ndx>update!osa_node_tree_admin interface method</ndx>
   <fun>update</fun> updates the properties of a node in the node tree.
   <arg>node_id</arg> specifies the node to be updated and <arg>props</arg>
   is a dictionary of the properties that should be updated.

   <ndx>set_property!osa_node_tree_admin interface method</ndx>
   <fun>set_property</fun> updates one property of a node in the node tree.
   This is similar to <fun>update</fun> but there is no need to build up a
   dictionary. <arg>node_id</arg> specifies the node to be updated,
   <arg>key</arg> the key of the property to be updated and <arg>value</arg>
   the value to update the property with.

   <ndx>reset!osa_node_tree_admin interface method</ndx>
   <fun>reset</fun> resets a node, <arg>node_id</arg>, to the properties
   specified by <arg>props</arg>. All the children for the node will be removed
   and all properties except the ones in <arg>props</arg> will be removed. If
   the reset node was previously active it will be deactivated.

   All methods have the limitation that data and dictionary types are not
   supported as the value of a property. The keys of the <arg>props</arg>
   dictionary must all be of string type.

   <ndx>remove!osa_node_tree_admin interface method</ndx>
   <fun>remove</fun> removes the node with the ID specified by
   <arg>node_id</arg> from the node tree.

   <ndx>event!osa_node_tree_admin interface method</ndx>
   <fun>event</fun> registers a new event, associated with the given
   <arg>node_id</arg>. The <arg>event_name</arg> argument is the name of the
   event, this name can also be used by a user to only listen to a specific
   event type per node tree. The <arg>event_data</arg> argument is the data
   associated with the event and it is up to the responsible tracker to
   document its exact form. An event differes from other properties in the way
   that they are not persistent.

   <ndx>activate!osa_node_tree_admin interface method</ndx>
   <fun>activate</fun> sets <arg>node_id</arg> as the active node for
   processor <arg>cpu</arg> in the node tree where <arg>node_id</arg> exists.
   All the ancestors of <arg>node_id</arg> will also be set as active. Any
   previously active node for <arg>cpu</arg> will be deactivated.
   
   <ndx>deactivate!osa_node_tree_admin interface method</ndx>
   <fun>deactivate</fun> deactivates processor <arg>cpu</arg> in the node tree
   where <arg>node_id</arg> exists. The <arg>node_id</arg> argument should be
   set to the node that was previously active on <arg>cpu</arg> in the node
   tree.

   <ndx>register_formatter!osa_node_tree_admin interface method</ndx>
   <fun>register_formatter</fun> registers a callback function which will be
   called when the property specified by the <arg>key</arg> argument should be
   formatted in a specific way. This is typically called from
   <fun>"get_formatted_properties"</fun> in the
   <iface>osa_node_tree_query</iface> interface. This is useful for
   systems where an integer property should be formatted in hexadecimal to make
   a closer match to the target OS mapper of that property. For example, a node
   with the following properties:

   <pre size="smaller">{"name": "foo", "tid": 4711}</pre>

   could then be formatted as:

   <pre size="smaller">{"name": "foo", "tid": "0x1267"}</pre>

   The function itself must return a string, given as an attr_value_t. The
   mapper must have been registered by calling the <fun>create</fun> function
   in the <iface>osa_node_tree_admin</iface> interface before registering a
   formatter.

   The <fun>register_formatter</fun> function returns a cancel id that can be
   passed to the <fun>unregister_formatter</fun> function to unregister the
   formatting function. Registering a new formatter on the same
   <arg>node_id</arg> and <arg>key</arg> as a previous formatter will override
   the previous formatter with the new one. This is useful when dealing with
   stacked trackers and a sub-tracker needs to register a formatter for a node
   that already have a registered formatter.

   <ndx>unregister_formatter!osa_node_tree_admin interface method</ndx>
   <fun>unregister_formatter</fun> unregisters a previously registered
   formatter function using the <fun>register_formatter</fun> function.

   <insert-until text="// ADD INTERFACE osa_node_tree_admin_interface"/>
   </add>

   <add id="osa_node_tree_admin_interface_exec_context">
   Cell Context for all methods.
   </add>
*/
SIM_INTERFACE(osa_node_tree_admin) {
        transaction_id_t (*begin)(conf_object_t *NOTNULL obj,
                                  conf_object_t *initiator);
        void (*end)(conf_object_t *NOTNULL obj,
                    transaction_id_t transaction_id);
        node_id_t (*create)(conf_object_t *NOTNULL obj,
                            conf_object_t *NOTNULL mapper, attr_value_t props);
        node_id_t (*add)(conf_object_t *NOTNULL obj,
                         node_id_t parent_id, attr_value_t props);
        void (*update)(conf_object_t *NOTNULL obj, node_id_t node_id,
                       attr_value_t props);
        void (*remove)(conf_object_t *NOTNULL obj, node_id_t node_id);
        void (*event)(conf_object_t *NOTNULL obj, node_id_t node_id,
                      const char *event_name, attr_value_t event_data);
        void (*activate)(conf_object_t *NOTNULL obj, node_id_t node_id,
                         conf_object_t *NOTNULL cpu);
        void (*deactivate)(conf_object_t *NOTNULL obj, node_id_t node_id,
                           conf_object_t *NOTNULL cpu);
        cancel_id_t (*register_formatter)(
                conf_object_t *NOTNULL obj, node_id_t node_id,
                const char *NOTNULL key,
                bool recursive, attr_value_t (*formatter)(attr_value_t val));
        void (*unregister_formatter)(
                conf_object_t *NOTNULL obj, cancel_id_t node_id);
        void (*reset)(conf_object_t *NOTNULL obj,
                      node_id_t node_id, attr_value_t props);
        void (*set_property)(conf_object_t *NOTNULL obj, node_id_t node_id,
                             const char *key, attr_value_t value);
};

#define OSA_NODE_TREE_ADMIN_INTERFACE "osa_node_tree_admin"
// ADD INTERFACE osa_node_tree_admin_interface

#if defined(__cplusplus)
}
#endif

#endif  /* ! SIMICS_SIMULATOR_IFACE_OSA_NODE_TREE_ADMIN_INTERFACE_H */
