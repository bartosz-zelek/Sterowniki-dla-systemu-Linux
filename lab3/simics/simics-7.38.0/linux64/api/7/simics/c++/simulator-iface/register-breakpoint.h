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

#ifndef SIMICS_CPP_SIMULATOR_IFACE_REGISTER_BREAKPOINT_H
#define SIMICS_CPP_SIMULATOR_IFACE_REGISTER_BREAKPOINT_H

#include "simics/simulator-iface/register-breakpoint.h"

#include <simics/detail/conf-object-util.h>  // get_interface
#include <simics/iface/interface-info.h>

namespace simics {
namespace iface {

class RegisterBreakpointInterface {
  public:
    using ctype = register_breakpoint_interface_t;

    // Function override and implemented by user
    virtual int add_breakpoint(const char *reg_name, uint64 value, uint64 mask, bool break_upon_change) = 0;
    virtual bool remove_breakpoint(int id) = 0;
    virtual attr_value_t get_breakpoints() = 0;

    // Function convert C interface call to C++ interface call
    class FromC {
      public:
        static int add_breakpoint(conf_object_t *obj, const char *reg_name, uint64 value, uint64 mask, bool break_upon_change) {
            return detail::get_interface<RegisterBreakpointInterface>(obj)->add_breakpoint(reg_name, value, mask, break_upon_change);
        }
        static bool remove_breakpoint(conf_object_t *obj, int id) {
            return detail::get_interface<RegisterBreakpointInterface>(obj)->remove_breakpoint(id);
        }
        static attr_value_t get_breakpoints(conf_object_t *obj) {
            return detail::get_interface<RegisterBreakpointInterface>(obj)->get_breakpoints();
        }
    };

    // Function convert C++ interface call to C interface call
    class ToC {
      public:
        ToC() : obj_(nullptr), iface_(nullptr) {}
        ToC(conf_object_t *obj, const RegisterBreakpointInterface::ctype *iface)
            : obj_(obj), iface_(iface) {}

        int add_breakpoint(const char *reg_name, uint64 value, uint64 mask, bool break_upon_change) const {
            return iface_->add_breakpoint(obj_, reg_name, value, mask, break_upon_change);
        }
        bool remove_breakpoint(int id) const {
            return iface_->remove_breakpoint(obj_, id);
        }
        attr_value_t get_breakpoints() const {
            return iface_->get_breakpoints(obj_);
        }

        const RegisterBreakpointInterface::ctype *get_iface() const {
            return iface_;
        }

      private:
        conf_object_t *obj_;
        const RegisterBreakpointInterface::ctype *iface_;
    };

    class Info : public InterfaceInfo {
      public:
        // InterfaceInfo
        std::string name() const override { return REGISTER_BREAKPOINT_INTERFACE; }
        const interface_t *cstruct() const override {
            static constexpr RegisterBreakpointInterface::ctype funcs {
                FromC::add_breakpoint,
                FromC::remove_breakpoint,
                FromC::get_breakpoints,
            };
            return &funcs;
        }
    };
};

}  // namespace iface
}  // namespace simics

#endif
