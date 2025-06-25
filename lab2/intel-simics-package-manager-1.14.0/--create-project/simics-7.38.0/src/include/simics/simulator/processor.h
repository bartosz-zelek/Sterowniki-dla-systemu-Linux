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

#ifndef SIMICS_SIMULATOR_PROCESSOR_H
#define SIMICS_SIMULATOR_PROCESSOR_H

#include <simics/base/types.h>
#include <simics/base/attr-value.h>

#if defined(__cplusplus)
extern "C" {
#endif

EXPORTED bool SIM_object_is_processor(conf_object_t *NOTNULL obj);

EXPORTED int SIM_number_processors(void);

EXPORTED void SIM_reset_processor(conf_object_t *NOTNULL cpu, int hard_reset); 

EXPORTED attr_value_t SIM_get_all_processors(void);
EXPORTED conf_object_t *SIM_get_processor(int proc_no);
EXPORTED int SIM_get_processor_number(const conf_object_t *NOTNULL cpu);
EXPORTED conf_object_t *VT_get_current_processor(void);
EXPORTED conf_object_t *VT_get_current_processor_old(void);

EXPORTED int SIM_processor_privilege_level(conf_object_t *NOTNULL cpu);

EXPORTED tuple_int_string_t SIM_disassemble_address(
        conf_object_t *NOTNULL cpu, generic_address_t address,
        int logical, int sub);

EXPORTED conf_object_t *VT_first_clock(void);

EXPORTED conf_object_t *SIM_current_clock(void);

#if defined(__cplusplus)
}
#endif

#endif
