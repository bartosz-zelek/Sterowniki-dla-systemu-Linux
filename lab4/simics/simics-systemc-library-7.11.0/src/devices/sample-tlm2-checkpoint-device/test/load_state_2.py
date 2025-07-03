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

SIM_read_configuration(scratch_file('sample-tlm2-checkpoint-device-state2'))
common.enable_trace('checkpoint_device')
log = common.add_logger()

device = SIM_get_object("checkpoint_device")

# Expected output from state2 -> SIM_continue(1): match_log_target_to_initiator, delta_count(3)
SIM_continue(10**6)
# NOTE: "Entering thread" is _not_ expected here as the thread only runs every
#       other 2 us, i.e. on 2, 4, 6, ... but not on 3 where we are right now.
common.match_log_target_to_initiator('checkpoint_device', log)
common.match_delta_count(device, 3)

# Run 1 us more to see the requester thread
SIM_continue(10**6)
common.match_log("[checkpoint_device info] Entering thread @ 4", log)
