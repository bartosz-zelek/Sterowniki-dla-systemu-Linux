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

#ifndef SIMICS_CPP_MODEL_IFACE_VGA_TEXT_INTERFACE_H
#define SIMICS_CPP_MODEL_IFACE_VGA_TEXT_INTERFACE_H

#include "simics/model-iface/vga-text-interface.h"

#include <simics/detail/conf-object-util.h>  // get_interface
#include <simics/iface/interface-info.h>

namespace simics {
namespace iface {

class VgaTextInterface {
  public:
    using ctype = vga_text_interface_t;

    // Function override and implemented by user
    virtual int add_string_notification(char *substring, double sample_interval) = 0;

    // Function convert C interface call to C++ interface call
    class FromC {
      public:
        static int add_string_notification(conf_object_t *obj, char *substring, double sample_interval) {
            return detail::get_interface<VgaTextInterface>(obj)->add_string_notification(substring, sample_interval);
        }
    };

    // Function convert C++ interface call to C interface call
    class ToC {
      public:
        ToC() : obj_(nullptr), iface_(nullptr) {}
        ToC(conf_object_t *obj, const VgaTextInterface::ctype *iface)
            : obj_(obj), iface_(iface) {}

        int add_string_notification(char *substring, double sample_interval) const {
            return iface_->add_string_notification(obj_, substring, sample_interval);
        }

        const VgaTextInterface::ctype *get_iface() const {
            return iface_;
        }

      private:
        conf_object_t *obj_;
        const VgaTextInterface::ctype *iface_;
    };

    class Info : public InterfaceInfo {
      public:
        // InterfaceInfo
        std::string name() const override { return VGA_TEXT_INTERFACE; }
        const interface_t *cstruct() const override {
            static constexpr VgaTextInterface::ctype funcs {
                FromC::add_string_notification,
            };
            return &funcs;
        }
    };
};

class VgaTextInfoInterface {
  public:
    using ctype = vga_text_info_interface_t;

    // Function override and implemented by user
    virtual bool text_mode() = 0;
    virtual bool font_size(int *width, int *height) = 0;
    virtual bool screen_size(int *columns, int *rows) = 0;
    virtual bool text(uint8 *text, uint8 *line_lengths) = 0;

    // Function convert C interface call to C++ interface call
    class FromC {
      public:
        static bool text_mode(conf_object_t *obj) {
            return detail::get_interface<VgaTextInfoInterface>(obj)->text_mode();
        }
        static bool font_size(conf_object_t *obj, int *width, int *height) {
            return detail::get_interface<VgaTextInfoInterface>(obj)->font_size(width, height);
        }
        static bool screen_size(conf_object_t *obj, int *columns, int *rows) {
            return detail::get_interface<VgaTextInfoInterface>(obj)->screen_size(columns, rows);
        }
        static bool text(conf_object_t *obj, uint8 *text, uint8 *line_lengths) {
            return detail::get_interface<VgaTextInfoInterface>(obj)->text(text, line_lengths);
        }
    };

    // Function convert C++ interface call to C interface call
    class ToC {
      public:
        ToC() : obj_(nullptr), iface_(nullptr) {}
        ToC(conf_object_t *obj, const VgaTextInfoInterface::ctype *iface)
            : obj_(obj), iface_(iface) {}

        bool text_mode() const {
            return iface_->text_mode(obj_);
        }
        bool font_size(int *width, int *height) const {
            return iface_->font_size(obj_, width, height);
        }
        bool screen_size(int *columns, int *rows) const {
            return iface_->screen_size(obj_, columns, rows);
        }
        bool text(uint8 *text, uint8 *line_lengths) const {
            return iface_->text(obj_, text, line_lengths);
        }

        const VgaTextInfoInterface::ctype *get_iface() const {
            return iface_;
        }

      private:
        conf_object_t *obj_;
        const VgaTextInfoInterface::ctype *iface_;
    };

    class Info : public InterfaceInfo {
      public:
        // InterfaceInfo
        std::string name() const override { return VGA_TEXT_INFO_INTERFACE; }
        const interface_t *cstruct() const override {
            static constexpr VgaTextInfoInterface::ctype funcs {
                FromC::text_mode,
                FromC::font_size,
                FromC::screen_size,
                FromC::text,
            };
            return &funcs;
        }
    };
};

class VgaTextUpdateInterface {
  public:
    using ctype = vga_text_update_interface_t;

    // Function override and implemented by user
    virtual void write(char value) = 0;

    // Function convert C interface call to C++ interface call
    class FromC {
      public:
        static void write(conf_object_t *obj, char value) {
            detail::get_interface<VgaTextUpdateInterface>(obj)->write(value);
        }
    };

    // Function convert C++ interface call to C interface call
    class ToC {
      public:
        ToC() : obj_(nullptr), iface_(nullptr) {}
        ToC(conf_object_t *obj, const VgaTextUpdateInterface::ctype *iface)
            : obj_(obj), iface_(iface) {}

        void write(char value) const {
            iface_->write(obj_, value);
        }

        const VgaTextUpdateInterface::ctype *get_iface() const {
            return iface_;
        }

      private:
        conf_object_t *obj_;
        const VgaTextUpdateInterface::ctype *iface_;
    };

    class Info : public InterfaceInfo {
      public:
        // InterfaceInfo
        std::string name() const override { return VGA_TEXT_UPDATE_INTERFACE; }
        const interface_t *cstruct() const override {
            static constexpr VgaTextUpdateInterface::ctype funcs {
                FromC::write,
            };
            return &funcs;
        }
    };
};

}  // namespace iface
}  // namespace simics

#endif
