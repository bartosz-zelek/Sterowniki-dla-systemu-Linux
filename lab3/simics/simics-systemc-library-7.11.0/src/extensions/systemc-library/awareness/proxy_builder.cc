// -*- mode: C++; c-file-style: "virtutech-c++" -*-

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

#include <simics/systemc/awareness/class_attributes.h>
#include <simics/systemc/awareness/proxy_builder.h>
#include <simics/systemc/awareness/proxy_factory_base.h>
#if !defined(SIMICS_6_API)
#define SIMICS_6_API
#include <simics/systemc/awareness/init.h>
#undef SIMICS_6_API
#endif
#include <simics/systemc/adapter_log_groups.h>

#include <simics/simulator-api.h>

#include <string>

namespace simics {
namespace systemc {
namespace awareness {

ProxyBuilder::ProxyBuilder()
    : class_registry_(proxy_factory_registry_),
      blocklist_(&proxy_factory_registry_) {
    proxy_factory_registry_.addFactory(&factory_);
    proxy_factory_registry_.addFactory(&factory_initiator_gasket_);
    proxy_factory_registry_.addFactory(&factory_signal_);
    proxy_factory_registry_.addFactory(&factory_process_);
    proxy_factory_registry_.addFactory(&factory_export_);
    proxy_factory_registry_.addFactory(&factory_port_);
#if INTC_EXT
    registerSocketType<32, tlm::tlm_base_protocol_types>();
    registerSocketType<64, tlm::tlm_base_protocol_types>();
#endif
    proxy_factory_registry_.addFactory(&factory_event_);
    proxy_factory_registry_.addFactory(&factory_signal_port_in_);
    proxy_factory_registry_.addFactory(&factory_signal_port_out_);
    proxy_factory_registry_.addFactory(&factory_signal_port_in_out_);
    proxy_factory_registry_.addFactory(&factory_vector_);
}

void ProxyBuilder::suppressProxyBuild(sc_core::sc_object* obj) {
    factory_.addSuppressProxyBuild(obj);
}

void ProxyBuilder::buildProxies(ConfObjectRef object,
                                iface::SimulationInterface *simulation,
                                ObjProxyInterfaceMap *links) {
    ProxyFactoryBase::set_log_object(object);
    ObjProxyInterfaceMap new_links;
    ClassAttributes::set_adapter_class_name(
        SIM_get_class_name(SIM_object_class(object)));
    // Use (void) to avoid coverity warning "unchecked_return_value"
    (void)SIM_clear_exception();
    registerProxyClasses(object);
    ProxyCreateTraverser create(object, simulation, &proxy_factory_registry_,
                                &class_registry_, links, &new_links);
    // Create proxy instances for SC_Objects mapped by the factories.
    blocklist_.setTraverser(&create);
    blocklist_.traverseAll();

    for (auto& i : new_links) {
        if (i.second)
            i.second->allProxiesInitialized();
    }
}

void ProxyBuilder::setProxyAttributes(ObjProxyInterfaceMap *links,
                                      Attributes *attributes) {
    // Create a new set of instance attributes for this particular Adapter
    // instance
    attributes->init();
    // Set attribute values of proxy instances (i.e. assign the Attributes
    // "database" to each proxy object instance). This must be done in a second
    // step after buildProxies() because the mapping of SC objects to proxy
    // objects (i.e. the links map) needs to be completed before assignment.
    ProxyAttributeSetter setter(links, attributes);
    blocklist_.setTraverser(&setter);
    blocklist_.traverseAll();
}

void ProxyBuilder::registerProxyClass(ConfObjectRef log_obj,
                                      ProxyFactoryInterface *factory,
                                      std::string sc_kind) {
    if (factory->isClassRegistered(sc_kind)) {
        return;
    }
    if (!factory->registerClass(sc_kind, sc_kind.c_str())) {
        SIM_LOG_ERROR(log_obj, Log_Awareness,
                      "Could not register class %s!", sc_kind.c_str());
    }
}

void ProxyBuilder::registerProxyClasses(ConfObjectRef log_obj) {
    // Register basic proxies and load commands.
    registerProxyClass(log_obj, &factory_, "sc_module");
    registerProxyClass(log_obj, &factory_process_, "sc_method_process");
    registerProxyClass(log_obj, &factory_process_, "sc_thread_process");

    loadModule(log_obj, "systemc-tools");
    loadModule(log_obj, "systemc-commands");
    loadModule(log_obj, "systemc-interfaces");
}

void ProxyBuilder::loadModule(ConfObjectRef log_obj, const char* name) {
    SIM_load_module(name);
    if (SIM_clear_exception() != SimExc_No_Exception) {
        SIM_LOG_INFO(1, log_obj, Log_Awareness,
                     "Failed to load module: '%s'"
                     ", some features will not be available."
                     " To enable these features please install the"
                     " SystemC Library add-on package.", name);
    }
}

ProxyFactoryRegistry *ProxyBuilder::proxy_factory_registry() {
    return &proxy_factory_registry_;
}

}  // namespace awareness
}  // namespace systemc
}  // namespace simics
