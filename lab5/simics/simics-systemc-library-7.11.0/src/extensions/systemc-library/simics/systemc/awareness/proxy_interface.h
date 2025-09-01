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

#ifndef SIMICS_SYSTEMC_AWARENESS_PROXY_INTERFACE_H
#define SIMICS_SYSTEMC_AWARENESS_PROXY_INTERFACE_H

#include <simics/cc-api.h>  // ConfObjectRef
#include <simics/systemc/awareness/proxy_feature_interface.h>
#include <simics/systemc/iface/simulation_interface.h>

#include <systemc>

namespace simics {
namespace systemc {
namespace awareness {

class ProxyInterface {
  public:
    virtual void init(sc_core::sc_object *obj,
                      iface::SimulationInterface *simulation) = 0;
    virtual void allProxiesInitialized() = 0;
    virtual void breakSimulation() = 0;
    virtual ConfObjectRef simics_obj() = 0;
    virtual sc_core::sc_object *systemc_obj() = 0;
    virtual void buildEventObjects() = 0;
    virtual iface::SimulationInterface *simulation() = 0;
    virtual void addFeature(ProxyFeatureInterface *feature) = 0;
    virtual ProxyFeatureInterface *feature(ClassType type) = 0;
    virtual void simulationStarted() = 0;
    virtual void simulationEnded() = 0;
    virtual ~ProxyInterface() {}
};

}  // namespace awareness
}  // namespace systemc
}  // namespace simics

#endif
