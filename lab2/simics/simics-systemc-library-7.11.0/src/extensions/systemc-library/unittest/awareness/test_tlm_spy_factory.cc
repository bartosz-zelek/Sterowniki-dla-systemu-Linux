// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2014 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#if INTC_EXT

#include <boost/test/unit_test.hpp>

#include <simics/systemc/awareness/tlm_spy_factory.h>
#include <simics/systemc/awareness/tlm_fw_transport_if_handler.h>

#include <unittest/mock/awareness/mock_tlm_fw_transport_if_handler.h>
#include <unittest/mock/awareness/mock_tlm_bw_transport_if_handler.h>
#include <unittest/mock/mock_initiator_socket.h>
#include <unittest/mock/mock_target_socket.h>
#include <simics/systemc/device.h>
#include <simics/systemc/kernel.h>

BOOST_AUTO_TEST_SUITE(TestTlmSpyFactory)

struct MockInterface {
    typedef  tlm::tlm_generic_payload tlm_payload_type;
    typedef  tlm::tlm_phase tlm_phase_type;
};

template <typename TYPES = tlm::tlm_base_protocol_types>
class TestBase {
  public:
    typedef typename awareness::unittest::MockTlmFwTransportIfHandler<
        intc::sc_port_spy<tlm::tlm_fw_transport_if<
        TYPES> >, TYPES> PortFWHandler;

    typedef typename awareness::unittest::MockTlmBwTransportIfHandler<
        intc::sc_port_spy<tlm::tlm_bw_transport_if<
        TYPES> >, TYPES> PortBWHandler;

    typedef typename awareness::unittest::MockTlmBwTransportIfHandler<
        intc::sc_export_spy<tlm::tlm_bw_transport_if<
        TYPES> >, TYPES> ExportBWHandler;

    typedef typename awareness::unittest::MockTlmFwTransportIfHandler<
        intc::sc_export_spy<tlm::tlm_fw_transport_if<
        TYPES> >, TYPES> ExportFWHandler;

    TestBase() : context_(new sc_core::sc_simcontext), kernel_(context_) {
        PortFWHandler::reset();
        PortBWHandler::reset();
        ExportBWHandler::reset();
        ExportFWHandler::reset();
    }
    virtual ~TestBase() {
        delete context_;
    }

    sc_core::sc_simcontext *context_;
    simics::systemc::Kernel kernel_;
    simics::systemc::awareness::TlmSpyFactory<TYPES, PortFWHandler,
                                              PortBWHandler, ExportBWHandler,
                                              ExportFWHandler> factory;
};

template <unsigned int BUSWIDTH = 32,
          typename TYPES = tlm::tlm_base_protocol_types>
class TestEnvironment : public TestBase<TYPES> {
  public:
    TestEnvironment() {
        initiator_ = new unittest::MockTlmInitiatorSocket<BUSWIDTH, TYPES>
            ("TestInitiator");
        target_ = new unittest::MockTlmTargetSocket<BUSWIDTH,
                                                    TYPES>("TestTarget");
        target_->target_socket(initiator_->initiator_socket);
    }
    ~TestEnvironment() {
        delete target_;
        delete initiator_;
    }
    void testForwardPath() {
        tlm::tlm_generic_payload trans;
        sc_core::sc_time t;

        initiator_->initiator_socket->b_transport(trans, t);
        BOOST_CHECK_EQUAL(1, target_->b_transport_count_);
        BOOST_CHECK_EQUAL(1,
            TestBase<TYPES>::PortFWHandler::b_transport_count_);
        BOOST_CHECK_EQUAL(1,
            TestBase<TYPES>::ExportFWHandler::b_transport_count_);
    }
    void testBackwardPath() {
        target_->target_socket->invalidate_direct_mem_ptr(0, 0);
        BOOST_CHECK_EQUAL(1, initiator_->invalidate_direct_mem_ptr_count_);
        BOOST_CHECK_EQUAL(1,
            TestBase<TYPES>::PortBWHandler::invalidate_direct_mem_ptr_count_);
        BOOST_CHECK_EQUAL(1,
            TestBase<TYPES>::ExportBWHandler::invalidate_direct_mem_ptr_count_);
    }

