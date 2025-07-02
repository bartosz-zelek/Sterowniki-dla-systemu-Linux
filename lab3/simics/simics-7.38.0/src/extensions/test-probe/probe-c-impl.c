/*
  © 2023 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <simics/base/conf-object.h>
#include <simics/simulator-iface/probe-interface.h>

typedef struct {
        conf_object_t obj;
        int val;
        attr_value_t props;
} probe_t;

static conf_object_t *
alloc_object(void *data)
{
        probe_t *p = MM_MALLOC(1, probe_t);
        return &p->obj;
}

static void *
init_object(conf_object_t *obj, void *data)
{
        probe_t *p = (probe_t *)obj;
        p->val = 0;
        p->props =
                VT_make_attr("[[is]]", (uint64)Probe_Key_Kind, "test.c.probe");
        return &p->obj;
}

static int
delete_instance(conf_object_t *obj)
{
        probe_t *p = (probe_t *)obj;
        MM_FREE(p);
        return 0;
}

static set_error_t
incr(void *dont_care, conf_object_t *obj,
            attr_value_t *val, attr_value_t *idx)
{
        probe_t *p = (probe_t *)obj;
        p->val++;
        return Sim_Set_Ok;
}

static attr_value_t
value(conf_object_t *obj)
{
        probe_t *p = (probe_t *)obj;
        return SIM_make_attr_uint64(p->val);
}

static attr_value_t
properties(conf_object_t *obj)
{
        probe_t *p = (probe_t *)obj;
        return p->props;
}

void
init_local()
{
        const class_data_t funcs = {
                .alloc_object = alloc_object,
                .init_object = init_object,
                .delete_instance = delete_instance,
                .class_desc = "probe implemented in C",
                .description =
                        "Example of an object written in C implementing the probe interface."
        };

        conf_class_t *cls = SIM_register_class("c_test_probe", &funcs);

        static const probe_interface_t probe_iface = {
                .value = value,
                .properties = properties,
        };
        SIM_REGISTER_INTERFACE(cls, probe, &probe_iface);

        SIM_register_typed_attribute(cls, "increment",
                                     NULL, NULL, incr, NULL,
                                     Sim_Attr_Pseudo, NULL, NULL,
                                     "Increment the probe value");
}
