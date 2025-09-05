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

static const xml_reg_spec_t core[] = {
        {.bits=64, .name="rax"},
        {.bits=64, .name="rbx"},
        {.bits=64, .name="rcx"},
        {.bits=64, .name="rdx"},
        {.bits=64, .name="rsi"},
        {.bits=64, .name="rdi"},
        {.bits=64, .name="rbp", .xml_type="data_ptr"},
        {.bits=64, .name="rsp", .xml_type="data_ptr"},
        {.bits=64, .name="r8"},
        {.bits=64, .name="r9"},
        {.bits=64, .name="r10"},
        {.bits=64, .name="r11"},
        {.bits=64, .name="r12"},
        {.bits=64, .name="r13"},
        {.bits=64, .name="r14"},
        {.bits=64, .name="r15"},
        {.bits=64, .name="rip"},
        {.bits=32, .name="rflags", .xml_name="eflags"},
        {.bits=32, .name="cs"},
        {.bits=32, .name="ss"},
        {.bits=32, .name="ds"},
        {.bits=32, .name="es"},
        {.bits=32, .name="fs"},
        {.bits=32, .name="gs"},
        {.bits=80, .name="f0", .xml_name="st0", .xml_type="i387_ext"},
        {.bits=80, .name="f1", .xml_name="st1", .xml_type="i387_ext"},
        {.bits=80, .name="f2", .xml_name="st2", .xml_type="i387_ext"},
        {.bits=80, .name="f3", .xml_name="st3", .xml_type="i387_ext"},
        {.bits=80, .name="f4", .xml_name="st4", .xml_type="i387_ext"},
        {.bits=80, .name="f5", .xml_name="st5", .xml_type="i387_ext"},
        {.bits=80, .name="f6", .xml_name="st6", .xml_type="i387_ext"},
        {.bits=80, .name="f7", .xml_name="st7", .xml_type="i387_ext"},
        {.bits=32, .name="fcw", .xml_name="fctrl"},
        {.bits=32, .name="fsw", .xml_name="fstat"},
        {.bits=32, .name="ftw", .xml_name="ftag"},
        /* fi* and fo* registers could be extracted using
           x86_reg_access.get_fpu_env(), but should not be used in 64-bit.
           Mainly still there for 16- and 32-bit compatibility. */
        {.bits=32, .name="fiseg", .cls=regclass_unused},
        {.bits=32, .name="fioff", .cls=regclass_unused},
        {.bits=32, .name="foseg", .cls=regclass_unused},
        {.bits=32, .name="fooff", .cls=regclass_unused},
        {.bits=32, .name="fop", .cls=regclass_unused},
        {.name = NULL},
};

static const xml_reg_spec_t sse[] = {
        {.bits=128, .name="xmm0"},
        {.bits=128, .name="xmm1"},
        {.bits=128, .name="xmm2"},
        {.bits=128, .name="xmm3"},
        {.bits=128, .name="xmm4"},
        {.bits=128, .name="xmm5"},
        {.bits=128, .name="xmm6"},
        {.bits=128, .name="xmm7"},
        {.bits=128, .name="xmm8"},
        {.bits=128, .name="xmm9"},
        {.bits=128, .name="xmm10"},
        {.bits=128, .name="xmm11"},
        {.bits=128, .name="xmm12"},
        {.bits=128, .name="xmm13"},
        {.bits=128, .name="xmm14"},
        {.bits=128, .name="xmm15"},
        {.bits=32, .name = "mxcsr"},
        {.name = NULL},
};

static const xml_reg_group_t xml_groups[] = {
        { .name = "org.gnu.gdb.i386.core", .regs = core },
        { .name = "org.gnu.gdb.i386.sse", .regs = sse },
        { .name = NULL },
};


const gdb_arch_t gdb_arch_x86_64 = {
        .name = "x86-64",
        .arch_name = "i386:x86-64",
        .help = {
                .target_flag = "x86_64-pc-linux-gnu",
                .prompt_cmd = "set architecture i386:x86-64"
        },
        .is_be = false,
        .bit_extend = false,
        .regs.xml = xml_groups,
};
