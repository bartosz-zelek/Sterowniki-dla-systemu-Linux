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

#ifndef SIMICS_BASE_EVENT_H
#define SIMICS_BASE_EVENT_H

#include <simics/base/types.h>
#include <simics/base/attr-value.h>
#include <simics/base/time.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef enum {
        Sim_EC_No_Flags = 0,
        Sim_EC_Notsaved = 1,    /* event not saved in configurations */
        Sim_EC_Slot_Early = 2,  /* event should run before events not marked
                                   with this flag */
        Sim_EC_Slot_Late = 4,   /* event should run after events not marked
                                   with this flag */
        Sim_EC_Machine_Sync = 8,/* synchronize machine on event */
        Sim_EC_No_Serialize = 16 /* can execute in parallel with execution */
} event_class_flag_t;

#ifndef PYWRAP
struct event_class {
        /* identifier, unique within the conf_class (malloced) */
        const char *name;

        /* conf class this event class belongs to; posting object must
           belong to this class. */
        conf_class_t *conf_class;

        event_class_flag_t flags;

        /* function called when event expires */
        void (*callback)(conf_object_t *obj, lang_void *data);

        /* function called when the event is removed without expiry.
           May be null.
           Must not access the Simics configuration in any way! */
        void (*destroy)(conf_object_t *obj, lang_void *data);

        /* convert event data to an attribute value.
           May be null if flags has Sim_EC_Notsaved set.
           Event should not be saved if return value is invalid. */
        attr_value_t (*get_value)(conf_object_t *obj, lang_void *data);

        /* convert attribute value to event data.
           May be null if flags has Sim_EC_Notsaved set. */
        lang_void *(*set_value)(conf_object_t *obj, attr_value_t value);

        /* human-readable event description. The returned string must be
           malloced; it will be freed by the caller. */
        char *(*describe)(conf_object_t *obj, lang_void *data);

        /* internal - absolute priority for event */
        unsigned slot;
};
#endif

typedef struct event_class event_class_t;

EXPORTED event_class_t *SIM_register_event(
        const char *NOTNULL name,
        conf_class_t *cl,
        event_class_flag_t flags,
        void (*NOTNULL callback)(conf_object_t *obj, lang_void *data),
        void (*destroy)(conf_object_t *obj, lang_void *data),
        attr_value_t (*get_value)(conf_object_t *obj, lang_void *data),
        lang_void *(*set_value)(conf_object_t *obj, attr_value_t value),
        char *(*describe)(conf_object_t *obj, lang_void *data));

EXPORTED void SIM_event_post_time(
        conf_object_t *NOTNULL clock, event_class_t *NOTNULL evclass,
        conf_object_t *NOTNULL obj, double seconds, lang_void *user_data);
EXPORTED void SIM_event_post_cycle(
        conf_object_t *NOTNULL clock, event_class_t *NOTNULL evclass,
        conf_object_t *NOTNULL obj, cycles_t cycles, lang_void *user_data);

EXPORTED void SIM_event_cancel_time(
        conf_object_t *NOTNULL clock, event_class_t *NOTNULL evclass,
        conf_object_t *NOTNULL obj,
        int (*pred)(lang_void *data, lang_void *match_data),
        lang_void *match_data);

EXPORTED cycles_t SIM_event_find_next_cycle(
        conf_object_t *NOTNULL clock, event_class_t *NOTNULL evclass,
        conf_object_t *NOTNULL obj,
        int (*pred)(lang_void *data, lang_void *match_data),
        lang_void *match_data);
EXPORTED double SIM_event_find_next_time(
        conf_object_t *NOTNULL clock, event_class_t *NOTNULL evclass,
        conf_object_t *NOTNULL obj,
        int (*pred)(lang_void *data, lang_void *match_data),
        lang_void *match_data);

EXPORTED event_class_t *SIM_get_event_class(
        conf_class_t *NOTNULL cl, const char *NOTNULL name);

EXPORTED event_class_t *VT_get_event_class(
        conf_class_t *NOTNULL cl, const char *NOTNULL name);

EXPORTED event_class_flag_t SIM_event_class_flags(
        event_class_t *NOTNULL ec);

EXPORTED void VT_stacked_post(
        conf_object_t *NOTNULL obj,
        void (*NOTNULL func)(conf_object_t *obj, lang_void *param),
        lang_void *user_data);


typedef int64 pico_secs_t;

/* <add-type id="pc_step_t def"></add-type> */
typedef simtime_t pc_step_t;

EXPORTED void SIM_event_post_step(
        conf_object_t *NOTNULL clock, event_class_t *NOTNULL evclass,
        conf_object_t *NOTNULL obj, pc_step_t steps, lang_void *user_data);

EXPORTED void SIM_event_cancel_step(
        conf_object_t *NOTNULL clock, event_class_t *NOTNULL evclass,
        conf_object_t *NOTNULL obj,
        int (*pred)(lang_void *data, lang_void *match_data),
        lang_void *match_data);
EXPORTED pc_step_t SIM_event_find_next_step(
        conf_object_t *NOTNULL clock, event_class_t *NOTNULL evclass,
        conf_object_t *NOTNULL obj,
        int (*pred)(lang_void *data, lang_void *match_data),
        lang_void *match_data);

EXPORTED pc_step_t SIM_step_count(conf_object_t *NOTNULL obj);

#if defined(__cplusplus)
}
#endif

#endif
