/******************************************************************************
INTEL CONFIDENTIAL
Copyright 2020-2025 Intel Corporation.

This software and the related documents are Intel copyrighted materials, and
your use of them is governed by the express license under which they were
provided to you (License). Unless the License provides otherwise, you may not
use, modify, copy, publish, distribute, disclose or transmit this software or
the related documents without Intel's prior written permission.

This software and the related documents are provided as is, with no express or
implied warranties, other than those that are expressly stated in the License.
******************************************************************************/

#ifndef DOCEA_OBS_INTERFACE_H
#define DOCEA_OBS_INTERFACE_H

#include <simics/base/cbdata.h>
#include <simics/device-api.h>
#include <simics/pywrap.h>

#include "docea_obs_types.h"

#ifdef __cplusplus
extern "C" {
#endif

// Below are defined the types of the callback functions that can be registered.
// Each callback function accepts a private data (the one specified by the
// caller when registering the callback), plus the arguments that are relevant
// to the event that triggers the callback. Each callback type comes with two
// variants: the first is compatible with languages supported by Simics, namely
// Python, the second one (the "_c" variant) is specific to the C language.
typedef void (*docea_accessible_results_callback_t)(
    cbdata_call_t callback_context, double time);
typedef void (*docea_accessible_results_callback_c_t)(void *callback_context,
                                                      double time);
typedef void (*docea_cdyn_event_callback_t)(cbdata_call_t callback_context,
                                            double time, docea_id_t id,
                                            double value);
typedef void (*docea_cdyn_event_callback_c_t)(void *callback_context,
                                              double time, docea_id_t id,
                                              double value);

SIM_INTERFACE(docea_obs) {
  // Return the most recent time for which outputs can be computed on a
  // deterministic manner, given the internal state of the simulator (more
  // precisely, given the update and read events previously received).
  //  - When results_delay is set to 0, the time for which outputs can be
  //    computed is the most recent time value before 'time' (or 'time') for
  //    which all entities that expect cdyns or performance counters inputs have
  //    received them.
  //  - When results_delay is set to a > 0 value to deal with
  //    temporally-decoupled notification of events, to comply with the
  //    deterministic requirement, the time for which outputs can be computed
  //    also depends on the results_delay.
  //
  // In case cdyn aggregation is activated, the returned time value may be a bit
  // further in the past than that in order not to return time values in the
  // middle of a cdyn aggregation window. This makes the returned value
  // consistent with time values notified through the
  // accessible_results_callback.
  //
  // The time returned by the latter function is guaranteed to be ok to be used
  // as an input time in observer functions to read outputs (e.g. temperature).
  docea_double_result_t (*most_recent_results_time)(conf_object_t * obj);

  // Return the temperature at a given time for a given id. It will be computed
  // if needed, but this calculation will not impact the simulation in any way.
  // A status != 0 is returned in the result if the provided time is beyond the
  // time of the most recent time as notified by the accessible results
  // callback.
  docea_double_result_t (*temperature)(conf_object_t * obj, double time,
                                       docea_id_t id);

  // Return the current at a given time for a given id. It will be computed
  // if needed, but this calculation will not impact the simulation in any way.
  // A status != 0 is returned in the result if the provided time is beyond the
  // time of the most recent time as notified by the accessible results
  // callback.
  docea_double_result_t (*current)(conf_object_t * obj, double time,
                                   docea_id_t id);

  // Get the aggregated cdyn value corresponding to a provided id for a given
  // time. It will correspond to the aggregated value of cdyn which is
  // applicable for this value of time, computed from cdyns received in inputs.
  // In case the simulator performs no cdyn aggregation (averaging over a time
  // window), this value is equal to the applicable input cdyn for the same
  // time.
  // A status != 0 is returned in the result if the provided time is beyond the
  // the most recent time, as notified by the accessible results callback.
  docea_double_result_t (*aggregated_cdyn)(conf_object_t * obj, double time,
                                           docea_id_t id);

  // Get the applicable frequency value corresponding to a provided id for a
  // given time. It will correspond to the value set in input for this time if
  // it exists, or to the one set to the most recent time in the past.
  // A status != 0 is returned in the result if no applicable value is found
  // (e.g. time is before simulator initial time).
  docea_double_result_t (*frequency)(conf_object_t * obj, double time,
                                     docea_id_t id);

  // Get the applicable voltage value corresponding to a provided id for a
  // given time. It will correspond to the value set in input for this time if
  // it exists, or to the one set to the most recent time in the past.
  // A status != 0 is returned in the result if no applicable value is found
  // (e.g. time is before simulator initial time).
  docea_double_result_t (*voltage)(conf_object_t * obj, double time,
                                   docea_id_t id);

  // Get the applicable cdyn value corresponding to a provided id for a given
  // time. It will correspond to the value received in input for this time if
  // it exists, or to the next set value in the future.
  // In case the simulator performs cdyn aggregation (averaging over a time
  // window), this value may not be equal to the one actually applied to the
  // power model after aggregation.
  // A status != 0 is returned in the result if no applicable value is found
  // (e.g. time is after the last received cdyn value for this id)
  docea_double_result_t (*raw_cdyn)(conf_object_t * obj, double time,
                                    docea_id_t id);

  // Set the Simics callback to be called when new current/temperature results
  // are accessible for a timing value, i.e. when the time horizon value of the
  // most recent results time has changed. Results are accessible means that all
  // the required events have been received to be able to compute these results.
  //
  // In case the simulator has a results_delay > 0 set, the time horizon
  // reflects this delay so that simulation outputs are deterministic in case of
  // temporally decoupled notification of events.
  // Hence this callback also reflects this delay, and will only be triggered
  // with time values for which results do not depend on events the simulator
  // might have not received yet.
  // The fact that the callback is triggered does not mean that the results have
  // necessarily been computed. They will be if requested.
  //
  // Note that in case cdyn aggregation is active, this callback will NOT be
  // triggered automatically for every cdyn received that moves forward the time
  // horizon, but only when enough cdyn have been received to aggregate them, to
  // be consistent in notification with timesteps actually used by the
  // simulation.
  //
  // callback_context: private data that will be given back to the caller by the
  // callback in first argument when it is triggered.
  docea_status_result_t (*set_accessible_results_callback)(
      conf_object_t * obj, docea_accessible_results_callback_t callback,
      cbdata_register_t _callback_context);

  // Set the callback to be called when a new cdyn event is received.
  docea_status_result_t (*set_cdyn_event_callback)(
      conf_object_t * obj, docea_cdyn_event_callback_t callback,
      cbdata_register_t callback_context);

#if !defined(PYWRAP)
  // See set_accessible_results_callback, but set a C-specific callback.
  docea_status_result_t (*set_accessible_results_callback_c)(
      conf_object_t * obj, docea_accessible_results_callback_c_t callback,
      void *callback_context);

  // See set_cdyn_event_callback, but set a C-specific callback.
  docea_status_result_t (*set_cdyn_event_callback_c)(
      conf_object_t * obj, docea_cdyn_event_callback_c_t callback,
      void *callback_context);
#endif
};

#define DOCEA_OBS_INTERFACE "docea_obs"

#ifdef __cplusplus
}
#endif

#endif
