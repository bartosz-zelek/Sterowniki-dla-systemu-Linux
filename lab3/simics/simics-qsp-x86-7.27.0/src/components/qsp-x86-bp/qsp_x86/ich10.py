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


import re
from blueprints import (
    Alias, Builder, ConfObject, MemMap, Namespace, Port, SignalPort)
from simmod.std_bp.state import PCIEDevice
from ..x86_cpu.state import CPUFabric
from .state import X58Backplane, ICH10Backplane, X58BackplaneConfig
import cli

def serial(bp: Builder, name: Namespace):
    sb = bp.read_state(name, ICH10Backplane)

    bp.obj(name, 'namespace')

    # Serial Ports
    for x in range(4):
        sb.lpc_uart[x].uart = bp.obj(name.com[x], "NS16550",
            irq_dev = sb.lpc_uart[x].irq_signal,
            console = sb.connectors.com[x].remote,
        )
        sb.connectors.com[x].uart = ConfObject(name.com[x])

    # This is not according to specs
    sb.io_port_map.extend([
        MemMap(base + i, name.com[d], 0, i, 1)
        for (d, base) in enumerate((0x3f8, 0x2f8, 0x3e8, 0x2e8))
        for i in range(8)
    ])

def floppy(bp: Builder, name: Namespace):
    sb = bp.read_state(name, ICH10Backplane)
    bp.obj(name, "i82077",
        irq_level = 6,
        irq_dev = sb.isa_irq_dev,
        dma_channel = 2,
        dma_dev = sb.isa_dma,
        drives = [name.fd0, name.fd1],
    )
    bp.obj(name.fd0, "floppy-drive")
    bp.obj(name.fd1, "floppy-drive")

    # TODO should be mapped by FFD Decode Range, 13.1.21 p399
    sb.io_port_map.extend([
        MemMap(0x3f2, name, 0, 0, 1),
        MemMap(0x3f4, name, 0, 2, 1),
        MemMap(0x3f5, name, 0, 3, 1),
        MemMap(0x3f7, name, 0, 5, 1),
    ])

def i8042(bp: Builder, name: Namespace):
    sb = bp.read_state(name, ICH10Backplane)

    bp.obj(name, "i8042",
        console = sb.connectors.gfx_console.console,
        kbd_irq_level = 1,
        mouse_irq_level = 12,
        irq_dev = sb.isa_irq_dev,
        reset_target = sb.i8042_reset,
    )
    sb.connectors.gfx_console.mouse = ConfObject(name)
    sb.connectors.gfx_console.keyboard = ConfObject(name)
    sb.io_port_map.extend([
        MemMap(0x60, name, 0, 0, 1),
        MemMap(0x64, name, 0, 4, 1),
    ])

def super_io(bp: Builder, name: Namespace):
    bp.obj(name, 'namespace')
    bp.expand(name, "kbd", i8042)
    bp.expand(name, "com", serial)
    bp.expand(name, "flp", floppy)


