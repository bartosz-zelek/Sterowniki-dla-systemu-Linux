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

#include "sample-linux-tracker-control-iface.h"
#include <simics/device-api.h>
#include <simics/simulator-iface/os-awareness-tracker-interfaces.h>
#include "sample-linux-tracker.h"
#include "sample-linux-tracker-common.h"
#include "sample-linux-tracker-monitor.h"

static attr_value_t
get_default_root_props(sample_linux_tracker_t *tracker)
{
        attr_value_t name_props = SIM_alloc_attr_dict(1);
        SIM_attr_dict_set_item(&name_props, 0, SIM_make_attr_string("name"),
                               SIM_make_attr_string(tracker->params->name));
        return name_props;
}

static void
add_root_entity(sample_linux_tracker_t *tracker)
{
        attr_value_t root_props = get_default_root_props(tracker);
        transaction_id_t id = tracker->parent.state_admin->begin(
                tracker->parent.obj, &tracker->obj,  NULL);
        tracker->parent.state_admin->add(tracker->parent.obj, ROOT_ENTITY_ID,
                                         root_props);
        tracker->parent.state_admin->end(tracker->parent.obj, id);
        SIM_attr_free(&root_props);
        tracker->root_added = true;
}

static bool
has_state(sample_linux_tracker_t *tracker)
{
        return tracker->root_added;
}

static bool
enable(conf_object_t *obj)
{
        sample_linux_tracker_t *tracker = (sample_linux_tracker_t *)obj;
        SIM_LOG_INFO(2, &tracker->obj, 0, "Enabling tracker");
        if (tracker->enabled) {
                /* Should not happen, but lets ignore if it does. */
                return true;
        }
        tracker->enabled = true;

        if (has_state(tracker)) {
                /* The tracker is enabled from a checkpointed state, no new
                   entities should be added, but notifications should be
                   installed. */
                monitor_system_state_after_checkpoint(tracker);
                return true;
        }
        add_root_entity(tracker);
        return true;
}

static void
clear_internal_state(sample_linux_tracker_t *tracker)
{
        tracker->root_added = false;
        tracker->booted = false;
        tracker->active_task.valid = false;
        tracker->cpu = NULL;
        stop_monitoring(tracker);
        ht_delete_int_table(&tracker->tasks, true);
}

static void
disable(conf_object_t *obj)
{
        sample_linux_tracker_t *tracker = (sample_linux_tracker_t *)obj;
        SIM_LOG_INFO(2, &tracker->obj, 0, "Disabling tracker");
        if (!tracker->enabled) {
                /* Should not happen, but lets ignore if it does. */
                return;
        }
        clear_internal_state(tracker);
        tracker->enabled = false;
}

static void
clear_state(conf_object_t *obj)
{
        sample_linux_tracker_t *tracker = (sample_linux_tracker_t *)obj;
        SIM_LOG_INFO(3, &tracker->obj, 0, "Clearing state");
        clear_internal_state(tracker);
}

static bool
add_processor(conf_object_t *obj, conf_object_t *cpu)
{
        sample_linux_tracker_t *tracker = (sample_linux_tracker_t *)obj;
        SIM_LOG_INFO(3, &tracker->obj, 0,
                     "Adding processor '%s'", SIM_object_name(cpu));
        if (tracker->cpu) {
                SIM_LOG_ERROR(&tracker->obj, 0, "Sample Linux tracker only"
                              " supports uniprocessor system");
                return false;
        }
        tracker->cpu = cpu;

        if (tracker->enabled && tracker->params)
                monitor_system_state(tracker, NULL);
        return true;
}

static bool
remove_processor(conf_object_t *obj, conf_object_t *cpu)
{
        sample_linux_tracker_t *tracker = (sample_linux_tracker_t *)obj;
        SIM_LOG_INFO(3, &tracker->obj, 0,
                     "Removing processor '%s'", SIM_object_name(cpu));
        if (cpu != tracker->cpu) {
                SIM_LOG_ERROR(&tracker->obj, 0,
                              "Removing unknown processors '%s'",
                              SIM_object_name(cpu));
                return false;
        }
        tracker->cpu = NULL;
        stop_monitoring(tracker);
        return true;
}

void
register_tracker_control_iface(conf_class_t *cls)
{
        static const osa_tracker_control_interface_t tracker_control_iface = {
                .disable = disable,
                .enable = enable,
                .clear_state = clear_state,
                .add_processor = add_processor,
                .remove_processor = remove_processor,
        };
        SIM_register_interface(cls, OSA_TRACKER_CONTROL_INTERFACE,
                               &tracker_control_iface);
}
