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

#ifndef SIMICS_SYSTEMC_IFACE_SIGNAL_SIMICS_ADAPTER_H
#define SIMICS_SYSTEMC_IFACE_SIGNAL_SIMICS_ADAPTER_H

#include <simics/devs/signal.h>
#include <simics/systemc/iface/signal_interface.h>
#include <simics/systemc/iface/simics_adapter.h>

#include <string>
#include <vector>

namespace simics {
namespace systemc {
namespace iface {

/** Adapter for Simics signal interface. */
template<typename TBase, typename TInterface = SignalInterface>
class SignalSimicsAdapter : public SimicsAdapter<signal_interface_t> {
  public:
    SignalSimicsAdapter()
        : SimicsAdapter<signal_interface_t>(
            SIGNAL_INTERFACE, init_iface()) {
    }

  protected:
    static void raise(conf_object_t *obj) {
        return adapter<TBase, TInterface>(obj)->raise();
    }
    static void lower(conf_object_t *obj) {
        return adapter<TBase, TInterface>(obj)->lower();
    }

  private:
    std::vector<std::string> description(conf_object_t *obj,
                                         DescriptionType type) {
        return descriptionBase<TBase, TInterface>(obj, type);
    }
    signal_interface_t init_iface() {
        signal_interface_t iface = {};
        iface.signal_raise = raise;
        iface.signal_lower = lower;
        return iface;
    }
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
