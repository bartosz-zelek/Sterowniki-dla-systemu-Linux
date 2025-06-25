# © 2018 Intel Corporation
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

from dev_util import Register_LE
from simics import SIM_get_object

def test(_class):
    (device_name, banks, device) = common.create_test_device(_class)
    tool_name = common.create_tool(banks['bank0'])

    r1 = Register_LE(banks['bank0'], 0, 4)
    r2 = Register_LE(banks['bank0'], 4, 4)

    r1.write(0)
    r2.write(0)

    connection = SIM_get_object('%s_0' % tool_name)

    stest.expect_equal(len(connection.write_accesses), 2)
    stest.expect_true([0, 4, 1] in connection.write_accesses)
    stest.expect_true([4, 4, 1] in connection.write_accesses)
