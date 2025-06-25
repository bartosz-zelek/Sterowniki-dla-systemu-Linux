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

(device_name, device) = common.create_sample_device()
(hit, _) = common.sample_registers(device)

hit.read()
hit.write(0)
common.create_patch_tool(common.bank_name(device_name), inject = True)
common.expect_miss(device.bank.regs, hit.read)
common.expect_miss(device.bank.regs, lambda: hit.write(0))
