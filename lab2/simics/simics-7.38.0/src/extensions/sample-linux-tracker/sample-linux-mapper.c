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

#include "sample-linux-mapper.h"
#include <simics/device-api.h>
#include <simics/simulator-iface/os-awareness-tracker-interfaces.h>
#include "sample-linux-mapper-admin-iface.h"
#include "sample-linux-mapper-control-iface.h"
#include "sample-linux-mapper-query-iface.h"
#include "sample-linux-tracker-common.h"

static conf_object_t *
alloc_object(void *data)
{
        sample_linux_mapper_t *mapper = MM_ZALLOC(1, sample_linux_mapper_t);
        return &mapper->obj;
}

/* Initialize the object before any attributes are set. */
static void *
init_object(conf_object_t *obj, void *data)
{
        sample_linux_mapper_t *mapper = (sample_linux_mapper_t *)obj;
        mapper->enabled = false;
        mapper->root_added = false;
        ht_init_int_table(&mapper->entity_to_node);
        return obj;
}

static void
update_node_property(sample_linux_mapper_t *mapper, node_id_t node_id,
                     const char *key, attr_value_t value)
{
        mapper->parent.nt_admin_iface->set_property(
                mapper->parent.obj, node_id, key, value);
}

typedef struct {
        const char *name;
        prop_type_t type;
} prop_type_map_t;

static const prop_type_map_t prop_type_map[] = {
        {"name", Prop_Name}, {"pid", Prop_Pid}, {"tid", Prop_Tid},
        {"is_kernel", Prop_Is_Kernel}, {"cpu", Prop_Active}
};

static prop_type_t
prop_name_to_type(const char *prop_name)
{
        for (int i = 0; i < ALEN(prop_type_map); i++) {
                const prop_type_map_t prop_map = prop_type_map[i];
                if (strcmp(prop_name, prop_map.name) == 0)
                        return prop_map.type;
        }
        return Prop_Unsupported;
}

static void
update_root(sample_linux_mapper_t *mapper, attr_value_t props)
{
        for (int i = 0; i < SIM_attr_dict_size(props); i++) {
                attr_value_t value = SIM_attr_dict_value(props, i);
                const char *key = SIM_attr_string(SIM_attr_dict_key(props, i));
                update_root_property(mapper, key, value);
        }
}

static node_props_t *
entity_props_to_node_props(attr_value_t props)
{
        node_props_t *node_props = MM_ZALLOC(1, node_props_t);
        for (int i = 0; i < SIM_attr_dict_size(props); i++) {
                const char *prop_name = SIM_attr_string(
                        SIM_attr_dict_key(props, i));
                attr_value_t value = SIM_attr_dict_value(props, i);
                prop_type_t type = prop_name_to_type(prop_name);
                switch (type) {
                case Prop_Name:
                        node_props->name = MM_STRDUP(SIM_attr_string(value));
                        break;
                case Prop_Pid:
                        node_props->pid = SIM_attr_integer(value);
                        break;
                case Prop_Tid:
                        node_props->tid = SIM_attr_integer(value);
                        break;
                case Prop_Is_Kernel:
                        node_props->is_kernel = SIM_attr_boolean(value);
                        break;
                case Prop_Active:
                        node_props->active = SIM_attr_object(
                                SIM_attr_list_item(value, 0));
                        node_props->mode = SIM_attr_integer(
                                SIM_attr_list_item(value, 1));
                        break;
                case Prop_Unsupported:
                        break;
                }
        }
        return node_props;
}

static void
append_to_dict(attr_value_t *dict, const char *key, attr_value_t value)
{
        unsigned new_size = SIM_attr_dict_size(*dict) + 1;
        unsigned last_pos = new_size - 1;
        SIM_attr_dict_resize(dict, new_size);
        SIM_attr_dict_set_item(dict, last_pos, SIM_make_attr_string(key),
                               value);
}

