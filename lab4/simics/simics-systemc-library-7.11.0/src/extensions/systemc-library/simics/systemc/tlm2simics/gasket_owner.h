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

#ifndef SIMICS_SYSTEMC_TLM2SIMICS_GASKET_OWNER_H
#define SIMICS_SYSTEMC_TLM2SIMICS_GASKET_OWNER_H

#include <simics/systemc/tlm2simics/gasket_interface.h>
#include <simics/systemc/tlm2simics/null_gasket.h>

namespace simics {
namespace systemc {
namespace tlm2simics {

/**
 * Base class, responsible for handling a gasket. The sub-class will use the
 * gasket passed by set_gasket() method to register itself as the transaction
 * handler for the gasket. See Gasket class for details.
 */
class GasketOwner {
  public:
    GasketOwner() : gasket_(new NullGasket) {}
    GasketOwner(const GasketOwner&) = delete;
    GasketOwner& operator=(const GasketOwner&) = delete;
    virtual ~GasketOwner() = default;

    virtual void set_gasket(GasketInterface::Ptr gasketInterface) {
        FATAL_ERROR_IF(!gasketInterface, "GasketOwner initialized with NULL!");
        // coverity[copy_instead_of_move:SUPPRESS] Ptr is a pointer
        gasket_ = gasketInterface;
    }
    GasketInterface::Ptr gasket() const {
        return gasket_;
    }

  protected:
    GasketInterface::Ptr gasket_;
};

}  // namespace tlm2simics
}  // namespace systemc
}  // namespace simics

#endif
