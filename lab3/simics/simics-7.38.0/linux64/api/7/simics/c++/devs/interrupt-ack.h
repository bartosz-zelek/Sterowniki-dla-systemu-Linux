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

#ifndef SIMICS_CPP_DEVS_INTERRUPT_ACK_H
#define SIMICS_CPP_DEVS_INTERRUPT_ACK_H

#include "simics/devs/interrupt-ack.h"

#include <simics/detail/conf-object-util.h>  // get_interface
#include <simics/iface/interface-info.h>

namespace simics {
namespace iface {

class InterruptAckInterface {
  public:
    using ctype = interrupt_ack_interface_t;

    // Function override and implemented by user
    virtual void raise_interrupt(interrupt_ack_fn_t cb, conf_object_t *ack_obj) = 0;
    virtual void lower_interrupt(interrupt_ack_fn_t cb) = 0;

    // Function convert C interface call to C++ interface call
    class FromC {
      public:
        static void raise_interrupt(conf_object_t *obj, interrupt_ack_fn_t cb, conf_object_t *ack_obj) {
            detail::get_interface<InterruptAckInterface>(obj)->raise_interrupt(cb, ack_obj);
        }
        static void lower_interrupt(conf_object_t *obj, interrupt_ack_fn_t cb) {
            detail::get_interface<InterruptAckInterface>(obj)->lower_interrupt(cb);
        }
    };

    // Function convert C++ interface call to C interface call
    class ToC {
      public:
        ToC() : obj_(nullptr), iface_(nullptr) {}
        ToC(conf_object_t *obj, const InterruptAckInterface::ctype *iface)
            : obj_(obj), iface_(iface) {}

        void raise_interrupt(interrupt_ack_fn_t cb, conf_object_t *ack_obj) const {
            iface_->raise_interrupt(obj_, cb, ack_obj);
        }
        void lower_interrupt(interrupt_ack_fn_t cb) const {
            iface_->lower_interrupt(obj_, cb);
        }

        const InterruptAckInterface::ctype *get_iface() const {
            return iface_;
        }

      private:
        conf_object_t *obj_;
        const InterruptAckInterface::ctype *iface_;
    };

    class Info : public InterfaceInfo {
      public:
        // InterfaceInfo
        std::string name() const override { return INTERRUPT_ACK_INTERFACE; }
        const interface_t *cstruct() const override {
            static constexpr InterruptAckInterface::ctype funcs {
                FromC::raise_interrupt,
                FromC::lower_interrupt,
            };
            return &funcs;
        }
    };
};

class InterruptCpuInterface {
  public:
    using ctype = interrupt_cpu_interface_t;

    // Function override and implemented by user
    virtual int ack() = 0;

    // Function convert C interface call to C++ interface call
    class FromC {
      public:
        static int ack(conf_object_t *obj) {
            return detail::get_interface<InterruptCpuInterface>(obj)->ack();
        }
    };

    // Function convert C++ interface call to C interface call
    class ToC {
      public:
        ToC() : obj_(nullptr), iface_(nullptr) {}
        ToC(conf_object_t *obj, const InterruptCpuInterface::ctype *iface)
            : obj_(obj), iface_(iface) {}

        int ack() const {
            return iface_->ack(obj_);
        }

        const InterruptCpuInterface::ctype *get_iface() const {
            return iface_;
        }

      private:
        conf_object_t *obj_;
        const InterruptCpuInterface::ctype *iface_;
    };

    class Info : public InterfaceInfo {
      public:
        // InterfaceInfo
        std::string name() const override { return INTERRUPT_CPU_INTERFACE; }
        const interface_t *cstruct() const override {
            static constexpr InterruptCpuInterface::ctype funcs {
                FromC::ack,
            };
            return &funcs;
        }
    };
};

}  // namespace iface
}  // namespace simics

#endif
