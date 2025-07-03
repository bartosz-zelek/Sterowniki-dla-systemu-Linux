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

#include <systemc>
#include <tlm>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>

#include <simics/systemc/iface/map_info_extension.h>
#include <simics/types/map_info.h>

#include "simcontext_wrapper.h"

BOOST_AUTO_TEST_SUITE(TestMapInfoExtension_standalone)

namespace sif = simics::systemc::iface;

class TestModule : public sc_core::sc_module {
  public:
    explicit TestModule(sc_core::sc_module_name = "TestModule") {
        sif::MapInfoExtension ext;
        payload_.set_extension<sif::MapInfoExtension>(&ext);
        payload_.clear_extension<sif::MapInfoExtension>();
    }

    tlm::tlm_generic_payload payload_;
};

class TestEnvironment {
  public:
    unittest::SimContextWrapper context_;
};

BOOST_FIXTURE_TEST_CASE(testScMain, TestEnvironment) {
    TestModule module;

    sc_core::sc_start(sc_core::SC_ZERO_TIME);
}

BOOST_AUTO_TEST_SUITE_END()
