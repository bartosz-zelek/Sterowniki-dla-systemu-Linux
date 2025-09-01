// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2023 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef EXTENSIONS_X86_RAR_INTERFACE_RAR_INTERFACE_CC_H 
#define EXTENSIONS_X86_RAR_INTERFACE_RAR_INTERFACE_CC_H

#include "rar-interface.h"

#include <simics/iface/interface-info.h>
#include <simics/utility.h>  // get_interface

namespace simics {
namespace iface {

class X86RarInterruptInterface {
  public:
    using ctype = x86_rar_interrupt_interface_t;

    // Function override and implemented by user
    virtual bool is_rar_requested() = 0;
    virtual int ack_rar() = 0;

    // Function convert C interface call to C++ interface call
    class FromC {
      public:
        static bool is_rar_requested(conf_object_t *obj) {
            return get_interface<X86RarInterruptInterface>(obj)->is_rar_requested();
        }
        static int ack_rar(conf_object_t *obj) {
            return get_interface<X86RarInterruptInterface>(obj)->ack_rar();
        }
    };

    // Function convert C++ interface call to C interface call
    class ToC {
      public:
        ToC() : obj_(nullptr), iface_(nullptr) {}
        ToC(conf_object_t *obj, const X86RarInterruptInterface::ctype *iface)
            : obj_(obj), iface_(iface) {}

        bool is_rar_requested() const {
            return iface_->is_rar_requested(obj_);
        }
        int ack_rar() const {
            return iface_->ack_rar(obj_);
        }

        const X86RarInterruptInterface::ctype *get_iface() const {
            return iface_;
        }

      private:
        conf_object_t *obj_;
        const X86RarInterruptInterface::ctype *iface_;
    };

    class Info : public InterfaceInfo {
      public:
        // InterfaceInfo
        std::string name() const override { return X86_RAR_INTERRUPT_INTERFACE; }
        const interface_t *cstruct() const override {
            static constexpr X86RarInterruptInterface::ctype funcs {
                FromC::is_rar_requested,
                FromC::ack_rar,
            };
            return &funcs;
        }
    };
};

}  // namespace iface
}  // namespace simics

#endif // EXTENSIONS_X86_RAR_INTERFACE_RAR_INTERFACE_CC_H
