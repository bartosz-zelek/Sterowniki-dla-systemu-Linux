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
#include <simics/systemc/iface/extension_dispatcher.h>

#include <unittest/mock/iface/mock_extension_sender.h>

BOOST_AUTO_TEST_SUITE(TestExtensionDispatcher)

class MockInterface {
  public:
    virtual void method() = 0;
    virtual ~MockInterface() {}
};

class MockDevice : public MockInterface {
  public:
    MockDevice() : call_(0) {}
    virtual void method() {
        ++call_;
    }

    int call_;
};

class MockExtension : public simics::systemc::iface::Extension<MockExtension,
                                                               MockInterface> {
  public:
    virtual void call(MockInterface *device) {
        device->method();
    }
    virtual void method() {
        send();
    }
};

class TestEnvironment {
  public:
    TestEnvironment()
        : extension_(new MockExtension), sender_(&payload_) {
        payload_.set_extension(extension_);
        extension_->init(&sender_);
        sender_.set_receiver(&dispatcher_);
    }
    MockExtension *extension_;
    tlm::tlm_generic_payload payload_;
    simics::systemc::iface::ExtensionDispatcher dispatcher_;
    MockDevice device_;
    MockExtensionSender sender_;
};

BOOST_FIXTURE_TEST_CASE(testDispatchWithSubscribedExtension, TestEnvironment) {
    dispatcher_.subscribe(MockExtension::createReceiver(&device_));
    extension_->method();

    BOOST_CHECK_EQUAL(device_.call_, 1);
}

BOOST_FIXTURE_TEST_CASE(testDispatchWithoutSubscribedExtension,
                        TestEnvironment) {
    extension_->method();

    BOOST_CHECK_EQUAL(device_.call_, 0);
}

BOOST_AUTO_TEST_SUITE_END()
