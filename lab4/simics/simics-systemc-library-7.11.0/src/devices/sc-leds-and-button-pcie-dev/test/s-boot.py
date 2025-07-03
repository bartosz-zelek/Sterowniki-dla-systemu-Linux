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


# mimic the register access during QSP booting a DML PCI device
# The comment is the PCI device logs when booting Linux
# here we test the read value are the same and write the same value
# to setup the device during boot

from config import create_pci_device, du
from stest import expect_equal

(device, pci_bus, regs, _) = create_pci_device("pci_dev")
# on the QSP, this is actually the board.mb.nb.pcie_p2_bus with
# offset=0x20_0000, but here a bus number of 32 would produce the same
# conf_space offset
pci_bus.bus_number = 32
pci_bus.pci_devices.append([0, 0, device])

def read_memory(addr, size):
    conf_read = pci_bus.conf_space.iface.memory_space.read
    return du.tuple_to_value_le(conf_read(None, addr, size, 0))

def write_memory(addr, size, value):
    conf_write = pci_bus.conf_space.iface.memory_space.write
    return conf_write(None, addr, du.value_to_tuple_le(value, size), 0)

# Read from register pci_config.vendor_id (addr 0x200000) -> 0x8086
# Read from register pci_config.class_code (addr 0x20000a) -> 0x980
# Read from register pci_config.header_type (addr 0x20000e) -> 0
expect_equal(read_memory(0x200000, 2), 0x8086)
expect_equal(read_memory(0x20000a, 2), 0x980)
expect_equal(read_memory(0x20000e, 1), 0)

# 4 byte read
# Read from register pci_config.vendor_id (addr 0x200000) -> 0x8086
# Read from register pci_config.device_id (addr 0x200000) -> 0x0d61
# Read from register pci_config.revision_id (addr 0x200008) -> 0x1
# Read from register pci_config.class_code (addr 0x200008) -> 0x98000
expect_equal(read_memory(0x200000, 4), 0x0d618086)
expect_equal(read_memory(0x200008, 4), 0x9800001)

# Read from register pci_config.base_address_0 (addr 0x200010) -> 0
# Write to register pci_config.base_address_0 (addr 0x200010) <- 0xffffffff
# Read from register pci_config.base_address_0 (addr 0x200010) -> 0xfffff000
# Write to register pci_config.base_address_0 (addr 0x200010) <- 0
# Read from register pci_config.base_address_1 (addr 0x200014) -> 0
# Write to register pci_config.base_address_1 (addr 0x200014) <- 0xffffffff
# Read from register pci_config.base_address_1 (addr 0x200014) -> 0xffffff00
# Write to register pci_config.base_address_1 (addr 0x200014) <- 0
# Read from register pci_config.base_address_2 (addr 0x200018) -> 0
# Write to register pci_config.base_address_2 (addr 0x200018) <- 0xffffffff
# Read from register pci_config.base_address_2 (addr 0x200018) -> 0xffffff00
# Write to register pci_config.base_address_2 (addr 0x200018) <- 0
expect_equal(regs.bar0.read(), 0)
regs.bar0.write(0xffffffff)
expect_equal(regs.bar0.read(), 0xfffff000)
regs.bar0.write(0)
expect_equal(regs.bar1.read(), 0)
regs.bar1.write(0xffffffff)
expect_equal(regs.bar1.read(), 0xffffff00)
regs.bar1.write(0)
expect_equal(regs.bar2.read(), 0)
regs.bar2.write(0xffffffff)
expect_equal(regs.bar2.read(), 0xffffff00)
regs.bar2.write(0)

# Read from register pci_config.base_address_3 (addr 0x20001c) -> 0
# Write to register pci_config.base_address_3 (addr 0x20001c) <- 0xffffffff
# Read from register pci_config.base_address_3 (addr 0x20001c) -> 0
# Write to register pci_config.base_address_3 (addr 0x20001c) <- 0
# ...
for reg in [regs.bar3, regs.bar4, regs.bar5]:
    expect_equal(reg.read(), 0)
    reg.write(0xffffffff)
    expect_equal(reg.read(), 0)
    reg.write(0)

# Read from register pci_config.expansion_rom_base (addr 0x200030) -> 0
# Write to register pci_config.expansion_rom_base (addr 0x200030) <- 0xfffff800
# Read from register pci_config.expansion_rom_base (addr 0x200030) -> 0
# Write to register pci_config.expansion_rom_base (addr 0x200030) <- 0
expect_equal(read_memory(0x200030, 4), 0)
write_memory(0x200030, 4, 0xfffff800)
expect_equal(read_memory(0x200030, 4), 0)
write_memory(0x200030, 4, 0)

# Write to register pci_config.base_address_0 (addr 0x200010) <- 0xfe000000
# Write to register pci_config.base_address_1 (addr 0x200014) <- 0xfe001000
# Write to register pci_config.base_address_2 (addr 0x200018) <- 0xfe002000
regs.bar0.write(0xfe000000)
regs.bar1.write(0xfe001000)
regs.bar2.write(0xfe002000)

