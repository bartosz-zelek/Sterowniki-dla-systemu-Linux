// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2014 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_IFACE_SIGNAL_INTERFACE_H
#define SIMICS_SYSTEMC_IFACE_SIGNAL_INTERFACE_H

namespace simics {
namespace systemc {
namespace iface {

/**
 * Simics signal interface. Please note that the Simics signal interface has
 * defined it to be an error to raise or lower the signal twice when already
 * high/low.
 */
class SignalInterface {
  public:
    virtual void raise() = 0;
    virtual void lower() = 0;
    virtual ~SignalInterface() {}
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
