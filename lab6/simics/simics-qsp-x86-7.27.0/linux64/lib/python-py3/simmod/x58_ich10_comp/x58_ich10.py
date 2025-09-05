# © 2010 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


# TODO fix
from simmod.x86_comp.x86_comp_cmos import register_cmos_commands
from simmod.x86_comp.x86_comp import x86_chassis
from simmod.x86_comp.x86_connector import (
    X86ApicProcessorDownConnector, X86ResetBusDownConnector,
    I2CLinkV1DownConnector, MouseDownConnector, KeyboardDownConnector,
    X86LpcBusDownConnector)
from simics import (Sim_X86_Msr_Attribute_Access, Sim_X86_Msr_Ok, SIM_log_info,
                    SIM_get_all_modules, SIM_log_critical, SIM_get_object,
                    SIM_class_has_attribute)

import os, time
import cli, conf

from comp import *
import systempanel
import systempanel.widgets as w
from component_utils import get_highest_2exp

kB = 1<<10
MB = 1<<20
GB = 1<<30

def get_module_for_class(classname, sim):
    modules = [ m for m in SIM_get_all_modules() if classname in m[7] ]
    if len(modules) == 0:
        SIM_log_critical(sim, 0,
                   f'Could not find module providing class <{classname}>')
        return None
    if len(modules) > 1:
        SIM_log_info(1, sim, 0,
                   f'WARNING: Multiple modules offer <{classname}>.' +
                   f' Using module {modules[0][0]} for check.')
    return modules[0]


def check_apic_and_apic_bus(apic, apic_bus):
    sim     = SIM_get_object('sim')
    a_module = get_module_for_class(apic.__class_name__, sim)
    b_module = get_module_for_class(apic_bus.__class_name__, sim)
    if not apic_bus.__class_name__ in a_module[7]:
        SIM_log_info(1, sim, 0,
                   f'\nWARNING: Apic "{apic.__object_name__}" loaded from'+
                   f' module "{a_module[0]}".\n'+
                   f'         while apic bus "{apic_bus.__object_name__}'+
                   f' is loaded from module "{b_module[0]}"')

class X86ApicProcessorDownConnectorWithApicBusCheck(X86ApicProcessorDownConnector):
    def __init__(self, id, phys_mem, port_mem, apic_bus, callback = None, required = False, clock = None):
        X86ApicProcessorDownConnector.__init__(self, id, phys_mem, port_mem, apic_bus, callback, required, clock)

    def connect(self, cmp, cnt, attr):
        (cpus,) = attr
        for cpu in cpus:
            check_apic_and_apic_bus(cpu[0].apic, cmp.get_slot(self.apic_bus))
        X86ApicProcessorDownConnector.connect(self, cmp, cnt, attr)

class chassis_x58_ich10(x86_chassis):
    '''Chassis for Intel® X58-ICH10 systems.'''
    _class_desc = 'an Intel® X58-ICH10 chassis'

    class system_icon(StandardComponent.system_icon):
        val = "x58-ich10.png"

    def setup(self):
        x86_chassis.setup(self)

class chassis_qsp_x86(x86_chassis):
    '''Chassis for QSP x86 systems.'''
    _class_desc = 'a QSP x86 chassis'

    class system_icon(StandardComponent.system_icon):
        val = "x58-ich10.png"

class x86_reset_signal_conv(pyobj.ConfObject):
    '''Converts reset signals from the signal interface to the x86 reset bus'''
    _class_desc = "converter of reset signals"
    class target(pyobj.SimpleAttribute("o", attr=simics.Sim_Attr_Required)):
        '''Reset target, implementing the x86_reset_bus interface'''
        pass
    class signal(pyobj.Interface):
        def signal_raise(self):
            self._up.target.val.iface.x86_reset_bus.reset_all()
        def signal_lower(self):
            pass

class auto_apic_bus(pyobj.ConfObject):
    '''Dummy class to prevent loading just "some" apic as default'''
    _class_desc = "dummy apic class"

### northbridge
class northbridge_x58(StandardConnectorComponent):
    """Intel® X58 Express Chipset."""
    _class_desc = "an Intel® X58 Express Chipset"
    _no_new_command = object()
    _no_create_command  = object()

    class root_port_class(SimpleConfigAttribute('x58-pcie-port', 's')):
        """The class used for the root port. Defaults to x58-pcie-port."""
        def setter(self, val):
            if self._up.obj.configured:
                return simics.Sim_Set_Illegal_Value
            self.val = val

    def setup(self):
        if not self.instantiated.val:
            self.add_northbridge_x58_objects()
        self.add_northbridge_x58_connectors()

    def add_northbridge_x58_connectors(self):
        self.add_connector('gpu', PciBusDownConnector(15, 'pci_bus',
                                                      hotpluggable=False))
        self.add_slot(
            'pci_slot', [self.add_connector(
                    None, PciBusDownConnector(11 + i, 'pci_bus')) for i in range(2)])

        # We need this to support old checkpoints
        if self.get_slot('pcie_p1').classname == 'x58-pcie-port':
            port = 'pcie_p%d.downstream_port'
        else:
            port = 'pcie_p%d_bus'  # legacy pcie-bus object

        connectors = [self.add_connector(None, PciBusDownConnector(0, port % d))
                      for d in (1, 2, 3, 4, 5, 7)]  # 6 omitted on purpose
        self.add_slot('pcie_slot', connectors)

    def add_pcie_port(self, name, port_index, pci_bus, link_width=4):
        bridge = self.add_pre_obj(name, self.root_port_class.val)
        if SIM_class_has_attribute(self.root_port_class.val, 'link_width'):
            bridge.link_width = link_width
        else:
            SIM_log_info(
                2,
                self.obj,
                0,
                f"Object {self.obj.name}.{name} with class {self.root_port_class.val} does not have the attribute link_width. Ignoring...",
            )
        if SIM_class_has_attribute(self.root_port_class.val, "port_index"):
            bridge.port_index = port_index
        else:
            SIM_log_info(
                2,
                self.obj,
                0,
                f"Object {self.obj.name}.{name} with class {self.root_port_class.val} does not have the attribute port_index. Ignoring...",
            )
        return bridge

    def add_northbridge_x58_objects(self):
        # CPU Reset
        cpu_reset_bus = self.add_pre_obj('cpu_reset_bus', 'x86-reset-bus')
        cpu_reset_bus.reset_targets = []

        # Reset from south bridge
        corerst_bus = self.add_pre_obj('corerst_bus', 'signal-bus')
        corerst_bus.targets = []
        cpu_init_bus = self.add_pre_obj('cpu_init_bus', 'signal-bus')
        cpu_init_bus.targets = []

        pci_bus = self.add_pre_obj('pci_bus', 'pcie-downstream-port-legacy')
        pci_bus.pci_devices = []
        pci_bus.mem_space.map = []
        pci_bus.memory_space = pci_bus.mem_space  # for legacy connectors
        corerst_bus.targets += [pci_bus.port.HOT_RESET]

        # Core Registers
        cr = self.add_pre_obj('core', 'x58-core')

        # DMI (Uncore / bridge)
        bridge = self.add_pre_obj('bridge', 'x58-dmi')
        bridge.default_remapping_unit = cr.remap_unit[0]
        bridge.downstream_target = pci_bus.port.downstream
        pci_bus.upstream_target = bridge

        # PCI Express Port Setup
        #
        # The X58 northbridge PCI express ports can be configured
        # to support different lane width allocations according to
        # the following table:
        #
        # Port   1   2   3   4   5   6   7   8   9   10
        #       x2  x2  x4  x4  x4  x4  x4  x4  x4   x4
        #       x4   -  x8   -  x8   -  x8   -  x8    -
        #               x16  -   -   -  x16  -   -    -
        #
        # This configuration is hardwired on a physical board.
        # Select lane widths when creating the pcie_pX ports
        # and then modify pci_bus.pci_devices accordingly.
        #

        # PCI Express Ports 1-2
        pcie_p1 = self.add_pcie_port('pcie_p1', 1, pci_bus, 2)
        pcie_p2 = self.add_pcie_port('pcie_p2', 2, pci_bus, 2)

        # PCI Express Ports 3-4
        pcie_p3 = self.add_pcie_port('pcie_p3', 3, pci_bus, 4)
        pcie_p4 = self.add_pcie_port('pcie_p4', 4, pci_bus, 4)

        # PCI Express Ports 5-6
        pcie_p5 = self.add_pcie_port('pcie_p5', 5, pci_bus, 8)
        # pcie_p6 = self.add_pcie_port('pcie_p6', 6, pci_bus, 4)

        # PCI Express Ports 7-8
        pcie_p7 = self.add_pcie_port('pcie_p7', 7, pci_bus, 16)
        # pcie_p8 = self.add_pcie_port('pcie_p8', 8, pci_bus, 4)

        # PCI Express Ports 9-10
        # pcie_p9 = self.add_pcie_port('pcie_p9', 9, pci_bus, 4)
        # pcie_p10 = self.add_pcie_port('pcie_p10', 10, pci_bus, 4)

        # QuickPath Interconnects
        qp = self.add_pre_obj('qpi_port[2]', 'x58-qpi-port')
        qp[0].bank.pcie_config[0].device_id = 0x3425
        qp[0].bank.pcie_config[1].device_id = 0x3426
        qp[1].bank.pcie_config[0].device_id = 0x3427
        qp[1].bank.pcie_config[1].device_id = 0x3428

        # Northbridge IOxAPIC
        ioxapic = self.add_pre_obj('ioxapic', 'x58-ioxapic')

        # Add PCIe devices
        pci_bus.devices = [
            [0, bridge],
            [1, 0, pcie_p1],
            [2, 0, pcie_p2],
            [3, 0, pcie_p3],
            [4, 0, pcie_p4],
            [5, 0, pcie_p5],
            # [6, 0, pcie_p6],
            [7, 0, pcie_p7],
            # [8, 0, pcie_p8],
            # [9, 0, pcie_p9],
            # [10, 0, pcie_p10],
            [16, 0, qp[0]],
            [17, 0, qp[1]],
            [19, 0, ioxapic],
            [20, 0, cr],
        ]



