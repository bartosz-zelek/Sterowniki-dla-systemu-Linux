/*
  © 2016 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SAMPLE_LINUX_MAPPER_H
#define SAMPLE_LINUX_MAPPER_H

#include <simics/device-api.h>
#include <simics/simulator-iface/os-awareness-tracker-interfaces.h>

typedef struct {
        conf_object_t *obj;
        const osa_node_tree_admin_interface_t *nt_admin_iface;
        const osa_tracker_state_query_interface_t *ts_query_iface;
        const osa_tracker_state_notification_interface_t *ts_notify_iface;
} mapper_parent_t;

typedef struct {
        node_id_t root_node_id;
        node_id_t user_node_id;
        node_id_t kernel_node_id;
        node_id_t other_node_id;
        node_id_t threads_node_id;
} base_nodes_t;

typedef struct {
        conf_object_t obj;
        mapper_parent_t parent;
        conf_object_t *tracker;
        bool enabled;
        bool root_added;
        base_nodes_t base_nodes;
        ht_int_table_t entity_to_node;  /* entity_id -> node_t * */
} sample_linux_mapper_t;

typedef enum {
        Prop_Name,
        Prop_Pid,
        Prop_Tid,
        Prop_Is_Kernel,
        Prop_Active,
        Prop_Unsupported,
} prop_type_t;

typedef struct {
        char *name;
        uint64 pid;
        uint64 tid;
        bool is_kernel;
        conf_object_t *active;
        processor_mode_t mode;
} node_props_t;

typedef enum {
        Kernel_Thread,
        User_Thread,
        Process,
} node_type_t;

typedef struct {
        node_type_t type;
        node_id_t id;
        node_id_t parent_id;
} node_t;


/* called once when the device module is loaded into Simics */
void sample_linux_mapper_init_local();
void add_entities(sample_linux_mapper_t *mapper, attr_value_t entities);
void update_entity(sample_linux_mapper_t *mapper, entity_id_t entity_id,
                   attr_value_t updates);
void remove_entities(sample_linux_mapper_t *mapper, attr_value_t entities);
void update_root_property(sample_linux_mapper_t *mapper, const char *key,
                          attr_value_t value);
void clear_mapper_state(sample_linux_mapper_t *mapper);
void set_memspace(sample_linux_mapper_t *mapper, node_id_t node_id,
                  node_id_t memory_space_id);

#endif // SAMPLE_LINUX_MAPPER_H
