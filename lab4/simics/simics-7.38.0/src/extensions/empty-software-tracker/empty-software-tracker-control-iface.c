/*
  © 2016 Intel Corporation
*/

#include "empty-software-tracker-control-iface.h"
#include <simics/device-api.h>
#include <simics/simulator-iface/os-awareness-tracker-interfaces.h>
#include "empty-software-tracker.h"
#include "empty-software-tracker-common.h"

static attr_value_t
get_default_root_props()
{
        attr_value_t name_props = SIM_alloc_attr_dict(1);
        SIM_attr_dict_set_item(&name_props, 0, SIM_make_attr_string("name"),
                               SIM_make_attr_string("tracker root"));
        return name_props;
}

static void
add_root_entity(empty_software_tracker_t *tracker)
{
        attr_value_t root_props = get_default_root_props();
        transaction_id_t id = tracker->parent.state_admin->begin(
                tracker->parent.obj, &tracker->obj,  NULL);
        tracker->parent.state_admin->add(tracker->parent.obj, ROOT_ENTITY_ID,
                                         root_props);
        tracker->parent.state_admin->end(tracker->parent.obj, id);
        SIM_attr_free(&root_props);
        tracker->root_added = true;
}

static bool
has_state(empty_software_tracker_t *tracker)
{
        return tracker->root_added;
}

static bool
enable(conf_object_t *obj)
{
        empty_software_tracker_t *tracker = (empty_software_tracker_t *)obj;
        SIM_LOG_INFO(2, &tracker->obj, 0, "Enabling tracker");
        if (tracker->enabled) {
                /* Should not happen, but lets ignore if it does. */
                return true;
        }
        tracker->enabled = true;

        if (has_state(tracker)) {
                /* The tracker is enabled from a checkpointed state, no new
                   entities should be added. */
                return true;
        }
        add_root_entity(tracker);
        return true;
}

static void
remove_all_processors(empty_software_tracker_t *tracker)
{
        VFREE(tracker->cpus);
}

static void
clear_internal_state(empty_software_tracker_t *tracker)
{
        tracker->root_added = false;
        remove_all_processors(tracker);
}

static void
disable(conf_object_t *obj)
{
        empty_software_tracker_t *tracker = (empty_software_tracker_t *)obj;
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
        empty_software_tracker_t *tracker = (empty_software_tracker_t *)obj;
        SIM_LOG_INFO(3, &tracker->obj, 0, "Clearing state");
        clear_internal_state(tracker);
}

static bool
add_processor(conf_object_t *obj, conf_object_t *cpu)
{
        empty_software_tracker_t *tracker = (empty_software_tracker_t *)obj;
        SIM_LOG_INFO(3, &tracker->obj, 0,
                     "Adding processor '%s'", SIM_object_name(cpu));
        VADD(tracker->cpus, cpu);
        return true;
}

static bool
remove_processor(conf_object_t *obj, conf_object_t *cpu)
{
        empty_software_tracker_t *tracker = (empty_software_tracker_t *)obj;
        SIM_LOG_INFO(3, &tracker->obj, 0,
                     "Removing processor '%s'", SIM_object_name(cpu));
        VREMOVE_FIRST_MATCH(tracker->cpus, cpu);
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
