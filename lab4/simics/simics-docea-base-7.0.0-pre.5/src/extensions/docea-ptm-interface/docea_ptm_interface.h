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

#ifndef DOCEA_PTM_INTERFACE_H
#define DOCEA_PTM_INTERFACE_H

#include <simics/device-api.h>
#include <simics/pywrap.h>
#include <stddef.h>

#include "docea_ptm_types.h"

#ifdef __cplusplus
extern "C" {
#endif

SIM_INTERFACE(docea_ptm) {
  /* Update a frequency that changes at the given time to become the given
   value.

   obj: The object implementing this interface.

   time: the instant, in seconds, when the frequency changes to become the given
   value.

   id: the unique identifier of the clock source.

   value: the new frequency value, in Hz. */
  docea_status_result_t (*update_frequency)(conf_object_t * obj, double time,
                                            docea_id_t id, double value);

  /* Update a voltage that changes at the given time to become the given value.

   obj: The object implementing this interface.

   time: the instant, in seconds, when the voltage changes to become the given
   value.

   id: the unique identifier of the voltage regulator output.

   value: the new voltage value, in V. */
  docea_status_result_t (*update_voltage)(conf_object_t * obj, double time,
                                          docea_id_t id, double value);

  /* Update a power model state that changed at the given time to become the
     given value.

   obj: The object implementing this interface.

   time: the instant, in seconds, when the state changes to become the given
   value.

   id: the unique identifier of the power model entity where to apply the new
   state.

   value: the new state value. If that was a dynamically allocated char*, the
   method will not take ownership. A string literal can also be used. */
  docea_status_result_t (*update_state)(conf_object_t * obj, double time,
                                        docea_id_t id, const char *value);

  /* Get a current at a given time.

   obj: The object implementing this interface.

   time: the time, in seconds, when the value is requested. The result may
   be not computable up to the exact given time, hence it may be returned
   for a lesser time. The time in the returned result gives the exact time
   for which the current could be computed.

   id: unique identifier of the object where the current is captured. */
  docea_double_timed_result_t (*current)(conf_object_t * obj, double time,
                                         docea_id_t id);

  /* Get a temperature at a given time.

   obj: The object implementing this interface.

   time: the time, in seconds, when the value is requested. The result may
   be not computable up to the exact given time, hence it may be returned
   for a lesser time. The time in the returned result gives the exact time
   for which the temperature could be computed.

   id: unique identifier of the object where the temperature is captured. */
  docea_double_timed_result_t (*temperature)(conf_object_t * obj, double time,
                                             docea_id_t id);
};

#define DOCEA_PTM_INTERFACE "docea_ptm"

#ifdef __cplusplus
}
#endif

#endif /* DOCEA_PTM_INTERFACE_H */
