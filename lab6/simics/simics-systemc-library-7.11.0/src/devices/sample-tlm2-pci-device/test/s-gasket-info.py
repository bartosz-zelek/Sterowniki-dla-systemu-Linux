# © 2020 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


import config
import stest

config.create_pci_device('sample_pci')

# Trace log handler
logs = []
def log(data, src, len):
    global logs
    for line in src.splitlines():
        logs.append(line)

def reset_log():
    global logs
    logs = []

SIM_add_output_handler(log, None)
SIM_run_command('sample_pci.gasket-info')

stest.expect_true(logs == [
'simics2tlm',
'┌──────────┬───────────┬────────────────────────────────────────────────────┬───────────────────────────────────────────┐',
'│  OBJECT  │ INTERFACE │                       GASKET                       │                  SOCKET                   │',
'├──────────┼───────────┼────────────────────────────────────────────────────┼───────────────────────────────────────────┤',
'│sample_pci│io_memory  │gasket_PciMappingInterconnect_target_socket         │PciMappingInterconnect.target_socket       │',
'│sample_pci│io_memory  │gasket_PciMappingInterconnect_target_socket_0       │PciMappingInterconnect.target_socket       │',
'│sample_pci│io_memory  │gasket_pci_device_io_target_socket                  │pci_device.io_target_socket                │',
'│sample_pci│io_memory  │gasket_pci_device_mem64_target_socket               │pci_device.mem64_target_socket             │',
'│sample_pci│io_memory  │gasket_pci_device_mem_target_socket                 │pci_device.mem_target_socket               │',
'│sample_pci│io_memory  │gasket_pci_device_io_target_socket_0                │pci_device.io_target_socket                │',
'│sample_pci│io_memory  │gasket_pci_device_mem64_target_socket_0             │pci_device.mem64_target_socket             │',
'│sample_pci│io_memory  │gasket_pci_device_mem_target_socket_0               │pci_device.mem_target_socket               │',
'│sample_pci│pci_device │gasket_PciMappingInterconnect_simics_target_socket  │PciMappingInterconnect.simics_target_socket│',
'│sample_pci│pci_express│gasket_PciMappingInterconnect_simics_target_socket_0│PciMappingInterconnect.simics_target_socket│',
'└──────────┴───────────┴────────────────────────────────────────────────────┴───────────────────────────────────────────┘'])
