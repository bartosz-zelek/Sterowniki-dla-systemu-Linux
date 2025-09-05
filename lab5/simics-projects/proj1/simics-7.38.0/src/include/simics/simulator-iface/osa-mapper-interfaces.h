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

#ifndef SIMICS_SIMULATOR_IFACE_OSA_MAPPER_INTERFACES_H
#define SIMICS_SIMULATOR_IFACE_OSA_MAPPER_INTERFACES_H

#include <simics/device-api.h>
#include <simics/pywrap.h>
#include "osa-types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* <add id="osa_mapper_admin_interface_t">
   <name>osa_mapper_admin_interface_t</name>
   <ndx>osa_mapper_admin_interface_t</ndx>

   <ndx>tracker_updated!osa_mapper_admin interface method</ndx>
   <fun>tracker_updated</fun> is called from the node tree when entities are
   added, modified or removed. To receive such an update, the mapper must
   subscribe to tracker updates by calling <fun>subscribe_tracker</fun> in
   the <iface>osa_tracker_state_notification</iface> interface. The
   <arg>initiator</arg> argument is the processor object that initiated the
   transaction. This can be nil if the transaction was not initiated by a
   processor (for example, enable or disable tracker). The format of
   <arg>changeset</arg> is a dictionary with tracker objects that were updated
   as keys and other dictionaries as values.

   The dictionary that is set as the value for each tracker contains the
   following keywords: <arg>"added"</arg>, <arg>"modified"</arg>,
   <arg>"removed"</arg> and <arg>"events"</arg>.

   The value for <arg>"added"</arg> is a dictionary which contains the added
   entity IDs as keys and the attributes of those entities as values.

   An example of <arg>"added"</arg> where two entities were added:
   <pre size="smaller">"added": {0x1000: {"name": "task1", "pid": 1},
          0x2000: {"name": "task2", "pid": 2}}</pre>

   The value for <arg>"modified"</arg> is a dictionary which contains the
   modified entity IDs as keys and a dictionary with the attribute name
   as key and a list of old and new values for that attribute as its value.
   Only the attributes that have changed will be included. If an attribute
   is removed that will be set so that it changed to nil. If an attribute was
   added that will be set so that it changed from nil.

   An example of <arg>"modified"</arg>, entity 0x1000 had <arg>"name"</arg>
   changed and entity 0x2000 had <arg>"tgid"</arg> added and <arg>"pid"</arg>
   removed:
   <pre size="smaller">"modified": {0x1000: {"name": ["task1", "new_name"]},
             0x2000: {"tgid": [None, 4], "pid": [2, None]}}</pre>

   The value for <arg>"removed"</arg> is a list which contains the entity IDs
   for the entities that have been removed.

   The value for <arg>"events"</arg> is a dictionary indexed by the entity
   id. Each value is a list of lists, each inner list is an event. The first
   element of the inner list is the event name and the second element is user
   defined data associated with the event.

   An example of <arg>"events"</arg> where entity 0x1000 has two associated
   events and entity 0x2000 has one associated event:
   <pre size="smaller">"events": {0x1000: [["event1", {}], ["event2", {}]],
           0x2000: [["syscall", {'sys_num': 0x10, 'sys_name': "open"}]]}</pre>

   An example of how the complete <arg>changeset</arg> might look like:
   <pre size="smaller">{&lt;the tracker 'tracker1_obj'&gt;: {"added": {0x1000: {"name": "task"}},
                                "modified": {0x2000: {"dummy": [1, 2]}},
                                "removed": [0x3000, 0x3100]},}</pre>

   <insert-until text="// ADD INTERFACE osa_mapper_admin_interface"/>
   </add>

   <add id="osa_mapper_admin_interface_exec_context">
   Cell Context for all methods.
   </add>
*/
SIM_INTERFACE(osa_mapper_admin) {
        void (*tracker_updated)(conf_object_t *NOTNULL obj,
                                conf_object_t *initiator,
                                attr_value_t changeset);
};
#define OSA_MAPPER_ADMIN_INTERFACE "osa_mapper_admin"
// ADD INTERFACE osa_mapper_admin_interface

