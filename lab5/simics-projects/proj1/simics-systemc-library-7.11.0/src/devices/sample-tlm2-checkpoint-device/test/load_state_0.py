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

from simics import SIM_continue, SIM_get_object, SIM_read_configuration
from stest import expect_equal, scratch_file

SIM_read_configuration(scratch_file('sample-tlm2-checkpoint-device-state0'))
common.enable_trace('checkpoint_device')
log = common.add_logger()

device = SIM_get_object("checkpoint_device")
expect_equal(device.integer, 0)

# Expected output from init state -> SIM_continue(1): match_log_empty, delta_count(1)
SIM_continue(1)
common.match_log_empty(log)
common.match_delta_count(device, 1)