    unittest::MockTlmInitiatorSocket<BUSWIDTH, TYPES> *initiator_;
    unittest::MockTlmTargetSocket<BUSWIDTH, TYPES> *target_;
};

template <unsigned int BUSWIDTH = 32,
          typename TYPES = tlm::tlm_base_protocol_types>
class TestEnvironmentMultiTarget : public TestBase<TYPES> {
  public:
    TestEnvironmentMultiTarget() {
        initiator_ = new unittest::MockTlmInitiatorSocket<BUSWIDTH,
            TYPES>("TestInitiator");
        initiator2_ = new unittest::MockTlmInitiatorSocket<BUSWIDTH,
            TYPES>("TestInitiator2");
        target_ = new unittest::MockMultiPassthroughTargetSocket<BUSWIDTH,
            TYPES>("TestTarget");
        initiator_->initiator_socket.bind(target_->target_socket);
        initiator2_->initiator_socket.bind(target_->target_socket);
    }
    void testForwardPath() {
        tlm::tlm_generic_payload trans;
        sc_core::sc_time t;

        initiator_->initiator_socket->b_transport(trans, t);
        BOOST_CHECK_EQUAL(1, target_->b_transport_count_);
        BOOST_CHECK_EQUAL(1,
            TestBase<TYPES>::PortFWHandler::b_transport_count_);
        BOOST_CHECK_EQUAL(1,
            TestBase<TYPES>::ExportFWHandler::b_transport_count_);

        initiator2_->initiator_socket->b_transport(trans, t);
        BOOST_CHECK_EQUAL(2, target_->b_transport_count_);
        BOOST_CHECK_EQUAL(2,
            TestBase<TYPES>::PortFWHandler::b_transport_count_);
        BOOST_CHECK_EQUAL(2,
            TestBase<TYPES>::ExportFWHandler::b_transport_count_);
    }

    void testBackwardPath() {
        target_->target_socket->invalidate_direct_mem_ptr(0, 0);

        BOOST_CHECK_EQUAL(1, initiator_->invalidate_direct_mem_ptr_count_);
        BOOST_CHECK_EQUAL(0, initiator2_->invalidate_direct_mem_ptr_count_);
        BOOST_CHECK_EQUAL(1,
            TestBase<TYPES>::ExportBWHandler::invalidate_direct_mem_ptr_count_);
        BOOST_CHECK_EQUAL(1,
            TestBase<TYPES>::PortBWHandler::invalidate_direct_mem_ptr_count_);

        target_->target_socket[0]->invalidate_direct_mem_ptr(0, 0);
        BOOST_CHECK_EQUAL(2, initiator_->invalidate_direct_mem_ptr_count_);
        BOOST_CHECK_EQUAL(0, initiator2_->invalidate_direct_mem_ptr_count_);
        BOOST_CHECK_EQUAL(2,
            TestBase<TYPES>::ExportBWHandler::invalidate_direct_mem_ptr_count_);
        // multi_passthrough_target_socket overloads operator[] from port.
        // The bw calls are directly forwarded instead of using the port.
        BOOST_CHECK_EQUAL(1,
            TestBase<TYPES>::PortBWHandler::invalidate_direct_mem_ptr_count_);

        target_->target_socket[1]->invalidate_direct_mem_ptr(0, 0);
        BOOST_CHECK_EQUAL(2, initiator_->invalidate_direct_mem_ptr_count_);
        BOOST_CHECK_EQUAL(1, initiator2_->invalidate_direct_mem_ptr_count_);
        BOOST_CHECK_EQUAL(3,
            TestBase<TYPES>::ExportBWHandler::invalidate_direct_mem_ptr_count_);
        BOOST_CHECK_EQUAL(1,
            TestBase<TYPES>::PortBWHandler::invalidate_direct_mem_ptr_count_);
    }
    ~TestEnvironmentMultiTarget() {
        delete target_;
        delete initiator2_;
        delete initiator_;
    }

