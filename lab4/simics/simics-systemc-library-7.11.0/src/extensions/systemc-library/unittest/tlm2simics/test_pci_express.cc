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

#include <boost/assign/list_of.hpp>
#include <boost/test/unit_test.hpp>

#include <simics/devs/pci.h>
#if !defined(SIMICS_6_API)
#define SIMICS_6_API
#include <simics/systemc/iface/pci_express_extension.h>
#include <simics/systemc/tlm2simics/pci_express.h>
#undef SIMICS_6_API
#endif

#include <algorithm>
#include <vector>

#include "test_gasket_environment.h"

BOOST_AUTO_TEST_SUITE(TestTlm2SimicsPciExpress)

class MockPciExpressSimicsInterfaceImpl : public pci_express_interface_t {
  public:
    MockPciExpressSimicsInterfaceImpl() : dst_(NULL), src_(NULL),
                                          type_(PCIE_HP_Power_Indicator_Off),
                                          payload_(),
                                          send_message_called_(false) {
        instance_ = this;
        send_message = mock_send_message;
    }
    ~MockPciExpressSimicsInterfaceImpl() {
        delete[] payload_.str;
    }
    static int mock_send_message(conf_object_t *dst,
                                 conf_object_t *src,
                                 pcie_message_type_t type,
                                 byte_string_t payload) {
        instance_->send_message_called_ = true;

        instance_->dst_ = dst;
        instance_->src_ = src;
        instance_->type_ = type;

        instance_->payload_.len = payload.len;
        instance_->payload_.str = new uint8_t[payload.len];
        std::copy(payload.str, payload.str + payload.len,
                  instance_->payload_.str);

        return 4711;
    }

    static MockPciExpressSimicsInterfaceImpl *instance_;

    conf_object_t *dst_;
    conf_object_t *src_;
    pcie_message_type_t type_;
    byte_string_t payload_;

    bool send_message_called_;
};

MockPciExpressSimicsInterfaceImpl *
    MockPciExpressSimicsInterfaceImpl::instance_ = NULL;

class TestEnv
    : public TestGasketEnvironment<simics::systemc::tlm2simics::PciExpress,
                                   simics::systemc::iface::PciExpressExtension,
                                   MockPciExpressSimicsInterfaceImpl> {
  public:
    TestEnv() : TestGasketEnvironment(PCI_EXPRESS_INTERFACE) {}
};

BOOST_FIXTURE_TEST_CASE(testWrite, TestEnv) {
    std::vector<uint8_t> exp_payload = boost::assign::list_of(1)(2)(3);
    BOOST_CHECK_EQUAL(extension_.send_message(PCIE_HP_Power_Indicator_On,
                                              exp_payload), 4711);
    BOOST_CHECK_EQUAL(response_, tlm::TLM_OK_RESPONSE);

    BOOST_CHECK(interface_.send_message_called_);
    BOOST_CHECK_EQUAL(interface_.type_,
                      static_cast<int>(PCIE_HP_Power_Indicator_On));

    std::vector<uint8_t> got_payload(
        interface_.payload_.str,
        interface_.payload_.str + interface_.payload_.len);
    BOOST_CHECK_EQUAL_COLLECTIONS(got_payload.begin(), got_payload.end(),
                                  exp_payload.begin(), exp_payload.end());

    BOOST_CHECK_EQUAL(interface_.dst_, simulation_.simics_object().object());
    BOOST_CHECK_EQUAL(interface_.src_, simulation_.simics_object().object());
}

BOOST_AUTO_TEST_SUITE_END()
