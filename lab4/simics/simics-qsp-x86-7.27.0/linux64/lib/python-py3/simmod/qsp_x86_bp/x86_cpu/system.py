# © 2019 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


from blueprints import Builder, Namespace
from .cpu import generic_cpu
from .state import CPUSocket

MB = 0x100000

# Trivial x86 system, instantiating a generic x86 CPU blueprint
def simple_system(bp: Builder, name: Namespace, memory_megs=1024):
    socket = bp.expose_state(name.cpu, CPUSocket)
    fabric = socket.fabric

    socket.socket_id = 0

    ram_size = memory_megs * MB

    bp.obj(name, "namespace", queue = socket.first_cpu)
    bp.expand(name, "cpu", generic_cpu)
    fabric.mem_space = bp.obj(name.mem, "memory-space", map = [[0, name.ram, 0, 0, ram_size]])
    fabric.io_space = bp.obj(name.io, "memory-space")
    fabric.cell = bp.obj(name.cell, "cell")
    fabric.apic_bus = bp.obj(name.apic_bus, "apic-bus", apics = fabric.apic_list, ioapic = None)
    fabric.broadcast_bus = bp.obj(name.bcast, "x86_broadcast", cpus = fabric.cpu_list, images = [])
    bp.obj(name.ram, "ram", image = name.ram.image)
    bp.obj(name.ram.image, "image", size = ram_size)
