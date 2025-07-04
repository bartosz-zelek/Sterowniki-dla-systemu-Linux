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
        {64, "r0",  regclass_i},
        {64, "r1",  regclass_i},
        {64, "r2",  regclass_i},
        {64, "r3",  regclass_i},
        {64, "r4",  regclass_i},
        {64, "r5",  regclass_i},
        {64, "r6",  regclass_i},
        {64, "r7",  regclass_i},
        {64, "r8",  regclass_i},
        {64, "r9",  regclass_i},
        {64, "r10", regclass_i},
        {64, "r11", regclass_i},
        {64, "r12", regclass_i},
        {64, "r13", regclass_i},
        {64, "r14", regclass_i},
        {64, "r15", regclass_i},
        {64, "r16", regclass_i},
        {64, "r17", regclass_i},
        {64, "r18", regclass_i},
        {64, "r19", regclass_i},
        {64, "r20", regclass_i},
        {64, "r21", regclass_i},
        {64, "r22", regclass_i},
        {64, "r23", regclass_i},
        {64, "r24", regclass_i},
        {64, "r25", regclass_i},
        {64, "r26", regclass_i},
        {64, "r27", regclass_i},
        {64, "r28", regclass_i},
        {64, "r29", regclass_i},
        {64, "r30", regclass_i},
        {64, "r31", regclass_i},
        {64, "f0",  regclass_i_opt},
        {64, "f1",  regclass_i_opt},
        {64, "f2",  regclass_i_opt},
        {64, "f3",  regclass_i_opt},
        {64, "f4",  regclass_i_opt},
        {64, "f5",  regclass_i_opt},
        {64, "f6",  regclass_i_opt},
        {64, "f7",  regclass_i_opt},
        {64, "f8",  regclass_i_opt},
        {64, "f9",  regclass_i_opt},
        {64, "f10", regclass_i_opt},
        {64, "f11", regclass_i_opt},
        {64, "f12", regclass_i_opt},
        {64, "f13", regclass_i_opt},
        {64, "f14", regclass_i_opt},
        {64, "f15", regclass_i_opt},
        {64, "f16", regclass_i_opt},
        {64, "f17", regclass_i_opt},
        {64, "f18", regclass_i_opt},
        {64, "f19", regclass_i_opt},
        {64, "f20", regclass_i_opt},
        {64, "f21", regclass_i_opt},
        {64, "f22", regclass_i_opt},
        {64, "f23", regclass_i_opt},
        {64, "f24", regclass_i_opt},
        {64, "f25", regclass_i_opt},
        {64, "f26", regclass_i_opt},
        {64, "f27", regclass_i_opt},
        {64, "f28", regclass_i_opt},
        {64, "f29", regclass_i_opt},
        {64, "f30", regclass_i_opt},
        {64, "f31", regclass_i_opt},
        {64, "pc",  regclass_i_pc},
        {64, "msr", regclass_i_opt},
        {32, "cr",  regclass_i},
        {64, "lr",  regclass_i},
        {64, "ctr", regclass_i},
        {32, "xer", regclass_i},
        {32, "mq",  regclass_unused},
        {32, "fpscr", regclass_i_opt}
};

const gdb_arch_t gdb_arch_ppc64 = {
        .name = "ppc64",
        .arch_name = "powerpc:common64",
        .help = {
                .target_flag = "powerpc64-elf-linux",
                .prompt_cmd = "set architecture powerpc:common64"
        },
        .is_be = true,
        .regs = {
                .array = regs,
                .count = ALEN(regs)
        }
};
