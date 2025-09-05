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

#ifndef SIMICS_SIMULATOR_IFACE_OSA_TRACKER_STATE_INTERFACES_H
#define SIMICS_SIMULATOR_IFACE_OSA_TRACKER_STATE_INTERFACES_H

#include <simics/device-api.h>
#include "osa-types.h"

#if defined(__cplusplus)
extern "C" {
#endif


/* <add id="osa_tracker_state_admin_interface_t">
   <name>osa_tracker_state_admin_interface_t</name>
   <ndx>osa_tracker_state_admin_interface_t</ndx>

   <ndx>begin!osa_tracker_state_admin interface method</ndx> <fun>begin</fun>
   is called from the tracker to start a transaction when modifying an entity in
   the node tree. The <arg>tracker</arg> argument specifies the tracker object
   for which the updates are done for.  The <arg>initiator</arg> argument
   specifies the processor that initiated the transaction, this can be nil if
   it was not a processor that caused the transaction to begin, when enabling
   the tracker as an example. The <arg>initiator</arg> is passed as an argument
   to the <fun>tracker_updated</fun> function in the
   <iface>osa_mapper_admin</iface> interface. The function returns an ID that
   is used when calling <fun>end</fun>.

   <ndx>end!osa_tracker_state_admin interface method</ndx> <fun>end</fun> is
   called from the tracker to end a transaction. This should be called at the
   end of a transaction when all entity modifications are done. The
   <arg>transaction_id</arg> argument should be the value returned from
   <fun>begin</fun> that started the transaction.  Stacked calls to
   <fun>begin</fun> and <fun>end</fun> are possible, then the transaction will
   be ended when the first <fun>begin</fun> is ended. The <fun>begin</fun>
   methods must be ended in the opposite order that they were called. For
   stacked calls to <fun>begin</fun> only the initiator of the first call to
   begin will be used.

   <ndx>add!osa_tracker_state_admin interface method</ndx>
   <fun>add</fun> adds a new entity with ID <arg>entity_id</arg> to the node
   tree. The new entity's attributes are set in the <arg>attributes</arg>
   argument, which is a dictionary.

   <ndx>remove!osa_tracker_state_admin interface method</ndx>
   <fun>remove</fun> removes the entity with ID <arg>entity_id</arg> for the
   current tracker from the node tree.

   <ndx>remove_all!osa_tracker_state_admin interface method</ndx>
   <fun>remove_all</fun> removes all entities for the current tracker from
   the node tree.

   <ndx>set_attribute!osa_tracker_state_admin interface method</ndx>
   <fun>set_attribute</fun> adds or updates an attribute <arg>key</arg> for
   the entity with ID <arg>entity_id</arg>. The new value is specified in
   <arg>val</arg>.

   <ndx>update!osa_tracker_state_admin interface method</ndx>
   <fun>update</fun> updates or adds one or more attributes for the entity with
   ID <arg>entity_id</arg>. The <arg>attributes</arg> argument is a dictionary
   with the attributes to be updated.

   In order to remove a property, set the property value to nil when calling
   <fun>update</fun> or <fun>set_attribute</fun>.

   <ndx>event!osa_tracker_state_admin interface method</ndx>
   <fun>event</fun> registers a new event, associated with the given
   <arg>entity_id</arg> argument. The <arg>event_name</arg> argument is the
   name of the event. The <arg>event_data</arg> argument is the data associated
   with the event and it is up to the responsible tracker to document its exact
   form. An event differes from other properties in the way that they are not
   persistent.

   Entity attributes have the limitation that the value of the attribute can
   not be of type data or dictionary. The keys of entity attributes must be of
   string type.

   <insert-until text="// ADD INTERFACE osa_tracker_state_admin_interface"/>
   </add>

   <add id="osa_tracker_state_admin_interface_exec_context">
   Cell Context for all methods.
   </add>
*/
SIM_INTERFACE(osa_tracker_state_admin) {
        transaction_id_t (*begin)(conf_object_t *NOTNULL obj,
                                  conf_object_t *NOTNULL tracker,
                                  conf_object_t *initiator);
        void (*end)(conf_object_t *NOTNULL obj, transaction_id_t txid);
        void (*add)(conf_object_t *NOTNULL obj, entity_id_t entity_id,
                    attr_value_t attributes);
        void (*remove)(conf_object_t *NOTNULL obj, entity_id_t entity_id);
        void (*remove_all)(conf_object_t *NOTNULL obj);
        void (*set_attribute)(conf_object_t *NOTNULL obj, entity_id_t entity_id,
                              const char *key, attr_value_t val);
        void (*update)(conf_object_t *NOTNULL obj, entity_id_t entity_id,
                       attr_value_t attributes);
        void (*event)(conf_object_t *NOTNULL obj, entity_id_t entity_id,
                      const char *event_name, attr_value_t event_data);
};

#define OSA_TRACKER_STATE_ADMIN_INTERFACE "osa_tracker_state_admin"
// ADD INTERFACE osa_tracker_state_admin_interface


/* <add id="osa_tracker_state_notification_interface_t">
   <name>osa_tracker_state_notification_interface_t</name>
   <ndx>osa_tracker_state_notification_interface_t</ndx>

   <ndx>subscribe_tracker!osa_tracker_state_notification interface method</ndx>
   <fun>subscribe_tracker</fun> is called to make <arg>mapper</arg> receive
   updates for entities of <arg>tracker</arg>. When such an update occurs, the
   function <fun>tracker_updated</fun> in interface
   <iface>osa_mapper_admin</iface> will be called.

   <ndx>unsubscribe_tracker!osa_tracker_state_notification interface method</ndx>
   <fun>unsubscribe_tracker</fun> cancels a subscription of entity
   updates to <arg>mapper></arg> that was started by
   <fun>subscribe_tracker</fun> for the specified <arg>tracker</arg>. A tracker
   without guest trackers does not need to call this, as it will be
   automatically done when the framework is disabled. However, a tracker with
   guest trackers, must call this function when a guest is removed.

   <insert-until
    text="// ADD INTERFACE osa_tracker_state_notification_interface"/>
    </add>

   <add id="osa_tracker_state_notification_interface_exec_context">
   Cell Context for all methods.
   </add>
*/

SIM_INTERFACE(osa_tracker_state_notification) {
        void (*subscribe_tracker)(conf_object_t *NOTNULL obj,
                                  conf_object_t *NOTNULL mapper,
                                  conf_object_t *NOTNULL tracker);
        void (*unsubscribe_tracker)(conf_object_t *NOTNULL obj,
                                    conf_object_t *NOTNULL mapper,
                                    conf_object_t *NOTNULL tracker);
};

#define OSA_TRACKER_STATE_NOTIFICATION_INTERFACE \
        "osa_tracker_state_notification"
// ADD INTERFACE osa_tracker_state_notification_interface


/* <add id="osa_tracker_state_query_interface_t">
   <name>osa_tracker_state_query_interface_t</name>
   <ndx>osa_tracker_state_query_interface_t</ndx>

   <ndx>get_entities!osa_tracker_state_query interface method</ndx>
   <fun>get_entities</fun> returns a dictionary of entities that are stored for
   <arg>tracker</arg>. The dictionary maps entity id to properties, which in
   turn is a dictionary, mapping property name to property value. Returns nil
   if <arg>tracker</arg> is not known.

   <ndx>get_entity!osa_tracker_state_query interface method</ndx>
   <fun>get_entity</fun> returns the properties for the given entity
   <arg>id</arg> and <arg>tracker</arg>, as a dictionary that maps property
   name to property value. Returns nil if the entity can not be found.

   <insert-until text="// ADD INTERFACE osa_tracker_state_query_interface"/>
   </add>

   <add id="osa_tracker_state_query_interface_exec_context">
   Cell Context for all methods.
   </add>
*/

SIM_INTERFACE(osa_tracker_state_query) {
        attr_value_t (*_deprecated)(conf_object_t *NOTNULL obj);
        attr_value_t (*get_entities)(conf_object_t *NOTNULL obj,
                                     conf_object_t *NOTNULL tracker);
        attr_value_t (*get_entity)(conf_object_t *NOTNULL obj,
                                   conf_object_t *NOTNULL tracker,
                                   entity_id_t id);
};

#define OSA_TRACKER_STATE_QUERY_INTERFACE "osa_tracker_state_query"
// ADD INTERFACE osa_tracker_state_query_interface

#if defined(__cplusplus)
}
#endif

#endif  /* ! SIMICS_SIMULATOR_IFACE_OSA_TRACKER_STATE_INTERFACES_H */
