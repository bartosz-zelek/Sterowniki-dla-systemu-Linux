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


import cli
from . import utility

master_device_class_name = "sample_tlm2_i2c_master"
slave_device_class_name = "sample_tlm2_i2c_slave"
slave_port_class_name = slave_device_class_name + ".port"

#
# ------------------------ info -----------------------
#

def get_i2c_slave_info(obj):
    return [("I2C0",
             [("link", obj.I2C0_i2c_link_v2),
              ("7-bit address", "0x%x" % obj.I2C0_i2c_address)]),
            ("I2C1",
             [("link", obj.I2C1_i2c_link_v2),
              ("7-bit address", "0x%x" % obj.I2C1_i2c_address)])]

cli.new_info_command(slave_device_class_name, get_i2c_slave_info)
cli.new_info_command(slave_port_class_name, lambda x: [])

def get_i2c_master_info(obj):
    return []

cli.new_info_command(master_device_class_name, get_i2c_master_info)

#
# ------------------------ status -----------------------
#

def get_i2c_slave_status(obj):
    status = [("I2C0",
               [("Register", cli.number_str(obj.I2C0_register))]),
              ("I2C1",
               [("Register", cli.number_str(obj.I2C1_register))])]
    status += utility.get_status(obj)
    return status

cli.new_status_command(slave_device_class_name, get_i2c_slave_status)
cli.new_status_command(slave_port_class_name, lambda x: [])

def get_i2c_master_status(obj):
    return [(None,
             [("Address", obj.address),
              ("Data read", obj.data_rd),
              ("Data write", obj.data_wr),
              ("ACK", obj.ack)])]

cli.new_status_command(master_device_class_name, get_i2c_master_status)
