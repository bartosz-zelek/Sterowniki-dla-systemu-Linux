# © 2017 Intel Corporation
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
import simics

import common

device = common.create_device('sample_tlm2_checkpoint_device', 'checkpoint_device')

device.io = [3, 5, 7]
cli.run_command('take-snapshot pingpong')

simics.SIM_continue(1)
device.io = [0, 0, 0]

cli.run_command('restore-snapshot pingpong')
stest.expect_equal([3, 5, 7], device.io)
