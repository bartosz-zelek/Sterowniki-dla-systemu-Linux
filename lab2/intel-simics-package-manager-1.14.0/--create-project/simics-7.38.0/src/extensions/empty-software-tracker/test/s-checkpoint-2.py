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

# Test that state is cleared when continuing the simulation, without
# enabling the tracker framework, after loading a checkpoint.

import os
import simics
import stest
import empty_software_tracker_common as common

checkpoint_name = common.get_checkpoint_file()
stest.expect_true(os.path.exists(checkpoint_name))
simics.SIM_read_configuration(checkpoint_name)

sw = common.get_sw()

stest.expect_different(sw.tracker.mapper_obj.root_node, None)

# Continue simulation without enabling tracker to enforce clearing of state.
simics.SIM_continue(1)
stest.expect_equal(sw.tracker.mapper_obj.root_node, None)

(ok, rid_or_msg) = sw.iface.osa_control_v2.request("Test")
stest.expect_true(ok, rid_or_msg)
stest.expect_different(sw.tracker.mapper_obj.root_node, None)