### Southbridge

# System panel
class southbridge_ich10_panel(systempanel.SystemPanel):
    '''System panel for the <class>southbridge_ich10</class> component'''
    _class_desc = """an Intel® ICH10 system panel"""
    default_layout = w.Row(contents=[
        # Keyboard LEDs. Eventually, we could include more of the keyboard.
        w.LabeledBox('Keyboard', w.Grid(columns=2, contents=[
                        w.Led('kbd_caps_lock'), w.Label("Caps Lock"),
                        w.Led('kbd_num_lock'), w.Label("Num Lock"),
                        w.Led('kbd_scroll_lock'), w.Label("Scroll Lock")])),

        # An ethernet port with LEDs.
        w.LabeledBox('Ethernet', w.Grid(columns=2, contents=[
                        w.Led('eth_link'), w.Label("Link"),
                        w.Led('eth_tx'), w.Label("Tx"),
                        w.Led('eth_rx'), w.Label("Rx")])),
        ])
    objects = default_layout.objects()
    # added automatically to all systems, so no need to have creation commands
    _no_new_command     = object()
    _no_create_command  = object()

class southbridge_ich10(StandardConnectorComponent):
    """Intel® ICH10 (southbridge)."""
    _class_desc = "an Intel® ICH10 (southbridge)"
    _no_new_command     = object()
    _no_create_command  = object()

    def setup(self):
        super().setup()
        if not self.instantiated.val:
            self.add_objects()
        self.add_connectors()

    class rtc_time(SimpleConfigAttribute('19-07-04 08:00:00', 's')):
        """System real time in format \"YY-MM-DD hh:mm:ss\".  Please note that
 time-zone information is not supported and will be silently dropped when
 passed to the RTC object."""
        def setter(self, val):
            if self._up.obj.configured:
                return simics.Sim_Set_Illegal_Value
            self.val = val

    class mac_address(ConfigAttribute):
        """Mac address ('XX:XX:XX:XX:XX:XX' string)"""
        attrtype = "s"
        def _initialize(self):
            self.val = '20:20:20:20:20:20'
        def getter(self):
            return self.val
        def setter(self, val):
            self.val = val

    class basename(StandardConnectorComponent.basename):
        val = 'ich10_cmp'

    class spi_flash(SimpleConfigAttribute('', 's')):
        """The ICH10 SPI flash file to use."""
        def lookup(self):
            if self.val:
                lookup = simics.SIM_lookup_file(self.val)
                if not lookup:
                    print('lookup of SPI flash %s failed' % self.val)
                    return ''
                return lookup
            return self.val

    class lan_bios(SimpleConfigAttribute(None, 's|n')):
        '''The Expansion ROM BIOS file for the ICH10 LAN.'''
        def setter(self, val):
            if self._up.obj.configured:
                return simics.Sim_Set_Illegal_Value
            self.val = val
        def lookup(self):
            if self.val:
                lookup = simics.SIM_lookup_file(self.val)
                if not lookup:
                    print('lookup of LAN BIOS %s failed' % self.val)
                    return ''
                return lookup
            return self.val

    class smbus_use_i2c_v2(SimpleConfigAttribute(False, 'b')):
        """Set to True if i2c v2 shall be used by smbus controller."""

    def add_objects(self):
        # CF9
        cf9 = self.add_pre_obj('cf9', 'ich10_cf9')

        # Platform reset bus
        pltrst_bus = self.add_pre_obj('pltrst_bus', 'signal-bus')
        pltrst_bus.targets = []
        init_bus = self.add_pre_obj('init_bus', 'signal-bus')
        init_bus.targets = []
        cf9.reset_signal = pltrst_bus
        cf9.init_signal = init_bus

        # ISA
        isa = self.add_pre_obj('isa', 'ISA')
        isa.irq_to_pin = [2,   1,  0,  3,  4,  5,  6,  7,  8,  9,
                          10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
                          20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
                          30, 31]
        isa_bus = self.add_pre_obj('isa_bus', 'port-space')
        isa_bus.map = []

        # LPC
        lpc = self.add_pre_obj('lpc', 'ich10_lpc')

        # IOAPIC
        ioapic = self.add_pre_obj('ioapic', 'io-apic')
        ioapic.ioapic_id = 8 << 24
        isa.ioapic = ioapic
        if conf.sim.build_id >= 6045:
            pltrst_bus.targets += [ioapic.port.RESET]
        else:
            pltrst_bus.targets += [[ioapic, "RESET"]]

        # SPI flash M25Pxx and its master device
        spi_image = self.add_pre_obj('spi_image', 'image')
        spi_image.size = 1 << 21
        spi_flash = self.add_pre_obj('spi_flash_obj', 'M25Pxx')
        spi_flash.mem_block = spi_image
        spi_flash.sector_number = (1 << 21) >> 16
        spi_flash.sector_size = 1 << 16
        spi = self.add_pre_obj('spi', 'ich10_spi')
        spi.spi_slave = spi_flash
        spi_flash.spi_master = spi
        if self.spi_flash.lookup():
            spi_image.files = [[self.spi_flash.val, 'ro', 0, 0]]

        pic = self.add_pre_obj('pic', 'i8259x2')
        if conf.sim.build_id >= 6045:
            pltrst_bus.targets += [pic.port.RESET]
        else:
            pltrst_bus.targets += [[pic, "RESET"]]

        for (offset, ports) in ((0x20, (0x20, 0x24, 0x28, 0x2C, 0x30, 0x34, 0x38, 0x3C)),
                                (0x21, (0x21, 0x25, 0x29, 0x2D, 0x31, 0x35, 0x39, 0x3D)),
                                (0xA0, (0xA0, 0xA4, 0xA8, 0xAC, 0xB0, 0xB4, 0xB8, 0xBC)),
                                (0xA1, (0xA1, 0xA5, 0xA9, 0xAD, 0xB1, 0xB5, 0xB9, 0xBD))):
            for p in ports:
                isa_bus.map += [[p, pic, 0, offset, 1]]
        for i in range(2): # 0 --- Master PIC, 1 --- Slave PIC
            isa_bus.map += [[0x4d0 + i, pic, 0, 0x4d0 + i, 1]]
        isa.pic = pic

        # SMBus
        smbus_image = self.add_pre_obj('smbus_image', 'image', size = 32)
        if not self.smbus_use_i2c_v2.val:
            i2c_link = self.add_pre_obj('i2c_link', 'i2c_link')
            self.add_pre_obj('smbus',
                             'ich10_smbus',
                             smbus = i2c_link,
                             sram_image = smbus_image)
        else:
            self.add_pre_obj('smbus',
                             'ich10_smbus_i2c_v2',
                             sram_image = smbus_image)

        # Thermal
        self.add_pre_obj('thermal', 'ich10_thermal')

        # Ethernet controller
        phy = self.add_pre_obj('phy', 'generic_eth_phy')
        phy.address = 1
        lan = self.add_pre_obj('lan', 'ich10_lan_v2')
        lan.phy = phy
        lan.mii = phy
        lan.phy_address = 1
        lan.flash = [spi, "gbe_regs"]
        lan.flash_func = 1
        lan.chipset_config = [lpc, 'cs_conf']
        phy.mac  = lan
        phy.registers = [0, 0, 0x02A8, 0x0390] + [0] * 28
        phy.tx_bandwidth = 1000000000 # 1Gb/s
        if self.lan_bios.val:
            prom_image = self.add_pre_obj('lan_prom_image', 'image')
            prom = self.add_pre_obj('lan_prom', 'rom')
            prom.image = prom_image

        # High-precision event timer
        hpet = self.add_pre_obj('hpet', 'ich10_hpe_timer')
        hpet.intc_8259 = pic
        hpet.intc_apic = ioapic
        hpet.comp_write_slow_cycles = 200
        hpet.lpc = lpc
        lpc.hpet = hpet
        pltrst_bus.targets += [[hpet, 'SRESET']]

        # DMI-to-PCI bridge, and three external PCI slots
        bridge = self.add_pre_obj('bridge', 'ich10_bridge')
        pltrst_bus.targets += [[bridge, 'pltrst']]
        ext_pci = self.add_pre_obj('ext_pci_bus', 'pci-bus')
        ext_conf = self.add_pre_obj('ext_conf', 'memory-space')
        ext_mem = self.add_pre_obj('ext_mem', 'memory-space')
        ext_io  = self.add_pre_obj('ext_io', 'memory-space')
        ext_conf.map = []
        ext_mem.map = []
        ext_io.map = []
        ext_pci.pci_devices = []
        ext_pci.conf_space = ext_conf
        ext_pci.io_space = ext_io
        ext_pci.memory_space = ext_mem
        bridge.secondary_bus = ext_pci
        ext_pci.bridge = bridge
        ext_pci.upstream_target = bridge

        # Intel® 8254 Timer
        timer = self.add_pre_obj('timer', 'i8254')
        timer.irq_dev = hpet
        timer.irq_level = 0
        timer.out = [1, 1, 1]
        for i in range(4):
            isa_bus.map += [[0x40 + i, timer, 0, i, 1]]
            isa_bus.map += [[0x50 + i, timer, 0, i, 1]]
        isa_bus.map += [[0x61, timer, 1, 0, 1]] #TODO remove?
        if conf.sim.build_id >= 6045:
            pltrst_bus.targets += [timer.port.RESET]
        else:
            pltrst_bus.targets += [[timer, "RESET"]]

        # DMA
        dma = self.add_pre_obj('dma', 'i8237x2')
        for i in range(0x00, 0x10):
            isa_bus.map += [[i + 0x00, dma, 0, i, 1]]
            isa_bus.map += [[i + 0x10, dma, 0, i, 1]]
        for i in range(0x81, 0x90):
            isa_bus.map += [[i + 0x00, dma, 0, i, 1]]
            if i != 0x82: # Port 0x92 is used to enable A20 pin
                isa_bus.map += [[i + 0x10, dma, 0, i, 1]]
        for i in range(0xc0, 0xe0, 2):
            isa_bus.map += [[i + 0x0, dma, 0, i, 1]]
            isa_bus.map += [[i + 0x1, dma, 0, i, 1]]

        # RTC Timer
        # NOTE:
        #   U128E always is 0 for chipset configuration is dummy
        #   RTC is mapped to fixed IO space (0x70 ~ 0x73) and (0x74 ~ 0x77)
        rtc = self.add_pre_obj('rtc', 'ich10_rtc', time = self.rtc_time.val)
        rtc.rtc_reg_a = 0x20
        rtc.rtc_reg_b = 0x02
        rtc.irq_level = 8
        rtc.irq_dev = hpet
        for (offset, ports) in ((0x0, (0x70, 0x74, 0x72, 0x76)),
                                (0x1, (0x71, 0x75, 0x73, 0x77))):
            for p in ports:
                isa_bus.map += [[p, rtc, 0, offset, 1]]

        # FLP
        fd = self.add_pre_obj('fd[2]', 'floppy-drive')
        flp = self.add_pre_obj('flp', 'i82077')
        flp.dma_channel = 2
        flp.drives = [fd[0], fd[1]]
        flp.irq_level = 6
        flp.irq_dev = isa
        flp.dma_dev = dma
        # TODO should be mapped by FFD Decode Range, 13.1.21 p399
        isa_bus.map += [[0x3f2, flp, 0, 0, 1],
                        [0x3f4, flp, 0, 2, 1],
                        [0x3f5, flp, 0, 3, 1],
                        [0x3f7, flp, 0, 5, 1]]

        # KBD
        kbd = self.add_pre_obj('kbd', 'i8042')
        kbd.kbd_irq_level = 1
        kbd.mouse_irq_level = 12
        kbd.irq_dev = isa
        isa_bus.map += [[0x60, kbd, 0, 0, 1],
                        [0x64, kbd, 0, 4, 1]]

        # LPC devices
        com = self.add_pre_obj('com[%d]' % 4, 'NS16550')
        lpc_io = self.add_pre_obj('lpc_io', 'memory-space')
        lpc_mem = self.add_pre_obj('lpc_mem', 'memory-space')
        for d in range(4):
            com[d].irq_dev = [lpc, 'com%d_in' % (d + 1)]
            base = (0x3F8, 0x2F8, 0x3E8, 0x2E8)[d]
            for i in range(8):
                isa_bus.map += [[base + i, com[d], 0, i, 1]]

        lpc.serial_port = com
        lpc.coma_level = 3
        lpc.comb_level = 4
        lpc.coma_pirq = isa
        lpc.comb_pirq = isa
        lpc.flash = [spi, "spi_regs"]
        lpc.irq_dev = pic
        lpc.ioapic = ioapic
        lpc.lpc_io = lpc_io
        lpc.lpc_memory = lpc_mem
        isa_bus.map += [[0xb2, lpc, 2, 0, 1],
                        [0xb3, lpc, 2, 1, 1]]

        # SATA
        ide = self.add_pre_obj('ide[4]', 'ide')
        self.get_slot('ide[0]').irq_level = 14
        self.get_slot('ide[1]').irq_level = 15
        self.get_slot('ide[2]').irq_level = 0 # useless
        self.get_slot('ide[3]').irq_level = 0 # useless
        sata2 = self.add_pre_obj('sata2', 'ich10_sata_f2')
        sata2.ide = ide[0:2]
        sata2.chipset_config = [lpc, 'cs_conf']
        sata5 = self.add_pre_obj('sata5', 'ich10_sata_f5')
        sata5.ide = ide[2:4]
        sata5.chipset_config = [lpc, 'cs_conf']
        for i in range(4):
            ide[i].bus_master_dma = self.get_slot('sata%d' % (5, 2)[i < 2])
            ide[i].primary = (1, 0)[i % 2]
            ide[i].interrupt_delay = 1.0e-5
            ide[i].irq_dev = isa
        # Legacy primary IDE mappings
        isa_bus.map += [[0x1f0, ide[0], 0, 0x0, 4]] # ???4
        for i in range(1, 8):
            isa_bus.map += [[0x1f0 + i, ide[0], 0, i, 1]]
        isa_bus.map += [[0x3f6, ide[0], 0, 8, 1]]
        # Legacy secondary IDE mappings
        isa_bus.map += [[0x170, ide[1], 0, 0x0, 4]] # ???4
        for i in range(1, 8):
            isa_bus.map += [[0x170 + i, ide[1], 0, i, 1]]
        isa_bus.map += [[0x376, ide[1], 0, 8, 1]]

        # USB host controllers
        uhci = self.add_pre_obj('uhci[%d]' % 6, 'ich10_usb_uhci')
        for i in range(6):
            dev_num = [[29, 0], [29, 1], [29, 2], [26, 0], [26, 1], [26, 2]]
            uhci[i].chipset_config = [lpc, 'cs_conf']
            uhci[i].pci_dev_num = dev_num[i][0]
            uhci[i].pcidev_func_num = dev_num[i][1]
            uhci[i].frame_list_polling_enabled = True
        ehci = self.add_pre_obj('ehci[%d]' % 2, 'ich10_usb_ehci')
        for i in range(2):
            dev_num = [[29, 7], [26, 7]]
            ehci[i].chipset_config = [lpc, 'cs_conf']
            ehci[i].pci_dev_num = dev_num[i][0]
            ehci[i].pcidev_func_num = dev_num[i][1]
            ehci[i].async_list_polling_enabled = True
            ehci[i].periodic_list_polling_enabled = True
            idx = i * 3
            ehci[i].companion_hc = [uhci[idx], uhci[idx],
                                    uhci[idx + 1], uhci[idx + 1],
                                    uhci[idx + 2], uhci[idx + 2]]

        # Panel
        self.add_component('system_panel', 'southbridge_ich10_panel', [])

        # Keyboard leds
        kbd.led_caps_lock = self.get_slot('system_panel.kbd_caps_lock')
        kbd.led_num_lock = self.get_slot('system_panel.kbd_num_lock')
        kbd.led_scroll_lock = self.get_slot('system_panel.kbd_scroll_lock')

        # PHY leds
        phy.link_led = self.get_slot('system_panel.eth_link')
        phy.tx_led = self.get_slot('system_panel.eth_tx')
        phy.rx_led = self.get_slot('system_panel.eth_rx')

    def update_mac_address(self):
        # First store the MAC in the ich10
        lan = self.get_slot('lan')
        lan.mac_address = self.mac_address.val

        spi = self.get_slot('spi_image')
        ### update the initial 6 bytes with the mac address

        mac = bytes(int(x, 16) for x in self.mac_address.val.split(':'))
        spi.iface.image.set(0x81000, mac)

        # calculate the flash checksum of the first 126 bytes and write the
        # last word so that the end result becomes 0xBABA
        data = spi.iface.image.get(0x81000, 126)
        checksum = 0
        for x in range(0, 126, 2):
            val = ord(data[x:x+1]) | ord(data[x+1:x+2]) << 8
            checksum += val
            checksum &= 0xFFFF

        checksum = 0xBABA - checksum
        if checksum < 0:
            checksum += 0x10000

        spi.iface.image.set(0x8107e, bytes((checksum & 0xff, checksum >> 8)))

    def add_connectors(self):
        class X58_IdeSlotDownConnector(IdeSlotDownConnector):
            def __init__(self, device, master, port, is_sata2):
                self.port = port
                self.is_sata2 = is_sata2
                IdeSlotDownConnector.__init__(self, device, master)
            def set_link_status(self, cmp, status):
                if self.is_sata2:
                    setattr(cmp.get_slot("sata2"),
                            "ahci_p%dpcr_sata_ssts" % self.port, status)
                else:
                    sts = getattr(cmp.get_slot("sata5"), "sata_ch_ssts")
                    sts[self.port - 4] = status

            def connect(self, cmp, cnt, attr):
                IdeSlotDownConnector.connect(self, cmp, cnt, attr)
                self.set_link_status(cmp, 0x113)
            def disconnect(self, cmp, cnt):
                IdeSlotDownConnector.disconnect(self, cmp, cnt)
                self.set_link_status(cmp, 0x0)

        pci_slots = []
        for i in range(4):
            pci_slots.append(self.add_connector(None,
                               PciBusDownConnector(i, 'ext_pci_bus')))
        self.add_slot('pci_slot', pci_slots)

        # IDE slots to connect external IDE disks
        ide_slots = []
        for i in range(4):
            ide_slots.append(self.add_connector(None,
                   X58_IdeSlotDownConnector('ide[%d]' % i, True,
                                            (0, 1, 4, 5)[i], i < 2)))
            if i < 2:
                ide_slots.append(self.add_connector(None,
                   X58_IdeSlotDownConnector('ide[%d]' % i, False,
                                            (2, 3)[i], i < 2)))

        self.add_slot('ide_slot', ide_slots)

        # SATA slots to connect external SATA disks
        sata_slots = []
        for i in range(6):
            sata_slots.append(self.add_connector(None,
                    SataSlotDownConnector('sata2','sata_ctr_p%d' % i, i)))
        self.add_slot('sata_slot', sata_slots)

        # Serial ports to connect such devices as modems.
        serial_ports = []
        for i in range(4):
            serial_ports.append(self.add_connector(None,
                                        SerialDownConnector('com[%d]' % i)))
        self.add_slot('serial', serial_ports)

        self.add_connector('kbd_console', KeyboardDownConnector('kbd'))

        self.add_connector('mse_console', MouseDownConnector('kbd'))

        # USB ports to connect external USB devices
        usb_ports = []
        for i in range(12):
            usb_ports.append(self.add_connector(None,
                                    UsbPortDownConnector('ehci[%d]' % (i // 6))))
        self.add_slot('usb_port', usb_ports)

        # Ethernet port to connect external ethernet link
        self.add_connector('eth_slot', EthernetLinkDownConnector('phy'))

        if self.smbus_use_i2c_v2.val:
            # SMBus connector to connect the controller to the SMBUS
            # at MB level
            # External smbus slaves can be connected via connectors on
            # i2c link
            self.add_connector('sm_bus_mst',
                               I2cLinkDownConnector('smbus.port.master_side'))
            self.add_connector('sm_bus_sl',
                               I2cLinkDownConnector('smbus.port.slave_side'))
        else:
            # SMBus connector to connect external smbus slave devices
            self.add_connector('sm_bus', I2CLinkV1DownConnector('i2c_link'))

        # LPC Bus connector to connect external LPC bus slave device
        # ICH10 consumer chipset LPC bridge supports serial IRQ and IO, DMA,
        # Bus master memory cycles, only SERIRQ and IO cycles are implemented
        self.add_connector(
            'lpc_bus', X86LpcBusDownConnector('lpc', lpc_io = 'lpc_io'))

    class component(StandardComponent.component):
        def pre_instantiate(self):
            if not self._up.lan_bios.val:
                return True
            file_name = self._up.lan_bios.lookup()
            size = os.stat(file_name).st_size
            map_size = get_highest_2exp(size - 1) << 1
            self._up.get_slot('lan_prom_image').size = map_size
            self._up.get_slot('lan').expansion_rom = self._up.get_slot(
                'lan_prom')
            self._up.get_slot('lan').expansion_rom_size = map_size
            self._up.get_slot('lan_prom_image').files = [
                [self._up.lan_bios.val, 'ro', 0, size]]
            return True

        def post_instantiate(self):
            self._up.update_mac_address()

### Motherboard

class motherboard_x58_ich10(StandardConnectorComponent):
    """Motherboard with Intel® X58 Express Chipset (northbridge) and
       Intel® ICH10 (southbridge)."""
    _class_desc = "motherboard with X58 and ICH10"

    class num_sockets(SimpleConfigAttribute(8, 'i')):
        """Number of CPU sockets or modules that can be connected."""

    def setup(self):
        super().setup()
        if not self.instantiated.val:
            self.cpu_map = {} # Store cpu -> apic, is_logical
            self.memory_megs = 0
            self.add_motherboard_x58_ich10_objects()
        self.add_motherboard_x58_ich10_connectors()
        if not self.instantiated.val and self.smbus_use_i2c_v2.val:
            i2c_link = self.get_slot('smbus')
            for tgt_conn in ['sm_bus_sl', 'sm_bus_mst']:
                i2c_conn0 = i2c_link.cli_cmds.get_available_connector(
                                                               type='i2c-link')
                # we cannot use self.connect because that does not work for
                # dynamic connectors
                cli.global_cmds.connect(
                    cnt0=i2c_conn0,
                    cnt1=self.get_slot(f'sb.{tgt_conn}'))


    class root_port_class(SimpleConfigAttribute('x58-pcie-port', 's')):
        """The class used for the root port. Defaults to x58-pcie-port."""
        def setter(self, val):
            if self._up.obj.configured:
                return simics.Sim_Set_Illegal_Value
            self.val = val

    class uefi_device_name(SimpleConfigAttribute('', 's')):
        """Name of Simics UEFI device. Will not be instantiated
           if set to empty string (the default)."""

    class spi_flash(SimpleConfigAttribute('', 's')):
        """The ICH10 SPI flash file to use."""

    class lan_bios(SimpleConfigAttribute(None, 's|n')):
        '''The Expansion ROM BIOS file for the ICH10 LAN.'''
        def setter(self, val):
            if self._up.obj.configured:
                return simics.Sim_Set_Illegal_Value
            self.val = val

    class mac_address(SimpleConfigAttribute('', 's')):
        """MAC address"""
        def _initialize(self):
            self.val = '20:20:20:20:20:20'
        def getter(self):
            return self.val
        def setter(self, val):
            self.val = val

    # NOTE: Should not be a ConfigAttribute, updated from processor connector
    class cpus_per_slot(Attribute):
        """The processors connected to the motherboard in ID order."""
        attrtype = '[[o|n*]*]'
        def _initialize(self):
            self.val = []
        def getter(self):
            if component_utils.get_writing_template() or not self._up.instantiated.val:
                return []
            return self.val
        def setter(self, val):
            self.val = val
            self._up.cpu_list.rebuild()

        def get_cpu_list(self):
            ret = []
            for l in self.val:
                if l:
                    for c in l:
                        ret.append(c)
            return ret

        def add_cpu(self, id, cpu):
            # TODO, should be done in a smarter way
            if len(self.val) <= id:
                self.val += [None] * (id + 1 - len(self.val))
            if self.val[id] == None:
                self.val[id] = [cpu]
            else:
                self.val[id].append(cpu)
            self._up.cpu_list.rebuild()


    class basename(StandardComponent.basename):
        val = "motherboard"

    class rtc_time(SimpleConfigAttribute(default_rtc_start_time, 's')):
        """The date and time of the Real-Time clock. Please note that time-zone
 information is not supported and will be silently dropped when passed to the
 RTC object."""
        def setter(self, val):
            def illegal_value(val):
                simics.SIM_attribute_error(
                    "Expected time format: YYYY-MM-DD HH:MM:SS, "
                    "e.g. 2006-01-25 12:43:31; got '%s'" % val)
                return simics.Sim_Set_Illegal_Value

            # Strip timezone
            # (same logic is used when passing value to RTC in init_cmos())
            m = re.match(r'(\d+)-(\d+)-(\d+) (\d+):(\d+):(\d+)', val)
            if not m:
                return illegal_value(val)
            # m.group(N)... to please fisketur :(
            self.val = '%s-%s-%s %s:%s:%s' % (m.group(1), m.group(2),
                                              m.group(3), m.group(4),
                                              m.group(5), m.group(6))

            # Validate more than just the format
            try:
                time.strptime(self.val, '%Y-%m-%d %H:%M:%S')
            except Exception:
                return illegal_value(val)

            # Issue warning when deprecated time-zone format is used. Time-zone
            # format is supported for backwards compatibility of target
            # scripts, but the underlying RTC does not support it and that
            # should not go silently undetected.
            if len(val.split()) > 2:
                print('Warning: time-zone format not supported by RTC')

    class break_on_reboot(SimpleConfigAttribute(False, 'b')):
        """If true, the simulation will stop when machine is rebooted."""

    class bios(SimpleConfigAttribute('', 's')):
        """The x86 BIOS file to use."""
        def lookup(self):
            if self.val:
                lookup = simics.SIM_lookup_file(self.val)
                if not lookup:
                    print('lookup of bios file %s failed' % self.val)
                    return ''
                return lookup
            return self.val

    class acpi(SimpleConfigAttribute(True, 'b')):
        """Use ACPI when True, default value is True."""

    class system_clock(SimpleConfigAttribute(False, 'b')):
        """If true, the motherboard will contain a clock separate from the processor, which will be used as queue for all devices. The class used for the clock is taken from system_clock_class."""

    class system_clock_class(SimpleConfigAttribute('clock', 's')):
        """The class used for the system_clock."""

    class apic_bus_class(SimpleConfigAttribute('auto_apic_bus', 's')):
        """APIC bus classname"""

    class smbus_use_i2c_v2(SimpleConfigAttribute(False, 'b')):
        """Set to True if i2c v2 shall be used by smbus controller."""

    class do_not_initialize_mtrr(SimpleConfigAttribute(False, 'b')):
        """Set to True to prevent this component from initializing the mtrr
           registers of the cores."""

    class cpu_list(StandardConnectorComponent.cpu_list):
        def getter(self):
            if component_utils.get_writing_template() or not self._up.instantiated.val:
                return []
            return self.val

        def rebuild(self):
            self.val = self._up.cpus_per_slot.get_cpu_list()

    class component_queue(StandardComponent.component_queue):
        def getter(self):
            if self._up.system_clock.val:
                return self._up.get_slot('clock')
            else:
                if not self._up.cpu_list.val:
                    return None
                return self._up.cpu_list.val[0]


    class component(StandardComponent.component):
        def pre_instantiate(self):
            self._up.setup_memory()
            self._up.get_slot('sb.lpc').cpus = self._up.cpu_list.val
            if self._up.get_slot('apic_bus').__class_name__ == 'auto_apic_bus':
                SIM_log_info(1, self._up.obj, 0,
                   "Apic bus class was neither set via config attribute nor after creation." +
                   " Falling back to apic-bus class now.")
                self._up.get_slot('apic_bus').__class_name__ = 'apic-bus'
            return True
        def post_instantiate(self):
            self._up.update_cpus()
            self._up.init_bios()
            self._up.init_cmos()
            self._up.copy_shadow()
            self._up.setup_apic()
            if not self._up.do_not_initialize_mtrr.val:
                for cpu in self._up.cpu_list.val:
                    self._up.set_mtrr(cpu)


    def mem_bus_callback(self, cnt, attr):
        (memory_megs, memory_ranks) = attr
        self.memory_megs += memory_megs

    def pc_config_cpu_list(self):
        return [[cpu, self.cpu_map[cpu][0],
                                self.cpu_map[cpu][1]] for cpu in self.cpu_list.val]

    def x86_processor_callback(self, socket_id, info):
        (cpu, apic, is_logical) = info
        self.cpu_map[cpu] = (apic, is_logical)
        self.cpus_per_slot.add_cpu(socket_id, cpu)
        cpu.system = self.get_slot('bcast')
        self.get_slot('nb.cpu_reset_bus').reset_targets = self.cpu_list.val
        self.get_slot('nb.corerst_bus').targets.append([cpu, "RESET"])
        self.get_slot('nb.cpu_init_bus').targets.append([cpu, "INIT"])
        self.get_slot('bcast').cpus = self.cpu_list.val
        self.get_slot('conf').cpu_list = self.pc_config_cpu_list()
        self.get_slot('conf').ioapic_id = 8

        socket_bus = self.get_slot(f'socket[{socket_id}].pcie_bus')
        pcie_bus = self.get_slot('nb.pci_bus')
        # this function is called for each core on the cpu inserted
        # into the socket, we must add the socket bus only once.
        if socket_bus.port.downstream not in pcie_bus.devices:
            pcie_bus.devices += [socket_bus.port.downstream]


    def add_motherboard_x58_ich10_connectors(self):
        # Memory DIMMs
        dimms = []
        for i in range(4):
            # The first socket is required, can't run without memory
            dimms.append(self.add_connector(
                    None, MemBusDownConnector(
                        'smbus', 0x50 + 2 * i, connect_callback = self.mem_bus_callback,
                        required = (i == 0))))
        self.add_slot('dimm', dimms)

        # Reset
        self.add_connector('reset_bus', X86ResetBusDownConnector('reset'))

        # Processors
        cpus = []
        clock_slot = None
        if self.system_clock.val:
            clock_slot = 'clock'
        for i in range(self.num_sockets.val):
            cpus.append(self.add_connector('', X86ApicProcessorDownConnectorWithApicBusCheck(
                        i, 'phys_mem', 'port_mem_m', 'apic_bus',
                        self.x86_processor_callback, clock = clock_slot)))
        self.add_slot('socket', cpus)

    def add_motherboard_x58_ich10_objects(self):
        if self.system_clock.val:
            # Clock
            clock = self.add_pre_obj('clock', self.system_clock_class.val)
            clock.freq_mhz = 2000

        # Northbridge and Southbridge
        self.add_component('nb', 'northbridge_x58', [
            ['root_port_class', self.root_port_class.val]
        ])
        self.add_component('sb', 'southbridge_ich10',
                           [['rtc_time', self.rtc_time.val],
                            ['spi_flash', self.spi_flash.val],
                            ['lan_bios', self.lan_bios.val],
                            ['smbus_use_i2c_v2', self.smbus_use_i2c_v2.val]])

        # Required by many devices
        pci_bus = self.get_slot('nb.pci_bus')

        # CPU sockets
        for i in range(self.num_sockets.val):
            qpi = self.add_pre_obj(f'socket[{i}].qpi_arch', 'x58-qpi-arch')
            qpi.socket_id = i
            qpi.cfg_space = pci_bus.port.cfg
            socket_bus = self.add_pre_obj(
                f'socket[{i}].pcie_bus', 'pcie-downstream-port')
            socket_bus.transparent_enabled = True
            socket_bus.devices = [[0, qpi]]

        # RAM
        dram_space = self.add_pre_obj('dram_space', 'memory-space')
        ram_image = self.add_pre_obj('ram_image', 'image')
        ram = self.add_pre_obj('ram', 'ram')
        ram.image = ram_image

        # PC config
        pc_conf = self.add_pre_obj('conf', 'pc-config')
        pc_conf.megs = self.memory_megs
        pc_conf.user_rsdp_address = 0
        pc_conf.build_acpi_tables = self.acpi.val
        pc_conf.memory_space = dram_space

        # BIOS in ROM
        if self.bios.lookup():
            rom_image = self.add_pre_obj('rom_image', 'image')
            rom = self.add_pre_obj('rom', 'rom')
            rom.image = rom_image

        # Port space
        port_mem_m = self.add_pre_obj('port_mem_m', 'memory-space')
        port_mem = self.add_pre_obj('port_mem', 'port-space')
        port_mem.map = [
            [0x510, pc_conf, 3, 0, 2],
            [0x511, pc_conf, 4, 0, 1],
            [0xfff0, pc_conf, 0, 0, 1],
            [0xfff1, pc_conf, 1, 0, 1],
            [0xfff2, pc_conf, 2, 0, 2]]
        port_mem_m.map = [[0, port_mem, 0, 0, 0x10000, None]]

        phys_mem = self.add_pre_obj('phys_mem', 'memory-space')
        phys_mem.map = []
        phys_mem.default_target = [pci_bus.port.mem, 0, 0, None]

        # PC shadow
        shadow = self.add_pre_obj('shadow', 'pc-shadow')
        shadow_mem = self.add_pre_obj('shadow_mem', 'memory-space')
        shadow_mem.map = [[0x100000, dram_space, 0, 0, 0x100000]]
        port_mem.map += [
            [0xfff4, shadow, 0,     0, 1],
            [0xfff5, shadow, 0,     1, 1]]

        # a dummy space to avoid errors when BIOS probes for Option ROMs
        dummy_rom = self.add_pre_obj('dummy_rom', 'set-memory')
        shadow_mem.map += [[0xc0000, dummy_rom, 0, 0, 0x20000, None, 15]]

        # Add shadow paravirt device to reset bus
        if conf.sim.build_id >= 6045:
            self.get_slot('nb.corerst_bus').targets += [shadow.port.RESET]
        else:
            self.get_slot('nb.corerst_bus').targets += [[shadow, "RESET"]]

        # Broadcast object
        bcast = self.add_pre_obj('bcast', 'x86_broadcast')
        bcast.images = [ram_image]

        # SMBUS
        if self.smbus_use_i2c_v2.val:
            self.add_component('smbus', 'i2c_link_v2', [])
        else:
            self.add_pre_obj('smbus', 'i2c-bus')

        # APIC
        apic_bus = self.add_pre_obj('apic_bus', self.apic_bus_class.val)
        apic_bus.apics = []
        apic_bus.ioapic = []
        apic_bus.pic = self.get_slot('sb.pic')

        # configure remap units
        cr = self.get_slot('nb.core')
        for i in range(2):
            vtd = cr.remap_unit[i]
            vtd.apic_bus = self.get_slot('apic_bus')
            vtd.memory_space = dram_space

        pci_bus.mem_space.default_target = [dram_space, 0, 0, None]

        # Northbridge to motherboard device connections
        self.get_slot('nb.ioxapic').ioapic.apic_bus = self.get_slot('apic_bus')

        # Southbridge to motherboard device connections
        self.get_slot('sb.pic').irq_dev = apic_bus
        self.get_slot('sb.dma').memory = pci_bus.mem_space

        # The 8042 can reset the CPUs (but not the platform)
        self.get_slot('sb.kbd').reset_target = self.get_slot('nb.cpu_reset_bus')

        ioapic = self.get_slot('sb.ioapic')
        ioapic.apic_bus = self.get_slot('apic_bus')
        ioapic.ioapic_version = (0x17 << 16) | 0x20
        apic_bus.ioapic = [ioapic]
        pci_bus.mem_space.map += [[0xfec00000, ioapic, 0, 0, 0x20, None, -1]]

        # Setup southbridge port mappings
        cf9 = self.get_slot('sb.cf9')
        port_mem.map += [
            [0xcf9, cf9, 0, 0x0, 1]]

        # Connect SB PLTRST to NB CORERST
        self.get_slot('sb.pltrst_bus').targets += [self.get_slot('nb.corerst_bus')]
        # Connect SB(CF9) INIT to NB INIT
        self.get_slot('sb.init_bus').targets += [self.get_slot('nb.cpu_init_bus')]

        # Setup northbridge port mappings
        port_mem = self.get_slot('port_mem')
        bridge = self.get_slot('nb.bridge')
        port_mem.map += [
            [0xcf8, bridge.bank.io_regs, 0, 0xcf8, 4],
            [0xcfc, bridge.bank.io_regs, 0, 0xcfc, 4],
            [0xcfd, bridge.bank.io_regs, 0, 0xcfd, 2],
            [0xcfe, bridge.bank.io_regs, 0, 0xcfe, 2],
            [0xcff, bridge.bank.io_regs, 0, 0xcff, 1]]

        # hotfs, only enable the hfs when bios size is less than 1.5 MB
        # MB. Otherwise, address conflict will happen between bios and hfs.
        if self.get_bios_size() <= MB:
            hfs = self.add_pre_obj('hfs', 'hostfs')
            pci_bus.mem_space.map += [[0x0ffe81000, hfs,    0, 0, 0x10]]

        # Setup default mappings
        port_mem.default_target = [pci_bus.io_space, 0, 0, None]
        pci_bus.io_space.default_target = [
            self.get_slot('sb.isa_bus'), 0, 0, None]

        # Add southbridge PCI devices to PCI-bus
        def add_pci_device(pci_bus, dev, dev_num, func_num, legacy=True):
            if legacy:
                pci_bus.pci_devices += [[dev_num, func_num, dev]]
                dev.pci_bus = pci_bus
            else:
                pci_bus.devices += [[dev_num, func_num, dev]]

        lpc = self.get_slot('sb.lpc')
        add_pci_device(pci_bus, lpc, 31, 0)

        sata2 = self.get_slot('sb.sata2')
        add_pci_device(pci_bus, sata2, 31, 2)

        smbus = self.get_slot('sb.smbus')
        add_pci_device(pci_bus, smbus, 31, 3, False)

        sata5 = self.get_slot('sb.sata5')
        add_pci_device(pci_bus, sata5, 31, 5)

        thermal = self.get_slot('sb.thermal')
        add_pci_device(pci_bus, thermal, 31, 6)

        bridge = self.get_slot('sb.bridge')
        add_pci_device(pci_bus, bridge, 30, 0)
        bridge.upstream_target = pci_bus

        lan = self.get_slot('sb.lan')
        add_pci_device(pci_bus, lan, 25, 0)

        if self.mac_address:
            sb = self.get_slot('sb')
            sb.mac_address = self.mac_address.val

        for (device, dev_num, func_num) in (
            ('uhci[0]', 29, 0),
            ('uhci[1]', 29, 1),
            ('uhci[2]', 29, 2),
            ('ehci[0]', 29, 7),
            ('uhci[3]', 26, 0),
            ('uhci[4]', 26, 1),
            ('uhci[5]', 26, 2),
            ('ehci[1]', 26, 7)):
            dev = self.get_slot('sb.' + device)
            add_pci_device(pci_bus, dev, dev_num, func_num)

        # Send all interrupts on the PCI bus to LPC
        bridge = self.get_slot('nb.bridge')
        bridge.interrupt = self.get_slot('sb.lpc')

        if self.uefi_device_name.val:
            simics_uefi = self.add_pre_obj(self.uefi_device_name.val, 'simics-uefi')
            self.get_slot('nb.pci_bus').devices.append([0, 7, simics_uefi])

    def copy_shadow(self):
        # TODO, should be done in a smarter way
        # move all ROM mappings to shadow memory
        pci_mem = self.get_slot('nb.pci_bus.mem_space')
        self.get_slot('shadow_mem').map += ([x for x in pci_mem.map
                                             if x[0] >= 0xc0000 and x[0] < 0x100000])
        pci_mem.map = [x for x in pci_mem.map
                       if x[0] < 0xc0000 or x[0] >= 0x100000]
        pci_mem.map += ([
                [0x0000c0000, self.get_slot('shadow'), 0, 0, 0x40000,
                 self.get_slot('shadow_mem'), 1]])

    def get_bios_size(self):
        if not self.bios.lookup():
            return 0
        bios_size = os.stat(self.bios.lookup()).st_size
        # Default bios contains trailing garbage
        if self.bios.val.startswith("rombios-2."):
            return 64 * kB
        if bios_size > 128 * MB:
            raise CompException(
                "The BIOS file %s is %d bytes, the limit is 128 MB." % (
                    self.bios.val, bios_size))
        return bios_size

    def setup_memory(self):
        if not self.memory_megs:
            raise CompException("No memory DIMMs connected can not setup component.")

        if self.bios.val and not self.bios.lookup():
            raise CompException("BIOS attribute set, but file not found."
                                " Can not setup component.")

        self.get_slot('ram_image').size = self.memory_megs * MB
        self.get_slot('dram_space').map = [
            [0, self.get_slot('ram'), 0, 0, self.memory_megs * MB]]
        self.get_slot('conf').megs = self.memory_megs
        if not self.bios.val:
            self.get_slot('phys_mem').map += [
                [0x00000000, self.get_slot('dram_space'), 0, 0, self.memory_megs * MB]]

        # from may_instantiate
        if not self.bios.lookup():
            return
        bios_size = self.get_bios_size()
        self.get_slot('rom_image').size = bios_size
        pci_mem = self.get_slot('nb.pci_bus.mem_space')
        pci_mem.map += [
            [0x100000000 - bios_size, self.get_slot('rom'), 0, 0, bios_size]]

        pci_mem.map += [
            [0x000e0000, self.get_slot('rom'), 0, bios_size - 0x20000, 0x20000]]

        ram_map = [[0x000000000, self.get_slot('dram_space'), 0, 0, 0xa0000]]

        pci_window_size = 512
        high_mem = 4096 - pci_window_size
        if self.memory_megs > high_mem:
            high_mem *= MB
            highest = (self.memory_megs - 4096) * 1024 * 1024
            ram_map += [
                [0x000100000, self.get_slot('dram_space'), 0, 0x100000,
                 high_mem  - 0x100000, None, 0]]
            if highest > 0:
                ram_map += [[0x100000000, self.get_slot('dram_space'), 0, 0x100000000,
                             highest, None, 0]]
        else:
            megs = (self.memory_megs - 1) * MB
            ram_map += [
                [0x000100000, self.get_slot('dram_space'), 0, 0x100000, megs, None, 0]]
        self.get_slot('phys_mem').map += ram_map

    def set_mtrr(self, cpu):
        def calc_mtrr_mask(size):
            return (~(size - 1) & ((1 << cpu.physical_bits) - 1))

        def get_msr(cpu, index):
            msr = cpu.iface.x86_msr.get(index, Sim_X86_Msr_Attribute_Access)
            if msr.status == Sim_X86_Msr_Ok:
                return msr.value
            else:
                return 0

        def set_msr(cpu, index, value):
            ret = cpu.iface.x86_msr.set(index, value,
                                        Sim_X86_Msr_Attribute_Access)
            assert ret == Sim_X86_Msr_Ok

        mtrrcap = get_msr(cpu, 0xfe)
        if mtrrcap == 0:
            return

        IA32_MTRR_DEF_TYPE = 0x2ff
        set_msr(cpu, IA32_MTRR_DEF_TYPE, 0xc00)

        megs_remaining = self.memory_megs
        next_mtrr = 0
        next_base = 0
        IA32_MTRR_PHYSBASE0 = 0x200
        IA32_MTRR_PHYSMASK0 = 0x201

        while megs_remaining:
            if next_mtrr > (mtrrcap & 0xff):
                print(('Warning: %d megabytes of memory not mapped by '
                       'MTRRs' % megs_remaining))
                break
            this_size = get_highest_2exp(megs_remaining)
            mask = calc_mtrr_mask(this_size * 1024 * 1024)

            set_msr(cpu, IA32_MTRR_PHYSBASE0 + 2 * next_mtrr, next_base | 0x6)
            set_msr(cpu, IA32_MTRR_PHYSMASK0 + 2 * next_mtrr, mask | 0x800)

            megs_remaining = megs_remaining - this_size
            next_base = next_base + this_size * 1024 * 1024
            next_mtrr += 1

        if mtrrcap & (1 << 8):
            IA32_MTRR_FIX_64K_00000 = 0x250
            IA32_MTRR_FIX_16K_80000 = 0x258
            IA32_MTRR_FIX_16K_A0000 = 0x259
            IA32_MTRR_FIX_4K_C0000 = 0x268
            IA32_MTRR_FIX_4K_C8000 = 0x269
            IA32_MTRR_FIX_4K_D0000 = 0x26a
            IA32_MTRR_FIX_4K_D8000 = 0x26b
            IA32_MTRR_FIX_4K_E0000 = 0x26c
            IA32_MTRR_FIX_4K_E8000 = 0x26d
            IA32_MTRR_FIX_4K_F0000 = 0x26e
            IA32_MTRR_FIX_4K_F8000 = 0x26f
            set_msr(cpu, IA32_MTRR_FIX_64K_00000, 0x0606060606060606)
            set_msr(cpu, IA32_MTRR_FIX_16K_80000, 0x0606060606060606)
            set_msr(cpu, IA32_MTRR_FIX_16K_A0000, 0)
            set_msr(cpu, IA32_MTRR_FIX_4K_C0000, 0)
            set_msr(cpu, IA32_MTRR_FIX_4K_C8000, 0)
            set_msr(cpu, IA32_MTRR_FIX_4K_D0000, 0)
            set_msr(cpu, IA32_MTRR_FIX_4K_D8000, 0)
            set_msr(cpu, IA32_MTRR_FIX_4K_E0000, 0)
            set_msr(cpu, IA32_MTRR_FIX_4K_E8000, 0)
            set_msr(cpu, IA32_MTRR_FIX_4K_F0000, 0)
            set_msr(cpu, IA32_MTRR_FIX_4K_F8000, 0)

    def init_bios(self):
        if self.bios.lookup():
            # Load the bios into the ROM area, so that checkpoints not
            # depend on the BIOS file being available all time.
            simics.SIM_load_file(self.get_slot('rom_image'), self.bios.val, 0, 0)

    def init_cmos(self):
        try:
            rtc = self.get_slot('sb.rtc')
        except CompException:
            rtc = None
        if not rtc:
            #print "CMOS device not found - can not write information."
            return
        # set nvram info
        cli.run_command('%s.cmos-init' % self.obj.name)
        cli.run_command('%s.cmos-base-mem 640' % self.obj.name)
        cli.run_command('%s.cmos-extended-mem %d' %
                    (self.obj.name, self.memory_megs - 1))
        m = re.match(r'(\d+)-(\d+)-(\d+) (\d+):(\d+):(\d+)', self.rtc_time.val)
        cli.run_command(('%s.set-date-time '
                     + 'year=%s month=%s mday=%s '
                     + 'hour=%s minute=%s second=%s')
                    % ((rtc.name,) + m.groups()))
        cli.run_command('%s.cmos-boot-dev C' % self.obj.name)
        try:
            flp = self.get_slot('sio.flp')
        except CompException:
            flp = None
        if flp:
            if len(flp.drives):
                cli.run_command('%s.cmos-floppy A "1.44"' % self.obj.name)
            if len(flp.drives) > 1:
                cli.run_command('%s.cmos-floppy B "1.44"' % self.obj.name)
        try:
            ide0 = self.get_slot('sb.ide0')
        except CompException:
            try:
                ide0 = self.get_slot('sb.ide[0]')
            except CompException:
                ide0 = None
        if ide0 and ide0.master:
            size = ide0.master.disk_sectors
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
            cli.run_command('%s.cmos-hd C %d %d %d' % (self.obj.name,
                                                   bios_C, bios_H, bios_S))

    def update_cpus(self):
        cpus_per_slot = []
        for slot_val in self.cpus_per_slot.val:
            val = []
            for cpu in slot_val:
                if cpu == None:
                    val.append(None)
                else:
                    val.append(get_pre_obj_object(cpu))
            cpus_per_slot.append(val)
        self.cpus_per_slot.val = cpus_per_slot
        self.cpu_list.rebuild()

    def setup_apic(self):
        apic_list = []
        a_id = 0
        for i in range(len(self.cpu_list.val)):
            apic_list.append(self.cpu_list.val[i].apic)
            self.cpu_list.val[i].apic.apic_id = a_id
            self.cpu_list.val[i].cpuid_physical_apic_id = a_id
            a_id += 1
            # use ID 8 and 9 for ioapics (old linux kernels require unique IDs)
            if a_id == 8:
                a_id = 10

        self.get_slot('apic_bus').apics = apic_list
        for c in self.cpu_list.val:
            is_bsp = (c == self.cpu_list.val[0])
            if is_bsp:
                c.apic.iface.apic_cpu.power_on(1, c.cpuid_physical_apic_id)
            else:
                c.iface.x86_reg_access.set_activity_state(
                    simics.X86_Activity_Wait_For_SIPI)
            # Set attributes for CPUs from 7123 package
            if hasattr(c, "bsp"):
                c.bsp = int(is_bsp)
            if hasattr(c, "fabric_bsp"):
                c.fabric_bsp = int(is_bsp)


register_cmos_commands('motherboard_x58_ich10')
