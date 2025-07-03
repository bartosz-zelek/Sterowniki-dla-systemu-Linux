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

# Test the interface implemented on the sample device

import common
import stest

dev = common.create_sample_pcie_device()
up = common.create_upstream_target()

def test_signal_interface():
    '''Sample device implemented signal to support warm reset'''
    dev.log_level = 4
    with stest.expect_log_mgr(log_type="info", regex=r"Warm reset"):
        dev.iface.signal.signal_raise()
    dev.iface.signal.signal_lower()
    dev.log_level = 1

def test_pcie_device_interface():
    # Cannot connect to None
    stest.expect_log(dev.iface.pcie_device.connected, [None, 0xdea0], dev,
                     "error",
                     "can't connect to NULL")
    stest.expect_equal(dev.upstream_target, None)
    stest.expect_equal(up.object_data.add_function, [])

    # Cannot disconnect from None
    stest.expect_log(dev.iface.pcie_device.disconnected, [None, 0xdea0], dev,
                     "error",
                     "can't disconnect from NULL")

    # Test upstream target connection
    dev.iface.pcie_device.connected(up, 0xdea0)
    stest.expect_equal(dev.upstream_target, up)
    stest.expect_equal(up.object_data.add_function, [0xdea0, 0xdea5])
    stest.expect_equal(up.object_data.del_function, [0xdea0, 0xdea5])

    up.object_data.del_function.clear()
    dev.iface.pcie_device.disconnected(up, 0xdea0)
    stest.expect_equal(dev.upstream_target, None)
    stest.expect_equal(up.object_data.del_function, [0xdea0, 0xdea5])

    # Test hot_reset
    dev.log_level = 4
    stest.expect_log(dev.iface.pcie_device.hot_reset, [], dev,
                     "info", "Hot Reset")
    stest.expect_equal(dev.command, [0, 0])
    # BAR0 is hardwired as 64 bits BAR
    stest.expect_equal(dev.base_address, [0xc, 0, 0, 0, 0, 0, 0] * 2)
    dev.log_level = 1

def test_transaction_interface():
    # Test direct call the interface
    t = transaction_t(size = 4, read = True, pcie_type = simics.PCIE_Type_Cfg,
                      pcie_device_id = 0)
    dev.iface.transaction.issue(t, 0x0)
    stest.expect_equal(t.value_le, 0xac10104c)

    # Test different PCIe type are handled correctly
    dev.log_level = 4
    for pcie_type, type_str in [[simics.PCIE_Type_Cfg, "Config"],
                                [simics.PCIE_Type_Mem, "Memory"],
                                [simics.PCIE_Type_IO, "I/O"],
                                [simics.PCIE_Type_Msg, "Message"]]:
        t = transaction_t(size = 4, read = True, pcie_type = pcie_type,
                          pcie_device_id = 0)
        with stest.expect_log_mgr(
                log_type="info",
                regex=r"Received a PCIe transaction with type %s" % type_str):
            simics.SIM_issue_transaction(dev, t, 0x100)
    dev.log_level = 1

    # Other routing
    t.pcie_type = simics.PCIE_Type_Other
    exc = SIM_issue_transaction(dev, t, 0x100)
    stest.expect_equal(exc, simics.Sim_PE_IO_Error)

    # Test different PCIe device ID are handled correctly
    dev.log_level = 4
    for pcie_device_id, id_str in [[0, r"0x0 \(0:0.0\)"],
                                   [5, r"0x5 \(0:0.5\)"],
                                   [0xac10, r"0xac10 \(172:2.0\)"]]:
        t = transaction_t(size = 4, read = True, pcie_type = simics.PCIE_Type_Cfg,
                          pcie_device_id = pcie_device_id)
        with stest.expect_log_mgr(
                log_type="info",
                regex=r"device id %s" % id_str):
            simics.SIM_issue_transaction(dev, t, 0x100)
    dev.log_level = 1

test_signal_interface()
test_pcie_device_interface()
test_transaction_interface()
