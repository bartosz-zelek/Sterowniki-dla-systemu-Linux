# © 2024 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

# Test the PCI protocol

import common
import stest

dev = common.create_sample_pcie_device()

def test_with_fake_upstream_target():
    up = common.create_upstream_target()
    dev.upstream_target = up

    def test_write_command_reg(device_id):
        dev.command = [0, 0]

        # Verify command register is 0
        read_t = transaction_t(size = 2, read = True,
                               pcie_type = simics.PCIE_Type_Cfg,
                               pcie_device_id = device_id)
        simics.SIM_issue_transaction(dev, read_t, 0x4)
        stest.expect_equal(read_t.value_le, 0)
        stest.expect_equal(dev.command, [0, 0])

        # Write 0xa1 to command register
        write_t = transaction_t(size = 2, write = True,
                                pcie_type = simics.PCIE_Type_Cfg,
                                pcie_device_id = device_id,
                                value_le = 0xa1)
        simics.SIM_issue_transaction(dev, write_t, 0x4)

        # Verify the write is successful
        if device_id & 7 == 0:
            stest.expect_equal(dev.command, [0xa1, 0])
        else:
            stest.expect_equal(dev.command, [0, 0xa1])

        # Reset command
        dev.command = [0, 0]

    def test_write_version_reg(device_id):
        dev.version = [0, 0]

        # Verify version register is 0
        read_t = transaction_t(size = 4, read = True,
                               pcie_type = simics.PCIE_Type_Mem)
        simics.SIM_issue_transaction(dev, read_t,
                                     0x0 if device_id == 0 else 0x50000)
        stest.expect_equal(read_t.value_le, 0)
        stest.expect_equal(dev.version, [0, 0])

        # Write 0xa1 to version register
        write_t = transaction_t(size = 4, write = True,
                                pcie_type = simics.PCIE_Type_Mem,
                                value_le = 0xa1)
        simics.SIM_issue_transaction(dev, write_t,
                                     0x0 if device_id == 0 else 0x50000)

        # Verify the write is successful
        if device_id & 7 == 0:
            stest.expect_equal(dev.version, [0xa1, 0])
        else:
            stest.expect_equal(dev.version, [0, 0xa1])

        # Reset version
        dev.version = [0, 0]

    def test_map_unmap_pcie_memory(device_id):
        dev.log_level = 4

        # Test PCIe BAR is mapped when command is written 0x3
        write_t = transaction_t(size = 2, write = True,
                                pcie_type = simics.PCIE_Type_Cfg,
                                pcie_device_id = device_id,
                                value_le = 0x3)
        with stest.expect_log_mgr(
                log_type="info",
                regex=r"Memory address for function %d and bar ID 0 in range of \[%s-%s\] maps to socket ID %d" % (device_id,
                                   "0x0" if device_id == 0 else "0x50000",
                                   "0x10000" if device_id == 0 else "0x60000",
                                   2 if device_id == 0 else 4)):
            simics.SIM_issue_transaction(dev, write_t, 0x4)

        test_write_version_reg(device_id)

        # Test PCIe BAR is unmapped when command is written 0x0
        write_t = transaction_t(size = 2, write = True,
                                pcie_type = simics.PCIE_Type_Cfg,
                                pcie_device_id = device_id,
                                value_le = 0x0)
        with stest.expect_log_mgr(
                log_type="info",
                regex=r"Deleting mapping of type: Memory, base: %s" % ("0x0" if device_id == 0 else "0x50000")):
            simics.SIM_issue_transaction(dev, write_t, 0x4)

        dev.log_level = 1

    def test_reset(device_id):
        # Update BAR0 to 0x10000 and map it to test during the reset
        # it can be correctly unmapped (with the right address)
        write_t = transaction_t(size = 4, write = True,
                                pcie_type = simics.PCIE_Type_Cfg,
                                pcie_device_id = device_id,
                                value_le = 0x10000)
        simics.SIM_issue_transaction(dev, write_t, 0x10)
        if device_id == 0:
            stest.expect_equal(dev.base_address[0], 0x1000c)
        else:
            stest.expect_equal(dev.base_address[7], 0x1000c)

        write_t = transaction_t(size = 2, write = True,
                                pcie_type = simics.PCIE_Type_Cfg,
                                pcie_device_id = device_id,
                                value_le = 0x2)
        simics.SIM_issue_transaction(dev, write_t, 0x4)

        stest.expect_equal(dev.address_id_mem_map,
                           [[[0x10000, 0x20000], 2 if device_id == 0 else 4]])

        dev.log_level = 4
        with stest.expect_log_mgr(
                log_type="info",
                regex=r"Deleting mapping of type: Memory, base: 0x10000"):
            dev.iface.pcie_device.hot_reset()
        dev.log_level = 1

        # The BAR0's last 3 bits are not reset to 0
        stest.expect_equal(dev.base_address, [0xc, 0, 0, 0, 0, 0, 0] * 2)

    # Access the device using transaction interface
    for device_id in (0, 5):
        test_write_command_reg(device_id)

    # Update BAR0 in F5 to avoid overlap with BAR0 in F0
    write_t = transaction_t(size = 4, write = True,
                            pcie_type = simics.PCIE_Type_Cfg,
                            pcie_device_id = 5,
                            value_le = 5 << 16)
    simics.SIM_issue_transaction(dev, write_t, 0x10)

    for device_id in (0, 5):
        test_map_unmap_pcie_memory(device_id)
    for device_id in (0, 5):
        test_reset(device_id)

    dev.upstream_target = None

