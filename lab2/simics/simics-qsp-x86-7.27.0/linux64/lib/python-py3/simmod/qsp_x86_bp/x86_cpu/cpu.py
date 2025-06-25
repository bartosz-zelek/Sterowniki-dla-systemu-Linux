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


from blueprints import Builder, ConfObject, DefaultTarget, Namespace

import simics

def apic_common(bp: Builder, name: Namespace,
                phys_bcast_addr: int, version: int, apic_type: str):
    from .state import APICInterconnect
    ai = bp.read_state(name, APICInterconnect)
    ai.apic = bp.obj(name, "x2apic_v2",
        apic_id = ai.apic_id,
        apic_bus = ai.apic_bus,
        cpu = ai.cpu,
        cpu_bus_divisor = ai.cpu_bus_divisor,
        apic_type = apic_type,
        physical_broadcast_address = phys_bcast_addr,
    )
    bp.set(name.bank.apic_regs, Version = version)

def apic_p4(bp: Builder, name: Namespace):
    bp.expand(name, "", apic_common,
        phys_bcast_addr = 255, version = 0x14, apic_type = "P4")

def apic_p6(bp: Builder, name: Namespace):
    bp.expand(name, "", apic_common,
        phys_bcast_addr = 15, version = 0x18, apic_type = "P6")

def x86_cpu_finalize(cpu, apic, apic_id):
    import vmp_common
    if apic_id == 0:
        apic.iface.apic_cpu.power_on(1, 0)
    # Enable VMP
    vmp_common.setup_vmp(cpu, True, True, True)

def cpu_thread(bp: Builder, name: Namespace):
    from .state import CPUConfig, APICInterconnect, CPUThreadInterconnect
    # Obtain the 'x86-cpu-thread' context key
    thread = bp.read_state(name, CPUThreadInterconnect)
    apic = bp.expose_state(name, APICInterconnect)
    config = bp.get_config(name, CPUConfig)

    # shortcuts
    socket = thread.socket
    fabric = socket.fabric

    # Retrieve needed parameters
    if socket.socket_id == thread.core_id == thread.thread_id == 0:
        activity_state = simics.X86_Activity_Normal
    else:
        activity_state = simics.X86_Activity_Wait_For_SIPI

    def intel_cpu_name(model, freq_mhz, sku = ""):
        cpu_name = model + " " * (36 - len(model) - len(sku)) + sku
        return f"{cpu_name}  @ {freq_mhz / 1000.0:.2f}GHz"

    cpu_name = intel_cpu_name(config.model_name, config.freq_mhz)

    logical_processor_count = config.num_cores * config.num_threads
    is_logical = (thread.core_id == thread.thread_id == 0)

    # Pad the reserved APIC IDs for the core level to
    # represent 8 cores (for 16 logical CPUs in total).
    # FIXME: Verify that 0 is the appropriate default value
    apic_id_shift_count = 3 if config.num_cores <= 8 else 0

    # Provide APIC parameters
    apic.apic_id = thread.apic_id
    apic.apic_bus = fabric.apic_bus
    apic.cpu_bus_divisor = config.freq_mhz / config.apic_freq_mhz

    apic.cpu = bp.obj(name, config.cpu_class,
        tlb = name.tlb,
        apic = name.apic,
        activity_state = activity_state,
        cpuid_physical_apic_id = thread.apic_id,
        cpuid_logical_processor_count = logical_processor_count,
        cpuid_core_level_apic_id_shift_count = apic_id_shift_count,
        queue = name,
        physical_memory = name.mem,
        shared_physical_memory = fabric.mem_space,
        system = fabric.broadcast_bus,
        port_space = fabric.io_space,
        freq_mhz = config.freq_mhz,
        cell = fabric.cell,
        threads = thread.siblings,
        package_group = thread.package_group,
        current_context = fabric.context,
        cpuid_stepping = 8,
        cpuid_processor_name = cpu_name,
    )
    bp.obj(name.tlb, "x86-tlb", cpu = name)
    bp.obj(name.mem, "memory-space",
        map = [[0xfee00000, name.apic.bank.apic_regs, 0, 0, 0x4000]],
        default_target = DefaultTarget(fabric.mem_space),
    )
    bp.at_post_instantiate(name, x86_cpu_finalize,
        cpu=name,
        apic=name.apic,
        apic_id=thread.apic_id)
    bp.expand(name, "apic", config.apic_comp)

    cpu_apic_logical = (ConfObject(name), ConfObject(name.apic), is_logical)
    fabric.cpu_apic_logical_list.append(cpu_apic_logical)
    fabric.cpu_list.append(ConfObject(name))
    fabric.apic_list.append(ConfObject(name.apic))


# Generic CPU blueprint which adds a number of CPU thread blueprints
def generic_cpu(bp: Builder, name: Namespace):
    from .state import CPUConfig, CPUSocket, CPUThreadInterconnect
    bp.obj(name, 'namespace')
    socket = bp.read_state(name, CPUSocket)
    config = bp.get_config(name, CPUConfig)

    socket.first_cpu = ConfObject(name.core[0][0])

    # Obtain list with all CPUs in the system (needed for apic-ID assignment)
    all_cpus = list(socket.fabric.cpu_list)

    def get_apic_id(cpu):
        ind = all_cpus.index(cpu) if cpu in all_cpus else 0
        # reserve ID 8 and 9 are for ioapic/ioxapic
        return ind if ind < 8 else ind + 2

    n_threads = config.num_threads
    for c in range(config.num_cores):
        siblings = [ConfObject(name.core[c][i]) for i in range(n_threads)]
        for t in range(n_threads):
            thread = bp.expose_state(name.core[c][t], CPUThreadInterconnect)
            thread.core_id = c
            thread.thread_id = t
            thread.apic_id = get_apic_id(ConfObject(name.core[c][t]))
            thread.package_group = ConfObject(name.core[0][0])
            thread.siblings = siblings
            thread.socket = socket
            if config.cpu_thread_comp:
                bp.expand(name, f"core[{c}][{t}]", config.cpu_thread_comp)
