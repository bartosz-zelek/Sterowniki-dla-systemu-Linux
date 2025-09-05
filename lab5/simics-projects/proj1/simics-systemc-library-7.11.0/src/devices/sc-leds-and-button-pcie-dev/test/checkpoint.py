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


'''Checkpoint utility to save and restore a checkpoint. It also included
simulation state check with/without the checkpoint.'''

from config import du
from stest import scratch_file, expect_equal, expect_true

import os
import shutil
import simics

def save_checkpoint(number):
    '''Save a checkpoint with number suffix, it will overwrite
    previous saved checkpoint'''
    ckpt_name = 'sc-leds-and-button-pcie-dev-state%d' % number
    ckpt = scratch_file(ckpt_name)
    if os.path.exists(ckpt):
        shutil.rmtree(ckpt)
    simics.SIM_write_configuration_to_file(ckpt, 0)

def load_checkpoint(number):
    ckpt_name = 'sc-leds-and-button-pcie-dev-state%d' % number
    du.Dev([du.Signal], name = "system_onoff_led", create_sim_obj=False)
    simics.SIM_read_configuration(scratch_file(ckpt_name))

def run_next_ckpt_test(number):
    simics.SIM_run_command("restart-simics read-configuration%d.py" % number)

def check_state(number):
    '''Check the state corresponding to the checkpoint number'''
    print("checking state", number)
    device = simics.SIM_get_object('pci_dev')
    bar0 = du.Register_LE((device, 255, 0x10), size = 4)
    dma_button_signal = device.pci_device.dma_button.iface.sc_signal_read
    dma_button_gasket_signal = device.gasket_pci_device_dma_button.output_pin.iface.sc_signal_read
    system_onoff_gasket_signal = device.gasket_pci_device_system_onoff_led_out.signal.iface.sc_signal_read
    if number == 1:
        # Initial state
        expect_equal(device.system_onoff_led_value, False)
        expect_equal(bar0.read(), 0)
        expect_equal(dma_button_signal.read(), False)
        expect_equal(dma_button_gasket_signal.read(), False)
    elif number == 2:
        expect_equal(device.system_onoff_led_value, True)
        expect_equal(bar0.read(), 0xfe000000)
        expect_equal(dma_button_signal.read(), True)
        expect_equal(dma_button_gasket_signal.read(), True)
    elif number == 3:
        expect_equal(device.system_onoff_led_value, False)
        expect_equal(bar0.read(), 0)
        expect_equal(dma_button_signal.read(), False)
        expect_equal(dma_button_gasket_signal.read(), False)
        expect_equal(system_onoff_gasket_signal.read(), False)
    elif number == 4:
        expect_equal(device.system_onoff_led_value, True)
        expect_equal(system_onoff_gasket_signal.read(), True)

    expect_equal(device.pci_device.dma.iface.sc_process.initialize(), False)
    expect_true(device.pci_device.mem_target_socket.iface.sc_export.if_typename() in ['N3tlm19tlm_fw_transport_ifINS_23tlm_base_protocol_typesEEE', 'class tlm::tlm_fw_transport_if<struct tlm::tlm_base_protocol_types>'])