def test_with_downstream_port():
    up = common.create_upstream_target(fake=False, name="up2")

    stest.expect_equal(dev.upstream_target, None)
    up.devices = [[0x40, dev]]
    stest.expect_equal(dev.upstream_target, up)

    # Map 2 functions 0 and 5
    stest.expect_equal(up.functions,
                       [[0x40, dev.port.cfg0], [0x45, dev.port.cfg5]])

    # Check cfg_space
    stest.expect_equal(up.cfg_space.map,
                       [[0x400000, dev.port.cfg0, 0, 0, 65536, None, 0, 0, 0],
                        [0x450000, dev.port.cfg5, 0, 0, 65536, None, 0, 0, 0]])

    def test_write_command_reg(device_id):
        dev.command = [0, 0]

        # Verify command register is 0
        ret = up.cfg_space.iface.memory_space.read(
            up, 0x400004 | (device_id << 16), 2, 0)
        stest.expect_equal(ret[0], 0)
        stest.expect_equal(dev.command, [0, 0])

        # Write 0xa1 to command register
        up.cfg_space.iface.memory_space.write(up,
                                              0x400004 | (device_id << 16),
                                              (0xa1, 0x0), 2)

        # Verify the write is successful
        if device_id & 7 == 0:
            stest.expect_equal(dev.command, [0xa1, 0])
        else:
            stest.expect_equal(dev.command, [0, 0xa1])

        # Reset command
        dev.command = [0, 0]

    def test_write_bar_register(device_id):
        dev.log_level = 4
        stest.expect_equal(dev.base_address[0 if device_id == 0 else 7], 0xc)

        ret = up.cfg_space.iface.memory_space.read(
            up, 0x400010 | (device_id << 16), 4, 0)
        stest.expect_equal(ret[0], 0xc)

        # Write 0 to BAR0 does not alter the memory type (bit 2-1)
        up.cfg_space.iface.memory_space.write(up,
                                              0x400010 | (device_id << 16),
                                              (0x0, 0x0, 0x0, 0x0), 4)
        stest.expect_equal(dev.base_address[0 if device_id == 0 else 7], 0xc)

        # Move the BAR0-1 to device_id << 16
        up.cfg_space.iface.memory_space.write(up,
                                              0x400010 | (device_id << 16),
                                              (0x0, 0x0, device_id, 0x0), 4)
        stest.expect_equal(dev.base_address[0 if device_id == 0 else 7],
                           0xc | (device_id << 16))

        dev.log_level = 1

    def test_write_version_reg(device_id):
        dev.version = [0, 0]

        ret = up.mem_space.iface.memory_space.read(up, device_id << 16, 4, 0)
        stest.expect_equal(ret[0], 0)
        stest.expect_equal(dev.version, [0, 0])

        up.mem_space.iface.memory_space.write(up, device_id << 16, (0xa1, 0, 0, 0), 4)
        if device_id & 7 == 0:
            stest.expect_equal(dev.version, [0xa1, 0])
        else:
            stest.expect_equal(dev.version, [0, 0xa1])

    for device_id in (0, 5):
        test_write_command_reg(device_id)
        test_write_bar_register(device_id)

    # Map the PCI memory
    dev.log_level = 4
    with stest.expect_log_mgr(
            log_type="info",
            regex=r"Memory address for function 0 and bar ID 0 in range of \[0x0-0x10000\] maps to socket ID 2"):
        up.cfg_space.iface.memory_space.write(up, 0x400004, (0x3, 0x0), 2)

    with stest.expect_log_mgr(
            log_type="info",
            regex=r"Memory address for function 5 and bar ID 0 in range of \[0x50000-0x60000\] maps to socket ID 4"):
        up.cfg_space.iface.memory_space.write(up, 0x450004, (0x3, 0x0), 2)
    dev.log_level = 1

    for device_id in (0, 5):
        test_write_version_reg(device_id)

    up.devices = []
    stest.expect_equal(dev.upstream_target, None)
    stest.expect_equal(up.functions, [])

test_with_fake_upstream_target()
test_with_downstream_port()
