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


from blueprints import (
    Alias, Builder, ConfObject, Default, DefaultTarget, MemMap,
    Namespace, Port, SignalPort, BlueprintFun)
from .state import (
    RTCDefaultConfig, X58Backplane, ScriptConsole, X58BackplaneConfig)
from blueprints import blueprint
from blueprints.params import Param
from . import disks, x58, ich10
from simmod.std_bp import disks as std_disks
from .x58 import qsp_cpu
from simmod.std_bp import state, usb, console
from simmod.std_bp.gpu import AccelVGAParams, accel_vga
from ..x86_cpu.state import CPUConfig
from ..classes.x86_classes import cmos_post_instantiate
import os
import simics

def pc_config(bp: Builder, name: Namespace):
    plane = bp.read_state(name, X58Backplane)
    config = bp.get_config(name, X58BackplaneConfig)

    bp.obj(name, "pc-config",
        megs = config.memory_size_mb,
        user_rsdp_address = 0,
        ioapic_id = 8,
        build_acpi_tables = True,
        memory_space = plane.dram_space,
        cpu_list = plane.fabric.cpu_apic_logical_list     # cpu, apic, logical
    )
    plane.system_io_port_map.extend([
        MemMap(0x402, Port(name), 0, 0, 1),
        MemMap(0x510, Port(name), 3, 0, 2),
        MemMap(0x511, Port(name), 4, 0, 1),
        MemMap(0xfff0, Port(name), 0, 0, 1),
        MemMap(0xfff1, Port(name), 1, 0, 1),
        MemMap(0xfff2, Port(name), 2, 0, 2),
    ])


def dram(bp: Builder, name: Namespace):
    plane = bp.read_state(name, X58Backplane)
    config = bp.get_config(name, X58BackplaneConfig)
    mb = config.memory_size_mb
    size = mb * 0x100000

    plane.dram_space = bp.obj(name, "memory-space",
                              map = [[0, name.ram, 0, 0, size]])
    bp.obj(name.ram, "ram", image = name.ram.image)
    bp.obj(name.ram.image, "image", size = size)

    def low_bytes():
        pci_window_size = 512
        size_mb = min(4096 - pci_window_size, mb) - 1
        return size_mb * 0x100000

    plane.system_mem_map.extend([
        MemMap(0, Port(name), 0, 0, 0xa0000),
        MemMap(0x100000, Port(name), 0, 0x100000, low_bytes()),
    ])
    if mb > 4096:
        hmem_bytes = (mb - 4096) * 0x100000
        plane.system_mem_map.append(
            MemMap(0x100000000, Port(name), 0, 0x100000000, hmem_bytes))


def rom(bp: Builder, name: Namespace):
    plane = bp.read_state(name, X58Backplane)
    config = bp.get_config(name, X58BackplaneConfig)

    def file_size(name):
        f = simics.SIM_lookup_file(name)
        if f:
            return os.stat(f).st_size
        else:
            bp.error("file not found:", name)
            return 0

    size = file_size(config.bios) if config.bios else 0
    if size == 0:
        bp.error(f"Missing BIOS '{config.bios}'")

    plane.pci_bus.mem_map.append(
        MemMap(0x100000000 - size, name, 0, 0, size)
    )
    plane.shadow_mem_map.append(
        MemMap(0xe0000, Port(name), 0, size - 0x20000, 0x20000)
    )
    bp.obj(name, "rom", image = name.image)
    bp.obj(name.image, "image",
        size = size,
        files = [[config.bios, "ro", 0, size, 0]],
    )


def pc_shadow(bp: Builder, name: Namespace):
    plane = bp.read_state(name, X58Backplane)

    plane.shadow_mem_map.extend([
        MemMap(0x100000, plane.dram_space, 0, 0, 0x100000),
        # A dummy space to avoid errors when BIOS probes for Option ROMs
        MemMap(0xc0000, Port(name.dummy_rom), 0, 0, 0x20000, priority=15),
    ])
    plane.system_io_port_map.extend([
        MemMap(0xfff4, Port(name.dev), 0, 0, 1),
        MemMap(0xfff5, Port(name.dev), 0, 1, 1),
    ])
    plane.pci_bus.mem_map.extend([
        MemMap(0xc0000, name.dev, 0, 0, 0x40000, name, 1),
    ])
    plane.CORERST_tgts.append(SignalPort(name.dev, "RESET"))

    bp.obj(name, "memory-space", map = plane.shadow_mem_map)
    bp.obj(name.dev, "pc-shadow")
    bp.obj(name.dummy_rom, "set-memory")


def motherboard(bp: Builder, name: Namespace):
    plane = bp.read_state(name, X58Backplane)

    bp.expand(name, "sb", ich10.x58_ich10)
    bp.expand(name, "nb", x58.x58)
    bp.expand(name, "", x58.cpu_sockets)
    bp.expand(name, "dram", dram)
    bp.expand(name, "rom", rom)
    bp.expand(name, "conf", pc_config)
    bp.expand(name, "shadow", pc_shadow)

    # Add corerst signal-bus, triggered by PLTRST
    bp.obj(name.corerst, "signal-bus",
        targets = plane.CORERST_tgts)

    # Add various motherboard objects
    plane.motherboard = bp.obj(name, "x58-mb",
        rtc = plane.rtc)
    bp.at_post_instantiate(name, cmos_post_instantiate)
    plane.fabric.mem_space = bp.obj(name.mem, "memory-space",
        map = plane.system_mem_map,
        default_target = DefaultTarget(plane.pci_bus.mem))
    plane.fabric.io_space = bp.obj(name.io, "port-space",
        map = plane.system_io_port_map,
        default_target = DefaultTarget(plane.pci_bus.io))
    bp.obj(name.io_m, "memory-space",
        map = [MemMap(0, name.io, 0, 0, 0x10000)])
    plane.fabric.apic_bus = bp.obj(name.apic_bus, "apic_bus_v2",
        apics = plane.fabric.apic_list,
        ioapic = plane.io_apic_list,
        pic = plane.pic_8259)

    plane.PLTRST_tgts.append(SignalPort(name.corerst))

