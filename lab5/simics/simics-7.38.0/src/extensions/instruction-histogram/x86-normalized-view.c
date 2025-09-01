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

#include "x86-normalized-view.h"
#include "instruction-histogram.h"
#include <simics/model-iface/processor-info.h>
#include <simics/util/hashtab.h>
#include <simics/simulator/conf-object.h>
#include <simics/base/log.h>
#include <ctype.h>

#define COUNTERS_IN_BUCKET 256

/* Bucket of counters */
typedef struct count_bucket {
        uint64 count[COUNTERS_IN_BUCKET];
        int next_avail;                      /* Which count[idx] to use next */
        struct count_bucket *next;
} count_bucket_t;

typedef struct {
        view_connection_t view;          /* generic stuff, must be first */

        /* Private cached interfaces */
        const processor_info_v2_interface_t *info_iface;

        /* Private data */
        ht_str_table_t instructions;
        struct count_bucket *current_bucket;

} x86_connection_t;

static conf_class_t *this_class;

static x86_connection_t *
x86_connection_of_obj(conf_object_t *obj)
{
        return (x86_connection_t *) obj;
}


struct reg_translation {
        const char *from;
        const char *to;
} reg_translation[] = {
        { "al", "r8" },
        { "cl", "r8" },
        { "dl", "r8" },
        { "bl", "r8" },
        { "ah", "r8" },
        { "ch", "r8" },
        { "dh", "r8" },
        { "bh", "r8" },
        
        { "spl", "r8" },
        { "bpl", "r8" },
        { "sil", "r8" },
        { "dil", "r8" },
        { "r8b", "r8" },
        { "r9b", "r8" },
        { "r10b", "r8" },
        { "r11b", "r8" },
        { "r12b", "r8" },
        { "r13b", "r8" },
        { "r14b", "r8" },
        { "r15b", "r8" },
        
        { "ax", "r16" },
        { "cx", "r16" },
        { "dx", "r16" },
        { "bx", "r16" },
        { "sp", "r16" },
        { "bp", "r16" },
        { "si", "r16" },
        { "di", "r16" },
        { "r8w", "r16" },
        { "r9w", "r16" },
        { "r10w", "r16" },
        { "r11w", "r16" },
        { "r12w", "r16" },
        { "r13w", "r16" },
        { "r14w", "r16" },
        { "r15w", "r16" },
        
        { "eax", "r32" },
        { "ecx", "r32" },
        { "edx", "r32" },
        { "ebx", "r32" },
        { "esp", "r32" },
        { "ebp", "r32" },
        { "esi", "r32" },
        { "edi", "r32" },
        { "r8d", "r32" },
        { "r9d", "r32" },
        { "r10d", "r32" },
        { "r11d", "r32" },
        { "r12d", "r32" },
        { "r13d", "r32" },
        { "r14d", "r32" },
        { "r15d", "r32" },
        
        { "rax", "r64" },
        { "rcx", "r64" },
        { "rdx", "r64" },
        { "rbx", "r64" },
        { "rsp", "r64" },
        { "rbp", "r64" },
        { "rsi", "r64" },
        { "rdi", "r64" },
        { "r8", "r64" },
        { "r9", "r64" },
        { "r10", "r64" },
        { "r11", "r64" },
        { "r12", "r64" },
        { "r13", "r64" },
        { "r14", "r64" },
        { "r15", "r64" },
        
        { "mm0", "m64" },
        { "mm1", "m64" },
        { "mm2", "m64" },
        { "mm3", "m64" },
        { "mm4", "m64" },
        { "mm5", "m64" },
        { "mm6", "m64" },
        { "mm7"  "m64" },
        
        { "es", "seg" },
        { "cs", "seg" },
        { "ss", "seg" },
        { "ds", "seg" },
        { "fs", "seg" },
        { "gs", "seg" },
        
        { "xmm0", "x128" },
        { "xmm1", "x128" },
        { "xmm2", "x128" },
        { "xmm3", "x128" },
        { "xmm4", "x128" },
        { "xmm5", "x128" },
        { "xmm6", "x128" },
        { "xmm7", "x128" },
        { "xmm8", "x128" },
        { "xmm9", "x128" },
        { "xmm10", "x128" },
        { "xmm11", "x128" },
        { "xmm12", "x128" },
        { "xmm13", "x128" },
        { "xmm14", "x128" },
        { "xmm15", "x128" },
        { "xmm16", "x128" },
        { "xmm17", "x128" },
        { "xmm18", "x128" },
        { "xmm19", "x128" },
        { "xmm20", "x128" },
        { "xmm21", "x128" },
        { "xmm22", "x128" },
        { "xmm23", "x128" },
        { "xmm24", "x128" },
        { "xmm25", "x128" },
        { "xmm26", "x128" },
        { "xmm27", "x128" },
        { "xmm28", "x128" },
        { "xmm29", "x128" },
        { "xmm30", "x128" },
        { "xmm31", "x128" },

        { "ymm0", "y256" },
        { "ymm1", "y256" },
        { "ymm2", "y256" },
        { "ymm3", "y256" },
        { "ymm4", "y256" },
        { "ymm5", "y256" },
        { "ymm6", "y256" },
        { "ymm7", "y256" },
        { "ymm8", "y256" },
        { "ymm9", "y256" },
        { "ymm10", "y256" },
        { "ymm11", "y256" },
        { "ymm12", "y256" },
        { "ymm13", "y256" },
        { "ymm14", "y256" },
        { "ymm15", "y256" },
        { "ymm16", "y256" },
        { "ymm17", "y256" },
        { "ymm18", "y256" },
        { "ymm19", "y256" },
        { "ymm20", "y256" },
        { "ymm21", "y256" },
        { "ymm22", "y256" },
        { "ymm23", "y256" },
        { "ymm24", "y256" },
        { "ymm25", "y256" },
        { "ymm26", "y256" },
        { "ymm27", "y256" },
        { "ymm28", "y256" },
        { "ymm29", "y256" },
        { "ymm30", "y256" },
        { "ymm31", "y256" },

        { "zmm0", "z512" },
        { "zmm1", "z512" },
        { "zmm2", "z512" },
        { "zmm3", "z512" },
        { "zmm4", "z512" },
        { "zmm5", "z512" },
        { "zmm6", "z512" },
        { "zmm7", "z512" },
        { "zmm8", "z512" },
        { "zmm9", "z512" },
        { "zmm10", "z512" },
        { "zmm11", "z512" },
        { "zmm12", "z512" },
        { "zmm13", "z512" },
        { "zmm14", "z512" },
        { "zmm15", "z512" },
        { "zmm16", "z512" },
        { "zmm17", "z512" },
        { "zmm18", "z512" },
        { "zmm19", "z512" },
        { "zmm20", "z512" },
        { "zmm21", "z512" },
        { "zmm22", "z512" },
        { "zmm23", "z512" },
        { "zmm24", "z512" },
        { "zmm25", "z512" },
        { "zmm26", "z512" },
        { "zmm27", "z512" },
        { "zmm28", "z512" },
        { "zmm29", "z512" },
        { "zmm30", "z512" },
        { "zmm31", "z512" },

        { "k0", "k64"},
        { "k1", "k64"},
        { "k2", "k64"},
        { "k3", "k64"},
        { "k4", "k64"},
        { "k5", "k64"},
        { "k6", "k64"},
        { "k7", "k64"},
};

