// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2018 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_IFACE_EVENT_DELTA_SIMICS_ADAPTER_H
#define SIMICS_SYSTEMC_IFACE_EVENT_DELTA_SIMICS_ADAPTER_H

#include <simics/model-iface/event-delta.h>
#include <simics/systemc/iface/event_delta_interface.h>
#include <simics/systemc/iface/simics_adapter.h>

#include <string>
#include <vector>

namespace simics {
namespace systemc {
namespace iface {

/** \internal */
template<typename TBase, typename TInterface = EventDeltaInterface>
class EventDeltaSimicsAdapter : public SimicsAdapter<event_delta_interface_t> {
  public:
    EventDeltaSimicsAdapter()
        : SimicsAdapter<event_delta_interface_t>(EVENT_DELTA_INTERFACE,
                                                 init_iface()) {
    }

  protected:
    static ::uint64 set_delta(conf_object_t *NOTNULL obj,
                              conf_object_t *NOTNULL event_handler_obj,
                              const event_class_t *next_event_ec,
                              ::uint64 delta) {
        return adapter<TBase, TInterface>(obj)->set_delta(event_handler_obj,
                                                          next_event_ec,
                                                          delta);
    }
    static ::uint64 get_delta(conf_object_t *NOTNULL obj,
                              conf_object_t *NOTNULL event_handler_obj) {
        return adapter<TBase, TInterface>(obj)->get_delta(event_handler_obj);
    }

  private:
    std::vector<std::string> description(conf_object_t *obj,
                                         DescriptionType type) {
        return descriptionBase<TBase, TInterface>(obj, type);
    }
    event_delta_interface_t init_iface() {
        event_delta_interface_t iface = {};
        iface.set_delta = set_delta;
        iface.get_delta = get_delta;
        return iface;
    }
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
