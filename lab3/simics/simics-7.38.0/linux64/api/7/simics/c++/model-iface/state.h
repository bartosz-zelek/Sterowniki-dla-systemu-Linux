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

#ifndef SIMICS_CPP_MODEL_IFACE_STATE_H
#define SIMICS_CPP_MODEL_IFACE_STATE_H

#include "simics/model-iface/state.h"

#include <simics/detail/conf-object-util.h>  // get_interface
#include <simics/iface/interface-info.h>

namespace simics {
namespace iface {

class Uint64StateInterface {
  public:
    using ctype = uint64_state_interface_t;

    // Function override and implemented by user
    virtual void set(uint64 level) = 0;

    // Function convert C interface call to C++ interface call
    class FromC {
      public:
        static void set(conf_object_t *obj, uint64 level) {
            detail::get_interface<Uint64StateInterface>(obj)->set(level);
        }
    };

    // Function convert C++ interface call to C interface call
    class ToC {
      public:
        ToC() : obj_(nullptr), iface_(nullptr) {}
        ToC(conf_object_t *obj, const Uint64StateInterface::ctype *iface)
            : obj_(obj), iface_(iface) {}

        void set(uint64 level) const {
            iface_->set(obj_, level);
        }

        const Uint64StateInterface::ctype *get_iface() const {
            return iface_;
        }

      private:
        conf_object_t *obj_;
        const Uint64StateInterface::ctype *iface_;
    };

    class Info : public InterfaceInfo {
      public:
        // InterfaceInfo
        std::string name() const override { return UINT64_STATE_INTERFACE; }
        const interface_t *cstruct() const override {
            static constexpr Uint64StateInterface::ctype funcs {
                FromC::set,
            };
            return &funcs;
        }
    };
};

}  // namespace iface
}  // namespace simics

#endif
