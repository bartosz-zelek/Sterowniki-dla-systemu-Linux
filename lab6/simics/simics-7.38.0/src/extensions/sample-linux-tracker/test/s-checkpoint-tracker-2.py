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

stest.expect_true(tracker.root_entity_added)
stest.expect_true(tracker.booted)
stest.expect_different(tracker.cpu, None)
tasks_before_clear = tracker.tasks

# Test clearing state
simics.SIM_continue(1)
stest.expect_false(tracker.root_entity_added)
stest.expect_false(tracker.booted)
stest.expect_equal(tracker.tasks, [])
stest.expect_equal(tracker.active_task, None)
stest.expect_equal(tracker.cpu, None)

# Enable tracker
(rc, rid_or_msg) = sw.iface.osa_control_v2.request("Test")
stest.expect_true(rc, rid_or_msg)

# Test enabling again and see that tracker state is found
stest.expect_true(tracker.root_entity_added)
stest.expect_true(tracker.booted)
tasks = tracker.tasks
# Assume no task changes occurred while running one cycle forward
stest.expect_equal(tasks, tasks_before_clear)
stest.expect_different(tracker.cpu, None)
