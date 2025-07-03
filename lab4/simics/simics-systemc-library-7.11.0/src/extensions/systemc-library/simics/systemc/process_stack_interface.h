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

#ifndef SIMICS_SYSTEMC_PROCESS_STACK_INTERFACE_H
#define SIMICS_SYSTEMC_PROCESS_STACK_INTERFACE_H

#include <systemc>

namespace simics {
namespace systemc {

/** The process stack helps maintain a LIFO order of function calls that cross
    the SystemC/Simics border, as this is what the Simics interfaces
    expects. */
class ProcessStackInterface {
  public:
    virtual ~ProcessStackInterface() {}
    // Pushes the current process on the stack so it can be waited on before
    // returning from a simics2tlm gasket call. Push is typically called from a
    // tlm2simics gasket before calling into Simics.
    virtual void push() = 0;
    // Pops the top process from the stack. Pop is typically called from a
    // tlm2simics gasket after the call to Simics returns.
    virtual void pop() = 0;
    // Waits for the current process to become the top entry on the stack, in
    // order to maintain a LIFO order of Simics interface function
    // calls. Typically called from a simics2tlm gasket before exit/return.
    virtual void waitForCurrentProcessOnTop() = 0;
    // Unwinds the stack by running SystemC kernel to the next pending activity
    // until the stack is empty. NOTE: it is not guaranteed that the stack will
    // become empty with this approach. Processes that wait forever, or on each
    // other in a dead-lock situation, will lock here (and this is
    // expected). Typically called from the event callback to make sure more
    // time is spent unwinding the stack. For transactions, there is already a
    // loop that waits for the transaction to complete.
    virtual void runUntilStackEmpty() = 0;
    // Waits until current process can return from the function call, by
    // invoking either waitForCurrentProcessOnTop() or runUntilStackEmpty()
    // depending on the process kind that entered the current function where
    // waitForExitCondition() was invoked.
    virtual void waitForExitCondition(
        sc_core::sc_curr_proc_kind enter_process_kind) = 0;
};

}  // namespace systemc
}  // namespace simics

#endif
