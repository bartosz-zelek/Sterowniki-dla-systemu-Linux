/*
  © 2016 Intel Corporation
*/

#include "empty-software-mapper.h"
#include <simics/device-api.h>
#include <simics/simulator-iface/os-awareness-tracker-interfaces.h>
#include "empty-software-mapper-admin-iface.h"
#include "empty-software-mapper-control-iface.h"
#include "empty-software-mapper-query-iface.h"
#include "empty-software-tracker-common.h"

static conf_object_t *
alloc_object(void *data)
{
        empty_software_mapper_t *mapper = MM_ZALLOC(1, empty_software_mapper_t);
        return &mapper->obj;
}

/* Initialize the object before any attributes are set. */
static void *
init_object(conf_object_t *obj, void *data)
{
        empty_software_mapper_t *mapper = (empty_software_mapper_t *)obj;
        mapper->enabled = false;
        mapper->root_added = false;
        return obj;
}

static void
update_node_property(empty_software_mapper_t *mapper, node_id_t node_id,
                     const char *key, attr_value_t value)
{
        mapper->parent.nt_admin_iface->set_property(
                mapper->parent.obj, node_id, key, value);
}

/* Update the name of the root node, all other properties are ignored. */
void
update_root_property(empty_software_mapper_t *mapper, const char *key,
                     attr_value_t value)
{
        if (strcmp(key, "name") == 0) {
                update_node_property(mapper, mapper->root_id, key, value);
        }
}

static void
update_root(empty_software_mapper_t *mapper, attr_value_t props)
{
        for (int i = 0; i < SIM_attr_dict_size(props); i++) {
                attr_value_t value = SIM_attr_dict_value(props, i);
                const char *key = SIM_attr_string(SIM_attr_dict_key(props, i));
                update_root_property(mapper, key, value);
        }
}

void
add_entities(empty_software_mapper_t *mapper, attr_value_t entities)
{
        for (int i = 0; i < SIM_attr_dict_size(entities); i++) {
                entity_id_t entity_id = SIM_attr_integer(
                        SIM_attr_dict_key(entities, i));
                if (entity_id == ROOT_ENTITY_ID) {
                        update_root(mapper, SIM_attr_dict_value(entities, i));
                } else {
                        /* TODO: If this entity should map to one or more nodes
                           in the node tree, do that here. */
                }
        }
}

static set_error_t
set_parent_attr(void *arg, conf_object_t *obj, attr_value_t *val,
                attr_value_t *idx)
{
        empty_software_mapper_t *mapper = (empty_software_mapper_t *)obj;
        conf_object_t *parent_obj = SIM_attr_object(*val);

        const osa_node_tree_admin_interface_t *nt_admin =
                SIM_c_get_interface(parent_obj, OSA_NODE_TREE_ADMIN_INTERFACE);
        if (!nt_admin) {
                SIM_attribute_error("object does not implement the "
                                    OSA_NODE_TREE_ADMIN_INTERFACE
                                    " interface");
                return Sim_Set_Interface_Not_Found;
        }

        const osa_tracker_state_query_interface_t *ts_query =
                SIM_c_get_interface(parent_obj,
                                    OSA_TRACKER_STATE_QUERY_INTERFACE);
        if (!ts_query) {
                SIM_attribute_error("object does not implement the "
                                    OSA_TRACKER_STATE_QUERY_INTERFACE
                                    " interface");
                return Sim_Set_Interface_Not_Found;
        }

        const osa_tracker_state_notification_interface_t *ts_notify =
                SIM_c_get_interface(parent_obj,
                                    OSA_TRACKER_STATE_NOTIFICATION_INTERFACE);
        if (!ts_notify) {
                SIM_attribute_error("object does not implement the "
                                    OSA_TRACKER_STATE_NOTIFICATION_INTERFACE
                                    " interface");
                return Sim_Set_Interface_Not_Found;
        }
        mapper->parent.obj = parent_obj;
        mapper->parent.nt_admin_iface = nt_admin;
        mapper->parent.ts_query_iface = ts_query;
        mapper->parent.ts_notify_iface = ts_notify;
        return Sim_Set_Ok;
}

