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

#ifndef SIMICS_SYSTEMC_AWARENESS_PROXY_FACTORY_H
#define SIMICS_SYSTEMC_AWARENESS_PROXY_FACTORY_H

#include <simics/base/types.h>
#include <simics/base/sim-exception.h>
#include <simics/simulator/sim-get-class.h>

#include <simics/systemc/adapter_log_groups.h>
#include <simics/systemc/awareness/cci_attribute.h>
#include <simics/systemc/awareness/class_attributes.h>
#include <simics/systemc/awareness/instance_attributes.h>
#include <simics/systemc/awareness/proxy_attribute_name.h>
#include <simics/systemc/awareness/proxy_factory_base.h>
#include <simics/systemc/awareness/proxy_factory_interface.h>
#include <simics/systemc/awareness/proxy.h>
#include <simics/systemc/cci_configuration.h>
#include <simics/systemc/iface/sc_object_simics_adapter.h>

#include <vector>
#include <string>
#include <set>
#include <utility>  // move

namespace simics {
namespace systemc {
namespace awareness {

template <typename TProxy = Proxy>
class ProxyFactory : public ProxyFactoryBase, public ProxyFactoryInterface {
  public:
    virtual bool mapToProxy(sc_core::sc_object *object) const {
        if (suppress_proxy_build_.find(object) != suppress_proxy_build_.end())
            return false;
        return true;
    }
    virtual bool canManufacture(sc_core::sc_object *object) const {
        return true;
    }
    virtual bool needUniqueConfClassName(sc_core::sc_object *object) const {
#if INTC_EXT && USE_SIMICS_CCI
        CciConfiguration cfg;
        return cfg.getParameters(object).size() > 0;
#else
        return false;
#endif
    }
    virtual const ClassAttributesInterface *classAttributes(
        sc_core::sc_object *object) const {
        return &sc_class_attributes;
    }
    virtual const InstanceAttributesInterface *instanceAttributes(
        sc_core::sc_object *object) const {
        return &sc_instance_attributes;
    }
    virtual conf_class_t *createConfClass(sc_core::sc_object *object,
                                          std::string name,
                                          std::string description,
                                          std::string documentation) const {
        return *make_class<TProxy>(name, description, documentation,
                                   Sim_Class_Kind_Pseudo).get();
    }
    virtual void registerAttributes(sc_core::sc_object *object,
                                    conf_class_t *cls) const {
        if (!object)
            return;

#if INTC_EXT && USE_SIMICS_CCI
        CciConfiguration cfg;
        std::vector <cci::cci_param_handle> parameters =
                cfg.getParameters(object);
        std::vector <cci::cci_param_handle>::iterator i;
        for (i = parameters.begin(); i != parameters.end(); ++i) {
            const char *type = cfg.simicsType(*i);
            if (!type)
                continue;

            int key = CciAttribute::define(i->name());
            ProxyAttributeName attribute_name(cfg.simicsName(*i));
            if (attribute_name.transformed()) {
                SIM_LOG_INFO(4, log_object_, Log_Awareness,
                             "CCI Attribute %s is now %s.",
                             i->name(),
                             attribute_name.c_str());
            }

            char *key_arg = NULL;
            key_arg += key;
            SIM_register_typed_attribute(
                cls, attribute_name.c_str(),
                Proxy::getAttribute, key_arg,
                Proxy::setAttribute, key_arg,
                Sim_Attr_Pseudo, cfg.simicsType(*i), 0,
                i->get_description().c_str());
            if (SIM_clear_exception() != SimExc_No_Exception) {
                SIM_LOG_ERROR(log_object_, Log_Awareness,
                             "Failed to register CCI attribute: '%s'",
                             attribute_name.c_str());
            }
        }
#endif
    }
    virtual void registerInterfaces(sc_core::sc_object *object,
                                    conf_class_t *cls) const {
        registerInterface<iface::ScObjectSimicsAdapter<TProxy> >(cls);
        if (dynamic_cast<sc_core::sc_module *>(object)) {
            // TODO(ah): replace this with a proper SimicsAdapter class and a
            // working implementation once the interface is used for something
            // more than just type-info.
            static sc_module_interface_t iface = {};
            iface._not_valid = NULL;
            SIM_register_interface(cls, SC_MODULE_INTERFACE, &iface);
        }
    }
    virtual void registerLogGroups(sc_core::sc_object *object,
                                   conf_class_t *cls) const {
    }
    virtual void registerFeatures(sc_core::sc_object *object,
                                  ProxyInterface *proxy) const {
    }
    virtual bool registerClass(std::string sc_kind,
                               const char *class_name) const {
        std::string name = sc_class_attributes.name(class_name);
        conf_class_t *cls = SIM_get_class(name.c_str());
        if (SIM_clear_exception() != SimExc_No_Exception) {
            std::string description = std::string("SystemC ") + sc_kind;
            std::string documentation = std::string("The <class>") +
                name + "</class> class corresponds to the generic " +
                description + " class.";
            cls = createConfClass(NULL, std::move(name), std::move(description),
                                  std::move(documentation));
            if (cls == NULL) {
                // Use (void) to avoid coverity warning "unchecked_return_value"
                (void)SIM_clear_exception();
                return false;
            }

            registerAttributes(NULL, cls);
            registerInterfaces(NULL, cls);
            registerLogGroups(NULL, cls);

            // TODO(ah): hack! should be handled by registerInterfaces(), but
            // since we invoke it with NULL the dynamic cast to sc_module* will
            // always fail and the sc_module interface will never be
            // registered. Not sure why we need to call registerClass(...,
            // "sc_module") from ProxyBuilder::registerProxyClasses(), but we
            // must handle this more gracefully.
            {
                static sc_module_interface_t iface = {};
                iface._not_valid = NULL;
                SIM_register_interface(cls, SC_MODULE_INTERFACE, &iface);
            }
        }

        return true;
    }
    virtual bool isClassRegistered(std::string sc_kind) const {
        std::string name = sc_class_attributes.name(sc_kind.c_str());
        SIM_get_class(name.c_str());
        return SIM_clear_exception() == SimExc_No_Exception;
    }
    void addSuppressProxyBuild(sc_core::sc_object* obj) {
        suppress_proxy_build_.insert(obj);
    }
    template <class A>
    void registerInterface(conf_class_t *cls) const {
        static A adapter;
        SIM_register_interface(cls, adapter.name().c_str(),
                               adapter.cstruct());
    }

  private:
    ClassAttributes sc_class_attributes;
    InstanceAttributes sc_instance_attributes;
    std::set<sc_core::sc_object *> suppress_proxy_build_;
};

}  // namespace awareness
}  // namespace systemc
}  // namespace simics

#endif
