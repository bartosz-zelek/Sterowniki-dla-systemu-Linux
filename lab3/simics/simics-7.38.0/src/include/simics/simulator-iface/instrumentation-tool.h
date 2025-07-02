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

#ifndef SIMICS_SIMULATOR_IFACE_INSTRUMENTATION_TOOL_H
#define SIMICS_SIMULATOR_IFACE_INSTRUMENTATION_TOOL_H

#include <simics/base/types.h>
#include <simics/base/conf-object.h>
#include <simics/base/attr-value.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif
        
/* <add id="instrumentation_tool_interface_t">

   This interface is intended to be implemented by instrumentation tools using
   the instrumentation framework. The instrumentation framework handles setting
   up, controlling and removing the connection between providers and tools, but
   the actual communication is handled by instrumentation specific interfaces.

   When a connection with a provider is being established, the
   <fun>connect</fun> method is called. The <arg>provider</arg> argument
   specifies the provider which should be connected. The <arg>args</arg>
   are tool specific arguments that can be used allowing the connection
   to be configured a certain way. The tool should create a new
   dedicated connection object, which register itself with the provider
   using the dedicated interface. The connection object created
   is returned by the <fun>connect</fun> function.   
   If, for any reason, the tool cannot successfully connect
   to the provider, NULL should be returned to indicate failure.

   If a connection should be removed, the <fun>disconnect</fun> method
   is called. The <arg>conn_obj</arg> argument is the connection object
   returned earlier in <fun>connect</fun>.
   It is up to the tool to delete the created object which should
   unregister itself from the provider.

   <insert-until text="// ADD INTERFACE instrumentation_tool_interface"/>
   </add>
   <add id="instrumentation_tool_interface_exec_context">
   Global Context for all methods.
   </add> */        
SIM_INTERFACE(instrumentation_tool) {
        conf_object_t *(*connect)(
                conf_object_t *NOTNULL obj,
                conf_object_t *NOTNULL provider,
                attr_value_t args);
        void  (*disconnect)(
                conf_object_t *NOTNULL obj,
                conf_object_t *NOTNULL conn_obj);
};
#define INSTRUMENTATION_TOOL_INTERFACE "instrumentation_tool"
// ADD INTERFACE instrumentation_tool_interface

/* <add id="instrumentation_connection_interface_t">

   This interface is intended to be implemented by instrumentation connections
   using the instrumentation framework. This interface is used to request that
   instrumentation should be temporarily disabled and then re-enabled. That is,
   the connection should not collect any data when it is being disabled. How
   this is achieved is up to the tool, it could tell the provider to stop
   sending information, or simply throw away anything it sends.
   This interface can be used with high frequency while simulation is running,
   so it should be implemented with performance in mind.
   <insert-until text="// ADD INTERFACE instrumentation_connection_interface"/>
   </add>
   <add id="instrumentation_connection_interface_exec_context">
   Cell Context for all methods.
   </add> */        

SIM_INTERFACE(instrumentation_connection) {
        void (*enable)(conf_object_t *obj);
        void (*disable)(conf_object_t *obj);
};

#define INSTRUMENTATION_CONNECTION_INTERFACE "instrumentation_connection"
// ADD INTERFACE instrumentation_connection_interface

#if defined(__cplusplus)
}
#endif
#endif
