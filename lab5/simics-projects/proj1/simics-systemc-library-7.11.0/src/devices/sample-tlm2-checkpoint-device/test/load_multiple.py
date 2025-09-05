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


import os
import stest
import simics

import common

simics.SIM_read_configuration(stest.scratch_file('multiple3'))
device = SIM_get_object('dut')

stest.expect_equal(3, len(device.sc_state))
(chkpt1, chkpt2, chkpt3) = device.sc_state
stest.expect_true(chkpt1.endswith(os.path.join('multiple1', 'systemc', 'dut')))
stest.expect_true(chkpt2.endswith(os.path.join('multiple2', 'systemc', 'dut')))
stest.expect_true(chkpt3.endswith(os.path.join('multiple3', 'systemc', 'dut')))

SIM_write_configuration_to_file(stest.scratch_file('multiple4'), 0)

stest.expect_equal(4, len(device.sc_state))
(_, _, _, chkpt4) = device.sc_state
stest.expect_true(chkpt4.endswith(os.path.join('multiple4', 'systemc', 'dut')))
