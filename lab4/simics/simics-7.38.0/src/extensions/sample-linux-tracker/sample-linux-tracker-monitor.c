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

#include "sample-linux-tracker-monitor.h"
#include "sample-linux-tracker-task-struct.h"
#include "sample-linux-tracker-common.h"

static void read_and_update_task_structs(sample_linux_tracker_t *tracker,
                                         conf_object_t *initiator);
typedef struct {
        sample_linux_tracker_t *tracker;
        uint64 ts_addr;
        int ref_cnt;
} ts_bp_data_t;

static void
ts_bp_free(void *data)
{
        ts_bp_data_t *bp_data = data;
        bp_data->ref_cnt--;
        ASSERT(bp_data->ref_cnt >= 0);
        if (bp_data->ref_cnt == 0)
                MM_FREE(data);
}

static const cbdata_type_t ts_bp_data_type = {
        "ts_bp_data",
        ts_bp_free
};

static ts_bp_data_t *
ts_bp_alloc(sample_linux_tracker_t *tracker, uint64 ts_addr)
{
        ts_bp_data_t *ts_data = MM_MALLOC(1, ts_bp_data_t);
        ts_data->ts_addr = ts_addr;
        ts_data->tracker = tracker;
        ts_data->ref_cnt = 0;
        return ts_data;
}

static uint64
get_ts_offset(uint64 hit_addr, uint64 ts_addr)
{
        return hit_addr - ts_addr;
}

static bool
contains_end_marker(uint64 val, unsigned len)
{
        for (int i = 0; i < len; i++) {
                if (((val >> (i * 8)) & 0xff) == 0)
                        return true;
        }
        return false;
}

static void
comm_update(cbdata_call_t data, conf_object_t *cpu, uint64 address,
            unsigned len, uint64 old, uint64 new)
{
        ts_bp_data_t *ts_data = SIM_cbdata_data(&data);
        sample_linux_tracker_t *tracker = ts_data->tracker;
        uint64 ts_addr = ts_data->ts_addr;
        uint64 comm_offset = (get_ts_offset(address, ts_addr)
                              - tracker->params->comm);
        uint64 last_byte = comm_offset + len - 1;
        if (last_byte >= COMM_LEN || contains_end_marker(new, len)) {
                task_t *task = read_task_struct(tracker, ts_addr);
                if (!task) {
                        SIM_LOG_ERROR(&tracker->obj, 0,
                                      "Could not read task struct for 0x%llx"
                                      " when updating comm field", ts_addr);
                        return;
                }
                transaction_id_t id = tracker->parent.state_admin->begin(
                        tracker->parent.obj, &tracker->obj,  cpu);
                attr_value_t name = SIM_make_attr_string(task->comm);
                tracker->parent.state_admin->set_attribute(
                        tracker->parent.obj, ts_addr, "name", name);
                SIM_attr_free(&name);
                tracker->parent.state_admin->end(tracker->parent.obj, id);
                free_task_struct(task);
        }
}

static void
tasks_update(cbdata_call_t data, conf_object_t *cpu, uint64 address,
             unsigned len, uint64 old, uint64 new)
{
        ts_bp_data_t *ts_data = SIM_cbdata_data(&data);
        sample_linux_tracker_t *tracker = ts_data->tracker;
        /* Next or prev pointer has been updated, go through the task list
           again to find added or removed tasks. */
        read_and_update_task_structs(tracker, cpu);
}

static cancel_id_t
plant_task_field_write_bp(sample_linux_tracker_t *tracker, uint64 vaddr,
                          int size, ts_bp_data_t *ts_data,
                          void (*cb)(cbdata_call_t data,
                                     conf_object_t *NOTNULL cpu,
                                     uint64 address, unsigned len, uint64 old,
                                     uint64 new))
{
        ts_data->ref_cnt++;
        cbdata_t cbdata = SIM_make_cbdata(&ts_bp_data_type, ts_data);
        cancel_id_t cid =
                tracker->parent.machine_notify->notify_write_breakpoint(
                        tracker->parent.obj, &tracker->obj, tracker->cpu, vaddr,
                        size, true, cb, cbdata);
        return cid;
}