static attr_value_t
node_props_to_attr_val(node_props_t *node_props, bool include_pid,
                       bool include_tid, bool multiprocessor)
{
        attr_value_t props = SIM_alloc_attr_dict(2);
        SIM_attr_dict_set_item(&props, 0, SIM_make_attr_string("name"),
                               SIM_make_attr_string(node_props->name));
        SIM_attr_dict_set_item(
                &props, 1, SIM_make_attr_string("multiprocessor"),
                SIM_make_attr_boolean(multiprocessor));
        if (include_pid) {
                append_to_dict(&props, "pid",
                               SIM_make_attr_uint64(node_props->pid));
        }
        if (include_tid) {
                append_to_dict(&props, "tid",
                               SIM_make_attr_uint64(node_props->tid));
        }
        return props;
}

void
set_memspace(sample_linux_mapper_t *mapper, node_id_t node_id,
             node_id_t memory_space_id)
{
        update_node_property(mapper, node_id, "memory_space",
                             SIM_make_attr_uint64(memory_space_id));
}

static node_t *
make_node(node_type_t type, node_id_t node_id, node_id_t parent_id)
{
        node_t *node = MM_MALLOC(1, node_t);
        node->type = type;
        node->id = node_id;
        node->parent_id = parent_id;
        return node;
}

static void
set_active_node(sample_linux_mapper_t *mapper, entity_id_t entity_id,
                conf_object_t *cpu, processor_mode_t mode)
{
        SIM_LOG_INFO(4, &mapper->obj, 0,
                     "Making entity %llu active", entity_id);

        node_t *node = ht_lookup_int(&mapper->entity_to_node, entity_id);
        node_id_t node_id;
        if (!node || (node->type == User_Thread && mode != Sim_CPU_Mode_User)) {
                /* Unknown node (likely root node) or user node active in
                   kernel space, set other node as active. */
                node_id = mapper->base_nodes.other_node_id;
        } else {
                node_id = node->id;
        }
        mapper->parent.nt_admin_iface->activate(
                mapper->parent.obj, node_id, cpu);
}

static void
set_extra_id(sample_linux_mapper_t *mapper, node_id_t node_id,
             const char *prop_name)
{
        attr_value_t extra_id = SIM_make_attr_list(
                1, SIM_make_attr_string(prop_name));
        update_node_property(mapper, node_id, "extra_id", extra_id);
        SIM_attr_free(&extra_id);
}

static void
add_thread(sample_linux_mapper_t *mapper, entity_id_t entity_id,
           node_id_t parent_id, node_id_t memoryspace, node_props_t *node_props)
{
        attr_value_t props = node_props_to_attr_val(
                node_props, !node_props->is_kernel, true, false);

        node_id_t node_id = mapper->parent.nt_admin_iface->add(
                mapper->parent.obj, parent_id, props);

        set_memspace(mapper, node_id, memoryspace);
        node_type_t type = node_props->is_kernel ? Kernel_Thread : User_Thread;
        node_t *node = make_node(type, node_id, parent_id);
        ht_insert_int(&mapper->entity_to_node, entity_id, node);
        if (node_props->active) {
                set_active_node(mapper, entity_id, node_props->active,
                                node_props->mode);
        }
        SIM_attr_free(&props);
        set_extra_id(mapper, node_id, "tid");
}

static void
add_kernel_thread(sample_linux_mapper_t *mapper, entity_id_t entity_id,
                  node_props_t *node_props)
{
        SIM_LOG_INFO(4, &mapper->obj, 0, "Adding new kernel entity: %s, %llu",
                     node_props->name, node_props->tid);

        node_id_t kernel_id = mapper->base_nodes.kernel_node_id;
        node_id_t threads_id = mapper->base_nodes.threads_node_id;
        add_thread(mapper, entity_id, threads_id, kernel_id, node_props);
}

static void
add_user_thread(sample_linux_mapper_t *mapper, entity_id_t entity_id,
                node_id_t parent_id, node_props_t *thread_props)
{
        SIM_LOG_INFO(4, &mapper->obj, 0,
                     "Adding thread '%s' (%llu, %llu) under node %llu",
                     thread_props->name, thread_props->pid, thread_props->tid,
                     parent_id);
        add_thread(mapper, entity_id, parent_id, parent_id, thread_props);
}

