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
import checkpointing_common as ckpt

checkpoint_name = ckpt.get_mapper_checkpoint_file()
stest.expect_true(os.path.exists(checkpoint_name))
simics.SIM_read_configuration(checkpoint_name)

sw = common.get_sw()

stest.expect_different(sw.tracker.mapper_obj.base_nodes, None)
(ok, rid_or_msg) = sw.iface.osa_control_v2.request("Test")
stest.expect_true(ok, rid_or_msg)

ckpt.expect_node_tree(sw)
ckpt.test_adding_tasks(sw)

cpu = sw.iface.osa_node_tree_query.get_all_processors()[0]
ckpt.test_activating_tasks(sw, cpu)
ckpt.test_removing_tasks(sw)
