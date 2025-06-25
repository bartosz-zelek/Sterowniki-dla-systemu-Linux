/*
  © 2022 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <simics/base/iface-call.h>
#include <simics/base/log.h>
#include <simics/base/notifier.h>
#include <simics/devs/frequency.h>
#include <simics/simulator/control.h>
#include <simics/simulator/callbacks.h>
#include <simics/model-iface/execute.h>
#include <simics/model-iface/cycle-event.h>
#include <simics/model-iface/event-delta.h>
#include <simics/processor/processor-platform.h>
#include <simics/simulator/conf-object.h>
#include <simics/model-iface/co-execute.h>
#include <simics/model-iface/concurrency.h>

typedef struct {
        conf_object_t obj;
        IFACE_OBJ(event_handler) vtime;
        IFACE_OBJ(frequency) freq_provider;

        /* Delta imposed on us from event_delta->set_delta. */
        uint64 delta;

        /* SIM_cycle_count() stamp obtained from the executor. The executor is
               'executor_elapsed' + SIM_cycle_count() - executor_stamp
           cycles ahead of the cycle clock associated with this object. */
        conf_object_t *executor;
        uint64 executor_stamp;
        uint64 executor_elapsed;

        IFACE_OBJ(execute) execute;

        /* stop event posted */
        bool executor_stop_event_posted;

        /* execute->stop() called */
        bool executor_stop_requested;

        /* This object is inside execute.run() */
        bool is_async_bridge_running;

        /* This object has called the executor's execute.run(). */
        bool is_executor_running;

        int sync_count;

        IFACE_OBJ(execute_control) execute_control;
        IFACE_OBJ(concurrency_mode) concurrency_mode;

        /* Execution */
        execute_environ_t *execute_env;
} async_bridge_t;

static event_class_t *executor_stop_ev;

static inline async_bridge_t *
async_bridge_of_obj(conf_object_t *obj)
{
        return (async_bridge_t *)obj;
}

/* One should calculate current executor_elapsed before using it.
   This function does the calculation. */
static void
calculate_current_executor_elapsed(async_bridge_t *x)
{
        uint64 stamp = SIM_cycle_count(x->executor);
        x->executor_elapsed += stamp - x->executor_stamp;
        x->executor_stamp = stamp;        
}

static void
update_delta(async_bridge_t *x)
{
        /* Only advance our clock when we are running. */
        if (!x->is_async_bridge_running)
                return;

        calculate_current_executor_elapsed(x);

        if (x->executor_elapsed < x->delta) {
                x->delta -= x->executor_elapsed;
                x->executor_elapsed = 0;
        } else {
                x->executor_elapsed -= x->delta;
                x->delta = 0;
        }
}

static uint64
get_delta(conf_object_t *obj, conf_object_t *ev_handler)
{
        async_bridge_t *x = async_bridge_of_obj(obj);
        update_delta(x);
        return x->delta;
}

static uint64
set_delta(conf_object_t *obj, conf_object_t *ev_handler,
          const event_class_t *next_event_ec, uint64 delta)
{
        async_bridge_t *x = async_bridge_of_obj(obj);
        if (delta >= x->executor_elapsed) {
                if (x->executor_stop_event_posted) {
                        SIM_event_cancel_time(
                                x->executor, executor_stop_ev, &x->obj,
                                NULL, NULL);
                        x->executor_stop_event_posted = false;
                }
                if (x->sync_count == 0) {
                        SIM_event_post_cycle(
                                x->executor, executor_stop_ev, &x->obj,
                                delta - x->executor_elapsed, NULL);
                        x->executor_stop_event_posted = true;
                }
        }
        x->delta = delta;
        return delta;
}

static void
stop_executor(async_bridge_t *x)
{
        x->executor_stop_requested = true;
        CALL(x->execute, stop)();
}

