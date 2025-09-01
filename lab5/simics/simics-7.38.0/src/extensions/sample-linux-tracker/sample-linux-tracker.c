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

#include "sample-linux-tracker.h"
#include <simics/device-api.h>
#include <simics/simulator-iface/os-awareness-tracker-interfaces.h>
#include "sample-linux-tracker-control-iface.h"
#include "sample-linux-tracker-common.h"
#include "sample-linux-tracker-monitor.h"
#include "sample-linux-mapper.h"
#include "sample-linux-tracker-params.h"
#include <simics/util/vect.h>

static conf_object_t *
alloc_object(void *data)
{
        sample_linux_tracker_t *tracker = MM_ZALLOC(1, sample_linux_tracker_t);
        return &tracker->obj;
}

bool
vect_contains_value(uint64_vect_t vect, uint64 value)
{
        VFORT(vect, uint64, x) {
                if (x == value)
                        return true;
        }
        return false;
}

void
cancel_one_task(sample_linux_tracker_t *tracker, cancel_id_vect_t *cancel_ids)
{
        VFORT(*cancel_ids, cancel_id_t, cid) {
                tracker->parent.machine_notify->cancel(
                        tracker->parent.obj, &tracker->obj, cid);
        }
        VFREE(*cancel_ids);
}

void
insert_task(sample_linux_tracker_t *tracker, uint64 ts_addr)
{
        cancel_id_vect_t *cancel_ids = MM_MALLOC(1, cancel_id_vect_t);
        VINIT(*cancel_ids);
        cancel_id_vect_t *old_vect = ht_update_int(&tracker->tasks, ts_addr,
                                                   cancel_ids);
        if (old_vect) {
                SIM_LOG_ERROR(&tracker->obj, 0,
                              "Inserting task 0x%llx which already exits",
                              ts_addr);
                cancel_one_task(tracker, old_vect);
                MM_FREE(old_vect);
        }
}

static bool
is_loading_mc(conf_object_t *obj)
{
        return SIM_is_restoring_state(obj) && SIM_object_is_configured(obj);
}

static set_error_t
set_parent_attr(void *arg, conf_object_t *obj, attr_value_t *val,
                attr_value_t *idx)
{
        if (is_loading_mc(obj)) {
                /* parent cannot change between micro-checkpoints */
                return Sim_Set_Ok;
        }

        sample_linux_tracker_t *tracker = (sample_linux_tracker_t *)obj;
        if (tracker->enabled) {
                SIM_attribute_error("You may not change parent while the"
                                    " tracker is enabled");
                return Sim_Set_Illegal_Value;
        }

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
        sample_linux_tracker_t *tracker = (sample_linux_tracker_t *)obj;
        return SIM_make_attr_object(tracker->parent.obj);
}

static set_error_t
set_root_entity_added_attribute(void *arg, conf_object_t *obj,
                                attr_value_t *val, attr_value_t *idx)
{
        sample_linux_tracker_t *tracker = (sample_linux_tracker_t *)obj;
        tracker->root_added = SIM_attr_boolean(*val);
        return Sim_Set_Ok;
}

static attr_value_t
get_root_entity_added_attribute(void *arg, conf_object_t *obj,
                                attr_value_t *idx)
{
        sample_linux_tracker_t *tracker = (sample_linux_tracker_t *)obj;
        return SIM_make_attr_boolean(tracker->root_added);
}

static set_error_t
set_cpu_attribute(void *arg, conf_object_t *obj, attr_value_t *val,
                  attr_value_t *idx)
{
        sample_linux_tracker_t *tracker = (sample_linux_tracker_t *)obj;
        tracker->cpu = SIM_attr_object_or_nil(*val);
        return Sim_Set_Ok;
}

static attr_value_t
get_cpu_attribute(void *arg, conf_object_t *obj, attr_value_t *idx)
{
        sample_linux_tracker_t *tracker = (sample_linux_tracker_t *)obj;
        return SIM_make_attr_object(tracker->cpu);
}

static attr_value_t
get_enabled_attribute(void *arg, conf_object_t *obj, attr_value_t *idx)
{
        sample_linux_tracker_t *tracker = (sample_linux_tracker_t *)obj;
        return SIM_make_attr_boolean(tracker->enabled);
}

