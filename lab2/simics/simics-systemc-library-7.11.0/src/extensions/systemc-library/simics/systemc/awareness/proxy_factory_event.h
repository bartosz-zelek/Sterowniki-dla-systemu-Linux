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

#ifndef SIMICS_SYSTEMC_AWARENESS_PROXY_FACTORY_EVENT_H
#define SIMICS_SYSTEMC_AWARENESS_PROXY_FACTORY_EVENT_H


#include <simics/systemc/awareness/proxy_factory.h>
#include <simics/systemc/awareness/proxy_event.h>
#include <simics/systemc/awareness/sc_event_object.h>
#include <simics/systemc/iface/sc_event_simics_adapter.h>
#include <simics/systemc/iface/instrumentation/provider_controller_simics_adapter.h>

#include <string>

namespace simics {
namespace systemc {
namespace awareness {

class EventInstanceAttributes : public InstanceAttributes {
  public:
    virtual std::string name(sc_core::sc_object *object) const {
        sc_core::sc_event *event = get_event(object);
        if (!event)
            return InstanceAttributes::name(object);

        return event->name();
    }
    virtual std::string basename(sc_core::sc_object *object) const {
        sc_core::sc_event *event = get_event(object);
        if (!event)
            return InstanceAttributes::basename(object);

        return event->basename();
    }
  protected:
    sc_core::sc_event *get_event(sc_core::sc_object *object) const {
        ScEventObject *eventProxy = dynamic_cast<ScEventObject *> (object);
        if (!eventProxy)
            return NULL;

        return eventProxy->get_event();
    }
};

class ProxyFactoryEvent : public ProxyFactory<ProxyEvent> {
  public:
    virtual bool canManufacture(sc_core::sc_object *object) const {
        return std::string("ScEventObject") == object->kind();
    }
    virtual const InstanceAttributesInterface *instanceAttributes(
        sc_core::sc_object *object) const {
        return &instance_attributes_;
    }
    void registerInterfaces(sc_core::sc_object *object,
                            conf_class_t *cls) const {
        ProxyFactory<ProxyEvent>::registerInterfaces(object, cls);
        registerInterface<iface::ScEventSimicsAdapter<ProxyEvent> >(cls);
        registerInterface<
            iface::instrumentation::ProviderControllerSimicsAdapter<
                ProxyEvent> >(cls);
    }

  private:
    EventInstanceAttributes instance_attributes_;
};

}  // namespace awareness
}  // namespace systemc
}  // namespace simics

#endif