# Read from register pci_config.interrupt_pin (addr 0x20003d) -> 0
# Read from register pci_config.command (addr 0x200004) -> 0
# Write to register pci_config.command (addr 0x200004) <- 0x103
expect_equal(read_memory(0x20003d, 1), 0)
expect_equal(read_memory(0x200004, 2), 0)
write_memory(0x200004, 2, 0x103)

# Read from register pci_config.status (addr 0x200006) -> 0x10
# Read from register pci_config.capabilities_ptr (addr 0x200034) -> 0x70
# Read from register pci_config.msix_capability_header (addr 0x200070) -> 0x11
# Read from register pci_config.msix_capability_header (addr 0x200071) -> 0x80
# Read from register pci_config.exp_capability_header (addr 0x200080) -> 0x10
# Read from register pci_config.exp_capabilities (addr 0x200082) -> 0x2
# Read from register pci_config.exp_dev_cap (addr 0x200084) -> 0x8240
expect_equal(read_memory(0x200006, 2), 0x10)
expect_equal(read_memory(0x200034, 1), 0x70)
expect_equal(read_memory(0x200070, 1), 0x11)
expect_equal(read_memory(0x200071, 1), 0x80)
expect_equal(read_memory(0x200080, 1), 0x10)
expect_equal(read_memory(0x200082, 2), 0x2)
expect_equal(read_memory(0x200084, 2), 0x8240)

# Read from register pci_config.command (addr 0x200004) -> 0x103
# Write to register pci_config.command (addr 0x200004) <- 0x100
# Read from register pci_config.base_address_0 (addr 0x200010) -> 0xfe000000
# Write to register pci_config.base_address_0 (addr 0x200010) <- 0xffffffff
# Read from register pci_config.base_address_0 (addr 0x200010) -> 0xfffff000
# Write to register pci_config.base_address_0 (addr 0x200010) <- 0xfe000000
# Read from register pci_config.base_address_1 (addr 0x200014) -> 0xfe001000
# Write to register pci_config.base_address_1 (addr 0x200014) <- 0xffffffff
# Read from register pci_config.base_address_1 (addr 0x200014) -> 0xffffff00
# Write to register pci_config.base_address_1 (addr 0x200014) <- 0xfe001000
# Read from register pci_config.base_address_2 (addr 0x200018) -> 0xfe002000
# Write to register pci_config.base_address_2 (addr 0x200018) <- 0xffffffff
# Read from register pci_config.base_address_2 (addr 0x200018) -> 0xffffff00
# Write to register pci_config.base_address_2 (addr 0x200018) <- 0xfe002000
# Write to register pci_config.command (addr 0x200004) <- 0x103
# Read from register pci_config.command (addr 0x200004) -> 0x103
# Write to register pci_config.command (addr 0x200004) <- 0x100
expect_equal(read_memory(0x200004, 2), 0x103)
write_memory(0x200004, 2, 0x100)
expect_equal(regs.bar0.read(), 0xfe000000)
regs.bar0.write(0xffffffff)
expect_equal(regs.bar0.read(), 0xfffff000)
regs.bar0.write(0)
expect_equal(regs.bar1.read(), 0xfe001000)
regs.bar1.write(0xffffffff)
expect_equal(regs.bar1.read(), 0xffffff00)
regs.bar1.write(0)
expect_equal(regs.bar2.read(), 0xfe002000)
regs.bar2.write(0xffffffff)
expect_equal(regs.bar2.read(), 0xffffff00)
regs.bar2.write(0)
write_memory(0x200004, 2, 0x103)
expect_equal(read_memory(0x200004, 2), 0x103)
write_memory(0x200004, 2, 0x100)

# Read from register pci_config.subsystem_vendor_id (addr 0x20002c) -> 0
# Read from register pci_config.subsystem_id (addr 0x20002e) -> 0
# Read from register pci_config.status (addr 0x200006) -> 0x10
# Read from register pci_config.capabilities_ptr (addr 0x200034) -> 0x70
# Read from register pci_config.msix_capability_header (addr 0x200070) -> 0x11
# Read from register pci_config.msix_capability_header (addr 0x200071) -> 0x80
# Read from register pci_config.exp_capability_header (addr 0x200080) -> 0x10
# Read from register pci_config.exp_capability_header (addr 0x200081) -> 0
# Read from register pci_config.msix_control (addr 0x200072) -> 0x1
# Write to register pci_config.msix_control (addr 0x200072) <- 0x1
# Read from register pci_config.exp_dev_control (addr 0x200088) -> 0
# Read from register pci_config.cache_line_size (addr 0x20000c) -> 0
expect_equal(read_memory(0x20002c, 2), 0)
expect_equal(read_memory(0x20002e, 2), 0)
expect_equal(read_memory(0x200006, 2), 0x10)
expect_equal(read_memory(0x200034, 1), 0x70)
expect_equal(read_memory(0x200070, 1), 0x11)
expect_equal(read_memory(0x200071, 1), 0x80)
expect_equal(read_memory(0x200080, 1), 0x10)
expect_equal(read_memory(0x200081, 1), 0)
expect_equal(read_memory(0x200072, 1), 0x1)
write_memory(0x200072, 1, 0x1)
expect_equal(read_memory(0x200088, 2), 0)
expect_equal(read_memory(0x20000c, 2), 0)
