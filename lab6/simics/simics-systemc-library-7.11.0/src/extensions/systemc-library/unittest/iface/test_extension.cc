// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2015 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <boost/test/unit_test.hpp>

#include <tlm>

#include <simics/systemc/iface/extension.h>
#include <simics/systemc/iface/extension_ignore_receiver.h>
#include <simics/systemc/iface/extension_receiver.h>
#include <simics/systemc/iface/extension_sender_interface.h>
#include <simics/systemc/iface/receiver_interface.h>
#include <simics/systemc/iface/transaction.h>

#include <unittest/mock/iface/mock_extension_sender.h>

using simics::systemc::iface::Extension;
using simics::systemc::iface::ExtensionIgnoreReceiver;
using simics::systemc::iface::ExtensionReceiver;
using simics::systemc::iface::ExtensionSenderInterface;
using simics::systemc::iface::ReceiverInterface;
using simics::systemc::iface::Transaction;

BOOST_AUTO_TEST_SUITE(TestExtension)

class MockInterface {
  public:
    virtual int method(int p) = 0;
    virtual ~MockInterface() {}
};

class MockExtension : public Extension<MockExtension, MockInterface> {
  public:
    virtual void call(MockInterface *device) {
        method_return_ = device->method(method_input_[0].value<int>());
    }
    virtual int method(int p) {
        method_ = 1;
        method_input_.push_back(p);
        send();
        return method_return_.value<int>();
    }
};

class MockExtensionInvalid
    : public Extension<MockExtensionInvalid, MockInterface> {
  public:
    virtual void call(MockInterface *device) {
        method_return_ = device->method(method_input_[0].value<int>());
    }
    virtual int method(int p) {
        method_ = 1;
        method_input_.push_back(p);
        send();
        return method_return_.value<int>();
    }
};

class MockExtensionWithDefaultReturn : public MockExtension {
  public:
    virtual int method(int p) {
        method_ = 1;
        method_input_.push_back(p);
        method_return_error_ = 1;
        send();
        return method_return_.value<int>();
    }
};

class MockDevice : public MockInterface {
  public:
    MockDevice() : method_p_(-1), method_cnt_(0) {}
    virtual int method(int p) {
        method_p_ = p;
        return ++method_cnt_;
    }

    int method_p_;
    int method_cnt_;
};

class MockDeviceReentry : public MockDevice {
  public:
    MockDeviceReentry()
        : MockDevice(), reentry_cnt_(1), parameter_(-1), extension_(NULL) {}
    virtual int method(int p) {
        ++method_cnt_;
        --reentry_cnt_;
        if (parameter_ < 0)
            parameter_ = p;

        BOOST_CHECK_EQUAL(parameter_, p);
        int r = method_cnt_;
        if (reentry_cnt_ >= 0) {
            parameter_ = p + 1;
            BOOST_CHECK_EQUAL(extension_->method(p + 1), r + 1);
        }
        return r;
    }
    int reentry_cnt_;
    int parameter_;
    MockExtension *extension_;
};

template<typename TExtension = MockExtension, typename TDevice = MockDevice>
class TestEnvironment {
  public:
    TestEnvironment()
        : receiver_(&device_), sender_(&payload_) {
        payload_.set_extension(&extension_);
        extension_.init(&sender_);
        sender_.set_receiver(&receiver_);
    }
    TDevice device_;
    TExtension extension_;
    tlm::tlm_generic_payload payload_;
    ExtensionReceiver<TExtension, MockInterface> receiver_;
    ExtensionIgnoreReceiver<TExtension> ignore_receiver_;
    MockExtensionSender sender_;
};

BOOST_FIXTURE_TEST_CASE(testSend, TestEnvironment<>) {
    BOOST_CHECK_EQUAL(extension_.method(1), 1);
    BOOST_CHECK_EQUAL(device_.method_cnt_, 1);
    BOOST_CHECK_EQUAL(device_.method_p_, 1);
    BOOST_CHECK_EQUAL(sender_.send_failed_cnt_, 0);
}

BOOST_FIXTURE_TEST_CASE(testSendInvalid, TestEnvironment<>) {
    MockExtensionInvalid invalidExtension;
    ExtensionReceiver<MockExtensionInvalid, MockInterface> receiver2(&device_);
    payload_.set_extension(&invalidExtension);
    sender_.set_receiver(&receiver2);

    BOOST_CHECK_EQUAL(extension_.method(1), 0);
    BOOST_CHECK_EQUAL(sender_.send_failed_cnt_, 1);

    payload_.clear_extension(&invalidExtension);
}

typedef TestEnvironment<MockExtension,
                        MockDeviceReentry> TestEnvironmentReentry;
BOOST_FIXTURE_TEST_CASE(testSendWithReentry, TestEnvironmentReentry) {
    device_.extension_ = &extension_;
    device_.reentry_cnt_ = 10;
    BOOST_CHECK_EQUAL(extension_.method(1), 1);
    BOOST_CHECK_EQUAL(device_.method_cnt_, 11);
    BOOST_CHECK_EQUAL(sender_.send_failed_cnt_, 0);
}

BOOST_FIXTURE_TEST_CASE(testSendFailed, TestEnvironment<>) {
    sender_.set_receiver(NULL);

    BOOST_CHECK_EQUAL(extension_.method(1), 0);
    BOOST_CHECK_EQUAL(device_.method_cnt_, 0);
    BOOST_CHECK_EQUAL(sender_.send_failed_cnt_, 1);
}

BOOST_FIXTURE_TEST_CASE(testSendFailedReturnDefaultValue,
                        TestEnvironment<MockExtensionWithDefaultReturn>) {
    sender_.set_receiver(NULL);

    BOOST_CHECK_EQUAL(extension_.method(1), 1);
    BOOST_CHECK_EQUAL(device_.method_cnt_, 0);
    BOOST_CHECK_EQUAL(sender_.send_failed_cnt_, 1);
}

BOOST_FIXTURE_TEST_CASE(testSendIgnore,
                        TestEnvironment<MockExtensionWithDefaultReturn>) {
    sender_.set_receiver(&ignore_receiver_);

    BOOST_CHECK_EQUAL(extension_.method(1), 1);
    BOOST_CHECK_EQUAL(device_.method_cnt_, 0);
    BOOST_CHECK_EQUAL(sender_.send_failed_cnt_, 0);
}

BOOST_AUTO_TEST_SUITE_END()
