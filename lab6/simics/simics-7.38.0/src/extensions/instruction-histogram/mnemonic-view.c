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

#include "mnemonic-view.h"
#include "instruction-histogram.h"
#include <simics/model-iface/processor-info.h>
#include <simics/util/hashtab.h>
#include <simics/simulator/conf-object.h>
#include <simics/base/log.h>

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

} mnemonic_connection_t;


static conf_class_t *this_class;

static mnemonic_connection_t *
mnemonic_connection_of_obj(conf_object_t *obj)
{
        return (mnemonic_connection_t *) obj;
}

static mnemonic_connection_t *
view_to_mnemonic_connection(view_connection_t *vc)
{
        return (mnemonic_connection_t *) vc;
}

/* Return a pointer to a new counter. The counters are stored in buckets, if we
   run out of counters in a bucket a new bucket is created and we maintain a
   linked list of all created buckets. */
static uint64 *
new_counter(mnemonic_connection_t *mc)
{
        count_bucket_t *bucket;
        if (!mc->current_bucket
            || mc->current_bucket->next_avail == COUNTERS_IN_BUCKET) {
                bucket = MM_ZALLOC(1, count_bucket_t);
                bucket->next = mc->current_bucket;
                mc->current_bucket = bucket;
        }
        int idx = mc->current_bucket->next_avail;
        mc->current_bucket->next_avail++;
        return &mc->current_bucket->count[idx]; 
}

/* For a given instruction name, pick up a counter to be incremented when this
   instruction is being executed. */
static uint64 *
get_counter_for_name(mnemonic_connection_t *mc, char *instruction_name)
{
        uint64 *counter = ht_lookup_str(&mc->instructions, instruction_name);
        if (counter)
                return counter;

        counter = new_counter(mc);
        ht_insert_str(&mc->instructions, instruction_name, counter);
        return counter;
}        

/* This is specifically for Xtensa which can run multiple instructions in
   parallel. For example, does the following conversion:
   "{ addi a14, a1, 16; movi a15, 4 }" -> "{addi;movi}"
*/
static void
extract_bundle_mnemonics(const char *disass, char *output)
{
        ASSERT(disass[0] == '{');
        strcpy(output, "{");
        const char *d = &disass[1];
        const char *end = disass + strlen(disass);
        while (d < end) {
                /* Skip starting spaces */
                if (*d == ' ') { d++; continue; }

                int eoi = strcspn(d, ";}\n"); /* End of instruction */
                int eom = strcspn(d, " ;}\n");
                strncat(output, d, eom);     /* Skip parameters */

                if (d[eoi] == '}') {
                        strcat(output, "}");
                        return;
                }
                strcat(output, ";");
                d += eoi + 1;
        }
        /* Exit while(), but never found ending '}' */
}

/* Generic bundle handling for instructions separated by ||
   INS1 operands || INS2 operands -> INS1||INS2 */
static void
extract_bundle_mnemonics2(const char *disass, char *output)
{
        const char *d = disass;
        const char *end = disass + strlen(disass);
        *output = 0;                         /* Start with empty string */
        while (d < end) {
                /* Skip starting spaces */
                if (*d == ' ') { d++; continue; }

                int eom = strcspn(d, " |\n");
                strncat(output, d, eom);     /* Copy mnemonic, skip parameters */
                d += eom;

                char *next = strstr(d, "||"); /* End of instruction */
                if (!next) {
                        strcat(output, "\0"); /* No more instructions */
                        return;
                }
                strcat(output, "||");
                /* Continue on next instruction */
                d = next + 2;
        }
}

static int
x86_consume_rep_prefix(const char *da_string)
{
        if (strncmp(da_string, "rep ", 4) == 0)
                return 4;
        if (strncmp(da_string, "repe ", 5) == 0)
                return 5;
        if (strncmp(da_string, "repne ", 6) == 0)
                return 6;
        return 0;
}

