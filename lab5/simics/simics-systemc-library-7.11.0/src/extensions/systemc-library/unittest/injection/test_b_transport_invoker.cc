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

#include <boost/test/unit_test.hpp>

#include <simics/systemc/injection/b_transport_invoker.h>

#include <tlm>

#include "environment.h"
#include "mock/mock_initiator_socket.h"
#include "mock/mock_target_socket.h"

BOOST_AUTO_TEST_SUITE(TestBTransportInvoker)

class MemoryManager : public tlm::tlm_mm_interface {
  public:
    MemoryManager() : gp_(NULL) {}
    virtual void free(tlm::tlm_generic_payload *gp) {
        gp_ = gp;
    }
    tlm::tlm_generic_payload *gp_;
};

BOOST_FIXTURE_TEST_CASE(testInvoke, Environment) {
    unittest::MockTlmInitiatorSocket<> is("is");
    unittest::MockTlmTargetSocket<> ts("ts");
    is.initiator_socket(ts.target_socket);
    simics::systemc::injection::BTransportInvoker<
        tlm::tlm_base_protocol_types> invoker;

    sc_core::sc_start(sc_core::SC_ZERO_TIME);

    MemoryManager mm;
    tlm::tlm_generic_payload *payload = new tlm::tlm_generic_payload(&mm);
    payload->acquire();
    sc_core::sc_time offset;
    invoker.enqueue(&is.initiator_socket, payload, &offset);

    BOOST_CHECK_EQUAL(ts.b_transport_count_, 0);

    sc_core::sc_start(sc_core::SC_ZERO_TIME);

    BOOST_CHECK_EQUAL(ts.b_transport_count_, 1);
    BOOST_CHECK_EQUAL(payload, mm.gp_);

    delete mm.gp_;
}

BOOST_AUTO_TEST_SUITE_END()
