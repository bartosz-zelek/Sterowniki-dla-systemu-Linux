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

#include <simics/systemc/awareness/signal_reader.h>
#include <simics/systemc/context.h>

namespace simics {
namespace systemc {
namespace awareness {

SignalReader::SignalReader(ProxyInterface *proxy) : proxy_(proxy) {}

SignalReader::~SignalReader() {}

attr_value_t SignalReader::read() {
    Context ctxt(proxy_->simulation());
    attr_value_t value;
    if (!registry_.read(proxy_->systemc_obj(), &value))
        SIM_LOG_ERROR(proxy_->simics_obj(), 0, "Signal type not supported.");

    return value;
}

}  // namespace awareness
}  // namespace systemc
}  // namespace simics
