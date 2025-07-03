/*
  © 2010 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_MODEL_IFACE_COMPONENTS_H
#define SIMICS_MODEL_IFACE_COMPONENTS_H

#include <simics/base/types.h>
#include <simics/base/attr-value.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/*
   <add id="component_interface_t">
   <name index="true">component_interface_t</name>

   All component classes must implement the <iface>component</iface>
   interface. All functions in the interface must be implemented.

   The <fun>pre_instantiate</fun> function is called before the
   component is instantiated. The function returns <tt>true</tt> if the
   component can be instantiated, or <tt>false</tt> if not.

   The component might need to do some extra work after the component
   has been instantiated. This should be done when called via the
   <fun>post_instantiate</fun> function.

   The <fun>create_cell</fun> function returns <tt>true</tt> if the
   configuration system can create a default cell object for the
   component, or <tt>false</tt> if not. Both
   <fun>pre_instantiate</fun> and <fun>create_cell</fun> typically
   return <tt>true</tt>.

   Component has slots. A slot has key and value. The key is the slot
   name as a string. The value is a conf object, a pre conf object, or
   None, or nested lists of such types.

   Slots are either defined in the component or added after the
   component has been created. Slots defined in the component are
   static slots which can not be deleted, but the slot value can be
   changed. Slots added to the component after creation are
   dynamic slots and they can be removed when wanted.

   The <fun>get_slots</fun> function returns a dictionary with slot
   names as dictionary keys and slot values as dictionary values.

   The <fun>get_slot_objects</fun> function returns a list of all conf
   objects and pre conf objects extracted from all slot values.

   The <fun>get_slot_value</fun> returns the slot value. The slot name
   is passed as <arg>slot</arg> argument. A slot value is set using
   the <fun>set_slot_value</fun> function. The <arg>value</arg>
   argument should be a conf object, pre conf object, or None, or
   nested lists of such types. The get function returns NULL on failure.
   The set function does not return anything to indicate failure.

   The <fun>has_slot</fun> function returns <tt>true</tt> if the
   <arg>slot</arg> exists, otherwise <tt>false</tt>. The slot can
   either be a static slot or a dynamic slot. The <fun>add_slot</fun>
   function adds the slot named <arg>slot</arg>. Adding a slot can
   fail if the slot already exist. The added slot will be a dynamic
   slot. A dynamic slot can be deleted. The <fun>del_slot</fun>
   function deletes a dynamic slot. Deleting a slot will fail if the
   slot does not exist or if the slot is static. Both
   <fun>add_slot</fun> and <fun>del_slot</fun> returns <tt>true</tt>
   on success or <tt>false</tt> on failure.

   <insert-until text="// ADD INTERFACE component_interface"/>
   </add>

   <add id="component_interface_exec_context">
   <table border="false">
   <tr><td><fun>post_instantiate</fun></td><td>Global Context
           </td></tr>
   <tr><td><fun>pre_instantiate</fun></td><td>Global Context
           </td></tr>
   <tr><td><fun>create_cell</fun></td><td>Global Context</td></tr>
   <tr><td><fun>get_slots</fun></td><td>Cell Context</td></tr>
   <tr><td><fun>get_objects</fun></td><td>Cell Context</td></tr>
   <tr><td><fun>get_slot_value</fun></td><td>Global Context</td></tr>
   <tr><td><fun>set_slot_value</fun></td><td>Global Context</td></tr>
   <tr><td><fun>add_slot</fun></td><td>Global Context</td></tr>
   <tr><td><fun>del_slot</fun></td><td>Global Context</td></tr>
   </table>
   </add>
*/
SIM_INTERFACE(component) {
        bool (*pre_instantiate)(conf_object_t *obj);
        void (*post_instantiate)(conf_object_t *obj);
        bool (*create_cell)(conf_object_t *obj);

        attr_value_t (*get_slots)(conf_object_t *obj);
        attr_value_t (*get_slot_objects)(conf_object_t *obj);

        attr_value_t (*get_slot_value)(conf_object_t *obj,
                                 const char *NOTNULL slot);
        void (*set_slot_value)(conf_object_t *obj,
                         const char *NOTNULL slot,
                         attr_value_t value);

        bool (*has_slot)(conf_object_t *obj,
                         const char *NOTNULL slot);
        bool (*add_slot)(conf_object_t *obj,
                         const char *NOTNULL slot);
        bool (*del_slot)(conf_object_t *obj,
                         const char *NOTNULL slot);
};
#define COMPONENT_INTERFACE "component"
// ADD INTERFACE component_interface

