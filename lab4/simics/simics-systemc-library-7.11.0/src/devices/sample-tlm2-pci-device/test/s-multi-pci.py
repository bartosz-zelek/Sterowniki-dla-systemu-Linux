# © 2013 Intel Corporation
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

from config import *

pci1, pci_mem, regs1, _ = create_pci_device('pci1')
pci2, pci_mem2, regs2, _ = create_pci_device('pci2')

# Test the PCI vendor and device IDs
def test_id_regs():
    for (pci, regs) in [(pci1, regs1), (pci2, regs2)]:
        # Configuration Space
        stest.expect_equal(regs.vendor_id.read(), 0x104c)
        stest.expect_equal(regs.device_id.read(), (0xac10 << 16) | 0x104c)
        stest.expect_equal(pci.vendor_id, 0x104c)
        stest.expect_equal(pci.device_id, 0xac10)
        # Device registers
        stest.expect_equal(regs.version.read(), 0x4711)
        stest.expect_equal(pci.version, 0x4711)

# Test setting BAR to map the device in memory
def test_multi_mapping():
    addr = 0x100
    regs1.bar2.write(addr << 32)  # configure 64-bit bar or it will overlap
    regs1.bar5.write(addr)  # Map bank at addr
    regs1.cmd.write(2)     # Enable memory access
    stest.expect_equal(pci_mem.map[0][1], pci1,
                       "PCI device should have been mapped")
    mem_read = pci_mem.iface.memory_space.read
    stest.expect_equal(du.tuple_to_value_le(mem_read(None, addr, 4, 0)),
                       0x4711, "Version should be read")

    pci2.version = 0x3457
    addr = 0x300
    regs2.bar2.write(addr << 32)  # configure 64-bit bar or it will overlap
    regs2.bar5.write(addr)  # Map bank at addr
    regs2.cmd.write(2)     # Enable memory access
    stest.expect_equal(pci_mem.map[1][1], pci2,
                       "PCI device should have been mapped")
    stest.expect_equal(du.tuple_to_value_le(mem_read(None, addr, 4, 0)),
                       0x3457, "Version should be read")


cli.run_command('log-level 2')
test_id_regs()
test_multi_mapping()

print("All tests passed.")