def lpc_bridge(bp: Builder, name: Namespace, slot: int):
    sb = bp.read_state(name, ICH10Backplane)
    fabric = bp.read_state(name, CPUFabric)
    config = bp.get_config(name, X58BackplaneConfig)

    bp.obj(name, 'namespace')

    # IOAPIC
    sb.io_apic_list.append(ConfObject(name.ioapic))
    sb.pci_bus.mem_map.append(
        MemMap(0xfec00000, name.ioapic, 0, 0, 0x20)
    )

    bp.obj(name.ioapic, "io-apic",
        ioapic_version = (0x17 << 16) | 0x20,
        ioapic_id = 8 << 24,
        apic_bus = fabric.apic_bus,
    )
    platform_reset = [SignalPort(name.ioapic, "RESET")]

    # PIC (i8259x2)
    pic_io_map = [
        MemMap(p, name.pic, 0, offset, 1)
        for (offset, ports) in (
            (0x20, (0x20, 0x24, 0x28, 0x2C, 0x30, 0x34, 0x38, 0x3C)),
            (0x21, (0x21, 0x25, 0x29, 0x2D, 0x31, 0x35, 0x39, 0x3D)),
            (0xA0, (0xA0, 0xA4, 0xA8, 0xAC, 0xB0, 0xB4, 0xB8, 0xBC)),
            (0xA1, (0xA1, 0xA5, 0xA9, 0xAD, 0xB1, 0xB5, 0xB9, 0xBD)))
        for p in ports
    ]
    pic_io_map += [
        MemMap(0x4d0 + i, name.pic, 0, 0x4d0 + i, 1)
        for i in [0, 1]
    ]
    sb.io_port_map.extend(pic_io_map)
    sb.pic_8259 = bp.obj(name.pic, "i8259x2", irq_dev = fabric.apic_bus)
    platform_reset += [SignalPort(name.pic, "RESET")]

    # ISA APIC/pic routing
    sb.isa_irq_dev = bp.obj(name.isa, "ISA",
        pic = name.pic,
        ioapic = name.ioapic,
        irq_to_pin = [
            2,   1,  0,  3,  4,  5,  6,  7,  8,  9,
            10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
            20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
            30, 31],
    )

    # LPC
    bp.obj(name.lpc.io, "memory-space")
    bp.obj(name.lpc.mem, "memory-space")
    sb.lpc_obj = bp.obj(name.lpc, "ich10_lpc",
        coma_level = 3,
        comb_level = 4,
        coma_pirq = name.isa,
        comb_pirq = name.isa,
        irq_dev = name.pic,
        ioapic = name.ioapic,
        flash = sb.spi_flash_bank,
        cpus = fabric.cpu_list,
        pci_bus = sb.pci_bus.bus,
        lpc_io = name.lpc.io,
        lpc_memory = name.lpc.mem,
        hpet = name.hpet,
        serial_port = [sb.lpc_uart[i].uart for i in range(4)],
    )
    sb.io_port_map.extend([
        MemMap(0xb2, name.lpc, 2, 0, 1),
        MemMap(0xb3, name.lpc, 2, 1, 1),
    ])
    sb.lpc_chipset_config_bank = Port(name.lpc, "cs_conf")

    sb.pci_bus.devices.append(PCIEDevice(slot, 0, name.lpc))

    # Devices on the LPC bus
    # This is *not* according to the specs
    for i in range(4):
        sb.lpc_uart[i].irq_signal = SignalPort(name.lpc, f"com{i + 1}_in")

    # HPET (high-precision event timer)
    bp.obj(name.hpet, "ich10_hpe_timer",
        intc_8259 = name.pic,
        intc_apic = name.ioapic,
        comp_write_slow_cycles = 200,
        lpc = name.lpc,
    )

    platform_reset += [SignalPort(name.hpet, "SRESET")]

    # DMA
    sb.isa_dma = bp.obj(name.dma, "i8237x2", memory = sb.pci_bus.mem)

    dma_io_map = []
    for i in range(0x00, 0x10):
        dma_io_map += [[i + 0x00, name.dma, 0, i, 1]]
        dma_io_map += [[i + 0x10, name.dma, 0, i, 1]]
    for i in range(0x81, 0x90):
        dma_io_map += [[i + 0x00, name.dma, 0, i, 1]]
        if i != 0x82: # Port 0x92 is used to enable A20 pin
            dma_io_map += [[i + 0x10, name.dma, 0, i, 1]]
    for i in range(0xc0, 0xe0, 2):
        dma_io_map += [[i + 0x0, name.dma, 0, i, 1]]
        dma_io_map += [[i + 0x1, name.dma, 0, i, 1]]
    sb.io_port_map.extend(dma_io_map)

    # i8254 Timer
    bp.obj(name.timer, "i8254",
        irq_dev = name.hpet,
        irq_level = 0,
        out = [1, 1, 1]
    )
    timer_io_map = []
    for i in range(4):
        timer_io_map += [[0x40 + i, name.timer, 0, i, 1]]
        timer_io_map += [[0x50 + i, name.timer, 0, i, 1]]
    timer_io_map += [[0x61, name.timer, 1, 0, 1]] #TODO remove?
    sb.io_port_map.extend(timer_io_map)
    platform_reset += [SignalPort(name.timer, "RESET")]


    # RTC Timer
    # NOTE:
    #   U128E always is 0 for chipset configuration is dummy
    #   RTC is mapped to fixed IO space (0x70 ~ 0x73) and (0x74 ~ 0x77)
    sb.rtc = bp.obj(name.rtc, "ich10_rtc",
          time = config.rtc_time,
          rtc_reg_a = 0x20,
          rtc_reg_b = 0x02,
          irq_level = 8,
          irq_dev = name.hpet,
    )
    rtc_io_map = []
    for (offset, ports) in ((0x0, (0x70, 0x74, 0x72, 0x76)),
                            (0x1, (0x71, 0x75, 0x73, 0x77))):
        for p in ports:
            rtc_io_map += [[p, name.rtc, 0, offset, 1]]
    sb.io_port_map.extend(rtc_io_map)

    # Super IO (floppy, kbd, serial ports)
    bp.expand(name, "super_io", super_io)

    sb.PLTRST_tgts.extend(platform_reset)


