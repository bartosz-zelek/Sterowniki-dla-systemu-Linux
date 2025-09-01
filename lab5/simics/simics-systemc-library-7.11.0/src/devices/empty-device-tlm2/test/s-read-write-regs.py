# © 2010 Intel Corporation

import stest
import dev_util
import config

device = config.create_device()

reg1 = dev_util.Register_BE(device, 0, size = 4)
stest.expect_true(reg1.read() != 0x00c0ffee)
reg1.write(0x00c0ffee)
stest.expect_equal(reg1.read(), 0x00c0ffee)
