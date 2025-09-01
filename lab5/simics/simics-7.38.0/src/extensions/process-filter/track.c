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
#include <simics/util/strbuf.h>
#include <simics/simulator-iface/os-awareness-interfaces.h>

/* Forward declarations */
typedef struct track track_t;
typedef struct pattern_instance pattern_instance_t;

/* The process(es) the filter wants to enable instrumentation for */
typedef struct {
        const char *name;
        attr_value_t name_attr;              /* attr_string of name */
        track_t *parent;
        int64 active_count;                 /* instances currently scheduled */
        VECT(pattern_instance_t *) instances;
} pattern_t;

/* Detected processes live, that have matched a pattern  */
struct pattern_instance {
        node_id_t node_id;
        pattern_t *parent;

        /* Callback id:s */
        int destroy_id;
        int move_to_id;
        int move_from_id;
};

/* Instrumentation tools which are connected and are enabled/disabled */
typedef struct {
        struct {
                conf_object_t *obj;          /* slave/connection */
                const instrumentation_filter_slave_interface_t *iface;
        } slave;
        conf_object_t *trigger_obj;          /* provider */
} slave_t;

/* The process-filter conf_object */
struct track {
        conf_object_t obj;

        conf_object_t *cell;       /* The cell where os-awareness lives */
        unsigned source_id;                  /* filter "pin" to toggle */
        VECT(slave_t *) slaves;    /* The tool-connections we enable/disable */
        
        VECT(pattern_t *) processes;      /* The processes being filtered */
        
        /* OS-Awareness objects/interfaces */
        struct {
                conf_object_t *software_comp;
                const osa_component_interface_t *comp_iface;
                conf_object_t *admin;                
                const osa_control_v2_interface_t *ctrl_iface;
                const osa_node_tree_notification_interface_t *nt_iface;
                const osa_node_tree_query_interface_t *nt_query_iface;
                const osa_node_path_interface_t *np_iface;
                
                node_id_t root_id;
                bool tracker_enabled;
                int id;                      /* tracker-id */

                cancel_id_t notify_create_id;
                cancel_id_t notify_prop_id;
        } osa;
};

FORCE_INLINE track_t *
track_of_obj(conf_object_t *obj) { return (track_t *)obj; }

static void
remove_process_instance(track_t *t, pattern_t *p, pattern_instance_t *pi)
{
        t->osa.nt_iface->cancel_notify(t->osa.admin, pi->destroy_id);
        t->osa.nt_iface->cancel_notify(t->osa.admin, pi->move_to_id);
        t->osa.nt_iface->cancel_notify(t->osa.admin, pi->move_from_id);
        MM_FREE(pi);
}


/* Called when a tracked process instance dies. */
static void
destroy_cb(cbdata_call_t data, conf_object_t *admin, conf_object_t *cpu,
           node_id_t node_id)
{
        pattern_instance_t *pi = SIM_cbdata_data(&data);
        pattern_t *p= pi->parent;
        track_t *t = p->parent;

        remove_process_instance(t, p, pi);
        VREMOVE_FIRST_MATCH(p->instances, pi);        
}

/* Called when a tracked process instance starts to execute on a certain CPU */
static void
move_to_cb(cbdata_call_t data, conf_object_t *admin, conf_object_t *cpu,
           attr_value_t node_path)
{
        pattern_instance_t *pi = SIM_cbdata_data(&data);
        pattern_t *p = pi->parent;
        track_t *t = p->parent;

        VFORP(t->slaves, slave_t, s) {
                if (s->trigger_obj == cpu) {
                        s->slave.iface->enable(s->slave.obj, t->source_id);
                }
        }
}

/* Called when a tracked process stops executing on a certain CPU */
static void
move_from_cb(cbdata_call_t data, conf_object_t *admin, conf_object_t *cpu,
             attr_value_t node_path)
{
        pattern_instance_t *pi = SIM_cbdata_data(&data);
        pattern_t *p= pi->parent;
        track_t *t = p->parent;

        VFORP(t->slaves, slave_t, s) {
                if (s->trigger_obj == cpu) {
                        s->slave.iface->disable(s->slave.obj, t->source_id);
                }
        }
}