def sata(bp: Builder, name: Namespace, slot: int):
    sb = bp.read_state(name, ICH10Backplane)

    bp.obj(name, 'namespace')

    # SATA connectors
    sata_device = [ConfObject()] * 32
    for i in range(6):
        sb.connectors.sata[i].controller = Port(name.sata2, f"sata_ctr_p{i}")
        sata_device[i] = sb.connectors.sata[i].device

    def ssts(v):
        return 0x113 if v else 0
    bp.obj(name.sata2, "ich10_sata_f2",
        sata_device = sata_device,
        chipset_config = sb.lpc_chipset_config_bank,
        ide = [name.ide[0], name.ide[1]],
        pci_bus = sb.pci_bus.bus,
        ahci_p0pcr_sata_ssts = ssts(sb.connectors.ide[0].master),
        ahci_p2pcr_sata_ssts = ssts(sb.connectors.ide[0].slave),
        ahci_p1pcr_sata_ssts = ssts(sb.connectors.ide[1].master),
        ahci_p3pcr_sata_ssts = ssts(sb.connectors.ide[1].slave),
    )
    bp.obj(name.sata5, "ich10_sata_f5",
        chipset_config = sb.lpc_chipset_config_bank,
        ide = [name.ide[2], name.ide[3]],
        pci_bus = sb.pci_bus.bus,
        sata_ch_ssts = [
            ssts(sb.connectors.ide[2].master),
            ssts(sb.connectors.ide[3].master),
        ],
    )
    sb.connectors.ide[2].slave = None
    sb.connectors.ide[3].slave = None

    sb.pci_bus.devices.extend([
        PCIEDevice(slot, 2, name.sata2),
        PCIEDevice(slot, 5, name.sata5),
    ])

    irq_level = [14, 15, 0, 0]
    primary = [1, 0, 1, 0]
    dma = [name.sata2, name.sata2, name.sata5, name.sata5]
    for i in range(4):
        bp.obj(name.ide[i], "ide",
            irq_level = irq_level[i],
            master = sb.connectors.ide[i].master,
            slave = sb.connectors.ide[i].slave if i < 2 else None,
            primary = primary[i],
            bus_master_dma = dma[i],
        )

    # Primary
    io_map = [MemMap(0x1f0, name.ide[0], 0, 0x0, 4)] # ???4
    for i in range(1, 8):
        io_map += [MemMap(0x1f0 + i, name.ide[0], 0, i, 1)]
    io_map += [MemMap(0x3f6, name.ide[0], 0, 8, 1)]
    # Secondary
    io_map += [MemMap(0x170, name.ide[1], 0, 0x0, 4)] # ???4
    for i in range(1, 8):
        io_map += [MemMap(0x170 + i, name.ide[1], 0, i, 1)]
    io_map += [MemMap(0x376, name.ide[1], 0, 8, 1)]
    sb.io_port_map.extend(io_map)



def lan(bp: Builder, name: Namespace, slot: int):
    sb = bp.read_state(name, ICH10Backplane)
    config = bp.get_config(name, X58BackplaneConfig)

    bp.obj(name, "ich10_lan_v2",
        pci_bus = sb.pci_bus.bus,
        phy = name.phy,
        mii = name.phy,
        phy_address = 1,
        mac_address = config.mac_address,
        flash = sb.lan_flash_bank,
        flash_func = sb.lan_flash_func,
        chipset_config = sb.lpc_chipset_config_bank,
    )
    sb.connectors.eth0.local = bp.obj(name.phy, "generic_eth_phy",
        mac = name,
        registers = [0, 0, 0x02A8, 0x0390] + [0] * 28,
        tx_bandwidth = 1000000000, # 1Gb/s
        address = 1,
        link = sb.connectors.eth0.remote,
    )
    sb.pci_bus.devices.append(PCIEDevice(slot, 0, name))

    # Lan rom not handled


