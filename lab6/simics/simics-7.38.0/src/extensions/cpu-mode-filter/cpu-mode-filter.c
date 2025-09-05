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

#include <simics/simulator-iface/instrumentation-filter.h>
#include <simics/model-iface/cpu-instrumentation.h>
#include <simics/model-iface/processor-info.h>
#include <simics/processor-api.h>

typedef struct filter filter_t;              /* forward declaration */

typedef struct {
        conf_object_t *slave;                  /* slave/connection */
        filter_t *filter;
        conf_object_t *trigger_obj;            /* cpu */

        /* cached interfaces */
        const instrumentation_filter_slave_interface_t *slave_iface;       
        const processor_info_v2_interface_t *pi_iface;
        const cpu_instrumentation_subscribe_interface_t *cis_iface; 
        cpu_cb_handle_t *mc_handle;
} slave_t;

typedef VECT(processor_mode_t) mode_vect_t;

struct filter {
        conf_object_t obj;

        conf_object_t *cell;       /* The cell for the first slave */
        unsigned source_id;        /* filter "pin" to toggle */
        VECT(slave_t *) slaves;    /* The tool-connections we enable/disable */

        mode_vect_t modes;
};

FORCE_INLINE filter_t *
filter_of_obj(conf_object_t *obj) { return (filter_t *)obj; }

/* instrumentation_filter_master::set_source_id */
static void
ifm_set_source_id(conf_object_t *obj, unsigned source_id)
{
        filter_t *f = filter_of_obj(obj);
        f->source_id = source_id;
}

static void
update(filter_t *f, slave_t *s, processor_mode_t mode)
{
        VFORI(f->modes, i) {
                if (mode == VGET(f->modes, i)) {
                        s->slave_iface->enable(s->slave, f->source_id);
                        return;
                }
        }
        s->slave_iface->disable(s->slave, f->source_id);
}

static void
update_slaves(filter_t *f)
{
        VFORP(f->slaves, slave_t, s)
                update(f, s, s->pi_iface->get_processor_mode(s->trigger_obj));
}

static void
mode_change_cb(conf_object_t *filter, conf_object_t *cpu,
               processor_mode_t old_mode,
               processor_mode_t new_mode,
               void *user_data)
{
        slave_t *s = user_data;
        update(s->filter, s, new_mode);
}

/* instrumentation_filter_master::add_slave */
static bool
ifm_add_slave(conf_object_t *obj, conf_object_t *slave,
              conf_object_t *trigger_obj)
{
        filter_t *f = filter_of_obj(obj);

        ASSERT(trigger_obj);
        if (!f->cell)
                f->cell = VT_object_cell(trigger_obj);

        if (VT_object_cell(trigger_obj) != f->cell) {
                /* Cannot have slaves living in another cell */
                SIM_LOG_ERROR(
                        obj, 0,
                        "slave %s in different cell (%s vs %s)",
                        SIM_object_name(trigger_obj),
                        SIM_object_name(VT_object_cell(trigger_obj)),
                        SIM_object_name(f->cell));                        
                return false;
        }

        const cpu_instrumentation_subscribe_interface_t *cis_iface = 
                SIM_C_GET_INTERFACE(trigger_obj, cpu_instrumentation_subscribe);
        if (!cis_iface) {
                SIM_LOG_ERROR(
                        obj, 0, "Couldn't find the "
                        CPU_INSTRUMENTATION_SUBSCRIBE_INTERFACE
                        " interface in %s", SIM_object_name(trigger_obj));
                return false;
        }

        const processor_info_v2_interface_t *pi_iface = 
                SIM_C_GET_INTERFACE(trigger_obj, processor_info_v2);
        if (!pi_iface) {
                SIM_LOG_ERROR(
                        obj, 0,"Couldn't find the "
                        PROCESSOR_INFO_V2_INTERFACE
                        " interface in %s", SIM_object_name(trigger_obj));
                return false;
        }

        const instrumentation_filter_slave_interface_t *slave_ifc =
                SIM_C_GET_INTERFACE(slave, instrumentation_filter_slave);
        ASSERT(slave_ifc);
        
        slave_t *s = MM_MALLOC(1, slave_t);
        s->slave = slave;
        s->filter = f;
        s->slave_iface = slave_ifc;
        s->trigger_obj = trigger_obj;
        s->pi_iface = pi_iface;
        s->cis_iface = cis_iface;
        
        VADD(f->slaves, s);

        s->mc_handle = cis_iface->register_mode_change_cb(
                trigger_obj, NULL,
                mode_change_cb, s);

        update(f, s, pi_iface->get_processor_mode(trigger_obj));               
        return true;
}

static void
delete_slave(slave_t *s)
{
        s->slave_iface->enable(s->slave, s->filter->source_id);
        s->cis_iface->remove_callback(s->trigger_obj,
                                      s->mc_handle);
        MM_FREE(s);
}

