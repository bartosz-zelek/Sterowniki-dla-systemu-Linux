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
        {32, "r0",  regclass_i32l},
        {32, "r1",  regclass_i32l},
        {32, "r2",  regclass_i32l},
        {32, "r3",  regclass_i32l},
        {32, "r4",  regclass_i32l},
        {32, "r5",  regclass_i32l},
        {32, "r6",  regclass_i32l},
        {32, "r7",  regclass_i32l},
        {32, "r8",  regclass_i32l},
        {32, "r9",  regclass_i32l},
        {32, "r10", regclass_i32l},
        {32, "r11", regclass_i32l},
        {32, "r12", regclass_i32l},
        {32, "r13", regclass_i32l},
        {32, "r14", regclass_i32l},
        {32, "r15", regclass_i32l},
        {32, "r16", regclass_i32l},
        {32, "r17", regclass_i32l},
        {32, "r18", regclass_i32l},
        {32, "r19", regclass_i32l},
        {32, "r20", regclass_i32l},
        {32, "r21", regclass_i32l},
        {32, "r22", regclass_i32l},
        {32, "r23", regclass_i32l},
        {32, "r24", regclass_i32l},
        {32, "r25", regclass_i32l},
        {32, "r26", regclass_i32l},
        {32, "r27", regclass_i32l},
        {32, "r28", regclass_i32l},
        {32, "r29", regclass_i32l},
        {32, "r30", regclass_i32l},
        {32, "r31", regclass_i32l},
        {32, "r0",  regclass_i32h},
        {32, "r1",  regclass_i32h},
        {32, "r2",  regclass_i32h},
        {32, "r3",  regclass_i32h},
        {32, "r4",  regclass_i32h},
        {32, "r5",  regclass_i32h},
        {32, "r6",  regclass_i32h},
        {32, "r7",  regclass_i32h},
        {32, "r8",  regclass_i32h},
        {32, "r9",  regclass_i32h},
        {32, "r10", regclass_i32h},
        {32, "r11", regclass_i32h},
        {32, "r12", regclass_i32h},
        {32, "r13", regclass_i32h},
        {32, "r14", regclass_i32h},
        {32, "r15", regclass_i32h},
        {32, "r16", regclass_i32h},
        {32, "r17", regclass_i32h},
        {32, "r18", regclass_i32h},
        {32, "r19", regclass_i32h},
        {32, "r20", regclass_i32h},
        {32, "r21", regclass_i32h},
        {32, "r22", regclass_i32h},
        {32, "r23", regclass_i32h},
        {32, "r24", regclass_i32h},
        {32, "r25", regclass_i32h},
        {32, "r26", regclass_i32h},
        {32, "r27", regclass_i32h},
        {32, "r28", regclass_i32h},
        {32, "r29", regclass_i32h},
        {32, "r30", regclass_i32h},
        {32, "r31", regclass_i32h},
        {32, "pc",      regclass_i_pc},
        {32, "msr",     regclass_i_opt},
        {32, "cr",      regclass_i},
        {32, "lr",      regclass_i},
        {32, "ctr",     regclass_i},
        {32, "xer",     regclass_i},
        {64, "acc",     regclass_i_opt},
        {32, "spefscr", regclass_i_opt},
};

const gdb_arch_t gdb_arch_ppce500 = {
        .name = "ppce500",
        .arch_name = "powerpc:e500",
        .help = {
                .target_flag = "powerpc64-elf-linux",
                .prompt_cmd = "set architecture powerpc:e500"
        },
        .is_be = true,
        .regs = {
                .array = regs,
                .count = ALEN(regs)
        }
};
