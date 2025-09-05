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

#include <simics/systemc/injection/inject_gp.h>
#include <simics/systemc/iface/injection/sc_tlm_bw_transport_simics_adapter.h>

#include "mock/mock_simulation.h"
#include "stubs.h"

using simics::systemc::injection::InjectGp;
using simics::systemc::iface::injection::ScTlmBwTransportSimicsAdapter;
using simics::ConfObject;

BOOST_AUTO_TEST_SUITE(TestScTlmBwTransportSimicsAdapter)

template <typename TYPES>
class MockProxy
    : public ConfObject, public tlm::tlm_bw_transport_if<TYPES>,
      public unittest::MockSimulation {
  public:
    MockProxy() : ConfObject(NULL), phase_(tlm::UNINITIALIZED_PHASE),
                  sync_(tlm::TLM_COMPLETED),
                  start_range_(0), end_range_(0), invoked_(false) {
    }
    typedef typename TYPES::tlm_payload_type transaction_type;
    typedef typename TYPES::tlm_phase_type phase_type;

    virtual tlm::tlm_sync_enum nb_transport_bw(transaction_type &trans,  // NOLINT
                                               phase_type &phase,  // NOLINT
                                               sc_core::sc_time &offset) {  // NOLINT
        trans_.deep_copy_from(trans);
        phase_ = phase;
        offset_ = offset;
        invoked_ = true;
        return sync_;
    }

    virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range,
                                           sc_dt::uint64 end_range) {
        start_range_ = start_range;
        end_range_ = end_range;
        invoked_ = true;
    }

    transaction_type trans_;
    phase_type phase_;
    sc_core::sc_time offset_;
    tlm::tlm_sync_enum sync_;
    sc_dt::uint64 start_range_;
    sc_dt::uint64 end_range_;
    bool invoked_;
};

class TestEnvironment {
  public:
    TestEnvironment() {
        attr_ = SIM_alloc_attr_dict(1);
        conf_.instance_data = &proxy_;
    }
    ~TestEnvironment() {
        SIM_attr_free(&attr_);
    }
    void setAttrToValue(const char* key, attr_value_t value) {
        SIM_attr_dict_set_item(&attr_, 0, SIM_make_attr_string(key), value);
    }

    MockProxy<tlm::tlm_base_protocol_types> proxy_;
    conf_object_t conf_;

    Stubs stubs_;
    attr_value_t attr_;
    ScTlmBwTransportSimicsAdapter<
        MockProxy<tlm::tlm_base_protocol_types> > adapter_;
    InjectGp<tlm::tlm_generic_payload> inject_gp_;
};

BOOST_FIXTURE_TEST_CASE(testNbTransportBw, TestEnvironment) {
    setAttrToValue("gp.command", SIM_make_attr_uint64(tlm::TLM_WRITE_COMMAND));
    sc_tlm_sync_enum_t ret = adapter_.nb_transport_bw(&conf_, attr_, 20, 100);

    BOOST_CHECK_EQUAL(SC_TLM_COMPLETED, ret);
    BOOST_CHECK_EQUAL(proxy_.trans_.get_command(), tlm::TLM_WRITE_COMMAND);
    BOOST_CHECK_EQUAL(proxy_.phase_, 20);
    BOOST_CHECK_EQUAL(proxy_.offset_, sc_core::sc_time::from_value(100));
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 0);
}

BOOST_FIXTURE_TEST_CASE(testNbTransportBwError, TestEnvironment) {
    setAttrToValue("gp.command", SIM_make_attr_uint64(3));
    sc_tlm_sync_enum_t ret = adapter_.nb_transport_bw(&conf_, attr_, 20, 100);

    BOOST_CHECK_EQUAL(SC_TLM_ATTR_ERROR, ret);
    BOOST_CHECK_EQUAL(proxy_.invoked_, false);
    BOOST_CHECK(Stubs::instance_.SIM_log_error_count_ > 0);
}

BOOST_FIXTURE_TEST_CASE(testInvalidateDirectMemPtr, TestEnvironment) {
    adapter_.invalidate_direct_mem_ptr(&conf_, 16, 64);
    BOOST_CHECK_EQUAL(proxy_.start_range_, 16);
    BOOST_CHECK_EQUAL(proxy_.end_range_, 64);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 0);
}


BOOST_AUTO_TEST_SUITE_END()
