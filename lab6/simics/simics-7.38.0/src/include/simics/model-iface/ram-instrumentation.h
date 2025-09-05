/*
  © 2016 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_MODEL_IFACE_RAM_INSTRUMENTATION_H
#define SIMICS_MODEL_IFACE_RAM_INSTRUMENTATION_H

#include <simics/devs/ram.h>
#include <simics/processor-api.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct ram_callback_entry ram_cb_handle_t;

/* <add id="access_filter_cb_t DOC">
   <ndx>access_filter_cb_t</ndx>
   <name index="true">access_filter_cb_t</name>
   <doc> 
   <doc-item name="NAME">access_filter_cb_t</doc-item>
   <doc-item name="SYNOPSIS"><insert id="access_filter_cb_t"/>
   </doc-item>
   <doc-item name="DESCRIPTION"> 

   Instrumentation callback function registered through the
   <fun>register_access_filter_cb</fun> method of the
   <iface>ram_access_subscribe_interface_t</iface> interface to get a callback
   when a direct page is requested which is typically done by CPU models. The
   <param>obj</param> is the object that registered the callback, and the
   <param>initiator</param> is the object which tries to get the direct
   page. The target object of the direct page lookup is passed in the
   <param>ram</param> argument. Typically, it is an object of the
   <class>ram</class> class. The <param>offset</param>, <param>size</param> and
   <param>access</param> are the ram offset, the size of the cached page, and
   the access type, respectively. The <param>user_data</param> is the user data
   for the callback. To allow caching of a page, the corresponding bits in an
   <type>access_t</type> should be returned. If 0 is returned the page
   will not be cached.

   </doc-item>
   </doc>
   </add>
        
<add-type id="access_filter_cb_t"></add-type> */
typedef access_t (*access_filter_cb_t)(conf_object_t *obj,
                                      conf_object_t *ram,
                                      conf_object_t *initiator,
                                      uint64 offset,
                                      unsigned size,
                                      access_t access,
                                      lang_void *user_data);

/* <add id="ram_access_cb_t DOC">
   <ndx>ram_access_cb_t</ndx>
   <name index="true">ram_access_cb_t</name>
   <doc>
   <doc-item name="NAME">ram_access_cb_t</doc-item>
   <doc-item name="SYNOPSIS"><insert id="ram_access_cb_t"/>
   </doc-item>
   <doc-item name="DESCRIPTION">

   Instrumentation callback function registered through the
   <fun>register_access_before_cb</fun> or <fun>register_access_after_cb</fun>
   methods in the <iface>ram_access_subscribe_interface_t</iface> interface to
   get a callback when a ram/rom object is accessed. <param>obj</param> is the
   object that registered the callback, and the <param>ram</param> is the
   ram/rom object that is accessed. The <param>ram_offset</param> is the offset
   in the ram/rom object where the transaction hits. The <param>t</param>
   argument is a pointer to the <type>transaction_t</type> that is
   performed. See the access primitives of the <type>transaction_t</type> type
   to see how to examine and modify the transaction. Depending on if the
   register function is of before or after type, the transaction will be, or
   already has been completed, at the time of this callback. This places
   restrictions on what can be done with the transaction.
   </doc-item>
   </doc>
   </add>
        
<add-type id="ram_access_cb_t"></add-type> */
typedef void (*ram_access_cb_t)(conf_object_t *obj,
                                conf_object_t *ram,
                                uint64 ram_offset,
                                transaction_t *t,
                                lang_void *user_data);
        
/* <add id="ram_access_subscribe_interface_t">
   <ndx>ram_access_subscribe_interface_t</ndx>

   This interface is used to register callbacks to instrument ram/rom accesses.

   The <fun>register_access_before_cb</fun> method registers a callback that is
   called before any memory access reached the backing storage in a ram/rom
   image. This makes it possible to modify the transaction before it reaches
   its destination. See the documentation of the <type>ram_access_cb_t</type>
   type for more information. A <type>ram_cb_handle_t</type> pointer is
   returned as a reference to the callback.

   The <fun>register_access_after_cb</fun> method registers a callback that is
   called after any memory access has reached the backing storage in a ram/rom
   image. This makes it possible to modify the transaction after the access is
   completed. See the documentation of the <type>ram_access_cb_t</type>
   type for more information. A <type>ram_cb_handle_t</type> pointer is
   returned as a reference to the callback.

   Both of these register callbacks above will receive all types of accesses,
   read, write, or execute, from any initiator hitting any address range. It is
   up to the callback to filter the information if needed, e.g., to only trace
   read accesses. Normally, ram/rom pages can be cached in object using them by
   using the <iface>direct_memory_lookup</iface> interface. This caching must
   be blocked by this interface to allow the callbacks to be called. This has
   severe impact on simulation speed. However, the following method should be
   used to allow caching for accesses that the callbacks have no interest in.

   The <fun>register_access_filter_cb</fun> method can be used to register a
   function callback that allows ram/rom pages to be cached by a user of the
   <iface>direct_memory_lookup</iface> interface. If caching is allowed the
   access may be invisible to the callbacks installed by
   <fun>register_access_before_cb</fun> and <fun>register_access_after_cb</fun>
   methods above. Even if an access is allowed to be cached it does not mean
   that it will be, which means that the callbacks can be called anyway.

   See the documentation of the <type>access_filter_cb_t</type> type for more
   information about the callback and how to allow caching. A
   <type>ram_cb_handle_t</type> pointer is returned as a reference to the
   callback.

   The <fun>remove_callback</fun> method removes an earlier installed
   callback. The handle is used to identify the callback to be removed. All
   register function above returns such handle.

   The <fun>enable_callback</fun> and <fun>disable_callback</fun> methods
   temporarily enables and disables a previously installed
   callback. The handle is used to identify the callback. All
   register function above returns such handle.

   <insert-until text="// ADD INTERFACE ram_access_subscribe_interface"/>
   </add>
   <add id="ram_access_subscribe_interface_exec_context">
   Global Context for all methods.
   </add>
*/
SIM_INTERFACE(ram_access_subscribe) {
        void (*remove_callback)(conf_object_t *NOTNULL obj,
                                ram_cb_handle_t *handle);
        void (*enable_callback)(conf_object_t *NOTNULL obj,
                                ram_cb_handle_t *handle);
        void (*disable_callback)(conf_object_t *NOTNULL obj,
                                 ram_cb_handle_t *handle);
        ram_cb_handle_t *(*register_access_before_cb)(
                conf_object_t *NOTNULL obj,
                conf_object_t *conn_obj,
                ram_access_cb_t cb,
                lang_void *data);
        ram_cb_handle_t *(*register_access_after_cb)(
                conf_object_t *NOTNULL obj,
                conf_object_t *conn_obj,
                ram_access_cb_t cb,
                lang_void *data);
        ram_cb_handle_t *(*register_access_filter_cb)(
                conf_object_t *NOTNULL obj,
                conf_object_t *connection,
                access_filter_cb_t cb,
                lang_void *data);
};

#define RAM_ACCESS_SUBSCRIBE_INTERFACE "ram_access_subscribe"
// ADD INTERFACE ram_access_subscribe_interface
        
#if defined(__cplusplus)
}
#endif

#endif
