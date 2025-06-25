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

#ifndef SYSTEMC_LIBRARY_UNITTEST_MOCK_AWARENESS_MOCK_PROXY_H
#define SYSTEMC_LIBRARY_UNITTEST_MOCK_AWARENESS_MOCK_PROXY_H

#include <simics/systemc/awareness/proxy_interface.h>

namespace unittest {
namespace awareness {

using simics::systemc::awareness::ProxyFeatureInterface;
using simics::systemc::awareness::ProxyInterface;
using simics::systemc::ClassType;
using simics::systemc::iface::SimulationInterface;

class MockProxy : public conf_object, public simics::ConfObject,
                  public ProxyInterface {
  public:
    MockProxy() : simics::ConfObject(this),
                  systemc_obj_(NULL), simulation_(NULL), feature_(NULL) {
        instance_data = this;
    }
    virtual void init(sc_core::sc_object *obj,
                      SimulationInterface *simulation) {
        systemc_obj_ = obj;
        simulation_ = simulation;
    }
    virtual void allProxiesInitialized() {
    }
    virtual void breakSimulation() {
    }
    virtual simics::ConfObjectRef simics_obj() {
        return this;
    }
    virtual sc_core::sc_object *systemc_obj() {
        return systemc_obj_;
    }
    virtual void buildEventObjects() {
    }
    virtual void addFeature(ProxyFeatureInterface* feature) {
        delete feature;
        feature_ = feature;
    }
    virtual ProxyFeatureInterface *feature(ClassType type) {
        return feature_;
    }
    virtual void simulationStarted() {
    }
    virtual void simulationEnded() {
    }
    SimulationInterface *simulation() {
        return simulation_;
    }
    sc_core::sc_object *systemc_obj_;
    SimulationInterface *simulation_;
    ProxyFeatureInterface* feature_;
};

}  // namespace awareness
}  // namespace unittest

#endif
