/*
  © 2010 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <simics/simulator-api.h>
#include <simics/util/os.h>

typedef int64 msec_t; /* milliseconds of real time */
typedef int64 usec_t; /* microseconds of real time */
typedef int64 ticks_t; /* ticks of real-time clock */

#if defined(_WIN32)
static ticks_t ticks_per_second = 0;
#else /* !_WIN32 */
#define ticks_per_second ((ticks_t)1000000)
#endif /* !_WIN32 */

#define STRINGIFY(x) STRINGIFY1(x)
#define STRINGIFY1(x) #x

#define DEFAULT_CHECK_INTERVAL 100 /* ten times per second (simulator time) */
#define DEFAULT_SPEED 1.0 /* run at 100% of real-time */
#define DEFAULT_DRIFT_COMPENSATE 0.1
#define DEFAULT_MAX_OVERSLEEP 10000 /* 10 ms; this should be a
                                                 reasonable default */
/* An anchor point is a point in the past when realtime and simtime were
   synchronized. */
typedef struct {
        ticks_t realtime; /* realtime of anchor point (ticks) */
        double simtime; /* simtime of anchor point (s) */
} anchor_t;

typedef struct {
        conf_object_t obj;
        conf_object_t *clock;
        msec_t check_interval; /* poll how often? */
        double speed; /* fraction of realtime */
        int enabled;
        double drift_compensate; /* how fast to compensate for drift? */
        usec_t max_oversleep; /* sleep() may oversleep by at most this much */

        /* The far anchor is set when the simulation starts running. The near
           anchor is set in the beginning of each check interval. We have a
           hard limit (determined by drift_compensate) on how much we are
           allowed to drift from the near anchor; subject to this restriction,
           we do our best to not drift from the far anchor. (Of course, if the
           host is not fast enough, we might drift no matter what.) */
        anchor_t far_anchor;
        anchor_t near_anchor;
} realtime_object_t;


static event_class_t *event_rt_check;

static hap_type_t hap_realtime_enabled;

static void update_callbacks(realtime_object_t *rt);

/* ceil(n/d). Assumes d > 0. */
FORCE_INLINE ticks_t
ceil_div(ticks_t n, ticks_t d)
{
        return (n - 1)/d + 1;
}

static ticks_t
get_realtime()
{
#if defined(_WIN32)
        LARGE_INTEGER t;
        QueryPerformanceCounter(&t);
        return t.QuadPart;
#else /* !_WIN32 */
        return os_current_time_us();
#endif /* !_WIN32 */
}

/* Reset an anchor point; that is, proclaim that the present moment is the
   point against which we will synchronize time. */
FORCE_INLINE void
reset_anchor_point(anchor_t *anchor, double simtime)
{
        anchor->simtime = simtime;
        anchor->realtime = get_realtime();
}

/* Reset all anchor points. */
static void
reset_anchor_points(realtime_object_t *rt)
{
        reset_anchor_point(&rt->near_anchor, SIM_time(rt->clock));
        rt->far_anchor = rt->near_anchor;
}

/* Called when simulation starts (or resumes). */
static void
real_time_continue(lang_void *callback_data, conf_object_t *trigger_obj)
{
        realtime_object_t *rt = (realtime_object_t *)callback_data;
        reset_anchor_points(rt);
}

static conf_object_t *
realtime_alloc_object(void *data)
{
        static bool allocated = false;

        /* Make sure that we don't create multiple instances. */
        if (allocated) {
                pr_err("Can only have one instance of the realtime class.\n");
                return NULL;
        }

        allocated = true;

        realtime_object_t *rt = MM_ZALLOC(1, realtime_object_t);
        return &rt->obj;
}

static void *
realtime_init_object(conf_object_t *obj, void *data)
{
        realtime_object_t *rt = (realtime_object_t *)obj;
        rt->check_interval = DEFAULT_CHECK_INTERVAL;
        rt->speed = DEFAULT_SPEED;
        rt->drift_compensate = DEFAULT_DRIFT_COMPENSATE;
        rt->max_oversleep = DEFAULT_MAX_OVERSLEEP;
        return rt;
}

static void
realtime_pre_delete_instance(conf_object_t *obj)
{
        realtime_object_t *rt = (realtime_object_t *)obj;

        /* Remove any existing callbacks. */
        SIM_event_cancel_time(rt->clock, event_rt_check, &rt->obj, NULL, NULL);
        SIM_hap_delete_callback("Core_Continuation",
                                (obj_hap_func_t)real_time_continue, rt);
}

