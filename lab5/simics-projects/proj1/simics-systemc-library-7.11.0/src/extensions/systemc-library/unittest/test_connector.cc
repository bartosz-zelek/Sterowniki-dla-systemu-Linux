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

#include <simics/cc-api.h>
#include <simics/systemc/connector.h>

BOOST_AUTO_TEST_SUITE(TestConnector)

using simics::ConfObjectRef;
using simics::systemc::Connector;
using simics::systemc::ConnectorProxy;

namespace {
class MockInterfaceProvider {
  public:
    MockInterfaceProvider() : obj_(NULL), fun_called_(0) {}
    explicit MockInterfaceProvider(const ConfObjectRef &obj)
        : obj_(obj), fun_called_(0) {}

    void set_target(ConfObjectRef o) {
        obj_ = o;
    }

    void fun() const {
        fun_called_++;
    }

    ConfObjectRef obj_;
    mutable int fun_called_;
};

class TestEnvironment {
  public:
    TestEnvironment() : target_(&conf_) {}

    conf_object_t conf_;
    simics::ConfObjectRef target_;
    Connector<MockInterfaceProvider> connector_;
};

}  // namespace

BOOST_FIXTURE_TEST_CASE(testConnector, TestEnvironment) {
    MockInterfaceProvider &iface_provider = connector_.provider();
    BOOST_CHECK_EQUAL(iface_provider.obj_.object(), nullptr);

    connector_.set(target_);
    BOOST_CHECK_EQUAL(iface_provider.obj_, target_);
    BOOST_CHECK_EQUAL(target_, connector_.get());

    BOOST_CHECK_EQUAL(iface_provider.fun_called_, 0);
    connector_->fun();
    BOOST_CHECK_EQUAL(iface_provider.fun_called_, 1);

    const Connector<MockInterfaceProvider> &con_ref = connector_;
    con_ref->fun();
    BOOST_CHECK_EQUAL(iface_provider.fun_called_, 2);
    const MockInterfaceProvider &iface_provider_ref = con_ref.provider();
    BOOST_CHECK_EQUAL(&iface_provider, &iface_provider_ref);
}

struct ConnectorsSetEnv {
    ConnectorsSetEnv() : c_1_(&c_root_), c_2_(&c_1_) {}
    Connector<MockInterfaceProvider> c_root_;
    ConnectorProxy<MockInterfaceProvider> c_1_;
    ConnectorProxy<MockInterfaceProvider> c_2_;
};

BOOST_FIXTURE_TEST_CASE(testConnectorProxySet, ConnectorsSetEnv) {
    conf_object_t conf;
    simics::ConfObjectRef target(&conf);

    c_root_.set(target);

    BOOST_CHECK_EQUAL(c_root_.provider().obj_, target);
    BOOST_CHECK_EQUAL(c_1_.provider().obj_, target);
    BOOST_CHECK_EQUAL(c_2_.provider().obj_, target);

    BOOST_CHECK_EQUAL(c_root_.get(), target);
    BOOST_CHECK_EQUAL(c_1_.get(), target);
    BOOST_CHECK_EQUAL(c_2_.get(), target);
}

struct ConnectorsSetEnvVar2 {
    ConnectorsSetEnvVar2() : c_1_(&c_root_), c_2_(&c_root_) {}
    Connector<MockInterfaceProvider> c_root_;
    ConnectorProxy<MockInterfaceProvider> c_1_;
    ConnectorProxy<MockInterfaceProvider> c_2_;
};

BOOST_FIXTURE_TEST_CASE(testConnectorProxySetVar2, ConnectorsSetEnvVar2) {
    conf_object_t conf;
    simics::ConfObjectRef target(&conf);

    c_root_.set(target);

    BOOST_CHECK_EQUAL(c_root_.provider().obj_, target);
    BOOST_CHECK_EQUAL(c_1_.provider().obj_, target);
    BOOST_CHECK_EQUAL(c_2_.provider().obj_, target);

    BOOST_CHECK_EQUAL(c_root_.get(), target);
    BOOST_CHECK_EQUAL(c_1_.get(), target);
    BOOST_CHECK_EQUAL(c_2_.get(), target);

    conf_object_t conf2;
    simics::ConfObjectRef target2(&conf2);

    c_root_.set(target2);

    BOOST_CHECK_EQUAL(c_root_.provider().obj_, target2);
    BOOST_CHECK_EQUAL(c_1_.provider().obj_, target2);
    BOOST_CHECK_EQUAL(c_2_.provider().obj_, target2);

    BOOST_CHECK_EQUAL(c_root_.get(), target2);
    BOOST_CHECK_EQUAL(c_1_.get(), target2);
    BOOST_CHECK_EQUAL(c_2_.get(), target2);
}

BOOST_FIXTURE_TEST_CASE(testConnectorAttribute, TestEnvironment) {
    attr_value_t attr = simics::std_to_attr<>(connector_);
    BOOST_CHECK(SIM_attr_is_nil(attr) == true);

    connector_.set(target_);
    BOOST_CHECK_EQUAL(target_, connector_.get());

    attr = simics::std_to_attr<>(connector_);
    BOOST_CHECK(SIM_attr_is_object(attr) == true);
    BOOST_CHECK(simics::attr_to_std<simics::ConfObjectRef>(attr) == target_);
    auto result = simics::attr_to_std<Connector<MockInterfaceProvider>>(attr);
    BOOST_CHECK(result == connector_);
}

BOOST_AUTO_TEST_CASE(testConnectorArrayAttribute) {
    std::array<Connector<MockInterfaceProvider>, 2> connectors;
    attr_value_t attr = simics::std_to_attr<>(connectors);
    auto result = simics::attr_to_std<
        std::array<Connector<MockInterfaceProvider>, 2>>(attr);
    BOOST_CHECK(result == connectors);
}

BOOST_AUTO_TEST_SUITE_END()
