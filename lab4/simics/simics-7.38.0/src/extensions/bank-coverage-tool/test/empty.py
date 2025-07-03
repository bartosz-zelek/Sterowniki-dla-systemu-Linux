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

from cli import run_command
from dev_util import Register_LE
from stest import expect_equal

def test(_class):
    (dev0_name, banks, dev0) = common.create_test_device(_class)
    tool_name = common.create_tool(banks['empty'])

    device_coverage = { device : coverage for (device, _, _, coverage)
                        in run_command('%s.coverage' % tool_name) }
    expect_equal(device_coverage[dev0_name], 1.0)
