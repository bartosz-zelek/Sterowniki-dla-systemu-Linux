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

#include "sample-linux-mapper-control-iface.h"
#include <simics/simulator-iface/os-awareness-tracker-interfaces.h>
#include "sample-linux-mapper.h"

static bool
has_state(sample_linux_mapper_t *mapper)
{
        return mapper->root_added;
}

static attr_value_t
make_root_props()
{
        attr_value_t props = SIM_alloc_attr_dict(1);
        SIM_attr_dict_set_item(&props, 0, SIM_make_attr_string("name"),
                               SIM_make_attr_string("Linux"));
        return props;
}

static void
create_root_node(sample_linux_mapper_t *mapper)
{
        attr_value_t root_props = make_root_props();
        mapper->base_nodes.root_node_id = mapper->parent.nt_admin_iface->create(
                mapper->parent.obj, &mapper->obj, root_props);
        SIM_attr_free(&root_props);
        mapper->root_added = true;
}

static attr_value_t
make_base_props(const char *name, bool multiprocessor)
{
        attr_value_t name_prop = SIM_alloc_attr_dict(2);
        SIM_attr_dict_set_item(&name_prop, 0, SIM_make_attr_string("name"),
                               SIM_make_attr_string(name));
        SIM_attr_dict_set_item(
                &name_prop, 1, SIM_make_attr_string("multiprocessor"),
                SIM_make_attr_boolean(multiprocessor));
        return name_prop;
}

static node_id_t
create_kernel_base_node(sample_linux_mapper_t *mapper, node_id_t parent_id,
                        const char *name, bool parent_as_memory_space,
                        bool multiprocessor)
{
        attr_value_t base_props = make_base_props(name, multiprocessor);
        node_id_t id = mapper->parent.nt_admin_iface->add(
                mapper->parent.obj, parent_id, base_props);
        SIM_attr_free(&base_props);
        if (parent_as_memory_space)
                set_memspace(mapper, id, parent_id);
        else
                set_memspace(mapper, id, id);
        return id;
}

static void
create_kernel_nodes(sample_linux_mapper_t *mapper)
{
        base_nodes_t *nodes = &mapper->base_nodes;
        nodes->kernel_node_id = create_kernel_base_node(
                mapper, nodes->root_node_id, "Kernel", false, true);
        nodes->other_node_id = create_kernel_base_node(
                mapper, nodes->kernel_node_id, "Other", true, false);
        nodes->threads_node_id = create_kernel_base_node(
                mapper, nodes->kernel_node_id, "Threads", true, true);
}

static void
create_user_node(sample_linux_mapper_t *mapper)
{
        attr_value_t base_props = make_base_props("Userspace", true);
        mapper->base_nodes.user_node_id = mapper->parent.nt_admin_iface->add(
                mapper->parent.obj, mapper->base_nodes.root_node_id,
                base_props);
        SIM_attr_free(&base_props);
}

static void
create_base_nodes(sample_linux_mapper_t *mapper)
{
        transaction_id_t txid = mapper->parent.nt_admin_iface->begin(
                mapper->parent.obj, NULL);
        create_root_node(mapper);
        create_kernel_nodes(mapper);
        create_user_node(mapper);
        mapper->parent.nt_admin_iface->end(mapper->parent.obj, txid);
}

static void
subscribe_tracker(sample_linux_mapper_t *mapper)
{
        mapper->parent.ts_notify_iface->subscribe_tracker(
                mapper->parent.obj, &mapper->obj, mapper->tracker);
}

static void
add_existing_entities(sample_linux_mapper_t *mapper)
{
        attr_value_t entities = mapper->parent.ts_query_iface->get_entities(
                mapper->parent.obj, mapper->tracker);
        if (SIM_attr_is_nil(entities))
                return;
        transaction_id_t txid = mapper->parent.nt_admin_iface->begin(
                mapper->parent.obj, NULL);
        add_entities(mapper, entities);
        mapper->parent.nt_admin_iface->end(mapper->parent.obj, txid);
        SIM_attr_free(&entities);
}

static bool
enable(conf_object_t *obj)
{
        sample_linux_mapper_t *mapper = (sample_linux_mapper_t *)obj;
        SIM_LOG_INFO(2, &mapper->obj, 0, "Enabling mapper");
        if (mapper->enabled) {
                /* Should not happen, but lets ignore if it does. */
                return true;
        }
        mapper->enabled = true;
        if (has_state(mapper)) {
                /* Enabled from a checkpointed state, nodes are already
                   added. */
                return true;
        }
        create_base_nodes(mapper);
        subscribe_tracker(mapper);
        add_existing_entities(mapper);
        return true;
}

static void
disable(conf_object_t *obj)
{
        sample_linux_mapper_t *mapper = (sample_linux_mapper_t *)obj;
        SIM_LOG_INFO(2, &mapper->obj, 0, "Disabling mapper");
        if (!mapper->enabled) {
                /* Should not happen, but lets ignore if it does. */
                return;
        }
        clear_mapper_state(mapper);
        mapper->enabled = false;
}

static void
clear_state(conf_object_t *obj)
{
        sample_linux_mapper_t *mapper = (sample_linux_mapper_t *)obj;
        SIM_LOG_INFO(3, &mapper->obj, 0, "Clearing internal state");
        clear_mapper_state(mapper);
}

void
register_mapper_control_iface(conf_class_t *cls)
{
        static const osa_mapper_control_interface_t mapper_control_iface = {
                .disable = disable,
                .enable = enable,
                .clear_state = clear_state,
        };
        SIM_register_interface(cls, OSA_MAPPER_CONTROL_INTERFACE,
                               &mapper_control_iface);
}
