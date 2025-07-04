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

#ifndef SIMICS_CPP_MODEL_IFACE_DATA_PROFILER_H
#define SIMICS_CPP_MODEL_IFACE_DATA_PROFILER_H

#include "simics/model-iface/data-profiler.h"

#include <simics/detail/conf-object-util.h>  // get_interface
#include <simics/iface/interface-info.h>

namespace simics {
namespace iface {

class DataProfilerInterface {
  public:
    using ctype = data_profiler_interface_t;

    // Function override and implemented by user
    virtual void save(const char *file) = 0;
    virtual void load(const char *file) = 0;
    virtual void clear() = 0;
    virtual void * get_prof_data() = 0;

    // Function convert C interface call to C++ interface call
    class FromC {
      public:
        static void save(conf_object_t *profiler, const char *file) {
            detail::get_interface<DataProfilerInterface>(profiler)->save(file);
        }
        static void load(conf_object_t *profiler, const char *file) {
            detail::get_interface<DataProfilerInterface>(profiler)->load(file);
        }
        static void clear(conf_object_t *profiler) {
            detail::get_interface<DataProfilerInterface>(profiler)->clear();
        }
        static void * get_prof_data(conf_object_t *profiler) {
            return detail::get_interface<DataProfilerInterface>(profiler)->get_prof_data();
        }
    };

    // Function convert C++ interface call to C interface call
    class ToC {
      public:
        ToC() : obj_(nullptr), iface_(nullptr) {}
        ToC(conf_object_t *obj, const DataProfilerInterface::ctype *iface)
            : obj_(obj), iface_(iface) {}

        void save(const char *file) const {
            iface_->save(obj_, file);
        }
        void load(const char *file) const {
            iface_->load(obj_, file);
        }
        void clear() const {
            iface_->clear(obj_);
        }
        void * get_prof_data() const {
            return iface_->get_prof_data(obj_);
        }

        const DataProfilerInterface::ctype *get_iface() const {
            return iface_;
        }

      private:
        conf_object_t *obj_;
        const DataProfilerInterface::ctype *iface_;
    };

    class Info : public InterfaceInfo {
      public:
        // InterfaceInfo
        std::string name() const override { return DATA_PROFILER_INTERFACE; }
        const interface_t *cstruct() const override {
            static constexpr DataProfilerInterface::ctype funcs {
                nullptr,
                FromC::save,
                FromC::load,
                FromC::clear,
                FromC::get_prof_data,
            };
            return &funcs;
        }
    };
};

}  // namespace iface
}  // namespace simics

#endif
