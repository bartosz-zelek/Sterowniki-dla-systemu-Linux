/*
  © 2021 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include "sample-core-timing.h"

#include <simics/simulator-iface/instrumentation-tool.h>
#include <simics/model-iface/cpu-instrumentation.h>
#include <simics/model-iface/processor-info.h>
#include <simics/devs/x86-cstate-iface.h>
#include <simics/devs/frequency.h>
#include <simics/arch/x86.h>

#include <simics/devs/telemetry-interface.h>

static const int STEPS_PERIOD = 10000;  // instructions
static const int IDLE_PERIOD = 10000;   // cycles

static conf_class_t  *connection_class;
static event_class_t *period_event;
static event_class_t *period_part2_event;
static event_class_t *idle_event;

typedef struct sample_core_timing_connection {
        conf_object_t obj;
        sample_core_timing_tool_t const *tool;
        bool enabled;

        conf_object_t *cpu;
        double frequency;
        uint32 cstate;

        uint64 state_changes;
        uint64 active_periods;
        uint64 idle_periods;
        uint64 sleeping_periods;

        struct {
                int allocated_dynamic_events;
                uint64 *dynamic_events;
                uint64 *dynamic_events_total;

                uint64 nr_read_ops;
                uint64 nr_read_ops_total;
                uint64 nr_write_ops;
                uint64 nr_write_ops_total;
        } events;

        // Used to transfer date between period_part_one
        // and period_part_two
        struct {
                cycles_t cycles;
                uint64   activity;
                uint64   instructions;
                uint64   freq;
        } period_data;

        // Used by telemetry interface
        struct {
                // Cumulative data
                uint64 activity;
                uint64 instructions;
                uint64 active_cycles;
                uint64 idle_cycles;
                uint64 all_cycles;

                // Last period data
                cycles_t period_cycles;
                uint64 period_activity;
                uint64 period_instructions;
                uint64 period_end_time_ps;
                uint64 period_frequency;
                uint32 period_state;
        } telemetry;

        cycles_t last_cycles;
        uint64   last_steps;

        /* Cached Interfaces */
        const cpu_cached_instruction_interface_t *cached_iface;
        const cpu_instruction_query_interface_t *iq_iface;
        const processor_info_interface_t *pi_iface;
        const x86_cstate_interface_t *cstate_iface;
        const frequency_interface_t *freq_iface;
        const cpu_instrumentation_subscribe_interface_t *is_iface;
        const x86_reg_access_interface_t *ra_iface;

        notifier_handle_t *cstate_notification_handle;
        notifier_handle_t *frequency_notification_handle;
        notifier_type_t    isim_notify_new_telemetry;
} connection_t;

FORCE_INLINE connection_t *
connection_of_obj(conf_object_t *obj) { return (connection_t *)obj; }

FORCE_INLINE conf_object_t *
obj_of_conn(connection_t *cc) { return &cc->obj; }

/* Telemetry */

typedef struct telemetry_entry {
        const char *name;
        const char *description;
        int id;
} telemetry_entry_t;

typedef struct class_entry {
        const char *name;
        const char *description;
        const telemetry_entry_t *telemetries;
        int nr_telemetries;
} telemetry_class_entry_t;


enum {
        Invalid_Telemetry_Class = -1,
};

enum {
        Invalid_Telemetry = -1,
        Telemetry_Period_Activity = 0,
        Telemetry_Period_Instructions,
        Telemetry_Period_End_Time,
        Telemetry_Period_Cycles,
        Telemetry_Period_Frequency,
        Telemetry_Period_State,

        Telemetry_Period_Activity_Per_Cycle,
        Telemetry_Period_Instruction_Per_Cycle,

        Telemetry_Activity,
        Telemetry_Instructions,
        Telemetry_Active_Cycles,
        Telemetry_Idle_Cycles,
        Telemetry_All_Cycles,
};

static const telemetry_entry_t sct_period_telemetries[] = {
        { "ACTIVITY",      "Activity during period",                   Telemetry_Period_Activity },
        { "CYCLES",        "Estimated number of cycles in period",     Telemetry_Period_Cycles },
        { "TIME",          "Time at end of period in pico seconds",    Telemetry_Period_End_Time },
        { "FREQUENCY",     "Frequency in Hz during period",            Telemetry_Period_Frequency },
        { "STATE",         "STATE state during period. 0 = active,"
                           "1 = idle, 1< sleep",                       Telemetry_Period_State },
        { "INSTRUCTIONS",  "Instructions retired during period",       Telemetry_Period_Instructions },
};

static const telemetry_entry_t sct_period_derived_telemetries[] = {
        { "ACTIVITY_PER_CYCLE",       "Average activity per cycle during period",
          Telemetry_Period_Activity_Per_Cycle },
        { "INSTRUCTION_PER_CYCLE",    "Average instruction per cycle during period",
          Telemetry_Period_Instruction_Per_Cycle },
};

static const telemetry_entry_t sct_cumulative_telemetries[] = {
        { "ACTIVITY",      "Activity, cumulative",                     Telemetry_Activity },
        { "INSTRUCTIONS",  "Instructions retired, cumulative",         Telemetry_Instructions },
        { "ACTIVE_CYCLES", "Accumulated active cycles (STATE=0)",      Telemetry_Active_Cycles },
        { "IDLE_CYCLES",   "Accumulated idle cycles (STATE=1)",        Telemetry_Idle_Cycles },
        { "TOTAL_CYCLES",  "Accumulated cycles regardless of state",   Telemetry_All_Cycles },
};

