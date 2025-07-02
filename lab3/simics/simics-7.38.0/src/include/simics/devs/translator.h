/*
  © 2014 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_DEVS_TRANSLATOR_H
#define SIMICS_DEVS_TRANSLATOR_H

#include <simics/base/cbdata.h>
#include <simics/base/types.h>
#include <simics/base/map-target.h>
#include <simics/base/transaction.h>
#include <simics/processor/types.h>
#include <simics/base/transaction.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add id="translator_interface_t">
   The <iface>translator</iface> interface is implemented by
   objects which perform address translations or map
   memory transactions to devices or address spaces.

   The <fun>translate</fun> method takes a physical address
   <param>addr</param> and returns a value of the type
   <type>translation_t</type> which describes the translation for
   an address range which must include <param>addr</param>. Please
   refer to the <type>translation_t</type> documentation for a complete
   description of the return value and more information regarding
   the implementation of translator objects.

   A translator can translate read transactions, write transactions
   and instruction fetches differently. The <param>access</param> parameter
   is set to all requested access types in the form of a bitmask.
   The translator should return null in the <param>target</param> field
   if a translation valid for all requested accesses cannot be performed;
   when this happens, the requestor is expected to repeat the interface call
   with just a single bit set in the access mask,
   e.g. <const>Sim_Access_Read</const>.

   The <param>default_target</param> parameter is a default target,
   which can be NULL, of the translator mapping (i.e.,
   the <i>target</i> field of the respective <class>memory-space</class>'s
   map entry specifying mapping).

   <insert-until text="// ADD INTERFACE translator"/>
   </add>

   <doc-item name="SEE ALSO">
     <type>translation_t</type>,
     <fun>SIM_map_target_flush</fun>
   </doc-item>

   <add id="translator_interface_exec_context">
   Cell Context for all methods.
   </add>
 */
SIM_INTERFACE(translator) {
        translation_t (*translate)(conf_object_t *NOTNULL obj,
                                   physical_address_t addr,
                                   access_t access,
                                   const map_target_t *default_target);
};

#define TRANSLATOR_INTERFACE "translator"
// ADD INTERFACE translator


/* <add id="transaction_translator_interface_t">
   The <iface>transaction_translator</iface> interface is implemented by
   objects which would like to inspect and/or modify transactions
   before forwarding them to devices or address spaces. In Simics documentation,
   such objects are usually called translator objects or, simply, translators.
   In comparison with the simpler <iface>translator</iface> interface,
   the <iface>transaction_translator</iface> interface provides
   an opportunity to inspect/modify transactions. We recommend
   using the <iface>translator</iface> interface when the additional
   functionality is not needed.

   The interface has one method called <fun>translate</fun>. The arguments
   passed to this method describe the access and thus allow
   a translator to perform a proper translation. Here is a short description of
   the <fun>translate</fun> method's parameters:<br/>
   - <param>addr</param> is an address of the access;<br/>
   - <param>access</param> is a bitmask specifying all requested
   access types;<br/>
   - <param>t</param> is a transaction;<br/>
   - <param>callback</param> is a callback that should be called by
   the object implementing the interface;<br/>
   - <param>data</param> should be passed to <param>callback</param>
   as it is when the callback is invoked by the translator object.<br/>
   All these arguments are described in detail below.

   The transactions done by the translators may be cached by Simics.
   To ensure efficient flushing of Simics internal caches we recommend
   that the objects implementing the <iface>transaction_translator</iface>
   interface implement also the <iface>translation_flush</iface> interface.
   The absence of the <iface>translation_flush</iface> interface may lead to
   a substantial decrease of simulation speed.

   Whenever a previously returned translation becomes invalid the translator
   objects are expected to use either <fun>SIM_map_target_flush</fun>
   (preferably) or <fun>SIM_translation_changed</fun> function to ensure that
   Simics will flush any cached translations. Please refer to the documentation
   of these functions for the information about how they should be used.

   An object implementing the <iface>transaction_translator</iface> interface
   should invoke <param>callback</param> passing to it tree arguments:
   <param>translation</param>, <param>transaction</param>
   and <param>data</param>. The <param>translation</param> argument
   describes the translation that was done, i.e. the transaction destination.
   Please refer to the <type>translation_t</type> documentation
   for the complete description.
   The <param>transaction</param> argument is either the original
   transaction passed to the <iface>transaction_translator</iface> interface
   or, if a translator object wants to append atoms to the <param>t</param>
   transaction, a new transaction created via transaction chaining.
   The transaction chaining is described in detail in the "Transaction Chaining"
   chapter of the <cite>Model Builder's User Guide</cite>. Finally, the
   <param>data</param> argument is a parameter which should be passed as it is
   when <param>callback</param> is invoked by the translator object.
   Simics will report an error if a translator object doesn't invoke
   <param>callback</param> from the <fun>translate</fun> method
   of the <iface>transaction_translator</iface>.

   If it is needed, a translator may route transactions based on the value of
   certain <param>transaction</param>'s atoms. The <type>initiator</type> atom
   is a good example of such an atom. Please note that not all atoms are suited
   for routing transactions based on their value. This should be checked
   in the documentation for the specific atom.

   The <param>callback</param> function returns transaction completion status.
   This status should be usually returned as it is by the <fun>translate</fun>
   method. The status value is also needed in the case when a translator object
   appends atoms via transaction chaining with a non-zero
   <type>completion</type> atom. In that case the returned status should be used
   when the <fun>SIM_monitor_chained_transaction</fun> function is called.

   Please note that memory accesses in Simics are not always issued with
   the transactions of the <type>transaction_t</type> type.
   For example, memory access can be done with the use of
   the legacy <type>generic_transaction_t</type> type.
   One more such example is lookup accesses which are done
   via the <iface>direct_memory_lookup</iface> interface. All such accesses
   are converted, if it is needed, and presented to the translator object via
   the <param>t</param> parameter.

   The <param>access</param> parameter specifies access
   types in the form of a bitmask. Its value may be of interest
   if a translation is done differently based on whether it is
   a read, write or instruction fetch transaction. It can be noted
   that a transaction itself already contains information
   whether it is a read, write or an instruction fetch.
   Usually, the <param>access</param> parameter just reflects that value.
   However, for lookup accesses done via
   the <iface>direct_memory_lookup</iface> interface
   the <param>access</param> argument may have more bits set. If a translation
   which are valid for all requested accesses cannot be performed, then
   a null value should be set in the <param>target</param> of
   the <param>translation</param>
   argument passed to <param>callback</param>. In this case, the requestor is
   expected to repeat the interface call with just a single bit set
   in the access mask, e.g. <const>Sim_Access_Read</const>.

   <insert-until text="// ADD INTERFACE transaction_translator"/>
   </add>

   <doc-item name="SEE ALSO">
     <type>translation_t</type>,
     <iface>translation_flush_interface_t</iface>,
     <fun>SIM_map_target_flush</fun>
   </doc-item>

   <add id="transaction_translator_interface_exec_context">
   Cell Context for all methods.
   </add>
*/
SIM_INTERFACE(transaction_translator) {
        exception_type_t (*translate)(
                conf_object_t *NOTNULL obj,
                uint64 addr,
                access_t access,
                transaction_t *t,
                exception_type_t (*NOTNULL callback)(
                        translation_t translation,
                        transaction_t *transaction,
                        cbdata_call_t data),
                cbdata_register_t data);
};

