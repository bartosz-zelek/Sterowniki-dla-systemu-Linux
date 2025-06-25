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


import cli
from . import utility
import systempanel
import systempanel.widgets as w
from comp import StandardConnectorComponent, PciBusUpConnector

device_class_name = "sc_leds_and_button_pcie_device"
port_class_name = device_class_name + ".port"

def get_info(obj):
    # Info: static information about the object
    return [(None,
             [("Attached to processor", obj.queue),
              ("Connect to PCI Bus", obj.pci_bus)])]

cli.new_info_command(device_class_name, get_info)
cli.new_info_command(port_class_name, lambda x: [])

def get_pci_status(obj):
    status = [(None,
               [ ("Vendor Id" , cli.number_str(obj.vendor_id)),
                 ("Device Id" , cli.number_str(obj.device_id)),
                 ("Command" , cli.number_str(obj.command)),
                 ("Status" , cli.number_str(obj.status)),
                 ("Base Address 0" , cli.number_str(obj.base_address_0)),
                 ("Base Address 2" , cli.number_str(obj.base_address_2)),
                 ("Version" , cli.number_str(obj.version))])]
    status += utility.get_status(obj)
    return status

cli.new_status_command(device_class_name, get_pci_status)
cli.new_status_command(port_class_name, lambda x: [])

## Component for leds-and-button example device set up
##
## The component wraps the PCIe device that actually does the job, and it also
## creates the system panel that is used for the user interface. And connects
## the device to the system panel.
##
## Its PCIe connector supports hot-plugging this subsystem into the virtual host
## system.
class sc_leds_and_button_pcie_comp(StandardConnectorComponent):
    """PCIe card with a set of LEDs and buttons on its panel. Used for
    PCIe-based modeling tutorial, and as an example of system panel usage"""
    _class_desc = "sample PCIe card with SC leds and buttons"
    _help_categories = ()

    def setup(self):
        super().setup()
        if not self.instantiated.val:
            self.add_objects()
        self.add_connector('pci', PciBusUpConnector(0, 'device'))

    def add_objects(self):
        # Create the actual PCI device object that does the job at run time
        pcie_dev = self.add_pre_obj('device', 'sc_leds_and_button_pcie_device')
        # Create the system panel as a subcomponent
        self.add_component('panel', 'leds_and_button_panel', [])
        # Connect the device and the panel
        pcie_dev.pin_out_0 = self.get_slot("panel.red_out")
        pcie_dev.pin_out_1 = self.get_slot("panel.yellow_out")
        pcie_dev.pin_out_2 = self.get_slot("panel.green_out")
        pcie_dev.pin_out_3 = self.get_slot("panel.blue_out")
        pcie_dev.system_onoff_led = self.get_slot("panel.led_onoff")
        self.get_slot("panel.button_dma").target = pcie_dev.port.dma

#----------------------------------------------------------------------
#
# LED and buttons panel
# - Red
# - Yellow
# - Green
# - Blue LEDs
# - Onoff
# - Activity LEDs
# - DMA button
#
#----------------------------------------------------------------------

class leds_and_button_panel(systempanel.SystemPanel):
    """The system panel to display the front end of the LEDs and button device."""
    _class_desc = "model of system panel"
    # Abstract layout
    default_layout = w.Row([
        w.LabeledBox('PCIe demo panel',
                     w.Row([
                         w.Canvas([
                             ( 0, 0,w.Image('led-background-plate.png')),
                             ( 6, 6,w.BitmapLed('red_out','red-off-32.png','red-on-32.png')),
                             (44, 6,w.BitmapLed('yellow_out','yellow-off-32.png','yellow-on-32.png')),
                             ( 6,44,w.BitmapLed('green_out','green-off-32.png','green-on-32.png')),
                             (44,44,w.BitmapLed('blue_out', 'blue-off-32.png','blue-on-32.png'))
                         ]),
                         w.Column([w.Button('button_dma',"DMA")]),
                         w.Grid(columns=2, contents=[w.Led('led_onoff'),  w.Label('power')]),
                     ])),
        ])
    objects = default_layout.objects()

sc_leds_and_button_pcie_comp.register()
leds_and_button_panel.register()