static void
monitor_one_task(sample_linux_tracker_t *tracker, uint64 ts_addr)
{
        cancel_id_vect_t *cancel_ids = ht_lookup_int(&tracker->tasks, ts_addr);
        ASSERT(cancel_ids);
        if (VLEN(*cancel_ids) != 0) {
                SIM_LOG_ERROR(&tracker->obj, 0,
                              "Trying to install monitoring for task 0x%llx"
                              " which already is monitored", ts_addr);
                return;
        }
        ts_bp_data_t *ts_data = ts_bp_alloc(tracker, ts_addr);
        cancel_id_t cid = plant_task_field_write_bp(
                tracker, ts_addr + tracker->params->comm, COMM_LEN, ts_data,
                comm_update);
        VADD(*cancel_ids, cid);

        cid = plant_task_field_write_bp(
                tracker, ts_addr + tracker->params->tasks, 2 * POINTER_SIZE,
                ts_data, tasks_update);
        VADD(*cancel_ids, cid);
}

static void
monitor_task_changes(sample_linux_tracker_t *tracker)
{
        HT_FOREACH_INT(&tracker->tasks, it) {
                uint64 ts_addr = ht_iter_int_key(it);
                monitor_one_task(tracker, ts_addr);
        }
}

static attr_value_t
properties_from_task(task_t *ts)
{
        attr_value_t task = SIM_alloc_attr_dict(4);
        SIM_attr_dict_set_item(&task, 0, SIM_make_attr_string("pid"),
                               SIM_make_attr_uint64(ts->pid));
        SIM_attr_dict_set_item(&task, 1, SIM_make_attr_string("tid"),
                               SIM_make_attr_uint64(ts->tid));
        SIM_attr_dict_set_item(&task, 2, SIM_make_attr_string("name"),
                               SIM_make_attr_string(ts->comm));
        SIM_attr_dict_set_item(&task, 3, SIM_make_attr_string("is_kernel"),
                               SIM_make_attr_boolean(is_kernel_task(ts)));
        return task;
}

static void
free_task(sample_linux_tracker_t *tracker, uint64 ts_addr)
{
        cancel_id_vect_t *cancel_ids = ht_lookup_int(&tracker->tasks, ts_addr);
        if (!cancel_ids) {
                SIM_LOG_ERROR(&tracker->obj, 0,
                              "Free task 0x%llx which does not exist", ts_addr);
                return;
        }
        MM_FREE(cancel_ids);
        ht_remove_int(&tracker->tasks, ts_addr);
}

static void
stop_monitoring_one_task(sample_linux_tracker_t *tracker, uint64 ts_addr)
{
        cancel_id_vect_t *cancel_ids = ht_lookup_int(&tracker->tasks, ts_addr);
        if (!cancel_ids) {
                SIM_LOG_ERROR(&tracker->obj, 0,
                              "Trying to stop monitoring for task 0x%llx which"
                              " is not monitored", ts_addr);
                return;
        }
        cancel_one_task(tracker, cancel_ids);
}

static void
stop_monitoring_all_tasks(sample_linux_tracker_t *tracker)
{
        HT_FOREACH_INT(&tracker->tasks, it) {
                cancel_id_vect_t *cancel_ids = ht_iter_int_value(it);
                cancel_one_task(tracker, cancel_ids);
        }
}


static void
add_task(sample_linux_tracker_t *tracker, uint64 ts_addr)
{
        task_t *ts = read_task_struct(tracker, ts_addr);
        if (!ts) {
                SIM_LOG_ERROR(&tracker->obj, 0,
                              "Could not read task struct at 0x%llx", ts_addr);
                return;
        }
        insert_task(tracker, ts_addr);
        attr_value_t props = properties_from_task(ts);
        tracker->parent.state_admin->add(tracker->parent.obj, ts_addr, props);
        SIM_attr_free(&props);
        free_task_struct(ts);
        monitor_one_task(tracker, ts_addr);
}

static void
remove_task(sample_linux_tracker_t *tracker, uint64 ts_addr)
{
        tracker->parent.state_admin->remove(tracker->parent.obj, ts_addr);
        stop_monitoring_one_task(tracker, ts_addr);
        free_task(tracker, ts_addr);
}

static uint64_vect_t
new_tasks(sample_linux_tracker_t *tracker, uint64_vect_t all_tasks)
{
        uint64_vect_t new = VNULL;
        VFORT(all_tasks, uint64, ts_addr) {
                if (!ht_lookup_int(&tracker->tasks, ts_addr))
                        VADD(new, ts_addr);
        }
        return new;
}

static uint64_vect_t
removed_tasks(sample_linux_tracker_t *tracker, uint64_vect_t all_tasks)
{
        uint64_vect_t removed = VNULL;
        HT_FOREACH_INT(&tracker->tasks, it) {
                uint64 ts_addr = ht_iter_int_key(it);
                if (!vect_contains_value(all_tasks, ts_addr))
                        VADD(removed, ts_addr);
        }
        return removed;
}

/* If the task list is stable, send out notifications about all removed or
   added task structs. */