static void
add_user_process(sample_linux_mapper_t *mapper, entity_id_t entity_id,
                 node_props_t *node_props)
{
        SIM_LOG_INFO(4, &mapper->obj, 0,
                     "Creating process '%s' (%llu, %llu)",
                     node_props->name, node_props->pid, node_props->tid);
        attr_value_t user_props = node_props_to_attr_val(
                node_props, true, false, true);
        node_id_t process_node_id = mapper->parent.nt_admin_iface->add(
                mapper->parent.obj,
                mapper->base_nodes.user_node_id, user_props);
        set_memspace(mapper, process_node_id, process_node_id);
        set_extra_id(mapper, process_node_id, "pid");

        add_user_thread(mapper, entity_id, process_node_id, node_props);
        SIM_attr_free(&user_props);
}

static void
free_node_props(node_props_t *node_props)
{
        MM_FREE(node_props->name);
        MM_FREE(node_props);
}

static void
add_kernel_or_user_node(sample_linux_mapper_t *mapper, entity_id_t entity_id,
                        attr_value_t props)
{
        node_props_t *node_props = entity_props_to_node_props(props);
        if (node_props->is_kernel)
                add_kernel_thread(mapper, entity_id, node_props);
        else
                add_user_process(mapper, entity_id, node_props);
        free_node_props(node_props);
}

void
add_entities(sample_linux_mapper_t *mapper, attr_value_t entities)
{
        for (int i = 0; i < SIM_attr_dict_size(entities); i++) {
                entity_id_t entity_id = SIM_attr_integer(
                        SIM_attr_dict_key(entities, i));
                if (entity_id == ROOT_ENTITY_ID) {
                        update_root(mapper, SIM_attr_dict_value(entities, i));
                } else {
                        add_kernel_or_user_node(
                                mapper, entity_id,
                                SIM_attr_dict_value(entities, i));
                }
        }
}

static void
update_name(sample_linux_mapper_t *mapper, node_t *node, attr_value_t new_name)
{
        update_node_property(mapper, node->id, "name", new_name);
        if (node->type == User_Thread) {
                update_node_property(mapper, node->parent_id, "name", new_name);
        }
}

static void
update_active_node(sample_linux_mapper_t *mapper, entity_id_t entity_id,
                   attr_value_t active)
{
        if (SIM_attr_is_nil(active)) {
                /* Entity is no longer active, another entity will be
                   active. No need to do anything here. */
                return;
        }
        conf_object_t *cpu = SIM_attr_object(SIM_attr_list_item(active, 0));
        ASSERT(cpu);
        processor_mode_t mode = SIM_attr_integer(SIM_attr_list_item(active, 1));
        set_active_node(mapper, entity_id, cpu, mode);
}

void
update_entity(sample_linux_mapper_t *mapper, entity_id_t entity_id,
              attr_value_t updates)
{
        node_t *node = ht_lookup_int(&mapper->entity_to_node, entity_id);
        if (!node) {
                SIM_LOG_ERROR(&mapper->obj, 0,
                              "Unknown entity %llu updated", entity_id);
                return;
        }

        for (int i = 0; i < SIM_attr_dict_size(updates); i++) {
                const char *key = SIM_attr_string(
                        SIM_attr_dict_key(updates, i));
                prop_type_t type = prop_name_to_type(key);

                /* An update value is a list of [old value, new value], for the
                   update functions we are only interested in the new value. */
                attr_value_t old_new_value = SIM_attr_dict_value(updates, i);
                attr_value_t new_value = SIM_attr_list_item(old_new_value, 1);
                if (type == Prop_Name)
                        update_name(mapper, node, new_value);
                else if (type == Prop_Active)
                        update_active_node(mapper, entity_id, new_value);
        }
}

/* Update active state and name of the root node, all other properties are
   ignored. */
