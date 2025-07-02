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


from blueprints import Builder, Namespace, ConfObject
from .state import CPUFabric, RISCVConfig

def riscv_cpu(bp: Builder, name: Namespace,
              fabric: CPUFabric, config: RISCVConfig):
    # Add secondary core
    hart_id = 0
    if config.s_core:
        if config.core_class == 'riscv-u54':
            bp.obj(name.s_core, 'riscv-s54',
                freq_mhz=config.freq_mhz,
                physical_memory=fabric.mem_space,
                clint=name.clint,
                cell=fabric.cell,
                queue=name.s_core,
                mhartid=hart_id)
            hart_id += 1
            fabric.hart.extend(ConfObject(name.s_core))
        else:
            bp.error("Only riscv-u54 cores support a secondary core")

    # Add all cores
    for i in range(config.num_cores):
        bp.obj(name.core[i], config.core_class,
            freq_mhz=config.freq_mhz,
            physical_memory=fabric.mem_space,
            clint=name.clint,
            queue=name.core[i],
            cell=fabric.cell,
            mhartid=hart_id)
        hart_id += 1
        fabric.hart.extend(ConfObject(name.core[i]))

    # Provide queue to rest of system
    fabric.bp.queue = ConfObject(name.core[0])

    # Add clint object
    bp.obj(name.clint, 'riscv-clint',
        freq_mhz=config.timebase_freq_mhz,
        queue=name.core[0],
        hart=(fabric.clic if config.has_clic else fabric.hart))
