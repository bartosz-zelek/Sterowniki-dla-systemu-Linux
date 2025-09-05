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
from os.path import isfile

def test(_class):
    (dev0_name, banks, dev0) = common.create_test_device(_class)
    tool_name = common.create_tool(banks['bank0'])

    filename = stest.scratch_file('coverage')
    run_command('%s.coverage to-file=%s' % (tool_name, filename))

    stest.expect_true(isfile(filename))
