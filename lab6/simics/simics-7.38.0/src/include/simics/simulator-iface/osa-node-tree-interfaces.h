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

#ifndef SIMICS_SIMULATOR_IFACE_OSA_NODE_TREE_INTERFACES_H
#define SIMICS_SIMULATOR_IFACE_OSA_NODE_TREE_INTERFACES_H

#include <simics/device-api.h>
#include <simics/base/cbdata.h>
#include <simics/simulator-api.h>
#include "osa-types.h"

#if defined(__cplusplus)
extern "C" {
#endif

/* <add id="osa_node_tree_query_interface_t">
   <name>osa_node_tree_query_interface_t</name>
   <ndx>osa_node_tree_query_interface_t</ndx>

   <ndx>get_root_nodes!osa_node_tree_query interface method</ndx>
   <fun>get_root_nodes</fun> returns a list of node IDs for the root nodes.

   <ndx>get_node!osa_node_tree_query interface method</ndx>
   <fun>get_node</fun> returns a dictionary for a given node, containing all
   the node's properties; or nil if no such node exists.

   <ndx>get_current_nodes!osa_node_tree_query interface method</ndx>
   <fun>get_current_nodes</fun> returns the current node path of the given
   processor. That is, a list of node IDs of all nodes that are currently
   running on the processor, the first node being the root node, and every
   subsequent node a child of the node before it. If the tracker is not
   tracking the given processor, the return value is nil.


   <ndx>get_current_processors!osa_node_tree_query interface method</ndx>
   <fun>get_current_processors</fun> returns a list of processors that are
   currently active on the given node, or one of it's descendants. The list
   will be empty if there is no active processor. Nil will be returned if the
   given node does not exist.

   <ndx>get_all_processors!osa_node_tree_query interface method</ndx>
   <fun>get_all_processors</fun> returns a list of all processors that are
   available for the OS Awareness framework.

   <ndx>get_parent!osa_node_tree_query interface method</ndx>
   <fun>get_parent</fun> returns the node ID of the specified node's parent;
   or nil if the node has no parent.

   <ndx>get_children!osa_node_tree_query interface method</ndx>
   <fun>get_children</fun> returns a list of node IDs for the children of
   the specified node, this will be an empty list if the node does not have
   any children. The method will return nil if the node does not exist.

   The <fun>get_formatted_properties</fun> function returns a dictionary
   containing all properties for a node, formatted as strings or integers. It
   will return nil upon a failure. For example, a tracker could format the
   value of property <tt>pid</tt> to show the value in hexadecimal form.

   <pre size="smaller">{'name': "Our OS", 'pid': "0x1234"}</pre>

   <insert-until text="// ADD INTERFACE osa_node_tree_query_interface"/>
   </add>

   <add id="osa_node_tree_query_interface_exec_context">
   Cell Context for all methods.
   </add>
*/

SIM_INTERFACE(osa_node_tree_query) {
        attr_value_t (*get_root_nodes)(conf_object_t *NOTNULL obj);
        attr_value_t (*get_node)(conf_object_t *NOTNULL obj, node_id_t node_id);
        attr_value_t (*get_current_nodes)(conf_object_t *NOTNULL obj,
                                          node_id_t base_id,
                                          conf_object_t *cpu);
        attr_value_t (*get_current_processors)(conf_object_t *NOTNULL obj,
                                               node_id_t node_id);
        attr_value_t (*get_all_processors)(conf_object_t *NOTNULL obj);
        conf_object_t *(*get_mapper)(conf_object_t *NOTNULL obj,
                                     node_id_t node_id);
        attr_value_t (*get_parent)(conf_object_t *NOTNULL obj,
                                   node_id_t node_id);
        attr_value_t (*get_children)(conf_object_t *NOTNULL obj,
                                     node_id_t node_id);
        attr_value_t (*get_formatted_properties)(conf_object_t *NOTNULL obj,
                                                 uint64 node_id);
};

#define OSA_NODE_TREE_QUERY_INTERFACE "osa_node_tree_query"
// ADD INTERFACE osa_node_tree_query_interface

/* <add id="osa_node_tree_notification_interface_t">
   <name>osa_node_tree_notification_interface_t</name>
   <ndx>osa_node_tree_notification_interface_t</ndx>

   <ndx>notify_create!osa_node_tree_notification interface method</ndx>
   <fun>notify_create</fun> and
   <ndx>notify_destroy!osa_node_tree_notification interface method</ndx>
   <fun>notify_destroy</fun> register callbacks to be
   called when the node is created and destroyed, respectively. The
   <arg>node_id</arg> argument specifies for which node the callback should be
   installed. By specifying both the <arg>node_id</arg> and the
   <arg>recursive</arg> arguments it is possible to get notifications for all
   nodes in a subtree. It is safe to read the node with <fun>get_node</fun> in
   the <iface>osa_node_tree_query</iface> interface from within the callback
   function.

   Calling <fun>notify_create</fun> without <arg>recursive</arg> being set is
   only useful when used together with reverse execution, as only then can the
   node ID of a node that is to be created later be known. 

   <ndx>notify_property_change!osa_node_tree_notification interface method</ndx>
   <fun>notify_property_change</fun> registers a callback that is
   triggered when the given property <arg>key</arg> changes on the node (or any
   property, if <arg>key</arg> is nil). The callback function will receive the
   name of the property that was changed in the <arg>key</arg> argument and the
   old and new values of that property in the <arg>old_val</arg> and
   <arg>new_val</arg> arguments.

   <ndx>notify_event!osa_node_tree_notification interface method</ndx>
   <fun>notify_event</fun> register a callback <arg>cb</arg> to be called when
   an event occurs for the given <arg>node_id</arg>. If the
   <arg>event_name</arg> argument is nil the callback will be associated with
   all events, by providing a specific event name instead the callback will
   only trigger for that particular event type. The <arg>event_data</arg>
   argument passed to the callback contains tracker specific data associated
   with the event. See specific tracker documentation for details.

   If <arg>recursive</arg>, the callback will be triggered for the given node's
   descendants as well as the node itself.

   Most callbacks have <arg>cpu</arg> and <arg>obj</arg> arguments. The
   <arg>cpu</arg> argument specifies the processor that caused the event, but
   may be nil if the event was not caused by a processor. The <arg>obj</arg>
   will contain the object that implements the interface. The
   <arg>node_id</arg> argument passed to the callback specifies the node ID for
   the node that has been created, destroyed, modified or has triggered an
   event.

   <ndx>notify_cpu_move_from!osa_node_tree_notification interface method</ndx>
   <fun>notify_cpu_move_from</fun> and
   <ndx>notify_cpu_move_to!osa_node_tree_notification interface method</ndx>
   <fun>notify_cpu_move_to</fun> register callbacks that are triggered when
   a processor moves from one node path to another&mdash;but only if either
   path lies in the subtree rooted at the given node <arg>node_id</arg>.

   Since a single update to the node tree can result in several different
   callbacks being triggered, reading nodes with <fun>get_node</fun> from a
   callback function may yield a result containing updates whose callbacks have
   not yet been run. For example, if two nodes change their <attr>name</attr>
   attributes simultaneously, the final state of both nodes may be visible to
   both property change callbacks. With
   <ndx>notify_callbacks_done!osa_node_tree_notification interface method</ndx>
   <fun>notify_callbacks_done</fun>, you can register a callback that will run
   when all other callbacks pertaining to a particular change in the node tree
   are finished.

   <fun>notify_enable</fun> register a callback <arg>cb</arg> to be called when
   the tracker framework is enabled.

   <fun>notify_disable</fun> register a callback <arg>cb</arg> to be called when
   the tracker framework is disabled.

   The functions that install callbacks return an integer ID. This ID can be
   passed to <fun>cancel_notify</fun>
   <ndx>cancel_notify!osa_node_tree_notification interface method</ndx> in
   order to uninstall the callback. In case of error in a notification function
   the returned cancel ID from that function will be 0.

   New callbacks registered inside a registered callback will not be called
   until the next transaction they trigger for. Canceled callbacks are canceled
   immediately.

   <insert-until text="// ADD INTERFACE osa_node_tree_notification_interface"/>
   </add>

   <add id="osa_node_tree_notification_interface_exec_context">
   Cell Context for all methods.
   </add>
*/

SIM_INTERFACE(osa_node_tree_notification)
{
        cancel_id_t (*notify_create)(conf_object_t *NOTNULL obj,
                                     node_id_t node_id, bool recursive,
                                     void (*cb)(cbdata_call_t data,
                                                conf_object_t *obj,
                                                conf_object_t *cpu,
                                                node_id_t node_id),
                                     cbdata_register_t data);
        cancel_id_t (*notify_destroy)(conf_object_t *NOTNULL obj,
                                      node_id_t node_id, bool recursive,
                                      void (*cb)(cbdata_call_t data,
                                                 conf_object_t *obj,
                                                 conf_object_t *cpu,
                                                 node_id_t node_id),
                                      cbdata_register_t data);
        cancel_id_t (*notify_property_change)(
                conf_object_t *NOTNULL obj, node_id_t node_id,
                const char *key, bool recursive,
                void (*cb)(cbdata_call_t data, conf_object_t *obj,
                           conf_object_t *cpu, node_id_t node_id,
                           const char *key, attr_value_t old_val,
                           attr_value_t new_val),
                cbdata_register_t data);
        cancel_id_t (*notify_cpu_move_from)(
                conf_object_t *NOTNULL obj, node_id_t node_id,
                void (*cb)(cbdata_call_t data, conf_object_t *obj,
                           conf_object_t *cpu, attr_value_t node_path),
                cbdata_register_t data);
        cancel_id_t (*notify_cpu_move_to)(
                conf_object_t *NOTNULL obj, node_id_t node_id,
                void (*cb)(cbdata_call_t data, conf_object_t *obj,
                           conf_object_t *cpu, attr_value_t node_path),
                cbdata_register_t data);
        cancel_id_t (*notify_event)(
                conf_object_t *NOTNULL obj, node_id_t node_id,
                const char *event_name, bool recursive,
                void (*cb)(cbdata_call_t data, conf_object_t *obj,
                           conf_object_t *cpu, node_id_t node_id,
                           const char *event_name, attr_value_t event_data),
                cbdata_register_t data);
        cancel_id_t (*notify_enable)(
                conf_object_t *NOTNULL obj,
                void (*cb)(cbdata_call_t data, conf_object_t *obj),
                cbdata_register_t data);
        cancel_id_t (*notify_disable)(
                conf_object_t *NOTNULL obj,
                void (*cb)(cbdata_call_t data, conf_object_t *obj),
                cbdata_register_t data);
        void (*cancel_notify)(conf_object_t *NOTNULL obj,
                              cancel_id_t cancel_id);
        cancel_id_t (*notify_callbacks_done)(
                conf_object_t *NOTNULL obj, uint64 node_id,
                void (*cb)(cbdata_call_t data, conf_object_t *obj),
                cbdata_register_t data);
};

#define OSA_NODE_TREE_NOTIFICATION_INTERFACE "osa_node_tree_notification"
// ADD INTERFACE osa_node_tree_notification_interface

#if defined(__cplusplus)
}
#endif

#endif  /* ! SIMICS_SIMULATOR_IFACE_OSA_NODE_TREE_INTERFACES_H */
