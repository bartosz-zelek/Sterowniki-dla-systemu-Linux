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

#ifndef SIMICS_CPP_ARCH_NIOS_H
#define SIMICS_CPP_ARCH_NIOS_H

#include "simics/arch/nios.h"

#include <simics/detail/conf-object-util.h>  // get_interface
#include <simics/iface/interface-info.h>

namespace simics {
namespace iface {

class NiosEicInterface {
  public:
    using ctype = nios_eic_interface_t;

    // Function override and implemented by user
    virtual logical_address_t handler() = 0;
    virtual uint32 level() = 0;
    virtual uint32 reg_set() = 0;
    virtual bool nmi() = 0;
    virtual void handled() = 0;

    // Function convert C interface call to C++ interface call
    class FromC {
      public:
        static logical_address_t handler(conf_object_t *obj) {
            return detail::get_interface<NiosEicInterface>(obj)->handler();
        }
        static uint32 level(conf_object_t *obj) {
            return detail::get_interface<NiosEicInterface>(obj)->level();
        }
        static uint32 reg_set(conf_object_t *obj) {
            return detail::get_interface<NiosEicInterface>(obj)->reg_set();
        }
        static bool nmi(conf_object_t *obj) {
            return detail::get_interface<NiosEicInterface>(obj)->nmi();
        }
        static void handled(conf_object_t *obj) {
            detail::get_interface<NiosEicInterface>(obj)->handled();
        }
    };

    // Function convert C++ interface call to C interface call
    class ToC {
      public:
        ToC() : obj_(nullptr), iface_(nullptr) {}
        ToC(conf_object_t *obj, const NiosEicInterface::ctype *iface)
            : obj_(obj), iface_(iface) {}

        logical_address_t handler() const {
            return iface_->handler(obj_);
        }
        uint32 level() const {
            return iface_->level(obj_);
        }
        uint32 reg_set() const {
            return iface_->reg_set(obj_);
        }
        bool nmi() const {
            return iface_->nmi(obj_);
        }
        void handled() const {
            iface_->handled(obj_);
        }

        const NiosEicInterface::ctype *get_iface() const {
            return iface_;
        }

      private:
        conf_object_t *obj_;
        const NiosEicInterface::ctype *iface_;
    };

    class Info : public InterfaceInfo {
      public:
        // InterfaceInfo
        std::string name() const override { return NIOS_EIC_INTERFACE; }
        const interface_t *cstruct() const override {
            static constexpr NiosEicInterface::ctype funcs {
                FromC::handler,
                FromC::level,
                FromC::reg_set,
                FromC::nmi,
                FromC::handled,
            };
            return &funcs;
        }
    };
};

class NiosCacheInterface {
  public:
    using ctype = nios_cache_interface_t;

    // Function override and implemented by user
    virtual void flushd(logical_address_t addr) = 0;
    virtual void flushda(logical_address_t addr) = 0;
    virtual void flushi(logical_address_t addr) = 0;
    virtual void flushp() = 0;
    virtual void initd(logical_address_t addr) = 0;
    virtual void initda(logical_address_t addr) = 0;
    virtual void initi(logical_address_t addr) = 0;
    virtual void sync() = 0;

    // Function convert C interface call to C++ interface call
    class FromC {
      public:
        static void flushd(conf_object_t *self, logical_address_t addr) {
            detail::get_interface<NiosCacheInterface>(self)->flushd(addr);
        }
        static void flushda(conf_object_t *self, logical_address_t addr) {
            detail::get_interface<NiosCacheInterface>(self)->flushda(addr);
        }
        static void flushi(conf_object_t *self, logical_address_t addr) {
            detail::get_interface<NiosCacheInterface>(self)->flushi(addr);
        }
        static void flushp(conf_object_t *self) {
            detail::get_interface<NiosCacheInterface>(self)->flushp();
        }
        static void initd(conf_object_t *self, logical_address_t addr) {
            detail::get_interface<NiosCacheInterface>(self)->initd(addr);
        }
        static void initda(conf_object_t *self, logical_address_t addr) {
            detail::get_interface<NiosCacheInterface>(self)->initda(addr);
        }
        static void initi(conf_object_t *self, logical_address_t addr) {
            detail::get_interface<NiosCacheInterface>(self)->initi(addr);
        }
        static void sync(conf_object_t *self) {
            detail::get_interface<NiosCacheInterface>(self)->sync();
        }
    };