# SystemParams is a currently a union of platform and CPU options
class SystemParams(X58BackplaneConfig, CPUConfig, AccelVGAParams):
    "Parameters for the x58-ich10 system."
#
# System Blueprint for x58-ich10
#
@blueprint([Param("memory_size_mb", "Memory size in MB", SystemParams),
            Param("mac_address", "Ethernet MAC address", SystemParams),
            Param("disk_bp", "Disk blueprint name", SystemParams),
            Param("disk_image_path", "Disk image path", SystemParams)],
           "qsp_system")
def system(bp: Builder, name: Namespace, params: SystemParams):
    "X86 QSP System with X58 and ICH10"
    plane = bp.expose_state(name, X58Backplane)

    # Replicate config parameters
    x58_config = bp.create_config(name, X58BackplaneConfig)
    for k in x58_config:
        setattr(x58_config, k, getattr(params, k))

    if params.disk_bp:
        # try x86 disks first, then standard disks
        try:
            plane.disk = getattr(disks, params.disk_bp)
        except:
            plane.disk = getattr(std_disks, params.disk_bp)
    else:
        plane.disk = params.disk

    # Make the 'queue' easily available through a 'queue' context.
    # Mostly useful for external blueprints.
    bp.expose_state(name, plane.queue)

    # Allow disks to provide a default RTC time (to avoid fchk)
    rtc_config = bp.expose_state(name, RTCDefaultConfig)
    rtc_config.rtc_time = Alias(params).rtc_time

    # Provide ScriptConsole (used by auto-login scripts)
    script_console = bp.expose_state(name, ScriptConsole)
    script_console.con = plane.connectors.com[0].remote

    # Alias parameters onto 'params'. E.g.
    #    bp.config.bios => params.bios
    #    bp.cpu_config.num_cpus => params.num_cpus
    #    gpu_config.vga_bios => params.vga_bios
    gpu_config = bp.create_config(name.gfx, AccelVGAParams)
    for k in gpu_config:
        setattr(gpu_config, k, getattr(Alias(params), k))

    cpu_config = bp.create_config(name, CPUConfig)
    cpu_config.cpu_class = Default("x86QSP1")
    cpu_config.model_name = Default("x86-64 QSP")
    cpu_config.num_cpus = Default(1)
    cpu_config.cpu_comp = Default(BlueprintFun(qsp_cpu))

    plane.fabric.context = ConfObject(name.context)
    plane.fabric.cell = ConfObject(name.cell)
    plane.system_ns = name

    bp.expand(name, "mb", motherboard)
    bp.obj(name, "blueprint-namespace", queue=plane.queue.queue)
    bp.obj(name.cell, "cell", time_quantum=0.0001)
    bp.obj(name.context, "context")

    # Optionally add a disk component
    if plane.disk:
        bp.expand(name, "sata0.disk", plane.disk, path=params.disk_image_path)

    # Instantiate a clock if no CPU is instantiated
    plane.queue.queue = Default(ConfObject(name.clock))
    if plane.queue.queue == name.clock:
        bp.obj(name.clock, "clock", freq_mhz = 10)

    # Add UART connectors
    bp.expand(name, "com0", state.uart, com = plane.connectors.com[0])
    bp.expand(name, "com1", state.uart, com = plane.connectors.com[1])

    # Add SATA connectors
    bp.expand(name, "sata0", state.sata, sata = plane.connectors.sata[0])
    bp.expand(name, "sata1", state.sata, sata = plane.connectors.sata[1])

    # Add USB connectors
    bp.expand(name, "usb0", state.usb, usb = plane.connectors.usb0)
    bp.expand(name, "usb1", state.usb, usb = plane.connectors.usb1)

    # Add Ethernet connector
    bp.expand(name, "eth0", state.eth, eth = plane.connectors.eth0)

    # Add PCIe slots
    for i in (1, 2, 3, 4, 5, 7):
        bp.expand(name, f"pcie{i}", state.pcie_slot,
            dev = 0, bus = plane.pcie_port[i])

    # Add GPU slot
    bp.expand(name, "gfx", state.gpu_slot,
        dev = 15, bus = plane.pci_bus, gfx = plane.connectors.gfx_console)

    # Add standard devices
    txt_con_conf = bp.create_config(name.com0.con, console.TextConsoleParams)
    txt_con_conf.visible = params.show_con
    bp.expand(name, "com0.con", console.text_console)
    gfx_con_conf = bp.create_config(name.gfx.console, console.GfxConsoleParams)
    gfx_con_conf.visible = params.show_gfx_con
    bp.expand(name, "gfx.console", console.gfx_console)
    bp.expand(name, "gfx.dev", accel_vga)

    bp.expand(name, "usb0.tablet", usb.tablet, gfx = plane.connectors.gfx_console)
