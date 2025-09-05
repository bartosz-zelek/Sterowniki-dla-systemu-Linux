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

#ifndef EXTENSIONS_X86_TSC_CYCLES_INTERFACE_TSC_CYCLES_INTERFACE_CC_H
#define EXTENSIONS_X86_TSC_CYCLES_INTERFACE_TSC_CYCLES_INTERFACE_CC_H

#include "tsc-cycles-interface.h"

#include <simics/iface/interface-info.h>
#include <simics/utility.h>  // get_interface

namespace simics {
namespace iface {

class X86TscCyclesInterface {
  public:
    using ctype = x86_tsc_cycles_interface_t;

    // Function override and implemented by user
    virtual uint64 tsc_to_cycles(uint64 tsc) = 0;
    virtual uint64 tsc_from_cycles(uint64 cycles) = 0;

    // Function convert C interface call to C++ interface call
    class FromC {
      public:
        static uint64 tsc_to_cycles(conf_object_t *obj, uint64 tsc) {
            return get_interface<X86TscCyclesInterface>(obj)->tsc_to_cycles(tsc);
        }
        static uint64 tsc_from_cycles(conf_object_t *obj, uint64 cycles) {
            return get_interface<X86TscCyclesInterface>(obj)->tsc_from_cycles(cycles);
        }
    };

    // Function convert C++ interface call to C interface call
    class ToC {
      public:
        ToC() : obj_(nullptr), iface_(nullptr) {}
        ToC(conf_object_t *obj, const X86TscCyclesInterface::ctype *iface)
            : obj_(obj), iface_(iface) {}

        uint64 tsc_to_cycles(uint64 tsc) const {
            return iface_->tsc_to_cycles(obj_, tsc);
        }
        uint64 tsc_from_cycles(uint64 cycles) const {
            return iface_->tsc_from_cycles(obj_, cycles);
        }

        const X86TscCyclesInterface::ctype *get_iface() const {
            return iface_;
        }

      private:
        conf_object_t *obj_;
        const X86TscCyclesInterface::ctype *iface_;
    };

    class Info : public InterfaceInfo {
      public:
        // InterfaceInfo
        std::string name() const override { return X86_TSC_CYCLES_INTERFACE; }
        const interface_t *cstruct() const override {
            static constexpr X86TscCyclesInterface::ctype funcs {
                FromC::tsc_to_cycles,
                FromC::tsc_from_cycles,
            };
            return &funcs;
        }
    };
};

}  // namespace iface
}  // namespace simics

#endif // EXTENSIONS_X86_TSC_CYCLES_INTERFACE_TSC_CYCLES_INTERFACE_CC_H
