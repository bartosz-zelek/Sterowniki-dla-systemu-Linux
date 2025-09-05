/*
  © 2017 Intel Corporation
*/

#include <simics/simulator-iface/instrumentation-filter.h>
#include <simics/model-iface/cpu-instrumentation.h>
#include <simics/model-iface/processor-info.h>
#include <simics/processor-api.h>

typedef struct filter filter_t;

/* Data associated with the connected "slaves" which are  */
typedef struct {
        conf_object_t *slave;                  /* slave/connection */
        filter_t *filter;                      /* filter owning this */
        conf_object_t *provider;

        /* USER-TODO: Add any additional data saved in the slave. */
        
        /* cached interfaces */
        const instrumentation_filter_slave_interface_t *slave_iface;
        /* USER-TODO: Add cached interfaces */
} slave_t;

struct filter {
        conf_object_t obj;                   /* Must be first */

        conf_object_t *cell;       /* The cell for the first slave */
        unsigned source_id;        /* filter "pin" to toggle */
        VECT(slave_t *) slaves;    /* The tool-connections we enable/disable */

        /* USER-TODO: add filter specific data to the conf_object */
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
update(filter_t *f, slave_t *s
       /* USER-TODO: add additional args*/) 
{
        /* USER-TODO: Add logic */
        if (1)
                s->slave_iface->enable(s->slave, f->source_id);
        else
                s->slave_iface->disable(s->slave, f->source_id);
}

UNUSED static void
update_slaves(filter_t *f)
{
        VFORP(f->slaves, slave_t, s)
                update(f, s);
}


/* instrumentation_filter_master::add_slave */
static bool
ifm_add_slave(conf_object_t *obj, conf_object_t *slave,
              conf_object_t *provider)
{
        filter_t *f = filter_of_obj(obj);

        ASSERT(provider);
        if (!f->cell)
                f->cell = VT_object_cell(provider);

        if (VT_object_cell(provider) != f->cell) {
                /* Cannot have slaves living in another cell */
                SIM_LOG_ERROR(
                        obj, 0,
                        "slave %s in different cell (%s vs %s)",
                        SIM_object_name(provider),
                        SIM_object_name(VT_object_cell(provider)),
                        SIM_object_name(f->cell));                        
                return false;
        }

        const instrumentation_filter_slave_interface_t *slave_ifc =
                SIM_C_GET_INTERFACE(slave, instrumentation_filter_slave);
        ASSERT(slave_ifc);

        /* USER-TODO: Possibly register callbacks for the changes in the
           provider */
        
        slave_t *s = MM_MALLOC(1, slave_t);
        s->slave = slave;
        s->filter = f;
        s->slave_iface = slave_ifc;
        s->provider = provider;
        
        VADD(f->slaves, s);

        /* Make sure the new slave is driven correctly depending on 
           the current state of the filter. */
        update(f, s);               
        return true;
}

static void
delete_slave(slave_t *s)
{
        /* Make sure the slave is not blocking on us, when we are not
           controlling this anymore. */
        s->slave_iface->enable(s->slave, s->filter->source_id);
        /* USER-TODO: Remove any slave associated connections */
        MM_FREE(s);
}

/* instrumentation_filter_master::remove_slave */
static void
ifm_remove_slave(conf_object_t *obj, conf_object_t *slave,
                 conf_object_t *provider)
{
        filter_t *f = filter_of_obj(obj);        
        VFORI(f->slaves, i) {
                slave_t *s = VGET(f->slaves, i);
                if (s->slave == slave && s->provider == provider) {
                        VREMOVE(f->slaves, i);
                        delete_slave(s);
                        return;
                }                
        }
        ASSERT(0);
}


/* instrumentation_filter_master::short_filter_config */
static char *
ifm_short_filter_config(conf_object_t *obj)
{
        UNUSED filter_t *f = filter_of_obj(obj);                
        strbuf_t out = SB_INIT;
        /* USER-TODO: Format a string with how the filter is configured. */
        sb_addfmt(&out, "%s", "unknown filter config");
        return sb_detach(&out);
}


static conf_object_t *
alloc_object(lang_void *arg)
{
        filter_t *f = MM_ZALLOC(1, filter_t);
        /* USER-TODO: initialize tool members */
        return &f->obj;
}

static void
pre_delete_instance(conf_object_t *obj)
{
        filter_t *f = filter_of_obj(obj);
        while (!VEMPTY(f->slaves))
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
                "empty_instrumentation_filter",
                &funcs);
        
        static const instrumentation_filter_master_interface_t ifm = {
                .set_source_id = ifm_set_source_id,
                .short_filter_config = ifm_short_filter_config,
                .add_slave = ifm_add_slave,
                .remove_slave = ifm_remove_slave,
        };
        SIM_REGISTER_INTERFACE(cl, instrumentation_filter_master, &ifm);

        /* USER-TODO: Add filter specific attributes */
}
