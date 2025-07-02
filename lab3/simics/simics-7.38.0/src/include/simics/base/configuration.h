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

#ifndef SIMICS_BASE_CONFIGURATION_H
#define SIMICS_BASE_CONFIGURATION_H

#include <simics/base/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

EXPORTED bool SIM_is_restoring_state(conf_object_t *obj);

/* will return 1 while getting persistent attributes only */
EXPORTED bool VT_is_saving_persistent_data(void);

EXPORTED conf_object_t *VT_object_cell(conf_object_t *NOTNULL obj);
EXPORTED uint32 VT_get_map_generation(conf_object_t *NOTNULL obj);

#if defined(__cplusplus)
}
#endif

#endif
