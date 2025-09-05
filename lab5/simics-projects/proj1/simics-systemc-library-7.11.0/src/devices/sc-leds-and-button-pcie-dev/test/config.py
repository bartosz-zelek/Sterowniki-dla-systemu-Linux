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


from simics import SIM_create_object, SIM_run_command, SIM_add_output_handler
import dev_util as du
import stest
import conf

# SIMICS-21543
conf.sim.deprecation_level = 0

# Common configurations
def create_pci_bus(name = 'pci'):
    class pci:
        pass

    pci.conf = SIM_create_object('memory-space', name + '_conf')
    pci.io = SIM_create_object('memory-space', name + '_io')
    pci.mem = SIM_create_object('memory-space', name + '_mem')

    pci.bus = SIM_create_object('pci-bus', name + '_bus',
                                [['conf_space', pci.conf],
                                 ['io_space', pci.io],
                                 ['memory_space', pci.mem]])
    return pci

def create_pci_device(name, pci_bus = None):
    class regs:
        pass

    if not pci_bus:
        pci_bus = create_pci_bus().bus
    system_onoff_led = du.Dev([du.Signal], name = "system_onoff_led")

    device = SIM_create_object('sc_leds_and_button_pcie_device', name,
                               [['pci_bus', pci_bus],
                                ['system_onoff_led', system_onoff_led.obj]])

    regs.bar0 = du.Register_LE((device, 255, 0x10), size = 4)
    regs.bar1 = du.Register_LE((device, 255, 0x14), size = 4)
    regs.bar2 = du.Register_LE((device, 255, 0x18), size = 4)
    regs.bar3 = du.Register_LE((device, 255, 0x1c), size = 4)
    regs.bar4 = du.Register_LE((device, 255, 0x20), size = 4)
    regs.bar5 = du.Register_LE((device, 255, 0x24), size = 4)
    # mmio mappings use a different mapping ID and can re-use same address
    id_offset = 0x10100
    regs.version = du.Register_LE((device, id_offset + 2, 0x0), size = 4)

    return device, pci_bus, regs, system_onoff_led

class LogHandler:
    _logs = []
    def __init__(self):
        SIM_add_output_handler(self.log, None)

    def log(self, data, src, _):
        for line in src.splitlines():
            self._logs.append(line)

    def match(self, line):
        stest.expect_equal(self._logs.pop(0), line)

    def match_partial(self, line):
        stest.expect_true(line in self._logs.pop(0))

    def contains(self, s):
        stest.expect_true(s, str(self._logs))

    def is_empty(self):
        stest.expect_false(self._logs)

    def reset(self):
        self._logs = []