    unittest::MockTlmInitiatorSocket<BUSWIDTH, TYPES> *initiator_;
    unittest::MockTlmInitiatorSocket<BUSWIDTH, TYPES> *initiator2_;
    unittest::MockMultiPassthroughTargetSocket<BUSWIDTH, TYPES> *target_;
};

template <unsigned int BUSWIDTH = 32,
          typename TYPES = tlm::tlm_base_protocol_types>
class TestEnvironmentMultiInitiator : public TestBase<TYPES> {
  public:
    TestEnvironmentMultiInitiator() {
        initiator_ = new unittest::MockMultiPassthroughInitiatorSocket<BUSWIDTH,
            TYPES>("TestInitiator");
        target_ = new unittest::MockTlmTargetSocket<BUSWIDTH,
            TYPES>("TestTarget");
        target2_ = new unittest::MockTlmTargetSocket<BUSWIDTH,
            TYPES>("TestTarget2");
        initiator_->initiator_socket.bind(target_->target_socket);
        initiator_->initiator_socket.bind(target2_->target_socket);
    }
    void testForwardPath() {
        tlm::tlm_generic_payload trans;
        sc_core::sc_time t;

        initiator_->initiator_socket->b_transport(trans, t);
        BOOST_CHECK_EQUAL(1, target_->b_transport_count_);
        BOOST_CHECK_EQUAL(0, target2_->b_transport_count_);
        BOOST_CHECK_EQUAL(1,
            TestBase<TYPES>::ExportFWHandler::b_transport_count_);
        BOOST_CHECK_EQUAL(1,
            TestBase<TYPES>::PortFWHandler::b_transport_count_);

        initiator_->initiator_socket[1]->b_transport(trans, t);
        BOOST_CHECK_EQUAL(1, target_->b_transport_count_);
        BOOST_CHECK_EQUAL(1, target2_->b_transport_count_);
        BOOST_CHECK_EQUAL(2,
            TestBase<TYPES>::ExportFWHandler::b_transport_count_);
        // multi_passthrough_initiator_socket overloads operator[] from port.
        // The fw calls are directly forwarded instead of using the port.
        BOOST_CHECK_EQUAL(1,
            TestBase<TYPES>::PortFWHandler::b_transport_count_);
    }

    void testBackwardPath() {
        target_->target_socket->invalidate_direct_mem_ptr(0, 0);
        BOOST_CHECK_EQUAL(1, initiator_->invalidate_direct_mem_ptr_count_);
        BOOST_CHECK_EQUAL(1,
            TestBase<TYPES>::PortBWHandler::invalidate_direct_mem_ptr_count_);
        BOOST_CHECK_EQUAL(1,
            TestBase<TYPES>::ExportBWHandler::invalidate_direct_mem_ptr_count_);

        target2_->target_socket->invalidate_direct_mem_ptr(0, 0);
        BOOST_CHECK_EQUAL(2, initiator_->invalidate_direct_mem_ptr_count_);
        BOOST_CHECK_EQUAL(2,
            TestBase<TYPES>::PortBWHandler::invalidate_direct_mem_ptr_count_);
        BOOST_CHECK_EQUAL(2,
            TestBase<TYPES>::ExportBWHandler::invalidate_direct_mem_ptr_count_);
    }
    ~TestEnvironmentMultiInitiator() {
        delete target2_;
        delete target_;
        delete initiator_;
    }

    unittest::MockMultiPassthroughInitiatorSocket<BUSWIDTH, TYPES> *initiator_;
    unittest::MockTlmTargetSocket<BUSWIDTH, TYPES> *target_;
    unittest::MockTlmTargetSocket<BUSWIDTH, TYPES> *target2_;
};

typedef TestEnvironment<32, MockInterface> TestEnvironmentMockInterface32;
typedef TestEnvironment<64, MockInterface> TestEnvironmentMockInterface64;

BOOST_FIXTURE_TEST_CASE(testForwardPath, TestEnvironment<>) {
    factory.traverseAll();
    sc_core::sc_start(sc_core::SC_ZERO_TIME);

    testForwardPath();
}

