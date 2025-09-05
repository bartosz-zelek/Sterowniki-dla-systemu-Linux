# © 2017 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


# It shows how to wrap the sample-tlm2-i2c-slave/master to a component,
# and uses the I2C link component to communicate with each other.

from stest import expect_equal
from config import I2C
from comp import StandardConnector, StandardConnectorComponent
from link_components import create_simple

class I2cLinkUpConnector(StandardConnector):
    """The I2cLinkUpConnector class handles i2c-link up connections. The
    first argument to the init method is the I2C device object."""

    def __init__(self, device, port):
        self.device = device
        self.port = port
        self.type = 'i2c-link'
        self.hotpluggable = False
        self.required = False
        self.multi = False
        self.direction = simics.Sim_Connector_Direction_Up

    def get_connect_data(self, cmp, cnt):
        if self.port:
            return [cmp.get_slot(self.device + '.port.' + self.port)]
        else:
            return [cmp.get_slot(self.device)]

    def connect(self, cmp, cnt, attr):
        if self.port:
            link_attr = "I2C0" if '[0]' in self.port else "I2C1"
            setattr(cmp.get_slot(self.device), f"{link_attr}_i2c_link_v2",
                    attr[0])
        else:
            cmp.get_slot(self.device).i2c_link_v2 = attr[0]

    def disconnect(self, cmp, cnt):
        if self.port:
            link_attr = "I2C0" if '[0]' in self.port else "I2C1"
            setattr(cmp.get_slot(self.device), f"{link_attr}_i2c_link_v2",
                    None)
        else:
            cmp.get_slot(self.device).i2c_link_v2 = None

class test_i2c_link_comp(
    create_simple(link_class = 'i2c-link-impl',
                  endpoint_class = 'i2c-link-endpoint',
                  connector_type = 'i2c-link',
                  class_desc = 'model of I2C link',
                  basename = 'test_i2c_link_comp')):
    '''The I2C link component'''

class sample_tlm2_i2c_slave_comp(StandardConnectorComponent):
    """Component wrapping a sample_tlm2_i2c_slave inside."""
    _class_desc = "sample TLM2 I2C slave component"
    _help_categories = ()

    def setup(self):
        super().setup()
        if not self.instantiated.val:
            self.add_objects()
        self.add_connectors()

    def add_objects(self):
        self.add_pre_obj('device', 'sample_tlm2_i2c_slave')

    def add_connectors(self):
        self.add_connector('i2c0', I2cLinkUpConnector('device', 'I2C[0]'))
        self.add_connector('i2c1', I2cLinkUpConnector('device', 'I2C[1]'))

class sample_tlm2_i2c_master_comp(StandardConnectorComponent):
    """Component wrapping a sample_tlm2_i2c_master inside."""
    _class_desc = "sample TLM2 I2C master component"
    _help_categories = ()

    def setup(self):
        super().setup()
        if not self.instantiated.val:
            self.add_objects()
        self.add_connectors()

    def add_objects(self):
        self.add_pre_obj('device', 'sample_tlm2_i2c_master')

    def add_connectors(self):
        self.add_connector('i2c', I2cLinkUpConnector('device', None))

def create_i2c_comps():
    run_command('load-module sample-tlm2-i2c-devices')
    run_command('create-sample-tlm2-i2c-slave-comp i2c_slave_comp')
    run_command('create-sample-tlm2-i2c-master-comp i2c_master_comp')
    SIM_create_object('test_i2c_link_comp', 'i2c_link')

    run_command('connect cnt0 = i2c_link.device0 cnt1 = i2c_slave_comp.i2c0')
    run_command('connect cnt0 = i2c_link.device1 cnt1 = i2c_slave_comp.i2c1')
    run_command('connect cnt0 = i2c_link.device2 cnt1 = i2c_master_comp.i2c')
    run_command('instantiate-components')

def init_i2c_operations(address):
    conf.i2c_master_comp.device.ack = I2C.noack
    conf.i2c_master_comp.device.address = address
    if address % 2 == 0:
        conf.i2c_master_comp.device.data_wr = 0xd1
        conf.i2c_slave_comp.device.I2C0_register = 0
    else:
        conf.i2c_master_comp.device.data_wr = 0
        conf.i2c_slave_comp.device.I2C0_register = 0xd0

def test_i2c_read_operation():
    init_i2c_operations(0x25)
    expect_equal(conf.i2c_master_comp.device.ack, I2C.noack)
    expect_equal(conf.i2c_master_comp.device.data_rd, 0)

    SIM_continue(22 * 10**6)
    # Master received the start acknowledge from both slaves
    expect_equal(conf.i2c_master_comp.device.ack, I2C.ack)

    SIM_continue(22 * 10**6)
    # Master received the read response from slave 0
    expect_equal(conf.i2c_master_comp.device.data_rd, 0xd0)

    # Master sends stop
    SIM_continue(22 * 10**6)

def test_i2c_write_operation():
    init_i2c_operations(0x24)
    expect_equal(conf.i2c_master_comp.device.ack, I2C.noack)
    expect_equal(conf.i2c_slave_comp.device.I2C0_register, 0)

    SIM_continue(22 * 10**6)
    # Master received the start acknowledge from both slaves
    expect_equal(conf.i2c_master_comp.device.ack, I2C.ack)

    SIM_continue(22 * 10**6)
    # Slave received the write date from master
    expect_equal(conf.i2c_slave_comp.device.I2C0_register, 0xd1)

    # Master sends stop
    SIM_continue(22 * 10**6)

create_i2c_comps()
# The default I2C address for sample-tlm2-i2c-slave is both 0x12(18)
# Change the address of I2C1 to 0x14(20)
conf.i2c_slave_comp.device.I2C1_i2c_address = 20

test_i2c_read_operation()
test_i2c_write_operation()
