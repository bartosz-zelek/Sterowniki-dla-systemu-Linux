# © 2021 Intel Corporation
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
import info_status
import m_control_common

# Verify that info/status commands have been registered for all
# classes in this module.
info_status.check_for_info_status(['m-control'])

# Create an instance of each class defined and test it
dev   = m_control_common.create_m_control(namespace="default")[0]
dev_s = m_control_common.create_m_control(namespace="stall",classname="m_control_stall")[0]

# Run info and status on each object. It is difficult to test whether
# the output is informative, so we just check that the commands
# complete nicely.
for obj in [dev,dev_s]:
    for cmd in ['info', 'status']:
        try:
            SIM_run_command(obj.name + '.' + cmd)
        except SimExc_General as e:
            stest.fail(cmd + ' command failed: ' + str(e))
