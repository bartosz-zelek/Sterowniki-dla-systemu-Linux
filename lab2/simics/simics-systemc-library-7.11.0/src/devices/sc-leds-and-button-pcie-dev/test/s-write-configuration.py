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


# Feature: Simics supports save the current SystemC simulation state
# and later restore from there and continue the simulation
# Test we can save and restore the state correctly

from config import create_pci_device
from checkpoint import save_checkpoint, check_state, run_next_ckpt_test

(device, _, regs, _) = create_pci_device("pci_dev")

check_state(1)
save_checkpoint(1)

# Check checkpoint of attribute, register and simics2systemc signal
device.system_onoff_led_value = True
regs.bar0.write(0xfe000000)
device.gasket_pci_device_dma_button.output_pin.iface.sc_signal_write.write(True)
SIM_continue(1)

check_state(2)
save_checkpoint(2)

# Check checkpoint of event, register and systemc2simics signal
device.pci_device.blink_onoff_event.iface.sc_event.notify()
regs.bar0.write(0x0)
device.gasket_pci_device_dma_button.output_pin.iface.sc_signal_write.write(False)

check_state(3)
save_checkpoint(3)

# Check checkpoint of event
simics.SIM_continue(5000001)
check_state(4)
save_checkpoint(4)

run_next_ckpt_test(1)
