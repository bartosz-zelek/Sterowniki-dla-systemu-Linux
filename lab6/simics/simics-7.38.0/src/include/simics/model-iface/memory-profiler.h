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

#ifndef SIMICS_MODEL_IFACE_MEMORY_PROFILER_H
#define SIMICS_MODEL_IFACE_MEMORY_PROFILER_H

#include <simics/base/types.h>
#include <simics/base/memory.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/*
   <add id="memory_profiler_interface_t">

   The <iface>memory_profiler</iface> interface is implemented by
   processors that support memory profiling. The <fun>get</fun>
   function returns the active profiler for the type of access
   specified in the <arg>access</arg> argument. NULL or None is
   returned if there is no active profiler for that type of access.

   The <fun>set</fun> function installs <arg>prof</arg> as a profiler
   for the accesses of type <arg>access</arg>. The <fun>set</fun>
   functions returns true if the setting was successful, and false
   otherwise.

   The <fun>get_granularity_log2</fun> gets the 2 logarithm of the
   profiling granularity in bytes, for example it returns 10 if the
   granularity for profiling is 1 KiB.

   <insert-until text="// ADD INTERFACE memory_profiler_interface_t"/>
   </add>
   <add id="memory_profiler_interface_exec_context">
   Cell Context for all methods.
   </add>
*/

SIM_INTERFACE(memory_profiler) {
        conf_object_t *(*get)(conf_object_t *obj, read_or_write_t access);
        bool (*set)(conf_object_t *obj, read_or_write_t access,
                    conf_object_t *prof);
        int (*get_granularity_log2)(conf_object_t *obj);
};

#define MEMORY_PROFILER_INTERFACE "memory_profiler"
// ADD INTERFACE memory_profiler_interface_t

#if defined(__cplusplus)
}
#endif

#endif
