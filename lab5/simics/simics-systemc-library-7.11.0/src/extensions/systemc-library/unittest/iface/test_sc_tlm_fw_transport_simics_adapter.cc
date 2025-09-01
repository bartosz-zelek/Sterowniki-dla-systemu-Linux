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
#include <simics/systemc/iface/injection/sc_tlm_fw_transport_simics_adapter.h>

#include "mock/mock_simulation.h"
#include "stubs.h"

using simics::systemc::injection::InjectGp;
using simics::systemc::iface::injection::ScTlmFwTransportSimicsAdapter;
using simics::ConfObject;

BOOST_AUTO_TEST_SUITE(TestScTlmFwTransportSimicsAdapter)

template <typename TYPES>
class MockProxy
    : public ConfObject, public tlm::tlm_fw_transport_if<TYPES>,
      public unittest::MockSimulation {
  public:
    MockProxy() : ConfObject(NULL), phase_(tlm::UNINITIALIZED_PHASE),
                  sync_(tlm::TLM_COMPLETED),
                  start_range_(0), end_range_(0), invoked_(false) {
    }
    typedef typename TYPES::tlm_payload_type transaction_type;
    typedef typename TYPES::tlm_phase_type phase_type;

    virtual tlm::tlm_sync_enum nb_transport_fw(transaction_type &trans,  // NOLINT
                                               phase_type &phase,  // NOLINT
                                               sc_core::sc_time &offset) {  // NOLINT
        trans_.deep_copy_from(trans);
        phase_ = phase;
        offset_ = offset;
        invoked_ = true;
        return sync_;
    }

    virtual void b_transport(transaction_type &trans,  // NOLINT
                             sc_core::sc_time &t) {  // NOLINT
        trans_.deep_copy_from(trans);
        offset_ = t;
        invoked_ = true;
    }

    virtual bool get_direct_mem_ptr(transaction_type &trans,  // NOLINT
                                    tlm::tlm_dmi &dmi_data) {  // NOLINT
        trans_.deep_copy_from(trans);
        dmi_data_ = dmi_data;
        invoked_ = true;
        return true;
    }

    virtual unsigned int transport_dbg(transaction_type &trans) {  // NOLINT
        trans_.deep_copy_from(trans);
        invoked_ = true;
        return 0;
    }

    transaction_type trans_;
    phase_type phase_;
    sc_core::sc_time offset_;
    tlm::tlm_sync_enum sync_;
    tlm::tlm_dmi dmi_data_;
    sc_dt::uint64 start_range_;
    sc_dt::uint64 end_range_;
    bool invoked_;
};

class TestEnvironment {
  public:
    TestEnvironment() {
        attr_ = SIM_alloc_attr_dict(1);
        attr_dmi_ = SIM_alloc_attr_dict(1);
        conf_.instance_data = &proxy_;
    }
    ~TestEnvironment() {
        SIM_attr_free(&attr_);
        SIM_attr_free(&attr_dmi_);
    }
    void setAttrToValue(const char* key, attr_value_t value) {
        SIM_attr_dict_set_item(&attr_, 0, SIM_make_attr_string(key), value);
    }
    void setAttrToValueForDmi(const char* key, attr_value_t value) {
        SIM_attr_dict_set_item(&attr_dmi_, 0, SIM_make_attr_string(key), value);
    }

    MockProxy<tlm::tlm_base_protocol_types> proxy_;
    conf_object_t conf_;

    Stubs stubs_;
    attr_value_t attr_;
    attr_value_t attr_dmi_;
    ScTlmFwTransportSimicsAdapter<
        MockProxy<tlm::tlm_base_protocol_types> > adapter_;
    InjectGp<tlm::tlm_generic_payload> inject_gp_;
};

BOOST_FIXTURE_TEST_CASE(testNbTransportFw, TestEnvironment) {
    setAttrToValue("gp.command", SIM_make_attr_uint64(tlm::TLM_WRITE_COMMAND));
    sc_tlm_sync_enum_t ret = adapter_.nb_transport_fw(&conf_, attr_, 20, 100);

    BOOST_CHECK_EQUAL(SC_TLM_COMPLETED, ret);
    BOOST_CHECK_EQUAL(proxy_.trans_.get_command(), tlm::TLM_WRITE_COMMAND);
    BOOST_CHECK_EQUAL(proxy_.phase_, 20);
    BOOST_CHECK_EQUAL(proxy_.offset_, sc_core::sc_time::from_value(100));
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 0);
}

BOOST_FIXTURE_TEST_CASE(testNbTransportFwError, TestEnvironment) {
    setAttrToValue("gp.command", SIM_make_attr_uint64(3));
    sc_tlm_sync_enum_t ret = adapter_.nb_transport_fw(&conf_, attr_, 20, 100);

    BOOST_CHECK_EQUAL(SC_TLM_ATTR_ERROR, ret);
    BOOST_CHECK_EQUAL(proxy_.invoked_, false);
    BOOST_CHECK(Stubs::instance_.SIM_log_error_count_ > 0);
}

BOOST_FIXTURE_TEST_CASE(testBTransportFw, TestEnvironment) {
    setAttrToValue("gp.command", SIM_make_attr_uint64(tlm::TLM_READ_COMMAND));
    adapter_.b_transport(&conf_, attr_, 110);

    BOOST_CHECK_EQUAL(proxy_.trans_.get_command(), tlm::TLM_READ_COMMAND);
    BOOST_CHECK_EQUAL(proxy_.offset_, sc_core::sc_time::from_value(110));
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 0);
}

BOOST_FIXTURE_TEST_CASE(testBTransportFwError, TestEnvironment) {
    setAttrToValue("gp.command", SIM_make_attr_uint64(3));
    adapter_.b_transport(&conf_, attr_, 110);

    BOOST_CHECK_EQUAL(proxy_.invoked_, false);
    BOOST_CHECK(Stubs::instance_.SIM_log_error_count_ > 0);
}

BOOST_FIXTURE_TEST_CASE(testGetDirectMemPtr, TestEnvironment) {
    setAttrToValue("gp.command", SIM_make_attr_uint64(tlm::TLM_READ_COMMAND));
    setAttrToValueForDmi("dmi.start_address", SIM_make_attr_uint64(16));
    BOOST_CHECK_EQUAL(adapter_.get_direct_mem_ptr(&conf_, attr_, attr_dmi_),
                      true);

    BOOST_CHECK_EQUAL(proxy_.trans_.get_command(), tlm::TLM_READ_COMMAND);
    BOOST_CHECK_EQUAL(proxy_.dmi_data_.get_start_address(), 16);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 0);
}

BOOST_FIXTURE_TEST_CASE(testTransportDbg, TestEnvironment) {
    setAttrToValue("gp.address", SIM_make_attr_uint64(1234));
    adapter_.transport_dbg(&conf_, attr_);

    BOOST_CHECK_EQUAL(proxy_.trans_.get_address(), 1234);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 0);
}

BOOST_FIXTURE_TEST_CASE(testTransportDbgError, TestEnvironment) {
    setAttrToValue("gp.address", SIM_make_attr_floating(1.234));
    adapter_.transport_dbg(&conf_, attr_);

    BOOST_CHECK_EQUAL(proxy_.invoked_, false);
    BOOST_CHECK(Stubs::instance_.SIM_log_error_count_ > 0);
}

BOOST_AUTO_TEST_SUITE_END()
