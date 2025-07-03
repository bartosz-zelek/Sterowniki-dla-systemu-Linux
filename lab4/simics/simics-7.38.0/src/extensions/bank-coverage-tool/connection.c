/*
  © 2018 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <connection.h>
#include <coverage-tool.h>

#include <simics/simulator-api.h>
#include <simics/simulator-iface/instrumentation-tool.h>

conf_class_t *connection_class;

static conf_object_t *
new_connection(void *arg)
{
        connection_t *c = MM_ZALLOC(1, connection_t);
        ht_init_int_table(&c->read_accesses);
        ht_init_int_table(&c->write_accesses);
        return &c->obj;
}

static int
delete_connection(conf_object_t *obj)
{
        MM_FREE(obj);
        return 0;
}

static void
increment_count(ht_int_table_t *accesses,
                physical_address_t offset,
                physical_address_t size)
{
        ht_int_table_t *access_count = ht_lookup_int(accesses, offset);
        if (!access_count) {
                access_count = MM_ZALLOC(1, ht_int_table_t);
                ht_insert_int(accesses, offset, access_count);
        }

        uintptr_t count = (uintptr_t)ht_lookup_int(access_count, size);
        ht_update_int(access_count, size, (void *)(count + 1));
}

static void
read(conf_object_t *obj,
     bank_before_read_interface_t *before_read,
     bank_access_t *handle,
     lang_void *user_data)
{
        increment_count(&connection(obj)->read_accesses,
                        before_read->offset(handle), before_read->size(handle));
}

static void
write(conf_object_t *obj,
      bank_before_write_interface_t *before_write,
      bank_access_t *handle,
      lang_void *user_data)
{
        increment_count(&connection(obj)->write_accesses,
                        before_write->offset(handle),
                        before_write->size(handle));
}

void
subscribe(connection_t *c)
{
        // An offset and a size of 0 instruments the entire bank
        c->subscribe->register_before_read(c->bank, &c->obj, 0, 0, read, NULL);
        c->subscribe->register_before_write(
            c->bank, &c->obj, 0, 0, write, NULL);
}

static void
unsubscribe(connection_t *c)
{
        c->subscribe->remove_connection_callbacks(c->bank, &c->obj);
}

static void
free_hashtab(ht_int_table_t *accesses)
{
        HT_FOREACH_INT(accesses, accesses_it) {
                ht_int_table_t *access_cnt = ht_iter_int_value(accesses_it);
                MM_FREE(access_cnt);
        }
}

static void
pre_delete_connection(conf_object_t *obj)
{
        connection_t *c = connection(obj);

        free_hashtab(&c->read_accesses);
        free_hashtab(&c->write_accesses);

        unsubscribe(c);
}

static void
enable(conf_object_t *obj)
{
        subscribe(connection(obj));
}

static void
disable(conf_object_t *obj)
{
        unsubscribe(connection(obj));
}

static attr_value_t
flatten_hashtab(ht_int_table_t *accesses)
{
        unsigned num_flattened = 0;
        HT_FOREACH_INT(accesses, accesses_it) {
                ht_int_table_t *access_cnt = ht_iter_int_value(accesses_it);
                num_flattened += ht_num_entries_int(access_cnt);
        }

        attr_value_t ret = SIM_alloc_attr_list(num_flattened);

        unsigned idx = 0;
        HT_FOREACH_INT(accesses, accesses_it) {
                ht_int_table_t *access_cnt = ht_iter_int_value(accesses_it);
                HT_FOREACH_INT(access_cnt, access_cnt_it) {
                        uint64 ofs = ht_iter_int_key(accesses_it);
                        uint64 size = ht_iter_int_key(access_cnt_it);
                        uintptr_t cnt = (uintptr_t)ht_iter_int_value(
                                access_cnt_it);

                        SIM_attr_list_set_item(&ret, idx++,
                                               SIM_make_attr_list(
                                                   3,
                                                   SIM_make_attr_uint64(ofs),
                                                   SIM_make_attr_uint64(size),
                                                   SIM_make_attr_uint64(cnt)));
                }
        }

        return ret;
}

#define GET_ACCESSES(OPTION)                                           \
        static attr_value_t                                            \
        get_##OPTION(void *arg, conf_object_t *obj, attr_value_t *idx) \
        {                                                              \
                return flatten_hashtab(&connection(obj)->OPTION);      \
        }

GET_ACCESSES(read_accesses);
GET_ACCESSES(write_accesses);

static attr_value_t
get_bank(void *arg, conf_object_t *obj, attr_value_t *idx)
{
        return SIM_make_attr_object(connection(obj)->bank);
}

void
init_connection()
{
        const class_data_t class_data = {
                .alloc_object = new_connection,
                .delete_instance = delete_connection,
                .description = "Coverage tool connection",
                .class_desc = "coverage tool connection",
                .kind = Sim_Class_Kind_Pseudo,
                .pre_delete_instance = pre_delete_connection,
        };
        connection_class = SIM_register_class("bank_coverage_tool_connection",
                                              &class_data);

        static const instrumentation_connection_interface_t
                connection_interface = {
                .enable = enable,
                .disable = disable,
        };
        SIM_REGISTER_INTERFACE(connection_class,
                               instrumentation_connection,
                               &connection_interface);

        SIM_register_typed_attribute(connection_class, "read_accesses",
                                     get_read_accesses, NULL,
                                     NULL, NULL,
                                     Sim_Attr_Pseudo | Sim_Attr_Read_Only,
                                     "[[iii]*]", NULL,
                                     "Instrumented read accesses.");

        SIM_register_typed_attribute(connection_class, "write_accesses",
                                     get_write_accesses, NULL,
                                     NULL, NULL,
                                     Sim_Attr_Pseudo | Sim_Attr_Read_Only,
                                     "[[iii]*]", NULL,
                                     "Instrumented write accesses.");

        SIM_register_typed_attribute(connection_class, "bank",
                                     get_bank, NULL,
                                     NULL, NULL,
                                     Sim_Attr_Pseudo | Sim_Attr_Read_Only,
                                     "o", NULL, "Bank.");
}
