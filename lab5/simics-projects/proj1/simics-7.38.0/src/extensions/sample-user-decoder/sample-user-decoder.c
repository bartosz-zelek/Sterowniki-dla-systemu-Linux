/*
  sample-user-decoder.c - decoder extension module for adding a user decoder

  © 2010 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <simics/simulator-api.h>
#include <simics/model-iface/decoder.h>
#include <simics/util/hashtab.h>

/*
 * This module works with several CPU architectures, and uses
 * different encodings for different architectures.
 */
static struct {
        const uint8 arm_nop[4];
        const uint8 mips32_nop[4];
        const uint8 ppc32_nop[4];
        const uint8 v9_nop[4];
        const uint8 x86_nop[1];

        uint8 *user_nop_code;
        int user_nop_length;
} ss = {
        .arm_nop = { 0, 0, 0xa0, 0xe1 },
        .mips32_nop = { 0, 0, 0, 0 },
        .ppc32_nop = { 0x60, 0, 0, 0 },
        .v9_nop = { 0x01, 0, 0, 0 },
        .x86_nop = { 0x90 },
};

typedef struct nop_info {        
        int length;
        const uint8 *op_code;
} nop_info_t;

static exception_type_t
my_nop(conf_object_t *cpu, uint64 arg, void *user_data)
{
        SIM_printf("- Executing my nop (arg = %lld) -\n", arg);
        
        return Sim_PE_No_Exception;
}

/* This function is called when we are called from a new CPU. */
static nop_info_t
get_nop_info(conf_object_t *cpu)
{
        if (ss.user_nop_code) {
                return (nop_info_t){
                        .length = ss.user_nop_length,
                        .op_code = ss.user_nop_code,
                };
        }

        attr_value_t attr = SIM_get_attribute(cpu, "architecture");
        const char *arch = SIM_attr_string(attr);

        nop_info_t nop = { 0 };

        if (strcmp(arch, "ppc32") == 0 || strcmp(arch, "ppc64") == 0) {
                nop.op_code = ss.ppc32_nop;
                nop.length  = sizeof ss.ppc32_nop;
        } else if (strcmp(arch, "sparc-v9") == 0) {
                nop.op_code = ss.v9_nop;
                nop.length  = sizeof ss.v9_nop;
        } else if (strcmp(arch, "arm") == 0) {
                nop.op_code = ss.arm_nop;
                nop.length  = sizeof ss.arm_nop;
        } else if (strcmp(arch, "mips32") == 0) {
                nop.op_code = ss.mips32_nop;
                nop.length  = sizeof ss.mips32_nop;
        } else if (strcmp(arch, "x86") == 0) {
                nop.op_code = ss.x86_nop;
                nop.length  = sizeof ss.x86_nop;
        } else if (strcmp(arch, "x86-64") == 0) {
                nop.op_code = ss.x86_nop;
                nop.length  = sizeof ss.x86_nop;
        } else {
                SIM_printf(
                        "sample-user-decoder: Unknown CPU architecture: %s\n",
                        arch);
        }
        SIM_attr_free(&attr);
        return nop;
}

static int
my_decode(uint8 *code, int valid_bytes, conf_object_t *cpu,
          instruction_info_t *ii, void *data)
{
        nop_info_t nop = get_nop_info(cpu); 

        if (nop.length > valid_bytes)
                return -nop.length;

        if (memcmp(code, nop.op_code, nop.length) == 0) {
                SIM_printf("- Decoding my nop -\n");
                ii->ii_Type           = UD_IT_SEQUENTIAL;
                ii->ii_ServiceRoutine = my_nop;
                ii->ii_Arg            = 13;
                ii->ii_UserData       = 0;
                return nop.length;
        } else
                return 0;
}

