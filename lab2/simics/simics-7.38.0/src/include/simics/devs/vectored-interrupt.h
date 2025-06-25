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

#ifndef SIMICS_DEVS_VECTORED_INTERRUPT_H
#define SIMICS_DEVS_VECTORED_INTERRUPT_H

#include <simics/base/types.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add id="vectored_interrupt_interface_t">
   This interface is typically implemented by processors with interrupts
   that have vectors and priorities supplied by their sources.

   The <fun>set_level()</fun> method sets the interrupt level on the target
   object; zero indicates that no interrupt is requested. The interrupt source
   should implement the <iface>vectored_interrupt_source</iface> interface.

   <insert-until text="// ADD INTERFACE vectored_interrupt_interface"/>
   </add>
   <add id="vectored_interrupt_interface_exec_context">
    Cell Context for all methods.
   </add> */
SIM_INTERFACE(vectored_interrupt) {
        void (*set_level)(conf_object_t *NOTNULL obj,
                          conf_object_t *NOTNULL source, int level);
};
#define VECTORED_INTERRUPT_INTERFACE "vectored_interrupt"
// ADD INTERFACE vectored_interrupt_interface


/* <add id="vectored_interrupt_source_interface_t"> 

   This interface is implemented by interrupt sources that use the
   <iface>vectored_interrupt</iface> interface.

   The <fun>acknowledge()</fun> method is called to acknowledge a requested
   interrupt of the given level on the interrupt target. It should return
   an interrupt vector number, -1 if no vector is supplied, or -2 if
   the object does not accept the acknowledgement.

   <insert-until text="// ADD INTERFACE vectored_interrupt_source_interface"/>
   </add>
   <add id="vectored_interrupt_source_interface_exec_context">
    Cell Context for all methods.
   </add> */
SIM_INTERFACE(vectored_interrupt_source) {
        int (*acknowledge)(conf_object_t *NOTNULL obj,
                           conf_object_t *NOTNULL target, int level);
};
#define VECTORED_INTERRUPT_SOURCE_INTERFACE "vectored_interrupt_source"
// ADD INTERFACE vectored_interrupt_source_interface

#if defined(__cplusplus)
}
#endif

#endif
