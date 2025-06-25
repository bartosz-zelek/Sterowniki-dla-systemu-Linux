# © 2014 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


import stest
import dev_util as du
import config

simple_device = config.create_simple_device()

reg1 = du.Register_LE(simple_device, 0, size = 4)
reg2 = du.Register_LE(simple_device, 4, size = 4)

stest.expect_true(reg1.read() != 0x00c0ffee)
reg1.write(0x00c0ffee)
stest.expect_equal(reg1.read(), 0x00c0ffee)

stest.expect_true(reg2.read() != 0xdeadbeef)
reg2.write(0xdeadbeef)
stest.expect_equal(reg2.read(), 0xdeadbeef)
