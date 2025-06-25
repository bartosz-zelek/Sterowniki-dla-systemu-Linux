/*
  © 2022 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_BASE_BREAKPOINTS_H
#define SIMICS_BASE_BREAKPOINTS_H

#include <simics/base/types.h>
#include <simics/processor/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add id="breakpoint_kind_t">
   <ndx>breakpoint_kind_t</ndx>
   <insert-upto text=";"/>
   </add>
 */
typedef enum {
        Sim_Break_Physical = 0,
        Sim_Break_Virtual  = 1,
        Sim_Break_Linear   = 2      /* x86 only */
} breakpoint_kind_t;

/* <add-type id="breakpoint_id_t"><ndx>breakpoint_id_t</ndx></add-type> */
typedef int breakpoint_id_t;

typedef uintptr_t breakpoint_handle_t;
#define BREAKPOINT_HANDLE_ANY ((uintptr_t)(-1))
#define BREAKPOINT_HANDLE_INVALID 0

typedef struct {
        breakpoint_handle_t   handle;
        access_t              read_write_execute;
        generic_address_t     start;
        generic_address_t     end;
} breakpoint_info_t;

typedef struct {
        int                   num_breakpoints;
        breakpoint_info_t    *breakpoints;
} breakpoint_set_t;

#if defined(__cplusplus)
}
#endif

#endif
