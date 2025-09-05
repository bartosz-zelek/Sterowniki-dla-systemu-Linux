# © 2019 Intel Corporation
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
import stest

from cli import quiet_run_command
from simics import SIM_get_object

def has_connection(name):
    try:
        SIM_get_object(name)
        return True
    except SimExc_General:
        return False

(_, banks0, _) = common.create_test_device('test_bank_coverage_12', 'dev0')
(_, banks1, _) = common.create_test_device('test_bank_coverage_12', 'dev1')
tool_name = common.create_tool(banks0['bank0'])
quiet_run_command('%s.add-instrumentation "%s"' % (
    tool_name, banks1['bank1[0]'].name))

stest.expect_true(has_connection('%s_0' % tool_name))
stest.expect_true(has_connection('%s_1' % tool_name))

quiet_run_command('%s.remove-instrumentation %s' % (
    tool_name, banks0['bank0'].name))

stest.expect_false(has_connection('%s_0' % tool_name))
stest.expect_true(has_connection('%s_1' % tool_name))

quiet_run_command('%s.add-instrumentation %s' % (
    tool_name, banks1['bank0'].name))

stest.expect_true(has_connection('%s_2' % tool_name))
stest.expect_true(has_connection('%s_1' % tool_name))
