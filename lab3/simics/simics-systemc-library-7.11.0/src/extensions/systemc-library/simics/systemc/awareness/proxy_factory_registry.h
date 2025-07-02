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

#ifndef SIMICS_SYSTEMC_AWARENESS_PROXY_FACTORY_REGISTRY_H
#define SIMICS_SYSTEMC_AWARENESS_PROXY_FACTORY_REGISTRY_H

#include <simics/systemc/awareness/proxy_factory_interface.h>

#include <string>
#include <deque>
#include <utility>  // move

namespace simics {
namespace systemc {
namespace awareness {

class ProxyFactoryRegistry : public ProxyFactoryInterface {
  public:
    ProxyFactoryRegistry() : factory_(factories_.end()) {}
    void addFactory(ProxyFactoryInterface *factory) {
        factories_.push_front(factory);
        factory_ = factories_.end();
    }
    virtual bool mapToProxy(sc_core::sc_object *object) const {
        for (std::deque<ProxyFactoryInterface*>::const_iterator i = factories_
            .begin(); i != factories_.end(); ++i) {
            if (!(*i)->mapToProxy(object))
                return false;
        }
        return true;
    }
    virtual bool canManufacture(sc_core::sc_object *object) const {
        for (factory_ = factories_.begin(); factory_ != factories_.end();
            ++factory_) {
            if ((*factory_)->canManufacture(object)) {
                return true;
            }
        }
        return false;
    }
    virtual bool needUniqueConfClassName(sc_core::sc_object *object) const {
        if (factory_ != factories_.end()) {
            if ((*factory_)->needUniqueConfClassName(object))
                return true;
        }
        return false;
    }
    virtual const ClassAttributesInterface *classAttributes(
        sc_core::sc_object *object) const {
        if (factory_ != factories_.end()) {
            return (*factory_)->classAttributes(object);
        }
        return NULL;
    }
    virtual const InstanceAttributesInterface *instanceAttributes(
        sc_core::sc_object *object) const {
        if (factory_ != factories_.end()) {
            return (*factory_)->instanceAttributes(object);
        }
        return NULL;
    }
    virtual conf_class_t *createConfClass(sc_core::sc_object *object,
                                          std::string name,
                                          std::string description,
                                          std::string documentation) const {
        if (factory_ != factories_.end()) {
            return (*factory_)->createConfClass(object, std::move(name),
                                                std::move(description),
                                                std::move(documentation));
        }
        return NULL;
    }
    virtual void registerAttributes(sc_core::sc_object *object,
                                    conf_class_t *cls) const {
        if (factory_ != factories_.end())
            (*factory_)->registerAttributes(object, cls);
    }
    virtual void registerInterfaces(sc_core::sc_object *object,
                                    conf_class_t *cls) const {
        if (factory_ != factories_.end())
            (*factory_)->registerInterfaces(object, cls);
    }
    virtual void registerLogGroups(sc_core::sc_object *object,
                                   conf_class_t *cls) const {
        if (factory_ != factories_.end())
            (*factory_)->registerLogGroups(object, cls);
    }
    virtual void registerFeatures(sc_core::sc_object *object,
                                  ProxyInterface *proxy) const {
        if (factory_ != factories_.end())
            (*factory_)->registerFeatures(object, proxy);
    }
    virtual bool registerClass(
        std::string sc_kind, const char *class_name) const {
        if (factory_ != factories_.end())
            return (*factory_)->registerClass(std::move(sc_kind), class_name);
        return false;
    }
    virtual bool isClassRegistered(std::string sc_kind) const {
        if (factory_ != factories_.end())
            return (*factory_)->isClassRegistered(std::move(sc_kind));
        return false;
    }

  private:
    std::deque<ProxyFactoryInterface*> factories_;
    mutable std::deque<ProxyFactoryInterface*>::const_iterator factory_;
};

}  // namespace awareness
}  // namespace systemc
}  // namespace simics

#endif
