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

#include <boost/test/unit_test.hpp>

#include <simics/systemc/awareness/proxy_class_registry.h>

#include "mock/awareness/mock_proxy_factory.h"
#include "mock/mock_sc_object.h"
#include "stubs.h"

using unittest::awareness::MockProxyFactory;

BOOST_AUTO_TEST_SUITE(TestProxyClassRegistry)

class MockProxyFactoryWithSimicsException : public MockProxyFactory {
  public:
    virtual conf_class_t *createConfClass(sc_core::sc_object *object,
                                          std::string name,
                                          std::string description,
                                          std::string documentation) const {
        Stubs::instance_.SIM_clear_exception_return_ = SimExc_General;
        return NULL;
    }
};

BOOST_AUTO_TEST_CASE(testNoClassFound) {
    MockProxyFactory factory;
    factory.create_conf_class_return_ = NULL;
    simics::systemc::awareness::ProxyClassRegistry registry(factory);
    unittest::MockScObject object("Mock");
    Stubs::instance_.SIM_clear_exception_return_ = SimExc_General;

    conf_class_t *conf_class = registry.confClass(&object);

    BOOST_CHECK_EQUAL(conf_class, static_cast<conf_class_t *>(NULL));
    BOOST_CHECK_EQUAL(factory.create_conf_class_count_, 1);
    BOOST_CHECK_EQUAL(factory.register_attributes_count_, 0);
    BOOST_CHECK_EQUAL(factory.register_interfaces_count_, 0);
}

BOOST_AUTO_TEST_CASE(testBadClassNameClearsSimicsException) {
    MockProxyFactoryWithSimicsException factory;
    simics::systemc::awareness::ProxyClassRegistry registry(factory);
    unittest::MockScObject object("Mock");
    Stubs::instance_.SIM_clear_exception_return_ = SimExc_General;

    conf_class_t *conf_class = registry.confClass(&object);

    BOOST_CHECK_EQUAL(conf_class, static_cast<conf_class_t *>(NULL));
    BOOST_CHECK_EQUAL(SIM_clear_exception(), SimExc_No_Exception);
}

BOOST_AUTO_TEST_SUITE_END()
