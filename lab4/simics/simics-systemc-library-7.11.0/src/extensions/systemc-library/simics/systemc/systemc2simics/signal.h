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

#ifndef SIMICS_SYSTEMC_SYSTEMC2SIMICS_SIGNAL_H
#define SIMICS_SYSTEMC_SYSTEMC2SIMICS_SIGNAL_H

#include <systemc>

#include <simics/systemc/interface_provider.h>
#include <simics/systemc/systemc2simics/gasket.h>
#include <simics/systemc/systemc2simics/gasket_interface.h>
#include <simics/systemc/systemc2simics/null_signal.h>

namespace simics {
namespace systemc {
namespace systemc2simics {

class SignalBase : public simics::systemc::InterfaceProvider {
  public:
    friend class SignalSerializable;
    SignalBase()
        : simics::systemc::InterfaceProvider("signal"),
          gasket_(new NullSignal) {
    }
    ~SignalBase();

    // Operator for accessing the signal
    // NOTE: non-const access is not a supported use-case
    const sc_core::sc_signal<bool,
                             sc_core::SC_MANY_WRITERS> *operator->() const {
        FATAL_ERROR_IF(dynamic_cast<Gasket*>(gasket_) == NULL,
                       "Missing call to systemc2simics::Signal::set_pin()");
        return gasket_->signal();
    }

    // API: must be called from Adapter CTOR
    void set_pin(sc_core::sc_inout<bool> *target_pin);

  private:
    virtual void create_gasket(sc_core::sc_module_name);

    GasketInterface *gasket_;
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

}  // namespace systemc2simics
}  // namespace systemc
}  // namespace simics

#endif
