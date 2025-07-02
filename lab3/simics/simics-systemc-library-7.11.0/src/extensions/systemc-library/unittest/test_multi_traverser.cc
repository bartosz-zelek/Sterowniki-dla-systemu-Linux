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

#include <simics/systemc/multi_traverser.h>

#include "mock/mock_sc_object.h"
#include "mock/mock_traverser.h"

BOOST_AUTO_TEST_SUITE(TestMultiTraverser)

class TestEnvironment {
  public:
    TestEnvironment() {
        multi_traverser_.add(&mock_traverser_);
    }
    unittest::MockTraverser mock_traverser_;
    simics::systemc::MultiTraverser multi_traverser_;
};

BOOST_FIXTURE_TEST_CASE(testApplyOn, TestEnvironment) {
    unittest::MockScObject mock_object("MockTestApplyOn");
    multi_traverser_.applyOn(&mock_object);
    BOOST_CHECK_EQUAL(mock_traverser_.apply_on_.size(), 1);
}

BOOST_FIXTURE_TEST_CASE(testDone, TestEnvironment) {
    multi_traverser_.done();
    BOOST_CHECK_EQUAL(mock_traverser_.done_count_, 1);
}

BOOST_AUTO_TEST_SUITE_END()