#define TRANSACTION_TRANSLATOR_INTERFACE "transaction_translator"
// ADD INTERFACE transaction_translator


/* <add id="translation_flush_interface_t">

   The <iface>translation_flush</iface> interface is an optional interface
   which can be implemented by objects implementing the
   <iface>transaction_translator</iface> or <iface>translator</iface> interface.
   The interface is used
   by Simics an optimization when Simics flushes its simulation caches.
   Simics caches translations returned by translators but
   quite often this cache needs to be invalidated, usually, due to
   the changes in the memory map of the target system. In such cases
   Simics may use this interface (if it is available) to do a fine-grain
   invalidation of its simulation caches.

   The <iface>translation_flush</iface> interface has one method
   - <fun>flush_range</fun> - which will be called whenever there is
   a need to flush simulation caches. The <fun>flush_range</fun> method
   has the following parameters:<br/>
   - <param>base</param> is the start of the region to flush;<br/>
   - <param>size</param> is the size of the region to flush;<br/>
   - <param>access</param> is a bitmask which specifies targets
   for what access types should be flushed;<br/>
   - <param>default_target</param> is only used for translators implementing
   the <iface>translator</iface> interface and has the same as value as
   the <param>default_target</param> parameter of
   the <fun>translate</fun> method of the <iface>translator</iface> interface.
   Please see the documentation of the <iface>translator</iface> for more
   information about the parameter.

   In the <fun>flush_range</fun> method, the translator object is expected
   to flush all previously returned destinations of the translation requests
   that the translator did for
   the [<param>base</param>, <param>base</param>+<param>size</param>) range.
   The flushing is done by calling the <fun>SIM_map_target_flush</fun> function
   for the destination map target(s). If no translation requests were processed
   to the range since the last invocation of the <fun>flush_range</fun> method
   then no flushing is needed, and the <fun>flush_range</fun> may immediately
   return the <const>true</const> value.

   Please note that there is no need to call
   the <fun>SIM_map_target_flush</fun> function for the translations which
   were tagged with the <const>Sim_Translation_Dynamic</const> flag. Either,
   no invalidation is needed for the parts of the range where nothing is mapped.

   The return value is used to report whether the invalidation request
   completed successfully, i.e. whether all calls to
   the <fun>SIM_map_target_flush</fun> function succeeded (i.e. returned
   the <const>true</const> value). If a call to
   the <fun>SIM_map_target_flush</fun> fails (i.e.
   the <const>false</const> value is returned) then
   the <fun>flush_range</fun> function is expected to
   return immediately with the <const>false</const> return value. If, for
   some reason, the translator cannot invalidate all possible destinations,
   then it can just immediately return the <const>false</const> value.

   If <const>false</const> is returned, then all translations in the simulator
   will typically be flushed, which could be an expensive operation.

   <doc-item name="EXAMPLE">
   Here is pseudo code providing a sample implementation of
   the flush_range method:
   <pre>
   bool flush_range(obj, base, size, access):
       for (map_target, start, size) in destinations(obj, base, size, access):
           if not SIM_map_target_flush(map_target, start, size, access):
               return False
       return True
   </pre>
   </doc-item>

   <insert-until text="// ADD INTERFACE translation_flush_interface"/>
   </add>

   <add id="translation_flush_interface_exec_context">
   Cell Context for all methods.
   </add>
*/

SIM_INTERFACE(translation_flush) {
        bool (*flush_range)(
                conf_object_t *obj,
                uint64 base,
                uint64 size,
                access_t access,
                const map_target_t *default_target);
};

#define TRANSLATION_FLUSH_INTERFACE "translation_flush"
// ADD INTERFACE translation_flush_interface

#if defined(__cplusplus)
}
#endif

#endif