#define ARRAY_LENGTH(ARR) (sizeof(ARR) / sizeof(ARR[0]))

static const telemetry_class_entry_t telemetry_classes[] =
{
        { "SAMPLE_CORE_TIMING_PERIOD",
          "Sample-core-timing telemetries for latest period.",
          sct_period_telemetries,
          ARRAY_LENGTH(sct_period_telemetries)
        },
        { "SAMPLE_CORE_TIMING_CUMULATIVE",
          "Cumulative sample-core-timing telemetries.",
          sct_cumulative_telemetries,
          ARRAY_LENGTH(sct_cumulative_telemetries)
        },
        { "SAMPLE_CORE_TIMING_PERIOD_DERIVED",
          "Derived sample-core-timing telemetries for latest period",
          sct_period_derived_telemetries,
          ARRAY_LENGTH(sct_period_derived_telemetries)
        },
};

static const int nr_telemetry_classes = ARRAY_LENGTH(telemetry_classes);

static bool
is_valid_class(int class_id)
{
        return class_id > Invalid_Telemetry_Class && class_id < nr_telemetry_classes;
}

static const telemetry_class_entry_t *
lookup_class(int class_id)
{
        if (is_valid_class(class_id))
                return &telemetry_classes[class_id];
        return NULL;
}

static telemetry_class_id_t
tsi_get_telemetry_class_id(conf_object_t *obj,
                           const char *telemetry_class_name)
{
        for (int i = 0; i < nr_telemetry_classes; ++i) {
                if (strcmp(telemetry_classes[i].name, telemetry_class_name) == 0) {
                        return i;
                }
        }
        return Invalid_Telemetry_Class;
}

static const char *
tsi_get_telemetry_class_name(conf_object_t *obj,
                             telemetry_class_id_t class_id)
{
        if (is_valid_class(class_id)) {
                return telemetry_classes[class_id].name;
        }
        return NULL;
}

static const char *
tsi_get_telemetry_class_description(conf_object_t *obj,
                                    telemetry_class_id_t class_id)
{
        if (is_valid_class(class_id)) {
                return telemetry_classes[class_id].description;
        }
        return NULL;
}

static telemetry_id_t
find_telemetry_id(const telemetry_entry_t *telemetry_list,
                  int nr_telemetries,
                  const char *telemetry_name)
{
        for (int id = 0; id < nr_telemetries; ++id) {
                if (strcmp(telemetry_list[id].name, telemetry_name) == 0) {
                        return id;
                }
        }
        return Invalid_Telemetry;
}

static telemetry_id_t
tsi_get_telemetry_id(conf_object_t *obj,
                     telemetry_class_id_t class_id,
                     const char *telemetry_name)
{
        const telemetry_class_entry_t * tcls = lookup_class(class_id);
        if (tcls) {
                return find_telemetry_id(tcls->telemetries,
                                         tcls->nr_telemetries, telemetry_name);
        }
        return Invalid_Telemetry;
}

static const telemetry_entry_t *
get_valid_telemetry(telemetry_class_id_t class_id, telemetry_id_t telemetry_id)
{
        const telemetry_class_entry_t * tcls = lookup_class(class_id);
        if (tcls) {
                if (0 <= telemetry_id && telemetry_id < tcls->nr_telemetries) {
                        return &tcls->telemetries[telemetry_id];
                }
        }
        return NULL;
}

static const char *
tsi_get_telemetry_name(conf_object_t *obj,
                       telemetry_class_id_t class_id,
                       telemetry_id_t telemetry_id)
{
        const telemetry_entry_t * te = get_valid_telemetry(class_id, telemetry_id);
        return te ? te->name : NULL;
}

static const char *
tsi_get_telemetry_description(conf_object_t *obj,
                              telemetry_class_id_t class_id,
                              telemetry_id_t telemetry_id)
{
        const telemetry_entry_t * te = get_valid_telemetry(class_id, telemetry_id);
        return te ? te->description : NULL;
}

static double
safe_x_per_y(double x, uint64 y)
{
        if (y)
                return x / y;
        else
                return 0.0;
}

