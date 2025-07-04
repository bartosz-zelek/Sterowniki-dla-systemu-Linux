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

#ifndef SIMICS_CPP_SIMULATOR_IFACE_OSA_TRACKER_COMPONENT_INTERFACE_H
#define SIMICS_CPP_SIMULATOR_IFACE_OSA_TRACKER_COMPONENT_INTERFACE_H

#include "simics/simulator-iface/osa-tracker-component-interface.h"

#include <simics/detail/conf-object-util.h>  // get_interface
#include <simics/iface/interface-info.h>

namespace simics {
namespace iface {

class OsaTrackerComponentInterface {
  public:
    using ctype = osa_tracker_component_interface_t;

    // Function override and implemented by user
    virtual conf_object_t * get_tracker() = 0;
    virtual conf_object_t * get_mapper() = 0;

    // Function convert C interface call to C++ interface call
    class FromC {
      public:
        static conf_object_t * get_tracker(conf_object_t *obj) {
            return detail::get_interface<OsaTrackerComponentInterface>(obj)->get_tracker();
        }
        static conf_object_t * get_mapper(conf_object_t *obj) {
            return detail::get_interface<OsaTrackerComponentInterface>(obj)->get_mapper();
        }
    };

    // Function convert C++ interface call to C interface call
    class ToC {
      public:
        ToC() : obj_(nullptr), iface_(nullptr) {}
        ToC(conf_object_t *obj, const OsaTrackerComponentInterface::ctype *iface)
            : obj_(obj), iface_(iface) {}

        conf_object_t * get_tracker() const {
            return iface_->get_tracker(obj_);
        }
        conf_object_t * get_mapper() const {
            return iface_->get_mapper(obj_);
        }

        const OsaTrackerComponentInterface::ctype *get_iface() const {
            return iface_;
        }

      private:
        conf_object_t *obj_;
        const OsaTrackerComponentInterface::ctype *iface_;
    };

    class Info : public InterfaceInfo {
      public:
        // InterfaceInfo
        std::string name() const override { return OSA_TRACKER_COMPONENT_INTERFACE; }
        const interface_t *cstruct() const override {
            static constexpr OsaTrackerComponentInterface::ctype funcs {
                FromC::get_tracker,
                FromC::get_mapper,
            };
            return &funcs;
        }
    };
};

}  // namespace iface
}  // namespace simics

#endif
