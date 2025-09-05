/*
  © 2017 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_MODEL_IFACE_TRANSACTION_H
#define SIMICS_MODEL_IFACE_TRANSACTION_H

#include <simics/base/types.h>
#include <simics/base/memory.h>
#include <simics/base/transaction.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/*
   <add id="transaction_interface_t">
   <ndx>transaction_t</ndx>

   The <iface>transaction</iface> interface is implemented by devices that
   can be mapped into address spaces. The <fun>issue</fun> method is called
   when a memory transaction <arg>t</arg> is issued to the object.

   The return value of the <fun>issue</fun> function is normally
   <tt>Sim_PE_No_Exception</tt>, but other pseudo <type>exception_type_t</type>
   values can be used to signal error conditions. The value
   <tt>Sim_PE_Deferred</tt> must be used when the transaction has been
   deferred using <fun>SIM_defer_transaction</fun> for completion at
   some later time.

   <insert-until text="// ADD INTERFACE transaction_interface_t"/>
   </add>

   <add id="transaction_interface_exec_context">
   Cell Context for all methods.
   </add>
*/
SIM_INTERFACE(transaction) {
        exception_type_t (*issue)(
                conf_object_t *NOTNULL obj,
                transaction_t *NOTNULL t,
                uint64 addr);
};
#define TRANSACTION_INTERFACE  "transaction"
// ADD INTERFACE transaction_interface_t

#if defined(__cplusplus)
}
#endif

#endif
