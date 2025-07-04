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

#ifndef SIMICS_CPP_ARCH_MIPS_INSTRUMENTATION_H
#define SIMICS_CPP_ARCH_MIPS_INSTRUMENTATION_H

#include "simics/arch/mips-instrumentation.h"

#include <simics/detail/conf-object-util.h>  // get_interface
#include <simics/iface/interface-info.h>

namespace simics {
namespace iface {

class MipsExceptionQueryInterface {
  public:
    using ctype = mips_exception_query_interface_t;

    // Function override and implemented by user
    virtual logical_address_t return_pc(exception_handle_t *handle) = 0;

    // Function convert C interface call to C++ interface call
    class FromC {
      public:
        static logical_address_t return_pc(conf_object_t *cpu, exception_handle_t *handle) {
            return detail::get_interface<MipsExceptionQueryInterface>(cpu)->return_pc(handle);
        }
    };

    // Function convert C++ interface call to C interface call
    class ToC {
      public:
        ToC() : obj_(nullptr), iface_(nullptr) {}
        ToC(conf_object_t *obj, const MipsExceptionQueryInterface::ctype *iface)
            : obj_(obj), iface_(iface) {}

        logical_address_t return_pc(exception_handle_t *handle) const {
            return iface_->return_pc(obj_, handle);
        }

        const MipsExceptionQueryInterface::ctype *get_iface() const {
            return iface_;
        }

      private:
        conf_object_t *obj_;
        const MipsExceptionQueryInterface::ctype *iface_;
    };

    class Info : public InterfaceInfo {
      public:
        // InterfaceInfo
        std::string name() const override { return MIPS_EXCEPTION_QUERY_INTERFACE; }
        const interface_t *cstruct() const override {
            static constexpr MipsExceptionQueryInterface::ctype funcs {
                FromC::return_pc,
            };
            return &funcs;
        }
    };
};

}  // namespace iface
}  // namespace simics

#endif
