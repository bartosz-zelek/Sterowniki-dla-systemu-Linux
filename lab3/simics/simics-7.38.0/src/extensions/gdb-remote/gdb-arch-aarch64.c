/*
  © 2016 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include "gdb-remote.h"

static const regspec_t armv8_regs_array_old_naming[] = {
        {64, "x0",         regclass_i},
        {64, "x1",         regclass_i},
        {64, "x2",         regclass_i},
        {64, "x3",         regclass_i},
        {64, "x4",         regclass_i},
        {64, "x5",         regclass_i},
        {64, "x6",         regclass_i},
        {64, "x7",         regclass_i},
        {64, "x8",         regclass_i},
        {64, "x9",         regclass_i},
        {64, "x10",        regclass_i},
        {64, "x11",        regclass_i},
        {64, "x12",        regclass_i},
        {64, "x13",        regclass_i},
        {64, "x14",        regclass_i},
        {64, "x15",        regclass_i},
        {64, "x16",        regclass_i},
        {64, "x17",        regclass_i},
        {64, "x18",        regclass_i},
        {64, "x19",        regclass_i},
        {64, "x20",        regclass_i},
        {64, "x21",        regclass_i},
        {64, "x22",        regclass_i},
        {64, "x23",        regclass_i},
        {64, "x24",        regclass_i},
        {64, "x25",        regclass_i},
        {64, "x26",        regclass_i},
        {64, "x27",        regclass_i},
        {64, "x28",        regclass_i},
        {64, "x29",        regclass_i},
        {64, "x30",        regclass_i},
        {64, "aarch64_sp", regclass_i},
        {64, "aarch64_pc", regclass_i_pc},
        {32, "cpsr",       regclass_i},
};

static const regs_specifier_t regs_old_naming = {
                .array = armv8_regs_array_old_naming,
                .count = ALEN(armv8_regs_array_old_naming)
};

static const regspec_t armv8_regs_array[] = {
        {64, "x0",         regclass_i},
        {64, "x1",         regclass_i},
        {64, "x2",         regclass_i},
        {64, "x3",         regclass_i},
        {64, "x4",         regclass_i},
        {64, "x5",         regclass_i},
        {64, "x6",         regclass_i},
        {64, "x7",         regclass_i},
        {64, "x8",         regclass_i},
        {64, "x9",         regclass_i},
        {64, "x10",        regclass_i},
        {64, "x11",        regclass_i},
        {64, "x12",        regclass_i},
        {64, "x13",        regclass_i},
        {64, "x14",        regclass_i},
        {64, "x15",        regclass_i},
        {64, "x16",        regclass_i},
        {64, "x17",        regclass_i},
        {64, "x18",        regclass_i},
        {64, "x19",        regclass_i},
        {64, "x20",        regclass_i},
        {64, "x21",        regclass_i},
        {64, "x22",        regclass_i},
        {64, "x23",        regclass_i},
        {64, "x24",        regclass_i},
        {64, "x25",        regclass_i},
        {64, "x26",        regclass_i},
        {64, "x27",        regclass_i},
        {64, "x28",        regclass_i},
        {64, "x29",        regclass_i},
        {64, "x30",        regclass_i},
        {64, "sp",         regclass_i},
        {64, "pc",      regclass_i_pc},
        {32, "cpsr",       regclass_i},
};

static const regs_specifier_t regs = {
                .array = armv8_regs_array,
                .count = ALEN(armv8_regs_array)
};

static const regs_specifier_t *
select_regs(gdb_remote_t *gdb, conf_object_t *cpu, const gdb_arch_t *arch)
{

        if (has_int_register_name(gdb, cpu, "aarch64_pc"))
                return &regs_old_naming;
        return &regs;
}


const gdb_arch_t gdb_arch_aarch64 = {
        .name = "arm64",
        .arch_name = "aarch64",
        .help = {
                .target_flag = "aarch64-elf",
                .prompt_cmd = NULL,
        },
        .is_be = false,
        .regs_selector = select_regs,
        .nonsteppable_watchpoint = true,
};
