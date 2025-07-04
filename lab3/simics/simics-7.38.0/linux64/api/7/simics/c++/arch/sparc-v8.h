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

#ifndef SIMICS_CPP_ARCH_SPARC_V8_H
#define SIMICS_CPP_ARCH_SPARC_V8_H

#include "simics/arch/sparc-v8.h"

#include <simics/detail/conf-object-util.h>  // get_interface
#include <simics/iface/interface-info.h>

namespace simics {
namespace iface {

class SparcV8Interface {
  public:
    using ctype = sparc_v8_interface_t;

    // Function override and implemented by user
    virtual uint64 read_window_register(int window, int reg) = 0;
    virtual void write_window_register(int window, int reg, uint64 value) = 0;
    virtual void power_down() = 0;

    // Function convert C interface call to C++ interface call
    class FromC {
      public:
        static uint64 read_window_register(conf_object_t *cpu, int window, int reg) {
            return detail::get_interface<SparcV8Interface>(cpu)->read_window_register(window, reg);
        }
        static void write_window_register(conf_object_t *cpu, int window, int reg, uint64 value) {
            detail::get_interface<SparcV8Interface>(cpu)->write_window_register(window, reg, value);
        }
        static void power_down(conf_object_t *cpu) {
            detail::get_interface<SparcV8Interface>(cpu)->power_down();
        }
    };

    // Function convert C++ interface call to C interface call
    class ToC {
      public:
        ToC() : obj_(nullptr), iface_(nullptr) {}
        ToC(conf_object_t *obj, const SparcV8Interface::ctype *iface)
            : obj_(obj), iface_(iface) {}

        uint64 read_window_register(int window, int reg) const {
            return iface_->read_window_register(obj_, window, reg);
        }
        void write_window_register(int window, int reg, uint64 value) const {
            iface_->write_window_register(obj_, window, reg, value);
        }
        void power_down() const {
            iface_->power_down(obj_);
        }

        const SparcV8Interface::ctype *get_iface() const {
            return iface_;
        }

      private:
        conf_object_t *obj_;
        const SparcV8Interface::ctype *iface_;
    };

    class Info : public InterfaceInfo {
      public:
        // InterfaceInfo
        std::string name() const override { return SPARC_V8_INTERFACE; }
        const interface_t *cstruct() const override {
            static constexpr SparcV8Interface::ctype funcs {
                FromC::read_window_register,
                FromC::write_window_register,
                FromC::power_down,
            };
            return &funcs;
        }
    };
};

class SparcV8EccFaultInjectionInterface {
  public:
    using ctype = sparc_v8_ecc_fault_injection_interface_t;

    // Function override and implemented by user
    virtual void inject_instr_access_exception() = 0;
    virtual void inject_data_access_exception() = 0;
    virtual void inject_reg_access_error() = 0;

    // Function convert C interface call to C++ interface call
    class FromC {
      public:
        static void inject_instr_access_exception(conf_object_t *cpu) {
            detail::get_interface<SparcV8EccFaultInjectionInterface>(cpu)->inject_instr_access_exception();
        }
        static void inject_data_access_exception(conf_object_t *cpu) {
            detail::get_interface<SparcV8EccFaultInjectionInterface>(cpu)->inject_data_access_exception();
        }
        static void inject_reg_access_error(conf_object_t *cpu) {
            detail::get_interface<SparcV8EccFaultInjectionInterface>(cpu)->inject_reg_access_error();
        }
    };

    // Function convert C++ interface call to C interface call
    class ToC {
      public:
        ToC() : obj_(nullptr), iface_(nullptr) {}
        ToC(conf_object_t *obj, const SparcV8EccFaultInjectionInterface::ctype *iface)
            : obj_(obj), iface_(iface) {}

        void inject_instr_access_exception() const {
            iface_->inject_instr_access_exception(obj_);
        }
        void inject_data_access_exception() const {
            iface_->inject_data_access_exception(obj_);
        }
        void inject_reg_access_error() const {
            iface_->inject_reg_access_error(obj_);
        }

        const SparcV8EccFaultInjectionInterface::ctype *get_iface() const {
            return iface_;
        }

      private:
        conf_object_t *obj_;
        const SparcV8EccFaultInjectionInterface::ctype *iface_;
    };

    class Info : public InterfaceInfo {
      public:
        // InterfaceInfo
        std::string name() const override { return SPARC_V8_ECC_FAULT_INJECTION_INTERFACE; }
        const interface_t *cstruct() const override {
            static constexpr SparcV8EccFaultInjectionInterface::ctype funcs {
                FromC::inject_instr_access_exception,
                FromC::inject_data_access_exception,
                FromC::inject_reg_access_error,
            };
            return &funcs;
        }
    };
};

}  // namespace iface
}  // namespace simics

#endif