void
update_root_property(sample_linux_mapper_t *mapper, const char *key,
                     attr_value_t value)
{
        prop_type_t prop_type = prop_name_to_type(key);
        switch (prop_type) {
        case Prop_Name:
                update_node_property(mapper, mapper->base_nodes.root_node_id,
                                     key, value);
                break;
        case Prop_Active:
                update_active_node(mapper, ROOT_ENTITY_ID, value);
                break;
        default:
                SIM_LOG_INFO(3, &mapper->obj, 0,
                             "Unknown property '%s' updated on root", key);
                break;
        }
}

static void
remove_entity(sample_linux_mapper_t *mapper, entity_id_t entity_id)
{
        node_t *node = ht_lookup_int(&mapper->entity_to_node, entity_id);
        if (!node) {
                SIM_LOG_ERROR(&mapper->obj, 0,
                              "Unknown entity %llu removed", entity_id);
                return;
        }
        node_id_t node_id_to_remove;
        if (node->type == User_Thread)
                node_id_to_remove = node->parent_id;
        else
                node_id_to_remove = node->id;

        mapper->parent.nt_admin_iface->remove(
                mapper->parent.obj, node_id_to_remove);
        ht_remove_int(&mapper->entity_to_node, entity_id);
        MM_FREE(node);
}

void
remove_entities(sample_linux_mapper_t *mapper, attr_value_t entities)
{
        for (int i = 0; i < SIM_attr_list_size(entities); i++) {
                entity_id_t entity_id = SIM_attr_integer(
                        SIM_attr_list_item(entities, i));
                if (entity_id == ROOT_ENTITY_ID) {
                        /* Should never happen */
                        SIM_LOG_ERROR(&mapper->obj, 0, "Removing root entity");
                } else {
                        remove_entity(mapper, entity_id);
                }
        }
}

static set_error_t
set_parent_attr(void *arg, conf_object_t *obj, attr_value_t *val,
                attr_value_t *idx)
{
        sample_linux_mapper_t *mapper = (sample_linux_mapper_t *)obj;
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
        sample_linux_mapper_t *mapper = (sample_linux_mapper_t *)obj;
        return SIM_make_attr_object(mapper->parent.obj);
}

static set_error_t
set_tracker_attr(void *arg, conf_object_t *obj, attr_value_t *val,
                 attr_value_t *idx)
{
        sample_linux_mapper_t *mapper = (sample_linux_mapper_t *)obj;
        mapper->tracker = SIM_attr_object(*val);

        return Sim_Set_Ok;
}

static attr_value_t
get_tracker_attr(void *arg, conf_object_t *obj, attr_value_t *idx)
{
        sample_linux_mapper_t *mapper = (sample_linux_mapper_t *)obj;
        return SIM_make_attr_object(mapper->tracker);
}

static attr_value_t
get_enabled_attribute(void *arg, conf_object_t *obj, attr_value_t *idx)
{
        sample_linux_mapper_t *mapper = (sample_linux_mapper_t *)obj;
        return SIM_make_attr_boolean(mapper->enabled);
}

static attr_value_t
base_nodes_as_list(sample_linux_mapper_t *mapper)
{
        base_nodes_t *base_nodes = &mapper->base_nodes;
        return SIM_make_attr_list(
                5, SIM_make_attr_uint64(base_nodes->root_node_id),
                SIM_make_attr_uint64(base_nodes->user_node_id),
                SIM_make_attr_uint64(base_nodes->kernel_node_id),
                SIM_make_attr_uint64(base_nodes->other_node_id),
                SIM_make_attr_uint64(base_nodes->threads_node_id));
}

static attr_value_t
get_base_nodes_attribute(void *arg, conf_object_t *obj, attr_value_t *idx)
{
        sample_linux_mapper_t *mapper = (sample_linux_mapper_t *)obj;
        if (!mapper->root_added)
                return SIM_make_attr_nil();
        return base_nodes_as_list(mapper);
}

