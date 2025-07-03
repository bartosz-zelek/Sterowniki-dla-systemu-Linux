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

#include <simics/systemc/simics2tlm/packet.h>

namespace simics {
namespace systemc {
namespace simics2tlm {

void Packet::transfer(const uint8 *data_ptr, size_t data_length) {
    iface::Transaction t = pool_.acquire();
    t->set_address(0);
    t->set_data_length(data_length);
    t->set_streaming_width(data_length);
    t->set_data_ptr(const_cast<unsigned char *>(data_ptr));
    t->set_write();
    gasket_->trigger(&t);
}

}  // namespace simics2tlm
}  // namespace systemc
}  // namespace simics
