# © 2013 Intel Corporation
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

#
# sample PCI Device
#

device_class_name = "sample_tlm2_pci_device"
port_class_names = [device_class_name + ".f0",
                    device_class_name + ".f5"]

#
# ------------------------ info -----------------------
#

def get_info(obj):
    return [(None,
             [("Connect to PCI Bus", obj.pci_bus)])]

cli.new_info_command(device_class_name, get_info)
for name in port_class_names:
    cli.new_info_command(name, lambda x: [])

#
# ------------------------ status -----------------------
#

def get_pci_status(obj):
    status = [(None,
               [ ("Vendor Id" , cli.number_str(obj.vendor_id)),
                 ("Device Id" , cli.number_str(obj.device_id)),
                 ("Command" , cli.number_str(obj.command)),
                 ("Status" , cli.number_str(obj.status)),
                 ("Base Address 0" , cli.number_str(obj.base_address_0)),
                 ("Base Address 2" , cli.number_str(obj.base_address_2)),
                 ("Base Address 5" , cli.number_str(obj.base_address_5)),
                 ("Version" , cli.number_str(obj.version))])]
    status += utility.get_status(obj)
    return status

cli.new_status_command(device_class_name, get_pci_status)
for name in port_class_names:
    cli.new_status_command(name, lambda x: [])

# Gasket objects are somewhat strange and does not have a good info
# command. The status command works on the "simulation" object (class
# _external) but it's unclear if anyone would figure this out.
gasket_object_class_name = "sample_tlm2_pci_device_external"
cli.new_info_command(gasket_object_class_name, lambda x: [])
cli.new_status_command(gasket_object_class_name, get_pci_status)