static void
read_and_update_task_structs(sample_linux_tracker_t *tracker,
                             conf_object_t *initiator)
{
        uint64_vect_t task_structs = get_task_structs(tracker);
        if (VEMPTY(task_structs)) {
                /* No stable task structs */
                return;
        }
        uint64_vect_t new = new_tasks(tracker, task_structs);
        uint64_vect_t removed = removed_tasks(tracker, task_structs);
        VFREE(task_structs);
        SIM_LOG_INFO(4, &tracker->obj, 0,
                     "Adding %d new tasks", VLEN(new));
        SIM_LOG_INFO(4, &tracker->obj, 0,
                     "Removing %d old tasks", VLEN(removed));
        /* Must do all adds and removes within a transaction */
        transaction_id_t id = tracker->parent.state_admin->begin(
                tracker->parent.obj, &tracker->obj,  initiator);
        VFORT(new, uint64, ts_addr) {
                add_task(tracker, ts_addr);
        }
        VFORT(removed, uint64, ts_addr) {
                remove_task(tracker, ts_addr);
        }
        tracker->parent.state_admin->end(tracker->parent.obj, id);
        VFREE(new);
        VFREE(removed);
}

static void
deactivate_old_active_task(sample_linux_tracker_t *tracker,
                           conf_object_t *initiator)
{
        if (!tracker->active_task.valid)
                return;
        transaction_id_t id = tracker->parent.state_admin->begin(
                tracker->parent.obj, &tracker->obj, initiator);
        tracker->parent.state_admin->set_attribute(
                tracker->parent.obj, tracker->active_task.val, "cpu",
                SIM_make_attr_nil());
        tracker->parent.state_admin->end(tracker->parent.obj, id);
}

static void
set_active_task(sample_linux_tracker_t *tracker, conf_object_t *initiator,
                uint64 curr_task)
{
        conf_object_t *cpu = tracker->cpu;
        processor_mode_t mode = tracker->parent.machine_query->cpu_mode(
                tracker->parent.obj, &tracker->obj, cpu);

        if (!ht_lookup_int(&tracker->tasks, curr_task)) {
                if (mode == Sim_CPU_Mode_User) {
                        SIM_LOG_ERROR(&tracker->obj, 0,
                                      "Active task, which is running in user"
                                      " mode, not found among known tasks");
                        return;
                }
                /* An unknown task that becomes active usually comes from that
                   the kernel might set a task that is being removed as active
                   during the time it is being removed. For this case we set
                   the root entity as active an let the mapper set the other
                   node as active. */
                curr_task = ROOT_ENTITY_ID;
        }
        transaction_id_t id = tracker->parent.state_admin->begin(
                tracker->parent.obj, &tracker->obj, initiator);
        deactivate_old_active_task(tracker, initiator);
        attr_value_t cpu_and_mode = SIM_make_attr_list(
                2, SIM_make_attr_object(cpu), SIM_make_attr_uint64(mode));

        SIM_LOG_INFO(3, &tracker->obj, 0,
                     "Setting active task 0x%llx to ['%s', %d]",
                     curr_task, SIM_object_name(cpu), mode);
        tracker->parent.state_admin->set_attribute(
                tracker->parent.obj, curr_task, "cpu", cpu_and_mode);
        tracker->parent.state_admin->end(tracker->parent.obj, id);
        tracker->active_task = (maybe_uint64_t){true, curr_task};
        SIM_attr_free(&cpu_and_mode);
}

static void
update_active_task(sample_linux_tracker_t *tracker,
                        conf_object_t *initiator)
{
        maybe_uint64_t curr_task = get_current_task(tracker);
        if (!curr_task.valid)
                return;
        set_active_task(tracker, initiator, curr_task.val);
}

static void
current_task_update(cbdata_call_t data, conf_object_t *cpu, uint64 address,
                    unsigned len, uint64 old, uint64 new)
{
        sample_linux_tracker_t *tracker = SIM_cbdata_data(&data);
        SIM_LOG_INFO(3, &tracker->obj, 0, "Current task updated to: 0x%llx",
                     new);
        task_t *ts = read_task_struct(tracker, new);
        if (!ts) {
                SIM_LOG_ERROR(&tracker->obj, 0,
                              "Bad active task at 0x%llx", new);
                return;
        }
        SIM_LOG_INFO(3, &tracker->obj, 0, "New active task is '%s'",
                     ts->comm);
        free_task_struct(ts);
        set_active_task(tracker, cpu, new);
}

