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

#include <simics/systemc/awareness/attributes.h>
#include <simics/systemc/awareness/attribute.h>
#include <simics/systemc/awareness/sc_attribute.h>

#include "simcontext_wrapper.h"

using simics::systemc::awareness::Attributes;
using simics::systemc::awareness::Attribute;
using simics::systemc::awareness::ScAttribute;

class MockAccess {
  public:
    attr_value_t get(sc_core::sc_attr_base *attr) const {
        sc_core::sc_attribute<int> *a =
            dynamic_cast<sc_core::sc_attribute<int> *>(attr);
        return SIM_make_attr_uint64(a->value);
    }
    bool set(sc_core::sc_attr_base *attr, attr_value_t *val) {
        sc_core::sc_attribute<int> *a =
            dynamic_cast<sc_core::sc_attribute<int> *>(attr);
        a->value = SIM_attr_integer(*val);
        return true;
    }
};

class TestScAttributeModule : public sc_core::sc_module {
  public:
    SC_CTOR(TestScAttributeModule) : attr_("attr") {
        add_attribute(attr_);
    }
    sc_core::sc_attribute<int> attr_;
};

BOOST_AUTO_TEST_SUITE(TestScAttribute)

BOOST_AUTO_TEST_CASE(testGetSetScAttribute) {
    unittest::SimContextWrapper wrapper;
    TestScAttributeModule module("module");
    ScAttribute<MockAccess> *attr =
        Attributes::defineAttribute<ScAttribute<MockAccess> >();
    attr->init(&module, &module.attr_);
    int key = attr->key();

    {
        unittest::SimContextWrapper wrapper;
        TestScAttributeModule module2("module");
        Attributes attrs;
        attrs.init();
        Attribute *attr2 = attrs.attribute<Attribute>(key);
        BOOST_REQUIRE(attr2 != NULL);

        attr_value_t val = SIM_make_attr_uint64(1234);
        attr2->set(&val);
        BOOST_CHECK_EQUAL(module2.attr_.value, 1234);

        attr_value_t val2 = attr2->get();
        BOOST_CHECK_EQUAL(SIM_attr_integer(val2), 1234);
    }
}

BOOST_AUTO_TEST_SUITE_END()
