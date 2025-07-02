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

#ifndef SIMICS_SYSTEMC_AWARENESS_PROXY_H
#define SIMICS_SYSTEMC_AWARENESS_PROXY_H

#include <simics/cc-api.h>


#include <simics/systemc/awareness/attributes.h>
#include <simics/systemc/awareness/proxy_interface.h>
#include <simics/systemc/awareness/sc_event_object.h>
#include <simics/systemc/iface/sc_object_interface.h>
#include <simics/systemc/simulation_interface_proxy.h>
#include <systemc>
#include <tlm>

#include <string>
#include <unordered_map>
#include <map>
#include <vector>

namespace simics {
namespace systemc {
namespace awareness {

class Proxy : public simics::ConfObject, public ProxyInterface,
              public simics::systemc::iface::ScObjectInterface,
              public SimulationInterfaceProxy {
  public:
    explicit Proxy(simics::ConfObjectRef o);
    virtual void init(sc_core::sc_object *obj,
                      iface::SimulationInterface *simulation);
    virtual void allProxiesInitialized();
    virtual void set_attributes(Attributes *attributes);
    static attr_value_t getAttribute(lang_void *ptr, conf_object_t *obj,
                                     attr_value_t *idx);
    static set_error_t setAttribute(lang_void *ptr, conf_object_t *obj,
                                    attr_value_t *val,
                                    attr_value_t *idx);
    std::vector<std::string> sc_print() const;
    std::vector<std::string> sc_dump() const;
    attr_value_t sc_kind() const;
    const char *sc_name() const;
    void breakSimulation();
    ConfObjectRef simics_obj();
    sc_core::sc_object *systemc_obj();
    void buildEventObjects();
    virtual iface::SimulationInterface *simulation();
    virtual void addFeature(ProxyFeatureInterface *feature);
    virtual ProxyFeatureInterface *feature(ClassType type);
    virtual void simulationStarted();
    virtual void simulationEnded();
    virtual void resizeExtensions(tlm::tlm_generic_payload *payload) const;
    template <class T> T *feature() {
        return dynamic_cast<T *> (feature(ClassType::typeForClass<T>()));
    }
    static ProxyInterface *findProxy(sc_core::sc_object *obj);
    static ProxyInterface *findProxy(sc_core::sc_interface *iface);
    static ScEventObject *findScEventObject(sc_core::sc_event *event);
    virtual ~Proxy();

  protected:
    sc_core::sc_object *object_;
    sc_core::sc_interface *iface_;

  private:
    Attributes *attributes_;
    std::vector<ScEventObject *> events_;
    static std::unordered_map<sc_core::sc_object *, ProxyInterface *> proxies_;
    static std::unordered_map<sc_core::sc_event *, ScEventObject *> sc_events_;
    static std::unordered_map<sc_core::sc_interface *, ProxyInterface *>
        proxies_by_iface_;
    std::map<ClassType, ProxyFeatureInterface *> features_;
};

}  // namespace awareness
}  // namespace systemc
}  // namespace simics

#endif