/* Initialize the object before any attributes are set. */
static void *
init_object(conf_object_t *obj, void *data)
{
        sample_linux_tracker_t *tracker = (sample_linux_tracker_t *)obj;
        tracker->enabled = false;
        tracker->root_added = false;
        tracker->cpu = NULL;
        tracker->params = default_parameters();
        return obj;
}

/* Cleanup of deleted object instance */
static int
delete_instance(conf_object_t *obj)
{
        sample_linux_tracker_t *tracker = (sample_linux_tracker_t *)obj;
        free_params(tracker->params);
        MM_FREE(tracker);
        return 0;
}

static attr_value_t
get_parameters_attribute(void *arg, conf_object_t *obj, attr_value_t *idx)
{
        sample_linux_tracker_t *tracker = (sample_linux_tracker_t *)obj;
        attr_value_t params = attr_dict_from_params(tracker->params);
        return params;
}

static set_error_t
set_parameters_attribute(void *arg, conf_object_t *obj, attr_value_t *val,
                         attr_value_t *idx)
{
        if (is_loading_mc(obj)) {
                /* parameters cannot change between micro-checkpoints */
                return Sim_Set_Ok;
        }

        sample_linux_tracker_t *tracker = (sample_linux_tracker_t *)obj;
        if (tracker->enabled) {
                SIM_attribute_error("You may not change the parameters"
                                    " while the tracker is enabled");
                return Sim_Set_Illegal_Value;
        }
        /* It is good to verify that all required parameters are set and other
           possible sanity checks. Especially as this can be provided by the
           user. We do not do it here for simplicity. */
        params_t *params = attr_dict_to_params(*val);
        if (!params)
                return Sim_Set_Illegal_Value;

        free_params(tracker->params);
        tracker->params = params;

        SIM_LOG_INFO(2, obj, 0, "Setting parameter configuration for"
                     " Sample Linux Tracker");

        return Sim_Set_Ok;
}

static int
cmp_uint64_increasing(const void *a, const void *b)
{
        return *(uint64 *)a - *(uint64 *)b;
}

static uint64_vect_t
ht_int_to_sorted_vect(ht_int_table_t *ht)
{
        uint64_vect_t vect = VNULL;
        HT_FOREACH_INT(ht, it) {
                uint64 key = ht_iter_int_key(it);
                VADD(vect, key);
        }
        VSORT(vect, cmp_uint64_increasing);
        return vect;
}

static attr_value_t
get_tasks_attribute(void *arg, conf_object_t *obj, attr_value_t *idx)
{
        sample_linux_tracker_t *tracker = (sample_linux_tracker_t *)obj;
        uint64_vect_t tasks_vect = ht_int_to_sorted_vect(&tracker->tasks);
        int nr_tasks = VLEN(tasks_vect);
        attr_value_t tasks = SIM_alloc_attr_list(nr_tasks);
        for (int i = 0; i < nr_tasks; i++) {
                uint64 ts_addr = VGET(tasks_vect, i);
                SIM_attr_list_set_item(&tasks, i,
                                       SIM_make_attr_uint64(ts_addr));
        }
        VFREE(tasks_vect);
        return tasks;
}

static set_error_t
set_tasks_attribute(void *arg, conf_object_t *obj, attr_value_t *val,
                     attr_value_t *idx)
{
        sample_linux_tracker_t *tracker = (sample_linux_tracker_t *)obj;
        ht_delete_int_table(&tracker->tasks, true);
        for (int i = 0; i < SIM_attr_list_size(*val); i++) {
                uint64 ts_addr = SIM_attr_integer(SIM_attr_list_item(*val, i));
                insert_task(tracker, ts_addr);
        }
        return Sim_Set_Ok;
}

static attr_value_t
get_active_task_attribute(void *arg, conf_object_t *obj, attr_value_t *idx)
{
        sample_linux_tracker_t *tracker = (sample_linux_tracker_t *)obj;
        if (tracker->active_task.valid)
                return SIM_make_attr_uint64(tracker->active_task.val);
        return SIM_make_attr_nil();
}

static set_error_t
set_active_task_attribute(void *arg, conf_object_t *obj, attr_value_t *val,
                          attr_value_t *idx)
{
        sample_linux_tracker_t *tracker = (sample_linux_tracker_t *)obj;
        if (SIM_attr_is_nil(*val)) {
                tracker->active_task.valid = false;
        } else {
                tracker->active_task.valid = true;
                tracker->active_task.val = SIM_attr_integer(*val);
        }
        return Sim_Set_Ok;
}

