// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2016 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <simics/systemc/awareness/proxy_signal.h>
#include <simics/systemc/awareness/proxy_signal_port.h>
#include <simics/systemc/sc_signal_access_registry.h>
#include <simics/systemc/sim_context_proxy.h>
#include <simics/systemc/internals.h>
#include <simics/systemc/iface/instrumentation/signal_port_action_interface.h>

#if INTC_EXT
#include <sysc/communication/sc_port_int.h>  // access to internal structs
#endif

#include <string>
#include <vector>

namespace simics {
namespace systemc {
namespace awareness {

using sc_core::sc_port_base;
using sc_core::sc_object;

ProxySignalPort::ProxySignalPort(ConfObjectRef o)
    : ProxyPort(o), SignalReader(this), SignalWriter(this) {
}

void ProxySignalPort::allProxiesInitialized() {
    sc_port_base *base = dynamic_cast<sc_port_base *>(object_);
    if (!base)
        return;

    sc_core::sc_interface *iface = base->get_interface();
    if (!iface)
        return;

    sc_core::sc_bind_info *info = sc_core::simContextProxy::get_bind_info(base);
    if (!info)
        return;

#if INTC_EXT
    std::vector<sc_core::sc_bind_elem *>::iterator i;
    for (i = info->vec.begin(); i != info->vec.end(); ++i) {
        sc_core::sc_interface *iface = (*i)->iface;
        if (!iface)
            continue;

        signals_.push_back(iface);
        ProxyInterface *proxy = findProxy(dynamic_cast<sc_object *>(iface));
        ProxySignal *signal = dynamic_cast<ProxySignal *>(proxy);
        if (signal)
            signal->add_callback(this);
    }
#endif
}

void ProxySignalPort::callback(const sc_object &signal_object) {
    DISPATCH_TOOL_CHAIN_THIS(iface::instrumentation::SignalPortActionInterface,
                             signal_port_value_update,
                             this,
                             const_cast<sc_object *>(&signal_object));
}

}  // namespace awareness
}  // namespace systemc
}  // namespace simics
