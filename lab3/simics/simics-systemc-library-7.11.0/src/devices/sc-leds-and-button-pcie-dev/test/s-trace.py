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


# Feature: Simics supports tracing different kind of SystemC objects when
# their 'state' is changed. Refer to SystemC Library Programming Guide for
# what kind of SystemC objects is tracable and what is considered as a 'state'
# Test we can enable trace on individual SystemC event object, SystemC signal
# port object, SystemC process and SystemC socket. Also test we can enable
# tracing on all smae type SystemC objects.

from config import create_pci_device, LogHandler

(device, _, _, _) = create_pci_device("pci_dev")
log = LogHandler()

# Trace single object
# SC signal port
run_command('pci_dev.pci_device.dma_button.trace-sc')
log.reset()

device.gasket_pci_device_dma_button.output_pin.iface.sc_signal_write.write(True)
SIM_continue(10**3)
log.match("[pci_dev.pci_device.dma_button trace-signal-port] 0 -> 1")

run_command('pci_dev.pci_device.dma_button.untrace-sc')
log.reset()

device.gasket_pci_device_dma_button.output_pin.iface.sc_signal_write.write(True)
SIM_continue(10**3)
log.is_empty()

# SC event
run_command('pci_dev.pci_device.blink_onoff_event.trace-sc')
log.reset()

device.pci_device.blink_onoff_event.iface.sc_event.notify()
log.is_empty()  # immediate notify outside of simulation is not allowed
SIM_continue(10**3) # must run to 'trigger' the notify which will trigger the event
log.match("[pci_dev.pci_device.blink_onoff_event trace-event] 2 ns pci_device.blink_onoff_event, event triggered")

run_command('pci_dev.pci_device.blink_onoff_event.untrace-sc')
log.reset()
device.pci_device.blink_onoff_event.iface.sc_event.notify()
SIM_continue(10**3)
log.is_empty()

# SC process
run_command('pci_dev.pci_device.blinkOnoff.trace-sc')
log.reset()

device.pci_device.blink_onoff_event.iface.sc_event.notify()
SIM_continue(10**3)
log.match("[pci_dev.pci_device.blinkOnoff trace-process] 4 ns pci_device.blinkOnoff, process activation")

run_command('pci_dev.pci_device.blinkOnoff.untrace-sc')
log.reset()

device.pci_device.blink_onoff_event.iface.sc_event.notify()
SIM_continue(10**3)
log.is_empty()

# SC socket
run_command('pci_dev.pci_device.pci_target_socket.trace-sc')
log.reset()

device.iface.pci_device.bus_reset()
SIM_continue(10**3)
log.match("[pci_dev.pci_device.pci_target_socket trace-b-in] ignore sz:0 addr:0x0")
log.match("[pci_dev.pci_device.pci_target_socket trace-b-out] ignore sz:0 addr:0x0 status:TLM_OK_RESPONSE")

run_command('pci_dev.pci_device.pci_target_socket.untrace-sc')
log.reset()
device.iface.pci_device.bus_reset()
SIM_continue(10**3)
log.is_empty()

# SC event
run_command('pci_dev.trace-sc-event-all')
log.reset()

device.pci_device.blink_onoff_event.iface.sc_event.notify()
SIM_continue(10**3)
log.match("[pci_dev.pci_device.blink_onoff_event trace-event] 8 ns pci_device.blink_onoff_event, event triggered")

run_command('pci_dev.untrace-sc-event-all')
log.reset()
device.pci_device.blink_onoff_event.iface.sc_event.notify()
SIM_continue(10**3)
log.is_empty()

# SC process
run_command('pci_dev.trace-sc-process-all')
log.reset()

device.pci_device.blink_onoff_event.iface.sc_event.notify()
SIM_continue(10**3)
log.match("[pci_dev.pci_device.blinkOnoff trace-process] 10 ns pci_device.blinkOnoff, process activation")
log.match("[pci_dev.gasket_pci_device_system_onoff_led_out.update trace-process] 10 ns gasket_pci_device_system_onoff_led_out.update, process activation")
log.contains("[pci_dev.pci_device.dma trace-process] 11 ns pci_device.dma, process activation")

run_command('pci_dev.untrace-sc-process-all')
log.reset()

device.pci_device.blink_onoff_event.iface.sc_event.notify()
SIM_continue(10**3)
log.is_empty()

# SC socket
run_command('pci_dev.trace-sc-socket-all')
log.reset()

