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

from blueprints import State, Config, ConfObject, BlueprintFun
from .cpu import cpu_thread, apic_p4

class APICInterconnect(State):
    apic_id = -1
    cpu_bus_divisor = 1.0
    apic_bus = ConfObject()
    cpu = ConfObject()
    apic = ConfObject()

class CPUConfig(Config):
    num_cpus = 1
    num_cores = 1
    num_threads = 1
    freq_mhz = 2000
    cpu_class = "x86-nehalem"
    model_name = "Intel(R) Core(TM) i7 CPU"
    apic_freq_mhz = 133
    # Blueprint customization
    apic_comp = BlueprintFun(apic_p4)
    cpu_thread_comp = BlueprintFun(cpu_thread)
    cpu_comp = BlueprintFun()

class CPUFabric(State):
    apic_bus = ConfObject()
    mem_space = ConfObject()
    broadcast_bus = ConfObject()
    io_space = ConfObject()
    cell = ConfObject()
    context = ConfObject()
    cpu_apic_logical_list: list[tuple[ConfObject, ConfObject, bool]] = []
    "List with element [cpu, apic, is_logical]. Populated by CPU blueprint."
    cpu_list: list[ConfObject] = []
    "List with all CPUs. Populated by the CPU blueprint."
    apic_list: list[ConfObject] = []
    "List with all APICs. Populated by the CPU bluieprint."

class CPUSocket(State):
    fabric = CPUFabric()
    config = CPUConfig()
    socket_id = -1
    first_cpu = ConfObject()

class CPUThreadInterconnect(State):
    socket = CPUSocket()
    thread_id = -1
    core_id = -1
    apic_id = -1
    siblings: list[ConfObject] = []
    package_group = ConfObject()