static attr_value_t
tsi_get_value(conf_object_t *obj,
              telemetry_class_id_t class_id,
              telemetry_id_t telemetry_id)
{
        connection_t *cc = connection_of_obj(obj);
        const telemetry_entry_t * te = get_valid_telemetry(class_id, telemetry_id);
        if (te) {
                switch (te->id) {
                case Telemetry_Period_Activity:
                        return SIM_make_attr_uint64(cc->telemetry.period_activity);
                case Telemetry_Period_Instructions:
                        return SIM_make_attr_uint64(cc->telemetry.period_instructions);
                case Telemetry_Period_Cycles:
                        return SIM_make_attr_uint64(cc->telemetry.period_cycles);
                case Telemetry_Period_End_Time:
                        return SIM_make_attr_uint64(cc->telemetry.period_end_time_ps);
                case Telemetry_Period_Frequency:
                        return SIM_make_attr_uint64(cc->telemetry.period_frequency);
                case Telemetry_Period_State:
                        return SIM_make_attr_uint64(cc->telemetry.period_state);

                case Telemetry_Period_Activity_Per_Cycle:
                        return SIM_make_attr_floating(
                                safe_x_per_y(cc->telemetry.period_activity,
                                             cc->telemetry.period_cycles));

                case Telemetry_Period_Instruction_Per_Cycle:
                        return SIM_make_attr_floating(
                                safe_x_per_y(cc->telemetry.period_instructions,
                                             cc->telemetry.period_cycles));

                case Telemetry_Activity:
                        return SIM_make_attr_uint64(cc->telemetry.activity);
                case Telemetry_Instructions:
                        return SIM_make_attr_uint64(cc->telemetry.instructions);
                case Telemetry_Active_Cycles:
                        return SIM_make_attr_uint64(cc->telemetry.active_cycles);
                case Telemetry_Idle_Cycles:
                        return SIM_make_attr_uint64(cc->telemetry.idle_cycles);
                case Telemetry_All_Cycles:
                        return SIM_make_attr_uint64(cc->telemetry.all_cycles);
                default:
                        break;
                }
        }

        if (!is_valid_class(class_id)) {
                SIM_LOG_ERROR(obj, 0, "Unknown telemetry_class_id %d used in"
                              " request for telemetry with id %d",
                              class_id, telemetry_id);
        } else {
                SIM_LOG_ERROR(obj, 0, "Unknown telemetry id %d requested",
                              telemetry_id);
        }

        return SIM_make_attr_uint64(0);
}

static bool
is_active(connection_t *cc)
{
        return cc->cstate == 0;
}

static bool
is_idle(connection_t *cc)
{
        return cc->cstate == 1;
}

static bool
is_sleeping(connection_t *cc)
{
        return cc->cstate > 1;
}

static void
new_telemetry_data(connection_t *cc, uint64 activity,
                   uint64 instructions, cycles_t cycles)
{
        cc->telemetry.period_activity = activity;
        cc->telemetry.period_end_time_ps = SIM_cycle_count(SIM_picosecond_clock(cc->cpu));
        cc->telemetry.period_cycles = cycles;
        cc->telemetry.period_frequency = cc->frequency;
        cc->telemetry.period_state = cc->cstate;
        cc->telemetry.period_instructions = instructions;

        cc->telemetry.activity += activity;
        cc->telemetry.instructions += instructions;

        cc->telemetry.all_cycles += cycles;

        SIM_notify(cc->cpu, cc->isim_notify_new_telemetry);
}

static void
new_active_telemetry_data(connection_t *cc, uint64 activity,
                          cycles_t cycles, uint64 instructions)
{
        cc->active_periods += 1;
        cc->telemetry.active_cycles += cycles;

        new_telemetry_data(cc, activity, instructions, cycles);
}

static void
new_idle_telemetry_data(connection_t *cc, uint64 activity,
                        cycles_t cycles)
{
        if (is_sleeping(cc)) {
                cc->sleeping_periods += 1;
        } else if (is_idle(cc)) {
                cc->idle_periods += 1;
                cc->telemetry.idle_cycles += cycles;
        }

        uint64 instructions = 0;
        new_telemetry_data(cc, activity, instructions, cycles);
}

const char telemetry_providers[] = "telemetry_providers";

static bool
has_telemetry_provider_list(conf_object_t *proxy)
{
        conf_class_t *proxy_cls = SIM_object_class(proxy);
        return SIM_class_has_attribute(proxy_cls, telemetry_providers);
}

static void
add_to_telemetry_provider_list(connection_t *cc,
                               conf_object_t *proxy)
{
        if (!has_telemetry_provider_list(proxy)) {
                SIM_LOG_INFO(1, obj_of_conn(cc), 0,
                             "Object %s does not have a '%s' attribute",
                             SIM_object_name(proxy), telemetry_providers);
                return;
        }

        attr_value_t list = SIM_get_attribute(proxy, telemetry_providers);
        if (!SIM_attr_is_list(list)) {
                SIM_LOG_INFO(1, obj_of_conn(cc), 0,
                             "Unexpected type for object %s attribute '%s'",
                             SIM_object_name(proxy), telemetry_providers);
                return;
        }

        int size = SIM_attr_list_size(list);
        SIM_attr_list_resize(&list, size + 1);
        SIM_attr_list_set_item(&list, size,
                               SIM_make_attr_object(obj_of_conn(cc)));
        SIM_set_attribute(proxy, telemetry_providers, &list);
}

static void
remove_from_telemetry_provider_list(connection_t *cc,
                                    conf_object_t *proxy)
{
        if (!has_telemetry_provider_list(proxy))
                return;

        attr_value_t list = SIM_get_attribute(proxy, telemetry_providers);
        if (!SIM_attr_is_list(list))
                return;

        int size = SIM_attr_list_size(list);
        int index_to_remove = -1;
        for (int i = 0; i < size; ++i) {
                if (SIM_attr_object(SIM_attr_list_item(list, i))
                    == obj_of_conn(cc)) {
                        index_to_remove = i;
                        break;
                }
        }

        if (index_to_remove != -1) {
                for (int i = index_to_remove; i < (size - 1); ++i) {
                        SIM_attr_list_set_item(&list, i,
                                               SIM_attr_list_item(
                                                       list, i + 1));
                }
                SIM_attr_list_resize(&list, size - 1);
                SIM_set_attribute(proxy, telemetry_providers, &list);
        }
}

