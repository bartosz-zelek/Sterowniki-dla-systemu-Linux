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

#include <simics/systemc/awareness/class_attributes.h>
#include <vector>

#include "simcontext_wrapper.h"

using simics::systemc::awareness::ClassAttributes;

BOOST_AUTO_TEST_SUITE(TestClassAttributes)

class TestModuleArray : public sc_core::sc_module {
  public:
    SC_CTOR(TestModuleArray) {}
    sc_core::sc_in<bool> array_[1];
};

class TestModuleVector : public sc_core::sc_module {
  public:
    SC_CTOR(TestModuleVector) : vector_("v", 1) {}
    sc_core::sc_vector< sc_core::sc_in<bool> > vector_;
};

BOOST_AUTO_TEST_CASE(testArrayOfObjects) {
    TestModuleArray device("device");
    ClassAttributes attributes;

    const std::vector<sc_core::sc_object*> &children
        = device.get_child_objects();
    BOOST_REQUIRE_EQUAL(children.size(), 1);

    BOOST_CHECK_EQUAL(attributes.name(*children.begin()), "sc_in");
    BOOST_CHECK_EQUAL(attributes.uniqueName(*children.begin()),
                      "sc_in_device_port_0");
}

BOOST_AUTO_TEST_CASE(testVectorOfObjects) {
    unittest::SimContextWrapper wrapper;
    TestModuleVector device("device");
    ClassAttributes attributes;

    const std::vector<sc_core::sc_object*> &children
        = device.get_child_objects();
    BOOST_REQUIRE_EQUAL(children.size(), 2);

    BOOST_CHECK_EQUAL(attributes.name(children[0]), "sc_vector");
    BOOST_CHECK_EQUAL(attributes.name(children[1]), "sc_in");
    BOOST_CHECK_EQUAL(attributes.uniqueName(children[0]), "sc_vector_device_v");
    BOOST_CHECK_EQUAL(attributes.uniqueName(children[1]), "sc_in_device_v_0");
}

BOOST_AUTO_TEST_CASE(testAdapterClassName) {
    ClassAttributes::set_adapter_class_name("prefix");
    TestModuleArray device("device");
    ClassAttributes attributes;

    const std::vector<sc_core::sc_object*> &children
        = device.get_child_objects();
    BOOST_REQUIRE_EQUAL(children.size(), 1);

    BOOST_CHECK_EQUAL(attributes.name(*children.begin()), "prefix_sc_in");
    BOOST_CHECK_EQUAL(attributes.uniqueName(*children.begin()),
                      "prefix_sc_in_device_port_0");
}


BOOST_AUTO_TEST_SUITE_END()