static void
sync_mode_execution_loop(async_bridge_t *x)
{
        SIM_LOG_INFO(3, &x->obj, 0, "Starting sync mode execution loop");
        x->executor_stop_requested = false;
        for (;;) {
                VT_execute(x->execute_env);
                if (x->sync_count == 0)
                        break;
                //VT_dispatch_async_events();
        }
        SIM_LOG_INFO(3, &x->obj, 0, "Leaving sync mode execution loop");
}

static int
enter_sync_mode(conf_object_t *obj)
{
        async_bridge_t *x = async_bridge_of_obj(obj);
        x->sync_count++;

        /* Ensure that the outer complete_sync_operation invokes stop_executor */
        if (x->sync_count == 1 && !x->is_executor_running) {
                SIM_LOG_INFO(
                        2, obj, 0, "Entering sync mode: starting execution");
                sync_mode_execution_loop(x);
        } else {
                SIM_LOG_INFO(
                        2, obj, 0, "Entering sync mode: already running"
                        " (sync_count = %d)", x->sync_count);
        }

        return x->sync_count;
}

static int
leave_sync_mode(conf_object_t *obj)
{
        async_bridge_t *x = async_bridge_of_obj(obj);
        x->sync_count--;
        ASSERT_MSG(x->sync_count >= 0, "synchronous_mode.leave has no matching"
                   " synchronous_mode.enter");

        SIM_LOG_INFO(2, obj, 0, "Leaving sync mode (sync_count =  %d)",
                     x->sync_count);

        if (x->sync_count == 0 && !x->executor_stop_requested) {
                if (x->is_async_bridge_running) {
                        update_delta(x);
                        if (x->delta == 0)
                                stop_executor(x);
                } else {
                        SIM_LOG_INFO(3, obj, 0, "Stopping execute environment");
                        VT_stop_execution(x->execute_env);
                        stop_executor(x);
                }
        }
        return x->sync_count;
}

/* Stop event posted on co-executor queue. */
static void
executor_stop_event_clbk(conf_object_t *obj, lang_void *data)
{
        async_bridge_t *x = async_bridge_of_obj(obj);
        if (x->sync_count == 0 && !x->executor_stop_requested)
                stop_executor(x);
}

/* execute.run method. */
static void
run(conf_object_t *obj)
{
        async_bridge_t *x = async_bridge_of_obj(obj);

        ASSERT(x->sync_count == 0);
        x->is_async_bridge_running = true;

        update_delta(x);
        while (x->is_async_bridge_running && !VT_async_events_pending()) {
                if (x->delta == 0)
                        CALL(x->vtime, handle_event)();
                else {
                        ASSERT(x->executor_stop_event_posted);
                        x->is_executor_running = true;
                        x->executor_stop_requested = false;
                        CALL(x->execute, run)();
                        x->is_executor_running = false;
                }
                update_delta(x);
        }
        x->is_async_bridge_running = false;

        if (x->sync_count > 0)
                sync_mode_execution_loop(x);

        ASSERT(x->sync_count == 0);
}

/* execute.stop method. */
static void
stop(conf_object_t *obj)
{
        async_bridge_t *x = async_bridge_of_obj(obj);
        if (x->is_async_bridge_running)
                update_delta(x);
        x->is_async_bridge_running = false;
        CALL(x->vtime, stop)();

        /* propagate to client. */
        if (x->is_executor_running && !x->executor_stop_requested
            && x->sync_count == 0)
                stop_executor(x);
}

static double
frequency_get(conf_object_t *obj)
{
        async_bridge_t *x = async_bridge_of_obj(obj);
        return x->freq_provider.obj ? CALL(x->freq_provider, get)() : 1.0;
}

static void
frequency_changed_notifier(conf_object_t *obj, conf_object_t *notifier,
                           lang_void *dummy)
{
        /* We have changed frequency too since we just forward the
           provider frequency. */
        SIM_notify(obj, Sim_Notify_Frequency_Change);
}

