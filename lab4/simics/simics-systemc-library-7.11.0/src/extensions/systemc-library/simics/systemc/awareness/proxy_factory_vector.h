// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2017 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_AWARENESS_PROXY_FACTORY_VECTOR_H
#define SIMICS_SYSTEMC_AWARENESS_PROXY_FACTORY_VECTOR_H

#include <simics/systemc/awareness/proxy_factory.h>
#include <simics/systemc/awareness/proxy_vector.h>
#include <simics/systemc/iface/sc_vector_simics_adapter.h>

namespace simics {
namespace systemc {
namespace awareness {

class ProxyFactoryVector : public ProxyFactory<ProxyVector> {
  public:
    virtual bool canManufacture(sc_core::sc_object *object) const {
        return dynamic_cast<sc_core::sc_vector_base *>(object);
    }

    void registerInterfaces(sc_core::sc_object *object,
                            conf_class_t *cls) const {
        ProxyFactory<ProxyVector>::registerInterfaces(object, cls);
        registerInterface<iface::ScVectorSimicsAdapter<ProxyVector> >(cls);
    }
};

}  // namespace awareness
}  // namespace systemc
}  // namespace simics

#endif
