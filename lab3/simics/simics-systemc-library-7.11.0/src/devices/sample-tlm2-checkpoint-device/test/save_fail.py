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


import stest
import simics

import common

device = common.create_device('sample_tlm2_checkpoint_device', 'checkpoint_device')

a = stest.scratch_file('sample-tlm2-checkpoint-device-a')
simics.SIM_write_configuration_to_file(a, 0)
stest.expect_equal(len(device.sc_state), 1)

simics.SIM_write_configuration_to_file(stest.scratch_file('sample-tlm2-checkpoint-device-b'), 0)
stest.expect_equal(len(device.sc_state), 2)

try:
    simics.SIM_write_configuration_to_file(a, 0)
except SimExc_General:
    pass

stest.expect_equal(len(device.sc_state), 2)
