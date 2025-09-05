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

from simics import SIM_get_object
from stest import expect_true

def test(_class):
    (_, banks, _) = common.create_test_device(_class)

    tool_name = common.create_tool(banks['bank0'])
    expect_true(SIM_get_object(tool_name) is not None)
