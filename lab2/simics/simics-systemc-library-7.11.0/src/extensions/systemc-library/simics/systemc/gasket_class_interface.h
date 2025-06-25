// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2017 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_GASKET_CLASS_INTERFACE_H
#define SIMICS_SYSTEMC_GASKET_CLASS_INTERFACE_H

#include <simics/cc-api.h>

#include <simics/systemc/iface/sc_version_interface.h>
#include <simics/systemc/iface/simulation_interface.h>

namespace simics {
namespace systemc {

/** \internal */
class GasketClassInterface {
  public:
    virtual ~GasketClassInterface() {}
    virtual iface::ScVersionInterface *version() = 0;
    virtual void createGasket(iface::SimulationInterface *simulation) = 0;
};

}  // namespace systemc
}  // namespace simics

#endif
