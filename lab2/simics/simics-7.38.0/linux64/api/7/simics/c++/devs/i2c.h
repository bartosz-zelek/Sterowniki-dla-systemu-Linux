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

#ifndef SIMICS_CPP_DEVS_I2C_H
#define SIMICS_CPP_DEVS_I2C_H

#include "simics/devs/i2c.h"

#include <simics/detail/conf-object-util.h>  // get_interface
#include <simics/iface/interface-info.h>

namespace simics {
namespace iface {

class I2cSlaveV2Interface {
  public:
    using ctype = i2c_slave_v2_interface_t;

    // Function override and implemented by user
    virtual void start(uint8 address) = 0;
    virtual void read() = 0;
    virtual void write(uint8 value) = 0;
    virtual void stop() = 0;
    virtual attr_value_t addresses() = 0;

    // Function convert C interface call to C++ interface call
    class FromC {
      public:
        static void start(conf_object_t *device, uint8 address) {
            detail::get_interface<I2cSlaveV2Interface>(device)->start(address);
        }
        static void read(conf_object_t *device) {
            detail::get_interface<I2cSlaveV2Interface>(device)->read();
        }
        static void write(conf_object_t *device, uint8 value) {
            detail::get_interface<I2cSlaveV2Interface>(device)->write(value);
        }
        static void stop(conf_object_t *device) {
            detail::get_interface<I2cSlaveV2Interface>(device)->stop();
        }
        static attr_value_t addresses(conf_object_t *device) {
            return detail::get_interface<I2cSlaveV2Interface>(device)->addresses();
        }
    };

    // Function convert C++ interface call to C interface call
    class ToC {
      public:
        ToC() : obj_(nullptr), iface_(nullptr) {}
        ToC(conf_object_t *obj, const I2cSlaveV2Interface::ctype *iface)
            : obj_(obj), iface_(iface) {}

        void start(uint8 address) const {
            iface_->start(obj_, address);
        }
        void read() const {
            iface_->read(obj_);
        }
        void write(uint8 value) const {
            iface_->write(obj_, value);
        }
        void stop() const {
            iface_->stop(obj_);
        }
        attr_value_t addresses() const {
            return iface_->addresses(obj_);
        }

        const I2cSlaveV2Interface::ctype *get_iface() const {
            return iface_;
        }

      private:
        conf_object_t *obj_;
        const I2cSlaveV2Interface::ctype *iface_;
    };

    class Info : public InterfaceInfo {
      public:
        // InterfaceInfo
        std::string name() const override { return I2C_SLAVE_V2_INTERFACE; }
        const interface_t *cstruct() const override {
            static constexpr I2cSlaveV2Interface::ctype funcs {
                FromC::start,
                FromC::read,
                FromC::write,
                FromC::stop,
                FromC::addresses,
            };
            return &funcs;
        }
    };
};

class I2cMasterV2Interface {
  public:
    using ctype = i2c_master_v2_interface_t;

    // Function override and implemented by user
    virtual void acknowledge(i2c_ack_t ack) = 0;
    virtual void read_response(uint8 value) = 0;

    // Function convert C interface call to C++ interface call
    class FromC {
      public:
        static void acknowledge(conf_object_t *device, i2c_ack_t ack) {
            detail::get_interface<I2cMasterV2Interface>(device)->acknowledge(ack);
        }
        static void read_response(conf_object_t *device, uint8 value) {
            detail::get_interface<I2cMasterV2Interface>(device)->read_response(value);
        }
    };

    // Function convert C++ interface call to C interface call
    class ToC {
      public:
        ToC() : obj_(nullptr), iface_(nullptr) {}
        ToC(conf_object_t *obj, const I2cMasterV2Interface::ctype *iface)
            : obj_(obj), iface_(iface) {}

        void acknowledge(i2c_ack_t ack) const {
            iface_->acknowledge(obj_, ack);
        }
        void read_response(uint8 value) const {
            iface_->read_response(obj_, value);
        }

        const I2cMasterV2Interface::ctype *get_iface() const {
            return iface_;
        }

      private:
        conf_object_t *obj_;
        const I2cMasterV2Interface::ctype *iface_;
    };

    class Info : public InterfaceInfo {
      public:
        // InterfaceInfo
        std::string name() const override { return I2C_MASTER_V2_INTERFACE; }
        const interface_t *cstruct() const override {
            static constexpr I2cMasterV2Interface::ctype funcs {
                FromC::acknowledge,
                FromC::read_response,
            };
            return &funcs;
        }
    };
};

}  // namespace iface
}  // namespace simics

#endif
