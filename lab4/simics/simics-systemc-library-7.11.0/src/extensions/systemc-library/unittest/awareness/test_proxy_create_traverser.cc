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

#include <simics/base/types.h>
#include <simics/base/conf-object.h>
#include <simics/systemc/awareness/proxy_create_traverser.h>
#include <simics/systemc/awareness/proxy_class_registry.h>

#include "mock/awareness/mock_proxy.h"
#include "mock/awareness/mock_proxy_factory.h"
#include "mock/mock_sc_object.h"
#include "environment.h"
#include "stubs.h"

#include <map>
#include <string>
#include <vector>

BOOST_AUTO_TEST_SUITE(TestProxyCreateTraverser)

class TestEnvironment : public Environment {
  public:
    TestEnvironment()
        : registry_(proxy_factory_),
          traverser_(&root_, &simulation_, &proxy_factory_, &registry_,
                     &links_, &new_links_),
          object_("MockObject") {
        Stubs::instance_.SIM_clear_exception_return_ = SimExc_General;
        Stubs::instance_.created_obj_ = &proxy_;
    }
    unittest::awareness::MockProxy root_;
    std::map<sc_core::sc_object *,
             simics::systemc::awareness::ProxyInterface *> links_;
    std::map<sc_core::sc_object *,
             simics::systemc::awareness::ProxyInterface *> new_links_;
    unittest::awareness::MockProxyFactory proxy_factory_;
    simics::systemc::awareness::ProxyClassRegistry registry_;
    simics::systemc::awareness::ProxyCreateTraverser traverser_;
    Stubs stubs_;
    unittest::MockScObject object_;
    unittest::awareness::MockProxy proxy_;
};

BOOST_FIXTURE_TEST_CASE(testProxyCreation, TestEnvironment) {
    traverser_.applyOn(&object_);
    BOOST_CHECK_EQUAL(links_.size(), 1U);
    BOOST_CHECK_EQUAL(1, Stubs::instance_.SIM_create_object_count_);
}

BOOST_FIXTURE_TEST_CASE(testHandleSimCreateReturningNull, TestEnvironment) {
    Stubs::instance_.created_obj_ = NULL;
    traverser_.applyOn(&object_);
    BOOST_CHECK_EQUAL(links_.size(), 0U);
    BOOST_CHECK_EQUAL(1, Stubs::instance_.SIM_create_object_count_);
}

BOOST_FIXTURE_TEST_CASE(testHandleSimRegisterClassReturningNull,
                        TestEnvironment) {
    proxy_factory_.create_conf_class_return_ = NULL;
    traverser_.applyOn(&object_);
    BOOST_CHECK_EQUAL(0, Stubs::instance_.SIM_create_object_count_);
}

BOOST_FIXTURE_TEST_CASE(testObjectWithSimicsPortNameIsCreated,
                        TestEnvironment) {
    unittest::MockScObject object("MockObject[]");
    traverser_.applyOn(&object);
    BOOST_CHECK_EQUAL(1, Stubs::instance_.SIM_create_object_count_);
}

class MockTop : public sc_core::sc_module {
  public:
    MockTop(sc_core::sc_module_name top_name,
            std::string module_name)
        : sc_core::sc_module(top_name) {
        module_ = new unittest::MockScObject(module_name.c_str());
    }
    unittest::MockScObject *module_;
};

BOOST_FIXTURE_TEST_CASE(testObjectWithBadHierarchicalName,
                        TestEnvironment) {
    unittest::MockScObject *module;
    {
        // Create a top module with a heap-allocated sub-module, and keep the
        // sub-module when top module goes out of scope.
        // This mistake was noticed in a test-case by Rocco, so we need to
        // handle it gracefully in Simics.
        MockTop top("a", "b");
        module = top.module_;
        BOOST_CHECK(module->get_parent_object() == &top);

        // Verify that our assumptions on the current hierarchy is correct
        const std::vector<sc_core::sc_object*> &v1
            = sc_core::sc_get_top_level_objects();
        BOOST_CHECK_EQUAL(v1.size(), 2);  // MockObject + a
        BOOST_CHECK_EQUAL(std::count(v1.begin(), v1.end(), &object_), 1);
        BOOST_CHECK_EQUAL(std::count(v1.begin(), v1.end(), &top), 1);
        const std::vector<sc_core::sc_object*> &v2
            = top.get_child_objects();
        BOOST_CHECK_EQUAL(v2.size(), 1);
        BOOST_CHECK_EQUAL(v2.front(), module);
    }

    // Verify that after top module has gone out of scope, we are left with a
    // broken hierarchy
    const std::vector<sc_core::sc_object*> &v1
        = sc_core::sc_get_top_level_objects();
    BOOST_CHECK_EQUAL(v1.size(), 2);  // MockObject + a.b (a no longer present)
    BOOST_CHECK_EQUAL(std::count(v1.begin(), v1.end(), &object_), 1);
    BOOST_CHECK_EQUAL(std::count(v1.begin(), v1.end(), module), 1);
    BOOST_CHECK(module->get_parent_object() == NULL);

    // Verify that Simics does not try to create a proxy for this object
    traverser_.applyOn(module);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_create_object_count_, 0);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_spec_violation_count_, 1);
}

BOOST_AUTO_TEST_SUITE_END()