/*
  <add id="connector_interface_t">
   <name index="true">connector_interface_t</name>

   The <iface>connector</iface> interface must be implemented by
   connector objects. The interface describes the connector and how it
   can be connected to other connectors. A connector is used for
   connecting component objects. The connector is just a proxy for the
   connection, the connector uses the <iface>component_connector</iface>
   interface in the components to setup the connection.

   The <fun>type</fun> function returns the connector type as a
   string. Two connectors can only be connected if the type is
   identical.

   A hotpluggable connector returns <tt>true</tt> when calling the
   <fun>hotpluggable</fun> function, otherwise <tt>false</tt>. A
   hotpluggable connector can be connected before or after the
   component is instantiated, an non hotpluggable connector must be
   connected before the component is instantiated.

   The <fun>required</fun> function returns <tt>true</tt> if the
   connector must be connected before instantiated, otherwise
   <tt>false</tt>.

   A connector can be connected to many connectors, but it is only
   supported if the <fun>multi</fun> function return <tt>true</tt>. It
   is not recommended that a connector support multi connections. A
   component can instead dynamically create connectors when needed.

   A connector has a direction; <em>up</em>, <em>down</em> or
   <em>any</em>. The direction decides in which order the connectors
   in a component tree are connected and the structure of the tree.

   Connections are setup in two steps. The first step is to add the
   connection, which is done using the <fun>add_destination</fun>
   function. Adding a connection can fail for several reasons and it
   is implementation dependent how the connection can fail. A
   connection can for instance fail because the destination object
   requires something that the source component did not pass when
   checking the connection. Just adding a connection does not mean
   that the components actually connect. The components have to save
   the data given with the <fun>add_destination</fun> function. The
   actual setup of the connection is made in the second step when the
   <fun>update</fun> function is called, which finalizes the
   connection.

   The <fun>add_destination</fun> and <fun>remove_destination</fun>
   functions sets the state of the connection. It is first when the
   <fun>update</fun> function is called when the connection is
   finalized. Both <fun>add_destination</fun> and
   <fun>remove_destination</fun> returns <tt>true</tt> if the call was
   successful otherwise they return <tt>false</tt>.

   Parameters to the <fun>add_destination</fun> function are the own
   object <param>obj</param> and the destination object
   <param>dst_obj</param>. The destination object must be a port
   object and it must implement the <iface>connector</iface>
   interface.

   The <fun>update</fun> function is called when the component should
   update its current connection status. The status of the connection
   has previously been set using the <fun>add_destination</fun>
   or/and <fun>remove_destination</fun> functions.

   The <fun>destination</fun> function returns a list of port objects
   that the its connected to. The <fun>destination</fun> function
   returns the state of the connection not the finalized state,
   i.e. the state of the connection before <fun>update</fun>
   functional call.

   The <fun>check</fun>, <fun>connect</fun>, and <fun>disconnect</fun>
   functions initiates a connection setup via the connector object. The
   connector will forward the setup to the components affected by the
   connection via the <iface>component_connector</iface> interface.

   The <fun>deletion_requested</fun> function is called after disconnecting
   components. A dynamic connector might want to return True in order to be
   deleted just after the disconnection.

   <insert-until text="// ADD INTERFACE connector_interface"/>
   </add>

   <add id="connector_interface_exec_context">
   Global Context for all methods.
   </add>
 */
