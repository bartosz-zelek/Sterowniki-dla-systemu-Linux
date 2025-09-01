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

#include <simics/systemc/awareness/proxy_signal.h>
#include <simics/systemc/iface/instrumentation/signal_action_interface.h>
#include <simics/systemc/internals.h>

#if INTC_EXT
#include <sysc/intc/communication/sc_signal_callback.h>
#endif

#include <sstream>
#include <string>
#include <vector>

namespace simics {
namespace systemc {
namespace awareness {

#if INTC_EXT
class SignalCallbackAdapter : public intc::sc_signal_callback_interface {
  public:
    SignalCallbackAdapter(sc_core::sc_object *obj,
                          SignalCallbackInterface *callback)
        : callback_(callback), attribute_(this) {
        obj->add_attribute(attribute_);
        name_ = obj->name();
    }
    virtual ~SignalCallbackAdapter() {
        sc_core::sc_object *object = sc_core::sc_find_object(name_.c_str());
        if (object)
            object->remove_attribute(attribute_.name());
    }
    virtual void callback(const sc_core::sc_object& signal_object) {
        callback_->callback(signal_object);
    }

  private:
    SignalCallbackInterface *callback_;
    intc::sc_signal_callback attribute_;
    std::string name_;
};
#endif

ProxySignal::ProxySignal(ConfObjectRef o)
    : Proxy(o), SignalReader(this), SignalWriter(this), adapter_(NULL) {}

ProxySignal::~ProxySignal() {
#if INTC_EXT
    delete adapter_;
#endif
}

void ProxySignal::init(sc_core::sc_object *obj,
                       SimulationInterface *simulation) {
    Proxy::init(obj, simulation);
#if INTC_EXT
    adapter_ = new SignalCallbackAdapter(obj, this);
#endif
}

void ProxySignal::callback(const sc_core::sc_object &signal_object) {
    DISPATCH_TOOL_CHAIN_THIS(iface::instrumentation::SignalActionInterface,
                             fired, this);

    std::vector<SignalCallbackInterface *>::iterator i;
    for (i = callbacks_.begin(); i != callbacks_.end(); ++i)
        (*i)->callback(signal_object);
}

void ProxySignal::add_callback(SignalCallbackInterface *callback) {
    callbacks_.push_back(callback);
}

}  // namespace awareness
}  // namespace systemc
}  // namespace simics
