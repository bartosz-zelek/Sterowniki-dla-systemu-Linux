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

#include <boost/test/unit_test.hpp>

#include <simics/systemc/iface/instrumentation/bank_instrumentation_subscribe_interface.h>
#include <simics/systemc/iface/instrumentation/bank_instrumentation_subscribe_simics_adapter.h>

#include "mock/mock_simulation.h"

namespace sclii = simics::systemc::iface::instrumentation;
using sclii::BankInstrumentationSubscribeInterface;
using sclii::BankInstrumentationSubscribeSimicsAdapter;
using simics::ConfObject;

BOOST_AUTO_TEST_SUITE(TestBankInstrumentationSubscribeSimicsAdapter)

class MockProxy
    : public ConfObject,
      public BankInstrumentationSubscribeInterface,
      public unittest::MockSimulation {
  public:
    MockProxy() : ConfObject(NULL) {}
    virtual bank_callback_handle_t register_before_read(
            conf_object_t *connection,
            uint64_t offset,
            uint64_t size,
            before_read_callback_t before_read,
            lang_void *user_data) {
        offset_ = offset;
        size_ = size;
        br_cb_ = before_read;
        return handler_;
    }
    virtual bank_callback_handle_t register_after_read(
            conf_object_t *connection,
            uint64_t offset,
            uint64_t size,
            after_read_callback_t after_read,
            lang_void *user_data) {
        offset_ = offset;
        size_ = size;
        ar_cb_ = after_read;
        return handler_;
    }
    virtual bank_callback_handle_t register_before_write(
            conf_object_t *connection,
            uint64_t offset,
            uint64_t size,
            before_write_callback_t before_write,
            lang_void *user_data) {
        offset_ = offset;
        size_ = size;
        bw_cb_ = before_write;
        return handler_;
    }
    virtual bank_callback_handle_t register_after_write(
            conf_object_t *connection,
            uint64_t offset,
            uint64_t size,
            after_write_callback_t after_write,
            lang_void *user_data) {
        offset_ = offset;
        size_ = size;
        aw_cb_ = after_write;
        return handler_;
    }

    virtual void remove_callback(bank_callback_handle_t callback) {
        callback_ = callback;
    }
    virtual void remove_connection_callbacks(
            conf_object_t *NOTNULL connection) {}
    virtual void enable_connection_callbacks(
            conf_object_t *NOTNULL connection) {}
    virtual void disable_connection_callbacks(
            conf_object_t *NOTNULL connection) {}

    uint64_t offset_;
    uint64_t size_;
    before_read_callback_t br_cb_;
    after_read_callback_t ar_cb_;
    before_write_callback_t bw_cb_;
    after_write_callback_t aw_cb_;
    bank_callback_handle_t handler_;
    bank_callback_handle_t callback_;
};

class TestEnvironment {
  public:
    TestEnvironment() {
        conf_.instance_data = &proxy_;
    }
    ~TestEnvironment() {}

    MockProxy proxy_;
    conf_object_t conf_;

    BankInstrumentationSubscribeSimicsAdapter<MockProxy> adapter_;
};

BOOST_FIXTURE_TEST_CASE(testRegisterBeforeRead, TestEnvironment) {
    before_read_callback_t *before_read = new before_read_callback_t;
    bank_callback_handle_t ret = adapter_.register_before_read(
            &conf_, NULL, 0xab, 0xcd, *before_read, NULL);

    BOOST_CHECK_EQUAL(proxy_.offset_, 0xab);
    BOOST_CHECK_EQUAL(proxy_.size_, 0xcd);
    BOOST_CHECK_EQUAL(proxy_.br_cb_, *before_read);

    delete before_read;
}

BOOST_FIXTURE_TEST_CASE(testRegisterAfterRead, TestEnvironment) {
    after_read_callback_t *after_read = new after_read_callback_t;
    bank_callback_handle_t ret = adapter_.register_after_read(
            &conf_, NULL, 0xef, 0xbe, *after_read, NULL);

    BOOST_CHECK_EQUAL(proxy_.offset_, 0xef);
    BOOST_CHECK_EQUAL(proxy_.size_, 0xbe);
    BOOST_CHECK_EQUAL(proxy_.ar_cb_, *after_read);

    delete after_read;
}

BOOST_FIXTURE_TEST_CASE(testRegisterBeforeWrite, TestEnvironment) {
    before_write_callback_t *before_write = new before_write_callback_t;
    bank_callback_handle_t ret = adapter_.register_before_write(
            &conf_, NULL, 0x12, 0x34, *before_write, NULL);

    BOOST_CHECK_EQUAL(proxy_.offset_, 0x12);
    BOOST_CHECK_EQUAL(proxy_.size_, 0x34);
    BOOST_CHECK_EQUAL(proxy_.bw_cb_, *before_write);

    delete before_write;
}

BOOST_FIXTURE_TEST_CASE(testRegisterAfterWrite, TestEnvironment) {
    after_write_callback_t *after_write = new after_write_callback_t;
    bank_callback_handle_t ret = adapter_.register_after_write(
            &conf_, NULL, 0x56, 0x78, *after_write, NULL);

    BOOST_CHECK_EQUAL(proxy_.offset_, 0x56);
    BOOST_CHECK_EQUAL(proxy_.size_, 0x78);
    BOOST_CHECK_EQUAL(proxy_.aw_cb_, *after_write);

    delete after_write;
}

BOOST_FIXTURE_TEST_CASE(testRemoveCallback, TestEnvironment) {
    bank_callback_handle_t *callback = new bank_callback_handle_t;
    adapter_.remove_callback(&conf_, *callback);

    BOOST_CHECK_EQUAL(proxy_.callback_, *callback);

    delete callback;
}

BOOST_AUTO_TEST_SUITE_END()