static int
realtime_delete_instance(conf_object_t *obj)
{
        realtime_object_t *rt = (realtime_object_t *)obj;
        MM_FREE(rt);
        return 0;
}

static void
sleep_ticks(ticks_t duration)
{
#if defined(_WIN32)
        DWORD msec = duration/ceil_div(ticks_per_second, 1000);
        if (msec > 0)
                Sleep(msec);
#else /* !_WIN32 */
        if (duration > 0)
                usleep(duration);
#endif /* !_WIN32 */
}

/* Sleep until the specified time has come. max_oversleep is an upper bound on
   how many ticks more than requested a call to sleep_ticks() may take. */
static void
wait_until(ticks_t end_of_wait, ticks_t max_oversleep)
{
        ticks_t end_of_sleep = end_of_wait - max_oversleep;
        ticks_t current_time;

        /* We must sleep repeatedly until we have slept enough, since we are
           awakened by signals all the time. And set the alarm clock
           max_oversleep ticks early, to make sure we don't oversleep; when we
           can't risk sleeping any longer, busy-wait the rest of the time. */
        while (end_of_sleep > (current_time = get_realtime()))
                sleep_ticks(end_of_sleep - current_time);
        while (end_of_wait > get_realtime())
                continue;
}

/* Given the current simulated time, an anchor point, and the speed of
   simulated time relative to real time, compute the corresponding real
   time. */
FORCE_INLINE ticks_t
compute_realtime(double current_simtime, anchor_t anchor, double speed)
{
        double elapsed_simtime = ticks_per_second
                * (current_simtime - anchor.simtime);
        return elapsed_simtime / speed + anchor.realtime;
}

/* Compute the desired value of real time for this instant in simulated time,
   given that we would like to not drift from the far anchor, but must keep the
   speed relative to the near anchor within a bound dictated by
   drift_compensate. */
static ticks_t
desired_realtime(realtime_object_t *rt, double current_simtime)
{
        ticks_t realt_min = compute_realtime(
                current_simtime, rt->near_anchor,
                rt->speed*(1.0 + rt->drift_compensate));
        ticks_t realt_max = compute_realtime(
                current_simtime, rt->near_anchor,
                rt->speed/(1.0 + rt->drift_compensate));
        ticks_t realt = compute_realtime(current_simtime,
                                         rt->far_anchor, rt->speed);
        return MIN(realt_max, MAX(realt, realt_min));
}

/* Periodic check. This is called periodically. It may sleep if simulated time
   is ahead of real time. It returns a value between 0.0 and 1.0 indicating how
   much useful work the processors should perform during the next quantum; if
   simulated time is behind real time, this return value can be less than
   1.0. */
static void
real_time_check(conf_object_t *obj, void *ptr)
{
        realtime_object_t *rt = (realtime_object_t *)obj;

        SIM_LOG_INFO(3, &rt->obj, 0, "Running periodic realtime check.");

        /* Compute the desired value of real time for this instant in simulated
           time, given that we would like to not drift from the far anchor, but
           must keep the speed relative to the near anchor within a bound
           dictated by drift_compensate. */
        double simt = SIM_time(rt->clock);
        ticks_t desired_realt = desired_realtime(rt, simt);

        wait_until(desired_realt,
                   ceil_div(ticks_per_second
                            * rt->max_oversleep, 1000000));

        /* Start a new interval by resetting the near anchor and reposting the
           callback. */
        reset_anchor_point(&rt->near_anchor, simt);
        SIM_event_post_time(rt->clock, event_rt_check, obj, rt->check_interval/1e3, NULL);
}

static void
update_callbacks(realtime_object_t *rt)
{
        /* Remove any existing callbacks. */
        SIM_event_cancel_time(rt->clock, event_rt_check, &rt->obj, NULL, NULL);
        SIM_hap_delete_callback("Core_Continuation",
                                (obj_hap_func_t)real_time_continue, rt);

        /* Set up callbacks if necessary. */
        if (rt->enabled) {
                SIM_event_post_time(rt->clock, event_rt_check,
                                    &rt->obj, rt->check_interval/1e3, NULL);
                SIM_hap_add_callback("Core_Continuation",
                                     (obj_hap_func_t)real_time_continue, rt);
                reset_anchor_points(rt);
        }
}

