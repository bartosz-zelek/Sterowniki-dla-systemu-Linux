/******************************************************************************
INTEL CONFIDENTIAL
Copyright 2023-2025 Intel Corporation.

This software and the related documents are Intel copyrighted materials, and
your use of them is governed by the express license under which they were
provided to you (License). Unless the License provides otherwise, you may not
use, modify, copy, publish, distribute, disclose or transmit this software or
the related documents without Intel's prior written permission.

This software and the related documents are provided as is, with no express or
implied warranties, other than those that are expressly stated in the License.
******************************************************************************/
#ifndef DOCEA_SESSION_INTERFACE_H
#define DOCEA_SESSION_INTERFACE_H

#include <simics/device-api.h>
#include <simics/pywrap.h>

#include "docea_session_types.h"

#ifdef __cplusplus
extern "C" {
#endif

// The "session" interface given some control on lifecycle of the power/thermal
// simulation.
SIM_INTERFACE(docea_session) {
  // Restart the power/thermal simulation by replacing the instance of the
  // underling power/thermal simulator by a new one.
  //
  // If a power/thermal state has been set prior to calling this function, then
  // it will be restored (the simulator will be re-instantiated from that
  // state).
  //
  // cfg: an optional configuration for the simulation session. If not NIL, its
  // fields will be used to configure the restarted session (possibly
  // overwriting fields in the objects that implements this interface).
  docea_session_status_result_t (*restart)(conf_object_t * obj, docea_session_configuration_t * cfg);
};

#define DOCEA_SESSION_INTERFACE "docea_session"

#ifdef __cplusplus
}
#endif

#endif
