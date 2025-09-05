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


import dev_util
import stest
import cli

from simics import SIM_hap_add_callback_obj

# Test the PCI vendor and device IDs
def test_id_regs(regs, sample_pci):
    # Configuration Space
    stest.expect_equal(regs.vendor_id.read(), 0x104c)
    stest.expect_equal(regs.device_id.read(), (0xac10 << 16) | 0x104c)
    stest.expect_equal(sample_pci.vendor_id, 0x104c)
    stest.expect_equal(sample_pci.device_id, 0xac10)
    # Device registers
    stest.expect_equal(regs.version.read(), 0x4711)
    stest.expect_equal(sample_pci.version, 0x4711)

# Test setting BAR to map the device in memory
def test_mapping(regs, addr, version, io_space, memory_space, sample_pci):
    regs.bar0.write(addr)  # IO
    regs.cmd.io = 1        # Enable io access
    stest.expect_equal(len(io_space.map), 1, "IO mapped")
    stest.expect_equal(io_space.map[0][1], sample_pci,
                       "PCI device should have been mapped, IO BAR")

    regs.bar5.write(addr)  # MEM
    addr64 = addr << 32
    regs.bar2.write(addr64)  # MEM64
    regs.cmd.mem = 1       # Enable memory access (both 32-bit and 64-bit!)
    stest.expect_equal(len(memory_space.map), 2, "MEM mapped")
    stest.expect_equal(memory_space.map[0][1], sample_pci,
                       "PCI device should have been mapped, 32-bit BAR")
    stest.expect_equal(memory_space.map[1][1], sample_pci,
                       "PCI device should have been mapped, 64-bit BAR")

    mem_read = memory_space.iface.memory_space.read
    stest.expect_equal(dev_util.tuple_to_value_le(mem_read(None, addr, 4, 0)),
                       version, "Version should be read, 32-bit BAR")
    stest.expect_equal(dev_util.tuple_to_value_le(mem_read(None, addr64, 4, 0)),
                       version, "Version should be read, 64-bit BAR")

    regs.cmd.write(0)  # clear mappings

# Test interface
def test_interface(adapter, sample_pci):
    logs = []
    def log_pci(arg, obj, logtype, msg):
        logs.append(msg)
    SIM_hap_add_callback_obj("Core_Log_Message", adapter, 0, log_pci, None)

    cli.run_command('log-level 2')
    sample_pci.iface.pci_device.bus_reset()
    stest.expect_true(logs[1].startswith(
        'PCI device operation: bus_reset'))
    sample_pci.iface.pci_device.system_error()
    stest.expect_true(logs[3].startswith(
        'PCI device operation: system_error'))
    sample_pci.iface.pci_device.interrupt_raised(7)
    stest.expect_true(logs[4].startswith(
        'PCI device operation: interrupt_raised pin = 7'))
    sample_pci.iface.pci_device.interrupt_lowered(9)
    stest.expect_true(logs[5].startswith(
        'PCI device operation: interrupt_lowered pin = 9'))
    cli.run_command('log-level 1')

# Test multi-function (MF) device
# NOTE: the PCI F5 has been designed to work just like F0
def test_mf_device(regs, io_space, memory_space, sample_pci):
    # Configuration Space
    stest.expect_equal(regs.f5.vendor_id.read(), 0x104c)
    stest.expect_equal(regs.f5.device_id.read(), (0xac15 << 16) | 0x104c)
    # Device registers
    stest.expect_equal(regs.f5.version.read(), 0x4712)
    # Mapping
    test_mapping(regs.f5, 0x200, 0x4712, io_space, memory_space, sample_pci)

# Test MF device mapped using pci_bus logic and the pci_multi_function_device interface
def test_mf_device_mapped_by_pci_bus(bus, pci):
    bus_num = 0
    dev_num = 10
    bus.pci_devices = [[dev_num, 0, pci]]
    stest.expect_equal(len(bus.conf_space.map), 2, "Both functions mapped")
    def test_fn(fn):
        # PCIe bus BDF mapping:
        addr = bus_num << 20 | dev_num << 15 | fn << 12
        l = cli.run_command('pci_conf.read %d -l' % addr)
        stest.expect_equal(l, (0xac10 + fn) << 16 | 0x104c, "vendor || device")

    for fn in [0, 5]:
        test_fn(fn)

# Test DMA/memory access
def test_dma(memory_space, sample_pci):
    mem = dev_util.Memory()
    memory_space.default_target = [mem.obj, 0, 0, None]
    size = 4
    data = dev_util.value_to_tuple_le(0xdeadbeef, size)
    src_addr = 0x1000
    dst_addr = 0x2000
    mem.write(src_addr, data)
    # the trigger will initiate an size bytes read from src_addr and write that
    # data back to dst_addr, so that we can test that our mem object now
    # contains both the original and the copied data
    sample_pci.dma_trigger = [src_addr, dst_addr, size]
    stest.expect_equal(mem.mem,
                       [(src_addr, list(data)),
                        (dst_addr, list(data))])
