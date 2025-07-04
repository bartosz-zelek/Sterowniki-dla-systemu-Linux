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

#ifndef SIMICS_CPP_MODEL_IFACE_REGISTER_VIEW_READ_ONLY_H
#define SIMICS_CPP_MODEL_IFACE_REGISTER_VIEW_READ_ONLY_H

#include "simics/model-iface/register-view-read-only.h"

#include <simics/detail/conf-object-util.h>  // get_interface
#include <simics/iface/interface-info.h>

namespace simics {
namespace iface {

class RegisterViewReadOnlyInterface {
  public:
    using ctype = register_view_read_only_interface_t;

    // Function override and implemented by user
    virtual bool is_read_only(unsigned reg) = 0;

    // Function convert C interface call to C++ interface call
    class FromC {
      public:
        static bool is_read_only(conf_object_t *obj, unsigned reg) {
            return detail::get_interface<RegisterViewReadOnlyInterface>(obj)->is_read_only(reg);
        }
    };

    // Function convert C++ interface call to C interface call
    class ToC {
      public:
        ToC() : obj_(nullptr), iface_(nullptr) {}
        ToC(conf_object_t *obj, const RegisterViewReadOnlyInterface::ctype *iface)
            : obj_(obj), iface_(iface) {}

        bool is_read_only(unsigned reg) const {
            return iface_->is_read_only(obj_, reg);
        }

        const RegisterViewReadOnlyInterface::ctype *get_iface() const {
            return iface_;
        }

      private:
        conf_object_t *obj_;
        const RegisterViewReadOnlyInterface::ctype *iface_;
    };

    class Info : public InterfaceInfo {
      public:
        // InterfaceInfo
        std::string name() const override { return REGISTER_VIEW_READ_ONLY_INTERFACE; }
        const interface_t *cstruct() const override {
            static constexpr RegisterViewReadOnlyInterface::ctype funcs {
                FromC::is_read_only,
            };
            return &funcs;
        }
    };
};

}  // namespace iface
}  // namespace simics

#endif
