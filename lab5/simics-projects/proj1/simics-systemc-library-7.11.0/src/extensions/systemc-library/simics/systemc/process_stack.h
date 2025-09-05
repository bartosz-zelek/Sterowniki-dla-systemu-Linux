// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2016 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_PROCESS_STACK_H
#define SIMICS_SYSTEMC_PROCESS_STACK_H

#include <simics/systemc/iface/simulation_interface.h>
#include <simics/systemc/process_stack_interface.h>

#include <systemc>
#include <stack>

namespace simics {
namespace systemc {

/** \internal used by Simulation */
class ProcessStack : public ProcessStackInterface {
  public:
    explicit ProcessStack(iface::SimulationInterface *simulation);
    /* ProcessStackInterface */
    void push(); /* override */
    void pop(); /* override */
    void waitForCurrentProcessOnTop(); /* override */
    void runUntilStackEmpty(); /* override */
    void waitForExitCondition(
        sc_core::sc_curr_proc_kind enter_process_kind); /* override */

  private:
    std::stack<sc_core::sc_process_handle> stack_;
    sc_core::sc_event return_event_;
    iface::SimulationInterface *simulation_;
};

}  // namespace systemc
}  // namespace simics

#endif