/* Connection functions */

static void
allocate_dynamic_events(connection_t *cc)
{
        int nr_events = sct_get_nr_dynamic_events(cc->tool);

        if (nr_events > cc->events.allocated_dynamic_events) {
                if (cc->events.dynamic_events) {
                        MM_FREE(cc->events.dynamic_events);
                        MM_FREE(cc->events.dynamic_events_total);
                }
                cc->events.allocated_dynamic_events = nr_events;
                cc->events.dynamic_events = MM_ZALLOC(nr_events, uint64);
                cc->events.dynamic_events_total = MM_ZALLOC(nr_events, uint64);
        }
}

static void
reset_period_counters(connection_t *cc)
{
        int nr_events = sct_get_nr_dynamic_events(cc->tool);
        for (int i = 0; i < nr_events; ++i) {
                cc->events.dynamic_events_total[i] +=
                        cc->events.dynamic_events[i];
                cc->events.dynamic_events[i] = 0;
        }
        cc->events.nr_read_ops_total += cc->events.nr_read_ops;
        cc->events.nr_write_ops_total += cc->events.nr_write_ops;
        cc->events.nr_read_ops = 0;
        cc->events.nr_write_ops = 0;
}

static void
enable_active_periods(connection_t *cc)
{
        conf_object_t *cpu = cc->cpu;

        cc->enabled = true;
        cc->last_cycles = SIM_cycle_count(cpu);
        cc->last_steps  = SIM_step_count(cpu);

        // Start a new period from fresh
        reset_period_counters(cc);
        SIM_event_cancel_time(cpu, idle_event, obj_of_conn(cc), NULL, NULL);
        SIM_event_cancel_step(cpu, period_event, obj_of_conn(cc), NULL, NULL);
        SIM_event_post_step(cpu, period_event, obj_of_conn(cc),
                            STEPS_PERIOD, NULL);
}

static cycles_t
get_done_cycles(connection_t *cc)
{
        cycles_t cycles = SIM_cycle_count(cc->cpu);
        cycles_t done_cycles = cycles - cc->last_cycles;
        cc->last_cycles = cycles;

        return done_cycles;
}

static cycles_t
calc_active_cycles(connection_t *cc, uint64 instructions)
{
        double cycles =
                instructions * cc->tool->base_cycles_per_instruction;
        int nr_events = sct_get_nr_dynamic_events(cc->tool);
        for (int i = 0; i < nr_events; ++i) {
                cycles += cc->events.dynamic_events[i]
                        * sct_get_extra_cycles(cc->tool, i);
        }
        cycles += cc->events.nr_read_ops * cc->tool->read_extra_cycles;
        cycles += cc->events.nr_write_ops * cc->tool->write_extra_cycles;

        return (cycles_t)cycles;
}

static uint64
calc_active_activity(connection_t *cc, uint64 instructions, cycles_t cycles)
{
        double activity = cc->tool->background_activity_per_cycle * cycles;
        activity += cc->tool->base_activity_per_instruction * instructions;
        int nr_events = sct_get_nr_dynamic_events(cc->tool);
        for (int i = 0; i < nr_events; ++i) {
                activity += cc->events.dynamic_events[i]
                        * sct_get_extra_activity(cc->tool, i);
        }
        activity += cc->events.nr_read_ops * cc->tool->read_extra_activity;
        activity += cc->events.nr_write_ops * cc->tool->write_extra_activity;
        return (uint64)activity;
}

static uint64
get_steps_in_period(connection_t *cc)
{
        uint64 current_steps = SIM_step_count(cc->cpu);
        uint64 period_steps = current_steps - cc->last_steps;
        cc->last_steps  = current_steps;

        return period_steps;
}

static double
get_frequency(connection_t *cc)
{
        return cc->frequency;
}

static void period_part_two(conf_object_t *obj, lang_void *not_used);

static void
period_part_one(conf_object_t *obj, lang_void *not_used)
{
        connection_t *cc = connection_of_obj(obj);
        conf_object_t *cpu = cc->cpu;

        uint64  instructions = get_steps_in_period(cc);
        cycles_t done_cycles = get_done_cycles(cc);
        cycles_t cycles = calc_active_cycles(cc, instructions);
        cycles_t extra_cycles = cycles - done_cycles;
        uint64 activity = calc_active_activity(cc, instructions, cycles);

        if (extra_cycles < 0) {
                // The simulation has taken more cycles than
                // what the model calculated
                extra_cycles = 0;
                cycles = done_cycles;
        }

        reset_period_counters(cc);

        cc->period_data.activity = activity;
        cc->period_data.instructions = instructions;
        cc->period_data.cycles = cycles;
        cc->period_data.freq   = get_frequency(cc);

        if (extra_cycles) {
                /* Stall to make the extra cycles pass */
                SIM_stall_cycle(cpu, extra_cycles);
                cc->last_cycles += extra_cycles;

                /* Continue after the stalled cycles */
                SIM_event_post_cycle(cpu, period_part2_event,
                                     &cc->obj, extra_cycles, NULL);
        } else {
                period_part_two(obj, NULL);
        }
}

