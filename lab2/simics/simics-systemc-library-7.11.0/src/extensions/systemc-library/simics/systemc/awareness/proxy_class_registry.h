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

#ifndef SIMICS_SYSTEMC_AWARENESS_PROXY_CLASS_REGISTRY_H
#define SIMICS_SYSTEMC_AWARENESS_PROXY_CLASS_REGISTRY_H

#include <simics/base/types.h>
#include <simics/base/sim-exception.h>
#include <simics/simulator/sim-get-class.h>

#include <simics/systemc/awareness/class_attributes_interface.h>
#include <simics/systemc/awareness/proxy_factory_interface.h>
#include <simics/systemc/awareness/proxy_class_name.h>

#include <systemc>
#include <string>
#include <utility>

namespace simics {
namespace systemc {
namespace awareness {

class ProxyClassRegistry {
  public:
    explicit ProxyClassRegistry(const ProxyFactoryInterface &factory)
        : factory_(factory) {
    }
    conf_class_t *confClass(sc_core::sc_object *object) {
        if (!factory_.canManufacture(object))
            return NULL;

        const ClassAttributesInterface *attributes = factory_.classAttributes(
            object);

        ProxyClassName name(
            factory_.needUniqueConfClassName(object) ?
                attributes->uniqueName(object) : attributes->name(object));
        conf_class_t *cls = SIM_get_class(name.c_str());
        if (SIM_clear_exception() != SimExc_No_Exception) {
            cls = factory_.createConfClass(object, std::move(name),
                                           attributes->description(object),
                                           attributes->documentation(object));
            if (cls == NULL) {
                // Use (void) to avoid coverity warning "unchecked_return_value"
                (void)SIM_clear_exception();
                return NULL;
            }

            factory_.registerAttributes(object, cls);
            factory_.registerInterfaces(object, cls);
            factory_.registerLogGroups(object, cls);
        }

        return cls;
    }

  protected:
    const ProxyFactoryInterface &factory_;
};

}  // namespace awareness
}  // namespace systemc
}  // namespace simics

#endif
