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

#ifndef SIMICS_PROCESSOR_TYPES_H
#define SIMICS_PROCESSOR_TYPES_H

#include <simics/base/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

#ifndef TURBO_SIMICS
/*
   <add id="device api types">
   <name index="true">data_or_instr_t</name>
   <insert id="data_or_instr_t DOC"/>
   </add> */

/* <add id="data_or_instr_t DOC">
   <ndx>data_or_instr_t</ndx>
   <doc>
   <doc-item name="NAME">data_or_instr_t</doc-item>
   <doc-item name="SYNOPSIS">
   <smaller>
   <insert id="data_or_instr_t def"/>
   </smaller>
   </doc-item>
   <doc-item name="DESCRIPTION">
   This type is used to differentiate between data and instruction, usually in
   a TBL or memory transaction context.
   </doc-item>
   </doc>
   </add>
*/

/* <add-type id="data_or_instr_t def">
   </add-type>
 */
typedef enum {
        Sim_DI_Instruction      = 0,
        Sim_DI_Data             = 1
} data_or_instr_t;

/* All combinations are allowed (logical or)
   <add id="access_t">
   <ndx>access_t</ndx>
   <insert-upto text=";"/>
   </add>
*/
typedef enum {
        Sim_Access_Read = 1,
        Sim_Access_Write = 2,
        Sim_Access_Execute = 4
} access_t;

#endif /* !TURBO_SIMICS */

/* <add id="device api types">
   <name index="true">processor_mode_t</name>
   <insert id="processor_mode_t DOC"/>
   </add> */

/* <add id="processor_mode_t DOC">
   <ndx>processor_mode_t</ndx>
   <doc>
   <doc-item name="NAME">processor_mode_t</doc-item>
   <doc-item name="SYNOPSIS"><insert id="processor_mode_t def"/></doc-item>
   <doc-item name="DESCRIPTION">
   The <type>processor_mode_t</type> data type is used to specify if a
   CPU is running in user mode or in a privileged mode (often called
   supervisor mode). For processor architectures with several
   privilege levels, the non-user levels are all identified as
   <const>Sim_CPU_Mode_Supervisor</const>.
   </doc-item>
   </doc>
   </add>

   <add-type id="processor_mode_t def"></add-type> */
typedef enum {
        Sim_CPU_Mode_User       = 0,
        Sim_CPU_Mode_Supervisor = 1,
        Sim_CPU_Mode_Hypervisor
} processor_mode_t;

typedef enum {
        Sim_Endian_Little,
        Sim_Endian_Big
} cpu_endian_t;

typedef struct {
        int                valid;
        physical_address_t address;
        physical_address_t block_start;
        physical_address_t block_end;
} physical_block_t;

#if defined(__cplusplus)
}
#endif

#endif
