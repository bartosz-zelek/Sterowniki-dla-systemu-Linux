# © 2015 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


import stest
import cli
import dev_util as du

from config import create_pci_device, create_pci_bus, pci_bus

pci_device_1, pci_mem1, _, pci_bus1 = create_pci_device('pci1')
pci_device_2, _, regs2, _ = create_pci_device('pci2')

pci_bus2 = create_pci_bus('pci_2')
pci_mem2 = pci_bus2.mem
pci_device_2.pci_bus = pci_bus2.bus

# Map bank at 0x300
addr = 0x300
regs2.bar5.write(addr)

# Configure 64-bit BAR, or else it will overlap with 32-bit BAR
regs2.bar2.write(addr << 32)

# Test to map pci_device_2 into pci_mem2
pci_device_2.version = 0x3458
regs2.cmd.write(2)     # Enable memory access
stest.expect_equal(pci_mem2.map[0][1], pci_device_2,
                   "PCI device should have been mapped")
mem_read = pci_mem2.iface.memory_space.read
stest.expect_equal(du.tuple_to_value_le(mem_read(None, addr, 4, 0)),
                   0x3458, "Version should be read")
regs2.cmd.write(0)     # Disable memory access

# Test to map pci_device_2 into pci_mem1
pci_device_2.pci_bus = pci_device_1.pci_bus
pci_device_2.version = 0x3459
regs2.cmd.write(2)     # Enable memory access
stest.expect_equal(pci_mem1.map[0][1], pci_device_2,
                   "PCI device should have been mapped")
mem_read = pci_mem1.iface.memory_space.read
stest.expect_equal(du.tuple_to_value_le(mem_read(None, addr, 4, 0)),
                   0x3459, "Version should be read")
regs2.cmd.write(0)     # Disable memory access

# Test error when assigned object does not implement pci bus interface.
pci_device_2.pci_bus = pci_device_1
pci_device_2.version = 0x345a
stest.expect_log(regs2.cmd.write, [2], msg = 'TLM b_transport abort.'
                 'The object pci1 does not implement the pci_bus interface.')

print("All tests passed.")
