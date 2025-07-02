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

#ifndef SIMICS_SYSTEMC_INJECTION_B_TRANSPORT_INVOKER_H
#define SIMICS_SYSTEMC_INJECTION_B_TRANSPORT_INVOKER_H

#include <simics/systemc/internal_processes.h>

#include <systemc>
#include <tlm>
#include <utility>  // move

namespace simics {
namespace systemc {
namespace injection {

/** \internal */
template <typename TYPES>
class BTransportInvoker {
    typedef typename TYPES::tlm_payload_type transaction_type;
    typedef typename TYPES::tlm_phase_type phase_type;

    class Invoker {
      public:
        Invoker(transaction_type *transaction, sc_core::sc_time *offset,
                tlm::tlm_fw_transport_if<TYPES> *socket)
            : transaction_(transaction), offset_(*offset), socket_(socket) {}
        void operator()() {
            socket_->b_transport(*transaction_, offset_);
            transaction_->release();
            InternalProcesses.erase(sc_core::sc_get_current_process_handle());
        }

      private:
        transaction_type *transaction_;
        sc_core::sc_time offset_;
        tlm::tlm_fw_transport_if<TYPES> *socket_;
    };

  public:
    void enqueue(tlm::tlm_base_socket_if *socket, transaction_type *trans,
                 sc_core::sc_time *offset) {
#ifdef SYSTEMC_2_3_3
        tlm::tlm_fw_transport_if<TYPES> *socket_
            = dynamic_cast<tlm::tlm_fw_transport_if<TYPES> *>(
            socket->get_port_base().get_interface());
#elif defined SYSTEMC_2_3_4 || defined SYSTEMC_3_0_0
        tlm::tlm_fw_transport_if<TYPES> *socket_
            = dynamic_cast<tlm::tlm_fw_transport_if<TYPES> *>(
            socket->get_base_port().get_interface());
#endif
        assert(socket_);

        Invoker call(trans, offset, socket_);
        InternalProcesses.insert(sc_core::sc_spawn(std::move(call)));
    }
};

}  // namespace injection
}  // namespace systemc
}  // namespace simics

#endif
