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
from pickle import load

def test(_class):
    (device_name, banks, device) = common.create_test_device(_class)
    tool_name = common.create_tool(banks['bank0'])

    Register_LE(banks['bank0'], 0, 4).write(0)

    filename = join(stest.sandbox, 'save')
    run_command('%s.save filename=%s' % (tool_name, filename))

    with open(filename, 'rb') as f:
        d = load(f)  # nosec
        stest.expect_true(banks['bank0'].name in d)
        stest.expect_equal(d[banks['bank0'].name], ([], [(0, 4, 1)]))