/* Add additional callbacks for a specific process-instance */
static void
track_process_instance(track_t *t, pattern_t *p, node_id_t node_id)
{
        pattern_instance_t *pi = MM_ZALLOC(1, pattern_instance_t);
        cbdata_t this = SIM_make_simple_cbdata(pi);

        /* Install new callbacks */
        int destroy_id = t->osa.nt_iface->notify_destroy(
                t->osa.admin, node_id, false, destroy_cb, this);

        int to_id = t->osa.nt_iface->notify_cpu_move_to(
                t->osa.admin, node_id, move_to_cb, this);

        int from_id = t->osa.nt_iface->notify_cpu_move_from(
                t->osa.admin, node_id, move_from_cb, this);
        
        pi->node_id = node_id;
        pi->parent = p;
        pi->destroy_id = destroy_id;
        pi->move_to_id = to_id;
        pi->move_from_id = from_id;
        VADD(p->instances, pi);                     
}


static void
untrack_process_instance(track_t *t, pattern_t *p, node_id_t node_id)        
{
        pattern_instance_t *searched_pi = NULL;
        VFORP(p->instances, pattern_instance_t, pi) {
                if (pi->node_id == node_id) {
                        searched_pi = pi;
                        break;
                }
        }
        ASSERT(searched_pi);
        remove_process_instance(t, p, searched_pi);
        VREMOVE_FIRST_MATCH(p->instances, searched_pi); 
}

static bool
matches_pattern(track_t *t, node_id_t node_id, pattern_t *p)
{
        attr_value_t nodes_info = t->osa.np_iface->matching_nodes(
                t->osa.admin, t->osa.root_id, p->name_attr);
        
        ASSERT(SIM_attr_is_list(nodes_info));
        ASSERT(SIM_attr_list_size(nodes_info) >= 2);
                
        attr_value_t ok = SIM_attr_list_item(nodes_info, 0);
        ASSERT(SIM_attr_boolean(ok));

        attr_value_t ids = SIM_attr_list_item(nodes_info, 1);
        ASSERT(SIM_attr_is_list(ids));

        for (int i = 0; i < SIM_attr_list_size(ids); i++) {
                attr_value_t id = SIM_attr_list_item(ids, i);
                ASSERT(SIM_attr_is_uint64(id));
                node_id_t nid = SIM_attr_integer(id);
                if (nid == node_id) {
                        SIM_attr_free(&nodes_info);
                        return true;
                }
        }
        SIM_attr_free(&nodes_info);
        return false;
}        

/* Called for each process node creation. We filter out only the
   process creation (not threads) and detects the processes we
   really follow. */
static void
process_created_cb(cbdata_call_t data, conf_object_t *admin,
                   conf_object_t *cpu, node_id_t node_id)
{
        track_t *t = SIM_cbdata_data(&data);

        VFORP(t->processes, pattern_t, p)
                if (matches_pattern(t, node_id, p))
                        track_process_instance(t, p, node_id);
}

/* Similar to process_created_cb, but this is when a process changes name,
   typically a fork()/exec() scenario, which is the most common case. */
static void
process_renamed_cb(cbdata_call_t data, conf_object_t *admin,
                   conf_object_t *cpu, node_id_t node_id,
                   const char *key, attr_value_t old_val, attr_value_t new_val)
{
        track_t *t = SIM_cbdata_data(&data);

        VFORP(t->processes, pattern_t, p) {
                VFORP(p->instances, pattern_instance_t, pi) {
                        if (pi->node_id == node_id) {
                                /* we are tracking this one, remove it */
                                untrack_process_instance(t, p, node_id);
                                break;
                        }
                }
        }

        VFORP(t->processes, pattern_t, p)
                if (matches_pattern(t, node_id, p))
                        track_process_instance(t, p, node_id);
}

/* Starting the tracker means it will disable VMP and do install various OSA
   callbacks to the simulation, so performance will be reduced. */
