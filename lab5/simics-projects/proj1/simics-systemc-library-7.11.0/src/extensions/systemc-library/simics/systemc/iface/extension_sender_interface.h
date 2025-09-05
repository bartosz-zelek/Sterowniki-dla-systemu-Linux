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

#ifndef SIMICS_SYSTEMC_IFACE_EXTENSION_SENDER_INTERFACE_H
#define SIMICS_SYSTEMC_IFACE_EXTENSION_SENDER_INTERFACE_H

#include <simics/systemc/iface/transaction.h>

namespace simics {
namespace systemc {
namespace iface {

/** Interface used by Extension class to send the extension. */
class ExtensionSenderInterface {
  public:
    /** Called by extension to get a new Transaction */
    virtual Transaction transaction() = 0;
    /** Called by extension after the extension is set on the payload */
    virtual void send_extension(Transaction *transaction) = 0;
    /** Called by extension if method_call invocation was missing */
    virtual void send_failed(Transaction *transaction) = 0;
    virtual ~ExtensionSenderInterface() {}
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
