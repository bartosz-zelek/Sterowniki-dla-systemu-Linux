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


# Test the LED can be controlled by writing pin control registers in PCI MEM
from config import create_pci_device, du
from stest import expect_equal

(device, pci_bus, regs, system_onoff_led) = create_pci_device("pci_dev")
pci_bus.bus_number = 32
pci_bus.pci_devices.append([0, 0, device])

def read_memory(space, addr, size):
    conf_read = space.iface.memory_space.read
    return du.tuple_to_value_le(conf_read(None, addr, size, 0))

def write_memory(space, addr, size, value):
    conf_write = space.iface.memory_space.write
    return conf_write(None, addr, du.value_to_tuple_le(value, size), 0)

# Map MEM space
regs.bar0.write(0x0)
regs.bar1.write(0x1000)
regs.bar2.write(0x1100)
expect_equal(read_memory(pci_bus.conf_space, 0x200004, 2), 0)
write_memory(pci_bus.conf_space, 0x200004, 2, 0x103)
SIM_continue(1)

# Write LED control registers
expect_equal(system_onoff_led.signal.level, 1)
expect_equal(read_memory(pci_bus.memory_space, 0x20, 4), 1)
write_memory(pci_bus.memory_space, 0x20, 4, 0x0)
SIM_continue(1)
expect_equal(system_onoff_led.signal.level, 0)

# Send PCIe messages to blink the onoff_led
device.iface.pci_express.send_message(device, 0x47, [])
SIM_continue(1)
expect_equal(system_onoff_led.signal.level, 1)
SIM_continue(5000000)
expect_equal(system_onoff_led.signal.level, 0)

# Send PCIe messages to turn on the onoff_led
device.iface.pci_express.send_message(device, 0x45, [])
SIM_continue(1)
expect_equal(system_onoff_led.signal.level, 1)

# Send PCIe messages to turn off the onoff_led
device.iface.pci_express.send_message(device, 0x44, [])
SIM_continue(1)
expect_equal(system_onoff_led.signal.level, 0)