static void
start_tracker(track_t *t)
{
        /* Request the tracker to start  */
        attr_value_t a = t->osa.ctrl_iface->request(
                t->osa.admin, SIM_object_name(&t->obj));
        ASSERT(SIM_attr_list_size(a) == 2);
        attr_value_t v0 = SIM_attr_list_item(a, 0);
        attr_value_t v1 = SIM_attr_list_item(a, 1);
        
        ASSERT(SIM_attr_is_boolean(v0));
        bool ok = SIM_attr_boolean(v0);
        if (!ok) {
                ASSERT(SIM_attr_is_string(v1));
                const char *err = SIM_attr_string(v1);
                SIM_LOG_ERROR(&t->obj, 0,
                              "error from osa-framework request(): %s", err);
                SIM_free_attribute(a);
                return;
        }
        
        ASSERT(SIM_attr_is_integer(v1));                
        t->osa.id = SIM_attr_integer(v1);
        t->osa.tracker_enabled = true;

        /* Most OSA interface functions require a node ID. Retrieve the root_id
           from the component. Using the root ID in combination with the
           recursive flag makes it possible to get notifications for the entire
           node tree. */
        maybe_node_id_t root = t->osa.comp_iface->get_root_node(
                t->osa.software_comp);
        if (!root.valid) {
                SIM_LOG_ERROR(&t->obj, 0, "get_root_node() failed");
                SIM_free_attribute(a);
                return;
        }
        t->osa.root_id = root.id;

        cbdata_t this = SIM_make_simple_cbdata(t);

        /* Get a callback when a process is created */
        t->osa.notify_create_id = t->osa.nt_iface->notify_create(
                t->osa.admin, t->osa.root_id, true,
                process_created_cb, this);

        /* Get a callback when a process is renamed */
        t->osa.notify_prop_id = t->osa.nt_iface->notify_property_change(
                t->osa.admin, t->osa.root_id, "name", true,
                process_renamed_cb, this);
        SIM_free_attribute(a);
}

/* When the filter does not have anything to filter on anymore, we
   can stop the tracker, allowing faster execution again. */
static void
stop_tracker(track_t *t)
{
        if (!t->osa.tracker_enabled)
                return;
        t->osa.nt_iface->cancel_notify(t->osa.admin, t->osa.notify_create_id);
        t->osa.nt_iface->cancel_notify(t->osa.admin, t->osa.notify_prop_id);
        t->osa.ctrl_iface->release(t->osa.admin, t->osa.id);
        t->osa.id = -1;        
        t->osa.tracker_enabled = false;        
}

typedef VECT(node_id_t) node_id_vect_t;

/* Recursively traverse through the OSA node-tree trying to find
   processes matching p->name */
static void
track_existing_instances(track_t *t, pattern_t *p, node_id_t id)
{
        if (matches_pattern(t, id, p))
                track_process_instance(t, p, id);

        attr_value_t chn = t->osa.nt_query_iface->get_children(
                t->osa.admin, id);
        int len = SIM_attr_list_size(chn);
        for (int i = 0; i < len; i++) {
                attr_value_t a = SIM_attr_list_item(chn, i);
                node_id_t cid = SIM_attr_integer(a);
                track_existing_instances(t, p, cid);
        }
        SIM_free_attribute(chn);
}

static void
switch_slaves(track_t *t, bool enable, conf_object_t *trigger_obj)
{
        VFORP(t->slaves, slave_t, s) {
                if (s->trigger_obj == trigger_obj) {
                        if (enable)
                                s->slave.iface->enable(
                                        s->slave.obj, t->source_id);
                        else
                                s->slave.iface->disable(
                                        s->slave.obj, t->source_id);
                }
        }
}

/* Enable/disable instrumentation if a process is tracked/untracked and
   currently being executed on a processor */