/* <add id="osa_mapper_control_interface_t">
   <name>osa_mapper_control_interface_t</name>
   <ndx>osa_mapper_control_interface_t</ndx>

   <ndx>disable!osa_mapper_control interface method</ndx> The
   <fun>disable</fun> function is called when the mapper should be
   disabled. The mapper should then clean-up the node tree and stop listening
   to changes from trackers.

   <ndx>enable!osa_mapper_control interface method</ndx> The
   <fun>enable</fun> function is called when the mapper should be enabled. The
   mapper should then create the node tree and start listening to changes from
   trackers.

   <ndx>clear_state!osa_mapper_control interface method</ndx> The
   <fun>clear_state</fun> is called to clear the mapper's state. The mapper
   should clear all its internal data when this is called, so that
   <fun>enable</fun> can be called again. This call can only occur while
   the mapper is disabled.

   <insert-until text="// ADD INTERFACE osa_mapper_control_interface"/>
   </add>

   <add id="osa_mapper_control_interface_exec_context">
   Cell Context for all methods.
   </add>

*/
SIM_INTERFACE(osa_mapper_control) {
        void (*disable)(conf_object_t *NOTNULL obj);
        bool (*enable)(conf_object_t *NOTNULL obj);
        void (*clear_state)(conf_object_t *NOTNULL obj);
};
#define OSA_MAPPER_CONTROL_INTERFACE "osa_mapper_control"
// ADD INTERFACE osa_mapper_control_interface

/* <add id="osa_mapper_query_interface_t">
   <name>osa_mapperquery_interface_t</name>
   <ndx>osa_mapper_query_interface_t</ndx>

   <ndx>get_process_list!osa_mapper_query interface method</ndx> The optional
   <fun>get_process_list</fun> function provides data for the 'list'
   command. It should return a two-element list, where the first element is a
   list of column headers, and the second element is a list of (row, subtable)
   two-element lists. All rows should have the same number of elements as the
   header list (this is the number of columns in the resulting table).

   The elements of the header list and row lists---that is, the individual
   elements in the table---should be of type string or integer. Integers will
   be formatted by the system, so in order to force a specific base, such as
   decimal or hexadecimal, convert them to strings.

   The subtables paired with each row should either be None (meaning no
   subtable) or a nested list such as that returned by get_process_list(), in
   which case that list will be printed, slightly indented, below the row. This
   makes it possible for the list command to handle stacked trackers. An
   example of how a complete return value from <fun>get_process_list</fun>
   function can look like:

   <pre size="smaller">[["Process", "Pid"], [[["ls", 1], None], [["cat", 2], None]]]</pre>

   If the function is not implemented, the function pointer should be set to
   NIL.

   <ndx>get_mapper!osa_mapper_query interface method</ndx> The optional
   <fun>get_mapper</fun> function returns the mapper that is responsible for
   the given node. A mapper that has guest mappers should forward the request
   to the guests as well if the node belongs to one of the guests. If the
   function is not implemented by the mapper, it is assumed that the node is
   owned by the mapper.

   If this function is not implemented, the function pointer should be set to
   NIL. Stacked trackers, which support guest trackers must implement this
   function.

   <insert-until text="// ADD INTERFACE osa_mapper_query_interface"/>
   </add>

   <add id="osa_mapper_query_interface_exec_context">
   Global Context for all methods.
   </add>
*/
SIM_INTERFACE(osa_mapper_query) {
        attr_value_t (*get_process_list)(conf_object_t *NOTNULL obj);
        conf_object_t *(*get_mapper)(conf_object_t *NOTNULL obj,
                                     node_id_t node_id);
};
#define OSA_MAPPER_QUERY_INTERFACE "osa_mapper_query"
// ADD INTERFACE osa_mapper_query_interface

#ifdef __cplusplus
}
#endif

#endif /* ! SIMICS_SIMULATOR_IFACE_OSA_MAPPER_INTERFACES_H */
