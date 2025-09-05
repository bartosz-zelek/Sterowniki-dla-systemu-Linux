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

#ifndef SIMICS_SIMULATOR_CALLBACKS_H
#define SIMICS_SIMULATOR_CALLBACKS_H

#include <simics/base/types.h>
#include <simics/base/event.h>
#include <simics/pywrap.h>

#if defined _WIN32 && (defined PYWRAP || defined GULP_API_HELP)
 #include <winsock2.h>
 #include <windows.h>
#endif

#if defined(__cplusplus)
extern "C" {
#endif

typedef enum {
        Sim_NM_Read,            /* when ready for reading/accept, or closed */
        Sim_NM_Write            /* when ready for writing/connect */
} notify_mode_t;

#ifdef _WIN32
EXPORTED void SIM_notify_on_object(
 /* Only use HANDLE as the argument type when generating Python bindings and
    the API reference, since we don't want all users of this header to get the
    Windows headers automatically. */
 #if defined PYWRAP || defined GULP_API_HELP
        HANDLE
 #else
        void *
 #endif
              obj,
        int run_in_thread,
        void (*callback)(lang_void *data), lang_void *data);
#else
EXPORTED void SIM_notify_on_descriptor(
        int fd, notify_mode_t mode, int run_in_thread,
        void (*callback)(lang_void *data), lang_void *data);
#endif
EXPORTED void SIM_notify_on_socket(
        socket_t sock, notify_mode_t mode, int run_in_thread,
        void (*callback)(lang_void *data), lang_void *data);

EXPORTED void SIM_register_work(
        void (*NOTNULL f)(lang_void *data), lang_void *data);

EXPORTED int SIM_process_work(
        int (*NOTNULL done)(lang_void *done_data), lang_void *done_data);
EXPORTED int SIM_process_pending_work(void);

EXPORTED void VT_post_local_work(
        conf_object_t *NOTNULL cell,
        void (*NOTNULL f)(lang_void *data), lang_void *data);

EXPORTED int64 SIM_realtime_event(
        unsigned delay_ms,
        void (*NOTNULL callback)(lang_void *data), lang_void *data,
        int run_in_thread, const char *desc);

EXPORTED int SIM_cancel_realtime_event(int64 id);

EXPORTED void SIM_run_alone(
        void (*NOTNULL f)(lang_void *data), lang_void *data);

EXPORTED void SIM_thread_safe_callback(
        void (*NOTNULL f)(lang_void *data), lang_void *data);

EXPORTED void SIM_run_async_work(
        lang_void *(*NOTNULL async_call)(lang_void *arg),
        void (*async_ready)(lang_void *async_ret, lang_void *arg),
        lang_void *arg);

EXPORTED void SIM_run_in_thread(
        void (*NOTNULL f)(lang_void *arg),
        lang_void *arg);

EXPORTED void VT_domain_event_soon(
        conf_object_t *NOTNULL domain, event_class_t *NOTNULL ec,
        conf_object_t *NOTNULL obj, lang_void *param);
EXPORTED void VT_domain_event_at(
        conf_object_t *NOTNULL domain, event_class_t *NOTNULL ec,
        conf_object_t *NOTNULL obj, conf_object_t *NOTNULL clock, double when,
        lang_void *param);

EXPORTED void VT_cross_cell_call(
        void (*NOTNULL f)(conf_object_t *obj, lang_void *arg),
        conf_object_t *NOTNULL obj,
        lang_void *arg);

EXPORTED void VT_process_cross_cell_calls(void);
EXPORTED void VT_dispatch_async_events(void);

EXPORTED void VT_run_outside_cell(void (*NOTNULL f)(lang_void *arg),
                                  lang_void *arg);

EXPORTED void VT_add_telemetry_data(const char *NOTNULL group,
                                    const char *NOTNULL key,
                                    attr_value_t *NOTNULL value);

#ifndef GULP
EXPORTED void VT_add_telemetry_data_str(const char *NOTNULL group,
                                        const char *NOTNULL key,
                                        const char *NOTNULL value);
EXPORTED void VT_add_telemetry_data_int(const char *NOTNULL group,
                                        const char *NOTNULL key,
                                        uint64 value);
EXPORTED void VT_add_telemetry_data_bool(const char *NOTNULL group,
                                         const char *NOTNULL key,
                                         bool value);
#endif
        
SIM_INTERFACE(telemetry_manager) {
        void (*add_data)(conf_object_t *obj,
                         const char *group, const char *key,
                         attr_value_t *value);
};

#define TELEMETRY_MANAGER_INTERFACE "telemetry_manager"

EXPORTED void SIM_trigger_global_message(const char *NOTNULL msg,
                                         lang_void *ref);
EXPORTED const char *SIM_get_global_message(lang_void *NOTNULL ref);

#if defined(__cplusplus)
}
#endif

#endif
