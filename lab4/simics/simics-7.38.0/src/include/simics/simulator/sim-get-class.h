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

/* SIM_get_class() is split from conf-object.h to allow the interface API
   (and thus, the device API available in Model Builder) to access it without
   bringing in all the configuration functions defined in conf-object.h */

#ifndef SIMICS_SIMULATOR_SIM_GET_CLASS_H
#define SIMICS_SIMULATOR_SIM_GET_CLASS_H

#include <simics/base/conf-object.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* Get registered class (returns NULL if class not found). */
EXPORTED conf_class_t *SIM_get_class(const char *NOTNULL name);

#if defined(__cplusplus)
}
#endif

#endif
