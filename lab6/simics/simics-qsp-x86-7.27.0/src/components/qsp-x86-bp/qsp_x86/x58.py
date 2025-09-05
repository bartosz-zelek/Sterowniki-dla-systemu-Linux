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
    Builder, DefaultTarget, MemMap, Namespace,
    Port, SignalPort, ConfObject)
from .state import X58Backplane
from simmod.std_bp.state import PCIEBus, PCIEDevice
from ..x86_cpu.state import CPUSocket, CPUConfig
from ..x86_cpu.cpu import generic_cpu

# x58 QPI devices
def qpi_devices(bp: Builder, name: Namespace, socket_id: int):
    plane = bp.read_state(name, X58Backplane)
    bp.obj(name, 'namespace')
    bp.obj(name.socket[socket_id], "namespace")
    bp.obj(name.socket[socket_id].qpi_arch, "x58-qpi-arch",
        socket_id = socket_id,
        cfg_space = plane.pci_bus.bus.resolve().cfg_space
    )
    bp.obj(name.socket[socket_id].pcie_bus, "pcie-downstream-port",
        transparent_enabled = True,
        devices = [[0, name.socket[socket_id].qpi_arch]]
    )
    plane.pci_bus.pcie_devices.append(name.socket[socket_id].pcie_bus.port.downstream)

# Thin blueprint which just configures the generic CPU as a QSP CPU
def qsp_cpu(bp: Builder, name: Namespace):
    bp.expand(name, "", generic_cpu)

def cpu_sockets(bp: Builder, name: Namespace):
    plane = bp.read_state(name, X58Backplane)

    config = bp.get_config(name, CPUConfig)
    fabric = plane.fabric
    fabric.broadcast_bus = bp.obj(name.bcast, "x86_broadcast",
        cpus = plane.fabric.cpu_list,
        images = []
    )
    INIT_signals = [Port(o.obj, "INIT") for o in plane.fabric.cpu_list]
    RESET_signals = [Port(o.obj, "RESET") for o in plane.fabric.cpu_list]

    bp.obj(name.cpusig.INIT, "signal-bus", targets = INIT_signals)
    bp.obj(name.cpusig.RESET, "signal-bus", targets = RESET_signals)

    # special object used by the i8042 to manage CPU INIT and A20 pins
    bp.obj(name.i8042_reset, "x86-reset-bus",
        reset_targets = plane.fabric.cpu_list,
    )
    plane.CORERST_tgts.append(SignalPort(name.cpusig.RESET))
    plane.i8042_reset = SignalPort(name.i8042_reset)
    plane.cpu_INIT = SignalPort(name.cpusig.INIT)

    for i in range(config.num_cpus):
        socket = bp.expose_state(name.cpu@i, CPUSocket)
        # Use a local key for this socket
        socket.socket_id = i
        # Make fabric and config available in 'socket'
        socket.fabric = plane.fabric
        if config.cpu_comp:
            bp.expand(name, f"cpu{i}", config.cpu_comp)
        bp.expand(name, f"cpu{i}.qpi", qpi_devices, socket_id = i)

        # Use first cpu as queue
        if i == socket.socket_id == 0:
            plane.queue.queue = socket.first_cpu

def pcie_port(bp: Builder, name: Namespace,
              device_id: int, lane_width: int, sec_bus: PCIEBus):
    plane = bp.read_state(name, X58Backplane)

    link_status = (lane_width << 4) | 1
    link_cap = (lane_width << 4) | 0x393c01

    bp.obj(name, "x58_pcie_port",
        pci_config_device_id = device_id,
        pci_bus = plane.pci_bus.bus,
        upstream_target = plane.pci_bus.bus,
        secondary_bus = name.bus,
        pci_config_exp_link_status = link_status,
        pci_config_exp_link_cap = link_cap,
    )
    sec_bus.bus = bp.obj(name.bus, "pcie-bus",
        bridge = name,
        upstream_target = name,
        conf_space = name.bus.conf,
        memory_space = name.bus.mem,
        io_space = name.bus.io,
        pci_devices = sec_bus.devices,
    )
    sec_bus.io = bp.obj(name.bus.io, "memory-space", map = sec_bus.io_map)
    sec_bus.mem = bp.obj(name.bus.mem, "memory-space", map = sec_bus.mem_map)
    bp.obj(name.bus.conf, "memory-space")

