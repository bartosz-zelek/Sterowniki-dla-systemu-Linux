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

checkpoint_file = ckpt.get_tracker_checkpoint_file()
stest.expect_true(os.path.exists(checkpoint_file))
simics.SIM_read_configuration(checkpoint_file)

sw = common.get_sw()
console = common.ConsoleWrapper(common.get_console(), common.root_prompt())
tracker = ckpt.get_tracker(sw)

stest.expect_true(tracker.booted)
tasks_before_enable = tracker.tasks
stest.expect_different(tracker.cpu, None)

(rc, rid_or_msg) = sw.iface.osa_control_v2.request("Test")
stest.expect_true(rc, rid_or_msg)

# Test that tasks and booted status does not change after enable
stest.expect_true(tracker.booted)
tasks = tracker.tasks
stest.expect_equal(tasks, tasks_before_enable)
stest.expect_different(tracker.cpu, None)

# Tests that tasks become active after a checkpoint
common.test_active(sw, console)
