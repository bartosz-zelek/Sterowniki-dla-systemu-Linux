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

#ifndef SIMICS_SIMULATOR_MEMORY_H
#define SIMICS_SIMULATOR_MEMORY_H

#include <simics/base/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

EXPORTED uint64 SIM_read_phys_memory(
        conf_object_t *NOTNULL cpu, physical_address_t paddr, int length);

EXPORTED void SIM_write_phys_memory(conf_object_t *NOTNULL cpu,
                                    physical_address_t paddr,
                                    uint64 value, int length);

EXPORTED uint8 SIM_read_byte(
        conf_object_t *NOTNULL obj, generic_address_t paddr);

EXPORTED void SIM_write_byte(
        conf_object_t *NOTNULL obj, generic_address_t paddr, uint8 value);

EXPORTED uint64 SIM_read_phys_memory_tags(
        conf_object_t *NOTNULL mem_space,
        physical_address_t paddr, unsigned ntags);
EXPORTED void SIM_write_phys_memory_tags(
        conf_object_t *NOTNULL mem_space, physical_address_t paddr,
        uint64 tag_bits, unsigned ntags);

EXPORTED physical_address_t SIM_load_binary(
        conf_object_t *NOTNULL obj, const char *NOTNULL file,
        physical_address_t offset, bool use_pa, bool verbose);

EXPORTED void SIM_load_file(
        conf_object_t *NOTNULL obj, const char *NOTNULL file,
        physical_address_t paddr, bool verbose);

#if defined(__cplusplus)
}
#endif

#endif