def pci_bridge(bp: Builder, name: Namespace):
    sb = bp.read_state(name, X58Backplane)

    sb.pci_bus.devices.append(PCIEDevice(30, 0, name))
    sb.PLTRST_tgts.append(SignalPort(name, "pltrst"))

    # DMI-to-PCI bridge, and three external PCI slots
    bp.obj(name, "ich10_bridge",
        pci_bus = sb.pci_bus.bus,
        secondary_bus = name.pci,
        upstream_target = sb.pci_bus.bus,
    )
    bp.obj(name.pci, "pci-bus",
        pci_devices = [],
        conf_space = name.pci.conf,
        memory_space = name.pci.mem,
        io_space = name.pci.io,
        bridge = name,
        upstream_target = name,
    )
    bp.obj(name.pci.conf, "memory-space")
    bp.obj(name.pci.mem, "memory-space")
    bp.obj(name.pci.io, "memory-space")


def usb(bp: Builder, name: Namespace):
    sb = bp.read_state(name, ICH10Backplane)

    sb.connectors.usb0.host = ConfObject(name.ehci[0])
    sb.connectors.usb1.host = ConfObject(name.ehci[1])

    bp.obj(name, 'namespace')

    # UHCI
    for i in range(6):
        dev_num = [[29, 0], [29, 1], [29, 2], [26, 0], [26, 1], [26, 2]]
        bp.obj(name.uhci[i], "ich10_usb_uhci",
            pci_bus = sb.pci_bus.bus,
            chipset_config = sb.lpc_chipset_config_bank,
            pci_dev_num = dev_num[i][0],
            pcidev_func_num = dev_num[i][1],
            frame_list_polling_enabled = True,
        )

    # EHCI
    for i in range(2):
        dev_num = [[29, 7], [26, 7]]
        bp.obj(name.ehci[i], "ich10_usb_ehci",
              pci_bus = sb.pci_bus.bus,
              chipset_config = sb.lpc_chipset_config_bank,
              pci_dev_num = dev_num[i][0],
              pcidev_func_num = dev_num[i][1],
              async_list_polling_enabled = True,
              periodic_list_polling_enabled = True,
              companion_hc = [
                  name.uhci[i * 3 + 0], name.uhci[i * 3 + 0],
                  name.uhci[i * 3 + 1], name.uhci[i * 3 + 1],
                  name.uhci[i * 3 + 2], name.uhci[i * 3 + 2],
              ],
        )

    sb.pci_bus.devices.extend([
        PCIEDevice(29, 0, name.uhci[0]),
        PCIEDevice(29, 1, name.uhci[1]),
        PCIEDevice(29, 2, name.uhci[2]),
        PCIEDevice(29, 7, name.ehci[0]),
        PCIEDevice(26, 0, name.uhci[3]),
        PCIEDevice(26, 1, name.uhci[4]),
        PCIEDevice(26, 2, name.uhci[5]),
        PCIEDevice(26, 7, name.ehci[1]),
    ])

def x58_ich10(bp: Builder, name: Namespace):
    """ICH10 blueprint with connections wiring to the X58 model."""
    sb = bp.read_state(name, ICH10Backplane)
    plane = bp.read_state(name, X58Backplane)
    bp.expose_state(name, plane.fabric)

    # Add the ich10 blueprint
    bp.expand(name, "", ich10)

    sb.i8042_reset = Alias(plane).i8042_reset
    sb.cpu_INIT = Alias(plane).cpu_INIT

    # Values from X58 -> ICH10
    sb.motherboard = Alias(plane).motherboard
    sb.PLTRST_tgts = Alias(plane).PLTRST_tgts

    # Shared state
    sb.pci_bus = plane.pci_bus
    sb.connectors = plane.connectors

    # Values from ICH10 -> X58
    plane.io_apic_list = Alias(sb).io_apic_list
    plane.pic_8259 = Alias(sb).pic_8259
    plane.rtc = Alias(sb).rtc
    plane.lpc_obj = Alias(sb).lpc_obj
    plane.sb_pci_io = Alias(sb).pci_io    # not 1-1

    # cf9 is put in the top-most IO-space (unclear why)
    plane.system_io_port_map.extend(sb.cf9_io_map)

