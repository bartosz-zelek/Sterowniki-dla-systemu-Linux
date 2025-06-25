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


# Bug: before (and include) Simics 5 buiuld-id 5137, the output
# streaming_width value in the trace log of TLM socket used hex value
# without 0x prefix. Now fixed by printing it in decimal format.

from config import create_pci_device, LogHandler
import dev_util as du

(device, _, _, _) = create_pci_device("pci_dev")
log = LogHandler()

# Stream_width only show in the trace log when size >= 16
test_reg1 = du.Register_LE((device, 255, 0x10), size = 16)
run_command('pci_dev.gasket_PciMappingInterconnect_target_socket.initiator_socket.trace-sc')

log.reset()
test_reg1.read()
log.match("[pci_dev.gasket_PciMappingInterconnect_target_socket.initiator_socket trace-b-out] read sz:16 addr:0x10 data:0x0000000000000000 streaming_width:16")

test_reg2 = du.Register_LE((device, 255, 0x18), size = 32)
log.reset()
test_reg2.read()
log.match("[pci_dev.gasket_PciMappingInterconnect_target_socket.initiator_socket trace-b-out] read sz:32 addr:0x18 data:0x0000000000000000 streaming_width:32")
