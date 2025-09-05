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


import common
import simics
import stest

device = common.create_device('sample_tlm2_checkpoint_device', 'checkpoint_device')
common.enable_trace('checkpoint_device')
log = common.add_logger()

# Test checkpoint from init state @ time zero
stest.expect_equal(device.integer, 0)
simics.SIM_write_configuration_to_file(stest.scratch_file('sample-tlm2-checkpoint-device-state0'), 0)

# Output from init state -> SIM_continue(1): match_log_empty, delta_count(1)
simics.SIM_continue(10**6)
common.match_log_empty(log)

device.integer = 1
simics.SIM_write_configuration_to_file(stest.scratch_file('sample-tlm2-checkpoint-device-state1'), 0)

# Output from state1 -> SIM_continue(1): match_log_initiator_to_target, delta_count(2)
simics.SIM_continue(10**6)
common.match_log("[checkpoint_device info] Entering thread @ 2", log)
common.match_log_initiator_to_target('checkpoint_device', log)

device.integer = 2
simics.SIM_write_configuration_to_file(stest.scratch_file('sample-tlm2-checkpoint-device-state2'), 0)

# Output from state2 -> SIM_continue(1): match_log_target_to_initiator, delta_count(3)
simics.SIM_continue(10**6)
common.match_log_target_to_initiator('checkpoint_device', log)

device.integer = 3
simics.SIM_write_configuration_to_file(stest.scratch_file('sample-tlm2-checkpoint-device-state3'), 0)

# Output from state3 -> SIM_continue(1): match_log_empty, delta_count(4)
simics.SIM_continue(10**6)
common.match_log_empty(log)
