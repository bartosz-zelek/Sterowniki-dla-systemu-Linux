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

#ifndef SAMPLE_LINUX_TRACKER_H
#define SAMPLE_LINUX_TRACKER_H

#include <simics/device-api.h>
#include <simics/simulator-iface/os-awareness-tracker-interfaces.h>
#include "sample-linux-tracker-params.h"

#define POINTER_SIZE 8

typedef VECT(uint64) uint64_vect_t;
typedef VECT(cancel_id_t) cancel_id_vect_t;

typedef struct {
        bool valid;
        uint64 val;
} maybe_uint64_t;

typedef struct {
        conf_object_t *obj;
        const osa_tracker_state_admin_interface_t *state_admin;
        const osa_machine_query_interface_t *machine_query;
        const osa_machine_notification_interface_t *machine_notify;
} parent_tracker_t;

typedef struct sample_linux_tracker {
        conf_object_t obj;
        parent_tracker_t parent;
        bool root_added;
        bool enabled;
        conf_object_t *cpu;
        params_t *params;
        /* tasks: ts_addr (uint64) -> cancel_id_vect_t *, for tasks that have
           not installed any notifications the vector will be an empty list. */
        ht_int_table_t tasks;
        cancel_id_t curr_task_monitor_cid;
        cancel_id_t mode_change_cid;
        cancel_id_t boot_monitor_cid;
        maybe_uint64_t active_task;
        bool booted;
} sample_linux_tracker_t;

bool vect_contains_value(uint64_vect_t vect, uint64 value);
void insert_task(sample_linux_tracker_t *tracker, uint64 ts_addr);
void cancel_one_task(sample_linux_tracker_t *tracker,
                     cancel_id_vect_t *cancel_ids);

#endif // SAMPLE_LINUX_TRACKER_H
