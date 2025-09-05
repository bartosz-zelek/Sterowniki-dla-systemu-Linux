/*
  © 2016 Intel Corporation
*/

#ifndef EMPTY_SOFTWARE_MAPPER_H
#define EMPTY_SOFTWARE_MAPPER_H

#include <simics/device-api.h>
#include <simics/simulator-iface/os-awareness-tracker-interfaces.h>

typedef struct {
        conf_object_t *obj;
        const osa_node_tree_admin_interface_t *nt_admin_iface;
        const osa_tracker_state_query_interface_t *ts_query_iface;
        const osa_tracker_state_notification_interface_t *ts_notify_iface;
} mapper_parent_t;

typedef struct {
        conf_object_t obj;
        mapper_parent_t parent;
        conf_object_t *tracker;
        bool enabled;
        bool root_added;
        node_id_t root_id;
} empty_software_mapper_t;

/* called once when the device module is loaded into Simics */
void empty_software_mapper_init_local();
void add_entities(empty_software_mapper_t *mapper, attr_value_t entities);
void update_root_property(empty_software_mapper_t *mapper, const char *key,
                          attr_value_t value);
void clear_mapper_state(empty_software_mapper_t *mapper);

#endif // EMPTY_SOFTWARE_MAPPER_H
