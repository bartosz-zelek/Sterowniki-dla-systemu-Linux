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

#ifndef SIMICS_SYSTEMC_AWARENESS_PROXY_PORT_H
#define SIMICS_SYSTEMC_AWARENESS_PROXY_PORT_H



#include <systemc>
#include <simics/systemc/awareness/proxy.h>
#include <simics/systemc/awareness/sc_port_connection.h>

namespace simics {
namespace systemc {
namespace awareness {

/** \internal
 * Class ProxyPort responsible for mapping class sc_port to ConfObject.
 */
class ProxyPort : public Proxy,
                  public ScPortConnection {
  public:
    explicit ProxyPort(simics::ConfObjectRef o);
    virtual void init(sc_core::sc_object *obj, SimulationInterface *simulation);
    virtual void simulationStarted();
};

}  // namespace awareness
}  // namespace systemc
}  // namespace simics

#endif
