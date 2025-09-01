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

#include <simics/systemc/internals.h>
#include <simics/systemc/awareness/proxy.h>

#if INTC_EXT
#include <sysc/kernel/sc_object_manager.h>
#endif

#include <iterator>
#include <limits>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace simics {
namespace systemc {
namespace awareness {

std::unordered_map<sc_core::sc_object *, ProxyInterface *> Proxy::proxies_;
std::unordered_map<sc_core::sc_interface *,
                   ProxyInterface *> Proxy::proxies_by_iface_;
std::unordered_map<sc_core::sc_event *, ScEventObject *> Proxy::sc_events_;

Proxy::Proxy(ConfObjectRef o)
    : ConfObject(o), object_(NULL), iface_(NULL), attributes_(NULL) {
}

void Proxy::init(sc_core::sc_object *obj,
                 iface::SimulationInterface *simulation) {
    object_ = obj;
    setSimulation(simulation);
    proxies_[object_] = this;
    iface_ = dynamic_cast<sc_core::sc_interface *>(obj);
    if (iface_)
        proxies_by_iface_[iface_] = this;

    for (auto& i : features_) {
        i.second->init(this, this);
    }
}

void Proxy::allProxiesInitialized() {
}

void Proxy::set_attributes(Attributes *attributes) {
    attributes_ = attributes;
}

iface::SimulationInterface *Proxy::simulation() {
    return this;
}

void Proxy::addFeature(ProxyFeatureInterface *feature) {
    features_[*feature->type()] = feature;
}

ProxyFeatureInterface *Proxy::feature(ClassType type) {
    auto i = features_.find(type);
    if (i == features_.end())
        return NULL;

    return i->second;
}

void Proxy::simulationStarted() {
}

void Proxy::simulationEnded() {
}

void Proxy::resizeExtensions(tlm::tlm_generic_payload *payload) const {
    payload->resize_extensions();
}

attr_value_t Proxy::getAttribute(lang_void *ptr, conf_object_t *obj,
                                 attr_value_t *idx) {
    uintptr_t key = reinterpret_cast<intptr_t>(ptr);
    Proxy *o = static_cast<Proxy*>(SIM_object_data(obj));
    attr_value_t result = o->attributes_->attribute<Attribute>(key)->get();
    return result;
}

set_error_t Proxy::setAttribute(lang_void *ptr, conf_object_t *obj,
                                attr_value_t *val,
                                attr_value_t *idx) {
    uintptr_t key = reinterpret_cast<intptr_t>(ptr);
    Proxy *o = static_cast<Proxy*>(SIM_object_data(obj));
    set_error_t result = o->attributes_->attribute<Attribute>(key)->set(val);
    return result;
}

std::vector<std::string> splitLines(std::string lines) {
    std::vector<std::string> ret;
    int state = 0;
    std::string s;
    std::string ws;
    for (std::string::iterator it = lines.begin(); it != lines.end(); ++it) {
        switch (state) {
            case 0: {
                // Skip initial whitespaces
                if (*it == '\r' || *it == '\n' || *it == ' ')
                    continue;

                // Build new line
                s.push_back(*it);
                state = 1;
            }
            break;

            case 1: {
                if (*it == '\r' || *it == '\n') {
                    // Add current line to result, without leading spaces
                    if (!s.empty())
                        ret.push_back(s);

                    s.clear();
                    ws.clear();
                    state = 0;
                    // Store space when in the middle of phrase
                } else if (*it == ' ') {
                    ws.push_back(*it);
                } else {
                    // Add stored spaces to phrase
                    if (!ws.empty()) {
                        s += ws;
                        ws.clear();
                    }

                    s.push_back(*it);
                }
            }
            break;
        }
    }

    // Add current line to result
    if (!s.empty())
        ret.push_back(s);

    return ret;
}

std::vector<std::string> Proxy::sc_print() const {
    std::ostringstream oss;
    object_->print(oss);
    return splitLines(oss.str());
}

std::vector<std::string> Proxy::sc_dump() const {
    std::ostringstream oss;
    object_->dump(oss);
    return splitLines(oss.str());
}

attr_value_t Proxy::sc_kind() const {
    return SIM_make_attr_string(object_->kind());
}

const char *Proxy::sc_name() const {
    return object_->name();
}

void Proxy::breakSimulation() {
#if INTC_EXT
    sc_core::sc_pause();
#else
    SIM_break_simulation("");
#endif
}

ConfObjectRef Proxy::simics_obj() {
    return ConfObject::obj();
}

sc_core::sc_object *Proxy::systemc_obj() {
    return object_;
}

void Proxy::buildEventObjects() {
#if INTC_EXT
    const std::vector<sc_core::sc_event*> &events = object_->get_child_events();
    std::vector<sc_core::sc_event *>::const_iterator i;
    context()->get_object_manager()->hierarchy_push(
#ifdef SYSTEMC_3_0_0
            dynamic_cast<sc_core::sc_object_host *>(object_));
#else
            object_);
#endif
    for (i = events.begin(); i != events.end(); ++i) {
        if (sc_events_.find(*i) == sc_events_.end()) {
            ScEventObject *obj = new ScEventObject(*i);
            events_.push_back(obj);
            std::pair<sc_core::sc_event *, ScEventObject *> pair(*i, obj);
            sc_events_.insert(pair);
        }
    }
    context()->get_object_manager()->hierarchy_pop();
#endif
}

ScEventObject *Proxy::findScEventObject(sc_core::sc_event *event) {
    auto i = sc_events_.find(event);
    if (i == sc_events_.end())
        return NULL;

    return i->second;
}

ProxyInterface *Proxy::findProxy(sc_core::sc_object *obj) {
    auto i = proxies_.find(obj);
    if (i == proxies_.end())
        return NULL;

    return i->second;
}

ProxyInterface *Proxy::findProxy(sc_core::sc_interface *iface) {
    auto i = proxies_by_iface_.find(iface);
    if (i == proxies_by_iface_.end())
        return NULL;

    return i->second;
}

Proxy::~Proxy() {
    proxies_.erase(object_);
    if (iface_)
        proxies_by_iface_.erase(iface_);

    for (auto& i : features_) {
        delete i.second;
    }
}

}  // namespace awareness
}  // namespace systemc
}  // namespace simics
