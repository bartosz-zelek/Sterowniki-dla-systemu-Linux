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

#ifndef SIMICS_SYSTEMC_SIMICS2SYSTEMC_GASKET_INTERFACE_H
#define SIMICS_SYSTEMC_SIMICS2SYSTEMC_GASKET_INTERFACE_H

#include <systemc>

#include <simics/systemc/interface_provider.h>

#include <string>

namespace simics {
namespace systemc {
namespace simics2systemc {

/** \internal Base class for Gasket and NullSignal */
class GasketInterface {
  public:
    virtual ~GasketInterface() {}
    virtual sc_core::sc_signal<bool> *output_pin() = 0;
    virtual std::string gasket_name() const = 0;
};

}  // namespace simics2systemc
}  // namespace systemc
}  // namespace simics

#endif
