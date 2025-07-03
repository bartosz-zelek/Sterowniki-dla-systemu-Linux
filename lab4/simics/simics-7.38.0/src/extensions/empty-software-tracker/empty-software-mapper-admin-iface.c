/*
  © 2016 Intel Corporation
*/

#include "empty-software-mapper-admin-iface.h"
#include "empty-software-mapper.h"
#include <simics/simulator-iface/os-awareness-tracker-interfaces.h>
#include "empty-software-mapper-control-iface.h"
#include "empty-software-tracker-common.h"

static void
handle_root_modification(empty_software_mapper_t *mapper, attr_value_t updates)
{
        SIM_LOG_INFO(3, &mapper->obj, 0, "Update root node");
        for (int i = 0; i < SIM_attr_dict_size(updates); i++) {
                const char *key = SIM_attr_string(
                        SIM_attr_dict_key(updates, i));
                attr_value_t value = SIM_attr_dict_value(updates, i);
                attr_value_t new_val = SIM_attr_list_item(value, 1);
                update_root_property(mapper, key, new_val);
        }
}

static void
handle_added_entities(empty_software_mapper_t *mapper, attr_value_t added)
{
        add_entities(mapper, added);
}

static void
handle_modified_entities(empty_software_mapper_t *mapper, attr_value_t updates)
{
        for (int i = 0; i < SIM_attr_dict_size(updates); i++) {
                entity_id_t entity_id = SIM_attr_integer(
                        SIM_attr_dict_key(updates, i));
                attr_value_t entity_changes = SIM_attr_dict_value(updates, i);
                if (entity_id == ROOT_ENTITY_ID)
                        handle_root_modification(mapper, entity_changes);
        }
}

static void
handle_removed_entities(empty_software_mapper_t *mapper, attr_value_t removed)
{
        /* TODO: Handle removed entities. */
}

static void
handle_events(empty_software_mapper_t *mapper, attr_value_t events)
{
        /* TODO: Handle tracker events, if it supports any. */
}

static void
tracker_updated(conf_object_t *obj, conf_object_t *initiator,
                attr_value_t changeset)
{
        empty_software_mapper_t *mapper = (empty_software_mapper_t *)obj;
        SIM_LOG_INFO(3, &mapper->obj, 0, "Tracker updated, initiated by '%s'",
                     initiator ? SIM_object_name(initiator) : "None");

        attr_value_t added = SIM_make_attr_nil();
        attr_value_t modified = SIM_make_attr_nil();
        attr_value_t removed = SIM_make_attr_nil();
        attr_value_t events = SIM_make_attr_nil();

        /* We have only subscribed to one tracker, so we will only receive
           updates for that tracker. */
        attr_value_t tr_changes = SIM_attr_dict_value(changeset, 0);
        for (int i = 0; i < SIM_attr_dict_size(tr_changes); i++) {
                const char *key = SIM_attr_string(
                        SIM_attr_dict_key(tr_changes, i));
                attr_value_t value = SIM_attr_dict_value(tr_changes, i);
                if (strcmp(key, "added") == 0
                    && SIM_attr_dict_size(value) > 0) {
                        added = value;
                } else if (strcmp(key, "modified") == 0
                           && SIM_attr_dict_size(value) > 0) {
                        modified = value;
                } else if (strcmp(key, "removed") == 0
                           && SIM_attr_list_size(value) > 0) {
                        removed = value;
                } else if (strcmp(key, "events") == 0
                           && SIM_attr_dict_size(value) > 0) {
                        events = value;
                }
        }
        transaction_id_t txid = mapper->parent.nt_admin_iface->begin(
                mapper->parent.obj, initiator);
        if (!SIM_attr_is_nil(added))
                handle_added_entities(mapper, added);
        if (!SIM_attr_is_nil(modified))
                handle_modified_entities(mapper, modified);
        if (!SIM_attr_is_nil(events))
                handle_events(mapper, events);
        if (!SIM_attr_is_nil(removed))
                handle_removed_entities(mapper, removed);
        mapper->parent.nt_admin_iface->end(mapper->parent.obj, txid);
}

void
register_mapper_admin_iface(conf_class_t *cls)
{
        static const osa_mapper_admin_interface_t mapper_admin_iface = {
                .tracker_updated = tracker_updated,
        };
        SIM_register_interface(cls, OSA_MAPPER_ADMIN_INTERFACE,
                               &mapper_admin_iface);
}
