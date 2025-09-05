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

#include <simics/systemc/systemc2simics/signal.h>
#include <simics/systemc/systemc2simics/gasket.h>

#include <string>

namespace simics {
namespace systemc {
namespace systemc2simics {

void SignalBase::set_pin(sc_core::sc_inout<bool> *target_pin) {
    FATAL_ERROR_IF(target_pin->name() == NULL,
                   "target pin not fully initialized, name missing");

    std::string name = "gasket_";
    name += target_pin->name();
    std::replace(name.begin(), name.end(), '.', '_');
    create_gasket(name.c_str());
    (*target_pin)(*gasket_->signal());
}

void SignalBase::create_gasket(sc_core::sc_module_name name) {
    delete gasket_;
    gasket_ = new GasketBase(name, this);
}

SignalBase::~SignalBase() {
    delete gasket_;
}

}  // namespace systemc2simics
}  // namespace systemc
}  // namespace simics
