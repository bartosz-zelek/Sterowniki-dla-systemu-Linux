/*
  © 2016 Intel Corporation
*/

#ifndef EMPTY_SOFTWARE_TRACKER_H
#define EMPTY_SOFTWARE_TRACKER_H

#include <simics/device-api.h>
#include <simics/simulator-iface/os-awareness-tracker-interfaces.h>

typedef VECT(conf_object_t *) conf_object_vect_t;

typedef struct {
        conf_object_t *obj;
        const osa_tracker_state_admin_interface_t *state_admin;
        const osa_machine_query_interface_t *machine_query;
        const osa_machine_notification_interface_t *machine_notify;
} parent_tracker_t;

typedef struct {
        conf_object_t obj;
        parent_tracker_t parent;
        bool root_added;
        bool enabled;
        conf_object_vect_t cpus;
} empty_software_tracker_t;

#endif // EMPTY_SOFTWARE_TRACKER_H
