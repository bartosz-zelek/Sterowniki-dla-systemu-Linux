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

#ifndef SIMICS_SYSTEMC_SIMICS2TLM_SERIAL_DEVICE_GASKET_ADAPTER_H
#define SIMICS_SYSTEMC_SIMICS2TLM_SERIAL_DEVICE_GASKET_ADAPTER_H

#include <simics/systemc/context.h>
#include <simics/systemc/iface/serial_device_interface.h>
#include <simics/systemc/iface/simulation_interface.h>
#include <simics/systemc/simics2tlm/gasket_adapter.h>

namespace simics {
namespace systemc {
namespace simics2tlm {

//:: pre SerialDeviceGasketAdapter {{
class SerialDeviceGasketAdapter
    : public iface::SerialDeviceInterface,
      public GasketAdapter<iface::SerialDeviceInterface> {
  public:
    SerialDeviceGasketAdapter(SerialDeviceInterface *serial_device,
                              iface::SimulationInterface *simulation)
        : serial_device_(serial_device), simulation_(simulation) {
    }
    int write(int value) override {
        Context context(simulation_);
        return serial_device_->write(value);
    }
    void receive_ready() override {
        Context context(simulation_);
        serial_device_->receive_ready();
    }
    simics2tlm::GasketOwner *gasket_owner() const override {
        return dynamic_cast<simics2tlm::GasketOwner *>(serial_device_);
    }

  private:
    SerialDeviceInterface *serial_device_;
    iface::SimulationInterface *simulation_;
};
// }}

}  // namespace simics2tlm
}  // namespace systemc
}  // namespace simics

#endif
