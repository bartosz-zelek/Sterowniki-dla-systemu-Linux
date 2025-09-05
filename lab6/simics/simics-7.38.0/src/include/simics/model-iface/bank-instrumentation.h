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

#ifndef SIMICS_MODEL_IFACE_BANK_INSTRUMENTATION_H
#define SIMICS_MODEL_IFACE_BANK_INSTRUMENTATION_H

#include <simics/base/conf-object.h>
#include <simics/base/memory-transaction.h>
#include <simics/model-iface/instrumentation-provider.h>
#include <simics/pywrap.h>

#ifdef __cplusplus
extern "C" {
#endif

/* <add-type id="bank_access_t">

   Handle used to monitor and modify the state of register accesses using the
   <iface>bank_before_read</iface>, <iface>bank_after_read</iface>,
   <iface>bank_before_write</iface>, and <iface>bank_after_write</iface>
   interfaces.
   </add-type>
*/
typedef struct bank_access bank_access_t;
/* <add-type id="bank_callback_handle_t">

   Unique handle associated with a callback registered through the
   <iface>bank_instrumentation_subscribe</iface> interface.
   </add-type>
*/
typedef uintptr_t bank_callback_handle_t;

/* <add id="bank_before_read_interface_t">

   The <iface>bank_before_read</iface> interface is used to monitor and modify
   the state of read accesses from the <fun>before_read_callback_t</fun>
   callback.

   <insert-until text="// ADD INTERFACE bank_before_read"/>

   <fun>offset</fun> retrieves the address of the access.

   <fun>size</fun> gets the size of the access.

   <fun>set_offset</fun> can be used to redirect the access to another address.

   <fun>inquire</fun> can turn the access into an inquiry
   access. Subsequent and corresponding after_read callbacks are
   invoked regardless.

   <fun>initiator</fun> returns the initiator of the access. This method may be
   <tt>NULL</tt>, although this is deprecated. If the interface was implemented
   by a DML/C/C++ bank, then the method may only be accessed if the bank was
   compiled with Simics Base 6.0.129 or newer.

   </add>
   <add id="bank_before_read_interface_exec_context">
   Cell Context for all methods, but must be called from a
   <fun>before_read_callback_t</fun> callback with a valid handle of type
   <type>bank_access_t</type>.
   </add>
*/
SIM_INTERFACE(bank_before_read) {
        physical_address_t (*offset)(bank_access_t *handle);
        physical_address_t (*size)(bank_access_t *handle);

        void (*set_offset)(bank_access_t *handle, physical_address_t offset);
        void (*inquire)(bank_access_t *handle);
        conf_object_t *(*initiator)(bank_access_t *handle);
};
#define BANK_BEFORE_READ_INTERFACE "bank_before_read"
// ADD INTERFACE bank_before_read

/* <add id="bank_after_read_interface_t">

   The <iface>bank_after_read</iface> interface is used to monitor and modify
   the state of read accesses from the <fun>after_read_callback_t</fun>
   callback.

   <insert-until text="// ADD INTERFACE bank_after_read"/>

   <fun>offset</fun> retrieves the address of the access.

   <fun>size</fun> gets the size of the access.

   <fun>missed</fun> tells you whether or not the access succeeded.

   <fun>value</fun> provides the read value. May not be invoked if
   <fun>missed</fun> is true.

   <fun>set_missed</fun> can be used to inject or forgive an access miss.

   <fun>set_value</fun> can change the read value.

   <fun>initiator</fun> returns the initiator of the access. This method may be
   <tt>NULL</tt>, although this is deprecated. If the interface was implemented
   by a DML/C/C++ bank, then the method may only be accessed if the bank was
   compiled with Simics Base 6.0.129 or newer.

   </add>
   <add id="bank_after_read_interface_exec_context">
   Cell Context for all methods, but must be called from an
   <fun>after_read_callback_t</fun> callback with a valid handle of type
   <type>bank_access_t</type>.
   </add>
*/
SIM_INTERFACE(bank_after_read) {
        physical_address_t (*offset)(bank_access_t *handle);
        physical_address_t (*size)(bank_access_t *handle);
        bool (*missed)(bank_access_t *handle);
        uint64 (*value)(bank_access_t *handle);

        void (*set_missed)(bank_access_t *handle, bool missed);
        void (*set_value)(bank_access_t *handle, uint64 value);
        conf_object_t *(*initiator)(bank_access_t *handle);
};
#define BANK_AFTER_READ_INTERFACE "bank_after_read"
// ADD INTERFACE bank_after_read

/* <add id="bank_before_write_interface_t">

   The <iface>bank_before_write</iface> interface is used to monitor and modify
   the state of write accesses from the <fun>before_write_callback_t</fun>
   callback.

   <insert-until text="// ADD INTERFACE bank_before_write"/>

   <fun>offset</fun> retrieves the address of the access.

   <fun>size</fun> gets the size of the access.

   <fun>value</fun> provides the write value.

   <fun>suppress</fun> may be used to prevent the write. Subsequent
   and corresponding after_write callbacks are invoked regardless.

   <fun>set_offset</fun> can be used to redirect the write to an another
   address.

   <fun>set_value</fun> can change the write value.

   <fun>initiator</fun> returns the initiator of the access. This method may be
   <tt>NULL</tt>, although this is deprecated. If the interface was implemented
   by a DML/C/C++ bank, then the method may only be accessed if the bank was
   compiled with Simics Base 6.0.129 or newer.

   </add>
   <add id="bank_before_write_interface_exec_context">
   Cell Context for all methods, but must be called from a
   <fun>before_write_callback_t</fun> callback with a valid handle of type
   <type>bank_access_t</type>.
   </add>
*/
SIM_INTERFACE(bank_before_write) {
        physical_address_t (*offset)(bank_access_t *handle);
        physical_address_t (*size)(bank_access_t *handle);
        uint64 (*value)(bank_access_t *handle);

        void (*suppress)(bank_access_t *handle);
        void (*set_offset)(bank_access_t *handle, physical_address_t offset);
        void (*set_value)(bank_access_t *handle, uint64 value);
        conf_object_t *(*initiator)(bank_access_t *handle);
};
#define BANK_BEFORE_WRITE_INTERFACE "bank_before_write"
// ADD INTERFACE bank_before_write

/* <add id="bank_after_write_interface_t">

   The <iface>bank_after_write</iface> interface is used to monitor and modify
   the state of write accesses from the <fun>after_write_callback_t</fun>
   callback.

   <insert-until text="// ADD INTERFACE bank_after_write"/>

   <fun>offset</fun> retrieves the address of the access.

   <fun>size</fun> gets the size of the access.

   <fun>missed</fun> tells you whether or not the access succeeded.

   <fun>set_missed</fun> can be used to inject or forgive an access miss.

   <fun>initiator</fun> returns the initiator of the access. This method may be
   <tt>NULL</tt>, although this is deprecated. If the interface was implemented
   by a DML/C/C++ bank, then the method may only be accessed if the bank was
   compiled with Simics Base 6.0.129 or newer.

   </add>
   <add id="bank_after_write_interface_exec_context">
   Cell Context for all methods, but must be called from an
   <fun>after_write_callback_t</fun> callback with a valid handle of type
   <type>bank_access_t</type>.
   </add>
*/
SIM_INTERFACE(bank_after_write) {
        physical_address_t (*offset)(bank_access_t *handle);
        physical_address_t (*size)(bank_access_t *handle);
        bool (*missed)(bank_access_t *handle);

        void (*set_missed)(bank_access_t *handle, bool missed);
        conf_object_t *(*initiator)(bank_access_t *handle);
};
#define BANK_AFTER_WRITE_INTERFACE "bank_after_write"
// ADD INTERFACE bank_after_write

/* <add-type id="before_read_callback_t"></add-type> */
typedef void (*before_read_callback_t)(conf_object_t *connection,
                                       bank_before_read_interface_t *access,
                                       bank_access_t *handle,
                                       lang_void *user_data);

/* <add-type id="after_read_callback_t"></add-type> */
typedef void (*after_read_callback_t)(conf_object_t *connection,
                                      bank_after_read_interface_t *access,
                                      bank_access_t *handle,
                                      lang_void *user_data);

/* <add-type id="before_write_callback_t"></add-type> */
typedef void (*before_write_callback_t)(conf_object_t *connection,
                                        bank_before_write_interface_t *access,
                                        bank_access_t *handle,
                                        lang_void *user_data);

/* <add-type id="after_write_callback_t"></add-type> */
typedef void (*after_write_callback_t)(conf_object_t *connection,
                                       bank_after_write_interface_t *access,
                                       bank_access_t *handle,
                                       lang_void *user_data);

/* <add id="bank_instrumentation_subscribe_interface_t">

   The <iface>bank_instrumentation_subscribe</iface> interface is implemented
   by non-anonymous register banks. The interface may be used to monitor and
   modify register accesses using callbacks.

   Similar to the CPU instrumentation framework, a bank that implements the
   interface is considered an instrumentation provider and is typically used by
   an instrumentation tool. The tool registers callbacks using this interface
   and performs its actions once they are called. Using this interface, tools
   may also group registered callbacks using connection objects. This can be
   useful to enforce a certain evaluation order of grouped callbacks.

   The <arg>bank</arg> argument in all methods is the register bank object
   implementing this interface.

   The <arg>connection</arg> can be used to group registered callbacks
   together, so that their order may be changed or their registered
   callbacks be enabled, disabled, or removed collectively. If
   <arg>connection</arg> is NULL when registering a callback, the callback is
   considered anonymous. Anonymous callbacks are evaluated before any other
   callbacks in the order of creation. See <iface>instrumentation_order</iface>
   interface for more details on the callback order.

   Each registration method installs a callback which is called at a
   particular time, before or after, read and write register
   accesses. Callbacks are not invoked during inquiry accesses. Using
   the <arg>offset</arg> and <arg>size</arg> arguments, a user may
   install the callback only for a particular range. If
   <arg>offset</arg> and <arg>size</arg> are 0 the callback is
   installed for the entire bank.

   The <arg>user_data</arg> is used to associate user defined data with every
   callback. Every time the callback is invoked the data is provided as an
   argument to the callback.

   <insert-until text="// ADD INTERFACE bank_instrumentation_subscribe"/>

   Every function that registers a callback returns a unique handle of type
   <type>bank_callback_handle_t</type>. The <fun>remove_callback</fun> method
   uninstalls the callback associated with the handle.  The
   <fun>remove_connection_callbacks</fun> uninstalls all callbacks associated
   with a connection object. Similarly, the
   <fun>enable_connection_callbacks</fun> and
   <fun>disable_connection_callbacks</fun> methods are used to temporarily
   enable or disable a group of callbacks.

   Callback functions registered through the
   <iface>bank_instrumentation_subscribe</iface> interface are called before or
   after read and write accesses, like so:

   <insert id="before_read_callback_t"/>
   <insert id="after_read_callback_t"/>
   <insert id="before_write_callback_t"/>
   <insert id="after_write_callback_t"/>

   The <param>connection</param> is the object used to group the callback with
   any other callbacks, which may be NULL. The <param>access</param> object
   provides a number of methods which may be used along with the
   <param>handle</param> to perform a certain set of actions at the particular
   point of the access. The <param>user_data</param> is the custom data
   which was associated with the callback at registration.

   For every callback, additional information may be accessed using a specific
   interface which is passed as a parameter to the callback. See
   <iface>bank_before_read</iface>, <iface>bank_after_read</iface>,
   <iface>bank_before_write</iface>, and <iface>bank_after_write</iface> for
   details.

   </add>
   <add id="bank_instrumentation_subscribe_interface_exec_context">
   Cell Context for all methods.
   </add>
 */
SIM_INTERFACE(bank_instrumentation_subscribe) {
        bank_callback_handle_t (*register_before_read)(
                conf_object_t *NOTNULL bank,
                conf_object_t *connection,
                uint64 offset,
                uint64 size,
                before_read_callback_t before_read,
                lang_void *user_data);
        bank_callback_handle_t (*register_after_read)(
                conf_object_t *NOTNULL bank,
                conf_object_t *connection,
                uint64 offset,
                uint64 size,
                after_read_callback_t after_read,
                lang_void *user_data);

        bank_callback_handle_t (*register_before_write)(
                conf_object_t *NOTNULL bank,
                conf_object_t *connection,
                uint64 offset,
                uint64 size,
                before_write_callback_t before_write,
                lang_void *user_data);
        bank_callback_handle_t (*register_after_write)(
                conf_object_t *NOTNULL bank,
                conf_object_t *connection,
                uint64 offset,
                uint64 size,
                after_write_callback_t after_write,
                lang_void *user_data);

        void (*remove_callback)(conf_object_t *NOTNULL bank,
                                bank_callback_handle_t callback);

        void (*remove_connection_callbacks)(conf_object_t *NOTNULL bank,
                                            conf_object_t *NOTNULL connection);
        void (*enable_connection_callbacks)(conf_object_t *NOTNULL bank,
                                            conf_object_t *NOTNULL connection);
        void (*disable_connection_callbacks)(conf_object_t *NOTNULL bank,
                                             conf_object_t *NOTNULL connection);

};
#define BANK_INSTRUMENTATION_SUBSCRIBE_INTERFACE \
        "bank_instrumentation_subscribe"
// ADD INTERFACE bank_instrumentation_subscribe

#ifdef __cplusplus
}
#endif

#endif
