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


# Test that the mapper handles adding, updating, activating and
# removing entities.

import cli
import simics
import stest
import sample_linux_tracker_common as common

(sw, _) = common.setup_test_system()
cpu = conf.board.mb.cpu0.core[0][0]

(ok, rid_or_msg) = sw.iface.osa_control_v2.request("test")
stest.expect_true(ok, rid_or_msg)

tracker = sw.tracker.tracker_obj
shadow = common.ShadowTracker(sw, tracker)

def nt_query(sw):
    return sw.iface.osa_node_tree_query

def make_entity(shadow, entity_id, name, tid, is_kernel, active, mode):
    props = {'name': name, 'pid': tid, 'tid': tid, 'is_kernel': is_kernel}
    shadow.add_entity(None, entity_id, props, active, mode)

def add_entity(sw, entity_id, parent_node_id, name, tid, is_kernel, active):
    stest.expect_different(parent_node_id, None)
    old_children = nt_query(sw).get_children(parent_node_id)
    mode = (simics.Sim_CPU_Mode_Supervisor if is_kernel
            else simics.Sim_CPU_Mode_User)
    make_entity(shadow, entity_id, name, tid, is_kernel, active, mode)
    children = nt_query(sw).get_children(parent_node_id)
    new_child = set(children) - set(old_children)
    stest.expect_equal(len(new_child), 1)
    child_id = new_child.pop()
    child = nt_query(sw).get_node(child_id)

    stest.expect_equal(child['name'], name)

    return (child_id, child)

def remove_entity(sw, entity_id):
    shadow.remove_entity(None, entity_id)

def update_entity(sw, entity_id, props):
    shadow.update_entity(None, entity_id, props)

def make_active(sw, entity_id, cpu, mode):
    shadow.make_active(None, entity_id, cpu, mode)

# This should create one node under the "Threads" node
def verify_add_kernel_entity(sw, entity_id, name, tid, active):
    parent_node_id = common.threads_node(sw)
    (child_id, child) = add_entity(
        sw, entity_id, parent_node_id, name, tid, True, active)

    stest.expect_equal(child['memory_space'], common.kernel(sw))
    stest.expect_equal(child['tid'], tid)
    stest.expect_false('pid' in child)

    if active:
        stest.expect_true(common.is_active(sw, cpu, child_id))
    return child_id

def verify_user_thread_props(thread, name, tid, process_node_id):
    stest.expect_equal(thread['name'], name)
    stest.expect_equal(thread['pid'], tid)
    stest.expect_equal(thread['tid'], tid)
    stest.expect_equal(thread['memory_space'], process_node_id)
    stest.expect_false(thread['multiprocessor'])

# This should create a process node, with a thread node, under the Userspace
# node
def verify_add_user_entity(sw, entity_id, name, tid, active):
    userspace_node_id = common.userspace(sw)
    (process_id, process) = add_entity(
        sw, entity_id, userspace_node_id, name, tid, False, active)

    threads = nt_query(sw).get_children(process_id)
    stest.expect_equal(len(threads), 1)
    thread_id = threads[0]
    thread = nt_query(sw).get_node(thread_id)

    stest.expect_equal(process['pid'], tid)
    stest.expect_false('tid' in process)
    stest.expect_true(process['multiprocessor'])
    stest.expect_equal(process['memory_space'], process_id)

    verify_user_thread_props(thread, name, tid, process_id)
    return (process_id, thread_id)

def verify_remove_entity(sw, entity_id, remove_nodes):
    remove_entity(sw, entity_id)
    for node_id in remove_nodes:
        node = nt_query(sw).get_node(node_id)
        stest.expect_equal(node, None)

def verify_update_kernel_entity(sw, entity_id, node_id, props):
    update_entity(sw, entity_id, props)
    node = nt_query(sw).get_node(node_id)
    for (key, val) in props.items():
        stest.expect_equal(node[key], val)

def verify_update_user_entity(sw, entity_id, node_id, props):
    update_entity(sw, entity_id, props)
    node = nt_query(sw).get_node(node_id)
    child_id = nt_query(sw).get_children(node_id)[0]
    thread = nt_query(sw).get_node(child_id)
    for (key, val) in props.items():
        stest.expect_equal(node[key], val)
        stest.expect_equal(thread[key], val)

def verify_make_active(sw, entity_id, node_id, cpu, mode):
    make_active(sw, entity_id, cpu, mode)
    stest.expect_true(common.is_active(sw, cpu, node_id))

def verify_make_active_user_in_supervisor_mode(sw, entity_id, node_id, cpu):
    make_active(sw, entity_id, cpu, simics.Sim_CPU_Mode_Supervisor)
    stest.expect_true(common.is_active(sw, cpu, common.other_node(sw)))

node_101 = verify_add_kernel_entity(sw, 101, "foo", 1, None)
node_102 = verify_add_kernel_entity(sw, 102, "bar", 2, cpu)
node_103 = verify_add_kernel_entity(sw, 103, "foobar", 3, None)
(node_104, node_104_thread) = verify_add_user_entity(sw, 104, "barfoo", 4, None)
(node_105, node_105_thread) = verify_add_user_entity(sw, 105, "cafe", 5, None)

verify_update_kernel_entity(sw, 101, node_101, {'name': "cool"})
verify_update_user_entity(sw, 105, node_105, {'name': "closed"})

verify_make_active(sw, 101, node_101, cpu, simics.Sim_CPU_Mode_Supervisor)
verify_make_active(sw, 104, node_104, cpu, simics.Sim_CPU_Mode_User)
verify_make_active_user_in_supervisor_mode(sw, 104, node_104, cpu)

verify_remove_entity(sw, 101, (node_101,))
verify_remove_entity(sw, 102, (node_102,))
verify_remove_entity(sw, 103, (node_103,))

verify_remove_entity(sw, 104, (node_104, node_104_thread))
verify_remove_entity(sw, 105, (node_105, node_105_thread))

sw.iface.osa_control_v2.release(rid_or_msg)