static attr_value_t
get_parent_attr(void *arg, conf_object_t *obj, attr_value_t *idx)
{
        empty_software_mapper_t *mapper = (empty_software_mapper_t *)obj;
        return SIM_make_attr_object(mapper->parent.obj);
}

static set_error_t
set_tracker_attr(void *arg, conf_object_t *obj, attr_value_t *val,
                 attr_value_t *idx)
{
        empty_software_mapper_t *mapper = (empty_software_mapper_t *)obj;
        mapper->tracker = SIM_attr_object(*val);

        return Sim_Set_Ok;
}

static attr_value_t
get_tracker_attr(void *arg, conf_object_t *obj, attr_value_t *idx)
{
        empty_software_mapper_t *mapper = (empty_software_mapper_t *)obj;
        return SIM_make_attr_object(mapper->tracker);
}

static attr_value_t
get_enabled_attribute(void *arg, conf_object_t *obj, attr_value_t *idx)
{
        empty_software_mapper_t *mapper = (empty_software_mapper_t *)obj;
        return SIM_make_attr_boolean(mapper->enabled);
}

static attr_value_t
get_root_node_attribute(void *arg, conf_object_t *obj, attr_value_t *idx)
{
        empty_software_mapper_t *mapper = (empty_software_mapper_t *)obj;
        if (!mapper->root_added)
                return SIM_make_attr_nil();
        return SIM_make_attr_uint64(mapper->root_id);
}

static set_error_t
set_root_node_attribute(void *arg, conf_object_t *obj, attr_value_t *val,
                        attr_value_t *idx)
{
        empty_software_mapper_t *mapper = (empty_software_mapper_t *)obj;
        if (SIM_attr_is_nil(*val)) {
                mapper->root_added = false;
        } else {
                mapper->root_added = true;
                mapper->root_id = SIM_attr_integer(*val);
        }
        return Sim_Set_Ok;
}

void
clear_mapper_state(empty_software_mapper_t *mapper)
{
        mapper->root_added = false;
}

/* Cleanup of deleted object instance */
static int
delete_instance(conf_object_t *obj)
{
        MM_FREE(obj);
        return 0;
}

void
empty_software_mapper_init_local()
{
        const class_data_t mapper_funcs = {
                .alloc_object = alloc_object,
                .init_object = init_object,
                .delete_instance = delete_instance,
                .class_desc = "mapper skeleton",
                .description = "Skeleton for creating a mapper."

        };
        conf_class_t *cls = SIM_register_class("empty_software_mapper",
                                               &mapper_funcs);

        register_mapper_control_iface(cls);
        register_mapper_query_iface(cls);
        register_mapper_admin_iface(cls);

        SIM_register_typed_attribute(
                cls, "parent",
                get_parent_attr, NULL, set_parent_attr, NULL,
                Sim_Attr_Required, "o", NULL,
                "The parent object. Must implement the"
                " " OSA_NODE_TREE_ADMIN_INTERFACE
                ", " OSA_TRACKER_STATE_QUERY_INTERFACE
                ", and " OSA_TRACKER_STATE_NOTIFICATION_INTERFACE
                " interfaces");

        SIM_register_typed_attribute(
                cls, "tracker",
                get_tracker_attr, NULL, set_tracker_attr, NULL,
                Sim_Attr_Required, "o", NULL, "Tracker associated with mapper");

        SIM_register_typed_attribute(
                cls, "enabled",
                get_enabled_attribute, NULL, NULL, NULL,
                Sim_Attr_Pseudo, "b", NULL, "Mapper enabled - read only");

        SIM_register_typed_attribute(
                cls, "root_node",
                get_root_node_attribute, NULL, set_root_node_attribute, NULL,
                Sim_Attr_Optional | Sim_Attr_Internal, "i|n", NULL,
                "Root node in node tree, nil if no root node has been added");
}
