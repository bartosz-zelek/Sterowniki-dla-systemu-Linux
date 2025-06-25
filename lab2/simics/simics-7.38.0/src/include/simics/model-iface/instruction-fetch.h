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

#ifndef SIMICS_MODEL_IFACE_INSTRUCTION_FETCH_H
#define SIMICS_MODEL_IFACE_INSTRUCTION_FETCH_H

#include <simics/base/types.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/*
   <add id="instruction_fetch_interface_t">

   The <iface>instruction_fetch</iface> interface is implemented by
   processors. The interface controls how instruction fetches should
   be modeled.

   The <fun>get_mode</fun> and <fun>set_mode</fun> functions get and
   set the instruction fetch mode. There are three available
   modes. All modes are not supported by all processor types. The
   <em>instruction_fetch_none</em> mode is the least accurate but the
   fastest mode. The other modes are more accurate but slower.

   The <fun>get_line_size</fun> and <fun>set_line_size</fun> functions
   get and set the fetch size on each instruction fetch. This is often
   related to cache line size or similar. The line size must be power
   of 2.

   <insert-until text="// ADD INTERFACE instruction_fetch_interface_t"/>

   </add>
   <add id="instruction_fetch_interface_exec_context">
   <table border="false">
   <tr><td><fun>get_mode</fun></td><td>Cell Context</td></tr>
   <tr><td><fun>set_mode</fun></td><td>Global Context</td></tr>
   <tr><td><fun>get_line_size</fun></td><td>Cell Context</td></tr>
   <tr><td><fun>set_line_size</fun></td><td>Global Context</td></tr>
   </table>
   </add>
*/
typedef enum {
        /* No instruction fetch sent to memory hierarchy */
        Instruction_Fetch_None = 0,

        /* Memory hierarchy gets fetch for each cache line access */
        Instruction_Fetch_Cache_Access_Trace = 1,

        /* Memory hierarchy gets fetch for each instruction fetch. Only
           x86/x86-64 */
        Instruction_Fetch_Trace = 2
} instruction_fetch_mode_t;

SIM_INTERFACE(instruction_fetch) {
        instruction_fetch_mode_t (*get_mode)(conf_object_t *obj);
        void (*set_mode)(conf_object_t *obj, instruction_fetch_mode_t mode);
        int (*get_line_size)(conf_object_t *obj);
        void (*set_line_size)(conf_object_t *obj, int size);
};
#define INSTRUCTION_FETCH_INTERFACE "instruction_fetch"
// ADD INTERFACE instruction_fetch_interface_t

#if defined(__cplusplus)
}
#endif

#endif
