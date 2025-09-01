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
import common
import stest
import simics

device = common.create_device('sample_tlm2_checkpoint_device',
                              'checkpoint_device')
common.enable_trace('checkpoint_device')
log = common.add_logger()

simics.SIM_continue(10**6)
device.integer = 1
cli.run_command('take-snapshot a')
log[:] = []

simics.SIM_continue(10**6)
common.match_log("[checkpoint_device info] Entering thread @ 2", log)
common.match_log_initiator_to_target('checkpoint_device', log)
device.integer = 2
cli.run_command('take-snapshot b')
log[:] = []

simics.SIM_continue(10**6)
common.match_log_target_to_initiator('checkpoint_device', log)
device.integer = 3
cli.run_command('take-snapshot c')
log[:] = []

simics.SIM_continue(10**6)
stest.expect_false(log)
device.integer = 4

cli.run_command('restore-snapshot c')
stest.expect_equal(device.integer, 3)

cli.run_command('restore-snapshot b')
stest.expect_equal(device.integer, 2)

cli.run_command('restore-snapshot a')
stest.expect_equal(device.integer, 1)