    // Function convert C++ interface call to C interface call
    class ToC {
      public:
        ToC() : obj_(nullptr), iface_(nullptr) {}
        ToC(conf_object_t *obj, const NiosCacheInterface::ctype *iface)
            : obj_(obj), iface_(iface) {}

        void flushd(logical_address_t addr) const {
            iface_->flushd(obj_, addr);
        }
        void flushda(logical_address_t addr) const {
            iface_->flushda(obj_, addr);
        }
        void flushi(logical_address_t addr) const {
            iface_->flushi(obj_, addr);
        }
        void flushp() const {
            iface_->flushp(obj_);
        }
        void initd(logical_address_t addr) const {
            iface_->initd(obj_, addr);
        }
        void initda(logical_address_t addr) const {
            iface_->initda(obj_, addr);
        }
        void initi(logical_address_t addr) const {
            iface_->initi(obj_, addr);
        }
        void sync() const {
            iface_->sync(obj_);
        }

        const NiosCacheInterface::ctype *get_iface() const {
            return iface_;
        }

      private:
        conf_object_t *obj_;
        const NiosCacheInterface::ctype *iface_;
    };

    class Info : public InterfaceInfo {
      public:
        // InterfaceInfo
        std::string name() const override { return NIOS_CACHE_INTERFACE; }
        const interface_t *cstruct() const override {
            static constexpr NiosCacheInterface::ctype funcs {
                FromC::flushd,
                FromC::flushda,
                FromC::flushi,
                FromC::flushp,
                FromC::initd,
                FromC::initda,
                FromC::initi,
                FromC::sync,
            };
            return &funcs;
        }
    };
};

class NiosCustomInterface {
  public:
    using ctype = nios_custom_interface_t;

    // Function override and implemented by user
    virtual uint32 custom(uint32 n, uint32 a, uint32 b, uint32 c, uint32 rA, uint32 rB, bool readra, bool readrb, bool writerc) = 0;

    // Function convert C interface call to C++ interface call
    class FromC {
      public:
        static uint32 custom(conf_object_t *self, uint32 n, uint32 a, uint32 b, uint32 c, uint32 rA, uint32 rB, bool readra, bool readrb, bool writerc) {
            return detail::get_interface<NiosCustomInterface>(self)->custom(n, a, b, c, rA, rB, readra, readrb, writerc);
        }
    };

    // Function convert C++ interface call to C interface call
    class ToC {
      public:
        ToC() : obj_(nullptr), iface_(nullptr) {}
        ToC(conf_object_t *obj, const NiosCustomInterface::ctype *iface)
            : obj_(obj), iface_(iface) {}

        uint32 custom(uint32 n, uint32 a, uint32 b, uint32 c, uint32 rA, uint32 rB, bool readra, bool readrb, bool writerc) const {
            return iface_->custom(obj_, n, a, b, c, rA, rB, readra, readrb, writerc);
        }

        const NiosCustomInterface::ctype *get_iface() const {
            return iface_;
        }

      private:
        conf_object_t *obj_;
        const NiosCustomInterface::ctype *iface_;
    };

    class Info : public InterfaceInfo {
      public:
        // InterfaceInfo
        std::string name() const override { return NIOS_CUSTOM_INTERFACE; }
        const interface_t *cstruct() const override {
            static constexpr NiosCustomInterface::ctype funcs {
                FromC::custom,
            };
            return &funcs;
        }
    };
};

}  // namespace iface
}  // namespace simics

#endif
