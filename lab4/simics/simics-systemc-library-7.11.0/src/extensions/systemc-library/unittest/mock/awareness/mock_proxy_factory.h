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

#ifndef SYSTEMC_LIBRARY_UNITTEST_MOCK_AWARENESS_MOCK_PROXY_FACTORY_H
#define SYSTEMC_LIBRARY_UNITTEST_MOCK_AWARENESS_MOCK_PROXY_FACTORY_H

#include <simics/systemc/awareness/proxy_factory_interface.h>
#include <string>

#include "mock_class_attributes.h"
#include "mock_instance_attributes.h"
#include "stubs.h"

typedef struct conf_class conf_class_t;

namespace unittest {
namespace awareness {

using simics::systemc::awareness::ProxyFactoryInterface;
using simics::systemc::awareness::ProxyInterface;
using simics::systemc::awareness::ClassAttributesInterface;
using simics::systemc::awareness::InstanceAttributesInterface;

class MockProxyFactory : public ProxyFactoryInterface {
  public:
    MockProxyFactory() : create_conf_class_count_(0),
                         register_attributes_count_(0),
                         register_interfaces_count_(0),
                         register_log_groups_count_(0),
                         register_features_count_(0),
                         create_conf_class_return_(&conf_class_) {}
    virtual bool mapToProxy(sc_core::sc_object *object) const {
        return true;
    }
    virtual bool canManufacture(sc_core::sc_object *object) const {
        return true;
    }
    virtual bool needUniqueConfClassName(sc_core::sc_object *object) const {
        return true;
    }
    virtual const ClassAttributesInterface *classAttributes(
        sc_core::sc_object *object) const {
        return &class_attributes_;
    }
    virtual const InstanceAttributesInterface *instanceAttributes(
        sc_core::sc_object *object) const {
        return &instance_attributes_;
    }
    virtual conf_class_t *createConfClass(sc_core::sc_object *object,
                                          std::string name,
                                          std::string description,
                                          std::string documentation) const {
        ++create_conf_class_count_;
        return create_conf_class_return_;
    }
    virtual void registerAttributes(sc_core::sc_object *object,
                                    conf_class_t *cls) const {
        ++register_attributes_count_;
    }
    virtual void registerInterfaces(sc_core::sc_object *object,
                                    conf_class_t *cls) const {
        ++register_interfaces_count_;
    }
    virtual void registerLogGroups(sc_core::sc_object *object,
                                   conf_class_t *cls) const {
        ++register_log_groups_count_;
    }
    virtual void registerFeatures(sc_core::sc_object *object,
                                  ProxyInterface *proxy) const {
        ++register_features_count_;
    }
    virtual bool registerClass(
        std::string sc_kind, const char *class_name) const {
        return true;
    }
    virtual bool isClassRegistered(std::string sc_kind) const {
        return true;
    }

    MockClassAttributes class_attributes_;
    MockInstanceAttributes instance_attributes_;
    mutable int create_conf_class_count_;
    mutable int register_attributes_count_;
    mutable int register_interfaces_count_;
    mutable int register_log_groups_count_;
    mutable int register_features_count_;
    conf_class_t conf_class_;
    conf_class_t *create_conf_class_return_;
};

}  // namespace awareness
}  // namespace unittest

#endif
