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

#ifndef SIMICS_MODEL_IFACE_JIT_CONTROL_H
#define SIMICS_MODEL_IFACE_JIT_CONTROL_H

#include <simics/base/types.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* The jit-control interface is implemented by processors. Its purpose is to 
   enable and disable JIT for a processor.

   Function set_compile_enable() turns JIT ON/OFF for a processor according to
   the argument 'enabled'. */
SIM_INTERFACE(jit_control) {
        void (*set_compile_enable)(conf_object_t *obj, bool enabled);
};
#define JIT_CONTROL_INTERFACE "jit_control"

#if defined(__cplusplus)
}
#endif

#endif
