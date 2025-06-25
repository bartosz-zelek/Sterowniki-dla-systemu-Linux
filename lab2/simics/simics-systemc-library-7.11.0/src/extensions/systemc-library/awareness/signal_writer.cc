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

#include <simics/systemc/awareness/signal_writer.h>
#include <simics/systemc/kernel_state_modifier.h>

namespace simics {
namespace systemc {
namespace awareness {

SignalWriter::SignalWriter(ProxyInterface *proxy) : proxy_(proxy) {}

SignalWriter::~SignalWriter() {}

void SignalWriter::write(const attr_value_t &value) {
    KernelStateModifier m(proxy_->simulation());
    if (!registry_.write(proxy_->systemc_obj(), &value))
        SIM_LOG_ERROR(proxy_->simics_obj(), 0, "Signal type not supported.");
}

}  // namespace awareness
}  // namespace systemc
}  // namespace simics
