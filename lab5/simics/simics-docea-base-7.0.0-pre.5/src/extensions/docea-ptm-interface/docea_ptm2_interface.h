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

#ifndef DOCEA_PTM2_INTERFACE_H
#define DOCEA_PTM2_INTERFACE_H

#include <simics/device-api.h>
#include <simics/pywrap.h>

#include "docea_ptm2_types.h"

#ifdef __cplusplus
extern "C" {
#endif

SIM_INTERFACE(docea_ptm2) {
  // Set the frequency to the new value.
  //
  // It is forbidden to change the past: an error is returned if the time is
  // older than the latest time for the same logical name.
  //
  // time: the instant, in picoseconds, when the frequency changes to become the
  // given value.
  //
  // name: the logical name identifying the clock source.
  //
  // value: the new frequency value, in Hz.
  docea_ptm2_status_result_t (*set_frequency)(conf_object_t * obj,
                                              docea_ptm2_time_t time,
                                              const char* name,
                                              double value);

  // Set the voltage to the new value.
  //
  // It is forbidden to change the past: an error is returned if the time is
  // older than the latest time for the same logical name.
  //
  // time: the instant, in picoseconds, when the voltage changes to become the
  // given value.
  //
  // name: the logical name identifying the voltage regulator output.
  //
  // value: the new voltage value, in V, of the voltage regulator output.
  docea_ptm2_status_result_t (*set_voltage)(conf_object_t * obj,
                                            docea_ptm2_time_t time,
                                            const char* name,
                                            double value);

  // Set the given heat transfer coefficient parameter to the new value.
  //
  // It is forbidden to change the past: an error is returned if the time is
  // older than the latest time for the same logical name.
  //
  // time: the instant, in picoseconds, when the heat transfer coefficient changes
  // to become the given value.
  //
  // name: the name of the parameter that drives this heat transfer coefficient.
  // Memory ownership is NOT taken.
  //
  // value: the new HTC value, in W/(K.m²).
  docea_ptm2_status_result_t (*set_heat_transfer_coefficient)(conf_object_t * obj,
                                                              docea_ptm2_time_t time,
                                                              const char* name,
                                                              double value);

  // Set the given power consumer temperature parameter to the new value.
  //
  // It is forbidden to change the past: an error is returned if the time is
  // older than the latest time for the same logical name.
  //
  // time: the instant, in picoseconds, when the power consumer temperature
  // changes to become the given value.
  //
  // name: the logical name identifying the power model entity where to apply
  // the new power consumer temperature. Memory ownership is NOT taken.
  //
  // value: the new power consumer temperature, in °C.
  docea_ptm2_status_result_t (*set_power_consumer_temperature)(conf_object_t * obj,
                                                               docea_ptm2_time_t time,
                                                               const char* name,
                                                               double value);

  // Set the power model state to the new value.
  //
  // It is forbidden to change the past: an error is returned if the time is
  // older than the latest time for the same logical name.
  //
  // time: the instant, in picoseconds, when the state changes to become the
  // given value.
  //
  // name: the logical name identifying the power model entity where to apply
  // the new state. Memory ownership is NOT taken.
  //
  // value: the new state value. Memory ownership is NOT taken.
  docea_ptm2_status_result_t (*set_state)(conf_object_t * obj,
                                          docea_ptm2_time_t time,
                                          const char* name,
                                          const char* value);

  // Get the current specified by its power/thermal name at the given time.
  //
  // time: the instant, in picoseconds, when the value is requested.
  // The current may not be available at this time because some inputs, e.g.,
  // Cdyn, may be missing. If so, the latest available current is returned. The
  // result contains the time for which the current could be computed.
  //
  // name: the logical name identifying the object where the current is
  // requested.
  //
  // The returned current is in A.
  docea_ptm2_double_timed_result_t (*get_current)(conf_object_t * obj,
                                                  docea_ptm2_time_t time,
                                                  const char* name);

  // Get the power specified by its power/thermal name at the given time.
  //
  // time: the instant, in picoseconds, when the value is requested.
  // The power may not be available at this time because some inputs, e.g.,
  // Cdyn, may be missing. If so, the latest available power is returned. The
  // result contains the time for which the power could be computed.
  //
  // name: the logical name identifying the object where the power is
  // requested.
  //
  // The returned power is in W.
  docea_ptm2_double_timed_result_t (*get_power)(conf_object_t * obj,
                                                docea_ptm2_time_t time,
                                                const char* name);

  // Get the temperature specified by its power/thermal name at the given time.
  //
  // time: the instant, in picoseconds, when the value is requested.
  // The temperature may not be available at this time because some inputs,
  // e.g., Cdyn, may be missing. If so, the latest available temperature is
  // returned. The result contains the time for which the temperature could be
  // computed.
  //
  // name: the logical name identifying the object where the temperature is
  // requested.
  //
  // The returned temperature is in °C.
  docea_ptm2_double_timed_result_t (*get_temperature)(conf_object_t * obj,
                                                      docea_ptm2_time_t time,
                                                      const char* name);

  // Get the power consumer temperature specified by its power/thermal name at
  // the given time.
  //
  // time: the instant, in picoseconds, when the value is requested.
  // The temperature may not be available at this time because some inputs,
  // e.g., Cdyn, may be missing. If so, the latest available temperature is
  // returned. The result contains the time for which the current could be
  // computed.
  //
  // name: the logical name identifying the object where the power consumer
  // temperature is requested
  //
  // The returned temperature is in °C.
  docea_ptm2_double_timed_result_t (*get_power_consumer_temperature)(conf_object_t * obj,
                                                                     docea_ptm2_time_t time,
                                                                     const char* name);
};

#define DOCEA_PTM2_INTERFACE "docea_ptm2"

#ifdef __cplusplus
}
#endif

#endif /* DOCEA_PTM2_INTERFACE_H */
