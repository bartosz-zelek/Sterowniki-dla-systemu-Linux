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

#ifndef SIMICS_DEVS_COREINT_H
#define SIMICS_DEVS_COREINT_H

/* <add id="coreint_interface_t">
   Interface between CoreInt capable processor and interrupt controller.

   This interface is implemented by CoreInt capable interrupt controllers and
   allows the processor to automatically acknowledge external interrupts
   without software intervention.

   The <fun>acknowledge</fun> function is called by the processor when an
   external interrupt is taken. If coreint is enabled in the interrupt
   controller, the interrupt controller should lower the interrupt signal
   towards the processor and return the interrupt source
   vector. This way the software doesn't have to go and query the
   interrupt controller for the source. If coreint is not enabled, the
   interrupt should not do anything and the vector value in the reply is
   undefined.

   <insert-until text="// ADD INTERFACE coreint_interface"/>

   </add>
   <add id="coreint_interface_exec_context">
   Cell Context for all methods.
   </add>
*/
#include <simics/pywrap.h>
#include <simics/base/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct {
        bool enabled;
        uint64 vector;
} coreint_reply_t;

#define COREINT_INTERFACE "coreint"
SIM_INTERFACE(coreint) {
        coreint_reply_t (*acknowledge)(conf_object_t *obj, conf_object_t *core);
};
// ADD INTERFACE coreint_interface

#if defined(__cplusplus)
}
#endif

#endif