static attr_value_t
get_catch_up_cycles(conf_object_t *obj)
{
        async_bridge_t *x = async_bridge_of_obj(obj);
        calculate_current_executor_elapsed(x);
        return SIM_make_attr_uint64(x->executor_elapsed);
}

static set_error_t
set_catch_up_cycles(conf_object_t *obj, attr_value_t *v)
{
        async_bridge_t *x = async_bridge_of_obj(obj);
        x->executor_elapsed = SIM_attr_integer(*v);
        if (SIM_object_is_configured(obj))
                x->executor_stamp = SIM_cycle_count(x->executor);
        return Sim_Set_Ok;
}

static attr_value_t
get_executor(conf_object_t *obj)
{
        async_bridge_t *x = async_bridge_of_obj(obj);
        return SIM_make_attr_object(x->executor);
}

static attr_value_t
execution_group(conf_object_t *obj, unsigned index)
{
        async_bridge_t *x = async_bridge_of_obj(obj);
        if (index == 0) {
                return SIM_make_attr_list(
                        2,
                        SIM_make_attr_object(obj),
                        SIM_make_attr_object(x->executor));
        } else {
                return SIM_make_attr_nil();
        }
}

static concurrency_mode_t
supported_modes(conf_object_t *obj)
{
    async_bridge_t *x = async_bridge_of_obj(obj);
    if (HAS_IFACE(x->concurrency_mode))
            return CALL(x->concurrency_mode, supported_modes)();
    else
            return Sim_Concurrency_Mode_Serialized;
}

static concurrency_mode_t
current_mode(conf_object_t *obj)
{
    async_bridge_t *x = async_bridge_of_obj(obj);
    if (HAS_IFACE(x->concurrency_mode))
            return CALL(x->concurrency_mode, current_mode)();
    else
            return Sim_Concurrency_Mode_Serialized;
}

static void
switch_mode(conf_object_t *NOTNULL obj, concurrency_mode_t mode)
{
    async_bridge_t *x = async_bridge_of_obj(obj);
    if (HAS_IFACE(x->concurrency_mode))
            CALL(x->concurrency_mode, switch_mode)(mode);
}

static void
message_pending(conf_object_t *obj)
{
    async_bridge_t *x = async_bridge_of_obj(obj);
    if (HAS_IFACE(x->execute_control))
            CALL(x->execute_control, message_pending)();
}

static void
yield_request(conf_object_t *obj)
{
    async_bridge_t *x = async_bridge_of_obj(obj);
    if (HAS_IFACE(x->execute_control))
            CALL(x->execute_control, yield_request)();
}

static conf_object_t *
alloc_object(conf_class_t *cls)
{
        async_bridge_t *x = MM_ZALLOC(1, async_bridge_t);
        return &x->obj;
}

static void *
init_object(conf_object_t *obj)
{
        async_bridge_t *x = async_bridge_of_obj(obj);
        conf_object_t *vtime = SIM_object_descendant(obj, "vtime");
        INIT_REQUIRED_IFACE(&x->vtime, event_handler, vtime);

        /* Try object parent first to make the case when class A has
           subobjects A.clock, A.clock.async_bridge work. */
        x->executor = SIM_object_parent(obj);
        if (!INIT_IFACE(&x->execute, execute, x->executor)) {
                x->executor = SIM_port_object_parent(obj);
                INIT_IFACE(&x->execute, execute, x->executor);
        }
        FATAL_ERROR_IF(!x->executor,
                       "async-bridge objects should always be instantiated"
                       " as subobjects");
        FATAL_ERROR_IF(!HAS_IFACE(x->execute),
                       "no execute interface found in '%s'",
                       SIM_object_name(x->executor));

        attr_value_t val = SIM_make_attr_boolean(true);
        SIM_set_attribute(x->executor, "do_not_schedule", &val);


        INIT_IFACE(&x->freq_provider, frequency, x->executor);
        SIM_add_notifier(x->executor, Sim_Notify_Frequency_Change,
                         &x->obj, frequency_changed_notifier, NULL);
        INIT_IFACE(&x->execute_control, execute_control, x->executor);
        INIT_IFACE(&x->concurrency_mode, concurrency_mode, x->executor);

        x->execute_env = VT_create_execute_environ();
        return x;
}

