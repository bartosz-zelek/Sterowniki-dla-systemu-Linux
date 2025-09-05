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

#ifndef SIMICS_PROCESSOR_GENERIC_SPR_H
#define SIMICS_PROCESSOR_GENERIC_SPR_H

#include <simics/base/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add-type id="gen_spr_access_type_t def"></add-type> */
typedef enum {
        /* Access from a mfspr/mtspr instruction */
        Sim_Gen_Spr_Instruction_Access,

        /* Access through attribute */
        Sim_Gen_Spr_Attribute_Access,

        /* Access through int_register interface */
        Sim_Gen_Spr_Int_Register_Access,

        /* For compatibility with former PPC-only implementation */
        Sim_PPC_Spr_Instruction_Access  = Sim_Gen_Spr_Instruction_Access,
        Sim_PPC_Spr_Attribute_Access    = Sim_Gen_Spr_Attribute_Access,
        Sim_PPC_Spr_Int_Register_Access = Sim_Gen_Spr_Int_Register_Access
} gen_spr_access_type_t;

/* <add-type id="gen_spr_ret_t def"></add-type> */
typedef enum {
        Sim_Gen_Spr_Ok,        /* SPR access was OK */
        Sim_Gen_Spr_Illegal,   /* SPR access should trigger illegal insn exc */
        Sim_Gen_Spr_Privilege, /* SPR access should trigger privilege exc */

        Sim_Gen_Spr_Processor_Sleeps, /* SPR access suspends the processor */

        /* For compatibility with former PPC-only implementation */
        Sim_PPC_Spr_Ok        = Sim_Gen_Spr_Ok,
        Sim_PPC_Spr_Illegal   = Sim_Gen_Spr_Illegal,
        Sim_PPC_Spr_Privilege = Sim_Gen_Spr_Privilege
} gen_spr_ret_t;

#ifndef TURBO_SIMICS
/* <add-type id="gen_spr_user_getter_func_t def"></add-type> */
typedef gen_spr_ret_t (*gen_spr_user_getter_func_t)(
        conf_object_t *cpu,
        int64 spr_number,
        gen_spr_access_type_t type,
        lang_void *user_data);

/* <add-type id="gen_spr_user_setter_func_t def"></add-type> */
typedef gen_spr_ret_t (*gen_spr_user_setter_func_t)(
        conf_object_t *cpu,
        int64 spr_number,
        uint64 value,
        gen_spr_access_type_t type,
        lang_void *user_data);
#endif  /* !TURBO_SIMICS */

#if defined(__cplusplus)
}
#endif

#endif
