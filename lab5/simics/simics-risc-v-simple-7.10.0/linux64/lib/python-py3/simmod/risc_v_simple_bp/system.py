# © 2022 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


from blueprints import Builder, Namespace, blueprint, Config
from blueprints.simtypes import Port, MemMap
from blueprints.params import params_from_config, Param, ParamGroup
from .state import RISCVConfig
from .cpu import CPUFabric, riscv_cpu
from simmod.std_bp import console, state
import simics

# Trivial RISC-V system, instantiating a generic RISC-V CPU blueprint
@blueprint(params_from_config(RISCVConfig),
           "riscv_simple_board")
def simple_system(bp: Builder, name: Namespace, config: RISCVConfig):
    fabric = bp.expose_state(name.cpu, CPUFabric)
    plane = fabric.bp

    # Make sure the queue is set for all objects
    bp.obj(name, "namespace", queue=plane.queue)

    if config.has_clic:
        if not simics.SIM_get_class_interface(
                config.core_class, "riscv_clic_interrupt"):
            bp.error(f"{config.core_class} foes not support CLIC")
        else:
            mem_map = []
            for i in range(len(fabric.hart)):
                bp.obj(name.clic[i], "sifive-clic-e2x", hart=fabric.hart[i])
                mem_map.append(MemMap(0x1000 * i, name.clic[i].bank.regs, 0, 0, 0x1000))
            bp.obj(name.clic_mem, "memory-space", map=mem_map)

    bp.expand(name, "cpu", riscv_cpu)

    bp.obj(name.plic, "riscv-plic",
        max_interrupt=127,
        max_priority=7,
        hart=(fabric.clic if config.has_clic else fabric.hart))

    boot_size = 0x1000
    ram_size = config.memory_megs * (1 << 20)
    bp.obj(name.boot_image, "image", size=boot_size)
    bp.obj(name.boot, "ram", image=name.boot_image)
    bp.obj(name.ram_image, "image", size=ram_size)
    bp.obj(name.ram, "ram", image=name.ram_image)
    bp.obj(name.cell, "cell")
    # Make sure all objects gets the same cell
    fabric.cell = name.cell
    plane.com.uart = bp.obj(name.uart, "NS16550",
        irq_dev=Port(name.plic, "IRQ[1]"),
        console=plane.com.remote)

    phys_mem_map =([
        MemMap(0x00001000, name.boot, 0, 0, boot_size),
        MemMap(0x02000000, Port(name.cpu.clint, 'regs'), 0, 0, 0xc000),
        MemMap(0x0c000000, Port(name.plic, 'regs'), 0, 0, 0x4000000),
        MemMap(0x10000000, name.uart, 0, 0, 0x11),
        MemMap(0x80000000, name.ram, 0, 0, ram_size),
    ] + ([MemMap(0x02800000, name.clic_mem, 0, 0, 0x1ff0001)]
         if config.has_clic else []))
    fabric.mem_space = bp.obj(name.phys_mem, "memory-space", map=phys_mem_map)

    bp.expand(name, "com", state.uart, com=plane.com)
    if config.create_con:
        bp.expand(name, "com.con", console.text_console)

    plane.processors = [str(c) for c in fabric.hart]

class RISCVParams(Config):
    board_name = "board"

@blueprint([Param("board_name", "Board name", RISCVParams),
            ParamGroup("riscv", "", import_bp="riscv_simple_board")],
            name="riscv_simple_system")
def riscv_simple_system(bp: Builder, name: Namespace, config: RISCVParams):
    bp.expand(name, config.board_name, simple_system)
