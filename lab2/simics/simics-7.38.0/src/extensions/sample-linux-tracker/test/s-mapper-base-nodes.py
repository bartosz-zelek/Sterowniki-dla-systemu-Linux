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


# Test that the mapper provides the correct base nodes

import cli
import stest
import sample_linux_tracker_common as common

(sw, _) = common.setup_test_system()

(ok, rid_or_msg) = sw.iface.osa_control_v2.request("test")
stest.expect_true(ok, rid_or_msg)

root = sw.iface.osa_component.get_root_node()
stest.expect_true(root.valid)

def nt_query(sw):
    return sw.iface.osa_node_tree_query

def multiprocessor_for_base_node(node_name):
    if node_name == "Other":
        return False
    return True

def verify_kernel_sub_nodes(sw, kernel_id):
    sub_names = set(["Other", "Threads"])
    children = nt_query(sw).get_children(kernel_id)
    stest.expect_equal(len(children), len(sub_names), children)
    for child_id in children:
        child = nt_query(sw).get_node(child_id)
        stest.expect_true(child['name'] in sub_names)
        sub_names.remove(child['name'])
        stest.expect_equal(child['multiprocessor'],
                           multiprocessor_for_base_node(child['name']))
        stest.expect_equal(child['memory_space'], kernel_id)

def verify_kernel_node(sw):
    node_id = common.kernel(sw)
    stest.expect_different(node_id, None)
    node = nt_query(sw).get_node(node_id)
    stest.expect_equal(node['multiprocessor'], True)
    stest.expect_equal(node['memory_space'], node_id)
    stest.expect_equal(node['name'], "Kernel")
    verify_kernel_sub_nodes(sw, node_id)

def verify_userspace_node(sw):
    node_id = common.userspace(sw)
    stest.expect_different(node_id, None)
    node = nt_query(sw).get_node(node_id)
    stest.expect_equal(node['multiprocessor'], True)
    stest.expect_false('memory_space' in node)
    stest.expect_equal(node['name'], "Userspace")
    stest.expect_equal(len(nt_query(sw).get_children(node_id)), 0)

verify_kernel_node(sw)
verify_userspace_node(sw)
