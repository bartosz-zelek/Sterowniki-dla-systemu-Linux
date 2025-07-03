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

#ifndef SIMICS_SYSTEMC_AWARENESS_PROXY_BUILDER_H
#define SIMICS_SYSTEMC_AWARENESS_PROXY_BUILDER_H

#include <simics/base/types.h>

#include <systemc>
#include <tlm>
#include <tlm_utils/multi_socket_bases.h>

#include <simics/systemc/awareness/attributes.h>
#include <simics/systemc/awareness/proxy_attribute_setter.h>
#include <simics/systemc/awareness/proxy_blocklist_traverser.h>
#include <simics/systemc/awareness/proxy_class_registry.h>
#include <simics/systemc/awareness/proxy_create_traverser.h>
#include <simics/systemc/awareness/proxy_interface.h>
#include <simics/systemc/awareness/proxy_factory.h>
#include <simics/systemc/awareness/proxy_factory_event.h>
#include <simics/systemc/awareness/proxy_factory_export.h>
#include <simics/systemc/awareness/proxy_factory_initiator_gasket.h>
#include <simics/systemc/awareness/proxy_factory_port.h>
#include <simics/systemc/awareness/proxy_factory_process.h>
#include <simics/systemc/awareness/proxy_factory_registry.h>
#include <simics/systemc/awareness/proxy_factory_signal.h>
#include <simics/systemc/awareness/proxy_factory_signal_port.h>
#include <simics/systemc/awareness/proxy_factory_vector.h>
#include <simics/systemc/iface/simulation_interface.h>

#include <map>
#include <string>

namespace simics {
namespace systemc {
namespace awareness {

class ProxyBuilder {
    using ObjProxyInterfaceMap = std::map<sc_core::sc_object *,
                                          ProxyInterface *>;

  public:
    ProxyBuilder();
    void suppressProxyBuild(sc_core::sc_object* obj);
    void buildProxies(ConfObjectRef object,
                      iface::SimulationInterface *simulation,
                      ObjProxyInterfaceMap *links);
    void setProxyAttributes(ObjProxyInterfaceMap *links,
                            Attributes *attributes);
    ProxyFactoryRegistry *proxy_factory_registry();
    void loadModule(simics::ConfObjectRef log_obj, const char* name);

  private:
    void registerProxyClass(ConfObjectRef log_obj,
                            ProxyFactoryInterface *factory,
                            std::string sc_kind);
    void registerProxyClasses(ConfObjectRef log_obj);

    ProxyFactoryRegistry proxy_factory_registry_;
    ProxyClassRegistry class_registry_;
    ProxyFactory<> factory_;
    ProxyFactorySignal factory_signal_;
    ProxyFactoryProcess factory_process_;
    ProxyFactoryInitiatorGasket factory_initiator_gasket_;
    ProxyFactoryEvent factory_event_;
    ProxyFactoryExport factory_export_;
    ProxyFactoryPort factory_port_;
    ProxyFactorySignalPortIn factory_signal_port_in_;
    ProxyFactorySignalPortOut factory_signal_port_out_;
    ProxyFactorySignalPortInOut factory_signal_port_in_out_;
    ProxyFactoryVector factory_vector_;
    ProxyBlocklistTraverser blocklist_;
};

}  // namespace awareness
}  // namespace systemc
}  // namespace simics

#endif