static tuple_int_string_t
my_disassemble(uint8 *code, int valid_bytes, conf_object_t *cpu, void *data)
{
        nop_info_t nop = get_nop_info(cpu);

        if (nop.length > valid_bytes)
                return (tuple_int_string_t){-nop.length, NULL};

        if (memcmp(code, nop.op_code, nop.length) == 0) {
                return (tuple_int_string_t){nop.length, MM_STRDUP("my_nop")};
        } else {
                return (tuple_int_string_t){0, NULL};
        }
}

static int
my_flush(instruction_info_t *ii, void *data)
{
        if (ii->ii_ServiceRoutine == my_nop) {
                /* This is the time to dealloc ii->ii_UserData */
                SIM_printf("# Flushing my nop #\n");
                return 1;
        } else {
                /* Not mine */
                return 0;
        }
}

static attr_value_t
get_user_nop(conf_class_t *c)
{
        if (ss.user_nop_code) {
                attr_value_t ret = SIM_alloc_attr_list(ss.user_nop_length);
                for (int i = 0; i < ss.user_nop_length; i++) {
                        SIM_attr_list_set_item(&ret, i,
                                               SIM_make_attr_uint64(
                                                       ss.user_nop_code[i]));
                }
                return ret;
        } else {
                return SIM_make_attr_nil();
        }
}

static set_error_t
set_user_nop(conf_class_t *c, attr_value_t *val)
{
        uint8 *new_code;
        int i;

        if (SIM_attr_is_nil(*val)) {
                MM_FREE(ss.user_nop_code);
                ss.user_nop_code = NULL;
        } else {
                new_code = MM_ZALLOC(SIM_attr_list_size(*val), uint8);
                for (i = 0; i < SIM_attr_list_size(*val); i++) {
                        if (!SIM_attr_is_integer(SIM_attr_list_item(*val, i))
                            || SIM_attr_integer(SIM_attr_list_item(*val, i))
                            < 0
                            || SIM_attr_integer(SIM_attr_list_item(*val, i))
                            > 0xff) {
                                MM_FREE(new_code);
                                return Sim_Set_Illegal_Value;
                        }
                        new_code[i] = 
                                SIM_attr_integer(SIM_attr_list_item(*val, i));
                }
                MM_FREE(ss.user_nop_code);
                ss.user_nop_code = new_code;
                ss.user_nop_length = SIM_attr_list_size(*val);
        }
        
        return Sim_Set_Ok;
}

void
init_local()
{
        decoder_t my_decoder = {
                .decode = my_decode,
                .disassemble = my_disassemble,
                .flush = my_flush,
        };

        /* classes for which we have registered my_decoder */
        ht_int_table_t classes = HT_INT_NULL();

        /* register a user decoder on all cpu classes that 
           implement the decoder interface */
        object_iter_t it = SIM_object_iterator(NULL);
        for (conf_object_t *o = SIM_object_iterator_next(&it);
             o != NULL;
             o = SIM_object_iterator_next(&it)) {
                const decoder_interface_t *di =
                        SIM_c_get_interface(o, DECODER_INTERFACE);
                if (di) {
                        conf_class_t *c = SIM_object_class(o);

                        /* already installed for this class? */
                        if (!ht_lookup_int(&classes, (uint64)c)) {
                                ht_insert_int(&classes, (uint64)c, c);
                                di->register_decoder(o, &my_decoder);
                                pr("Registering a user decoder for the"
                                   " '%s' class.\n", SIM_get_class_name(c));
                        }
                }
        }
        ht_delete_int_table(&classes, false);

        const class_info_t sample_class = {
                .short_desc = "sample user instruction decoder",
                .description =
                "Sample user decoder showing how to decode an instruction"
                " and change its execution.",
        };
        conf_class_t *c = SIM_create_class("sample-user-decoder",
                                           &sample_class);
        SIM_register_class_attribute(c, "user_nop",
                                     get_user_nop,
                                     set_user_nop,
                                     Sim_Attr_Pseudo, "[i+]|n",
                                     "User specified instruction pattern"
                                     " that this sample decoder replaces"
                                     " with a no-op.");
}