def ich10_finalize(motherboard, rtc, rtc_time,
                   memory_megs, ide0_master):
    mb_name = motherboard.name

    # set nvram info
    cli.run_command('%s.cmos-init' % mb_name)
    cli.run_command('%s.cmos-base-mem 640' % mb_name)
    cli.run_command('%s.cmos-extended-mem %d' % (mb_name, memory_megs - 1))
    m = re.match(r'(\d+)-(\d+)-(\d+) (\d+):(\d+):(\d+)', rtc_time)
    cli.run_command(('%s.set-date-time '
                     + 'year=%s month=%s mday=%s '
                     + 'hour=%s minute=%s second=%s')
                    % ((rtc.name,) + m.groups()))
    cli.run_command('%s.cmos-boot-dev C' % mb_name)
    #flp = obj.flp
    flp = None
    if flp:
        if len(flp.drives):
            cli.run_command('%s.cmos-floppy A "1.44"' % mb_name)
        if len(flp.drives) > 1:
            cli.run_command('%s.cmos-floppy B "1.44"' % mb_name)
    if ide0_master:
        size = ide0_master.disk_sectors
        # Our BIOS does LBA directly: set sectors 63 and heads to x * 16
        bios_S = 63
        # The following would probably work if our BIOS had support
        # for proper 'translation'. Now it seems to fail.
        #if size < 504 * 1024 * 1024 / 512:
        #    bios_H = 16
        #elif size < 1008 * 1024 * 1024 / 512:
        #    bios_H = 32
        #elif size < 2016 * 1024 * 1024 / 512:
        #    bios_H = 64
        #elif size < 4032 * 1024 * 1024 / 512:
        #    bios_H = 128
        if size < 4032 * 1024 * 1024 // 512:
            bios_H = 16
        else:
            # 255 is de facto standard since DOS and early Windows can't
            # handle 256 heads, this is known as the 4GB limit
            bios_H = 255
        bios_C = size // (bios_H * bios_S)
        #if bios_C * bios_H * bios_S != size:
        #    print 'Disk size can not be translated to exact BIOS CHS'
        #    print 'Using CHS: %d %d %d' % (bios_C, bios_H, bios_S)
        cli.run_command('%s.cmos-hd C %d %d %d' % (mb_name,
                                                   bios_C, bios_H, bios_S))

def ich10(bp: Builder, name: Namespace):
    "ICH10 blueprint."
    sb = bp.expose_state(name, ICH10Backplane)
    config = bp.get_config(name, X58BackplaneConfig)

    bp.obj(name, 'namespace')

    sb.pci_bus.pcie_devices.extend([
        PCIEDevice(31, 3, name.smbus),
    ])

    sb.pci_bus.devices.extend([
        PCIEDevice(31, 6, name.thermal),
    ])

    sb.cf9_io_map = [MemMap(0xcf9, name.cf9, 0, 0x0, 1)]

    # SPI flash bank
    sb.spi_flash_bank = Port(name.spi, "spi_regs")

    # LAN flash bank
    sb.lan_flash_bank = Port(name.spi, "gbe_regs")
    sb.lan_flash_func = 1

    bp.obj(name.pltrst, "signal-bus", targets = sb.PLTRST_tgts)
    bp.obj(name.cf9, "ich10_cf9",
          reset_signal = name.pltrst,
          init_signal = sb.cpu_INIT,
    )
    sb.pci_io = bp.obj(name.io, "port-space", map = sb.io_port_map)

    # SPI flash
    bp.obj(name.spi, "ich10_spi", spi_slave = name.spi.flash)
    bp.obj(name.spi.flash, "M25Pxx",
        sector_number = (1 << 21) >> 16,
        sector_size = 1 << 16,
        spi_master = name.spi,
        mem_block = name.spi.flash.image,
    )
    bp.obj(name.spi.flash.image, "image",
        size = 1 << 21,
        files = [[config.spi_flash_image, 'ro', 0, 0]],
    )


    # SMBus
    bp.obj(name.smbus, "ich10_smbus",
        smbus = name.smbus.i2c_link,
        sram_image = name.smbus.sram,
    )
    bp.obj(name.smbus.i2c_link, "i2c_link")
    bp.obj(name.smbus.sram, "image", size = 32)

    # Thermal
    bp.obj(name.thermal, "ich10_thermal", pci_bus = sb.pci_bus.bus)

    # Initialization
    bp.at_post_instantiate(name, ich10_finalize,
        motherboard=sb.motherboard,
        rtc=sb.rtc,
        rtc_time=config.rtc_time,
        memory_megs=config.memory_size_mb,
        ide0_master=sb.connectors.ide[0].master)

    bp.expand(name, "lpc", lpc_bridge, slot = 31)
    bp.expand(name, "sata", sata, slot = 31)
    bp.expand(name, "lan", lan, slot = 25)
    bp.expand(name, "usb", usb)

    # DMI-to-PCI bridge
    bp.expand(name, "bridge", pci_bridge)
