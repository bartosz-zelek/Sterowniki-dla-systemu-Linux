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

import simics
from blueprints import State, Builder, ConfObject, Namespace
from typing import NamedTuple

#:: pre target0 {{
def create_pci_blk_device0(name):
    top = simics.pre_conf_object(name, 'namespace')
    top.mem = simics.pre_conf_object('memory-space')
    top.bus = simics.pre_conf_object('pcie-downstream-port',
                                     upstream_target=top.mem)
    top.blk = simics.pre_conf_object('virtio_pcie_blk',
                                     upstream_target=top.bus)
    top.blk.image = simics.pre_conf_object('image', size=1024)
    top.blk.attr.image = top.blk.image
    top.bus.devices = [[1, 0, top.blk]]
    simics.SIM_add_configuration([top], None)
# }}

#:: pre target1 {{
def id_from_slot(slot):
    return [1, 2, 5, 8][slot]

def create_pci_blk_device1(name, size, pci_slot):
    top = simics.pre_conf_object(name, 'namespace')
    top.mem = simics.pre_conf_object('memory-space')
    top.bus = simics.pre_conf_object('pcie-downstream-port',
                                     upstream_target=top.mem)
    top.blk = simics.pre_conf_object('virtio_pcie_blk',
                                     upstream_target=top.bus)
    top.blk.image = simics.pre_conf_object('image', size=size)
    top.blk.attr.image = top.blk.image
    top.bus.devices = [[id_from_slot(pci_slot), 0, top.blk]]
    simics.SIM_add_configuration([top], None)
# }}

#:: pre target2 {{
# Data about a single PCIe device
class PCIEDevice(NamedTuple):
    device_id: int
    function_id: int
    dev: ConfObject

# State shared between PCIe bus and PCIe device blueprints
class PCIEBusData(State):
    bus: ConfObject = None
    devices: list[PCIEDevice] = []

# Blueprint creating a PCIe bus
def pcie_bus(builder: Builder, name: Namespace, data: PCIEBusData):
    # Register creation of PCIe bus
    data.bus = builder.obj(name, "pcie-downstream-port",
                           upstream_target=name.mem,
                           devices=data.devices)
    # Register creation of associated devices
    builder.obj(name.mem, "memory-space")

# Blueprint for PCIe device
def pcie_device(builder: Builder, name: Namespace, data: PCIEBusData,
                size=0, device_id=0):
    # Register creation of PCIe device
    builder.obj(name, "virtio_pcie_blk", upstream_target=data.bus)
    builder.set(name.attr, image=name.image)
    data.devices.extend([PCIEDevice(device_id, 0, ConfObject(name))])
    # Register creation of associated devices
    builder.obj(name.image, "image", size=size)

# Whole system state
class Fabric(State):
    pcie_bus = PCIEBusData()

# Blueprint for whole system
def system(builder: Builder, name: Namespace):
    # Create whole system state
    fabric = builder.expose_state(name, Fabric)
    # Expose PCIe bus state
    builder.expose_state(name, fabric.pcie_bus)
    # Expand PCIe bus blueprint in the PCIe object hierarchy
    builder.expand(name, "pci", pcie_bus)
    # Expand PCIe device blueprint
    builder.expand(name, "blk", pcie_device,
                   size=4096, device_id=id_from_slot(2))
# }}
