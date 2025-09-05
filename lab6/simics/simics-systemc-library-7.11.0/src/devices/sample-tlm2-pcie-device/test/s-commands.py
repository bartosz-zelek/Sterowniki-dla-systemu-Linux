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

# Test all commands can be run on the sample device

import common
import stest

dev = common.create_sample_pcie_device()

result = cli.quiet_run_command("dev.info")
stest.expect_equal(result[1],
'''Information about dev [class sample_tlm2_pcie_device]
=====================================================

Common:
    Connect to PCIe upstream target : none

Function 0:
                          Vendor ID : 0x104c
                          Device ID : 0xac10

Function 5:
                          Vendor ID : 0x104c
                          Device ID : 0xac10
''')

result = cli.quiet_run_command("dev.status")
stest.expect_equal(result[1],
'''Status of dev [class sample_tlm2_pcie_device]
=============================================

Function 0:
    Command register : 0x0
     Status register : 0x10
    Version register : 0x4711

Function 5:
    Command register : 0x0
     Status register : 0x10
    Version register : 0x4712
''')

cli.run_command("set-table-border-style style = borderless")
result = cli.quiet_run_command("dev.gasket-info")
stest.expect_equal(result[1],
'''simics2tlm
OBJECT   INTERFACE                               GASKET                                                      SOCKET                         
dev     pcie_device  gasket_PcieMappingInterconnect_dummy_pcie_device_target_socket  PcieMappingInterconnect.dummy_pcie_device_target_socket
dev     transaction  gasket_PcieMappingInterconnect_transaction_target_socket        PcieMappingInterconnect.transaction_target_socket      
simics2systemc
OBJECT  INTERFACE                 GASKET                                       SIGNAL                     
dev     signal     gasket_PcieMappingInterconnect_port_0  gasket_PcieMappingInterconnect_port_0.output_pin
''')  # noqa: W291 (there is trailing whitespace in output)
