// -*- mode: C++; c-file-style: "virtutech-c++" -*-

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

#ifndef SIMICS_SYSTEMC_AWARENESS_PROXY_FACTORY_SIGNAL_PORT_H
#define SIMICS_SYSTEMC_AWARENESS_PROXY_FACTORY_SIGNAL_PORT_H

#include <simics/systemc/awareness/proxy_factory.h>
#include <simics/systemc/awareness/proxy_signal_port.h>
#include <simics/systemc/iface/instrumentation/provider_controller_simics_adapter.h>
#include <simics/systemc/iface/sc_signal_read_simics_adapter.h>
#include <simics/systemc/iface/sc_signal_write_simics_adapter.h>

#include <string>

namespace simics {
namespace systemc {
namespace awareness {

class ProxyFactorySignalPort : public ProxyFactory<ProxySignalPort> {
  public:
    void registerInterfaces(sc_core::sc_object *object,
                            conf_class_t *cls) const {
        ProxyFactory<ProxySignalPort>::registerInterfaces(object, cls);
        registerInterface<
            iface::instrumentation::ProviderControllerSimicsAdapter<
                ProxySignalPort> >(cls);
        registerInterface<iface::ScPortSimicsAdapter<ProxySignalPort> >(cls);
    }
};

class ProxyFactorySignalPortIn : public ProxyFactorySignalPort {
  public:
    virtual bool canManufacture(sc_core::sc_object *object) const {
        return std::string("sc_in") == object->kind();
    }
    void registerInterfaces(sc_core::sc_object *object,
                            conf_class_t *cls) const {
        ProxyFactorySignalPort::registerInterfaces(object, cls);
        registerInterface<iface::ScSignalReadSimicsAdapter<
            ProxySignalPort> >(cls);
    }
};

class ProxyFactorySignalPortOut : public ProxyFactorySignalPort {
  public:
    virtual bool canManufacture(sc_core::sc_object *object) const {
        return std::string("sc_out") == object->kind();
    }
    void registerInterfaces(sc_core::sc_object *object,
                            conf_class_t *cls) const {
        ProxyFactorySignalPort::registerInterfaces(object, cls);
        registerInterface<iface::ScSignalWriteSimicsAdapter<
            ProxySignalPort> >(cls);
    }
};

class ProxyFactorySignalPortInOut : public ProxyFactorySignalPort {
  public:
    virtual bool canManufacture(sc_core::sc_object *object) const {
        return std::string("sc_inout") == object->kind();
    }
    void registerInterfaces(sc_core::sc_object *object,
                            conf_class_t *cls) const {
        ProxyFactorySignalPort::registerInterfaces(object, cls);
        registerInterface<iface::ScSignalReadSimicsAdapter<
            ProxySignalPort> >(cls);
        registerInterface<iface::ScSignalWriteSimicsAdapter<
            ProxySignalPort> >(cls);
    }
};

}  // namespace awareness
}  // namespace systemc
}  // namespace simics

#endif
