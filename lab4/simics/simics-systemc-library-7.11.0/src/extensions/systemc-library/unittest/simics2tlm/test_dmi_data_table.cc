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

#include <tlm>
#include <simics/systemc/simics2tlm/dmi_data_table.h>

BOOST_AUTO_TEST_SUITE(TestDmiDataTable)

class TestEnvironment {
  public:
    TestEnvironment() {
        dmi_read_.set_dmi_ptr(data_);
        dmi_read_.set_start_address(32);
        dmi_read_.set_end_address(47);
        dmi_read_.set_granted_access(tlm::tlm_dmi::DMI_ACCESS_READ);

        dmi_read2_.set_dmi_ptr(data2_);
        dmi_read2_.set_start_address(16);
        dmi_read2_.set_end_address(31);
        dmi_read2_.set_granted_access(tlm::tlm_dmi::DMI_ACCESS_READ);

        dmi_write_.set_dmi_ptr(data_);
        dmi_write_.set_start_address(32);
        dmi_write_.set_end_address(47);
        dmi_write_.set_granted_access(tlm::tlm_dmi::DMI_ACCESS_WRITE);

        dmi_read_and_write_.set_dmi_ptr(data_);
        dmi_read_and_write_.set_start_address(32);
        dmi_read_and_write_.set_end_address(47);
        dmi_read_and_write_.set_granted_access(
            tlm::tlm_dmi::DMI_ACCESS_READ_WRITE);
    }
    simics::systemc::simics2tlm::DmiDataTable table_;
    unsigned char data_[16];
    unsigned char data2_[16];
    tlm::tlm_dmi dmi_read_;
    tlm::tlm_dmi dmi_read2_;
    tlm::tlm_dmi dmi_write_;
    tlm::tlm_dmi dmi_read_and_write_;
};

BOOST_FIXTURE_TEST_CASE(TestDmiPointerEmptyTable, TestEnvironment) {
    BOOST_CHECK(0 == table_.dmiPointer(0, 0, true));
    BOOST_CHECK(0 == table_.dmiPointer(0, 64, true));
    BOOST_CHECK(0 == table_.dmiPointer(32, 64, true));

    BOOST_CHECK(0 == table_.dmiPointer(0, 0, false));
    BOOST_CHECK(0 == table_.dmiPointer(0, 64, false));
    BOOST_CHECK(0 == table_.dmiPointer(32, 64, false));
}

BOOST_FIXTURE_TEST_CASE(TestDmiPointerReadAccess, TestEnvironment) {
    table_.add(dmi_read_);

    BOOST_CHECK(0 == table_.dmiPointer(0, 0, true));
    BOOST_CHECK(0 == table_.dmiPointer(16, 16, true));
    BOOST_CHECK(0 == table_.dmiPointer(34, 16, true));
    BOOST_CHECK(data_ == table_.dmiPointer(32, 16, true));
    BOOST_CHECK(data_ == table_.dmiPointer(32, 14, true));
    BOOST_CHECK(&data_[2] == table_.dmiPointer(34, 14, true));

    BOOST_CHECK(0 == table_.dmiPointer(32, 16, false));
    BOOST_CHECK(0 == table_.dmiPointer(32, 14, false));
    BOOST_CHECK(0 == table_.dmiPointer(34, 14, false));
}

BOOST_FIXTURE_TEST_CASE(TestDmiPointerWriteAccess, TestEnvironment) {
    table_.add(dmi_write_);

    BOOST_CHECK(0 == table_.dmiPointer(0, 0, false));
    BOOST_CHECK(0 == table_.dmiPointer(16, 16, false));
    BOOST_CHECK(0 == table_.dmiPointer(34, 16, false));
    BOOST_CHECK(data_ == table_.dmiPointer(32, 16, false));
    BOOST_CHECK(data_ == table_.dmiPointer(32, 14, false));
    BOOST_CHECK(&data_[2] == table_.dmiPointer(34, 14, false));

    BOOST_CHECK(0 == table_.dmiPointer(32, 16, true));
    BOOST_CHECK(0 == table_.dmiPointer(32, 14, true));
    BOOST_CHECK(0 == table_.dmiPointer(34, 14, true));
}

BOOST_FIXTURE_TEST_CASE(TestDmiPointerReadAndWriteAccess, TestEnvironment) {
    table_.add(dmi_read_and_write_);

    BOOST_CHECK(0 == table_.dmiPointer(0, 0, false));
    BOOST_CHECK(0 == table_.dmiPointer(16, 16, false));
    BOOST_CHECK(0 == table_.dmiPointer(34, 16, false));
    BOOST_CHECK(data_ == table_.dmiPointer(32, 16, false));
    BOOST_CHECK(data_ == table_.dmiPointer(32, 14, false));
    BOOST_CHECK(&data_[2] == table_.dmiPointer(34, 14, false));

    BOOST_CHECK(0 == table_.dmiPointer(0, 0, true));
    BOOST_CHECK(0 == table_.dmiPointer(16, 16, true));
    BOOST_CHECK(0 == table_.dmiPointer(34, 16, true));
    BOOST_CHECK(data_ == table_.dmiPointer(32, 16, true));
    BOOST_CHECK(data_ == table_.dmiPointer(32, 14, true));
    BOOST_CHECK(&data_[2] == table_.dmiPointer(34, 14, true));
}

