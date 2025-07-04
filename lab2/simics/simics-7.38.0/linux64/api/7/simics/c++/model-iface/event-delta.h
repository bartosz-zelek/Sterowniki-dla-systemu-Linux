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

// This file is generated by the script bin/gen-cc-interface

#ifndef SIMICS_CPP_MODEL_IFACE_EVENT_DELTA_H
#define SIMICS_CPP_MODEL_IFACE_EVENT_DELTA_H

#include "simics/model-iface/event-delta.h"

#include <simics/detail/conf-object-util.h>  // get_interface
#include <simics/iface/interface-info.h>

namespace simics {
namespace iface {

class EventDeltaInterface {
  public:
    using ctype = event_delta_interface_t;

    // Function override and implemented by user
    virtual uint64 set_delta(conf_object_t *event_handler_obj, const event_class_t *next_event_ec, uint64 delta) = 0;
    virtual uint64 get_delta(conf_object_t *event_handler_obj) = 0;

    // Function convert C interface call to C++ interface call
    class FromC {
      public:
        static uint64 set_delta(conf_object_t *obj, conf_object_t *event_handler_obj, const event_class_t *next_event_ec, uint64 delta) {
            return detail::get_interface<EventDeltaInterface>(obj)->set_delta(event_handler_obj, next_event_ec, delta);
        }
        static uint64 get_delta(conf_object_t *obj, conf_object_t *event_handler_obj) {
            return detail::get_interface<EventDeltaInterface>(obj)->get_delta(event_handler_obj);
        }
    };

    // Function convert C++ interface call to C interface call
    class ToC {
      public:
        ToC() : obj_(nullptr), iface_(nullptr) {}
        ToC(conf_object_t *obj, const EventDeltaInterface::ctype *iface)
            : obj_(obj), iface_(iface) {}

        uint64 set_delta(conf_object_t *event_handler_obj, const event_class_t *next_event_ec, uint64 delta) const {
            return iface_->set_delta(obj_, event_handler_obj, next_event_ec, delta);
        }
        uint64 get_delta(conf_object_t *event_handler_obj) const {
            return iface_->get_delta(obj_, event_handler_obj);
        }

        const EventDeltaInterface::ctype *get_iface() const {
            return iface_;
        }

      private:
        conf_object_t *obj_;
        const EventDeltaInterface::ctype *iface_;
    };

    class Info : public InterfaceInfo {
      public:
        // InterfaceInfo
        std::string name() const override { return EVENT_DELTA_INTERFACE; }
        const interface_t *cstruct() const override {
            static constexpr EventDeltaInterface::ctype funcs {
                FromC::set_delta,
                FromC::get_delta,
            };
            return &funcs;
        }
    };
};

class EventHandlerInterface {
  public:
    using ctype = event_handler_interface_t;

    // Function override and implemented by user
    virtual bool handle_event() = 0;
    virtual void stop() = 0;

    // Function convert C interface call to C++ interface call
    class FromC {
      public:
        static bool handle_event(conf_object_t *obj) {
            return detail::get_interface<EventHandlerInterface>(obj)->handle_event();
        }
        static void stop(conf_object_t *obj) {
            detail::get_interface<EventHandlerInterface>(obj)->stop();
        }
    };

    // Function convert C++ interface call to C interface call
    class ToC {
      public:
        ToC() : obj_(nullptr), iface_(nullptr) {}
        ToC(conf_object_t *obj, const EventHandlerInterface::ctype *iface)
            : obj_(obj), iface_(iface) {}

        bool handle_event() const {
            return iface_->handle_event(obj_);
        }
        void stop() const {
            iface_->stop(obj_);
        }

        const EventHandlerInterface::ctype *get_iface() const {
            return iface_;
        }

      private:
        conf_object_t *obj_;
        const EventHandlerInterface::ctype *iface_;
    };

    class Info : public InterfaceInfo {
      public:
        // InterfaceInfo
        std::string name() const override { return EVENT_HANDLER_INTERFACE; }
        const interface_t *cstruct() const override {
            static constexpr EventHandlerInterface::ctype funcs {
                FromC::handle_event,
                FromC::stop,
            };
            return &funcs;
        }
    };
};

}  // namespace iface
}  // namespace simics

#endif