typedef enum {
        Sim_Connector_Direction_Up,
        Sim_Connector_Direction_Down,
        Sim_Connector_Direction_Any
} connector_direction_t;

SIM_INTERFACE(connector) {
        char *(*type)(conf_object_t *obj);
        bool (*hotpluggable)(conf_object_t *obj);
        bool (*required)(conf_object_t *obj);
        bool (*multi)(conf_object_t *obj);
        connector_direction_t (*direction)(conf_object_t *obj);

        bool (*add_destination)(conf_object_t *obj, conf_object_t *connector);
        bool (*remove_destination)(conf_object_t *obj,
                                   conf_object_t *connector);
        attr_value_t (*destination)(conf_object_t *obj); /* list of
                                                            connector objects */
        void (*update)(conf_object_t *obj);

        bool (*check)(conf_object_t *obj, attr_value_t attr);
        void (*connect)(conf_object_t *obj, attr_value_t attr);
        void (*disconnect)(conf_object_t *obj);
        bool (*deletion_requested)(conf_object_t *obj);
};
#define CONNECTOR_INTERFACE "connector"
// ADD INTERFACE connector_interface

/*
  <add id="component_connector_interface_t">
  <name index="true">component_connector_interface_t</name>

  The <iface>component_connector</iface> is implemented by components
  that use connector objects for handling connections between components.

  The connection setup is made in two stages, the check stage and the
  connect stage. The check stage is often not needed, but it can be
  used to make sure that the later connect step will not fail. Each
  connection is handled by a connector object. The connector object
  will both handle the connection in both direction, i.e. sending
  connect information and receiving connector information. Two
  components that should be connected must implement one connector
  object each.

  The <fun>get_check_data</fun> and <fun>get_connect_data</fun> will
  be called from the connector object to get connection data to send
  to the other part of the connection, i.e. to the destination. The
  data sent must be an <type>attr_value_t</type> type.

  The <fun>check</fun>, <fun>connect</fun>, and <fun>disconnect</fun>
  functions are called from the connector object when another
  connector wants to connect to this connection. The connection data
  is passed as the <arg>attr</arg> argument.

  <insert-until text="// ADD INTERFACE component_connector_interface"/>
  </add>

  <add id="component_connector_interface_exec_context">
  Global Context for all methods.
  </add>
*/
SIM_INTERFACE(component_connector) {
        attr_value_t (*get_check_data)(conf_object_t *obj,
                                       conf_object_t *NOTNULL connector);
        attr_value_t (*get_connect_data)(conf_object_t *obj,
                                         conf_object_t *NOTNULL connector);
        bool (*check)(conf_object_t *obj, conf_object_t *NOTNULL connector,
                      attr_value_t attr);
        void (*connect)(conf_object_t *obj, conf_object_t *NOTNULL connector,
                        attr_value_t attr);
        void (*disconnect)(conf_object_t *obj,
                           conf_object_t *NOTNULL connector);
};

#define COMPONENT_CONNECTOR_INTERFACE "component_connector"
// ADD INTERFACE component_connector_interface

/*
   <add id="disk_component_interface_t">

   The <iface>disk_component</iface> interface is implemented by components
   that provide disk storage. The <fun>size()</fun> member function should
   return the total disk size provided by the component, once configured.

   <insert-until text="// ADD INTERFACE disk_component_interface_t"/>
   </add>

   <add id="disk_component_interface_exec_context">
   Global Context for all methods.
   </add>
*/

SIM_INTERFACE(disk_component) {
        uint64 (*size)(conf_object_t *obj);
};
#define DISK_COMPONENT_INTERFACE "disk_component"
// ADD INTERFACE disk_component_interface_t

#if defined(__cplusplus)
}
#endif

#endif
