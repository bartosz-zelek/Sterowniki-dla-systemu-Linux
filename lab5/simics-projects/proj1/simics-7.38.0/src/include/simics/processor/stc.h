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

#ifndef SIMICS_PROCESSOR_STC_H
#define SIMICS_PROCESSOR_STC_H

#include <simics/base/types.h>
#include <simics/base/memory.h>

#if defined(__cplusplus)
extern "C" {
#endif

EXPORTED void SIM_flush_I_STC_logical(
        conf_object_t *NOTNULL cpu,
        logical_address_t vaddr, logical_address_t length);
EXPORTED void SIM_flush_D_STC_logical(
        conf_object_t *NOTNULL cpu,
        logical_address_t vaddr, logical_address_t length);
EXPORTED void SIM_flush_I_STC_physical(
        conf_object_t *NOTNULL cpu,
        physical_address_t paddr, physical_address_t length);
EXPORTED void SIM_flush_D_STC_physical(
        conf_object_t *NOTNULL cpu,
        physical_address_t paddr, physical_address_t length,
        read_or_write_t read_or_write);
EXPORTED void SIM_STC_flush_cache(conf_object_t *NOTNULL cpu);

#if defined(__cplusplus)
}
#endif

#endif