static void
period_part_two(conf_object_t *obj, lang_void *not_used)
{
        connection_t *cc = connection_of_obj(obj);
        conf_object_t *cpu = cc->cpu;

        cycles_t cycles     = cc->period_data.cycles;
        uint64 activity     = cc->period_data.activity;
        uint64 instructions = cc->period_data.instructions;
        uint64 freq         = cc->period_data.freq;

        SIM_LOG_INFO(3, obj, 0,
                     "Period: %lldMHz, %6lld cycles, %2.03g ipc,"
                     " activity/cycle: %.1f",
                     freq / 1000000, cycles,
                     (double)instructions / cycles,
                     (double)activity/cycles);

        new_active_telemetry_data(cc, activity, cycles, instructions);

        if (cc->enabled) {
                SIM_event_post_step(cpu, period_event,
                                    &cc->obj, STEPS_PERIOD, NULL);
        }
}

static void
enable_idle_periods(connection_t *cc, uint64 extra_cycles)
{
        conf_object_t *cpu = cc->cpu;

        cc->enabled = true;

        cc->last_cycles += extra_cycles;

        // Start a new HB from fresh
        reset_period_counters(cc);
        SIM_event_cancel_time(cpu, idle_event, &cc->obj, NULL, NULL);
        SIM_event_cancel_step(cpu, period_event, &cc->obj, NULL, NULL);
        SIM_event_post_cycle(cpu, idle_event, &cc->obj,
                             IDLE_PERIOD + extra_cycles, NULL);
}

static uint64
calc_idle_activity(connection_t *cc, cycles_t cycles)
{
        double activity;
        if (is_sleeping(cc)) {
                activity = 0.0;
        } else {
                activity = cc->tool->background_activity_per_cycle * cycles;
        }
        return (uint64)activity;
}

static void
idle_period(conf_object_t *obj, lang_void *not_used)
{
        connection_t *cc = connection_of_obj(obj);
        conf_object_t *cpu = cc->cpu;

        cycles_t period_cycles = get_done_cycles(cc);
        uint64 freq = get_frequency(cc);
        uint64 activity = calc_idle_activity(cc, period_cycles);

        SIM_LOG_INFO(3, obj, 0,
                     "Idle period: %lldMHz, %lld cycles, activity/cycle: %.1f",
                     freq / 1000000, period_cycles,
                     (double)activity/period_cycles);

        new_idle_telemetry_data(cc, activity, period_cycles);

        if (cc->enabled) {
                SIM_event_post_cycle(cpu, idle_event, &cc->obj,
                                     IDLE_PERIOD, NULL);
        }
}

static void
install_matching_instruction_class(connection_t *cc,
                                   cached_instruction_handle_t *handle,
                                   char const * disassm,
                                   int i)
{
        if (strstr(disassm, sct_get_instr_pattern(cc->tool, i))) {
                uint64 *eventp = &(cc->events.dynamic_events[i]);
                cc->cached_iface->add_counter(cc->cpu, handle,
                                              eventp, false);
        }
}


static void
cached_instruction_cb(conf_object_t *obj, conf_object_t *cpu,
                      cached_instruction_handle_t *handle,
                      instruction_handle_t *iq_handle, void *user_data)
{
        connection_t *cc = connection_of_obj(obj);

        /* Get hold of the instruction sequence Simics just saw */
        cpu_bytes_t opcode = cc->iq_iface->get_instruction_bytes(
                cpu, iq_handle);
        attr_value_t opc = SIM_make_attr_data(opcode.size, opcode.data);
        physical_address_t pa = cc->iq_iface->physical_address(
                cpu, iq_handle);
        tuple_int_string_t t = cc->pi_iface->disassemble(cpu, pa, opc, 0);
        SIM_attr_free(&opc);
        if (t.string) {
                int nr_events = sct_get_nr_dynamic_events(cc->tool);
                for (int i = 0; i < nr_events; ++i) {
                        install_matching_instruction_class(cc, handle,
                                                           t.string, i);
                }
                MM_FREE(t.string);
        }
}

static void
read_before_cb(conf_object_t *obj, conf_object_t *cpu,
               memory_handle_t *memory_handle, void *user_data)
{
        connection_t *cc = connection_of_obj(obj);
        cc->events.nr_read_ops += 1;
}

static void
write_before_cb(conf_object_t *obj, conf_object_t *cpu,
                memory_handle_t *memory_handle, void *user_data)
{
        connection_t *cc = connection_of_obj(obj);
        cc->events.nr_write_ops += 1;
}

static void
frequency_change_callback(conf_object_t *obj, conf_object_t *cpu,
                          void *user_data)
{
        connection_t *cc = connection_of_obj(obj);
        cc->frequency = cc->freq_iface->get(cpu);
}

static uint32
get_cstate_and_activity(connection_t *cc)
{
        conf_object_t *cpu = cc->cpu;
        x86_cstate_t current_cstate = cc->cstate_iface->get_cstate(cpu);
        x86_activity_t activity_state = cc->ra_iface->get_activity_state(cpu);

        if (current_cstate.state == 0
            && activity_state == X86_Activity_Normal) {
                return 0;
        } else if (current_cstate.state != 0) {
                return current_cstate.state;
        } else {
                return 6; // Handle waiting for SIPI
        }
}