BOOST_FIXTURE_TEST_CASE(testBackwardPath, TestEnvironment<>) {
    factory.traverseAll();
    sc_core::sc_start(sc_core::SC_ZERO_TIME);

    testBackwardPath();
}

BOOST_FIXTURE_TEST_CASE(testForwardPath64, TestEnvironment<64>) {
    factory.traverseAll();
    sc_core::sc_start(sc_core::SC_ZERO_TIME);

    testForwardPath();
}

BOOST_FIXTURE_TEST_CASE(testBackwardPath64, TestEnvironment<64>) {
    factory.traverseAll();
    sc_core::sc_start(sc_core::SC_ZERO_TIME);

    testBackwardPath();
}

BOOST_FIXTURE_TEST_CASE(testForwardPathMultiTarget,
                        TestEnvironmentMultiTarget<>) {
    factory.traverseAll();
    sc_core::sc_start(sc_core::SC_ZERO_TIME);

    testForwardPath();
}

BOOST_FIXTURE_TEST_CASE(testForwardPathMultiTarget64,
                        TestEnvironmentMultiTarget<64>) {
    factory.traverseAll();
    sc_core::sc_start(sc_core::SC_ZERO_TIME);

    testForwardPath();
}

BOOST_FIXTURE_TEST_CASE(testBackwardPathMultiTarget,
                        TestEnvironmentMultiTarget<>) {
    factory.traverseAll();
    sc_core::sc_start(sc_core::SC_ZERO_TIME);

    testBackwardPath();
}

BOOST_FIXTURE_TEST_CASE(testBackwardPathMultiTarget64,
                        TestEnvironmentMultiTarget<64>) {
    factory.traverseAll();
    sc_core::sc_start(sc_core::SC_ZERO_TIME);

    testBackwardPath();
}

BOOST_FIXTURE_TEST_CASE(testForwardPathMultiInitiator,
                        TestEnvironmentMultiInitiator<>) {
    factory.traverseAll();
    sc_core::sc_start(sc_core::SC_ZERO_TIME);

    testForwardPath();
}

BOOST_FIXTURE_TEST_CASE(testForwardPathMultiInitiator64,
                        TestEnvironmentMultiInitiator<64>) {
    factory.traverseAll();
    sc_core::sc_start(sc_core::SC_ZERO_TIME);

    testForwardPath();
}

BOOST_FIXTURE_TEST_CASE(testBackwardPathMultiInitiator,
                        TestEnvironmentMultiInitiator<>) {
    factory.traverseAll();
    sc_core::sc_start(sc_core::SC_ZERO_TIME);

    testBackwardPath();
}

BOOST_FIXTURE_TEST_CASE(testBackwardPathMultiInitiator64,
                        TestEnvironmentMultiInitiator<64>) {
    factory.traverseAll();
    sc_core::sc_start(sc_core::SC_ZERO_TIME);

    testBackwardPath();
}

BOOST_FIXTURE_TEST_CASE(testMockInterfaceForwardPath,
                        TestEnvironmentMockInterface32) {
    factory.traverseAll();
    sc_core::sc_start(sc_core::SC_ZERO_TIME);

    testForwardPath();
}

BOOST_FIXTURE_TEST_CASE(testMockInterfaceForwardPath64,
                        TestEnvironmentMockInterface64) {
    factory.traverseAll();
    sc_core::sc_start(sc_core::SC_ZERO_TIME);

    testForwardPath();
}

BOOST_FIXTURE_TEST_CASE(testMockInterfaceBackwardPath,
                        TestEnvironmentMockInterface32) {
    factory.traverseAll();
    sc_core::sc_start(sc_core::SC_ZERO_TIME);

    testBackwardPath();
}

BOOST_FIXTURE_TEST_CASE(testMockInterfaceBackwardPath64,
                        TestEnvironmentMockInterface64) {
    factory.traverseAll();
    sc_core::sc_start(sc_core::SC_ZERO_TIME);

    testBackwardPath();
}

BOOST_AUTO_TEST_SUITE_END()
#endif  // INTC_EXT
