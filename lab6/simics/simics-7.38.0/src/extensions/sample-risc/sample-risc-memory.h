/*
  sample-risc-memory.h - sample code for the page cache and memory interface

  © 2010 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SAMPLE_RISC_MEMORY_H
#define SAMPLE_RISC_MEMORY_H

#ifndef SAMPLE_RISC_HEADER
#define SAMPLE_RISC_HEADER "sample-risc.h"
#endif
#include SAMPLE_RISC_HEADER

#include "sample-risc.h"

void register_memory_interfaces(conf_class_t *cls);
void register_memory_attributes(conf_class_t *cls);

void init_page_cache(sample_risc_t *sr);

void check_virtual_breakpoints(sample_risc_t *sr,
                               sample_risc_core_t *core,
                               access_t access,
                               logical_address_t virt_start,
                               generic_address_t len,
                               uint8 *data);

bool write_memory(sample_risc_t *sr,
                  sample_risc_core_t *core,
                  physical_address_t phys_address,
                  physical_address_t len,
                  uint8 *data, bool check_bp);

bool read_memory(sample_risc_t *sr,
                 sample_risc_core_t *core,
                 physical_address_t phys_address,
                 physical_address_t len,
                 uint8 *data, bool check_bp);

bool fetch_instruction(sample_risc_t *sr,
                       sample_risc_core_t *core,
                       physical_address_t phys_address,
                       physical_address_t len,
                       uint8 *data,
                       bool check_bp);

sample_page_t *
get_page(sample_risc_t *sr, sample_risc_core_t *core,
         conf_object_t *phys_mem_obj,
         physical_address_t address, access_t access);

sample_page_t *
add_to_page_cache(sample_risc_t *sr, sample_page_t p);

sample_page_t *
search_page_cache(sample_risc_t *sr, conf_object_t *phys_mem_obj,
                  physical_address_t address);

void
clear_page_cache(sample_risc_t *sr);

void register_local_memory_interfaces(conf_class_t *cls);
set_error_t local_core_set_phys_memory(sample_risc_core_t *core,
                                       conf_object_t *oval);

#endif