static void
update_state_and_periods(connection_t *cc)
{
        conf_object_t *obj = &cc->obj;
        uint32 cstate = get_cstate_and_activity(cc);
        uint64 freq = get_frequency(cc);
        cycles_t period_cycles = get_done_cycles(cc);

        cc->state_changes += 1;

        if (cstate == 0) {
                if (!is_active(cc) && period_cycles > 100) {
                        uint64 activity = calc_idle_activity(cc, period_cycles);

                        SIM_LOG_INFO(3, obj, 0,
                                     "Idle period: %lldMHz, %lld cycles,"
                                     " activity/cycle: %.1f",
                                     freq / 1000000, period_cycles,
                                     (double)activity/period_cycles);

                        new_idle_telemetry_data(cc, activity, period_cycles);
                }

                enable_active_periods(cc);

        } else {
                cycles_t extra_cycles = 0;
                if (is_active(cc) && period_cycles > 100) {

                        uint64 instructions = get_steps_in_period(cc);
                        cycles_t cycles = calc_active_cycles(cc, instructions);
                        cycles_t extra_cycles = cycles - period_cycles;
                        uint64 activity = calc_active_activity(cc, instructions,
                                                               cycles);

                        if (extra_cycles < 0) {
                                // The simulation has taken more cycles than
                                // what the model calculated
                                extra_cycles = 0;
                                cycles = period_cycles;
                        }

                        SIM_LOG_INFO(3, obj, 0,
                                     "Period: %lldMHz, %6lld cycles, %2.03g ipc,"
                                     " activity/cycle: %.1f",
                                     freq / 1000000, cycles,
                                     (double)instructions / cycles,
                                     (double)activity/cycles);

                        // This call will be extra_cycles early
                        new_active_telemetry_data(cc, activity, cycles,
                                                  instructions);
                }

                enable_idle_periods(cc, extra_cycles);
        }

        cc->cstate = cstate;
}

static void
cstate_change_callback(conf_object_t *obj, conf_object_t *notifier, void *data)
{
        connection_t *cc = connection_of_obj(obj);
        update_state_and_periods(cc);
}

static void
ic_enable(conf_object_t *obj)
{
        connection_t *cc = connection_of_obj(obj);
        cc->enabled = true;
        uint32 cstate = get_cstate_and_activity(cc);
        if (cstate == 0) {
                enable_active_periods(cc);
        } else {
                enable_idle_periods(cc, 0);
        }
        cc->cstate = cstate;
}

static void
ic_disable(conf_object_t *obj)
{
        connection_t *cc = connection_of_obj(obj);
        conf_object_t *cpu = cc->cpu;

        cc->enabled = false;

        SIM_event_cancel_time(cpu, idle_event, obj, NULL, NULL);
        SIM_event_cancel_step(cpu, period_event, obj, NULL, NULL);
        SIM_event_cancel_step(cpu, period_part2_event, obj, NULL, NULL);
}

static conf_object_t *
ic_alloc(void *arg)
{
        connection_t *conn = MM_ZALLOC(1, connection_t);
        return &conn->obj;
}

static void
ic_pre_delete_connection(conf_object_t *obj)
{
        connection_t *cc = connection_of_obj(obj);
        conf_object_t *cpu = cc->cpu;

        ic_disable(obj);

        if (cc->is_iface)
                cc->is_iface->remove_connection_callbacks(cpu, obj);

        if (cc->cstate_notification_handle)
                SIM_delete_notifier(cpu, cc->cstate_notification_handle);

        remove_from_telemetry_provider_list(cc, cpu);
}

static int
ic_delete_connection(conf_object_t *obj)
{
        connection_t *cc = connection_of_obj(obj);
        MM_FREE(cc->events.dynamic_events);
        MM_FREE(cc->events.dynamic_events_total);
        MM_FREE(obj);
        return 0;
}

static attr_value_t
get_cpu(conf_object_t *obj)
{
        connection_t *cc = connection_of_obj(obj);
        return SIM_make_attr_object(cc->cpu);
}

static attr_value_t
get_av_pair(char const *name, uint64 value)
{
        return SIM_make_attr_list(2,
                                  SIM_make_attr_string(name),
                                  SIM_make_attr_uint64(value));
}

static attr_value_t
get_static_events(conf_object_t *obj)
{
        connection_t *cc = connection_of_obj(obj);
        uint64 active_cycles = cc->telemetry.active_cycles;
        uint64 idle_cycles = cc->telemetry.idle_cycles;
        uint64 sleeping_cycles =
                cc->telemetry.all_cycles - (active_cycles + idle_cycles);
        return SIM_make_attr_list(10,
                                  get_av_pair("instructions",
                                              cc->telemetry.instructions),
                                  get_av_pair("read access",
                                              cc->events.nr_read_ops_total),
                                  get_av_pair("write access",
                                              cc->events.nr_write_ops_total),
                                  get_av_pair("active periods", cc->active_periods),
                                  get_av_pair("active cycles", active_cycles),
                                  get_av_pair("idle periods", cc->idle_periods),
                                  get_av_pair("idle cycles", idle_cycles),
                                  get_av_pair("sleeping periods", cc->sleeping_periods),
                                  get_av_pair("sleeping cycles", sleeping_cycles),
                                  get_av_pair("state changes", cc->state_changes)
                );
}