static set_error_t
set_base_nodes_attribute(void *arg, conf_object_t *obj, attr_value_t *val,
                         attr_value_t *idx)
{
        sample_linux_mapper_t *mapper = (sample_linux_mapper_t *)obj;
        if (SIM_attr_is_nil(*val)) {
                mapper->root_added = false;
        } else {
                base_nodes_t *base_nodes = &mapper->base_nodes;
                base_nodes->root_node_id = SIM_attr_integer(
                        SIM_attr_list_item(*val, 0));
                base_nodes->user_node_id = SIM_attr_integer(
                        SIM_attr_list_item(*val, 1));
                base_nodes->kernel_node_id = SIM_attr_integer(
                        SIM_attr_list_item(*val, 2));
                base_nodes->other_node_id = SIM_attr_integer(
                        SIM_attr_list_item(*val, 3));
                base_nodes->threads_node_id = SIM_attr_integer(
                        SIM_attr_list_item(*val, 4));
                mapper->root_added = true;
        }
        return Sim_Set_Ok;
}

static attr_value_t
get_entities_attribute(void *arg, conf_object_t *obj, attr_value_t *idx)
{
        sample_linux_mapper_t *mapper = (sample_linux_mapper_t *)obj;
        attr_value_t entities = SIM_alloc_attr_list(
                ht_num_entries_int(&mapper->entity_to_node));
        int i = 0;
        HT_FOREACH_INT(&mapper->entity_to_node, it) {
                entity_id_t entity_id = ht_iter_int_key(it);
                node_t *node = ht_iter_int_value(it);
                attr_value_t entity = SIM_make_attr_list(
                        4, SIM_make_attr_uint64(entity_id),
                        SIM_make_attr_uint64(node->id),
                        SIM_make_attr_uint64(node->parent_id),
                        SIM_make_attr_uint64(node->type));
                SIM_attr_list_set_item(&entities, i, entity);
                i++;
        }
        return entities;
}

static set_error_t
set_entities_attribute(void *arg, conf_object_t *obj, attr_value_t *val,
                       attr_value_t *idx)
{
        sample_linux_mapper_t *mapper = (sample_linux_mapper_t *)obj;
        ht_delete_int_table(&mapper->entity_to_node, true);
        for (int i = 0; i < SIM_attr_list_size(*val); i++) {
                attr_value_t entity = SIM_attr_list_item(*val, i);
                entity_id_t entity_id = SIM_attr_integer(
                        SIM_attr_list_item(entity, 0));
                node_id_t node_id = SIM_attr_integer(
                        SIM_attr_list_item(entity, 1));
                node_id_t parent_id = SIM_attr_integer(
                        SIM_attr_list_item(entity, 2));
                node_type_t type = SIM_attr_integer(
                        SIM_attr_list_item(entity, 3));
                node_t *node = make_node(type, node_id, parent_id);
                ht_insert_int(&mapper->entity_to_node, entity_id, node);
        }
        return Sim_Set_Ok;
}

void
clear_mapper_state(sample_linux_mapper_t *mapper)
{
        ht_delete_int_table(&mapper->entity_to_node, true);
        mapper->base_nodes = (base_nodes_t){0};
        mapper->root_added = false;
}

/* Cleanup of deleted object instance */
static int
delete_instance(conf_object_t *obj)
{
        sample_linux_mapper_t *mapper = (sample_linux_mapper_t *)obj;
        clear_mapper_state(mapper);
        MM_FREE(mapper);
        return 0;
}

void
sample_linux_mapper_init_local()
{
        const class_data_t mapper_funcs = {
                .alloc_object = alloc_object,
                .init_object = init_object,
                .delete_instance = delete_instance,
                .class_desc = "a Linux sample mapper",
                .description = "An example of a Linux mapper that works with"
                " trackers of the sample_linux_tracker class."

        };
        conf_class_t *cls = SIM_register_class("sample_linux_mapper",
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
                cls, "base_nodes",
                get_base_nodes_attribute, NULL, set_base_nodes_attribute, NULL,
                Sim_Attr_Optional | Sim_Attr_Internal, "[iiiii]|n", NULL,
                "Base nodes ids [root, user, kernel, other, thread],"
                " nil if no nodes have been added");

        SIM_register_typed_attribute(
                cls, "entities",
                get_entities_attribute, NULL, set_entities_attribute, NULL,
                Sim_Attr_Optional, "[[iiii]*]", NULL,
                "Mapping of entities to nodes on the format:"
                " [entity id, node id, parent id, type]");
}
