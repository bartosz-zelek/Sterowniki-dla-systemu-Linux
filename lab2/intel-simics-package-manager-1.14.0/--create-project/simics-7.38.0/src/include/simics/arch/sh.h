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

#ifndef SIMICS_ARCH_SH_H
#define SIMICS_ARCH_SH_H

#include <simics/pywrap.h>
#include <simics/base/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* raise interrupt (used by the interrupt controller (INTC) to inform the
   CPU that an interrupt might be raised) */
SIM_INTERFACE(sh_interrupt) {
        void (*change_pending)(conf_object_t *cpu_obj, int level,
                               uint32 intevt, int mod_imask_flag);
};

#define SH_INTERRUPT_INTERFACE "sh_interrupt"

#if defined(__cplusplus)
}
#endif

#endif