static ht_str_table_t reg_ht;       /* register translation in a hash-table  */

static void
init_reg_hash_table()
{
        for (int i = 0; i < ALEN(reg_translation); i++) {
                ht_insert_str(&reg_ht, reg_translation[i].from,
                              reg_translation[i].to);
        }
}


FORCE_INLINE const char *
register_translation(const char *reg_name)
{
        return ht_lookup_str(&reg_ht, reg_name);
}


/* Return a pointer to a new counter. The counters are stored in buckets, if we
   run out of counters in a bucket a new bucket is created and we maintain a
   linked list of all created buckets. */
static uint64 *
new_counter(x86_connection_t *xc)
{
        count_bucket_t *bucket;
        if (!xc->current_bucket
            || xc->current_bucket->next_avail == COUNTERS_IN_BUCKET) {
                bucket = MM_ZALLOC(1, count_bucket_t);
                bucket->next = xc->current_bucket;
                xc->current_bucket = bucket;
        }
        int idx = xc->current_bucket->next_avail;
        xc->current_bucket->next_avail++;
        return &xc->current_bucket->count[idx]; 
}

static uint64 *
get_counter_for_name(x86_connection_t *xc, char *instruction_name)
{       
        uint64 *counter = ht_lookup_str(
                &xc->instructions, instruction_name);
        if (counter)
                return counter;

        counter = new_counter(xc);
        ht_insert_str(&xc->instructions, instruction_name, counter);
        return counter;
}        

static const char *
get_translation(const char *disass, int *len)
{
        char reg[256];
        /* Copy string upon first separator */
        int i;
        for (i = 0; i < ALEN(reg); i++) {
                if (!isalnum(disass[i]))
                        break;
                reg[i] = disass[i];
        }
        *len = i;             /* report back the length we looked at */
        if (i == ALEN(reg))
                return NULL;
        reg[i] = '\0';                       /* NUL terminate the string */
        const char *to = register_translation(reg);
        return to;            /* might be NULL if no translation was found */
}

static bool
is_immediate(const char *disass, int *len)
{
        int i = 0;
        while (isxdigit(disass[i]) || disass[i] =='x')
                i++;
        *len = i;
        return i > 0;
}

static int
get_end_of_mnemonic(const char *disass)
{
        for (int i = 0;; i++) {
                if (disass[i] == '\0')
                        return i;
                if (disass[i] == ' ')
                        return i;
        }
        ASSERT(0);
        return 0; 
}