static attr_value_t
get_clock_object(void *id, conf_object_t *obj, attr_value_t *idx)
{
        realtime_object_t *rt = (realtime_object_t *)obj;
        return SIM_make_attr_object(rt->clock);
}

static set_error_t
set_clock_object(void *id, conf_object_t *obj,
                 attr_value_t *val, attr_value_t *idx)
{
        realtime_object_t *rt = (realtime_object_t *)obj;
        if (rt->clock == NULL) {
                rt->clock = SIM_attr_object(*val);
                return Sim_Set_Ok;
        } else {
                return Sim_Set_Not_Writable;
        }
}

static attr_value_t
get_speed(void *_id, conf_object_t *obj, attr_value_t *idx)
{
        realtime_object_t *rt = (realtime_object_t *)obj;
        return SIM_make_attr_floating(rt->speed);
}

static set_error_t
set_speed(void *id, conf_object_t *obj, attr_value_t *val, attr_value_t *idx)
{
        realtime_object_t *rt = (realtime_object_t *)obj;
        if (SIM_attr_floating(*val) <= 0.0)
                return Sim_Set_Illegal_Value;
        rt->speed = SIM_attr_floating(*val);
        reset_anchor_points(rt);
        return Sim_Set_Ok;
}

static attr_value_t
get_check_interval(void *_id, conf_object_t *obj, attr_value_t *idx)
{
        realtime_object_t *rt = (realtime_object_t *)obj;
        return SIM_make_attr_uint64(rt->check_interval);
}

static set_error_t
set_check_interval(void *id, conf_object_t *obj, attr_value_t *val, 
                   attr_value_t *idx)
{
        realtime_object_t *rt = (realtime_object_t *)obj;
        if (SIM_attr_integer(*val) <= 0)
                return Sim_Set_Illegal_Value;
        rt->check_interval = SIM_attr_integer(*val);
        reset_anchor_points(rt);
        return Sim_Set_Ok;
}

static attr_value_t
get_enabled(void *_id, conf_object_t *obj, attr_value_t *idx)
{
        realtime_object_t *rt = (realtime_object_t *)obj;
        return SIM_make_attr_boolean(rt->enabled);
}

static set_error_t
set_enabled(void *id, conf_object_t *obj, attr_value_t *val, attr_value_t *idx)
{
        realtime_object_t *rt = (realtime_object_t *)obj;
        rt->enabled = !!SIM_attr_boolean(*val);
        update_callbacks(rt);
        SIM_c_hap_occurred_always(hap_realtime_enabled, obj, 0, rt->enabled);
        return Sim_Set_Ok;
}

static attr_value_t
get_drift_compensate(void *_id, conf_object_t *obj, attr_value_t *idx)
{
        realtime_object_t *rt = (realtime_object_t *)obj;
        return SIM_make_attr_floating(rt->drift_compensate);
}

static set_error_t
set_drift_compensate(void *id, conf_object_t *obj, attr_value_t *val,
                     attr_value_t *idx)
{
        realtime_object_t *rt = (realtime_object_t *)obj;
        double comp = SIM_attr_floating(*val);
        if (comp < 0.0)
                return Sim_Set_Illegal_Value;
        rt->drift_compensate = comp;
        return Sim_Set_Ok;
}

static attr_value_t
get_rtc_freq(void *data, conf_object_t *obj, attr_value_t *idx)
{
        return SIM_make_attr_uint64(ticks_per_second);
}

static attr_value_t
get_max_oversleep(void *_id, conf_object_t *obj, attr_value_t *idx)
{
        realtime_object_t *rt = (realtime_object_t *)obj;
        return SIM_make_attr_uint64(rt->max_oversleep);
}

static set_error_t
set_max_oversleep(void *id, conf_object_t *obj, attr_value_t *val,
                  attr_value_t *idx)
{
        realtime_object_t *rt = (realtime_object_t *)obj;
        usec_t over = SIM_attr_integer(*val);
        if (over < 0 || over > 1000000)
                return Sim_Set_Illegal_Value;
        rt->max_oversleep = over;
        return Sim_Set_Ok;
}

