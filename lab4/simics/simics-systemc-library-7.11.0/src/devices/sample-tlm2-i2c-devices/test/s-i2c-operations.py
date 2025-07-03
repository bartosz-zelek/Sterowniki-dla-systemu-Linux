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


# A fake I2C link is used to test I2C operations. It does not route
# any traffic but only log the received transactions for test purpose

from config import create_config, I2C
from stest import expect_equal
import dev_util as du

# Test I2C addresses call
def test_slave_addresses():
    expect_equal(i2c_slave_if.addresses(),
                 [i2c_address * 2, i2c_address * 2 + 1])

# Test I2C start request
def test_slave_start():
    i2c_slave_if.start(i2c_address * 2)
    expect_equal(fake_i2c_master_if.reqs.pop(), ('a', 0))
    i2c_slave_if.start(i2c_address * 2 + 1)
    expect_equal(fake_i2c_master_if.reqs.pop(), ('a', 0))
    i2c_slave_if.start(i2c_address * 2 + 2)
    expect_equal(fake_i2c_master_if.reqs.pop(), ('a', 1))

# Test I2C read request
def test_slave_read():
    i2c_slave_if.read()
    expect_equal(fake_i2c_master_if.reqs.pop(), ('r', reg1.read() & 0xff))

# Test I2C write request
def test_slave_write():
    val = (reg1.read() & 0xff) + 1
    i2c_slave_if.write(val)
    expect_equal(reg1.read() & 0xff, val)

# Test I2C stop request
def test_slave_stop():
    # Nothing to expect simple call it
    i2c_slave_if.stop()

# Test I2C acknowledge response
def test_master_acknowledge():
    i2c_master_if.acknowledge(I2C.noack)
    # A NACK will trigger the sample master to send a stop request
    expect_equal(fake_i2c_slave_if.reqs.pop(), ('stop',))
    i2c_master_if.acknowledge(I2C.ack)
    # An ACK with data_wr = 0 will trigger the sample master to
    # send a read request
    expect_equal(fake_i2c_slave_if.reqs.pop(), ('read',))
    i2c_master.data_wr = 0xd1
    i2c_master_if.acknowledge(I2C.ack)
    # An ACK with data_wr = 1 will trigger the sample master to
    # send a write request
    expect_equal(fake_i2c_slave_if.reqs.pop(), ('write', 0xd1))

# Test I2C read response
def test_master_read_response():
    i2c_master_if.read_response(0xd0)
    expect_equal(i2c_master.data_rd, 0xd0)

# Setup configuration
fake_link, i2c_slave, i2c_master = create_config()
reg1 = du.Register_LE(i2c_slave.port.I2C[0], 0, size = 4)
i2c_address = i2c_slave.I2C0_i2c_address
i2c_slave_if = i2c_slave.port.I2C[0].iface.i2c_slave_v2
i2c_master_if = i2c_master.iface.i2c_master_v2
fake_i2c_slave_if = fake_link.I2C2.i2c_slave_v2
fake_i2c_master_if = fake_link.I2C0.i2c_master_v2

test_slave_addresses()
test_slave_start()
test_slave_read()
test_slave_write()
test_slave_stop()
test_master_acknowledge()
test_master_read_response()
