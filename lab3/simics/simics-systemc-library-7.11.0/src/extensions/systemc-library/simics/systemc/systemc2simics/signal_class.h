// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2020 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_SYSTEMC2SIMICS_SIGNAL_CLASS_H
#define SIMICS_SYSTEMC_SYSTEMC2SIMICS_SIGNAL_CLASS_H

#include <simics/systemc/connector.h>
#include <simics/systemc/gasket_class_interface.h>
#include <simics/systemc/systemc2simics/signal.h>
#include <simics/systemc/version.h>

#include <string>

namespace simics {
namespace systemc {
namespace systemc2simics {

class SignalClassBase : public ConfObject,
                        public GasketClassInterface,
                        public Version {
    friend class SignalClassNonSerializable;
    friend class SignalClassSerializable;

  public:
    explicit SignalClassBase(ConfObjectRef o);
    void finalize() override;
    iface::ScVersionInterface *version() override;
    void createGasket(iface::SimulationInterface *simulation) override;

  private:
    virtual SignalBase *signal() {return nullptr;}

    std::string sc_object_;
    ConfObjectRef simulation_ref_;
};

class SignalClassNonSerializable : public SignalClassBase {
  public:
    using SignalClassBase::SignalClassBase;
    static conf_class_t *registerGasketClass(const char* class_name);

  private:
    SignalBase *signal() override {
        return signal_.operator->();;
    }

    Connector<SignalBase> signal_;
};

#if INTC_EXT && USE_SIMICS_CHECKPOINTING
class SignalClassSerializable : public SignalClassBase {
  public:
    explicit SignalClassSerializable(ConfObjectRef o);
    static conf_class_t *registerGasketClass(const char* class_name);

  private:
    SignalBase *signal() override {
        return signal_.operator->();;
    }

    Connector<SignalSerializable> signal_;
};
using SignalClass = SignalClassSerializable;
#else
using SignalClass = SignalClassNonSerializable;
#endif

}  // namespace systemc2simics
}  // namespace systemc
}  // namespace simics

#endif
