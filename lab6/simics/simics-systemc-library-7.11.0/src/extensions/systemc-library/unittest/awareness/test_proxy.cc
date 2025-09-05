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

#include <systemc>
#include <boost/test/unit_test.hpp>
#include <simics/systemc/awareness/attributes.h>
#include <simics/systemc/awareness/proxy.h>

#include "mock/awareness/mock_attribute.h"
#include "mock/mock_sc_object.h"
#include "mock/mock_simulation.h"
#include "stubs.h"

using simics::systemc::awareness::Attributes;
using simics::systemc::awareness::Attribute;
using simics::systemc::awareness::Proxy;

BOOST_AUTO_TEST_SUITE(TestProxyAttributes)

BOOST_AUTO_TEST_CASE(test_SetGetAttribute) {
    unittest::MockScObject sc_obj("Mock");
    conf_object_t conf_;
    simics::ConfObjectRef simics_obj(&conf_);

    unittest::MockSimulation simulation_;

    Proxy p(simics_obj);
    p.init(&sc_obj, &simulation_);
    conf_.instance_data = &p;

    int key = Attributes::defineAttribute<
        unittest::awareness::MockAttribute>()->key();

    Attributes attributes1;
    attributes1.init();

    p.set_attributes(&attributes1);

    unittest::awareness::MockAttribute *attribute1
        = attributes1.attribute<unittest::awareness::MockAttribute>(key);
    BOOST_CHECK_EQUAL(attribute1->init_, 1);
}

BOOST_AUTO_TEST_CASE(test_sc_print) {
    unittest::MockScObject sc_obj("Mock");
    conf_object_t conf_;
    simics::ConfObjectRef simics_obj(&conf_);

    unittest::MockSimulation simulation_;

    Proxy p(simics_obj);
    p.init(&sc_obj, &simulation_);
    conf_.instance_data = &p;

    sc_obj.print_string_ = "a\n";
    std::vector<std::string> lines = p.sc_print();
    BOOST_REQUIRE_EQUAL(lines.size(), 1);
    BOOST_CHECK_EQUAL(lines[0], "a");

    sc_obj.print_string_ = "a \n";
    lines = p.sc_print();
    BOOST_REQUIRE_EQUAL(lines.size(), 1);
    BOOST_CHECK_EQUAL(lines[0], "a");

    sc_obj.print_string_ = "a \n ";
    lines = p.sc_print();
    BOOST_REQUIRE_EQUAL(lines.size(), 1);
    BOOST_CHECK_EQUAL(lines[0], "a");

    sc_obj.print_string_ = "a\n ";
    lines = p.sc_print();
    BOOST_REQUIRE_EQUAL(lines.size(), 1);
    BOOST_CHECK_EQUAL(lines[0], "a");

    sc_obj.print_string_ = "\na";
    lines = p.sc_print();
    BOOST_REQUIRE_EQUAL(lines.size(), 1);
    BOOST_CHECK_EQUAL(lines[0], "a");

    sc_obj.print_string_ = "\n a";
    lines = p.sc_print();
    BOOST_REQUIRE_EQUAL(lines.size(), 1);
    BOOST_CHECK_EQUAL(lines[0], "a");

    sc_obj.print_string_ = " \na";
    lines = p.sc_print();
    BOOST_REQUIRE_EQUAL(lines.size(), 1);
    BOOST_CHECK_EQUAL(lines[0], "a");

    sc_obj.print_string_ = " \n a";
    lines = p.sc_print();
    BOOST_REQUIRE_EQUAL(lines.size(), 1);
    BOOST_CHECK_EQUAL(lines[0], "a");

    sc_obj.print_string_ = "\r\n\n\ra\r b - b \nc\r\nd";
    lines = p.sc_print();
    BOOST_REQUIRE_EQUAL(lines.size(), 4);
    BOOST_CHECK_EQUAL(lines[0], "a");
    BOOST_CHECK_EQUAL(lines[1], "b - b");
    BOOST_CHECK_EQUAL(lines[2], "c");
    BOOST_CHECK_EQUAL(lines[3], "d");
}

BOOST_AUTO_TEST_SUITE_END()
