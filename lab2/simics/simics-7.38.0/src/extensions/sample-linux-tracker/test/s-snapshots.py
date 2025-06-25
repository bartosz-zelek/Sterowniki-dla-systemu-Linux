# © 2016 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


import cli
import stest
import sample_linux_tracker_common as common

(sw, console) = common.setup_test_system()
cpu = common.get_cpu()
tracker = sw.tracker.tracker_obj

# Time to run before considering this to be a timeout.
timeout = 10000000000

class TasksData:
    def __init__(self):
        self.added = []
        self.name = None
        self.cancel_ids = []

def name_change(tasks_data, admin, cpu, node_id, key, old_val, new_val):
    if new_val != tasks_data.name:
        return
    tasks_data.added.append(node_id)
    SIM_break_simulation("Node '%s' added" % tasks_data.name)

def create_cb(tasks_data, admin, cpu, node_id):
    nt_query = admin.iface.osa_node_tree_query

    if nt_query.get_children(node_id):
        # Only add and break on threads
        return

    if nt_query.get_node(node_id).get("name") != tasks_data.name:
        nt_notify = admin.iface.osa_node_tree_notification
        # Install property change on the node as the expected name
        # will probably come after a rename.
        cid = nt_notify.notify_property_change(node_id, "name", False,
                                               name_change, tasks_data)
        tasks_data.cancel_ids.append(cid)
        return

    tasks_data.added.append(node_id)
    SIM_break_simulation("Node '%s' added" % tasks_data.name)

def destroy_cb(data, admin, cpu, node_id):
    SIM_break_simulation("Task %d removed" % node_id)

def run_until_node_added(sw, console, name):
    nt_notify = sw.iface.osa_node_tree_notification
    tasks_data = TasksData()
    tasks_data.name = name

    create_cid = nt_notify.notify_create(common.root_node(sw), True, create_cb,
                                         tasks_data)
    tasks_data.cancel_ids.append(create_cid)
    console.type_string(name + "\n")
    SIM_continue(timeout)
    for cid in tasks_data.cancel_ids:
        nt_notify.cancel_notify(cid)
    return tasks_data.added[-1]

def run_until_node_destroyed(sw, node_id):
    nt_notify = sw.iface.osa_node_tree_notification
    nt_query = sw.iface.osa_node_tree_query
    destroy_cid = nt_notify.notify_destroy(node_id, False, destroy_cb, None)
    SIM_continue(timeout)
    stest.expect_equal(nt_query.get_node(node_id), None)
    nt_notify.cancel_notify(destroy_cid)

(ok, rid_or_msg) = sw.iface.osa_control_v2.request("test")
stest.expect_true(ok, rid_or_msg)

common.boot_system(console)

SIM_take_snapshot("start")
tasks_at_start = tracker.tasks.copy()

added_id = run_until_node_added(sw, console, "ls")
tasks_after_add = tracker.tasks.copy()
stest.expect_different(tasks_at_start, tasks_after_add)
cycle_after_add = simics.SIM_cycle_count(cpu)
SIM_take_snapshot("after add")

run_until_node_destroyed(sw, added_id)
tasks_at_end = tracker.tasks.copy()
stest.expect_different(tasks_after_add, tasks_at_end)
cycle_at_end = simics.SIM_cycle_count(cpu)
SIM_take_snapshot("at end")

SIM_restore_snapshot("start")
stest.expect_equal(tracker.tasks, tasks_at_start)

cli.run_command("%s.bp-break-cycle -absolute %d" % (cpu.name, cycle_at_end))
SIM_continue(timeout)
stest.expect_equal(tracker.tasks, tasks_at_end)

SIM_restore_snapshot("after add")
stest.expect_equal(tracker.tasks, tasks_after_add)

SIM_restore_snapshot("at end")
stest.expect_equal(tracker.tasks, tasks_at_end)
