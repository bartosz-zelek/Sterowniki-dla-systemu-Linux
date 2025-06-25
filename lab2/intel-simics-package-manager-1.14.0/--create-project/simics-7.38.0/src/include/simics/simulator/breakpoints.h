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

#ifndef SIMICS_SIMULATOR_BREAKPOINTS_H
#define SIMICS_SIMULATOR_BREAKPOINTS_H

#include <simics/base/breakpoints.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add-type id="enum breakpoint_flag"></add-type> */
typedef enum breakpoint_flag {
        Sim_Breakpoint_Temporary = 1,
        Sim_Breakpoint_Simulation = 2,
        Sim_Breakpoint_Private = 4
} breakpoint_flag_t;

EXPORTED breakpoint_id_t SIM_breakpoint(
        conf_object_t *NOTNULL obj, breakpoint_kind_t type, access_t access,
        uint64 address, uint64 length, breakpoint_flag_t flags);
EXPORTED void SIM_delete_breakpoint(breakpoint_id_t id);
EXPORTED void SIM_enable_breakpoint(breakpoint_id_t id);
EXPORTED void SIM_disable_breakpoint(breakpoint_id_t id);
EXPORTED void SIM_breakpoint_remove(
        breakpoint_id_t id, access_t access,
        generic_address_t address, generic_address_t length);

#if defined(__cplusplus)
}
#endif

#endif
