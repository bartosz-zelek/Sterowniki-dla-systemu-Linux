// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2016 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_IFACE_INSTRUMENTATION_SIGNAL_PORT_ACTION_INTERFACE_H
#define SIMICS_SYSTEMC_IFACE_INSTRUMENTATION_SIGNAL_PORT_ACTION_INTERFACE_H

#include <simics/systemc/awareness/proxy_interface.h>

namespace simics {
namespace systemc {
namespace iface {
namespace instrumentation {

/** \internal */
class SignalPortActionInterface {
  public:
    virtual void signal_port_value_update(awareness::ProxyInterface *proxy,
                                          sc_core::sc_object *signal) = 0;
    virtual ~SignalPortActionInterface() {}
};

}  // namespace instrumentation
}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
