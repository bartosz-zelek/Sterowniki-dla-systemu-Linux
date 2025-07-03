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

#ifndef SIMICS_MODEL_IFACE_INSTRUMENTATION_PROVIDER_H
#define SIMICS_MODEL_IFACE_INSTRUMENTATION_PROVIDER_H

#include <simics/base/types.h>
#include <simics/base/attr-value.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add id="instrumentation_order_interface_t"> 

   This interface is used to control the dispatch order of connected
   instrumentation. It is implemented by instrumentation providers that have
   the ability to change the order in which instrumentation events occur. This
   works by associating every instrumentation event with a connection
   object. It is up to the provider to supply a way to do this. See the
   <iface>cpu_instrumentation_subscribe</iface> interface for an example.  Most
   users of any instrumentation will only be observing the state of the
   provider, in which case the order is unimportant. However, if users
   of instrumentation may change the behavior of the provider, this interface
   may be useful.

   The default order for callbacks that should be honored by all providers,
   where possible, regardless if they implement the
   <iface>instrumentation_order</iface> interface or not is:

   <ol>
   <li>all anonymous connections, i.e. NULL connections, in registration order</li>
   <li>connection order, which if not re-ordered will be the connection
   registration order</li>
   <li>callback registration order</li>
   </ol>

   The <arg>get_connections</arg> method should return an
   <type>attr_value_t</type> list with connection objects that represent the
   current order. The first element in the list is the first object in the
   dispatch order, etc.

   The <arg>move_before</arg> method moves the connection given by the
   <arg>connection</arg> argument before the connection given by the
   <arg>anchor</arg> argument. If the <arg>anchor</arg> is NULL the connection
   will be moved last. The given connection objects must be present in the
   current dispatch order for this to succeed.

   <insert-until text="// ADD INTERFACE instrumentation_order"/>

   </add>

   <add id="instrumentation_order_interface_exec_context">
   Global Context for all methods, but must be called from a callback
   receiving a handle of type <type>instruction_handle_t</type>.
   </add>
 */        
SIM_INTERFACE(instrumentation_order) {
        // Returns an object list in the connection order
        attr_value_t (*get_connections)(conf_object_t *obj);
        
        bool (*move_before)(conf_object_t *self, conf_object_t *connection,
                            conf_object_t *before); 
};
#define INSTRUMENTATION_ORDER_INTERFACE "instrumentation_order"
// ADD INTERFACE instrumentation_order

/* <add id="callback_info_interface_t"> 

   This interface can be implemented by any object that can issue callbacks.
   The <arg>get_callbacks</arg> method returns a <type>attr_value_t</type> list
   of type <tt>[[(o|n)sss*]]</tt> where <tt>o</tt> is the object that installed
   the callback (if applicable, otherwise NIL). The <tt>sss</tt> strings are:

   <ul>
   <li>A one liner description about the callback. Could for example contain the
   interface method that installed the callback, if such one exist.</li>
   <li>The function name.</li>
   <li>A string describing the user data.</li>
   </ul>

   <insert-until text="// ADD INTERFACE callback_info"/>

   </add>

   <add id="callback_info_interface_exec_context">
   Cell Context for all methods, but must be called from a callback
   receiving a handle of type <type>instruction_handle_t</type>.
   </add>
 */        
SIM_INTERFACE(callback_info) {
        attr_value_t (*get_callbacks)(conf_object_t *obj);
};
#define CALLBACK_INFO_INTERFACE "callback_info"
// ADD INTERFACE callback_info
        
#if defined(__cplusplus)
}
#endif

#endif