static void
check_if_process_scheduled(track_t *t, pattern_t *p, bool enable)
{
        if (!t->osa.tracker_enabled)
                return;
        
        attr_value_t all = t->osa.nt_query_iface->get_all_processors(
                t->osa.admin);
        ASSERT(SIM_attr_is_list(all));
        
        for (int i = 0; i < SIM_attr_list_size(all); i++) {
                attr_value_t cpu = SIM_attr_list_item(all, i);
                conf_object_t *obj = SIM_attr_object(cpu);
                attr_value_t node_path =
                        t->osa.nt_query_iface->get_current_nodes(
                                t->osa.admin, t->osa.root_id, obj);

                int len = SIM_attr_list_size(node_path);
                for (int n = 0; n < len; n++) {
                        attr_value_t a = SIM_attr_list_item(node_path, n);
                        node_id_t id = SIM_attr_integer(a);

                        if (matches_pattern(t, id, p))
                                switch_slaves(t, enable, obj);
                }
        }
        SIM_free_attribute(all);
}

/* Returns false if there was a problem with the node-path pattern
   and SIM_attribute_error has been set with details. */
static bool
track_process(track_t *t, const char *process_name)
{
        /* Validate the name so it contains valid node-pattern */
        attr_value_t s = SIM_make_attr_string(process_name);
        attr_value_t nodes_info = t->osa.np_iface->matching_nodes(
                t->osa.admin, t->osa.root_id, s);

        attr_value_t ok = SIM_attr_list_item(nodes_info, 0);
        if (!SIM_attr_boolean(ok)) {
                char error_msg[1024];
                attr_value_t msg = SIM_attr_list_item(nodes_info, 1);
                sprintf(error_msg, "node-pattern '%s' is invalid: %s\n",
                       process_name, SIM_attr_string(msg));
                SIM_attribute_error(error_msg);
                SIM_attr_free(&s);
                SIM_attr_free(&nodes_info);
                return false;
        }
        SIM_attr_free(&nodes_info);

        pattern_t *p = MM_ZALLOC(1, pattern_t);
        p->name = MM_STRDUP(process_name);
        p->name_attr = SIM_make_attr_string(process_name);
        p->parent = t;
        VADD(t->processes, p);
        if (t->osa.tracker_enabled) {
                track_existing_instances(t, p, t->osa.root_id);
                check_if_process_scheduled(t, p, true);
        }
        return true;
}

static void
untrack_process(track_t *t, pattern_t *p)
{
        check_if_process_scheduled(t, p, false);
        
        /* Get rid of any callbacks associated with this process */
        VFORP(p->instances, pattern_instance_t, pi)
                remove_process_instance(t, p, pi);
        VFREE(p->instances);
        MM_FREE((void *)p->name);
        SIM_attr_free(&p->name_attr);
        VREMOVE_FIRST_MATCH(t->processes, p);
        MM_FREE(p);        
}

/* instrumentation_filter_master::set_source_id */
static void
ifm_set_source_id(conf_object_t *obj, unsigned source_id)
{
        track_t *t = track_of_obj(obj);
        t->source_id = source_id;
}

/* instrumentation_filter_master::add_slave */
static bool
ifm_add_slave(conf_object_t *obj, conf_object_t *slave,
              conf_object_t *trigger_obj)
{
        track_t *t = track_of_obj(obj);

        ASSERT(t->cell);
        ASSERT(trigger_obj);
        if (VT_object_cell(trigger_obj) != t->cell) {
                /* Cannot have slaves living in another cell */
                SIM_LOG_ERROR(
                        obj, 0,
                        "slave %s in different cell (%s vs %s)",
                        SIM_object_name(trigger_obj),
                        SIM_object_name(VT_object_cell(trigger_obj)),
                        SIM_object_name(t->cell));                        
                return false;
        }

        const instrumentation_filter_slave_interface_t *slave_ifc =
                SIM_C_GET_INTERFACE(slave, instrumentation_filter_slave);
        ASSERT(slave_ifc);
        slave_t *s = MM_MALLOC(1, slave_t);
        s->slave.obj = slave;
        s->slave.iface = slave_ifc;
        s->trigger_obj = trigger_obj;
        VADD(t->slaves, s);

        /* Disable by default */
        s->slave.iface->disable(s->slave.obj, t->source_id);
        VFORP(t->processes, pattern_t, p) 
                check_if_process_scheduled(t, p, true);
        
        return true;
}

