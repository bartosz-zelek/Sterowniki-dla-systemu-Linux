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

#include <simics/systemc/awareness/proxy_port.h>

namespace simics {
namespace systemc {
namespace awareness {

ProxyPort::ProxyPort(ConfObjectRef o) : Proxy(o) {}

void ProxyPort::init(sc_core::sc_object *obj, SimulationInterface *simulation) {
    Proxy::init(obj, simulation);
    ScPortConnection::init(obj, simulation);
}

void ProxyPort::simulationStarted() {
    port_to_proxies();
}

}  // namespace awareness
}  // namespace systemc
}  // namespace simics
