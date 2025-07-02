/*
  © 2024 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_MODEL_IFACE_TRANSACTION_INSTRUMENTATION_H
#define SIMICS_MODEL_IFACE_TRANSACTION_INSTRUMENTATION_H

#include <simics/base/conf-object.h>
#include <simics/base/transaction.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct transaction_callback_entry transaction_cb_handle_t;

/* <add id="next_map_t DOC">
   <ndx>next_map_t</ndx>
   <name index="true">next_map_t</name>
   <doc>
   <doc-item name="NAME">next_map_t</doc-item>
   <doc-item name="SYNOPSIS"><insert id="next_map_t"/>
   </doc-item>
   <doc-item name="DESCRIPTION">
   This type is used to hold information about the next map target in a
   translation chain. It is exposed in the <type>do_map_target_cb_t</type>
   function callback used in the <type>transaction_issue_cb_t</type> callback
   in the <iface>transaction_subscribe</iface> interface. See
   <type>transaction_issue_cb_t</type> for more information.

    </doc-item> </doc>
   </add>

   <add-type id="next_map_t"></add-type> */
typedef struct {
        const map_target_t *mt;
#ifndef PYWRAP
        exception_type_t ex;
#else
        unsigned ex;
#endif
        uint64 addr;
} next_map_t;

/* <add id="do_map_target_cb_t DOC">
   <ndx>do_map_target_cb_t</ndx>
   <name index="true">do_map_target_cb_t</name>
   <doc>
   <doc-item name="NAME">do_map_target_cb_t</doc-item>
   <doc-item name="SYNOPSIS"><insert id="do_map_target_cb_t"/>
   </doc-item>
   <doc-item name="DESCRIPTION">
   </doc-item> </doc>
   </add>

   <add-type id="do_map_target_cb_t"></add-type> */
typedef next_map_t (*do_map_target_cb_t)(const map_target_t *mt,
                                         transaction_t *org_t,
                                         uint64 addr, map_info_t *map,
                                         lang_void *handle);

/* <add id="transaction_issue_cb_t DOC">
   <ndx>transaction_issue_cb_t</ndx>
   <name index="true">transaction_issue_cb_t</name>
   <doc>
   <doc-item name="NAME">transaction_issue_cb_t</doc-item>
   <doc-item name="SYNOPSIS"><insert id="transaction_issue_cb_t"/>
   </doc-item>
   <doc-item name="DESCRIPTION">

   Note: This callback is internal and may change without notice.

   Instrumentation callback function registered through the
   <fun>register_issue_cb</fun>
   method in the <iface>transaction_subscribe</iface> interface. In this
   callback a user can snoop and/or replace a transaction passing through a
   map target object.

   Argument <param>obj</param> is the object that registered the callback.  The
   <param>mt</param> argument is the map target used to map the transaction,
   and the <param>map_target_type</param> is a string describing the map
   target.  The <param>map</param> parameter is map information for the map
   target.  <param>addr</param> is the address in the mapped object where the
   transaction hits. You can retrieve the object by calling
   <fun>SIM_map_target_object</fun> on the map_target.  The <param>t</param>
   argument is a pointer to the transaction that is issued. See the access
   primitives of the <type>transaction_t</type> type to see how to examine and
   modify the transaction. The <param>do_mt</param> is the function to call
   that actually do the map target access, and it is the callback's
   responsibility to call it. This allows the map target, the map, the address,
   and the transaction to be modified by then callback. Se the documentation of
   the <type>do_mt</type> for a description of how to call it.

   The callback should return a <type>next_map_t</type> value that is the
   return value of the do_mt callback. This describes the next map target to be
   considered, if the map target translates to a new map target. In this case,
   the next target is stored in the <var>mt</var> field. If no new map target is
   used, the field will be NULL. The type also contains an <var>ex</var> field
   set to something else than Sim_PE_No_Exception, if an exception occurred. It
   also has a <var>addr</var> field which describes the new address for the next
   map target.

   If the do_mt function is not called, this effectively cancels the
   transaction.  The value map_next_t to return should then fill in the mt
   field with a NULL pointer.

   </doc-item> </doc>
   </add>
        
   <add-type id="transaction_issue_cb_t"></add-type> */

typedef next_map_t (*transaction_issue_cb_t)(conf_object_t *obj,
                                             const map_target_t *mt,
                                             const char *map_target_type,
                                             map_info_t *map,
                                             transaction_t *t,
                                             uint64 offset,
                                             do_map_target_cb_t do_mt,
                                             lang_void *handle,
                                             lang_void *user_data);

/* <add id="transaction_subscribe_interface_t">
   <ndx>transaction_subscribe_interface_t</ndx>

   Note: this interface is internal and may change without notice.

   This interface is used to register callbacks to instrument transactions
   issued through a map target by the SIM_issue_transaction API call. This
   includes all <class>memory_space</class> objects handling transactions.
   The interface is only implemented by the <obj>sim.transactions</obj>
   object which serves all transactions.

   The <fun>register_issue_cb</fun> method registers a callback that is
   called whan a transaction is passed to a map target. This makes it
   possible to modify or replace the transaction before it reaches its
   destination. See the documentation of the
   <type>transaction_issue_cb_t</type> type for more information on now to
   handle the transaction.

   A <type>transaction_cb_handle_t</type> pointer is returned as a reference to
   the callback.

   The registered callbacks above will receive all types of accesses,
   read, write, or execute, from any initiator hitting any address range. It is
   up to the callback to filter the information if needed, e.g., to only trace
   read accesses.

   If more than one cell is used and multithreading is enabled, more that one
   callback can be issued at the same time.

   Normally, for transactions to ram/rom, these accesses can be bypassed by
   caching the destination object by using the
   <iface>direct_memory_lookup</iface> interface. Then these accesses cannot be
   monitored by the <iface>transaction_subscribe</iface> interface. However, it
   is possible to block this caching by using the
   <fun>register_access_filter_cb</fun> method of the
   <iface>ram_access_subscribe</iface> interface. This has typically severe
   impact on simulation speed, but allows user to monitor all transactions in
   the system. Note however that a real systems also reduces the transactions to
   memory by using caches, which normally is not modeled by
   Simics,

   The <fun>remove_callback</fun> method removes an earlier installed
   callback. The handle is used to identify the callback to be removed. The
   register functions above return such handle.

   The <fun>enable_callback</fun> and <fun>disable_callback</fun> methods
   temporarily enables and disables a previously installed callback. Note that
   this will not necessary speed up the simulation, since the caching may be
   blocked anyway. The handle is used to identify the callback. The register
   functions above return such handle.

   <insert-until text="// ADD INTERFACE transaction_subscribe_interface"/>
   </add>
   <add id="transaction_subscribe_interface_exec_context">
   Outside execution context for all methods.
   </add>
*/
SIM_INTERFACE(transaction_subscribe) {
        void (*remove_callback)(conf_object_t *NOTNULL obj,
                                transaction_cb_handle_t *handle);
        void (*enable_callback)(conf_object_t *NOTNULL obj,
                                transaction_cb_handle_t *handle);
        void (*disable_callback)(conf_object_t *NOTNULL obj,
                                 transaction_cb_handle_t *handle);
        transaction_cb_handle_t *(*register_issue_cb)(
                conf_object_t *NOTNULL obj,
                conf_object_t *conn_obj,
                transaction_issue_cb_t cb,
                lang_void *data);
};

#define TRANSACTION_SUBSCRIBE_INTERFACE "transaction_subscribe"
// ADD INTERFACE transaction_subscribe_interface
        
#if defined(__cplusplus)
}
#endif

#endif
