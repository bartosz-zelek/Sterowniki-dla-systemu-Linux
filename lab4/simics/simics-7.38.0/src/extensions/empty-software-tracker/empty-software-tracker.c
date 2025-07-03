/*
  © 2016 Intel Corporation
*/

#include "empty-software-tracker.h"
#include <simics/device-api.h>
#include <simics/simulator-iface/os-awareness-tracker-interfaces.h>
#include "empty-software-tracker-control-iface.h"
#include "empty-software-tracker-common.h"
#include "empty-software-mapper.h"
#include <simics/util/vect.h>

static conf_object_t *
alloc_object(void *data)
{
        empty_software_tracker_t *tracker = MM_ZALLOC(
                1, empty_software_tracker_t);
        return &tracker->obj;
}

static set_error_t
set_parent_attr(void *arg, conf_object_t *obj, attr_value_t *val,
                attr_value_t *idx)
{
        empty_software_tracker_t *tracker = (empty_software_tracker_t *)obj;
        conf_object_t *parent_obj = SIM_attr_object(*val);

        const osa_tracker_state_admin_interface_t *state_admin;
        state_admin = SIM_c_get_interface(parent_obj,
                                          OSA_TRACKER_STATE_ADMIN_INTERFACE);
        if (!state_admin) {
                SIM_attribute_error("object does not implement the "
                                    OSA_TRACKER_STATE_ADMIN_INTERFACE
                                    " interface");
                return Sim_Set_Interface_Not_Found;
        }

        const osa_machine_query_interface_t *query =
                SIM_c_get_interface(parent_obj, OSA_MACHINE_QUERY_INTERFACE);
        if (!query) {
                SIM_attribute_error("object does not implement the "
                                    OSA_MACHINE_QUERY_INTERFACE
                                    " interface");
                return Sim_Set_Interface_Not_Found;
        }

        const osa_machine_notification_interface_t *notify =
                SIM_c_get_interface(parent_obj,
                                    OSA_MACHINE_NOTIFICATION_INTERFACE);
        if (!notify) {
                SIM_attribute_error("object does not implement the "
                                    OSA_MACHINE_NOTIFICATION_INTERFACE
                                    " interface");
                return Sim_Set_Interface_Not_Found;
        }
        tracker->parent.obj = parent_obj;
        tracker->parent.state_admin = state_admin;
        tracker->parent.machine_query = query;
        tracker->parent.machine_notify = notify;
        return Sim_Set_Ok;
}

static attr_value_t
get_parent_attr(void *arg, conf_object_t *obj, attr_value_t *idx)
{
        empty_software_tracker_t *tracker = (empty_software_tracker_t *)obj;
        return SIM_make_attr_object(tracker->parent.obj);
}

static set_error_t
set_root_entity_added_attribute(void *arg, conf_object_t *obj,
                                attr_value_t *val, attr_value_t *idx)
{
        empty_software_tracker_t *tracker = (empty_software_tracker_t *)obj;
        tracker->root_added = SIM_attr_boolean(*val);
        return Sim_Set_Ok;
}

static attr_value_t
get_root_entity_added_attribute(void *arg, conf_object_t *obj,
                                attr_value_t *idx)
{
        empty_software_tracker_t *tracker = (empty_software_tracker_t *)obj;
        return SIM_make_attr_boolean(tracker->root_added);
}

/* Converts an attr_value_t list of objects to a vector of conf_object_t
   pointers. The caller should free the returned vector. */
static conf_object_vect_t
object_list_to_vect(attr_value_t object_list)
{
        conf_object_vect_t object_vect = VNULL;
        for (int i = 0; i < SIM_attr_list_size(object_list); i++) {
                conf_object_t *obj = SIM_attr_object(
                        SIM_attr_list_item(object_list, i));
                VADD(object_vect, obj);
        }
        return object_vect;
}

/* Converts a vector of conf_object_t pointers to an attr_value_t list of
   objects. The caller should free the returned attribute. */
static attr_value_t
object_vect_to_list(conf_object_vect_t object_vect)
{
        attr_value_t object_list = SIM_alloc_attr_list(VLEN(object_vect));
        int i = 0;
        VFORP(object_vect, conf_object_t, obj) {
                SIM_attr_list_set_item(&object_list, i,
                                       SIM_make_attr_object(obj));
                i++;
        }
        return object_list;
}

