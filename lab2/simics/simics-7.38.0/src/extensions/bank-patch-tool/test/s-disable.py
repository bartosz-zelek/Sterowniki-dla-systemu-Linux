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

(device_name, device) = common.create_sample_device()
(_, miss) = common.sample_registers(device)

tool_name = common.create_patch_tool(common.bank_name(device_name))
miss.read()

run_command('%s.disable-instrumentation bank = %s' % (
    tool_name, common.bank_name(device_name)))
common.expect_miss(device.bank.regs, miss.read)
