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

from cli import run_command
from dev_util import Register_LE
from os.path import join

def test(_class):
    def coverage(tool_name):
        return { device : coverage for (device, _, _, coverage)
                 in run_command('%s.coverage' % tool_name) }

    (device_name, banks, device) = common.create_test_device(_class)
    tool0 = common.create_tool(banks['bank0'])
    tool1 = common.create_tool(banks['bank0'])

    run_command('%s.load filename=%s' % (tool0, join(stest.sandbox, 'save')))
    Register_LE(banks['bank0'], 4, 4).write(0)

    stest.expect_equal(coverage(tool0)[device_name], 1.0)
    stest.expect_equal(coverage(tool1)[device_name], 0.5)
