/******************************************************************************
INTEL CONFIDENTIAL
Copyright 2021-2025 Intel Corporation.

This software and the related documents are Intel copyrighted materials, and
your use of them is governed by the express license under which they were
provided to you (License). Unless the License provides otherwise, you may not
use, modify, copy, publish, distribute, disclose or transmit this software or
the related documents without Intel's prior written permission.

This software and the related documents are provided as is, with no express or
implied warranties, other than those that are expressly stated in the License.
******************************************************************************/

#ifndef DOCEA_OBS2_INTERFACE_H
#define DOCEA_OBS2_INTERFACE_H

#include <simics/device-api.h>
#include <simics/pywrap.h>

#include "docea_obs2_types.h"

#ifdef __cplusplus
extern "C" {
#endif

SIM_INTERFACE(docea_obs2) {
  // Return the most recent time for which a result (e.g., temperature) can be
  // computed in a deterministic manner, given all the timed events already
  // received by the simulator.
  //
  // The time result returned by this function is guaranteed to be safely usable
  // as the time arguments in result getters (e.g., temperature).
  //
  // The time result is the number of picoseconds since the start of the
  // simulation.
  docea_obs2_uint_result_t (*get_most_recent_result_time)(conf_object_t * obj);

  // Return the value of the frequency at this time. It corresponds to the value
  // set at this time or at the most recent time in the past.
  //
  // An error is returned if no applicable value is found (e.g. time is before
  // simulator initial time).
  //
  // time: the time, in picoseconds, when the value is captured.
  //
  // name: the logical name identifying the clock source.
  docea_obs2_double_result_t (*get_input_frequency)(conf_object_t * obj,
                                                    docea_obs2_time_t time,
                                                    const char* name);

  // Return the value of the voltage at this time. It corresponds to the value
  // set at this time or at the most recent time in the past.
  //
  // An error is returned if no applicable value is found (e.g. time is before
  // simulator initial time).
  //
  // time: the time, in picoseconds, when the value is captured.
  //
  // name: the logical name identifying the voltage regulator.
  docea_obs2_double_result_t (*get_input_voltage)(conf_object_t * obj,
                                                  docea_obs2_time_t time,
                                                  const char* name);

  // Return the value of the cdyn at this time. It corresponds to the value
  // set at this time or at the closest time in the future.
  //
  // In case the simulator performs cdyn aggregation (averaging over a time
  // window), this value may not be equal to the one actually applied to the
  // power model.
  //
  // An error is returned if no applicable value is found (e.g. time is after
  // the last received cdyn value for this logical name).
  //
  // time: the time, in picoseconds, when the value is captured.
  //
  // name: the logical name identifying the core/cdyn source.
  docea_obs2_double_result_t (*get_input_cdyn)(conf_object_t * obj,
                                               docea_obs2_time_t time,
                                               const char* name);

  // Return the value of the state at this time. It corresponds to the value
  // set at this time or at the most recent time in the past.
  //
  // An error is returned if no applicable value is found (e.g. time is before
  // simulator initial time).
  //
  // time: the time, in picoseconds, when the value is captured.
  //
  // name: the logical name identifying the state.
  //
  // The ownership of the character string in the result is transferred to the
  // caller, who will be in charge of freeing it.
  docea_obs2_string_result_t (*get_input_state)(conf_object_t * obj,
                                                docea_obs2_time_t time,
                                                const char* name);

  // Return the value of the aggregated cdyn at this time. It is computed from
  // cdyns received as inputs and it is the value actually applied to the power
  // model.
  //
  // In case the simulator performs no cdyn aggregation (averaging over a time
  // window), this value is equal to the input cdyn.
  //
  // An error is returned if no aggregated cdyn can be computed for this time
  // value.
  //
  // time: the time, in picoseconds, when the value is captured.
  //
  // name: the logical name identifying the core/cdyn source.
  docea_obs2_double_result_t (*get_aggregated_cdyn)(conf_object_t * obj,
                                                    docea_obs2_time_t time,
                                                    const char* name);

  // Return the value of the current at this time.
  //
  // An error is returned if provided time is beyond the time of the last time
  // (most recent) of accessible results notified by the accessible results
  // callback.
  //
  // time: the time, in picoseconds, when the value is captured.
  //
  // name: the logical name identifying the object where the current is
  // captured.
  docea_obs2_double_result_t (*get_current)(conf_object_t * obj,
                                            docea_obs2_time_t time,
                                            const char* name);

  // Return the value of the temperature at this time.
  //
  // An error is returned if provided time is beyond the last time (most recent)
  // of accessible results notified by the accessible results callback.
  //
  // time: the time, in picoseconds, when the value is captured.
  //
  // name: the logical name identifying the object where the temperature is
  // captured.
  docea_obs2_double_result_t (*get_temperature)(conf_object_t * obj,
                                                docea_obs2_time_t time,
                                                const char* name);

  // Register the callback to be called whenever a new result (e.g.,
  // temperature) is accessible in the simulator.
  //
  // callback: The function pointer to use as callback. Specify NULL to reset.
  //
  // callback_context: Private user data that will be given back to the callback
  // when triggered.
  docea_obs2_status_result_t (*set_accessible_result_callback)(conf_object_t * obj,
                                                               docea_obs2_accessible_result_callback_t callback,
                                                               cbdata_register_t callback_context);

  // Register the callback to be called when a new cdyn event is received by the
  // simulator.
  //
  // callback: The function pointer to use as callback. Specify NULL to reset.
  //
  // callback_context: Private user data that will be given back to the callback
  // when triggered.
  docea_obs2_status_result_t (*set_cdyn_event_callback)(conf_object_t * obj,
                                                        docea_obs2_cdyn_event_callback_t callback,
                                                        cbdata_register_t callback_context);
#if !defined(PYWRAP)
  // See set_accessible_result_callback, but for C language.
  docea_obs2_status_result_t (*set_accessible_result_callback_c)(conf_object_t * obj,
                                                                 docea_obs2_accessible_result_callback_c_t callback,
                                                                 void* callback_context);

  // See set_cdyn_event_callback, but for C language.
  docea_obs2_status_result_t (*set_cdyn_event_callback_c)(conf_object_t * obj,
                                                          docea_obs2_cdyn_event_callback_c_t callback,
                                                          void* callback_context);
#endif
};

#define DOCEA_OBS2_INTERFACE "docea_obs2"

#ifdef __cplusplus
}
#endif

#endif
