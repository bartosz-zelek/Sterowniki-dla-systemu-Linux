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


from simics import SIM_create_object
import dev_util as du
import conf

# SIMICS-21543
conf.sim.deprecation_level = 0

# make it possible to re-use the same bus for all devices
pci_bus = None

# Common configurations
def create_pci_bus(name = 'pci'):
    class pci:
        pass

    pci.conf = SIM_create_object('memory-space', name + '_conf')
    pci.io = SIM_create_object('memory-space', name + '_io')
    pci.mem = SIM_create_object('memory-space', name + '_mem')

    pci.bus = SIM_create_object('pcie-bus', name + '_bus',
                                [['conf_space', pci.conf],
                                 ['io_space', pci.io],
                                 ['memory_space', pci.mem]])
    return pci


def create_regs(device):
    class regs:
        class f5:
            pass

    command_bitfield = du.Bitfield_LE({'mem' : 1,
                                       'io'  : 0})

    # PCI only support DWORD (32-bit) access to configuration space
    regs.vendor_id = du.Register_LE((device, 255, 0x0), size = 2)
    regs.device_id = du.Register_LE((device, 255, 0x0), size = 4)
    regs.cmd = du.Register_LE((device, 255, 0x4), size = 4,
                              bitfield = command_bitfield)
    regs.bar0 = du.Register_LE((device, 255, 0x10), size = 4)
    regs.bar2 = du.Register_LE((device, 255, 0x18), size = 8)
    regs.bar5 = du.Register_LE((device, 255, 0x24), size = 4)
    # mmio mappings use a different mapping ID and can re-use same address
    id_offset = 0x10100
    regs.version = du.Register_LE((device, id_offset + 2, 0x0), size = 4)
    # PCI function F5
    regs.f5.offset = 5 << 8
    regs.f5.vendor_id = du.Register_LE((device, 255 + 5, 0x0), size = 2)
    regs.f5.device_id = du.Register_LE((device, 255 + 5, 0x0), size = 4)
    regs.f5.cmd = du.Register_LE((device, 255 + 5, 0x4), size = 4,
                                 bitfield = command_bitfield)
    regs.f5.bar0 = du.Register_LE((device, 255 + 5, 0x10), size = 4)
    regs.f5.bar2 = du.Register_LE((device, 255 + 5, 0x18), size = 8)
    regs.f5.bar5 = du.Register_LE((device, 255 + 5, 0x24), size = 4)
    regs.f5.version = du.Register_LE((device, id_offset + 2 + 3, 0x0), size = 4)
    return regs


def create_pci_device(name):
    global pci_bus
    if not pci_bus:
        pci_bus = create_pci_bus()

    device = SIM_create_object('sample_tlm2_pci_device', name,
                               [['pci_bus', pci_bus.bus]])

    regs = create_regs(device)
    return device, pci_bus.mem, regs, pci_bus
