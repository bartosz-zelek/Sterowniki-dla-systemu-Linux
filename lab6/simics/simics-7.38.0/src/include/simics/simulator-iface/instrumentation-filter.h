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

#ifndef SIMICS_SIMULATOR_IFACE_INSTRUMENTATION_FILTER_H
#define SIMICS_SIMULATOR_IFACE_INSTRUMENTATION_FILTER_H

#include <simics/base/types.h>
#include <simics/base/conf-object.h>
#include <simics/base/attr-value.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add id="instrumentation_filter_slave_interface_t">

   This interface is implemented by the
   <class>instrumentation_filter_aggregator</class> class objects,
   here referred to as "aggregator". The interface should be
   called by instrumentation filters to enable or disable a connection.
   The aggregator object is located between the filters and the connection,
   keeping the connections unaware of multiple filters that might
   be disabling them.

   The <fun>disable</fun> method tells the aggregator that the unique
   <param>source_id</param> currently wants to disable the connection.
   As long as there is one <param>source_id</param> that is disabled,
   the connection is disabled. Only when all sources are enabled
   the connection is enabled.

   Similarly, the <fun>enable</fun> enables the connection. That
   is, the filter is now in a state when it thinks the connection
   should be enabled.

   <insert-until text="// ADD INTERFACE instrumentation_filter_slave"/>
   </add>
   <add id="instrumentation_filter_slave_interface_exec_context">
   Cell Context for all methods.
   </add> */                
SIM_INTERFACE(instrumentation_filter_slave) {
        void (*disable)(conf_object_t *obj, unsigned source_id);
        void (*enable)(conf_object_t *obj, unsigned source_id);
};
#define INSTRUMENTATION_FILTER_SLAVE_INTERFACE "instrumentation_filter_slave"
// ADD INTERFACE instrumentation_filter_slave

/* <add id="instrumentation_filter_status_interface_t">

   This interface is implemented by the
   <class>instrumentation_filter_aggregator</class> class objects. This
   interface should only be used by Simics instrumentation framework itself.
   The <fun>get_disabled_sources</fun> method returns an
   <type>attr_value_t</type> list of integers representing the source_ids that
   currently causing the connection to be disabled.

   <insert-until text="// ADD INTERFACE instrumentation_filter_status"/>
   </add>
   <add id="instrumentation_filter_status_interface_exec_context">
   Global Context for all methods.
   </add> */                        
SIM_INTERFACE(instrumentation_filter_status) {
        attr_value_t (*get_disabled_sources)(conf_object_t *obj);
};
#define INSTRUMENTATION_FILTER_STATUS_INTERFACE "instrumentation_filter_status"
// ADD INTERFACE instrumentation_filter_status


        
/* <add id="instrumentation_filter_master_interface_t">

   This interface is intended to be implemented by instrumentation filters.
   Instrumentation filters should enable the associated slaves, which
   themselves are associated with instrumentation connections.

   The <fun>set_source_id</fun> method supplies the filter with an unique
   source number for the filter. This function should only be called once,
   the filter needs to store this number so it can be used when 
   calling the <iface>instrumentation_filter_slave</iface> interface
   methods.

   The <fun>add_slave</fun> method informs the filter that it should enable or
   disable this slave too. The <param>slave</param> object should implement the
   <iface>instrumentation_filter_slave</iface> interface which should be
   used. The <param>provider_obj</param> parameter is the provider object that
   is located behind the slave and its connection.  Depending on how the filter
   works, it may or may not make use of this parameter. For example, the
   process-filter might detect that the filtered process is now running on a
   particular processor, then it can enable this particular connection only,
   given that it can match the <param>provider_obj</param> with the processor
   currently running the tracked process.

   The <fun>remove_slave</fun> method removes a slave from the filter, so
   it should not call it anymore.

   The <fun>short_filter_config</fun> method should return a short textual
   description on how the filter is currently set up. This information
   is used in various print commands.
   
   <insert-until text="// ADD INTERFACE instrumentation_filter_master"/>
   </add>
   <add id="instrumentation_filter_master_interface_exec_context">
   Global Context for all methods.
   </add> */                
SIM_INTERFACE(instrumentation_filter_master) {
        void (*set_source_id)(conf_object_t *obj, unsigned source_id);
        bool (*add_slave)(conf_object_t *obj, conf_object_t *slave,
                          conf_object_t *provider_obj);
        void (*remove_slave)(conf_object_t *obj, conf_object_t *slave,
                             conf_object_t *provider_obj);
        char *(*short_filter_config)(conf_object_t *obj);
};
#define INSTRUMENTATION_FILTER_MASTER_INTERFACE "instrumentation_filter_master"
// ADD INTERFACE instrumentation_filter_master
        

#if defined(__cplusplus)
}
#endif
#endif
