/*
  toy-risc-core.h

  © 2010 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef TOY_RISC_CORE_H
#define TOY_RISC_CORE_H

#include "sample-risc-core.h"

/* global initialization -- called only once (at the beginning) */
void init_toy(void *cr_class);

/* create a new processor instance for a core */
void *toy_new_instance(sample_risc_core_t *core);

/* get and set the program counter */
void toy_set_pc(sample_risc_core_t *core, logical_address_t pc);
logical_address_t toy_get_pc(sample_risc_core_t *core);

/* access_type is Sim_Access_Read, Sim_Access_Write, Sim_Access_Execute */
/* returns boolean */
int toy_logical_to_physical(sample_risc_core_t *core,
                            logical_address_t ea,
                            physical_address_t *pa,
                            access_t access_type);

char *toy_string_decode(sample_risc_core_t *core, uint32 instr);

bool toy_fetch_and_execute_instruction(sample_risc_core_t *core);

#endif
