/*
  © 2021 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_BASE_DIRECT_MEMORY_H
#define SIMICS_BASE_DIRECT_MEMORY_H

#include <simics/base/types.h>
#include <simics/processor/types.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* Opaque type */
typedef struct granted_mem granted_mem_t;
/* <add-type id="direct_memory_handle_t"></add-type> */
typedef granted_mem_t *direct_memory_handle_t;

/* <add id="direct_memory_t">
   <ndx>direct_memory_t</ndx>
   <insert-upto text="direct_memory_t;"/>
   </add> */
typedef struct {
#ifndef PYWRAP
        uint8                 *data;
#endif
        access_t               permission;
        access_t               inhibit;
} direct_memory_t;

/* <add-type id="direct_memory_ack_id_t"></add-type> */
typedef uint64 direct_memory_ack_id_t;

/* <add id="direct_memory_lookup_t">
   <ndx>direct_memory_t</ndx>
   <insert-upto text="direct_memory_lookup_t;"/>
   </add> */
typedef struct {
        conf_object_t     *target;
        uint64             offs;

        access_t           breakpoints;      // conflicting breakpoints
        access_t           tracers;          // conflicting tracers
        access_t           access;           // handle valid for access
} direct_memory_lookup_t;

/* <add id="direct_memory_tags_t">
   <ndx>direct_memory_tags_t</ndx>
   <insert-upto text="direct_memory_tags_t;"/>
   </add> */
typedef struct {
#ifndef PYWRAP
        uint8 *data;
#endif
        unsigned len;
} direct_memory_tags_t;


#if defined(__cplusplus)
}
#endif

#endif   /* SIMICS_BASE_DIRECT_MEMORY_H */
