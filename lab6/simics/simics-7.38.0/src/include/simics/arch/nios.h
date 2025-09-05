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

#ifndef SIMICS_ARCH_NIOS_H
#define SIMICS_ARCH_NIOS_H

#include <simics/base/types.h>
#include <simics/base/memory-transaction.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* memory transaction specific to the Nios architecture */

/* <add id="devs api types">
   <name index="true">nios_memory_transaction_t</name>
   <doc>
   <doc-item name="NAME">nios_memory_transaction_t</doc-item>
   <doc-item name="SYNOPSIS">
   <smaller>
   <insert id="nios_memory_transaction_t def"/>
   </smaller>
   </doc-item>
   <doc-item name="DESCRIPTION">

   The <i>s</i> field contains generic information about memory operations (see
   <tt>generic_transaction_t</tt>).

   </doc-item>
   </doc>

   </add>
*/

/* <add-type id="nios_memory_transaction_t def"></add-type> */
typedef struct nios_memory_transaction {
        /* generic transaction */
        generic_transaction_t s;
} nios_memory_transaction_t;

/* <add id="nios_interface_t"> This interface is implemented by Intel&reg;
   Nios&reg; processors to provide various functionality that is specific for
   this class of processors.

   There are currently no Nios-specific functions.

   <insert-until text="// ADD INTERFACE nios_interface"/>
   </add>
   <add id="nios_interface_exec_context">Not applicable.</add>
*/

SIM_INTERFACE(nios) {
        int dummy;
};

#define NIOS_INTERFACE "nios"
// ADD INTERFACE nios_interface

/* <add id="nios_eic_interface_t">
   The <iface>nios_eic</iface> must be implemented by devices that acts as
   Nios&reg;
   External Interrupt Controllers. After the external IRQ signal in the CPU
   has been raised, the CPU will query the registered External Interrupt
   Controller for information about the current interrupt request. 
   When the CPU calls the <fun>handled</fun> function, it has finished 
   processing the request.

   The <fun>handler</fun> function should return the logical address of 
   the interrupt handler.

   The <fun>level</fun> should return the interrupt level.

   The <fun>reg_set</fun> should return the shadow register set number.

   The <fun>nmi</fun> should indicate if this is a non-maskable interrupt.
  
   See Nios&reg; II processor reference guide page 3-41 for further
   information.

   <insert-until text="// ADD INTERFACE nios_eic_interface"/>
   </add>

   <add id="nios_eic_interface_exec_context">
   Cell Context for all methods.
   </add>
*/
SIM_INTERFACE(nios_eic) {
        logical_address_t (*handler)(conf_object_t *NOTNULL obj);
        uint32 (*level)(conf_object_t *NOTNULL obj);
        uint32 (*reg_set)(conf_object_t *NOTNULL obj);
        bool (*nmi)(conf_object_t *NOTNULL obj);

        void (*handled)(conf_object_t *NOTNULL obj);
};

#define NIOS_EIC_INTERFACE "nios_eic"
// ADD INTERFACE nios_eic_interface

/* <add id="nios_cache_interface_t">

   This <iface>nios_cache</iface> interface is used when
   the side-effects are required for cache instructions.

   Each function is called when the corresponding instruction is executed. See
   Nios&reg; II processor reference guide, section 8, for further
   information.

   <insert-until text="// end of nios_cache interface"/>
   </add>

   <add id="nios_cache_interface_exec_context">
   Cell Context for all methods.
   </add>
*/
SIM_INTERFACE(nios_cache) {
        void (*flushd)(conf_object_t *NOTNULL self, logical_address_t addr);
        void (*flushda)(conf_object_t *NOTNULL self, logical_address_t addr);
        void (*flushi)(conf_object_t *NOTNULL self, logical_address_t addr);
        void (*flushp)(conf_object_t *NOTNULL self);
        void (*initd)(conf_object_t *NOTNULL self, logical_address_t addr);
        void (*initda)(conf_object_t *NOTNULL self, logical_address_t addr);
        void (*initi)(conf_object_t *NOTNULL self, logical_address_t addr);
        void (*sync)(conf_object_t *NOTNULL self);
};
#define NIOS_CACHE_INTERFACE "nios_cache"
// end of nios_cache interface

/* <add id="nios_custom_interface_t">

   This <iface>nios_custom</iface> interface is used when
   the custom instruction needs to be used.

   The <fun>custom</fun> function is called whenever the
   custom instruction is executed.

   See Nios&reg; II processor reference guide page 8-35 for further
   information about the parameters.

   <insert-until text="// end of nios_custom interface"/>
   </add>

   <add id="nios_custom_interface_exec_context">
   Cell Context
   </add>
*/
SIM_INTERFACE(nios_custom) {
        uint32 (*custom)(conf_object_t *NOTNULL self, uint32 n,
                         uint32 a, uint32 b, uint32 c, uint32 rA, uint32 rB,
                         bool readra, bool readrb, bool writerc);
};
#define NIOS_CUSTOM_INTERFACE "nios_custom"
// end of nios_custom interface

#if defined(__cplusplus)
}
#endif

#endif
