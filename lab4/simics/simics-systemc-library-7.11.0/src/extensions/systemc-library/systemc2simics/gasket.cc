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

#include <simics/devs/signal.h>

#include <simics/systemc/context.h>
#include <simics/systemc/interface_provider.h>
#include <simics/systemc/systemc2simics/gasket.h>

#include <sstream>

namespace {
// TODO(ah): putting this handling here for now, we can move it somewhere more
// generic once we have the need to support this type of error handling for
// other non-TLM2 gaskets. Currently we only have Signal. For TLM2 gaskets,
// this is handled by the ErrorTransactionHandler class.
using simics::systemc::InterfaceProvider;
void log_error(InterfaceProvider *interface_provider,
               const char *message) {
    if (interface_provider->target().object() == NULL) {
        std::ostringstream log;
        log << message
            << " The " << interface_provider->get_interface_name()
            << " interface provider is not configured (no object).";
        SC_REPORT_ERROR("/intel/Signal", log.str().c_str());
    } else {
        std::ostringstream log;
        log << message
            << " The object "
            << SIM_object_name(interface_provider->target().object())
            << " does not implement the "
            << interface_provider->get_interface_name()
            << " interface.";
        SC_REPORT_ERROR("/intel/Signal", log.str().c_str());
    }
}
}  // namespace

namespace simics {
namespace systemc {
namespace systemc2simics {

GasketBase::GasketBase(sc_core::sc_module_name,
                       simics::systemc::InterfaceProvider *provider)
    : signal_("signal"),
      provider_(provider) {
#ifndef RISC_SIMICS
    // To handle possible re-entry into SystemC, must use a SC_THREAD
    SC_THREAD(update);
#endif
}

void GasketBase::update() {
    while (1) {
#ifndef RISC_SIMICS
        wait(signal_.value_changed_event());
#endif
        call_iface(signal_.read());
    }
}

void GasketBase::call_iface(bool value) {
    SimicsTargetLock<const signal_interface_t> iface
        = provider_->get_interface<const signal_interface_t>();
    if (iface == NULL) {
        // TODO(ah): in order to log, we need a ConfObjectRef and how do
        // we do that without a gasket? We could put it into the 1st
        // top-level object using an sc_attribute. Or we simply log using
        // SC_REPORT_ERROR macro as it's currently implemented. This is
        // really part of a different bug (proper error handling for
        // optional signal connects or when passing objects without the
        // signal interface).
        log_error(provider_, "Signal::update abort.");
    } else {
        // Switching context to detect re-entry without setting context.
        Context c(&null_simulation_);

        if (value)
            iface->signal_raise(provider_->target().object());
        else
            iface->signal_lower(provider_->target().object());
    }
}

}  // namespace systemc2simics
}  // namespace systemc
}  // namespace simics
