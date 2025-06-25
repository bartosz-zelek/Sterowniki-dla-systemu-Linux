// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2025 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include "simics/event.h"

#include <string>

namespace simics {

EventInfo::EventInfo(const std::string &name, event_class_flag_t flags,
                     event_class_t **ev, ev_callback callback,
                     ev_destroy destroy, ev_value_getter get_value,
                     ev_value_setter set_value, ev_describe describe)
    : name_(name), flags_(flags), ev_(ev), callback_(callback),
      destroy_(destroy), get_value_(get_value), set_value_(set_value),
      describe_(describe) {
    if (name.empty()) {
        throw std::invalid_argument {
            "Event name cannot be empty"
        };
    }
    if (ev == nullptr) {
        throw std::invalid_argument {
            "Pointer of event class pointer for event " + name + \
            " is missing"
        };
    }
    if (callback == nullptr) {
        throw std::invalid_argument {
            "Callback function for event " + name + " is missing"
        };
    }
}

EventInfo::EventInfo(const std::string &name, event_class_t **ev,
                     ev_callback callback)
    : EventInfo(name, Sim_EC_No_Flags, ev, callback,
                nullptr, nullptr, nullptr, nullptr) {
}

Event::Event(ConfObject *obj, event_class_t *ev) : obj_(obj), ev_(ev) {
    if (obj == nullptr) {
        throw std::invalid_argument {
            "Device object can't be NULL"
        };
    }

    if (ev == nullptr) {
        throw std::invalid_argument {
            "Event is not registered yet. Call add() from the"
            " device class"
        };
    }
}

void Event::destroy(void *data) {}

attr_value_t Event::get_value(void *data) {
    return SIM_make_attr_nil();
}

void *Event::set_value(attr_value_t value) {
    return nullptr;
}

char *Event::describe(void *data) const {
    return nullptr;
}

Event::operator event_class_t *() const {
    return ev_;
}

const char *Event::name() const {
    return ev_->name;
}

}  // namespace simics
