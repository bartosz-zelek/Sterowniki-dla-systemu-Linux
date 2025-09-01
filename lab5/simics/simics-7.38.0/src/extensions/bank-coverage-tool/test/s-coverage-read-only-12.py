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

from cli import run_command
from dev_util import Register_LE
from stest import expect_equal

(dev_name, banks, dev) = common.create_test_device('test_bank_coverage_12')
tool_name = common.create_tool(banks['bank2'])

read_write = Register_LE(banks['bank2'], 0, 4)
read_only = Register_LE(banks['bank2'], 4, 4)

read_write.read()
read_only.read()

def coverage(read = False, write = False):
    modifiers = '%s %s' % ('-read' if read else '', '-write' if write else '')
    return {
        device : coverage for (device, _, _, coverage) in run_command(
            '%s.coverage %s' % (tool_name, modifiers)) }[dev_name]

expect_equal(coverage(read = True, write = True), 1)
expect_equal(coverage(read = True), 1)

read_write.write(0)

expect_equal(coverage(write = True), 0.5)
