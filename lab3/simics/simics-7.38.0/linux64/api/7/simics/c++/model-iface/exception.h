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

#ifndef SIMICS_CPP_MODEL_IFACE_EXCEPTION_H
#define SIMICS_CPP_MODEL_IFACE_EXCEPTION_H

#include "simics/model-iface/exception.h"

#include <simics/detail/conf-object-util.h>  // get_interface
#include <simics/iface/interface-info.h>

namespace simics {
namespace iface {

class ExceptionInterface {
  public:
    using ctype = exception_interface_t;

    // Function override and implemented by user
    virtual int get_number(const char *name) = 0;
    virtual const char * get_name(int exc) = 0;
    virtual int get_source(int exc) = 0;
    virtual attr_value_t all_exceptions() = 0;

    // Function convert C interface call to C++ interface call
    class FromC {
      public:
        static int get_number(conf_object_t *obj, const char *name) {
            return detail::get_interface<ExceptionInterface>(obj)->get_number(name);
        }
        static const char * get_name(conf_object_t *obj, int exc) {
            return detail::get_interface<ExceptionInterface>(obj)->get_name(exc);
        }
        static int get_source(conf_object_t *obj, int exc) {
            return detail::get_interface<ExceptionInterface>(obj)->get_source(exc);
        }
        static attr_value_t all_exceptions(conf_object_t *obj) {
            return detail::get_interface<ExceptionInterface>(obj)->all_exceptions();
        }
    };

    // Function convert C++ interface call to C interface call
    class ToC {
      public:
        ToC() : obj_(nullptr), iface_(nullptr) {}
        ToC(conf_object_t *obj, const ExceptionInterface::ctype *iface)
            : obj_(obj), iface_(iface) {}

        int get_number(const char *name) const {
            return iface_->get_number(obj_, name);
        }
        const char * get_name(int exc) const {
            return iface_->get_name(obj_, exc);
        }
        int get_source(int exc) const {
            return iface_->get_source(obj_, exc);
        }
        attr_value_t all_exceptions() const {
            return iface_->all_exceptions(obj_);
        }

        const ExceptionInterface::ctype *get_iface() const {
            return iface_;
        }

      private:
        conf_object_t *obj_;
        const ExceptionInterface::ctype *iface_;
    };

    class Info : public InterfaceInfo {
      public:
        // InterfaceInfo
        std::string name() const override { return EXCEPTION_INTERFACE; }
        const interface_t *cstruct() const override {
            static constexpr ExceptionInterface::ctype funcs {
                FromC::get_number,
                FromC::get_name,
                FromC::get_source,
                FromC::all_exceptions,
            };
            return &funcs;
        }
    };
};

}  // namespace iface
}  // namespace simics

#endif
