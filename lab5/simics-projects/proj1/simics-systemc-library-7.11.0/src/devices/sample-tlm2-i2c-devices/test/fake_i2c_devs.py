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


import dev_util as du
import conf

# SIMICS-21543
conf.sim.deprecation_level = 0

class I2cMaster(du.Iface):
    iface = "i2c_master_v2"
    def __init__(self):
        self.reqs = []
    def acknowledge(self, dev, ack):
        self.reqs.append(('a', ack))
    def read_response(self, dev, data):
        self.reqs.append(('r', data))

class I2cSlave(du.Iface):
    iface = "i2c_slave_v2"
    def __init__(self):
        self.reqs = []
    def start(self, dev, address):
        self.reqs.append(('start', address))
    def read(self, dev):
        self.reqs.append(('read',))
    def write(self, dev, value):
        self.reqs.append(('write', value))
    def stop(self, dev):
        self.reqs.append(('stop',))
    def addresses(self, dev):
        self.reqs.append(('addresses',))

def create_link():
    return du.Dev([('I2C0', I2cMaster), ('I2C1', I2cMaster), ('I2C2', I2cSlave)],
                  name = "I2C_link")
