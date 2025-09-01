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


# Feature: Simics created proxy for all SystemC objects, useful information
# about the SystemC object can be fetched through info and status of the
# proxy object
# Test different kinds of SystemC objects implemented info and status command
# on the corresponding Simics proxies.

from config import create_pci_device

(device, _, _, _) = create_pci_device("pci_dev")

for proxy_object in ["pci_dev", # Adapter
                     "pci_dev.pci_device", # SC object
                     "pci_dev.pci_device.blink_onoff_event", # SC event
                     "pci_dev.pci_device.pci_bus_socket_export_0",  # SC export
                     "pci_dev.pci_device.pci_target_socket_port_0", # SC port
                     "pci_dev.pci_device.dma_button", # SC in
                     "pci_dev.pci_device.system_onoff_led_out", # SC out
                     "pci_dev.pci_device.blinkOnoff", # SC method
                     "pci_dev.pci_device.dma", # SC thread
                     "pci_dev.pci_device.pci_bus_socket", # Initiator socket
                     "pci_dev.pci_device.pci_target_socket", # Target socket
]:
    quiet_run_command("%s.info" % proxy_object)
    quiet_run_command("%s.status" % proxy_object)
