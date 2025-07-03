// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2015 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_AWARENESS_PROXY_FACTORY_INITIATOR_GASKET_H
#define SIMICS_SYSTEMC_AWARENESS_PROXY_FACTORY_INITIATOR_GASKET_H

#include <simics/systemc/awareness/class_attributes_fixed_name.h>
#include <simics/systemc/awareness/proxy_initiator_gasket.h>
#include <simics/systemc/awareness/proxy_factory.h>
#include <simics/systemc/iface/sc_initiator_gasket_interface.h>
#include <simics/systemc/iface/sc_initiator_gasket_simics_adapter.h>
#include <simics/systemc/simics2tlm/gasket_interface.h>

namespace simics {
namespace systemc {
namespace awareness {

class ProxyFactoryInitiatorGasket : public ProxyFactory<ProxyInitiatorGasket> {
  public:
    ProxyFactoryInitiatorGasket() : class_attributes_("sc_initiator_gasket") {}
    bool canManufacture(sc_core::sc_object *object) const {
        return dynamic_cast<simics2tlm::GasketInterface*>(object) != NULL;
    }

    const ClassAttributesInterface *
    classAttributes(sc_core::sc_object *object) const {
        return &class_attributes_;
    }

    void registerInterfaces(sc_core::sc_object *object,
                            conf_class_t *cls) const {
        ProxyFactory<ProxyInitiatorGasket>::registerInterfaces(object, cls);
        registerInterface<iface::ScInitiatorGasketSimicsAdapter<
            ProxyInitiatorGasket> >(cls);
    }
  private:
    ClassAttributesFixedName class_attributes_;
};

}  // namespace awareness
}  // namespace systemc
}  // namespace simics

#endif
