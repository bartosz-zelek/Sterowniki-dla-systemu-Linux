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
import empty_software_tracker_common as common

sw = common.setup_test_system()

stest.expect_false(sw.tracker.tracker_obj.enabled)
stest.expect_false(sw.tracker.mapper_obj.enabled)

(ok, rid_or_msg) = sw.iface.osa_control_v2.request("test")
stest.expect_true(ok, rid_or_msg)

stest.expect_true(sw.tracker.tracker_obj.enabled)
stest.expect_true(sw.tracker.mapper_obj.enabled)

# TODO: Add your testing here, this is just an example

root = sw.iface.osa_component.get_root_node()
stest.expect_true(root.valid)
root_id = root.id

root_name = "Full"
sw.tracker.tracker_obj.root_name = root_name

nt_query = sw.iface.osa_node_tree_query
node = nt_query.get_node(root_id)
stest.expect_different(node, None)
stest.expect_equal(node['name'], root_name)

sw.iface.osa_control_v2.release(rid_or_msg)

stest.expect_false(sw.tracker.tracker_obj.enabled)
stest.expect_false(sw.tracker.mapper_obj.enabled)
