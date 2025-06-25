/*
  © 2016 Intel Corporation
*/

#include "empty-software-mapper-control-iface.h"
#include <simics/simulator-iface/os-awareness-tracker-interfaces.h>
#include "empty-software-mapper.h"

static bool
has_state(empty_software_mapper_t *mapper)
{
        return mapper->root_added;
}

static attr_value_t
make_root_props()
{
        attr_value_t props = SIM_alloc_attr_dict(1);
        SIM_attr_dict_set_item(&props, 0, SIM_make_attr_string("name"),
                               SIM_make_attr_string("mapper root"));
        return props;
}

static void
create_root_node(empty_software_mapper_t *mapper)
{
        attr_value_t root_props = make_root_props();
        transaction_id_t txid = mapper->parent.nt_admin_iface->begin(
                mapper->parent.obj, NULL);
        mapper->root_id = mapper->parent.nt_admin_iface->create(
                mapper->parent.obj, &mapper->obj, root_props);
        mapper->parent.nt_admin_iface->end(
                mapper->parent.obj, txid);
        SIM_attr_free(&root_props);
        mapper->root_added = true;
}

static void
subscribe_tracker(empty_software_mapper_t *mapper)
{
        mapper->parent.ts_notify_iface->subscribe_tracker(
                mapper->parent.obj, &mapper->obj, mapper->tracker);
}

static void
add_existing_entities(empty_software_mapper_t *mapper)
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
        empty_software_mapper_t *mapper = (empty_software_mapper_t *)obj;
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
        create_root_node(mapper);
        subscribe_tracker(mapper);
        add_existing_entities(mapper);
        return true;
}

static void
disable(conf_object_t *obj)
{
        empty_software_mapper_t *mapper = (empty_software_mapper_t *)obj;
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
        empty_software_mapper_t *mapper = (empty_software_mapper_t *)obj;
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