static void
get_format(char *out, const char *disass)
{
        int eoi = get_end_of_mnemonic(disass);
        strncpy(out, disass, eoi);

        char *o = out + eoi;
        bool last_sep = true;
        
        for (int i = eoi; disass[i];) {
                if (last_sep) {
                        int len;
                        const char *to = get_translation(&disass[i], &len);
                        if (to) {
                                for (int j = 0; j < strlen(to); j++)
                                        (*o++) = to[j];
                                i += len;
                                last_sep = !isalnum(disass[i-1]);
                                continue;
                        }
                        if (isdigit(disass[i])
                            && is_immediate(&disass[i], &len)) {
                                strcpy(o, "imm");
                                o += 3;
                                i += len;
                                last_sep = !isalnum(disass[i-1]);
                                continue;
                        }
                        if ((disass[i] == '+' || disass[i] == '-')
                            && is_immediate(&disass[i+1], &len)) {
                                strcpy(o, "+imm");
                                o += 4;
                                i += len + 1;
                                last_sep = !isalnum(disass[i-1]);
                                continue;
                        }
                }
                last_sep = !isalnum(disass[i]);
                *(o++) = disass[i++];
        }
        *o = '\0';
}


static uint64 *
get_counter(view_connection_t *vc, const uint8 *bytes, uint8 size)
{
        x86_connection_t *xc = (x86_connection_t *)vc;

        const generic_address_t addr = 0;
        attr_value_t data = SIM_make_attr_data(size, bytes);
        tuple_int_string_t ret = xc->info_iface->disassemble(
                xc->view.cpu, addr, data, 0);
        SIM_attr_free(&data);

        if (ret.integer <= 0)
                return NULL;                 /* Illegal instruction */

        char instruction_name[256];
        get_format(instruction_name, ret.string);

        uint64 *counter = get_counter_for_name(xc, instruction_name);
        MM_FREE(ret.string);
        return counter;
}

static attr_value_t
histogram(view_connection_t *vc)
{
        x86_connection_t *xc = (x86_connection_t *)vc;
        attr_value_t ret = SIM_alloc_attr_list(
                ht_num_entries_str(&xc->instructions));
        int i = 0;        
        HT_FOREACH_STR(&xc->instructions, hit) {
                uint64 *counter = ht_iter_str_value(hit);
                const char *name = ht_iter_str_key(hit);
                SIM_attr_list_set_item(
                        &ret, i,
                        SIM_make_attr_list(
                                2,
                                SIM_make_attr_string(name),
                                SIM_make_attr_uint64(*counter)));
                i++;
        }
        return ret;
}

static void
clear(view_connection_t *vc)
{
        x86_connection_t *xc = (x86_connection_t *)vc;        
        HT_FOREACH_STR(&xc->instructions, hit) {
                uint64 *counter = ht_iter_str_value(hit);
                *counter = 0;
        }
}

view_connection_t *
new_x86_normalized_view(conf_object_t *parent, conf_object_t *cpu, int num, attr_value_t attr)
{
        strbuf_t sb = SB_INIT;

        sb_addfmt(&sb, "%s.con%d", SIM_object_name(parent), num);
        conf_object_t *sub_obj = SIM_create_object(this_class, sb_str(&sb), attr);
        sb_free(&sb);
        
        if (!sub_obj)
                return NULL;

        x86_connection_t *xc = x86_connection_of_obj(sub_obj);
        xc->view.get_counter = get_counter;
        xc->view.get_histogram = histogram;
        xc->view.clear_histogram = clear;

        /* Init private members */
        ht_init_str_table(&xc->instructions, true);        
        xc->info_iface = SIM_C_GET_INTERFACE(cpu, processor_info_v2);
        if (!xc->info_iface) {
                SIM_LOG_ERROR(parent, 0, "No processor_info_v2 interface found");
                SIM_delete_object(sub_obj);
        }
        return &xc->view;        
}

static conf_object_t *
alloc_object(lang_void *arg)
{
        x86_connection_t *xc = MM_ZALLOC(1, x86_connection_t);
        return &xc->view.obj;
}

static void
pre_delete_instance(conf_object_t *obj)
{
        x86_connection_t *xc = x86_connection_of_obj(obj);
        if (xc->view.cpu)
                xc->view.cis_iface->remove_connection_callbacks(
                        xc->view.cpu, obj);
}

static int
delete_instance(conf_object_t *obj)
{
        x86_connection_t *xc = x86_connection_of_obj(obj);        
        /* Free counters */
        struct count_bucket *bucket = xc->current_bucket;
        while (bucket) {
                struct count_bucket *next = bucket->next;
                MM_FREE(bucket);
                bucket = next;
        }
        /* Free hash-tables */
        ht_delete_str_table(&xc->instructions, false);

        MM_FREE(obj);
        return 0;
}

void
init_x86_normalized_view()
{
        const class_data_t funcs = {
                .alloc_object = alloc_object,
                .delete_instance = delete_instance,
                .pre_delete_instance = pre_delete_instance,
                .description = "sub class for instruction-histogram",
                .class_desc = "sub-class for instruction-histogram",
                .kind = Sim_Class_Kind_Pseudo,
        };        
        conf_class_t *cl = SIM_register_class(
                "x86_normalized_histogram_connection",
                &funcs);

        add_generic_view_attributes(cl);
        
        this_class = cl;
        init_reg_hash_table();
}