static attr_value_t
get_dynamic_events(conf_object_t *obj)
{
        connection_t *cc = connection_of_obj(obj);
        int nr_events = sct_get_nr_dynamic_events(cc->tool);
        attr_value_t res = SIM_alloc_attr_list(nr_events);
        for (int i = 0; i < nr_events; ++i) {
                char const * p = sct_get_instr_pattern(cc->tool, i);
                uint64 ev_total = cc->events.dynamic_events_total[i];
                SIM_attr_list_set_item(&res, i,
                                       get_av_pair(p, ev_total));
        }
        return res;
}

static attr_value_t
get_telemetry_data(conf_object_t *obj)
{
        connection_t *cc = connection_of_obj(obj);

        uint64 period_cycles = cc->telemetry.period_cycles;
        uint64 active_cycles = cc->telemetry.active_cycles;
        double current_ipc = safe_x_per_y(cc->telemetry.period_instructions,
                                          period_cycles);
        double avg_ipc = safe_x_per_y(cc->telemetry.instructions,
                                      active_cycles);
        double current_activity = safe_x_per_y(cc->telemetry.period_activity,
                                               period_cycles);
        double avg_activity = 0.0;
        if (active_cycles) {
                uint64 idle_activity = calc_idle_activity(cc, cc->telemetry.idle_cycles);
                uint64 activity = cc->telemetry.activity - idle_activity;
                avg_activity = (double)activity / active_cycles;
        }

        attr_value_t ipc =
                SIM_make_attr_list(3,
                                   SIM_make_attr_string("ipc"),
                                   SIM_make_attr_floating(current_ipc),
                                   SIM_make_attr_floating(avg_ipc));
        attr_value_t activity =
                SIM_make_attr_list(3,
                                   SIM_make_attr_string("activity/cycle"),
                                   SIM_make_attr_floating(current_activity),
                                   SIM_make_attr_floating(avg_activity));
        return SIM_make_attr_list(2, ipc, activity);
}

static void
register_instrumentation(connection_t *cc)
{
        cc->is_iface->register_cached_instruction_cb(cc->cpu,
                                                     &cc->obj,
                                                     cached_instruction_cb,
                                                     NULL);

        cc->is_iface->register_read_before_cb(cc->cpu,
                                              &cc->obj,
                                              CPU_Access_Scope_Explicit,
                                              read_before_cb,
                                              NULL);

        cc->is_iface->register_write_before_cb(cc->cpu,
                                               &cc->obj,
                                               CPU_Access_Scope_Explicit,
                                               write_before_cb,
                                               NULL);
}

static void
setup_telemetry_notifier(connection_t *cc)
{
        cc->isim_notify_new_telemetry
                = SIM_notifier_type("isim-telemetry-update");
        SIM_register_notifier(
                SIM_object_class(cc->cpu),
                cc->isim_notify_new_telemetry,
                "Notifier that is triggered when new ISIM telemetry"
                " is available. The new telemetry data is available through"
                " the get_value method of the object's telemetry interface.");
}

static bool
cpu_already_connected(conf_object_t const *tool, conf_object_t *cpu,
                      char const *name)
{
        conf_object_t *other_obj = VT_get_object_by_name(name);
        if (other_obj) {
                conf_object_t const *other_tool_obj = NULL;

                if (SIM_object_class(other_obj) == connection_class) {
                        connection_t *occ = connection_of_obj(other_obj);
                        other_tool_obj = &occ->tool->obj;
                }

                conf_object_t *obj = (conf_object_t *)tool;

                if (other_tool_obj) {
                        SIM_LOG_INFO(1, obj, 0,
                                     "Core %s is already connected to %s",
                                     SIM_object_name(cpu),
                                     SIM_object_name(other_tool_obj));
                } else {
                        SIM_LOG_INFO(1, obj, 0,
                                     "Core %s is already connected to"
                                     " other tool",
                                     SIM_object_name(cpu));
                }
        }
        return other_obj != NULL;
}

static conf_object_t *
report_missing_interface(conf_object_t *obj,
                         conf_object_t *cpu,
                         const char *interface_name)
{
        SIM_LOG_ERROR(obj, 0, "Core %s does not implement '%s_interface'",
                     SIM_object_name(cpu), interface_name);
        SIM_delete_object(obj);
        return NULL;
}

