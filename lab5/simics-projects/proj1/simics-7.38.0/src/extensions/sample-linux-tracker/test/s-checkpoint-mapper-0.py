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
import simics
import stest
import sample_linux_tracker_common as common
import checkpointing_common as ckpt

(sw, _) = common.setup_test_system()

(rc, rid_or_msg) = sw.iface.osa_control_v2.request("Test")
stest.expect_true(rc, rid_or_msg)

ckpt.build_node_tree(sw)
ckpt.expect_node_tree(sw)


checkpoint_name = ckpt.get_mapper_checkpoint_file()
simics.SIM_write_configuration_to_file(checkpoint_name, 0)

# Add, modify and remove nodes that are not checkpointed but test so
# that this works the same way when loaded from a checkpoint as when
# done without any checkpoint.
ckpt.test_adding_tasks(sw)
cpu = sw.iface.osa_node_tree_query.get_all_processors()[0]
ckpt.test_activating_tasks(sw, cpu)
ckpt.test_removing_tasks(sw)
