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

#include <simics/base/sim-exception.h>
#include <simics/systemc/internals.h>

#include <simics/systemc/awareness/proxy_create_traverser.h>
#include <simics/systemc/awareness/proxy.h>
#include <simics/systemc/awareness/proxy_interface.h>
#include <simics/systemc/awareness/proxy_name.h>
#include <simics/systemc/awareness/log.h>
#include <simics/systemc/adapter_log_groups.h>

#include <string>

namespace simics {
namespace systemc {
namespace awareness {

ProxyCreateTraverser::ProxyCreateTraverser(
    ConfObjectRef object,
    iface::SimulationInterface *simulation,
    ProxyFactoryInterface *factory,
    ProxyClassRegistry *registry,
    ObjProxyInterfaceMap *links,
    ObjProxyInterfaceMap *new_links)
    : root_(object),
      prefix_(object.name().empty() ? "" : object.name() + "."),
      registry_(registry), links_(links), new_links_(new_links),
      simulation_(simulation), factory_(factory) {
}

void ProxyCreateTraverser::applyOn(sc_core::sc_object *obj) {
    // If a proxy already exists we can exit early, but first we must build any
    // event objects related to the current object.
    ProxyInterface *pi = Proxy::findProxy(obj);
    if (pi) {
        pi->buildEventObjects();
        return;
    }

    conf_class_t *conf_class = registry_->confClass(obj);
    const InstanceAttributesInterface *attr = factory_->instanceAttributes(obj);
    FATAL_ERROR_IF(!attr, "Inheritance from ProxyFactory missing.");

    std::string objname = attr->name(obj);
    ProxyName objname_safe(objname);
    if (objname_safe.transformed()) {
        SIM_LOG_INFO(4, root_, Log_Awareness,
                     "Object %s is now %s.",
                     objname.c_str(), objname_safe.c_str());
    }

    if (obj->get_parent_object() == NULL
        && objname.find('.') != std::string::npos) {
        // Confirmed to be a spec-violation at end-of-elaboration by Philipp
        // Hartmann, though not explicitly mentioned by the IEEE specification.
        SIM_LOG_SPEC_VIOLATION(1, root_, Log_Awareness,
                               "failed to create proxy object for: %s,"
                               " no parent found.", objname.c_str());
        return;
    }

    std::string full_name = prefix_ + objname_safe;
    std::string parent;
    for (std::string::iterator it = full_name.begin(); it != full_name.end();
         ++it) {
        if (*it == '.') {
            SIM_get_object(parent.c_str());
            if (SIM_clear_exception() != SimExc_No_Exception) {
                // Create missing namespace objects
                conf_class_t *ns_cls = SIM_get_class("namespace");
                attr_value_t attrs = SIM_make_attr_list(0);
                SIM_create_object(ns_cls, parent.c_str(), attrs);
                SIM_attr_free(&attrs);
            }
        }

        parent.push_back(*it);
    }

    attr_value_t attrs = SIM_make_attr_list(0);
    conf_object_t *proxy_obj = conf_class ? SIM_create_object(conf_class,
                                                              full_name.c_str(),
                                                              attrs)
                                          : NULL;
    SIM_attr_free(&attrs);

    if (proxy_obj == NULL) {
        // Use (void) to avoid coverity warning "unchecked_return_value"
        (void)SIM_clear_exception();
        SIM_LOG_SPEC_VIOLATION(1, root_, Log_Awareness,
                               "failed to create proxy object for: %s,"
                               " exception calling SIM_create_object(): %s",
                               objname.c_str(), SIM_last_error());
        return;
    }

    ProxyInterface *proxy = dynamic_cast<ProxyInterface*>(
        static_cast<simics::ConfObject *>(SIM_object_data(proxy_obj)));
    FATAL_ERROR_IF(!proxy, "Unable to cast to proxy");
    (*new_links_)[obj] = proxy;
    (*links_)[obj] = proxy;
    factory_->registerFeatures(obj, proxy);
    proxy->init(obj, simulation_);
    proxy->buildEventObjects();
}

void ProxyCreateTraverser::done() {
}

}  // namespace awareness
}  // namespace systemc
}  // namespace simics