device.iface.pci_device.bus_reset()
log.match("[pci_dev.PciMappingInterconnect.pci_bus_initiator_socket trace-b-out] ignore sz:0 addr:0x0")
log.match("[pci_dev.gasket_PciMappingInterconnect_pci_bus_initiator_socket.target_socket trace-b-in] ignore sz:0 addr:0x0")
log.match("[pci_dev.gasket_PciMappingInterconnect_pci_bus_initiator_socket.target_socket trace-b-out] ignore sz:0 addr:0x0 status:TLM_OK_RESPONSE")
log.match("[pci_dev.PciMappingInterconnect.pci_bus_initiator_socket trace-b-in] ignore sz:0 addr:0x0 status:TLM_OK_RESPONSE")
log.match("[pci_dev.PciMappingInterconnect.pci_bus_initiator_socket trace-b-out] ignore sz:0 addr:0x0")
log.match("[pci_dev.gasket_PciMappingInterconnect_pci_bus_initiator_socket.target_socket trace-b-in] ignore sz:0 addr:0x0")
log.match("[pci_dev.gasket_PciMappingInterconnect_pci_bus_initiator_socket.target_socket trace-b-out] ignore sz:0 addr:0x0 status:TLM_OK_RESPONSE")
log.match("[pci_dev.PciMappingInterconnect.pci_bus_initiator_socket trace-b-in] ignore sz:0 addr:0x0 status:TLM_OK_RESPONSE")
log.match("[pci_dev.PciMappingInterconnect.pci_bus_initiator_socket trace-b-out] ignore sz:0 addr:0x0")
log.match("[pci_dev.gasket_PciMappingInterconnect_pci_bus_initiator_socket.target_socket trace-b-in] ignore sz:0 addr:0x0")
log.match("[pci_dev.gasket_PciMappingInterconnect_pci_bus_initiator_socket.target_socket trace-b-out] ignore sz:0 addr:0x0 status:TLM_OK_RESPONSE")
log.match("[pci_dev.PciMappingInterconnect.pci_bus_initiator_socket trace-b-in] ignore sz:0 addr:0x0 status:TLM_OK_RESPONSE")
log.match("[pci_dev.gasket_PciMappingInterconnect_simics_target_socket.initiator_socket trace-b-out] ignore sz:0 addr:0x0")
log.match("[pci_dev.PciMappingInterconnect.simics_target_socket trace-b-in] ignore sz:0 addr:0x0")
log.match("[pci_dev.PciMappingInterconnect.simple_initiator_socket_0 trace-b-out] ignore sz:0 addr:0x0")
log.match("[pci_dev.pci_device.pci_target_socket trace-b-in] ignore sz:0 addr:0x0")
log.match("[pci_dev.pci_device.pci_target_socket trace-b-out] ignore sz:0 addr:0x0 status:TLM_OK_RESPONSE")
log.match("[pci_dev.PciMappingInterconnect.simple_initiator_socket_0 trace-b-in] ignore sz:0 addr:0x0 status:TLM_OK_RESPONSE")
log.match("[pci_dev.PciMappingInterconnect.simics_target_socket trace-b-out] ignore sz:0 addr:0x0 status:TLM_OK_RESPONSE")
log.match("[pci_dev.gasket_PciMappingInterconnect_simics_target_socket.initiator_socket trace-b-in] ignore sz:0 addr:0x0 status:TLM_OK_RESPONSE")
log.is_empty()

