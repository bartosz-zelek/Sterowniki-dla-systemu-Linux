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
from stest import scratch_file

SIM_read_configuration(scratch_file('sample-tlm2-checkpoint-device-state1'))
common.enable_trace('checkpoint_device')
log = common.add_logger()

device = SIM_get_object("checkpoint_device")

# Expected output from state1 -> SIM_continue(1): match_log_initiator_to_target, delta_count(2)
SIM_continue(10**6)
common.match_log("[checkpoint_device info] Entering thread @ 2", log)
common.match_log_initiator_to_target('checkpoint_device', log)
