/*
  © 2017 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <simics/simulator-api.h>
#include <simics/simulator-iface/instrumentation-tool.h>
#include <simics/simulator-iface/instrumentation-filter.h>


typedef struct {
        conf_object_t obj;
        struct {
                conf_object_t *obj;
                const instrumentation_connection_interface_t *ic_iface;
        } dest;
        
        uint64 disabled_bits;
} aggregator_t;

FORCE_INLINE aggregator_t *agg_of_obj(conf_object_t *obj)
{
        return (aggregator_t *) obj;
}

/* Set or remove a disabled source bit. As long as any disabled-bit is set, the
   connection is (or remains) disabled. */
static void
aggregate(aggregator_t *agg, int source_bit, int set)
{
        uint64 prev = agg->disabled_bits;
        agg->disabled_bits = (agg->disabled_bits & ~(1ULL << source_bit))
                | ((uint64)set << source_bit);

        if (agg->disabled_bits == 0 && prev != 0)
                agg->dest.ic_iface->enable(agg->dest.obj);
        else if (agg->disabled_bits != 0 && prev == 0)
                agg->dest.ic_iface->disable(agg->dest.obj);
}

/* instrumentation_filter::disable() */
static void
disable(conf_object_t *obj, unsigned source_num)
{
        ASSERT(source_num < 64);
        aggregator_t *agg = agg_of_obj(obj);
        aggregate(agg, source_num, 1);
}

/* instrumentation_filter::enable() */
static void
enable(conf_object_t *obj, unsigned source_num)
{
        ASSERT(source_num < 64);
        aggregator_t *agg = agg_of_obj(obj);
        aggregate(agg, source_num, 0);
}

/* instrumentation_filter_status::get_disabled_sources() */
static attr_value_t
get_disabled_sources(conf_object_t *obj)
{
        aggregator_t *agg = agg_of_obj(obj);
        VECT(attr_value_t) vect = VNULL;
        
        for (unsigned i = 0; i < 64; i++)
                if ((agg->disabled_bits >> i) & 1)
                        VADD(vect, SIM_make_attr_int64(i));

        attr_value_t ret = VT_make_attr_list_from_array(VLEN(vect), VVEC(vect));
        VFREE(vect);
        return ret;
}

static attr_value_t
attr_get_dest(void *param, conf_object_t *obj, attr_value_t *idx)
{
        aggregator_t *agg = agg_of_obj(obj);
        if (agg->dest.obj) 
                return SIM_make_attr_object(agg->dest.obj);
        return SIM_make_attr_nil();
}

static set_error_t
attr_set_dest(void *param, conf_object_t *obj, attr_value_t *val,
         attr_value_t *idx)
{
        aggregator_t *agg = agg_of_obj(obj);

        if (SIM_attr_is_nil(*val)) {
                agg->dest.obj = NULL;
                agg->dest.ic_iface = NULL;
                return Sim_Set_Ok;
        }

        conf_object_t *dest = SIM_attr_object(*val);
        const instrumentation_connection_interface_t *iface = 
                SIM_C_GET_INTERFACE(dest, instrumentation_connection);
        if (!iface)
                return Sim_Set_Interface_Not_Found;
        
        agg->dest.obj = dest;
        agg->dest.ic_iface = iface;                
        return Sim_Set_Ok;
}

static attr_value_t
attr_get_disabled_sources(void *param, conf_object_t *obj, attr_value_t *idx)
{
        return get_disabled_sources(obj);
}

static conf_object_t *
alloc_object(void *arg)
{
        aggregator_t *agg = MM_ZALLOC(1, aggregator_t);
        return &agg->obj;
}

static int
delete_instance(conf_object_t *obj)
{
        MM_FREE(obj);
        return 0;
}

void
init_local()
{
        const class_data_t funcs = {
                .alloc_object = alloc_object,
                .delete_instance = delete_instance,                
                .description = "Aggregates disable/enable sources to one output",
                .class_desc = "aggregates disable/enable sources to one output",
                .kind = Sim_Class_Kind_Pseudo
        };
        conf_class_t *cl = SIM_register_class(
                "instrumentation_filter_aggregator", &funcs);

        static const instrumentation_filter_slave_interface_t ifs = {
                .disable = disable,
                .enable = enable,
        };
        SIM_REGISTER_INTERFACE(cl, instrumentation_filter_slave, &ifs);

        static const instrumentation_filter_status_interface_t ifsi = {
                .get_disabled_sources = get_disabled_sources,
        };
        SIM_REGISTER_INTERFACE(cl, instrumentation_filter_status, &ifsi);

        SIM_register_typed_attribute(
                cl, "dest",
                attr_get_dest, NULL,
                attr_set_dest, NULL,
                Sim_Attr_Optional,
                "o|n", NULL,
                "Destination object, must implement instrumentation_connection"
                " interface.");

        SIM_register_typed_attribute(
                cl, "disabled_sources",
                attr_get_disabled_sources, NULL,
                NULL, NULL,
                Sim_Attr_Pseudo,
                "[i*]", NULL,
                "List of disabled sources as integers."
                );
}



