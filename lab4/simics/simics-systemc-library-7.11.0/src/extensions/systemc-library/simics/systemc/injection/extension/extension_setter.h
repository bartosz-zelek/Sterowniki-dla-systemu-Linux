// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2017 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_INJECTION_EXTENSION_EXTENSION_SETTER_H
#define SIMICS_SYSTEMC_INJECTION_EXTENSION_EXTENSION_SETTER_H

#include <simics/systemc/iface/extension_sender_interface.h>

#include <tlm>

namespace simics {
namespace systemc {
namespace injection {
namespace extension {

/** \internal
 * This Class is used to set SCL extensions in the GP.
 * For this, the extension is send via this Class. It preserve the state of
 * the extension object, so that it can be used later for sending over a
 * TLM socket, having a ExtensionReceiver on the terminating side. */
template <typename TPAYLOAD, typename TExtension>
class ExtensionSetter : public iface::ExtensionSenderInterface {
  public:
    explicit ExtensionSetter(TPAYLOAD *gp)
        : gp_at_call_(gp), extension_at_call_(NULL) {
        extension_.init(this);
    }
    virtual ~ExtensionSetter() {
        if (extension_at_call_) {
            gp_at_call_->resize_extensions();
            delete gp_at_call_->set_extension(extension_at_call_);
        }
    }
    ExtensionSetter(const ExtensionSetter &other) = delete;
    ExtensionSetter& operator=(const ExtensionSetter &other) = delete;

    TExtension *operator->() {
        return &extension_;
    }
    virtual iface::Transaction transaction() {
        return iface::Transaction(&gp_);
    }
    virtual void send_extension(iface::Transaction *transaction) {
        TExtension *e = (*transaction)->get_extension<TExtension>();
        assert(e);
        if (extension_at_call_)
            delete extension_at_call_;

        extension_at_call_ = static_cast<TExtension *>(e->clone());
    }
    virtual void send_failed(iface::Transaction *transaction) {}

  private:
    TPAYLOAD gp_;
    TPAYLOAD *gp_at_call_;
    TExtension extension_;
    TExtension *extension_at_call_;
};

}  // namespace extension
}  // namespace injection
}  // namespace systemc
}  // namespace simics

#endif
