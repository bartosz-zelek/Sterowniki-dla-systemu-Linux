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


import simics
import stest
import sample_linux_tracker_common as common

def get_mapper_checkpoint_file():
    return stest.scratch_file("mapper-test.ckpt")

def get_tracker_checkpoint_file():
    return stest.scratch_file("tracker-test.ckpt")

def make_entity(shadow, entity_id, name, tid, is_kernel):
    props = {'name': name, 'pid': tid, 'tid': tid, 'is_kernel': is_kernel}
    shadow.add_entity(None, entity_id, props, None, None)

def make_user_entity(shadow, entity_id, name, pid):
    make_entity(shadow, entity_id, name, pid, False)

def make_kernel_entity(shadow, entity_id, name, tid):
    make_entity(shadow, entity_id, name, tid, True)

def user_tasks():
    return ((0x1000, "first", 10),
            (0x1010, "second", 20),
            (0x2200, "third", 33),)

def kernel_threads():
    return ((0xf001000, "k1", 110),
            (0xf002000, "k2", 120),)

def get_tracker(sw):
    return sw.tracker.iface.osa_tracker_component.get_tracker()

def build_node_tree(sw):
    tracker = get_tracker(sw)
    shadow = common.ShadowTracker(sw, tracker)

    for (entity, name, pid) in user_tasks():
        make_user_entity(shadow, entity, name, pid)

    for (entity, name, tid) in kernel_threads():
        make_kernel_entity(shadow, entity, name, tid)

def nt_query(sw):
    return sw.iface.osa_node_tree_query

def collect_props(sw, node_ids, prop_names):
    collected = []
    for node_id in node_ids:
        props = nt_query(sw).get_node(node_id)
        collected.append(tuple([props[prop] for prop in prop_names]))
    return collected

def collect_names_and_tids(sw, node_ids):
    return collect_props(sw, node_ids, ("name", "tid"))

def collect_names_and_pids(sw, node_ids):
    return collect_props(sw, node_ids, ("name", "pid"))

def expect_kernel_nodes(sw):
    threads_node_id = common.threads_node(sw)
    threads_children = nt_query(sw).get_children(threads_node_id)
    found_kernel_threads = set(collect_names_and_tids(sw, threads_children))
    for (_, name, tid) in kernel_threads():
        stest.expect_true((name, tid) in found_kernel_threads)
        found_kernel_threads.remove((name, tid))
    stest.expect_equal(found_kernel_threads, set())

def expect_user_nodes(sw):
    userspace_node = common.userspace(sw)
    userspace_children = nt_query(sw).get_children(userspace_node)
    found_userspace_processes = set(
        collect_names_and_pids(sw, userspace_children))
    for (_, name, pid) in user_tasks():
        stest.expect_true((name, pid) in found_userspace_processes)
        found_userspace_processes.remove((name, pid))
    stest.expect_equal(found_userspace_processes, set())

def expect_node_tree(sw):
    expect_kernel_nodes(sw)
    expect_user_nodes(sw)

def test_adding_user_tasks(sw, shadow):
    userspace_node = common.userspace(sw)
    children = nt_query(sw).get_children(userspace_node)

    entity_id = 0x90000
    name = "new process"
    pid = 90000
    make_user_entity(shadow, entity_id, name, pid)

    new_children = nt_query(sw).get_children(userspace_node)
    stest.expect_equal(len(new_children), len(children) + 1)

    new_child = (set(new_children) - set(children)).pop()
    props = nt_query(sw).get_node(new_child)
    stest.expect_equal(props["name"], name)
    stest.expect_equal(props["pid"], pid)

def test_adding_kernel_tasks(sw, shadow):
    threads_node = common.threads_node(sw)
    children = nt_query(sw).get_children(threads_node)

    entity_id = 0xf090000
    name = "knew"
    tid = 1230000
    make_kernel_entity(shadow, entity_id, name, tid)

    new_children = nt_query(sw).get_children(threads_node)
    stest.expect_equal(len(new_children), len(children) + 1)

    new_child = (set(new_children) - set(children)).pop()
    props = nt_query(sw).get_node(new_child)
    stest.expect_equal(props["name"], name)
    stest.expect_equal(props["tid"], tid)

def test_adding_tasks(sw):
    tracker = get_tracker(sw)
    shadow = common.ShadowTracker(sw, tracker)
    test_adding_user_tasks(sw, shadow)
    test_adding_kernel_tasks(sw, shadow)

def get_child_node_by_name(sw, parent_node, name):
    children = nt_query(sw).get_children(parent_node)
    for child in children:
        if nt_query(sw).get_node(child)["name"] == name:
            return child
    return None

def test_activating_tasks(sw, cpu):
    tracker = get_tracker(sw)
    shadow = common.ShadowTracker(sw, tracker)

    (user_entity, user_name, _) = user_tasks()[1]
    shadow.make_active(None, user_entity, cpu, simics.Sim_CPU_Mode_User)

    user_node = get_child_node_by_name(sw, common.userspace(sw), user_name)
    stest.expect_true(common.is_active(sw, cpu, user_node))

    (kernel_entity, kernel_name, _) = kernel_threads()[1]
    shadow.make_active(None, kernel_entity, cpu, simics.Sim_CPU_Mode_Supervisor)

    kernel_node = get_child_node_by_name(sw, common.threads_node(sw),
                                         kernel_name)
    stest.expect_true(common.is_active(sw, cpu, kernel_node))

def test_removing_tasks(sw):
    tracker = get_tracker(sw)
    shadow = common.ShadowTracker(sw, tracker)

    userspace_node = common.userspace(sw)
    user_children = nt_query(sw).get_children(userspace_node)

    threads_node = common.threads_node(sw)
    threads_children = nt_query(sw).get_children(threads_node)

    user_entity_to_remove = user_tasks()[0][0]
    shadow.remove_entity(None, user_entity_to_remove)
    new_user_children = nt_query(sw).get_children(userspace_node)
    stest.expect_equal(len(new_user_children), len(user_children) - 1)

    kernel_entity_to_remove = kernel_threads()[0][0]
    shadow.remove_entity(None, kernel_entity_to_remove)
    new_threads_children = nt_query(sw).get_children(threads_node)
    stest.expect_equal(len(new_threads_children), len(threads_children) - 1)
