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


from config import create_pci_device, du
import stest

(device, pci_bus, regs, _) = create_pci_device("pci_dev")
pci_bus.bus_number = 32
pci_bus.pci_devices.append([0, 0, device])

def read_mem_memory(addr, size):
    mem_read = pci_bus.memory_space.iface.memory_space.read
    return du.tuple_to_value_le(mem_read(None, addr, size, 0))

def write_mem_memory(addr, size, value):
    mem_write = pci_bus.memory_space.iface.memory_space.write
    return mem_write(None, addr, du.value_to_tuple_le(value, size), 0)

def write_conf_memory(addr, size, value):
    conf_write = pci_bus.conf_space.iface.memory_space.write
    return conf_write(None, addr, du.value_to_tuple_le(value, size), 0)

# Map memory space
regs.bar0.write(0xfe000000)
regs.bar1.write(0xfe001000)
regs.bar2.write(0xfe002000)
write_conf_memory(0x200004, 2, 0x103)

src = regs.bar0.read() + 0x30
dst = src + 0x100
size = 0x100 - 0x30

# Initialize source memory content with its addr value
for addr in range(src, dst, 4):
    write_mem_memory(addr, 4, addr)
stest.expect_true(read_mem_memory(dst, 4) != read_mem_memory(src, 4))

# Trigger DMA button, each Simics cycle will copy 4 bytes from src to dst
device.port.dma.iface.signal.signal_raise()

for addr in range(dst, dst + size, 4):
    stest.expect_equal(read_mem_memory(addr, 4), addr - 0x100)
    SIM_continue(1000)