static void
monitor_current_task(sample_linux_tracker_t *tracker)
{
        maybe_uint64_t curr_task_addr = get_current_task_addr(tracker);
        if (!curr_task_addr.valid) {
                SIM_LOG_ERROR(&tracker->obj, 0,
                              "Could not monitor current task as the task"
                              " address could not be retrieved");
                return;
        }
        tracker->curr_task_monitor_cid =
                tracker->parent.machine_notify->notify_write_breakpoint(
                        tracker->parent.obj, &tracker->obj,
                        tracker->cpu, curr_task_addr.val, POINTER_SIZE, true,
                        current_task_update, SIM_make_simple_cbdata(tracker));
}

static void
mode_changed(cbdata_call_t data, conf_object_t *cpu,
             processor_mode_t old_mode, processor_mode_t new_mode)
{
        sample_linux_tracker_t *tracker = SIM_cbdata_data(&data);
        update_active_task(tracker, cpu);
}

static void
monitor_mode_change(sample_linux_tracker_t *tracker)
{
        tracker->mode_change_cid =
                tracker->parent.machine_notify->notify_mode_change(
                        tracker->parent.obj, &tracker->obj,
                        tracker->cpu, mode_changed,
                        SIM_make_simple_cbdata(tracker));
}

static void
cancel_boot_monitor(sample_linux_tracker_t *tracker)
{
        if (tracker->boot_monitor_cid) {
                tracker->parent.machine_notify->cancel(
                        tracker->parent.obj, &tracker->obj,
                        tracker->boot_monitor_cid);
                tracker->boot_monitor_cid = 0;
        }
}

void
stop_monitoring(sample_linux_tracker_t *tracker)
{
        if (!tracker->booted) {
                cancel_boot_monitor(tracker);
                return;
        }
        if (tracker->mode_change_cid) {
                tracker->parent.machine_notify->cancel(
                        tracker->parent.obj, &tracker->obj,
                        tracker->mode_change_cid);
                tracker->mode_change_cid = 0;
        }
        if (tracker->curr_task_monitor_cid) {
                tracker->parent.machine_notify->cancel(
                        tracker->parent.obj, &tracker->obj,
                        tracker->curr_task_monitor_cid);
                tracker->curr_task_monitor_cid = 0;
        }
        stop_monitoring_all_tasks(tracker);
}

static void
monitor_os_changes(sample_linux_tracker_t *tracker)
{
        monitor_current_task(tracker);
        monitor_mode_change(tracker);
}

static void
monitor_system_state_booted(sample_linux_tracker_t *tracker,
                            conf_object_t *initiator)
{
        transaction_id_t tx_id = tracker->parent.state_admin->begin(
                tracker->parent.obj, &tracker->obj, initiator);
        read_and_update_task_structs(tracker, initiator);
        monitor_os_changes(tracker);
        update_active_task(tracker, initiator);
        tracker->parent.state_admin->end(tracker->parent.obj, tx_id);
}

static bool
is_booted(sample_linux_tracker_t *tracker)
{
        if (tracker->booted)
                return true;

        uint64_vect_t task_structs = get_task_structs(tracker);
        int nr_of_tasks = VLEN(task_structs);
        VFREE(task_structs);
        if (nr_of_tasks <= 4)
                return false;
        tracker->booted = true;
        return true;
}

static void
boot_monitor_mode_change(cbdata_call_t data, conf_object_t *cpu,
                         processor_mode_t old_mode,
                         processor_mode_t new_mode)
{
        sample_linux_tracker_t *tracker = SIM_cbdata_data(&data);
        /* Changes to task structs can only have occur when mode is changed
           from kernel to user mode. */
        if (new_mode != Sim_CPU_Mode_User || !is_booted(tracker))
                return;
        SIM_LOG_INFO(3, &tracker->obj, 0, "Linux became booted");
        cancel_boot_monitor(tracker);
        monitor_system_state_booted(tracker, cpu);
}

static void
monitor_boot_state(sample_linux_tracker_t *tracker)
{
        tracker->boot_monitor_cid =
                tracker->parent.machine_notify->notify_mode_change(
                        tracker->parent.obj, &tracker->obj,
                        tracker->cpu, boot_monitor_mode_change,
                        SIM_make_simple_cbdata(tracker));
}

void
monitor_system_state(sample_linux_tracker_t *tracker, conf_object_t *initiator)
{
        if (!is_booted(tracker)) {
                monitor_boot_state(tracker);
                return;
        }
        monitor_system_state_booted(tracker, initiator);
}

void
monitor_system_state_after_checkpoint(sample_linux_tracker_t *tracker)
{
        if (!tracker->booted) {
                monitor_boot_state(tracker);
                return;
        }
        monitor_os_changes(tracker);
        monitor_task_changes(tracker);
}
