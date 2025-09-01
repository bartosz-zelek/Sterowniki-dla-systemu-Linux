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

/* For non arm-v8 models or older arm-v8 models. */
static const regspec_t armv7_array[] = {
        {32, "r0",   regclass_i},
        {32, "r1",   regclass_i},
        {32, "r2",   regclass_i},
        {32, "r3",   regclass_i},
        {32, "r4",   regclass_i},
        {32, "r5",   regclass_i},
        {32, "r6",   regclass_i},
        {32, "r7",   regclass_i},
        {32, "r8",   regclass_i},
        {32, "r9",   regclass_i},
        {32, "r10",  regclass_i},
        {32, "r11",  regclass_i},
        {32, "r12",  regclass_i},
        {32, "sp",   regclass_i},
        {32, "lr",   regclass_i},
        {32, "pc",   regclass_i_pc},
        {96, "f0",   regclass_unused},
        {96, "f1",   regclass_unused},
        {96, "f2",   regclass_unused},
        {96, "f3",   regclass_unused},
        {96, "f4",   regclass_unused},
        {96, "f5",   regclass_unused},
        {96, "f6",   regclass_unused},
        {96, "f7",   regclass_unused},
        {32, "fps",  regclass_unused},
        {32, "cpsr", regclass_i},
};

static const regs_specifier_t armv7_regs = {
        .array = armv7_array,
        .count = ALEN(armv7_array)
};


/* For aarch32 mode in ARM-v8 models */
static const regspec_t aarch32_array[] = {
        {32, "r0",   regclass_i},
        {32, "r1",   regclass_i},
        {32, "r2",   regclass_i},
        {32, "r3",   regclass_i},
        {32, "r4",   regclass_i},
        {32, "r5",   regclass_i},
        {32, "r6",   regclass_i},
        {32, "r7",   regclass_i},
        {32, "r8",   regclass_i},
        {32, "r9",   regclass_i},
        {32, "r10",  regclass_i},
        {32, "r11",  regclass_i},
        {32, "r12",  regclass_i},
        {32, "sp_aarch32", regclass_i},
        {32, "lr_aarch32", regclass_i},
        {32, "pc_aarch32", regclass_i_pc},
        {96, "f0",   regclass_unused},
        {96, "f1",   regclass_unused},
        {96, "f2",   regclass_unused},
        {96, "f3",   regclass_unused},
        {96, "f4",   regclass_unused},
        {96, "f5",   regclass_unused},
        {96, "f6",   regclass_unused},
        {96, "f7",   regclass_unused},
        {32, "fps",  regclass_unused},
        {32, "cpsr", regclass_i},
};

static const regs_specifier_t aarch32_regs = {
        .array = aarch32_array,
        .count = ALEN(aarch32_array)
};

static const regs_specifier_t *
select_regs(gdb_remote_t *gdb, conf_object_t *cpu, const gdb_arch_t *arch)
{
        if (has_int_register_name(gdb, cpu, "sp_aarch32"))
                return &aarch32_regs;
        return &armv7_regs;
}

const gdb_arch_t gdb_arch_armle = {
        .name = "armle",
        .arch_name = "arm",
        .help = {
                .target_flag = "armle-unknown-linux-gnu",
                .prompt_cmd = NULL,
        },
        .is_be = false,
        .regs_selector = select_regs,
        .nonsteppable_watchpoint = true,
};