BOOST_FIXTURE_TEST_CASE(TestAddDmiCreateOrderHighLow, TestEnvironment) {
    table_.add(dmi_read_);
    table_.add(dmi_read2_);

    BOOST_CHECK(data_ == table_.dmiPointer(32, 16, true));
    BOOST_CHECK(data2_ == table_.dmiPointer(16, 16, true));
}

BOOST_FIXTURE_TEST_CASE(TestAddDmiCreateOrderLowHigh, TestEnvironment) {
    table_.add(dmi_read2_);
    table_.add(dmi_read_);

    BOOST_CHECK(data_ == table_.dmiPointer(32, 16, true));
    BOOST_CHECK(data2_ == table_.dmiPointer(16, 16, true));
}

BOOST_FIXTURE_TEST_CASE(TestRemoveDmi, TestEnvironment) {
    table_.add(dmi_read_);

    table_.remove(32, 47);
    BOOST_CHECK(0 == table_.dmiPointer(32, 16, true));
}

BOOST_FIXTURE_TEST_CASE(TestRemoveDmiByteAtStart, TestEnvironment) {
    table_.add(dmi_read_);

    table_.remove(32, 32);
    BOOST_CHECK(&data_[1] == table_.dmiPointer(33, 1, true));
    BOOST_CHECK(&data_[1] == table_.dmiPointer(33, 15, true));
    BOOST_CHECK(0 == table_.dmiPointer(33, 16, true));
    BOOST_CHECK(0 == table_.dmiPointer(32, 1, true));
}

BOOST_FIXTURE_TEST_CASE(TestRemoveDmiByteAtEnd, TestEnvironment) {
    table_.add(dmi_read_);

    table_.remove(47, 47);
    BOOST_CHECK(&data_[0] == table_.dmiPointer(32, 15, true));
    BOOST_CHECK(&data_[14] == table_.dmiPointer(46, 1, true));
    BOOST_CHECK(0 == table_.dmiPointer(32, 16, true));
    BOOST_CHECK(0 == table_.dmiPointer(47, 1, true));
}

BOOST_FIXTURE_TEST_CASE(TestRemoveDmiBytesInMiddle, TestEnvironment) {
    table_.add(dmi_read_);

    table_.remove(39, 40);
    BOOST_CHECK(&data_[0] == table_.dmiPointer(32, 7, true));
    BOOST_CHECK(0 == table_.dmiPointer(32, 8, true));
    BOOST_CHECK(&data_[6] == table_.dmiPointer(38, 1, true));
    BOOST_CHECK(0 == table_.dmiPointer(38, 2, true));
    BOOST_CHECK(0 == table_.dmiPointer(38, 4, true));
    BOOST_CHECK(0 == table_.dmiPointer(39, 1, true));
    BOOST_CHECK(0 == table_.dmiPointer(40, 1, true));
    BOOST_CHECK(&data_[9] == table_.dmiPointer(41, 1, true));
    BOOST_CHECK(&data_[9] == table_.dmiPointer(41, 7, true));
    BOOST_CHECK(0 == table_.dmiPointer(41, 8, true));
}

BOOST_FIXTURE_TEST_CASE(TestRemoveDmiBytesInTwoSegments, TestEnvironment) {
    table_.add(dmi_read_);
    table_.add(dmi_read2_);

    BOOST_CHECK(data_ == table_.dmiPointer(32, 16, true));
    BOOST_CHECK(data2_ == table_.dmiPointer(16, 16, true));

    table_.remove(31, 32);

    BOOST_CHECK(&data_[1] == table_.dmiPointer(33, 1, true));
    BOOST_CHECK(&data_[1] == table_.dmiPointer(33, 15, true));
    BOOST_CHECK(0 == table_.dmiPointer(33, 16, true));
    BOOST_CHECK(0 == table_.dmiPointer(32, 1, true));

    BOOST_CHECK(&data2_[0] == table_.dmiPointer(16, 15, true));
    BOOST_CHECK(&data2_[14] == table_.dmiPointer(30, 1, true));
    BOOST_CHECK(0 == table_.dmiPointer(16, 16, true));
    BOOST_CHECK(0 == table_.dmiPointer(31, 1, true));
}

BOOST_AUTO_TEST_SUITE_END()
