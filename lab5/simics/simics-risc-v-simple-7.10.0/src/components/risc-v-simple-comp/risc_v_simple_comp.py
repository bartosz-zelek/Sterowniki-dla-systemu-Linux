# © 2021 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


from datetime import datetime

import simics
from comp import (SerialDownConnector, SimpleConfigAttribute,
                  StandardConnectorComponent, EthernetLinkDownConnector)
from component_utils import ComponentError

KB = 0x400
MB = KB * KB
GB = MB * KB
TB = GB * KB

def create_disk(board, num, irqchip, phys_mem):
    file = getattr(board, f"disk{num}_image").val
    size = getattr(board, f"disk{num}_size").val

    if file == "" and size == 0:
        return None

    board.add_component(f'disk{num}', 'virtio_mmio_blk_comp',
        [['size', size], ['file', file]])

    virtio = board.get_slot(f'disk{num}.virtio_disk')
    virtio.irq = irqchip.port.IRQ[3 + num]
    virtio.phys_mem = phys_mem

    return virtio

def try_create_disks(board, irqchip, phys_mem):
    devs = []
    for i in range(3):
        devs.append(create_disk(board, i, irqchip, phys_mem))

    return devs


def create_virtio_net(board, irqchip, phys_mem):
    virtio = board.add_pre_obj("virtio_net", "virtio_mmio_net")
    virtio.irq = irqchip.port.IRQ[2]
    virtio.phys_mem = phys_mem
    virtio.mac_address = board.mac_address.val
    return virtio


