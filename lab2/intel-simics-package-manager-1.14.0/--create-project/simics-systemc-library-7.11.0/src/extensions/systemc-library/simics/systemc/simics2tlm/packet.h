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

#ifndef SIMICS_SYSTEMC_SIMICS2TLM_PACKET_H
#define SIMICS_SYSTEMC_SIMICS2TLM_PACKET_H

#include <simics/systemc/iface/packet_interface.h>
#include <simics/systemc/iface/transaction_pool.h>
#include <simics/systemc/simics2tlm/gasket_owner.h>

namespace simics {
namespace systemc {
namespace simics2tlm {

/**
 * Class that implements the Simics packet interface and translates it
 * into a TLM transaction.
 */
class Packet : public iface::PacketInterface, public GasketOwner {
  public:
    virtual void transfer(const uint8 *data_ptr, size_t data_length);

  private:
    iface::TransactionPool pool_;
};

}  // namespace simics2tlm
}  // namespace systemc
}  // namespace simics

#endif
