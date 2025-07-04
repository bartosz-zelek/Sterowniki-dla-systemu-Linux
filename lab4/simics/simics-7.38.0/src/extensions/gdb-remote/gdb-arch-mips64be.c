/*
  © 2010 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include "gdb-remote.h"

static const regspec_t regs[] = {
        {64, "zero", regclass_i},
        {64, "at", regclass_i},
        {64, "v0", regclass_i},
        {64, "v1", regclass_i},
        {64, "a0", regclass_i},
        {64, "a1", regclass_i},
        {64, "a2", regclass_i},
        {64, "a3", regclass_i},
        {64, "t0", regclass_i},
        {64, "t1", regclass_i},
        {64, "t2", regclass_i},
        {64, "t3", regclass_i},
        {64, "t4", regclass_i},
        {64, "t5", regclass_i},
        {64, "t6", regclass_i},
        {64, "t7", regclass_i},
        {64, "s0", regclass_i},
        {64, "s1", regclass_i},
        {64, "s2", regclass_i},
        {64, "s3", regclass_i},
        {64, "s4", regclass_i},
        {64, "s5", regclass_i},
        {64, "s6", regclass_i},
        {64, "s7", regclass_i},
        {64, "t8", regclass_i},
        {64, "t9", regclass_i},
        {64, "k0", regclass_i},
        {64, "k1", regclass_i},
        {64, "gp", regclass_i},
        {64, "sp", regclass_i},
        {64, "fp", regclass_i},
        {64, "ra", regclass_i},
        {32, "status", regclass_i},
        {32, "dummy0", regclass_unused},
        {64, "lo", regclass_i},
        {64, "hi", regclass_i},
        {64, "badvaddr", regclass_i},
        {64, "cause", regclass_i},
        {64, "pc", regclass_i_pc},
        {64, "f0", regclass_unused},
        {64, "f1", regclass_unused},
        {64, "f2", regclass_unused},
        {64, "f3", regclass_unused},
        {64, "f4", regclass_unused},
        {64, "f5", regclass_unused},
        {64, "f6", regclass_unused},
        {64, "f7", regclass_unused},
        {64, "f8", regclass_unused},
        {64, "f9", regclass_unused},
        {64, "f10", regclass_unused},
        {64, "f11", regclass_unused},
        {64, "f12", regclass_unused},
        {64, "f13", regclass_unused},
        {64, "f14", regclass_unused},
        {64, "f15", regclass_unused},
        {64, "f16", regclass_unused},
        {64, "f17", regclass_unused},
        {64, "f18", regclass_unused},
        {64, "f19", regclass_unused},
        {64, "f20", regclass_unused},
        {64, "f21", regclass_unused},
        {64, "f22", regclass_unused},
        {64, "f23", regclass_unused},
        {64, "f24", regclass_unused},
        {64, "f25", regclass_unused},
        {64, "f26", regclass_unused},
        {64, "f27", regclass_unused},
        {64, "f28", regclass_unused},
        {64, "f29", regclass_unused},
        {64, "f30", regclass_unused},
        {64, "f31", regclass_unused},
        {32, "fcsr", regclass_unused},
        {32, "fir", regclass_unused},
        {32, "ffp", regclass_unused},
        {64, "inx", regclass_unused},
        {64, "rand", regclass_unused},
        {64, "elo", regclass_unused},
        {64, "ctxt", regclass_unused},
        {64, "ehi", regclass_unused},
        {64, "epc", regclass_i},
        {64, "prid", regclass_i},
};

const gdb_arch_t gdb_arch_mips64be = {
        .name = "mips64be",
        .arch_name = "mips:isa64r2",
        .help = {
                .target_flag = "mips64-elf64-linux64",
                .prompt_cmd = "set architecture mips:isa64r2",
        },
        .is_be = true,
        .bit_extend = true,
        .regs = {
                .array = regs,
                .count = ALEN(regs)
        },
        .nonsteppable_watchpoint = true,
};
