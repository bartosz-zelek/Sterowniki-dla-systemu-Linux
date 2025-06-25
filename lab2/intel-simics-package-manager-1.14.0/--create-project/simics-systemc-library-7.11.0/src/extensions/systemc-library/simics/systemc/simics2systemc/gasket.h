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

#ifndef SIMICS_SYSTEMC_SIMICS2SYSTEMC_GASKET_H
#define SIMICS_SYSTEMC_SIMICS2SYSTEMC_GASKET_H

#include <systemc>

#include <simics/systemc/simics2systemc/gasket_interface.h>

#include <string>

#if INTC_EXT && USE_SIMICS_CHECKPOINTING
#include <systemc-checkpoint/serialization/sc_signal.h>
#include <systemc-checkpoint/serialization/serializer.h>
#include <systemc-checkpoint/serialization/smd.h>
#endif

namespace simics {
namespace systemc {
namespace simics2systemc {

/** Gasket for translating a Simics signal interface into a SystemC sc_signal
    of type bool. */
class GasketBase : public GasketInterface
                 , public sc_core::sc_module {
  public:
    friend class GasketSerializable;
    explicit GasketBase(sc_core::sc_module_name) : output_pin_("output_pin") {}
    virtual sc_core::sc_signal<bool> *output_pin() {
        return &output_pin_;
    }
    virtual std::string gasket_name() const {
        return name();
    }

  private:
    sc_core::sc_signal<bool> output_pin_;
};

#if INTC_EXT && USE_SIMICS_CHECKPOINTING
/*
 * This class responsible for serializing the state of output_pin_
 *
 * This class is visible only when USE_SIMICS_CHECKPOINTING=yes
 */
class GasketSerializable : public GasketBase {
  public:
    explicit GasketSerializable(sc_core::sc_module_name);

    friend class cci::serialization::access;
    template <class Archive>
    void serialize(Archive& ar, const unsigned int version) {  // NOLINT
        ar & SMD(output_pin_);
    }
    static sc_checkpoint::serialization::Serializer<
        GasketSerializable> output_pin_serializer;
};
using Gasket = GasketSerializable;
#else
using Gasket = GasketBase;
#endif

}  // namespace simics2systemc
}  // namespace systemc
}  // namespace simics

#endif
