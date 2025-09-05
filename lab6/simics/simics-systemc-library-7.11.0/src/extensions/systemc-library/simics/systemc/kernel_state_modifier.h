// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2018 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_KERNEL_STATE_MODIFIER_H
#define SIMICS_SYSTEMC_KERNEL_STATE_MODIFIER_H

#include <simics/systemc/iface/simulation_interface.h>
#include <simics/systemc/kernel_state_comparator.h>

#include <systemc>

#include <unordered_map>

namespace simics {
namespace systemc {

/** \internal
 *
 * A RAII class designed to manage and modify the state of the SystemC kernel
 */
class KernelStateModifier {
  public:
    explicit KernelStateModifier(iface::SimulationInterface *simulation);
    virtual ~KernelStateModifier();

  private:
    static std::unordered_map<sc_core::sc_simcontext *,
                              KernelStateComparator> comparators_;
};

}  // namespace systemc
}  // namespace simics

#endif