static uint64 *
get_counter(view_connection_t *vc, const uint8 *bytes, uint8 size)
{
        mnemonic_connection_t *mc = view_to_mnemonic_connection(vc);
        const generic_address_t addr = 0;
        attr_value_t data = SIM_make_attr_data(size, bytes);
        tuple_int_string_t ret = mc->info_iface->disassemble(
                mc->view.cpu, addr, data, 0);
        SIM_attr_free(&data);

        if (ret.integer <= 0)
                return NULL;                 /* Illegal instruction */

        char instruction_name[512];
        if (ret.string[0] == '{') {
                /* Xtensa Flix Bundle of multiple instructions */
                extract_bundle_mnemonics(ret.string, instruction_name);
        } else if (strstr(ret.string, "||")) {
                /* Other bundles which are || separated instructions */
                extract_bundle_mnemonics2(ret.string, instruction_name);
        } else {
                int eoi = x86_consume_rep_prefix(ret.string);
                eoi += strcspn(ret.string + eoi, " \n");
                ASSERT(eoi < 256);
                strncpy(instruction_name, ret.string, eoi);
                instruction_name[eoi] = 0;
        }

        MM_FREE(ret.string);
        uint64 *counter = get_counter_for_name(mc, instruction_name);
        SIM_attr_free(&data);
        return counter;
}

static attr_value_t
histogram(view_connection_t *vc)
{        
        mnemonic_connection_t *mc = view_to_mnemonic_connection(vc);

        attr_value_t ret = SIM_alloc_attr_list(
                ht_num_entries_str(&mc->instructions));
        int i = 0;
        HT_FOREACH_STR(&mc->instructions, hit) {
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
        mnemonic_connection_t *mc = view_to_mnemonic_connection(vc);
        HT_FOREACH_STR(&mc->instructions, hit) {
                uint64 *counter = ht_iter_str_value(hit);
                *counter = 0;
        }
}

view_connection_t *
new_mnemonic_view(conf_object_t *parent, conf_object_t *cpu, int num, attr_value_t args)
{
        strbuf_t sb = SB_INIT;
        
        sb_addfmt(&sb, "%s.con%d", SIM_object_name(parent), num);
        conf_object_t *sub_obj = SIM_create_object(this_class, sb_str(&sb), args);
        sb_free(&sb);
        
        if (!sub_obj)
                return NULL;
        
        mnemonic_connection_t *mc = mnemonic_connection_of_obj(sub_obj);
        mc->view.get_counter = get_counter;
        mc->view.get_histogram = histogram;
        mc->view.clear_histogram = clear;
                
        /* Init private members */
        ht_init_str_table(&mc->instructions, true);
        mc->current_bucket = NULL;
        mc->info_iface = SIM_C_GET_INTERFACE(cpu, processor_info_v2);
        if (!mc->info_iface) {
                SIM_LOG_ERROR(parent, 0,
                              "No processor_info_v2 interface found");
                SIM_delete_object(sub_obj);
                return NULL;
        }                
        return &mc->view;        
}

static conf_object_t *
alloc_object(lang_void *arg)
{
        mnemonic_connection_t *mc = MM_ZALLOC(1, mnemonic_connection_t);
        return &mc->view.obj;
}

static void
pre_delete_instance(conf_object_t *obj)
{
        mnemonic_connection_t *mc = mnemonic_connection_of_obj(obj);
        if (mc->view.cpu)
                mc->view.cis_iface->remove_connection_callbacks(
                        mc->view.cpu, obj);
}

static int
delete_instance(conf_object_t *obj)
{
        mnemonic_connection_t *mc = mnemonic_connection_of_obj(obj);

        /* Free counters */
        struct count_bucket *bucket = mc->current_bucket;
        while (bucket) {
                struct count_bucket *next = bucket->next;
                MM_FREE(bucket);
                bucket = next;
        }
        /* Free hash-tables */
        ht_delete_str_table(&mc->instructions, false);

        MM_FREE(mc);
        return 0;
}

void
init_mnemonic_view()
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
                "mnemonic_histogram_connection",
                &funcs);

        add_generic_view_attributes(cl);
        
        this_class = cl;
        
}
