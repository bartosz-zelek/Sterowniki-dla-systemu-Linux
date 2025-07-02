// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2015 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_IFACE_EXTENSION_H
#define SIMICS_SYSTEMC_IFACE_EXTENSION_H

#include <tlm>

#include <simics/systemc/iface/extension_ignore_receiver.h>
#include <simics/systemc/iface/extension_receiver.h>
#include <simics/systemc/iface/extension_sender_interface.h>
#include <simics/systemc/iface/receiver_interface.h>
#include <simics/systemc/iface/transaction.h>
#include <simics/types/any_type.h>
#include <vector>

namespace simics {
namespace systemc {
namespace iface {

/**
 * Base class for TLM2 extension, responsible for marshal/unmarshal of a Simics
 * interface. The interface specific sub-class implements the marshal/unmarshal
 * using the functionality of the base class. Most important is the send() and
 * call() methods.
 */
template<typename TExtension, typename TInterface>
class Extension : public tlm::tlm_extension<TExtension>, public TInterface {
  public:
    Extension() : sender_(NULL), method_call_count_(0),
                  suppress_error_(false), valid_(false) {}
    /** Deprecated, use the init(ExtensionSenderInterface *sender) instead. */
    void init(ExtensionSenderInterface *sender,
              tlm::tlm_generic_payload *payload) {
        sender_ = sender;
    }
    void init(ExtensionSenderInterface *sender) {
        sender_ = sender;
    }
    void send() {
        // Clear returns from former call.
        method_return_ = types::AnyType();
        method_call_count_ = 0;
        suppress_error_ = false;
        // Copy method, method_input and method_return_error into new
        // extension object.
        Extension *e = static_cast<Extension*>(clone());
        // Clear method, method_input and method_return_error for reenter
        // from send_extension.
        method_input_.clear();
        method_ = types::AnyType();
        method_return_error_ = types::AnyType();
        e->valid_ = true;
        Transaction t = sender_->transaction();
        t->set_extension(e);
        /* During send_extension, the extension is dispatched to an
         * ExtensionReceiver or an ExtensionIgnoreReceiver. The receiver
         * invokes either method_call or method_call_ignore.*/
        sender_->send_extension(&t);
        // Copy method_return back.
        if (e->method_call_count_ != 1) {
            method_return_ = e->method_return_error_;
            if (!method_return_.isSet())
                method_return_ = 0;
            if (!e->suppress_error_)
                sender_->send_failed(&t);
        } else {
            method_return_ = e->method_return_;
        }
        t->clear_extension(e);
        e->free();
    }
    void method_call(TInterface *device) {
        ++method_call_count_;
        if (device)
            call(device);
    }
    void method_call_ignore() {
        suppress_error_ = true;
    }
    types::AnyType method_type() {
        return method_;
    }
    bool valid() {
        return valid_;
    }
    virtual tlm::tlm_extension_base *clone() const {
        return new TExtension(*static_cast<const TExtension *>(this));
    }
    virtual void copy_from(tlm::tlm_extension_base const &extension) {
        *this = static_cast<const TExtension &>(extension);
    }
    static ReceiverInterface *createReceiver(TInterface *device) {
        return new ExtensionReceiver<TExtension, TInterface>(device);
    }
    static ReceiverInterface *createIgnoreReceiver() {
        return new ExtensionIgnoreReceiver<TExtension>();
    }
    virtual ~Extension() {}

  protected:
    virtual void call(TInterface *device) = 0;

    ExtensionSenderInterface *sender_;
    types::AnyType method_;
    std::vector<types::AnyType> method_input_;
    types::AnyType method_return_;
    types::AnyType method_return_error_;
    int method_call_count_;
    bool suppress_error_;
    bool valid_;
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
