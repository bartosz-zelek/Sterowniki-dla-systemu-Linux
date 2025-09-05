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

#include "xed-iform-view.h"
#include "instruction-histogram.h"
#include <simics/base/log.h>
#include <simics/simulator/hap-consumer.h>
#include <simics/model-iface/int-register.h>
#include <simics/simulator/conf-object.h>
#include <simics/arch/x86-instrumentation.h>
#include "simics/arch/x86.h"

typedef struct {
        view_connection_t view;          /* generic stuff, must be first */

        /* Private data */
        uint64 *counts;
        const char **names;

        int xed_iform_last;
        const xed_access_interface_t *xed_iface;
} xed_connection_t;

static conf_class_t *this_class;

static xed_connection_t *
xed_connection_of_obj(conf_object_t *obj)
{
        return (xed_connection_t *) obj;
}

static uint64 *
get_counter(view_connection_t *vc, const uint8 *bytes, uint8 size)
{
        xed_connection_t *xc = (xed_connection_t *)vc;
        bytes_t b = {.data = bytes, .len = size};
        int iform = xc->xed_iface->decode(xc->view.cpu, Xed_Iform, b);
        return &xc->counts[iform];
}

static attr_value_t
histogram(view_connection_t *vc)
{
        xed_connection_t *xc = (xed_connection_t *)vc;
        attr_value_t ret = SIM_alloc_attr_list(xc->xed_iform_last - 1);
        for (int i = 1; i < xc->xed_iform_last; i++) { // skip invalid index
                SIM_attr_list_set_item(
                        &ret, i - 1,
                        SIM_make_attr_list(
                                2,
                                SIM_make_attr_string(xc->names[i]),
                                SIM_make_attr_uint64(xc->counts[i])));
        }        
        return ret;
}

static void
clear(view_connection_t *vc)
{
        xed_connection_t *xc = (xed_connection_t *)vc;
        memset(xc->counts, 0, xc->xed_iform_last * sizeof *xc->counts);
}

view_connection_t *
new_xed_iform_view(conf_object_t *parent, conf_object_t *cpu, int num, attr_value_t attr)
{
        strbuf_t sb = SB_INIT;
        sb_addfmt(&sb, "%s.con%d", SIM_object_name(parent), num);
        conf_object_t *sub_obj = SIM_create_object(this_class, sb_str(&sb), attr);
        sb_free(&sb);
        
        if (!sub_obj)
                return NULL;

        xed_connection_t *xc = xed_connection_of_obj(sub_obj);
        xc->view.get_counter = get_counter;
        xc->view.get_histogram = histogram;
        xc->view.clear_histogram = clear;
                               
        xc->xed_iface = SIM_C_GET_INTERFACE(cpu, xed_access);
        if (!xc->xed_iface) {
                SIM_LOG_ERROR(sub_obj, 0, "CPU object '%s' does not support"
                              " xed_access interface", SIM_object_name(cpu));
                return NULL;
        }

        xc->xed_iform_last = xc->xed_iface->get_last(xc->view.cpu, Xed_Iform);
        xc->counts = MM_ZALLOC(xc->xed_iform_last, uint64);
        if (!xc->counts)
                return NULL;

        xc->names = MM_ZALLOC(xc->xed_iform_last, const char *);
        if (!xc->names)
                return NULL;

        for (int i = 0; i < xc->xed_iform_last; i++)
                xc->names[i] = xc->xed_iface->to_string(xc->view.cpu,
                                                        Xed_Iform, i);
        
        return &xc->view;        
}

static conf_object_t *
alloc_object(lang_void *arg)
{
        xed_connection_t *xc = MM_ZALLOC(1, xed_connection_t);
        if (!xc)
                return NULL;

        return &xc->view.obj;
}

static void
pre_delete_instance(conf_object_t *obj)
{
        xed_connection_t *xc = xed_connection_of_obj(obj);
        if (xc->view.cpu) {
                xc->view.cis_iface->remove_connection_callbacks(
                        xc->view.cpu, obj);
        }
}

static int
delete_instance(conf_object_t *obj)
{
        xed_connection_t *xc = xed_connection_of_obj(obj);
        MM_FREE(xc->counts);
        MM_FREE(xc->names);
        MM_FREE(xc);
        return 0;
}

void
init_xed_iform_view()
{
        const class_data_t funcs = {
                .alloc_object = alloc_object,
                .delete_instance = delete_instance,
                .pre_delete_instance = pre_delete_instance,
                .description = "sub class for instruction-histogram",
                .class_desc =  "sub-class for instruction-histogram",
                .kind = Sim_Class_Kind_Pseudo,
        };        
        conf_class_t *cl = SIM_register_class(
                "xed_iform_histogram_connection",
                &funcs);

        add_generic_view_attributes(cl);

        this_class = cl;        
}

