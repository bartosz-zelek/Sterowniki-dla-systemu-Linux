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
    (dev0_name, banks0, dev0) = common.create_test_device(_class)
    tool_name = common.create_tool(banks0['bank0'])

    (dev1_name, banks1, dev1) = common.create_test_device(_class)
    common.add_instrumentation(tool_name, banks1['bank1[0]'])
    common.add_instrumentation(tool_name, banks1['bank1[1]'])

    dev0_r1r2 = Register_LE(banks0['bank0'], 0, 8)

    dev1_b1_r1 = Register_LE(banks1['bank1[0]'], 0, 4)
    dev1_b1_r2 = Register_LE(banks1['bank1[0]'], 4, 4)
    dev1_b2_r1 = Register_LE(banks1['bank1[1]'], 0, 4)

    dev0_r1r2.write(0)

    dev1_b1_r1.read()
    dev1_b1_r2.write(0)
    dev1_b2_r1.read()

    device_coverage = { device : coverage for (device, _, _, coverage)
                        in run_command('%s.coverage' % tool_name) }
    expect_equal(device_coverage[dev0_name], 1.0)
    expect_equal(device_coverage[dev1_name], 0.5)

    run_command('%s.add-instrumentation %s' % (
        tool_name, banks1['bank1[2]'].name))

    device_coverage = { device : coverage for (device, _, _, coverage)
                        in run_command('%s.coverage' % tool_name) }
    expect_equal(device_coverage[dev0_name], 1.0)
    expect_equal(device_coverage[dev1_name], 1.0/3)