/* Help function which creates a new connection and returns it. */
conf_object_t *
sct_new_connection(sample_core_timing_tool_t *tool,
                   conf_object_t *cpu,
                   attr_value_t args)
{
        strbuf_t sb = SB_INIT;

        sb_addfmt(&sb, "%s.sample_core_timing", SIM_object_name(cpu));

        if (cpu_already_connected(&tool->obj, cpu, sb_str(&sb)))
                return NULL;

        conf_object_t *conn_obj = SIM_create_object(connection_class,
                                                    sb_str(&sb), args);
        sb_free(&sb);

        if (!conn_obj)
                return NULL;

        tool->next_connection_number++;
        connection_t *cc = connection_of_obj(conn_obj);
        cc->cpu = cpu;
        cc->tool = tool;

        cc->cached_iface = SIM_C_GET_INTERFACE(cpu, cpu_cached_instruction);
        if (!cc->cached_iface)
                return report_missing_interface(conn_obj, cpu,
                                                CPU_CACHED_INSTRUCTION_INTERFACE);

        cc->cstate_iface = SIM_C_GET_INTERFACE(cpu, x86_cstate);
        if (!cc->cstate_iface)
                return report_missing_interface(conn_obj, cpu,
                                                X86_CSTATE_INTERFACE);

        cc->cstate_notification_handle =
                SIM_add_notifier(cpu,
                                 SIM_notifier_type("x86-cstate-change"),
                                 conn_obj, cstate_change_callback, NULL);

        cc->freq_iface = SIM_C_GET_INTERFACE(cpu, frequency);
        if (!cc->freq_iface)
                return report_missing_interface(conn_obj, cpu,
                                                FREQUENCY_INTERFACE);

        cc->cstate_notification_handle =
                SIM_add_notifier(cpu, Sim_Notify_Frequency_Change,
                                 conn_obj, frequency_change_callback, NULL);

        cc->frequency = cc->freq_iface->get(cpu);

        cc->iq_iface = SIM_C_GET_INTERFACE(cpu, cpu_instruction_query);
        if (!cc->iq_iface)
                return report_missing_interface(conn_obj, cpu,
                                                CPU_INSTRUCTION_QUERY_INTERFACE);

        cc->pi_iface = SIM_C_GET_INTERFACE(cpu, processor_info);
        if (!cc->pi_iface)
                return report_missing_interface(conn_obj, cpu,
                                                PROCESSOR_INFO_INTERFACE);

        cc->is_iface = SIM_C_GET_INTERFACE(cpu, cpu_instrumentation_subscribe);
        if (!cc->is_iface)
                return report_missing_interface(conn_obj, cpu,
                                                CPU_INSTRUMENTATION_SUBSCRIBE_INTERFACE);

        cc->ra_iface = SIM_C_GET_INTERFACE(cpu, x86_reg_access);
        if (!cc->ra_iface)
                return report_missing_interface(conn_obj, cpu,
                                                X86_REG_ACCESS_INTERFACE);

        register_instrumentation(cc);
        setup_telemetry_notifier(cc);
        sct_allocate_dynamic_events(conn_obj);

        add_to_telemetry_provider_list(cc, cpu);

        ic_enable(conn_obj);

        return conn_obj;
}

void
sct_allocate_dynamic_events(conf_object_t *conn_obj)
{
        allocate_dynamic_events(connection_of_obj(conn_obj));
}


void
sct_shutdown_connection(conf_object_t *conn_obj)
{
        connection_t *cc = connection_of_obj(conn_obj);

        cc->is_iface->remove_connection_callbacks(cc->cpu, conn_obj);
}

void
sct_init_connection_class()
{
        const class_data_t funcs = {
                .alloc_object = ic_alloc,
                .description = "Sample x86 core timing connection",
                .class_desc = "sample x86 core timing connection",
                .pre_delete_instance = ic_pre_delete_connection,
                .delete_instance = ic_delete_connection,
                .kind = Sim_Class_Kind_Pseudo
        };
        connection_class = SIM_register_class("sample_core_timing_connection",
                                              &funcs);

        static const instrumentation_connection_interface_t ic_iface = {
                .enable = ic_enable,
                .disable = ic_disable,
        };
        SIM_REGISTER_INTERFACE(connection_class,
                               instrumentation_connection, &ic_iface);

        static const telemetry_interface_t tsi = {
                .get_telemetry_class_id     = tsi_get_telemetry_class_id,
                .get_telemetry_class_name   = tsi_get_telemetry_class_name,
                .get_telemetry_class_description
                                    = tsi_get_telemetry_class_description,
                .get_telemetry_id           = tsi_get_telemetry_id,
                .get_telemetry_name         = tsi_get_telemetry_name,
                .get_telemetry_description  = tsi_get_telemetry_description,
                .get_value                  = tsi_get_value,
        };

        SIM_REGISTER_INTERFACE(connection_class, telemetry, &tsi);


        period_event = SIM_register_event("period",
                                             connection_class, 0,
                                             period_part_one,
                                             NULL, NULL, NULL, NULL);

        period_part2_event = SIM_register_event("period_part2",
                                              connection_class, 0,
                                              period_part_two,
                                              NULL, NULL, NULL, NULL);

        idle_event = SIM_register_event("idle_event", connection_class, 0,
                                   idle_period,
                                   NULL, NULL, NULL, NULL);

        SIM_register_attribute(
                connection_class, "cpu",
                get_cpu, NULL,
                Sim_Attr_Pseudo,
                "o",
                "Returns the connected cpu");

        SIM_register_attribute(
                connection_class, "static_events",
                get_static_events,
                NULL,
                Sim_Attr_Pseudo,
                "[[si]*]",
                "Statistics for static events, like read, write and"
                " total number of instructions.");

        SIM_register_attribute(
                connection_class, "dynamic_events",
                get_dynamic_events,
                NULL,
                Sim_Attr_Pseudo,
                "[[si]*]",
                "Statistics for instruction classes.");

        SIM_register_attribute(
                connection_class, "telemetry_data",
                get_telemetry_data,
                NULL,
                Sim_Attr_Pseudo,
                "[[sff]*]",
                "Telemetry for the core together with average values.");
}

