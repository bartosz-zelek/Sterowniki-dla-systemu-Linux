# © 2023 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


# A few sanity checks for telnet_frontend.

import stest
import info_status
import simics

# Verify that info/status commands have been registered for all
# classes in this module.
info_status.check_for_info_status(['telnet-frontend'])

cli.global_cmds.telnet_frontend()

# The telnet-frontend command provides no way to set telnet_frontend
# object name. We use SIM_object_iterator_for_class to find the created object.
tf_objs = list(simics.SIM_object_iterator_for_class('telnet_frontend'))

stest.expect_true(tf_objs, 'No telnet_frontend objects found')

test_obj = tf_objs[0]

# Run info and status on test_obj object. It is difficult to test whether
# the output is informative, so we just check that the commands complete nicely.
test_obj.cli_cmds.info()
test_obj.cli_cmds.status()
