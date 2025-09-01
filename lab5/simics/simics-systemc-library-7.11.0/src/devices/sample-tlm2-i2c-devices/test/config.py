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


from simics import SIM_create_object
from fake_i2c_devs import create_link

class I2C:
    ack = 0
    noack = 1

def create_config():
    fake_link = create_link()
    i2c_slave = SIM_create_object('sample_tlm2_i2c_slave', 'i2c_slave',
                                  [['I2C0_i2c_link_v2', [fake_link.obj, 'I2C0']],
                                   ['I2C1_i2c_link_v2', [fake_link.obj, 'I2C1']]
                                  ])
    i2c_master = SIM_create_object('sample_tlm2_i2c_master', 'i2c_master',
                                  [['i2c_link_v2', [fake_link.obj, 'I2C2']]
                                  ])
    return fake_link, i2c_slave, i2c_master
