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

#ifndef SIMICS_SYSTEMC_SYSTEMC2SIMICS_GASKET_INTERFACE_H
#define SIMICS_SYSTEMC_SYSTEMC2SIMICS_GASKET_INTERFACE_H

#include <systemc>

#include <string>

namespace simics {
namespace systemc {
namespace systemc2simics {

/** \internal Base class for Gasket and NullSignal */
class GasketInterface {
  public:
    virtual ~GasketInterface() {}
    virtual sc_core::sc_signal<bool, sc_core::SC_MANY_WRITERS> *signal() = 0;
    virtual std::string gasket_name() const = 0;
    virtual const InterfaceProvider *interface_provider() const = 0;
};

}  // namespace systemc2simics
}  // namespace systemc
}  // namespace simics

#endif