/* instrumentation_filter_master::remove_slave */
static void
ifm_remove_slave(conf_object_t *obj, conf_object_t *slave,
                 conf_object_t *trigger_obj)
{
        filter_t *f = filter_of_obj(obj);        
        VFORI(f->slaves, i) {
                slave_t *s = VGET(f->slaves, i);
                if (s->slave == slave && s->trigger_obj == trigger_obj) {
                        VREMOVE(f->slaves, i);
                        delete_slave(s);
                        return;
                }                
        }
        ASSERT(0);
}

struct {
        processor_mode_t mode;
        const char *string;
} mode_info[] = {
        { Sim_CPU_Mode_User,       "user" },
        { Sim_CPU_Mode_Supervisor, "supervisor" },
        { Sim_CPU_Mode_Hypervisor, "hypervisor" }
};

static const char *
get_mode_string(processor_mode_t mode)
{
        for (int i = 0; i < ALEN(mode_info); i++)
                if (mode_info[i].mode == mode)
                        return mode_info[i].string;
        ASSERT(0);
        return NULL;
}

static processor_mode_t
get_mode(const char *string)
{
        for (int i = 0; i < ALEN(mode_info); i++)
                if (strcmp(string, mode_info[i].string) == 0)
                        return mode_info[i].mode;
        return (processor_mode_t)-1;
}

/* instrumentation_filter_master::short_filter_config */
static char *
ifm_short_filter_config(conf_object_t *obj)
{
        filter_t *f = filter_of_obj(obj);                
        strbuf_t out = SB_INIT;
        VFORI(f->modes, i) {
                processor_mode_t m = VGET(f->modes, i);
                sb_addfmt(&out, "%s%s", i > 0 ? ", " : "", get_mode_string(m));
        }
        return sb_detach(&out);
}

static attr_value_t
get_tracked_modes(void *param, conf_object_t *obj, attr_value_t *idx)
{
        filter_t *f = filter_of_obj(obj);
        attr_value_t ret = SIM_alloc_attr_list(VLEN(f->modes));

        VFORI(f->modes, i) {
                processor_mode_t m = VGET(f->modes, i);
                const char *s = get_mode_string(m);
                SIM_attr_list_set_item(&ret, i, SIM_make_attr_string(s));
        }
        return ret;
}


static set_error_t
set_tracked_modes(void *arg, conf_object_t *obj, attr_value_t *val,
                  attr_value_t *idx)
{
        filter_t *f = filter_of_obj(obj);
        int len = SIM_attr_list_size(*val);

        mode_vect_t modes = VNULL;
        
        for (int i = 0; i < len; i++) {
                const char *s = SIM_attr_string(SIM_attr_list_item(*val, i));
                processor_mode_t mode = get_mode(s);
                if ((processor_mode_t)mode == -1)
                        return Sim_Set_Illegal_Value;                
                VADD(modes, mode);
        }
        VFREE(f->modes);
        f->modes = modes;
        update_slaves(f);
        return Sim_Set_Ok;
}

static conf_object_t *
alloc_object(lang_void *arg)
{
        filter_t *f = MM_ZALLOC(1, filter_t);
        VINIT(f->modes);
        return &f->obj;
}

static void
pre_delete_instance(conf_object_t *obj)
{
        filter_t *f = filter_of_obj(obj);
        while(!VEMPTY(f->slaves))
                delete_slave(VPOP(f->slaves));
}

/* Cleanup of deleted object instance */
static int
delete_instance(conf_object_t *obj)
{
        filter_t *f = filter_of_obj(obj);
        MM_FREE(f);
        return 0;
}

void
init_local()
{
        const class_data_t funcs = {
                .alloc_object = alloc_object,
                .pre_delete_instance = pre_delete_instance,                
                .delete_instance = delete_instance,
                .description = "Instrumentation filter for processes.",
                .class_desc = "instrumentation filter for processes",
                .kind = Sim_Class_Kind_Pseudo,
        };

        conf_class_t *cl = SIM_register_class(
                "cpu_mode_filter",
                &funcs);
        
        static const instrumentation_filter_master_interface_t ifm = {
                .set_source_id = ifm_set_source_id,
                .short_filter_config = ifm_short_filter_config,
                .add_slave = ifm_add_slave,
                .remove_slave = ifm_remove_slave,
        };
        SIM_REGISTER_INTERFACE(cl, instrumentation_filter_master, &ifm);

        SIM_register_typed_attribute(
                cl, "tracked_modes",
                get_tracked_modes, NULL,
                set_tracked_modes, NULL,
                Sim_Attr_Pseudo,
                "[s*]", NULL,
                "The modes being tracked.");        
}
