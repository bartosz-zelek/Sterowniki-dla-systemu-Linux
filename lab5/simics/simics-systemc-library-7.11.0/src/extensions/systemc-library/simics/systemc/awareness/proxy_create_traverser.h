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

#ifndef SIMICS_SYSTEMC_AWARENESS_PROXY_CREATE_TRAVERSER_H
#define SIMICS_SYSTEMC_AWARENESS_PROXY_CREATE_TRAVERSER_H

#include <simics/cc-api.h>
#include <simics/systemc/awareness/proxy_class_registry.h>
#include <simics/systemc/awareness/proxy_factory_interface.h>
#include <simics/systemc/iface/simulation_interface.h>
#include <simics/systemc/traverser_interface.h>

#include <map>
#include <string>

namespace simics {
namespace systemc {
namespace awareness {

class ProxyCreateTraverser : public simics::systemc::TraverserInterface {
    using ObjProxyInterfaceMap = std::map<sc_core::sc_object *,
                                          ProxyInterface *>;

  public:
    ProxyCreateTraverser(simics::ConfObjectRef object,
                         iface::SimulationInterface *simulation,
                         ProxyFactoryInterface *factory,
                         ProxyClassRegistry *registry,
                         ObjProxyInterfaceMap *links,
                         ObjProxyInterfaceMap *new_links);
    virtual void applyOn(sc_core::sc_object *obj);
    virtual void done();

  protected:
    conf_object_t *root_;
    std::string prefix_;
    ProxyClassRegistry *registry_;
    ObjProxyInterfaceMap *links_, *new_links_;
    iface::SimulationInterface *simulation_;
    ProxyFactoryInterface *factory_;
};

}  // namespace awareness
}  // namespace systemc
}  // namespace simics

#endif
