# © 2017 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


import os
from configuration import OBJ, OBJECT
import simics
import stest

def create_sample_risc(num_cpus = 1, start_num = 0):
    cnf  = ([OBJECT("cpu%d" % i, os.getenv('SAMPLE_RISC_CLASS'),
                    queue = OBJ("cpu%d" % i),
                    freq_mhz = 1,
                    cell = OBJ("cell%d" % start_num),
                    current_risc_core = OBJ("cpu_core%d" % i))
             for i in range(start_num, start_num + num_cpus)] +

            [OBJECT("cpu_core%d" % i, os.getenv('SAMPLE_RISC_CORE_CLASS'),
                    queue = OBJ("cpu%d" % i),
                    sample_risc = OBJ("cpu%d" % i),
                    physical_memory_space = OBJ("phys_mem%d" % start_num))
             for i in range(start_num, start_num + num_cpus)] +

            [OBJECT("phys_mem%d" % start_num, "memory-space",
                    queue = OBJ("cpu%d" % start_num),
                    map = [[0x00000000, OBJ("nested_mem%d" % start_num),
                            0, 0, 0x100000000]])] +

            [OBJECT("nested_mem%d" % start_num, "memory-space",
                    queue = OBJ("cpu%d" % start_num),
                    map = [[0x00000000, OBJ("memory%d" % start_num),
                            0, 0, 0x100000000]])] +

            [OBJECT("memory%d" % start_num, "ram",
                    image = OBJ("memory%d" % start_num + "_image"))] +

            [OBJECT("memory%d" % start_num + "_image", "image",
                    queue = OBJ("cpu%d" % start_num),
                    size = 0x100000000)] +

            [OBJECT("cell%d" % start_num, "cell",
                    queue = OBJ("cpu%d" % start_num))])

    simics.SIM_set_configuration(cnf)
    return simics.SIM_get_object("cpu%d" % start_num)

def make_b(ofs):
    return (0x1000 << 16) | (ofs & 0xffff)

def make_nop():
    return 0

def make_branch_direct(imm):
    return (1 << 28) | imm

def make_branch_indirect(src):
    return (1 << 28) | (1 << 24) | (src << 20)

def make_load(src, dst):
    return (2 << 28) | (1 << 24) | (src << 20) | (dst << 16)

def make_store(src, dst):
    return (3 << 28) | (1 << 24) | (src << 20) | (dst << 16)

def make_magic(n):
    return (5 << 28)

def make_add(src, dst, imm):
    return (6 << 28) | (src << 20) | (dst << 16) | imm

def set_reg(core, reg_name, val):
    core.iface.int_register.write(core.iface.int_register.get_number(reg_name), val)

def get_reg(core, reg_name):
    return core.iface.int_register.read(core.iface.int_register.get_number(reg_name))

def check_step(cpu, steps):
    stest.expect_equal(simics.SIM_step_count(cpu), steps)

def check_cycle(cpu, cycles):
    stest.expect_equal(simics.SIM_cycle_count(cpu), cycles)

def check_pc(cpu, pc):
    core = cpu.current_risc_core
    stest.expect_equal(core.iface.processor_info_v2.get_program_counter(), pc)

def check_reg(core, reg_name, val):
    stest.expect_equal(get_reg(core, reg_name), val)

got_break = False
def bp_handler(data, obj, bp_num, memop):
    global got_break
    got_break = True
    print("Breakpoint triggered, breaking")
    simics.SIM_break_simulation("breakpoint")
simics.SIM_hap_add_callback("Core_Breakpoint_Memop", bp_handler, None)

def check_got_break():
    global got_break
    stest.expect_equal(got_break, True)