static attr_value_t
get_booted_attribute(void *arg, conf_object_t *obj, attr_value_t *idx)
{
        sample_linux_tracker_t *tracker = (sample_linux_tracker_t *)obj;
        return SIM_make_attr_boolean(tracker->booted);
}

static set_error_t
set_booted_attribute(void *arg, conf_object_t *obj, attr_value_t *val,
                     attr_value_t *idx)
{
        sample_linux_tracker_t *tracker = (sample_linux_tracker_t *)obj;
        tracker->booted = SIM_attr_boolean(*val);
        return Sim_Set_Ok;
}

static attr_value_t
get_default_params_attribute(void *arg, conf_object_t *obj, attr_value_t *idx)
{
        params_t *params = default_parameters();
        attr_value_t params_dict = attr_dict_from_params(params);
        free_params(params);
        return params_dict;
}

static void
micro_checkpoints_started(conf_object_t *obj)
{
        sample_linux_tracker_t *tracker = (sample_linux_tracker_t *)obj;
        SIM_LOG_INFO(4, &tracker->obj, 0, "Micro checkpoint started");
        stop_monitoring(tracker);
}

static void
micro_checkpoints_finished(conf_object_t *obj)
{
        sample_linux_tracker_t *tracker = (sample_linux_tracker_t *)obj;
        SIM_LOG_INFO(4, &tracker->obj, 0, "Micro checkpoint finished");
        monitor_system_state_after_checkpoint(tracker);
}

static void
register_micro_checkpoint_iface(conf_class_t *cls)
{
        static const osa_micro_checkpoint_interface_t micro_checkpoint_iface = {
                .started = micro_checkpoints_started,
                .finished = micro_checkpoints_finished,
        };
        SIM_register_interface(cls, OSA_MICRO_CHECKPOINT_INTERFACE,
                               &micro_checkpoint_iface);
}

void
init_local()
{
        const class_data_t funcs = {
                .alloc_object = alloc_object,
                .init_object = init_object,
                .delete_instance = delete_instance,
                .class_desc = "sample linux tracker",
                .description = ("An example of how to write a tracker for Linux"
                                " that should work with the"
                                " qsp-linux-common.simics script."),
        };
        conf_class_t *cls = SIM_register_class(
                "sample_linux_tracker", &funcs);

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
                cls, "cpu",
                get_cpu_attribute, NULL, set_cpu_attribute, NULL,
                Sim_Attr_Optional | Sim_Attr_Internal, "o|n", NULL,
                "List of processors provided to the tracker");

        SIM_register_typed_attribute(
                cls, "enabled",
                get_enabled_attribute, NULL, NULL, NULL,
                Sim_Attr_Pseudo, "b", NULL, "Tracker enabled - read only");

        SIM_register_typed_attribute(
                cls, "parameters",
                get_parameters_attribute, NULL, set_parameters_attribute, NULL,
                Sim_Attr_Optional | Sim_Attr_Internal, "D", NULL,
                "The configuration parameters for Linux");

        SIM_register_typed_attribute(
                cls, "tasks",
                get_tasks_attribute, NULL, set_tasks_attribute, NULL,
                Sim_Attr_Optional | Sim_Attr_Internal, "[i*]", NULL,
                "Addresses of known tasks");

        SIM_register_typed_attribute(
                cls, "active_task",
                get_active_task_attribute, NULL,
                set_active_task_attribute, NULL,
                Sim_Attr_Optional | Sim_Attr_Internal, "i|n", NULL,
                "Tasks that is currently active, nil if no task is active");

        SIM_register_typed_attribute(
                cls, "booted",
                get_booted_attribute, NULL,
                set_booted_attribute, NULL,
                Sim_Attr_Optional | Sim_Attr_Internal, "b", NULL,
                "True if the tracker has found the system as booted");

        SIM_register_typed_attribute(
                cls, "default_parameters",
                get_default_params_attribute, NULL, NULL, NULL,
                Sim_Attr_Pseudo, "D", NULL,
                "Default parameters for the QSP platform");

        register_tracker_control_iface(cls);
        register_micro_checkpoint_iface(cls);
        sample_linux_mapper_init_local();
}