/* instrumentation_filter_master::remove_slave */
static void
ifm_remove_slave(conf_object_t *obj, conf_object_t *slave,
                 conf_object_t *trigger_obj)
{
        track_t *t = track_of_obj(obj);        
        VFORI(t->slaves, i) {
                slave_t *s = VGET(t->slaves, i);
                if (s->slave.obj == slave && s->trigger_obj == trigger_obj) {
                        s->slave.iface->enable(s->slave.obj, t->source_id);
                        VREMOVE(t->slaves, i);
                        MM_FREE(s);
                        return;
                }                
        }
        ASSERT(0);
}

/* instrumentation_filter_master::short_filter_config */
static char *
ifm_short_filter_config(conf_object_t *obj)
{
        track_t *t = track_of_obj(obj);                
        strbuf_t out = SB_INIT;
        VFORI(t->processes, i) {
                pattern_t *p = VGET(t->processes, i);
                sb_addfmt(&out, "%s%s", i > 0 ? ", " : "", p->name);
        }
        return sb_detach(&out);
}

static attr_value_t
get_software_comp(void *param, conf_object_t *obj, attr_value_t *idx)
{
        track_t *t = track_of_obj(obj);
        return SIM_make_attr_object(t->osa.software_comp);
}

#define CHECK_IFACE(iface, iface_name) do {             \
        if (!iface) {                                   \
                SIM_attribute_error(                    \
                "object does not implement the "        \
                #iface_name                             \
                " interface");                          \
                return Sim_Set_Interface_Not_Found;     \
        }                                               \
} while(0)
            
static set_error_t
set_software_comp(void *arg, conf_object_t *obj, attr_value_t *val,
                  attr_value_t *idx)
{
        track_t *t = track_of_obj(obj);
        conf_object_t *sw_comp = SIM_attr_object(*val);

        const osa_component_interface_t *comp_iface =
                SIM_C_GET_INTERFACE(sw_comp, osa_component);
        CHECK_IFACE(comp_iface, osa_component);        
        t->osa.software_comp = sw_comp;
        t->osa.comp_iface = comp_iface;

        /* Get the osa_admin object from the component, this will be used to
           access the node tree interfaces. */
        conf_object_t *admin = t->osa.comp_iface->get_admin(
                t->osa.software_comp);
                
        const osa_control_v2_interface_t *ctrl_iface =
                SIM_C_GET_INTERFACE(admin, osa_control_v2);

        const osa_node_tree_notification_interface_t *nt_iface =
                SIM_C_GET_INTERFACE(admin, osa_node_tree_notification);

        const osa_node_tree_query_interface_t *nt_query_iface =
                SIM_C_GET_INTERFACE(admin, osa_node_tree_query);

        const osa_node_path_interface_t *np_iface =
                SIM_C_GET_INTERFACE(admin, osa_node_path);
        
        CHECK_IFACE(ctrl_iface, osa_control_v2);
        CHECK_IFACE(nt_iface, osa_node_tree_notification);        
        CHECK_IFACE(nt_query_iface, osa_node_tree_query);
        
        t->osa.admin = admin;        
        t->osa.ctrl_iface = ctrl_iface;
        t->osa.nt_iface = nt_iface;
        t->osa.nt_query_iface = nt_query_iface;
        t->osa.np_iface = np_iface;

        t->cell = VT_object_cell(sw_comp);
        return Sim_Set_Ok;
}


static attr_value_t
get_tracked_processes(void *param, conf_object_t *obj, attr_value_t *idx)
{
        track_t *t = track_of_obj(obj);
        attr_value_t ret = SIM_alloc_attr_list(VLEN(t->processes));
        VFORI(t->processes, i) {
                pattern_t *p = VGET(t->processes, i);
                SIM_attr_list_set_item(
                        &ret, i,
                        SIM_make_attr_string(p->name));
        }
        return ret;
}