static set_error_t
set_cpus_attribute(void *arg, conf_object_t *obj, attr_value_t *val,
                   attr_value_t *idx)
{
        empty_software_tracker_t *tracker = (empty_software_tracker_t *)obj;
        VFREE(tracker->cpus);
        tracker->cpus = object_list_to_vect(*val);
        return Sim_Set_Ok;
}

static attr_value_t
get_cpus_attribute(void *arg, conf_object_t *obj, attr_value_t *idx)
{
        empty_software_tracker_t *tracker = (empty_software_tracker_t *)obj;
        return object_vect_to_list(tracker->cpus);
}

static attr_value_t
get_enabled_attribute(void *arg, conf_object_t *obj, attr_value_t *idx)
{
        empty_software_tracker_t *tracker = (empty_software_tracker_t *)obj;
        return SIM_make_attr_boolean(tracker->enabled);
}

/* Allows the user to modify the root entity name. The name of the entity
   should not be set when loading a checkpoint, as the framework is responsible
   for restoring entities to their correct state. */
static set_error_t
set_root_name_attr(void *arg, conf_object_t *obj, attr_value_t *val,
                   attr_value_t *idx)
{
        empty_software_tracker_t *tracker = (empty_software_tracker_t *)obj;
        if (!tracker->enabled) {
                SIM_attribute_error("Must be enabled to set root name");
                return Sim_Set_Illegal_Value;
        }
        transaction_id_t txid = tracker->parent.state_admin->begin(
                tracker->parent.obj, &tracker->obj, NULL);
        tracker->parent.state_admin->set_attribute(
                tracker->parent.obj, ROOT_ENTITY_ID, "name", *val);
        tracker->parent.state_admin->end(tracker->parent.obj, txid);
        return Sim_Set_Ok;
}

/* Initialize the object before any attributes are set. */
static void *
init_object(conf_object_t *obj, void *data)
{
        empty_software_tracker_t *tracker = (empty_software_tracker_t *)obj;
        tracker->enabled = false;
        tracker->root_added = false;
        VINIT(tracker->cpus);
        return obj;
}

/* Cleanup of deleted object instance */
static int
delete_instance(conf_object_t *obj)
{
        empty_software_tracker_t *tracker = (empty_software_tracker_t *)obj;
        VFREE(tracker->cpus);
        MM_FREE(tracker);
        return 0;
}

void
init_local()
{
        const class_data_t funcs = {
                .alloc_object = alloc_object,
                .init_object = init_object,
                .delete_instance = delete_instance,
                .class_desc = "tracker skeleton",
                .description = "Skeleton for creating a tracker."
        };
        conf_class_t *cls = SIM_register_class(
                "empty_software_tracker", &funcs);

        SIM_register_typed_attribute(
                cls, "parent",
                get_parent_attr, NULL, set_parent_attr, NULL,
                Sim_Attr_Required, "o", NULL,
                "The parent object. Must implement the"
                " " OSA_TRACKER_STATE_ADMIN_INTERFACE
                ", " OSA_MACHINE_QUERY_INTERFACE
                ", and " OSA_MACHINE_NOTIFICATION_INTERFACE
                " interfaces");

        SIM_register_typed_attribute(
                cls, "root_entity_added",
                get_root_entity_added_attribute, NULL,
                set_root_entity_added_attribute, NULL,
                Sim_Attr_Optional | Sim_Attr_Internal, "b", NULL,
                "True if the root entity has been added");

        SIM_register_typed_attribute(
                cls, "cpus",
                get_cpus_attribute, NULL, set_cpus_attribute, NULL,
                Sim_Attr_Optional | Sim_Attr_Internal, "[o*]", NULL,
                "List of processors provided to the tracker");

        SIM_register_typed_attribute(
                cls, "enabled",
                get_enabled_attribute, NULL, NULL, NULL,
                Sim_Attr_Pseudo, "b", NULL, "Tracker enabled - read only");

        SIM_register_typed_attribute(
                cls, "root_name",
                NULL, NULL, set_root_name_attr, NULL,
                Sim_Attr_Pseudo, "s", NULL,
                "Set the name of the root entity");

        register_tracker_control_iface(cls);
        empty_software_mapper_init_local();
}
