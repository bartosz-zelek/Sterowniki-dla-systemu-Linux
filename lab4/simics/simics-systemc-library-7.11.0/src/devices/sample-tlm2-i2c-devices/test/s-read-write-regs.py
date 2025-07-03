# © 2012 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


from config import create_config, I2C
import dev_util as du
from stest import expect_equal, expect_true

# Check I2C sample slave device
def check_i2c_slave_attr():
    expect_equal(i2c_slave.I2C0_i2c_address, 0x12)

    reg1 = du.Register_LE(i2c_slave.port.I2C[0], 0, size = 4)
    expect_true(reg1.read() != 0x00c0ffee)
    reg1.write(0x00c0ffee)
    expect_equal(reg1.read(), 0x00c0ffee)

# Check I2C sample master device
def check_i2c_master_attr():
    expect_equal(i2c_master.address, 0)
    expect_equal(i2c_master.data_rd, 0)
    expect_equal(i2c_master.data_wr, 0)
    expect_equal(i2c_master.ack, I2C.noack)

_, i2c_slave, i2c_master = create_config()
check_i2c_slave_attr()
check_i2c_master_attr()
