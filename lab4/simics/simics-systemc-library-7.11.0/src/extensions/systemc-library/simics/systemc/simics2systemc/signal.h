// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2013 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_SIMICS2SYSTEMC_SIGNAL_H
#define SIMICS_SYSTEMC_SIMICS2SYSTEMC_SIGNAL_H

#include <simics/cc-api.h>
#include <systemc>

#include <simics/systemc/iface/signal_interface.h>
#include <simics/systemc/iface/simulation_interface.h>
#include <simics/systemc/internal_interface.h>
#include <simics/systemc/simics2systemc/gasket_interface.h>

namespace simics {
namespace systemc {
namespace simics2systemc {

class SignalBase : public iface::SignalInterface {
  public:
    friend class SignalSerializable;
    SignalBase();
    SignalBase(const SignalBase&) = delete;
    SignalBase& operator=(const SignalBase&) = delete;
    ~SignalBase();

    // iface::SignalInterface
    void raise() override {
        validate_signal_level(true);
        update(true);
    }
    void lower() override {
        validate_signal_level(false);
        update(false);
    }

    void init(iface::SimulationInterface *simulation,
              InternalInterface *internal);
    void set_pin(sc_core::sc_in<bool> *target_pin, bool initial_level,
                 const ConfObjectRef &obj);
    void set_pin(sc_core::sc_inout<bool> *target_pin, bool initial_level,
                 const ConfObjectRef &obj);
    GasketInterface *gasket();

  private:
    void update(bool value);
    void set_pin(const char *target_pin_name, bool initial_level,
                 const ConfObjectRef &obj);
    void validate_signal_level(bool raise);
    virtual void create_gasket(sc_core::sc_module_name);

    ConfObjectRef *simics_obj_;
    GasketInterface *gasket_;
    iface::SimulationInterface *simulation_;
    InternalInterface *internal_;
};

#if INTC_EXT && USE_SIMICS_CHECKPOINTING
class SignalSerializable : public SignalBase {
  public:
    SignalSerializable();

  private:
    void create_gasket(sc_core::sc_module_name) override;
};

using Signal = SignalSerializable;
#else
using Signal = SignalBase;
#endif

}  // namespace simics2systemc
}  // namespace systemc
}  // namespace simics

#endif
