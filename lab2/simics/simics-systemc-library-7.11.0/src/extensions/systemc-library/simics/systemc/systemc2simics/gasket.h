// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2019 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_SYSTEMC2SIMICS_GASKET_H
#define SIMICS_SYSTEMC_SYSTEMC2SIMICS_GASKET_H

#include <systemc>
#include <simics/systemc/interface_provider.h>
#include <simics/systemc/null_simulation.h>
#include <simics/systemc/systemc2simics/gasket_interface.h>
#include <simics/systemc/registry.h>

#include <string>

#if INTC_EXT && USE_SIMICS_CHECKPOINTING
#include <systemc-checkpoint/serialization/sc_signal.h>
#include <systemc-checkpoint/serialization/serializer.h>
#include <systemc-checkpoint/serialization/smd.h>
#endif

namespace simics {
namespace systemc {
namespace systemc2simics {

/** Gasket for translating a SystemC sc_signal of type bool into a Simics
    signal interface call. */
class GasketBase : public Registrant<GasketInterface>
                 , public sc_core::sc_module {
  public:
#ifndef SYSTEMC_3_0_0
    // SC_HAS_PROCESS is deprecated and no long needed in 3.0.0
    SC_HAS_PROCESS(GasketBase);
#endif
    GasketBase(sc_core::sc_module_name,
               simics::systemc::InterfaceProvider *provider);

    virtual sc_core::sc_signal<bool, sc_core::SC_MANY_WRITERS> *signal() {
        return &signal_;
    }
    std::string gasket_name() const override {
        return name();
    }
    const InterfaceProvider *interface_provider() const override {
        return provider_;
    }

  private:
    void update();
    void call_iface(bool value);

    sc_core::sc_signal<bool, sc_core::SC_MANY_WRITERS> signal_;
    simics::systemc::InterfaceProvider *provider_;
    NullSimulation null_simulation_;
};

#if INTC_EXT && USE_SIMICS_CHECKPOINTING
class GasketSerializable : public GasketBase {
  public:
    GasketSerializable(sc_core::sc_module_name,
                       simics::systemc::InterfaceProvider *provider);

    friend class cci::serialization::access;
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version) {  // NOLINT
        ar & SMD(*signal());
    }
    static sc_checkpoint::serialization::Serializer<
        GasketSerializable> gasket_serializer;
};
using Gasket = GasketSerializable;
#else
using Gasket = GasketBase;
#endif

}  // namespace systemc2simics
}  // namespace systemc
}  // namespace simics

#endif