def x58(bp: Builder, name: Namespace):
    plane = bp.read_state(name, X58Backplane)

    bp.obj(name, "namespace")

    def is_rom_mapping(x: MemMap):
        if x.base < 0xc0000 or x.base >= 0x100000:
            return False
        if x.base + x.length == 0x100000:
            return False
        return True
    mmap = plane.pci_bus.mem_map
    pci_mem_not_rom = [x for x in mmap if not is_rom_mapping(x)]
    pci_mem_rom = [x for x in mmap if is_rom_mapping(x)]

    plane.shadow_mem_map.extend(pci_mem_rom)

    bp.obj(name.pci_bus, "pcie-downstream-port-legacy",
        memory_space = name.pci_bus.mem_space,
        pci_devices = plane.pci_bus.devices,
        devices = plane.pci_bus.pcie_devices,
        upstream_target = name.bridge,
        queue=plane.queue.queue,
    )
    bp.set(name.pci_bus.mem_space, map = pci_mem_not_rom)
    plane.CORERST_tgts.append(SignalPort(name.pci_bus.port.HOT_RESET))

    plane.pci_bus.bus = ConfObject(name.pci_bus)
    plane.pci_bus.mem = name.pci_bus.mem_space
    plane.pci_bus.io = ConfObject(name.pci_bus.io_space)

    bp.set(plane.pci_bus.io.resolve(), map = plane.pci_bus.io_map)
    bp.set(plane.pci_bus.io.resolve(), default_target = DefaultTarget(plane.sb_pci_io))

    for index, width in [[1, 2], [2, 2], [3, 4], [4, 4], [5, 8], [7, 16]]:
        bp.obj(name.pcie_p@index, "x58-pcie-port",
            port_index = index,
            link_width = width,
            queue=plane.queue.queue,
        )
        downstream_port = getattr(name, f'pcie_p{index}').downstream_port
        bp.set(downstream_port, pci_devices=plane.pcie_port[index].devices)
        bp.set(downstream_port, devices=plane.pcie_port[index].pcie_devices)
        plane.pcie_port[index].bus = downstream_port

    bp.obj(name.core, "x58-core",
        queue=plane.queue.queue
    )
    bp.obj(name.bridge, "x58-dmi",
        default_remapping_unit = name.core.remap_unit[0],
        downstream_target = name.pci_bus.port.downstream,
        queue=plane.queue.queue,
        interrupt = plane.lpc_obj,
    )

    for i in range(2):
        bp.set(name.core.remap_unit[i], apic_bus = plane.fabric.apic_bus)
        bp.set(name.core.remap_unit[i], memory_space = plane.dram_space)

    plane.system_io_port_map.extend([
        MemMap(0xcf8, name.bridge.bank.io_regs, 0, 0xcf8, 4),
        MemMap(0xcfc, name.bridge.bank.io_regs, 0, 0xcfc, 4),
        MemMap(0xcfd, name.bridge.bank.io_regs, 0, 0xcfd, 2),
        MemMap(0xcfe, name.bridge.bank.io_regs, 0, 0xcfe, 2),
        MemMap(0xcff, name.bridge.bank.io_regs, 0, 0xcff, 1),
    ])

    bp.obj(name.qpi_port0, 'x58-qpi-port', queue=plane.queue.queue)
    bp.obj(name.qpi_port1, 'x58-qpi-port', queue=plane.queue.queue)
    bp.set(name.qpi_port0.bank.pcie_config[0], device_id = 0x3425)
    bp.set(name.qpi_port0.bank.pcie_config[1], device_id = 0x3426)
    bp.set(name.qpi_port1.bank.pcie_config[0], device_id = 0x3427)
    bp.set(name.qpi_port1.bank.pcie_config[1], device_id = 0x3428)

    bp.obj(name.ioxapic, 'x58-ioxapic', queue=plane.queue.queue)
    bp.set(name.ioxapic.ioapic, apic_bus = plane.fabric.apic_bus)

    plane.pci_bus.pcie_devices.extend([
        PCIEDevice(0, 0, name.bridge),
        PCIEDevice(1, 0, name.pcie_p1),
        PCIEDevice(2, 0, name.pcie_p2),
        PCIEDevice(3, 0, name.pcie_p3),
        PCIEDevice(4, 0, name.pcie_p4),
        PCIEDevice(5, 0, name.pcie_p5),
        PCIEDevice(7, 0, name.pcie_p7),
        PCIEDevice(16, 0, name.qpi_port0),
        PCIEDevice(17, 0, name.qpi_port1),
        PCIEDevice(19, 0, name.ioxapic),
        PCIEDevice(20, 0, name.core),
    ])
