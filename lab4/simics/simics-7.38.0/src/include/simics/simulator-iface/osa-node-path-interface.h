/*
  © 2017 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SIMULATOR_IFACE_OSA_NODE_PATH_INTERFACE_H
#define SIMICS_SIMULATOR_IFACE_OSA_NODE_PATH_INTERFACE_H

#include <simics/device-api.h>
#include "osa-types.h"

#if defined(__cplusplus)
extern "C" {
#endif


/*  <add id="osa_node_path_interface_t">
    <name>osa_node_path_interface_t</name>
   <ndx>osa_node_path_interface_t</ndx>

   <ndx>matching_nodes!osa_node_path interface method</ndx>
   <fun>matching_nodes</fun> function returns a list of all nodes rooted at
   <arg>root_id</arg> that matches the given <arg>node_path_pattern</arg>. The
   node_path_pattern should be either an integer representing a node id or a
   string, see <cite>Analyzer User's Guide</cite> for more details.

   Upon success, the return value is a list where the first entry is true and
   the second entry is a list containing all the matching node ids.

   Upon failure, the return value is a list where the first entry is false and
   the second entry is a string describing the error.

   <ndx>node_path!osa_node_path interface method</ndx> <fun>node_path</fun>
   function translates a node id into a fully qualified node path string. See
   <cite>Analyzer User's Guide</cite> for more details.

   Upon success, the return value is a list where the first entry is true and
   the second entry is the node path string.

   Upon failure, the return value is a list where the first entry is false and
   the second entry is a string describing the error.

   <insert-until text="// ADD INTERFACE osa_node_path_interface"/>
   </add>

   <add id="osa_node_path_interface_exec_context">
   Global Context for all methods.
   </add>
*/

SIM_INTERFACE(osa_node_path) {
        attr_value_t (*matching_nodes)(conf_object_t *NOTNULL obj,
                                       node_id_t root_id,
                                       attr_value_t node_path_pattern);
        attr_value_t (*node_path)(conf_object_t *NOTNULL obj,
                                  node_id_t node_id);
};

#define OSA_NODE_PATH_INTERFACE "osa_node_path"
// ADD INTERFACE osa_node_path_interface

#if defined(__cplusplus)
}
#endif

#endif  /* SIMICS_SIMULATOR_IFACE_OSA_NODE_PATH_INTERFACE_H */
