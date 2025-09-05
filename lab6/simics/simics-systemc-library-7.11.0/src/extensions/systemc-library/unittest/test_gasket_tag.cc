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

#include <boost/test/unit_test.hpp>

#include <simics/systemc/module_loaded.h>
#include <simics/systemc/gasket_tag.h>
#include <vector>

#include "stubs.h"

BOOST_AUTO_TEST_SUITE(TestGasketTag)

using simics::systemc::GasketTag;

class Env : public Stubs {
  public:
    Env() : ref_(&obj_), tag_(ref_) {
        simics::systemc::ModuleLoaded::set_module_name("module");
    }

    conf_object_t obj_;
    simics::ConfObjectRef ref_;
    GasketTag tag_;
};


BOOST_FIXTURE_TEST_CASE(testAddSimics2tlmGasket, Env) {
    tag_.addGasket("gasket_id_1", "simics2tlm-IoMemory");
    tag_.setGasketTag("gasket_id_1", "target",
                      SIM_make_attr_string("socket_name"));

    std::vector<simics::ConfObjectRef> gaskets = tag_.createGasketObjects();
    BOOST_CHECK_EQUAL(1, gaskets.size());

    attr_value_t attr = Stubs::instance_.SIM_create_object_attrs_;
    BOOST_CHECK_EQUAL(2, SIM_attr_list_size(attr));

    attr_value_t target_attr = SIM_attr_list_item(attr, 0);
    BOOST_CHECK_EQUAL(2, SIM_attr_list_size(target_attr));
    BOOST_CHECK_EQUAL(SIM_attr_string(SIM_attr_list_item(target_attr, 0)),
                      "target");
    BOOST_CHECK_EQUAL(SIM_attr_string(SIM_attr_list_item(target_attr, 1)),
                      "socket_name");

    attr_value_t simulation_attr = SIM_attr_list_item(attr, 1);
    BOOST_CHECK_EQUAL(2, SIM_attr_list_size(simulation_attr));
    BOOST_CHECK_EQUAL(SIM_attr_string(SIM_attr_list_item(simulation_attr, 0)),
                      "simulation");
    BOOST_CHECK_EQUAL(SIM_attr_object(SIM_attr_list_item(simulation_attr, 1)),
                      &obj_);

    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 0);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_get_class_name_,
                      "module_gasket_simics2tlm_IoMemory");
}

BOOST_FIXTURE_TEST_CASE(testMissingAddGasketInvocation, Env) {
    tag_.setGasketTag("gasket_id_1", "target",
                      SIM_make_attr_string("socket_name"));

    std::vector<simics::ConfObjectRef> gaskets = tag_.createGasketObjects();
    BOOST_CHECK_EQUAL(0, gaskets.size());
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 1);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_,
                      "Missing addGasket invocation for id: gasket_id_1");
}

BOOST_AUTO_TEST_SUITE_END()
