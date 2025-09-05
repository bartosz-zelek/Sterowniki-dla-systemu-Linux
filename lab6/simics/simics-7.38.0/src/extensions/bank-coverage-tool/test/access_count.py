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
    (_, banks0, dev0) = common.create_test_device(_class)
    (_, banks1, dev1) = common.create_test_device(_class)

    tool_name = common.create_tool(banks0['bank0'])
    common.add_instrumentation(tool_name, banks1['bank0'])

    dev0_r1 = Register_LE(banks0['bank0'], 0, 4)
    dev1_r1 = Register_LE(banks1['bank0'], 0, 4)

    dev0_r1.write(0)
    dev0_r1.write(0)

    dev1_r1.read()

    def access_count(bank, read = True, write = True):
        return run_command('%s.access-count bank=%s %s %s' % (
            tool_name, bank.name,
            '-read' if read else '',
            '-write' if write else ''))

    expect_equal(access_count(banks0['bank0']), [['bank0_reg0', 0, 4, 2]])
    expect_equal(access_count(banks1['bank0']), [['bank0_reg0', 0, 4, 1]])
    expect_equal(access_count(banks1['bank0'], read = False), [])
