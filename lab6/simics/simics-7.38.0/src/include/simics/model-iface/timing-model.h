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

#ifndef SIMICS_MODEL_IFACE_TIMING_MODEL_H
#define SIMICS_MODEL_IFACE_TIMING_MODEL_H

#include <simics/base/types.h>
#include <simics/base/memory.h>
#include <simics/base/memory-transaction.h>
#include <simics/base/time.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add id="snoop_memory_interface_t">
   This interface is described with the <iface>timing_model</iface>
   interface.
   </add>
   <add id="snoop_memory_interface_exec_context">
   Cell Context for all methods.
   </add>
*/

/* <add id="timing_model_interface_t">
   The <iface>timing_model</iface> interface is used to communicate
   stall times for memory accesses. It is typically exported by cache
   models. The <fun>operate()</fun> function is then called on every
   memory access that misses in the STC, and the return value from the
   call is the number of cycles to stall.

   The <iface>snoop_memory</iface> interface has the exact same layout
   as the <iface>timing_model</iface> interface, but its
   <fun>operate()</fun> function is called after the memory access has
   been performed. The return value from the <fun>operate()</fun>
   function of a <iface>snoop_memory</iface> interface is ignored.

   The <fun>operate</fun> function is invoked via the <attr>timing_model</attr>
   attribute of the <class>memory-space</class> where the STC miss happens.

   See the <cite>Model Builder User's Guide</cite> for more information on
   how to use these interfaces.

   <insert-until text="// ADD INTERFACE timing_model_interface"/>
   </add>
   <add id="timing_model_interface_exec_context">
   Cell Context for all methods.
   </add>
*/
SIM_INTERFACE(timing_model) {
        cycles_t (*operate)(conf_object_t *NOTNULL mem_hier,
                            conf_object_t *NOTNULL space,
                            map_list_t *map_list,
                            generic_transaction_t *NOTNULL mem_op);
};

#define TIMING_MODEL_INTERFACE "timing_model"

SIM_INTERFACE(snoop_memory) {
        cycles_t (*operate)(conf_object_t *NOTNULL mem_hier,
                            conf_object_t *NOTNULL space,
                            map_list_t *map_list,
                            generic_transaction_t *NOTNULL mem_op);
};

#define SNOOP_MEMORY_INTERFACE "snoop_memory"
        
// ADD INTERFACE timing_model_interface
// ADD INTERFACE snoop_memory_interface

typedef cycles_t (*operate_func_t)(conf_object_t *NOTNULL mem_hier,
                                   conf_object_t *NOTNULL space,
                                   map_list_t *NOTNULL map_list,
                                   generic_transaction_t *NOTNULL mem_op);

#if defined(__cplusplus)
}
#endif

#endif