SIM_continue(int(2e3))
log.match("[pci_dev.pci_device.pci_bus_socket trace-b-out] read sz:4 addr:0x64 data:0x00000000 streaming_width:0")
log.match("[pci_dev.PciMappingInterconnect.pci_bus_target_socket trace-b-in] read sz:4 addr:0x64 data:0x00000000 streaming_width:0")
log.match("[pci_dev.PciMappingInterconnect.pci_bus_initiator_socket trace-b-out] read sz:4 addr:0x64 data:0x00000000 streaming_width:0")
log.match("[pci_dev.gasket_PciMappingInterconnect_pci_bus_initiator_socket.target_socket trace-b-in] read sz:4 addr:0x64 data:0x00000000 streaming_width:0")
log.match("[pci_dev.gasket_PciMappingInterconnect_pci_bus_initiator_socket.target_socket trace-b-out] read sz:4 addr:0x64 data:0x00000000 status:TLM_OK_RESPONSE streaming_width:0")
log.match("[pci_dev.PciMappingInterconnect.pci_bus_initiator_socket trace-b-in] read sz:4 addr:0x64 data:0x00000000 status:TLM_OK_RESPONSE streaming_width:0")
log.match("[pci_dev.PciMappingInterconnect.pci_bus_target_socket trace-b-out] read sz:4 addr:0x64 data:0x00000000 status:TLM_OK_RESPONSE streaming_width:0")
log.match("[pci_dev.pci_device.pci_bus_socket trace-b-in] read sz:4 addr:0x64 data:0x00000000 status:TLM_OK_RESPONSE streaming_width:0")
log.match("[pci_dev.pci_device.pci_bus_socket trace-b-out] write sz:4 addr:0x164 data:0x00000000 streaming_width:0")
log.match("[pci_dev.PciMappingInterconnect.pci_bus_target_socket trace-b-in] write sz:4 addr:0x164 data:0x00000000 streaming_width:0")
log.match("[pci_dev.PciMappingInterconnect.pci_bus_initiator_socket trace-b-out] write sz:4 addr:0x164 data:0x00000000 streaming_width:0")
log.match("[pci_dev.gasket_PciMappingInterconnect_pci_bus_initiator_socket.target_socket trace-b-in] write sz:4 addr:0x164 data:0x00000000 streaming_width:0")
log.match("[pci_dev.gasket_PciMappingInterconnect_pci_bus_initiator_socket.target_socket trace-b-out] write sz:4 addr:0x164 data:0x00000000 status:TLM_OK_RESPONSE streaming_width:0")
log.match("[pci_dev.PciMappingInterconnect.pci_bus_initiator_socket trace-b-in] write sz:4 addr:0x164 data:0x00000000 status:TLM_OK_RESPONSE streaming_width:0")
log.match("[pci_dev.PciMappingInterconnect.pci_bus_target_socket trace-b-out] write sz:4 addr:0x164 data:0x00000000 status:TLM_OK_RESPONSE streaming_width:0")
log.match("[pci_dev.pci_device.pci_bus_socket trace-b-in] write sz:4 addr:0x164 data:0x00000000 status:TLM_OK_RESPONSE streaming_width:0")
log.match("[pci_dev.pci_device.pci_bus_socket trace-b-out] read sz:4 addr:0x68 data:0x00000000 streaming_width:0")
log.match("[pci_dev.PciMappingInterconnect.pci_bus_target_socket trace-b-in] read sz:4 addr:0x68 data:0x00000000 streaming_width:0")
log.match("[pci_dev.PciMappingInterconnect.pci_bus_initiator_socket trace-b-out] read sz:4 addr:0x68 data:0x00000000 streaming_width:0")
log.match("[pci_dev.gasket_PciMappingInterconnect_pci_bus_initiator_socket.target_socket trace-b-in] read sz:4 addr:0x68 data:0x00000000 streaming_width:0")
log.match("[pci_dev.gasket_PciMappingInterconnect_pci_bus_initiator_socket.target_socket trace-b-out] read sz:4 addr:0x68 data:0x00000000 status:TLM_OK_RESPONSE streaming_width:0")
log.match("[pci_dev.PciMappingInterconnect.pci_bus_initiator_socket trace-b-in] read sz:4 addr:0x68 data:0x00000000 status:TLM_OK_RESPONSE streaming_width:0")
log.match("[pci_dev.PciMappingInterconnect.pci_bus_target_socket trace-b-out] read sz:4 addr:0x68 data:0x00000000 status:TLM_OK_RESPONSE streaming_width:0")
log.match("[pci_dev.pci_device.pci_bus_socket trace-b-in] read sz:4 addr:0x68 data:0x00000000 status:TLM_OK_RESPONSE streaming_width:0")
log.match("[pci_dev.pci_device.pci_bus_socket trace-b-out] write sz:4 addr:0x168 data:0x00000000 streaming_width:0")
log.match("[pci_dev.PciMappingInterconnect.pci_bus_target_socket trace-b-in] write sz:4 addr:0x168 data:0x00000000 streaming_width:0")
log.match("[pci_dev.PciMappingInterconnect.pci_bus_initiator_socket trace-b-out] write sz:4 addr:0x168 data:0x00000000 streaming_width:0")
log.match("[pci_dev.gasket_PciMappingInterconnect_pci_bus_initiator_socket.target_socket trace-b-in] write sz:4 addr:0x168 data:0x00000000 streaming_width:0")
log.match("[pci_dev.gasket_PciMappingInterconnect_pci_bus_initiator_socket.target_socket trace-b-out] write sz:4 addr:0x168 data:0x00000000 status:TLM_OK_RESPONSE streaming_width:0")
log.match("[pci_dev.PciMappingInterconnect.pci_bus_initiator_socket trace-b-in] write sz:4 addr:0x168 data:0x00000000 status:TLM_OK_RESPONSE streaming_width:0")
log.match("[pci_dev.PciMappingInterconnect.pci_bus_target_socket trace-b-out] write sz:4 addr:0x168 data:0x00000000 status:TLM_OK_RESPONSE streaming_width:0")
log.match("[pci_dev.pci_device.pci_bus_socket trace-b-in] write sz:4 addr:0x168 data:0x00000000 status:TLM_OK_RESPONSE streaming_width:0")
log.is_empty()

run_command('pci_dev.untrace-sc-socket-all')
log.reset()
device.iface.pci_device.bus_reset()
SIM_continue(1)
log.is_empty()

# SC signal port
run_command('pci_dev.trace-sc-signal-all')
log.reset()

device.gasket_pci_device_dma_button.output_pin.iface.sc_signal_write.write(False)
SIM_continue(1)
log.match("[pci_dev.gasket_pci_device_dma_button.output_pin trace-signal] 1 -> 0")
log.match("[pci_dev.pci_device.dma_button trace-signal-port] 1 -> 0")
log.is_empty()

run_command('pci_dev.untrace-sc-signal-all')
log.reset()

device.gasket_pci_device_dma_button.output_pin.iface.sc_signal_write.write(True)
SIM_continue(1)
log.is_empty()
