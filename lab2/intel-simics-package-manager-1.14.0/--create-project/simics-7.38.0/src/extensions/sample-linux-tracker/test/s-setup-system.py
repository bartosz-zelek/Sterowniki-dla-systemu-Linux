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
tracker = sw.tracker.tracker_obj
mapper = sw.tracker.mapper_obj

def contains_props(props, exp_props):
    for key in exp_props:
        if props.get(key) != exp_props[key]:
            return False
    return True

def node_in_children(sw, parent, exp_props):
    nt_query = sw.iface.osa_node_tree_query
    user_process_ids = nt_query.get_children(parent)
    for node_id in user_process_ids:
        props = nt_query.get_node(node_id)
        if contains_props(props, exp_props):
            return True
    return False

def expect_init_user_process(sw):
    userspace_node = common.userspace(sw)
    stest.expect_true(node_in_children(sw, userspace_node,
                                       {"pid": 1, "name": "init"}))

def expect_swapper_kernel_process(sw):
    threads_node = common.threads_node(sw)
    stest.expect_true(node_in_children(sw, threads_node,
                                       {"tid": 0, "name": "swapper/0"}))

def expect_root_node_name(sw, name):
    nt_query = sw.iface.osa_node_tree_query
    stest.expect_equal(nt_query.get_node(common.root_node(sw)).get("name"),
                       name)

stest.expect_false(tracker.enabled)
stest.expect_false(mapper.enabled)

(ok, rid_or_msg) = sw.iface.osa_control_v2.request("test")
stest.expect_true(ok, rid_or_msg)

stest.expect_true(tracker.enabled)
stest.expect_true(mapper.enabled)

stest.expect_false(tracker.booted)
expect_root_node_name(sw, "Linux")

common.boot_system(console)

stest.expect_true(tracker.booted)
expect_init_user_process(sw)
expect_swapper_kernel_process(sw)
