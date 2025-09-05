# © 2011 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


import pyobj, stest

class dummy_dev(pyobj.ConfObject):
    class write(pyobj.Attribute):
        attrattr = simics.Sim_Attr_Optional
        attrtype = '[id]'
        def _initialize(self):
            self.val = []
        def getter(self):
            return self.val
        def setter(self, val):
            (addr, data) = val
            self._up.memspace.val.iface.memory_space.write(
                self._up.obj, addr, data, 0)
    class read(pyobj.Attribute):
        attrattr = simics.Sim_Attr_Optional
        attrtype = '[ii]'
        def _initialize(self):
            self.val = []
        def getter(self):
            return self.val
        def setter(self, val):
            (addr, size) = val
            self.val = self._up.memspace.val.iface.memory_space.read(
                self._up.obj, addr, size, 0)
    class memspace(pyobj.SimpleAttribute(None, 'o')):
        pass

clk = pre_conf_object('clk', 'clock')
dev = pre_conf_object('dev', 'dummy_dev')
cache_dev = pre_conf_object('cache_dev', 'dummy_dev')
mem = pre_conf_object('mem', 'memory-space')
l2c = pre_conf_object('l2c', 'g-cache')
ram = pre_conf_object('ram', 'ram')
img = pre_conf_object('img', 'image')

l2c.cpus = None
l2c.config_line_number = (512*1024)//64
l2c.config_line_size = 64
l2c.config_assoc = 8
l2c.config_virtual_index = 0
l2c.config_virtual_tag = 0
l2c.config_replacement_policy = 'lru'
l2c.config_write_back = 1
l2c.config_write_allocate = 1
l2c.penalty_read = 1
l2c.penalty_read_next = 1
l2c.penalty_write = 1
l2c.penalty_write_next = 1
l2c.higher_level_caches = None
l2c.cacheable_devices = [cache_dev]

img.size = 0x10000000
ram.image = img
mem.timing_model = l2c
mem.map = [[0, ram, 0, 0, 0x10000000]]
dev.memspace = mem
cache_dev.memspace = mem
clk.freq_mhz = 1

devs = [dev, cache_dev, mem, l2c, ram, img, clk]
for d in devs:
    d.queue = clk
SIM_add_configuration(devs, None)

# write from a uncacheable device
conf.dev.write = [0x0, (1,2,3,4)]
stest.expect_equal(conf.l2c.stat_c_dev_data_write, 0)

# read from a uncacheable device
conf.dev.read = [0x0, 4]
stest.expect_equal(conf.l2c.stat_c_dev_data_read, 0)

# write from a cacheable device
conf.cache_dev.write = [0x4, (5,6,7,8)]
stest.expect_equal(conf.l2c.stat_c_dev_data_write, 1)

# read from a cacheable device
conf.cache_dev.read = [0x4, 4]
stest.expect_equal(conf.l2c.stat_c_dev_data_read, 1)