/* Compare if any of our tracked processes have been removed from the list. */
static void
check_for_removed_processes(track_t *t, attr_value_t *val)
{
        int len = SIM_attr_list_size(*val);
        VECT(pattern_t *) remove = VNULL;
        VFORP(t->processes, pattern_t, p) {
                bool found = false;
                for (int i = 0; i < len; i++) {
                        const char *attr_process =
                                SIM_attr_string(SIM_attr_list_item(*val, i));
                        if (strcmp(p->name, attr_process) == 0) {
                                found = true;
                                break;
                        }
                }
                if (!found)
                        VADD(remove, p);                
        }
        /* Cannot remove items in the VECT which we are iterating over,
           do it afterwards instead. */
        VFORP(remove, pattern_t, p)
                untrack_process(t, p);
        VFREE(remove);
}

/* Compare if there are new processes in the attr-list, not already tracked */
static bool
check_for_added_processes(track_t *t, attr_value_t *val)
{
        int len = SIM_attr_list_size(*val);
        for (int i = 0; i < len; i++) {
                const char *attr_process =
                        SIM_attr_string(SIM_attr_list_item(*val, i));

                bool found = false;                
                VFORP(t->processes, pattern_t, p) {
                        if (strcmp(p->name, attr_process) == 0) {
                                found = true;
                                break;
                        }
                }
                if (!found) {
                        if (!track_process(t, attr_process))
                                return false; /* invalid name */
                }
        }
        return true;
}

static set_error_t
set_tracked_processes(void *arg, conf_object_t *obj, attr_value_t *val,
                      attr_value_t *idx)
{
        track_t *t = track_of_obj(obj);
        int len = SIM_attr_list_size(*val);
        
        if (len && VLEN(t->processes) == 0)
                start_tracker(t);

        check_for_removed_processes(t, val);
        if (!check_for_added_processes(t, val))
                return Sim_Set_Illegal_Value;
                
        if (len == 0 && t->osa.tracker_enabled)
                stop_tracker(t);
        return Sim_Set_Ok;
}

static conf_object_t *
alloc_object(lang_void *arg)
{
        track_t *t = MM_ZALLOC(1, track_t);
        VINIT(t->processes);
        return &t->obj;
}

static void
pre_delete_instance(conf_object_t *obj)
{
        track_t *t = track_of_obj(obj);
        VECT(pattern_t *) remove = VNULL;

        VFORP(t->processes, pattern_t, p) {
                VFORP(p->instances, pattern_instance_t, pi)
                        remove_process_instance(t, p, pi);
                VFREE(p->instances);
                VADD(remove, p);
        }
        VFORP(remove, pattern_t, p)
                untrack_process(t, p);

        VFREE(remove);        
        
        /* Make sure that any connected slaves are not blocking
           by this object when removed. */           
        VFORP(t->slaves, slave_t, s)
                s->slave.iface->enable(s->slave.obj, t->source_id);
        stop_tracker(t);
}


/* Cleanup of deleted object instance */
static int
delete_instance(conf_object_t *obj)
{
        track_t *t = track_of_obj(obj);
        MM_FREE(t);
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
                "process_filter",
                &funcs);
        
        static const instrumentation_filter_master_interface_t ifm = {
                .set_source_id = ifm_set_source_id,
                .short_filter_config = ifm_short_filter_config,
                .add_slave = ifm_add_slave,
                .remove_slave = ifm_remove_slave,
        };
        SIM_REGISTER_INTERFACE(cl, instrumentation_filter_master, &ifm);

        SIM_register_typed_attribute(
                cl, "software_comp",
                get_software_comp, NULL,
                set_software_comp, NULL,
                Sim_Attr_Required,
                "o", NULL,
                "OSA software component to use.");

        SIM_register_typed_attribute(
                cl, "tracked_processes",
                get_tracked_processes, NULL,
                set_tracked_processes, NULL,
                Sim_Attr_Pseudo,
                "[s*]", NULL,
                "The processes being tracked.");        
}
