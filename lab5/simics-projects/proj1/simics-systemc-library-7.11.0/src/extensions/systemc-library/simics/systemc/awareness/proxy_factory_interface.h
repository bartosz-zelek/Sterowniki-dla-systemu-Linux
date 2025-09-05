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

#ifndef SIMICS_SYSTEMC_AWARENESS_PROXY_FACTORY_INTERFACE_H
#define SIMICS_SYSTEMC_AWARENESS_PROXY_FACTORY_INTERFACE_H

#include <simics/base/conf-object.h>
#include <simics/systemc/awareness/class_attributes_interface.h>
#include <simics/systemc/awareness/instance_attributes_interface.h>
#include <simics/systemc/awareness/proxy_interface.h>

#include <systemc>
#include <string>

namespace simics {
namespace systemc {
namespace awareness {

class ProxyFactoryInterface {
  public:
    virtual ~ProxyFactoryInterface() {}

    // Returns bool, used to decide if object should be used in the process
    // when creating proxy instances for SC objects. One reason for skipping
    // the mapping from sc_object to proxy object is that the sc_object should
    // be exposed as an attribute of the proxy
    virtual bool mapToProxy(sc_core::sc_object *object) const = 0;
    // Returns bool, used to indicate that the factory has a specialized
    // conf_class implementation for this object.
    virtual bool canManufacture(sc_core::sc_object *object) const = 0;
    // Returns bool, used to decided if a unique conf class needs to be
    // created.  When returning false, the first conf class created for this
    // type of sc_object will be reused.
    virtual bool needUniqueConfClassName(sc_core::sc_object *object) const = 0;
    // Provides name, description and documentation for object.
    virtual const ClassAttributesInterface *classAttributes(
        sc_core::sc_object *object) const = 0;
    // Provides name for proxy instance.
    virtual const InstanceAttributesInterface *instanceAttributes(
        sc_core::sc_object *object) const = 0;
    // Creates conf_class_t for object based on name, description and
    // documentation.
    virtual conf_class_t *createConfClass(sc_core::sc_object *object,
                                          std::string name,
                                          std::string description,
                                          std::string documentation) const = 0;
    // Registers all attributes for the proxy object
    virtual void registerAttributes(sc_core::sc_object *object,
                                    conf_class_t *cls) const = 0;
    // Register additional interfaces for the proxy object based on sc_object
    virtual void registerInterfaces(sc_core::sc_object *object,
                                    conf_class_t *cls) const = 0;
    // Registers the log groups for the proxy object
    virtual void registerLogGroups(sc_core::sc_object *object,
                                   conf_class_t *cls) const = 0;
    // Register additional proxy internal functionality
    virtual void registerFeatures(sc_core::sc_object *object,
                                  ProxyInterface *proxy) const = 0;
    // Returns bool, used to register a configuration class for a SystemC type
    // sc_kind named class_name. Returns false if the class could not be
    // created. Returns true if the class already existed or if it was
    // successfully created.
    virtual bool registerClass(
        std::string sc_kind, const char *class_name) const = 0;
    // Returns bool, used to decide if sc_kind is a configuration class.
    virtual bool isClassRegistered(std::string sc_kind) const = 0;
};

}  // namespace awareness
}  // namespace systemc
}  // namespace simics

#endif