static void
deinit_object(conf_object_t *obj)
{
        async_bridge_t *x = async_bridge_of_obj(obj);

        FATAL_ERROR_IF(x->sync_count != 0,
                       "Deletion of async-bridge objects in the middle"
                       " of operation is not supported");
        VT_free_execute_environ(x->execute_env);
}

static void
finalized(conf_object_t *obj)
{
        async_bridge_t *x = async_bridge_of_obj(obj);
        x->executor_stamp = SIM_cycle_count(x->executor);
        VT_set_execute_environ(x->executor, x->execute_env);
        VT_activate_executor(x->execute_env, x->executor);
}

static void
initialize_async_bridge()
{
        static const class_info_t funcs = {
                .alloc = alloc_object,
                .init = init_object,
                .deinit = deinit_object,
                .objects_finalized = finalized,
                .short_desc = "async-bridge class",
                .description =
                "The <class>async-bridge</class> class is used to decouple"
                " an executor from the Simics scheduler and allow it to run"
                " until synchronous requests have been completed."
                "\n\nDetailed description of <class>async-bridge</class>"
                " usage can be found in <file>README.md</file> located in"
                " <file>src/extensions/async-bridge</file>."
        };
        conf_class_t *cls = SIM_create_class("async-bridge", &funcs);

        static const execute_interface_t execute_iface = {
                .run = run,
                .stop = stop,
        };
        SIM_REGISTER_INTERFACE(cls, execute, &execute_iface);

        static const event_delta_interface_t event_delta_iface = {
                .set_delta = set_delta,
                .get_delta = get_delta,
        };
        SIM_REGISTER_INTERFACE(cls, event_delta, &event_delta_iface);

        static const frequency_interface_t freq_iface = {
                .get = frequency_get,
        };
        SIM_REGISTER_INTERFACE(cls, frequency, &freq_iface);

        static const synchronous_mode_interface_t sync_mode_iface = {
                .enter = enter_sync_mode,
                .leave = leave_sync_mode,
        };
        SIM_REGISTER_INTERFACE(cls, synchronous_mode, &sync_mode_iface);

        static const concurrency_group_interface_t cgroup_iface = {
                .execution_group = execution_group,
        };
        SIM_REGISTER_INTERFACE(cls, concurrency_group, &cgroup_iface);

        static const concurrency_mode_interface_t concurrency_mode_iface = {
                .supported_modes = supported_modes,
                .current_mode = current_mode,
                .switch_mode = switch_mode,
        };
        SIM_REGISTER_INTERFACE(cls, concurrency_mode, &concurrency_mode_iface);

        static const execute_control_interface_t execute_control_iface = {
                .message_pending = message_pending,
                .yield_request = yield_request,
        };
        SIM_REGISTER_INTERFACE(cls, execute_control, &execute_control_iface);

        SIM_register_attribute(
                cls, "catch_up_cycles",
                get_catch_up_cycles, set_catch_up_cycles,
                Sim_Attr_Optional, "i",
                "Number of cycles to execute to be in sync.");

        SIM_register_attribute(
                cls, "executor", get_executor, NULL, Sim_Attr_Pseudo, "o",
                "Execute object driven by async-bridge");

        conf_class_t *active_clock_cls = SIM_get_class("clock-extension");
        SIM_extend_class(cls, active_clock_cls);

        SIM_register_notifier(cls, Sim_Notify_Frequency_Change, NULL);

        executor_stop_ev = SIM_register_event(
                "Internal: stop", cls, Sim_EC_Notsaved,
                executor_stop_event_clbk, NULL, NULL, NULL, NULL);
}

void
init_local()
{
        initialize_async_bridge();
}
