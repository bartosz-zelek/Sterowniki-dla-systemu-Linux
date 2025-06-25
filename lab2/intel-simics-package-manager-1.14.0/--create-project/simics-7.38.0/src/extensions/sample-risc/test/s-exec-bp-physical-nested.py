# © 2012 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


import conf
from stest import *
import common

cpu = common.create_sample_risc()
core = cpu.current_risc_core

core.iface.processor_info_v2.set_program_counter(0x4000)
SIM_write_phys_memory(core, 0x4000, common.make_nop(), 4)
SIM_write_phys_memory(core, 0x4004, common.make_nop(), 4)
SIM_write_phys_memory(core, 0x4008, common.make_nop(), 4)
SIM_write_phys_memory(core, 0x400c, common.make_nop(), 4)
run_command("nested_mem0.bp-break-memory 0x4008 -x")
run_command("bp.step.break %s 4" % cpu.name)
SIM_continue(0)
common.check_step(cpu, 2)
common.check_pc(cpu, 0x4008)
common.check_got_break()