void
init_local()
{
        const class_data_t cd = {
                .alloc_object = realtime_alloc_object,
                .init_object = realtime_init_object,
                .pre_delete_instance = realtime_pre_delete_instance,
                .delete_instance = realtime_delete_instance,
                .description =
                "In some cases simulated time may run faster than real time;"
                " this can happen if the OS is in a tight idle loop or an"
                " instruction halts execution waiting for an interrupt, or if"
                " the host machine is simply sufficiently fast. This can cause"
                " problems for programs that interact with the real world (for"
                " example the user), since time-outs may expire really fast. A"
                " <obj>realtime</obj> object will, when enabled, periodically"
                " check the simulation speed and wait for a while if it is too"
                " high.",
                .class_desc = "attempts to run no faster than real time",
                .kind = Sim_Class_Kind_Pseudo
        };
        conf_class_t *cc = SIM_register_class("realtime", &cd);

        SIM_register_typed_attribute(
                cc, "clock_object",
                get_clock_object, NULL, set_clock_object, NULL,
                Sim_Attr_Required, "o", NULL,
                "The object used to measure simulated time. This can be"
                " any processor in the system.");
        SIM_register_typed_attribute(
                cc, "speed",
                get_speed, NULL, set_speed, NULL,
                Sim_Attr_Pseudo, "f", NULL,
                "How fast the simulated time runs compared to real time. The"
                " default is 1, which means that simulated time runs at the"
                " same speed as real time. Note that Simics may be unable to"
                " run as fast as requested if the host is not fast enough.");
        SIM_register_typed_attribute(
                cc, "check_interval",
                get_check_interval, NULL, set_check_interval, NULL,
                Sim_Attr_Pseudo, "i", NULL,
                "How frequently elapsed simulated time should be compared to"
                " elapsed real time. Specified in milliseconds of simulated"
                " time; the default is " STRINGIFY(DEFAULT_CHECK_INTERVAL) "."
                " The actual time between comparisons may be less than this."
                " It will never be less than <cmd>set-time-quantum</cmd>, so"
                " check that setting if you need very fine-grained realtime"
                " behavior.");
        SIM_register_typed_attribute(
                cc, "enabled",
                get_enabled, NULL, set_enabled, NULL,
                Sim_Attr_Pseudo, "b", NULL,
                "Whether the real-time behavior is enabled or not. Defaults"
                " to false.");
        SIM_register_typed_attribute(
                cc, "drift_compensate",
                get_drift_compensate, NULL, set_drift_compensate, NULL,
                Sim_Attr_Pseudo, "f", NULL,
                "The <attr>speed</attr> attribute says how fast the simulation"
                " <em>should</em> run, but the actual speed will always"
                " deviate a little from that value even if the host is fast"
                " enough. To keep these errors from accumulating, the"
                " simulation speed has to be adjusted;"
                " <attr>drift_compensate</attr> regulates how much it may be"
                " adjusted. If set to (for example) 0.25, simulation speed may"
                " be increased or decreased by up to 25% if necessary to make"
                " up for any accumulated drift with respect to real time. If"
                " set to zero (the default), the simulation speed may not be"
                " changed at all from its set value.");
        SIM_register_typed_attribute(
                cc, "rtc_freq",
                get_rtc_freq, NULL, 0, NULL,
                Sim_Attr_Pseudo, "i", NULL,
                "Frequency of the realtime clock, in Hz.");
        SIM_register_typed_attribute(
                cc, "max_oversleep",
                get_max_oversleep, NULL, set_max_oversleep, NULL,
                Sim_Attr_Pseudo, "i", NULL,
                "The sleep system call will usually sleep somewhat longer than"
                " requested. To compensate for this, Simics will ask for a"
                " shorter sleep, and busy-wait the rest of the time. This"
                " parameter determines how much shorter, in microseconds."
                " (This will only help against delays imposed by the sleep"
                " implementation, not against delays caused by other"
                " processes on the system.)");

        event_rt_check = SIM_register_event("real-time-mode check",
                                            cc,
                                            Sim_EC_Notsaved,
                                            real_time_check,
                                            NULL, NULL, NULL, NULL);

        hap_realtime_enabled = SIM_hap_get_number("Realtime_Enabled");

#if defined(_WIN32)
        {
                LARGE_INTEGER freq;
                QueryPerformanceFrequency(&freq);
                ticks_per_second = freq.QuadPart;
        }
#endif /* _WIN32 */
}
