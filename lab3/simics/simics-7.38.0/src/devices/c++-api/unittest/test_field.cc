// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2022 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <simics/field.h>

#include <gtest/gtest.h>

#include <string>
#include <utility>

#include "bank-object-fixture.h"
#include "mock/gtest_extensions.h"  // EXPECT_PRED_THROW
#include "mock/stubs.h"

namespace {
bool checkEmptyName(const std::exception &ex) {
    EXPECT_STREQ(ex.what(), "Cannot set with invalid name string: ");
    return true;
}

bool checkInvalidName(const std::exception &ex) {
    EXPECT_STREQ(ex.what(),
                 "Field name (b1.r2) does not match the field level"
                 " (bankA.registerB.fieldC)");
    return true;
}
}  // namespace

TEST_F(BankObjectFixture, TestFieldCreation) {
    // Empty name is not allowed
    EXPECT_PRED_THROW(simics::Field(&map_obj, ""),
                      std::invalid_argument, checkEmptyName);

    // The name has incorrect hierarchy level
    EXPECT_PRED_THROW(simics::Field(&map_obj, "b1.r2"),
                      std::invalid_argument, checkInvalidName);

    // The indices are allowed in the name
    simics::Field f1 {&map_obj, "b0.r1.f1[6]"};
    EXPECT_EQ(f1.name(), "f1[6]");

    std::string f2_name {"b0.r1.f2"};
    simics::Field f2 {&map_obj, f2_name};
    EXPECT_EQ(f2.name(), "f2");
    // Clear f2_name does not affect field f2's name
    f2_name.clear();
    EXPECT_EQ(f2.name(), "f2");
}

TEST_F(BankObjectFixture, TestFieldCTOR) {
    EXPECT_FALSE(std::is_copy_constructible<simics::Field>::value);
    EXPECT_TRUE(std::is_move_constructible<simics::Field>::value);

    auto f1 = simics::Field(&map_obj, "b0.r1.f3");

    // Field interface is registered on the map_obj
    auto *iface = map_obj.get_iface<simics::FieldInterface>("b0.r1.f3");
    EXPECT_EQ(iface, &f1);

    uint8_t test_field {0xab};
    // Test set and get bits
    simics::bits_type slice {{&test_field, 0xff}};
    f1.init("NA", slice, 0);
    EXPECT_EQ(f1.number_of_bits(), 8);
    EXPECT_EQ(f1.get(), 0xab);

    // Test add and get description
    f1.set_description("foo");
    EXPECT_EQ(f1.description(), "foo");
    f1.set_description("bar");
    EXPECT_EQ(f1.description(), "bar");

    // Verify field can be moved
    auto f2 {std::move(f1)};
    EXPECT_EQ(f2.name(), "f3");
    EXPECT_EQ(f2.description(), "bar");
    iface = map_obj.get_iface<simics::FieldInterface>("b0.r1.f3");
    EXPECT_EQ(iface, &f2);
    EXPECT_EQ(f2.number_of_bits(), 8);
    EXPECT_EQ(f2.get(), 0xab);

    // f1 is empty now (moved)
    EXPECT_EQ(f1.number_of_bits(), 0);

    f1 = std::move(f2);
    EXPECT_EQ(f1.name(), "f3");
    EXPECT_EQ(f1.description(), "bar");
    iface = map_obj.get_iface<simics::FieldInterface>("b0.r1.f3");
    EXPECT_EQ(iface, &f1);
    EXPECT_EQ(f1.number_of_bits(), 8);
    EXPECT_EQ(f1.get(), 0xab);

    // f2 is empty now (moved)
    EXPECT_EQ(f2.number_of_bits(), 0);
}

TEST_F(BankObjectFixture, TestFieldValue) {
    simics::Field f {&map_obj, "b0.r1.f2"};

    simics::bits_type slice;
    // Empty bits
    f.set_bits(slice);
    EXPECT_EQ(f.get(), 0);

    uint8_t bits {0b01011010};
    slice.push_back(std::make_pair(&bits, 0x3c));
    f.init("NA", slice, 2);
    // slice is bits[2:6] which is "0110"
    EXPECT_EQ(f.get(), 6);

    f.set(1);
    EXPECT_EQ(f.get(), 1);
    EXPECT_EQ(bits, 0b01000110);

    // Read with empty enabled_bits
    EXPECT_EQ(f.read(0), 0);

    f.set(0xf);
    EXPECT_EQ(f.read(1), 1);
    EXPECT_EQ(f.read(0b0110), 6);
    EXPECT_EQ(f.read(0xffffffffffffffff), 0xf);

    bits = 0;
    EXPECT_EQ(f.get(), 0);

    // Write with empty enabled_bits
    f.write(0xf, 0);
    EXPECT_EQ(f.get(), 0);

    f.write(0xf, 0b1010);
    EXPECT_EQ(f.get(), 0b1010);

    // Write with read value should not change the value
    f.write(f.read(0xf), 0xf);
    EXPECT_EQ(f.get(), 0b1010);
}

TEST_F(BankObjectFixture, TestFieldSetBits) {
    simics::Field f {&map_obj, "b0.r1.f2"};
    simics::bits_type slice;
    auto log_error_count_before = Stubs::instance_.sim_log_error_cnt_;

    f.set_bits(slice);
    EXPECT_EQ(Stubs::instance_.sim_log_error_cnt_,
                      log_error_count_before);

    set_configured();
    f.set_bits(slice);
    EXPECT_EQ(Stubs::instance_.sim_log_error_cnt_,
                      log_error_count_before + 1);
    EXPECT_EQ(Stubs::instance_.SIM_log_error_,
                      "Cannot set bits for field (b0.r1.f2) when device has"
                      " finalized");
}

namespace {
bool checkUninitializedOffset(const std::exception &ex) {
    EXPECT_STREQ(ex.what(), "Field offset has not been initialized");
    return true;
}
}  // namespace

TEST_F(BankObjectFixture, TestFieldInit) {
    auto log_error_count_before = Stubs::instance_.sim_log_error_cnt_;
    simics::bits_type slice;
    auto r_iface = reinterpret_cast<simics::RegisterInterface *>(0xa);
    map_obj.set_iface("b0.r1", r_iface);
    simics::Field f {&map_obj, "b0.r1.f2"};

    EXPECT_EQ(f.description(), "");
    EXPECT_PRED_THROW(f.offset(), std::runtime_error, checkUninitializedOffset);
    EXPECT_EQ(f.parent(), nullptr);
    f.init("some description", slice, 6);
    EXPECT_EQ(f.description(), "some description");
    EXPECT_EQ(f.offset(), 6);
    EXPECT_EQ(f.parent(), r_iface);
    EXPECT_EQ(Stubs::instance_.sim_log_error_cnt_,
              log_error_count_before);
}
