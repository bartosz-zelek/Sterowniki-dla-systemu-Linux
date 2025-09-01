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

#ifndef SIMICS_BASE_CLOCK_H
#define SIMICS_BASE_CLOCK_H

#include <simics/base/types.h>
#include <simics/model-iface/cycle.h>

#if defined(__cplusplus)
extern "C" {
#endif

EXPORTED int SIM_register_clock(conf_class_t *NOTNULL cls,
                                const cycle_interface_t *NOTNULL iface);

EXPORTED int VT_register_passive_clock(conf_class_t *NOTNULL cls,
                                       const cycle_interface_t *NOTNULL iface);

#if defined(__cplusplus)
}
#endif

#endif
