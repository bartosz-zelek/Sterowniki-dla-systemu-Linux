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

#include "size-view.h"
#include "instruction-histogram.h"
#include "string.h"
#include <simics/simulator/conf-object.h>

#define MAX_SIZE 0x100

typedef struct {
        view_connection_t view;          /* generic stuff, must be first */

        /* Private data */
        uint64 counter[MAX_SIZE];   /* Counters for each instruction-length */
} size_connection_t;


static conf_class_t *this_class;

static size_connection_t *
size_connection_of_obj(conf_object_t *obj)
{
        return (size_connection_t *) obj;
}

static size_connection_t *
view_to_size_connection(view_connection_t *vc)
{
        return (size_connection_t *) vc;
}


static uint64 *
get_counter(view_connection_t *vc, const uint8 *bytes, uint8 size)
{
        size_connection_t *sc = view_to_size_connection(vc);
        return &sc->counter[size];        
}

static attr_value_t
histogram(view_connection_t *vc)
{
        size_connection_t *sc = view_to_size_connection(vc);
        attr_value_t ret = SIM_alloc_attr_list(MAX_SIZE);
        for (int i = 0; i < MAX_SIZE; i++) {
                SIM_attr_list_set_item(
                        &ret, i,
                        SIM_make_attr_list(
                                2,
                                SIM_make_attr_uint64(i),
                                SIM_make_attr_uint64(sc->counter[i])));
        }
        return ret;
}

static void
clear(view_connection_t *vc)
{
        size_connection_t *sc = (size_connection_t*)vc;
        memset(sc->counter, 0, sizeof sc->counter);
}

view_connection_t *
new_size_view(conf_object_t *parent, conf_object_t *cpu, int num, attr_value_t attr)
{
        strbuf_t sb = SB_INIT;
        
        sb_addfmt(&sb, "%s.con%d", SIM_object_name(parent), num);
        conf_object_t *con_obj = SIM_create_object(this_class, sb_str(&sb), attr);
        sb_free(&sb);
        
        if (!con_obj)
                return NULL;

        size_connection_t *sc = size_connection_of_obj(con_obj);
        sc->view.get_counter = get_counter;
        sc->view.get_histogram = histogram;
        sc->view.clear_histogram = clear;
        return &sc->view;
}

static conf_object_t *
alloc_object(lang_void *arg)
{
        size_connection_t *sc = MM_ZALLOC(1, size_connection_t);
        return &sc->view.obj;
}

static void
pre_delete_instance(conf_object_t *obj)
{
        size_connection_t *sc = size_connection_of_obj(obj);
        if (sc->view.cpu)
                sc->view.cis_iface->remove_connection_callbacks(
                        sc->view.cpu, obj);
}

static int
delete_instance(conf_object_t *obj)
{
        size_connection_t *sc = size_connection_of_obj(obj);
        MM_FREE(sc);
        return 0;
}

void
init_size_view()
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
                "size_histogram_connection",
                &funcs);

        add_generic_view_attributes(cl);
        
        this_class = cl;
        
}
