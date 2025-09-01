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

#ifndef SIMICS_MODEL_IFACE_SIMULATOR_CACHE_H
#define SIMICS_MODEL_IFACE_SIMULATOR_CACHE_H

#include <simics/base/types.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/*
  <add id="simulator_cache_interface_t">

  The <iface>simulator_cache</iface> interface is optionally
  implemented by processors. The interface can be used for
  <iface>translate</iface> objects to force a flush of the internal
  caches in a processor model, if a memory area that it previously
  allowed for caching is not valid anymore.

  <insert-until text="// ADD INTERFACE simulator_cache_interface_t"/>
  </add>

  <add id="simulator_cache_interface_exec_context">
  <table border="false">
  <tr><td><fun>flush</fun></td><td>Cell Context</td></tr>
  </table>

  </add>
*/

SIM_INTERFACE(simulator_cache) {
        void (*flush)(conf_object_t *obj);
};
#define SIMULATOR_CACHE_INTERFACE "simulator_cache"
// ADD INTERFACE simulator_cache_interface_t

#if defined(__cplusplus)
}
#endif

#endif
