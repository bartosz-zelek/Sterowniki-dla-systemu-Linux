// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2024 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <boost/test/unit_test.hpp>

#include <simics/systemc/pcie_tlm_extension.h>

BOOST_AUTO_TEST_SUITE(TestPcieTlmExtension)

BOOST_AUTO_TEST_CASE(testTlmExtension) {
    simics::systemc::PcieTlmExtension ext;

    // Verify initial value
    BOOST_CHECK_EQUAL(ext.type, simics::types::PCIE_Type_Not_Set);
    BOOST_CHECK_EQUAL(ext.msg_type_set, false);
    BOOST_CHECK_EQUAL(ext.device_id_set, false);
    BOOST_CHECK_EQUAL(ext.pasid_set, false);
    BOOST_CHECK_EQUAL(ext.requester_id_set, false);
    BOOST_CHECK_EQUAL(ext.ide_secured_set, false);

    // Clone the ext and modify all fields
    auto *ext_clone = ext.clone();
    ext.type = simics::types::PCIE_Type_IO;
    ext.msg_type_set = true;
    ext.msg_type = simics::types::PCIE_PRS_Request;
    ext.device_id_set = true;
    ext.device_id = 0x4321;
    ext.pasid_set = true;
    ext.pasid = 0xdeadbeef;
    ext.requester_id_set = true;
    ext.requester_id = 0x1234;
    ext.ide_secured_set = true;

    // Test copy_from method brings back the previous saved value
    ext.copy_from(*ext_clone);
    BOOST_CHECK_EQUAL(ext.type, simics::types::PCIE_Type_Not_Set);
    BOOST_CHECK_EQUAL(ext.msg_type_set, false);
    BOOST_CHECK_EQUAL(ext.device_id_set, false);
    BOOST_CHECK_EQUAL(ext.pasid_set, false);
    BOOST_CHECK_EQUAL(ext.requester_id_set, false);
    BOOST_CHECK_EQUAL(ext.ide_secured_set, false);
}

BOOST_AUTO_TEST_SUITE_END()
