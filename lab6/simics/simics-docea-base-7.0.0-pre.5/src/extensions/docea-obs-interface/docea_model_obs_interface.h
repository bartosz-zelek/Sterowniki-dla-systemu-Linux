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

#ifndef DOCEA_MODEL_OBS_INTERFACE_H
#define DOCEA_MODEL_OBS_INTERFACE_H

#include <simics/device-api.h>
#include <simics/pywrap.h>

#include "docea_model_obs_types.h"

#ifdef __cplusplus
extern "C" {
#endif

SIM_INTERFACE(docea_model_obs) {
  docea_model_obs_double_result_t (*power_float_variable)(conf_object_t * obj,
                                                          docea_model_obs_time_t time,
                                                          const char *variable_path);

  docea_model_obs_bool_result_t (*power_bool_variable)(conf_object_t * obj,
                                                       docea_model_obs_time_t time,
                                                       const char *variable_path);

  docea_model_obs_string_result_t (*power_string_variable)(conf_object_t * obj,
                                                           docea_model_obs_time_t time,
                                                           const char *variable_path);

  docea_model_obs_double_result_t (*thermal_injected_power)(conf_object_t * obj,
                                                            docea_model_obs_time_t time,
                                                            const char *input_path);

  docea_model_obs_double_result_t (*thermal_source_temperature)(conf_object_t * obj,
                                                                docea_model_obs_time_t time,
                                                                const char *source_path);

  docea_model_obs_double_result_t (*thermal_probe_temperature)(conf_object_t * obj,
                                                               docea_model_obs_time_t time,
                                                               const char *probe_path);
};

#define DOCEA_MODEL_OBS_INTERFACE "docea_model_obs"

#ifdef __cplusplus
}
#endif

#endif
