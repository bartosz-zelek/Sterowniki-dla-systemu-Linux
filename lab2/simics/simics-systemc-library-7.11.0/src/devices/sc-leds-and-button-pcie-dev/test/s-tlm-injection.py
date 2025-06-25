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


# Feature: TLM Socket Payload Injection
# Sockets, registered with the awareness framework, provide the Simics
# sc_tlm_fw_transport and sc_tlm_bw_transport interfaces. The interfaces
# correspond to the SystemC TLM2 transport interfaces and support injection.
# Test injecting a TLM transaction to read some data from bar0

from config import create_pci_device, LogHandler

(device, _, regs, _) = create_pci_device("pci_dev")
log = LogHandler()

regs.bar0.write(0x12340000)

run_command("pci_dev.gasket_PciMappingInterconnect_target_socket.initiator_socket.trace-sc")
log.reset()

data_read = (0,) * 4
device.gasket_PciMappingInterconnect_target_socket.initiator_socket.iface.sc_tlm_fw_transport.b_transport({'gp.command' : 0, 'gp.address' : 0x10, 'gp.data_ptr' : data_read}, 0)

SIM_continue(1)
log.match("[pci_dev.gasket_PciMappingInterconnect_target_socket.initiator_socket trace-b-out] read sz:4 addr:0x10 data:0x00000000")
log.match("[pci_dev.gasket_PciMappingInterconnect_target_socket.initiator_socket trace-b-in] read sz:4 addr:0x10 data:0x12340000 status:TLM_OK_RESPONSE")
