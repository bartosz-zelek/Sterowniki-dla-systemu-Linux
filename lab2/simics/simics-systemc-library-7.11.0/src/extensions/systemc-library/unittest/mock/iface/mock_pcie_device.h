// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2024 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SYSTEMC_LIBRARY_UNITTEST_MOCK_IFACE_MOCK_PCIE_DEVICE_H
#define SYSTEMC_LIBRARY_UNITTEST_MOCK_IFACE_MOCK_PCIE_DEVICE_H

#include <simics/systemc/iface/pcie_device_interface.h>

class MockPcieDevice : public simics::systemc::iface::PcieDeviceInterface {
  public:
    MockPcieDevice()
        : connected_(0), connected_port_obj_(nullptr),
          connected_device_id_(0), disconnected_(0),
          disconnected_device_id_(0), hot_reset_(0) {
    }
    virtual void connected(conf_object_t *port_obj, uint16 device_id) {
        ++connected_;
        connected_port_obj_ = port_obj;
        connected_device_id_ = device_id;
    }
    virtual void disconnected(conf_object_t *port_obj, uint16 device_id) {
        ++disconnected_;
        disconnected_port_obj_ = port_obj;
        disconnected_device_id_ = device_id;
    }
    virtual void hot_reset() {
        ++hot_reset_;
    }

    int connected_;
    conf_object_t *connected_port_obj_;
    uint16 connected_device_id_;
    int disconnected_;
    conf_object_t *disconnected_port_obj_;
    uint16 disconnected_device_id_;
    int hot_reset_;
};

#endif