class risc_v_simple_base(StandardConnectorComponent):
    _do_not_init = object()

    class cpu_list(StandardConnectorComponent.cpu_list):
        def getter(self):
            return self._up.get_slot('hart')

    class top_level(StandardConnectorComponent.top_level):
        def _initialize(self):
            self.val = True

    class basename(StandardConnectorComponent.basename):
        val = 'board'

    class core_class(SimpleConfigAttribute('riscv-rv64', 's')):
        """Classname of primary core, default is riscv-rv64"""

    class num_cores(SimpleConfigAttribute(4, 'i')):
        """Number cores, default is 4"""

    class freq_mhz(SimpleConfigAttribute(100.0, 'f')):
        """Frequency in MHz of each core, default is 100 MHz"""

    class timebase_freq_mhz(SimpleConfigAttribute(1.0, 'f')):
        """Frequency in MHz of the timebase, i.e. 'mtime', default is 1MHz"""

    class memory_megs(SimpleConfigAttribute(2048, 'i')):
        """Size in MB of RAM memory, default is 2048"""


    class mac_address(SimpleConfigAttribute('01:02:03:04:05:06', 's')):
        """The MAC address of the virtual Ethernet card."""

    class disk0_image(SimpleConfigAttribute("", "s")):
        """disk0 image file."""

    class disk0_size(SimpleConfigAttribute(TB, "i")):
        """disk0 size, used if no disk image is provided.
           If set to zero no disk will be created."""

    class disk1_image(SimpleConfigAttribute("", "s")):
        """disk1 image file."""

    class disk1_size(SimpleConfigAttribute(TB, "i")):
        """disk1 size, used if no disk image is provided.
           If set to zero no disk will be created."""

    class disk2_image(SimpleConfigAttribute("", "s")):
        """disk2 image file."""

    class disk2_size(SimpleConfigAttribute(TB, "i")):
        """disk2 size, used if no disk image is provided.
           If set to zero no disk will be created."""

    class entropy_seed(SimpleConfigAttribute(0x2345, 'i')):
        """Seed for virtio entropy device."""

    class rtc_time(SimpleConfigAttribute(None, 's', attr = simics.Sim_Attr_Required)):
        """The date and time of the Real-Time clock in ISO 8601-like format.

        For the full format specification see the Python documentation for the
        classmethod datetime.fromisoformat in module datetime.
        """

    def setup(self):
        StandardConnectorComponent.setup(self)
        if not self.instantiated.val:
            self.add_objects()
        self.add_connectors()

    def add_objects(self):
        phys_mem = self.add_pre_obj('phys_mem', 'memory-space')

        clint = self.add_pre_obj('clint', 'riscv-clint',
                                 freq_mhz=self.timebase_freq_mhz.val)

        hart = []
        hart += [self.add_pre_obj(None, self.core_class.val,
                                  freq_mhz=self.freq_mhz.val,
                                  physical_memory=phys_mem,
                                  clint=clint)
                 for _ in range(self.num_cores.val)]
        self.add_slot('hart', hart)

        for (i, h) in enumerate(hart):
            h.mhartid = i

        clint.hart = hart

        irqchip = self.add_irqchip()

        boot = self.add_pre_obj('boot', 'ram')
        boot_image = self.add_pre_obj('boot.image', 'image', size=0x40000)
        boot.attr.image = boot_image

        ram = self.add_pre_obj('ram', 'ram')
        ram_image = self.add_pre_obj('ram.image', 'image', size = self.memory_megs.val * MB)
        ram.attr.image = ram_image

        uart = self.add_pre_obj('uart0', 'NS16550')
        uart.irq_dev = irqchip.port.IRQ[1]

        virtio_entropy = self.add_pre_obj('virtio_entropy', 'virtio-mmio-entropy',
                                          seed = self.entropy_seed.val,
                                          irq = irqchip.port.IRQ[6],
                                          phys_mem = phys_mem)

        rtc_time = int(datetime.fromisoformat(self.rtc_time.val).timestamp())
        goldfish_rtc = self.add_pre_obj('goldfish_rtc', 'goldfish-rtc',
                                        initial_time = rtc_time,
                                        irq_dev = irqchip.port.IRQ[7])

        phys_mem.map = [
            [0x00001000, boot,            0, 0, boot.image.attr.size],
            [0x02000000, clint.bank.regs, 0, 0,          0xc000],
            [0x10000000, uart.bank.regs,  0, 0,            0x11],
            [0x10080000, virtio_entropy.bank.mmio, 0, 0,    64 * KB],
            [0x10090000, goldfish_rtc.bank.regs, 0, 0, 0x20],
            [0x80000000, ram,             0, 0,  ram.image.size]
        ]
        self.map_irqchip(phys_mem)

    def add_connectors(self):
        self.add_connector('serial0', SerialDownConnector('uart0'))


class risc_v_simple_comp(risc_v_simple_base):
    """RISC-V Simple Board Component"""
    _class_desc = "simple RISC-V board"
    _do_not_init = object()

    def add_objects(self):
        risc_v_simple_base.add_objects(self)

        phys_mem = self.get_slot('phys_mem')
        irqchip = self.get_irqchip()

        virtio_net = create_virtio_net(self, irqchip, phys_mem)
        virtio_blk_devs = try_create_disks(self, irqchip, phys_mem)

        phys_mem.map += [
            [0x10010000, virtio_net.bank.mmio, 0, 0,    64 * KB],
        ]

        for i, dev in enumerate(virtio_blk_devs):
            if dev:
                phys_mem.map += [[0x10020000 + (i * 0x10000), dev.bank.mmio, 0, 0, 64 * KB]]

    def get_irqchip(self):
        return self.get_slot('plic')

    def map_irqchip(self, phys_mem):
        phys_mem.map += [[0x0c000000, self.get_slot('plic').bank.regs,  0, 0, 0x4000000]]

    def add_irqchip(self):
        plic = self.add_pre_obj('plic', 'riscv-plic')
        plic.max_interrupt = 127
        plic.max_priority = 7
        plic.hart = self.get_slot('hart')

        return self.get_irqchip()

    def add_connectors(self):
        risc_v_simple_base.add_connectors(self)
        self.add_connector('eth', EthernetLinkDownConnector('virtio_net.port.eth'))
