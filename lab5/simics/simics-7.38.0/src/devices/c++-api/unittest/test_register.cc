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

#include <simics/register.h>

#include <gtest/gtest.h>
#include <simics/field-templates.h>

#include <initializer_list>
#include <string>
#include <utility>

#include "bank-object-fixture.h"
#include "mock/gtest_extensions.h"  // EXPECT_PRED_THROW
#include "mock/stubs.h"

TEST_F(BankObjectFixture, TestRegisterCreation) {
    // Empty name is not allowed
    EXPECT_PRED_THROW(simics::Register(&map_obj, ""),
                      std::invalid_argument,
                      [](const std::exception &ex) {
                          EXPECT_STREQ(ex.what(),
                              "Cannot set with invalid name string: ");
                      });

    // Set up a valid bank object
    MockObject bank_obj {reinterpret_cast<conf_object_t *>(0xabc)};
    Stubs::instance_.sim_object_descendant_ret_ = bank_obj.obj().object();

    // The name has incorrect hierarchy level
    EXPECT_PRED_THROW(simics::Register(&map_obj, "b1"),
                      std::invalid_argument,
                      [](const std::exception &ex) {
                          EXPECT_STREQ(ex.what(),
        "Register name (b1) does not match the register level " \
        "(bankA.registerB)");
                      });

    // The indices are allowed in the name
    auto r = simics::Register(&map_obj, "b0.r[6]");
    EXPECT_EQ(r.name(), "r[6]");
}

TEST_F(BankObjectFixture, TestRegisterCTOR) {
    EXPECT_FALSE(std::is_copy_constructible<simics::Register>::value);
    EXPECT_TRUE(std::is_move_constructible<simics::Register>::value);

    auto r1 = simics::Register(&map_obj, "b0.r");
    auto *iface = map_obj.get_iface<simics::RegisterInterface>("b0.r");
    EXPECT_EQ(iface, &r1);

    auto r2 {std::move(r1)};
    EXPECT_EQ(r2.name(), "r");
    iface = map_obj.get_iface<simics::RegisterInterface>("b0.r");
    EXPECT_EQ(iface, &r2);

    r1 = std::move(r2);
    EXPECT_EQ(r1.name(), "r");
    iface = map_obj.get_iface<simics::RegisterInterface>("b0.r");
    EXPECT_EQ(iface, &r1);
}

TEST_F(BankObjectFixture, TestRegisterSetBytePointers) {
    auto r = simics::Register(&map_obj, "b0.r6");
    auto log_error_count_before = Stubs::instance_.sim_log_error_cnt_;

    // Set to empty vector
    r.set_byte_pointers({});
    EXPECT_EQ(Stubs::instance_.sim_log_error_cnt_,
              log_error_count_before + 1);
    EXPECT_EQ(Stubs::instance_.SIM_log_error_,
              "The supported register size is [1-8] bytes");

    log_error_count_before = Stubs::instance_.sim_log_error_cnt_;
    // Set to size>8 vector
    r.set_byte_pointers({
            bytes_.data(), bytes_.data() + 1,
            bytes_.data() + 2, bytes_.data() + 3,
            bytes_.data() + 4, bytes_.data() + 5,
            bytes_.data() + 6, bytes_.data() + 7,
            bytes_.data() + 8});
    EXPECT_EQ(Stubs::instance_.sim_log_error_cnt_,
              log_error_count_before + 1);
    EXPECT_EQ(Stubs::instance_.SIM_log_error_,
              "The supported register size is [1-8] bytes");

    log_error_count_before = Stubs::instance_.sim_log_error_cnt_;
    // Duplicated items
    r.set_byte_pointers({bytes_.data(), bytes_.data()});
    EXPECT_EQ(Stubs::instance_.sim_log_error_cnt_,
              log_error_count_before + 1);
    EXPECT_EQ(Stubs::instance_.SIM_log_error_,
              "The byte_pointers contains duplicate items");

    log_error_count_before = Stubs::instance_.sim_log_error_cnt_;
    // Cannot reset
    r.set_byte_pointers({bytes_.data()});
    r.set_byte_pointers({bytes_.data()});
    EXPECT_EQ(Stubs::instance_.sim_log_error_cnt_,
              log_error_count_before + 1);
    EXPECT_EQ(Stubs::instance_.SIM_log_error_,
              "Multiple calls to Register::set_byte_pointers() detected."
              " Make sure register name (b0.r6) is not duplicated within"
              " the same bank");
}

TEST_F(BankObjectFixture, TestRegisterConfig) {
    auto r = simics::Register(&map_obj, "b0.r6");
    r.set_byte_pointers({pointers_[0]});
    auto log_error_count_before = Stubs::instance_.sim_log_error_cnt_;

    // Too large number of bytes
    r.init("", 16, 0);
    EXPECT_EQ(Stubs::instance_.sim_log_error_cnt_,
                      log_error_count_before + 1);
    EXPECT_EQ(Stubs::instance_.SIM_log_error_,
                      "The supported register size is [1-8] bytes");

    log_error_count_before = Stubs::instance_.sim_log_error_cnt_;
    // Too small number of bytes
    r.init("", 0, 0);
    EXPECT_EQ(Stubs::instance_.sim_log_error_cnt_,
                      log_error_count_before + 1);
    EXPECT_EQ(Stubs::instance_.SIM_log_error_,
                      "The supported register size is [1-8] bytes");

    r.init("", 1, 0);
    EXPECT_EQ(r.number_of_bytes(), 1);
    EXPECT_EQ(r.is_read_only(), false);
}

TEST_F(BankObjectFixture, TestRegisterParseField) {
    auto r = simics::Register(&map_obj, "b0.r6");
    r.set_byte_pointers(pointers_);

    auto log_error_count_before = Stubs::instance_.sim_log_error_cnt_;
    // Invalid field bit: width = 0
    r.parse_field({"f1", "", 1, 0});
    EXPECT_EQ(Stubs::instance_.sim_log_error_cnt_,
                      log_error_count_before + 1);
    EXPECT_EQ(Stubs::instance_.SIM_log_error_,
      "Ignored invalid field as the width is 0");

    log_error_count_before = Stubs::instance_.sim_log_error_cnt_;
    // Invalid field bit: offset + width > 64
    r.parse_field({"f1", "", 1, 64});
    EXPECT_EQ(Stubs::instance_.sim_log_error_cnt_,
                      log_error_count_before + 1);
    EXPECT_EQ(Stubs::instance_.SIM_log_error_,
      "Ignored invalid field as the offset exceeds the number of bits");

    log_error_count_before = Stubs::instance_.sim_log_error_cnt_;
    auto log_info_count_before = Stubs::instance_.sim_log_info_cnt_;
    // Valid field
    r.parse_field({"f1", "", 0, 32});
    EXPECT_EQ(Stubs::instance_.sim_log_error_cnt_,
                      log_error_count_before);
    EXPECT_EQ(Stubs::instance_.sim_log_info_cnt_,
                      log_info_count_before + 1);
    EXPECT_EQ(Stubs::instance_.SIM_log_info_,
                      "Created default field for b0.r6.f1");

    log_error_count_before = Stubs::instance_.sim_log_error_cnt_;
    // Overlap field
    r.parse_field({"f2", "", 24, 24});
    EXPECT_EQ(Stubs::instance_.sim_log_error_cnt_,
                      log_error_count_before + 1);
    EXPECT_EQ(Stubs::instance_.SIM_log_error_,
                      "Ignored invalid field as it overlaps with other field");

    log_error_count_before = Stubs::instance_.sim_log_error_cnt_;
    // Change field f1 is not allowed
    r.parse_field({"f1", "", 32, 32});
    EXPECT_EQ(Stubs::instance_.sim_log_error_cnt_,
                      log_error_count_before + 1);
    EXPECT_EQ(Stubs::instance_.SIM_log_error_,
      "Duplicated field name(f1) on same register");

    // Field array
    log_error_count_before = Stubs::instance_.sim_log_error_cnt_;
    log_info_count_before = Stubs::instance_.sim_log_info_cnt_;
    r.parse_field({"f_array[3 stride 4]", "test field array", 32, 2});
    EXPECT_EQ(Stubs::instance_.sim_log_error_cnt_,
                      log_error_count_before);
    EXPECT_EQ(Stubs::instance_.sim_log_info_cnt_,
                      log_info_count_before + 3);
    EXPECT_EQ(Stubs::instance_.SIM_log_info_,
                      "Created default field for b0.r6.f_array[2]");

    // Multi-array
    r = simics::Register(&map_obj, "b0.r7");
    reset_register_memory();
    r.set_byte_pointers(pointers_);
    log_error_count_before = Stubs::instance_.sim_log_error_cnt_;
    log_info_count_before = Stubs::instance_.sim_log_info_cnt_;
    r.parse_field({"f_multi[3 stride 16][2][2 stride 2]",
                   "test multi array", 0, 1});
    auto fs = r.fields_info();
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 2; ++j) {
            for (int k = 0; k < 2; ++k) {
                const auto &[name, desc, offset, width] = fs[i * 4 + j * 2 + k];
                EXPECT_EQ(name,
                          "f_multi[" + std::to_string(i) + "]["
                          + std::to_string(j) + "]["
                          + std::to_string(k) + "]");
                EXPECT_EQ(desc, "test multi array");
                EXPECT_EQ(offset, i * 16 + j * 4 + k * 2);
                EXPECT_EQ(width, 1);
            }
        }
    }

    set_configured();
    r = simics::Register(&map_obj, "b0.r8");
    reset_register_memory();
    r.set_byte_pointers(pointers_);
    log_error_count_before = Stubs::instance_.sim_log_error_cnt_;
    r.parse_field({"f1", "", 0, 16});
    EXPECT_EQ(Stubs::instance_.sim_log_error_cnt_,
                      ++log_error_count_before);
    EXPECT_EQ(Stubs::instance_.SIM_log_error_,
                      "Cannot add fields for register (b0.r8) when device has"
                      " finalized");
}

TEST_F(BankObjectFixture, TestRegisterValue) {
    auto r1 = simics::Register(&map_obj, "b0.r1");
    r1.set_byte_pointers(pointers_);
    EXPECT_EQ(r1.number_of_bytes(), 8);

    auto log_info_count_before = Stubs::instance_.sim_log_info_cnt_;

    // Test set and get
    EXPECT_EQ(r1.get(), 0);
    r1.set(0xdeadbeef);
    EXPECT_EQ(r1.get(), 0xdeadbeef);
    EXPECT_EQ(Stubs::instance_.sim_log_info_cnt_,
                      log_info_count_before);

    // Verify read with different value of enabled_bits
    EXPECT_EQ(r1.read(0), 0);
    EXPECT_EQ(r1.read(0xffff0000), 0xdead0000);
    EXPECT_EQ(Stubs::instance_.sim_log_info_cnt_,
                      log_info_count_before + 1);
    EXPECT_EQ(Stubs::instance_.SIM_log_info_,
                      "Partial read from register r1: bytes 2-3 -> 0xdead0000");
    EXPECT_EQ(r1.read(0x0000ffff), 0x0000beef);
    EXPECT_EQ(Stubs::instance_.sim_log_info_cnt_,
                      log_info_count_before + 2);
    EXPECT_EQ(Stubs::instance_.SIM_log_info_,
                      "Partial read from register r1: bytes 0-1 -> 0xbeef");
    EXPECT_EQ(r1.read(0x00ffff00), 0x00adbe00);
    EXPECT_EQ(Stubs::instance_.sim_log_info_cnt_,
                      log_info_count_before + 3);
    EXPECT_EQ(Stubs::instance_.SIM_log_info_,
                      "Partial read from register r1: bytes 1-2 -> 0xadbe00");
    EXPECT_EQ(r1.read(0x00ffff0000000000), 0);
    EXPECT_EQ(Stubs::instance_.sim_log_info_cnt_,
                      log_info_count_before + 4);
    EXPECT_EQ(Stubs::instance_.SIM_log_info_,
                      "Partial read from register r1: bytes 5-6 -> 0x0");
    // Non-byte aligned enable_bytes
    EXPECT_EQ(r1.read(0x00fff800), 0x00adb800);
    EXPECT_EQ(Stubs::instance_.sim_log_info_cnt_,
                      log_info_count_before + 5);
    EXPECT_EQ(Stubs::instance_.SIM_log_info_,
                      "Partial read from register r1: bits 11-23 -> 0xadb800");
    EXPECT_EQ(r1.read(0x001ff000), 0x000db000);
    EXPECT_EQ(Stubs::instance_.sim_log_info_cnt_,
                      log_info_count_before + 6);
    EXPECT_EQ(Stubs::instance_.SIM_log_info_,
                      "Partial read from register r1: bits 12-20 -> 0xdb000");

    auto log_error_count_before = Stubs::instance_.sim_log_error_cnt_;

    // Malformed enable_bytes
    EXPECT_EQ(r1.read(0x00202300), 0);
    EXPECT_EQ(Stubs::instance_.sim_log_error_cnt_,
                      log_error_count_before + 1);
    EXPECT_EQ(Stubs::instance_.SIM_log_error_,
                      "enabled_bits(0x202300) is malformed: does not contain"
                      " consecutive ones");

    log_info_count_before = Stubs::instance_.sim_log_info_cnt_;

    // Verify write with different value of enabled_bits
    auto write_value = 0x123456789abcdef;
    r1.write(write_value, 0);
    EXPECT_EQ(r1.get(), 0xdeadbeef);
    EXPECT_EQ(Stubs::instance_.sim_log_info_cnt_,
                      log_info_count_before);
    r1.write(write_value, 0xffff0000);
    EXPECT_EQ(r1.get(), 0x89abbeef);
    EXPECT_EQ(Stubs::instance_.sim_log_info_cnt_,
                      log_info_count_before + 1);
    EXPECT_EQ(Stubs::instance_.SIM_log_info_,
                      "Partial write to register r1: bytes 2-3 <- 0x89ab0000");

    r1.set(0xdeadbeef);
    r1.write(write_value, 0x0000ffff);
    EXPECT_EQ(r1.get(), 0xdeadcdef);
    EXPECT_EQ(Stubs::instance_.sim_log_info_cnt_,
                      log_info_count_before + 2);
    EXPECT_EQ(Stubs::instance_.SIM_log_info_,
                      "Partial write to register r1: bytes 0-1 <- 0xcdef");

    r1.set(0xdeadbeef);
    r1.write(write_value, 0x00ffff00);
    EXPECT_EQ(r1.get(), 0xdeabcdef);
    EXPECT_EQ(Stubs::instance_.sim_log_info_cnt_,
                      log_info_count_before + 3);
    EXPECT_EQ(Stubs::instance_.SIM_log_info_,
                      "Partial write to register r1: bytes 1-2 <- 0xabcd00");

    r1.set(0xdeadbeef);
    r1.write(write_value, 0x00ffff0000000000);
    EXPECT_EQ(r1.get(), 0x234500deadbeef);
    EXPECT_EQ(Stubs::instance_.sim_log_info_cnt_,
                      log_info_count_before + 4);
    EXPECT_EQ(
            Stubs::instance_.SIM_log_info_,
            "Partial write to register r1: bytes 5-6 <- 0x23450000000000");

    // Non-byte aligned enable_bytes
    r1.set(0xdeadbeef);
    r1.write(write_value, 0x00fff800);
    EXPECT_EQ(r1.get(), 0xdeabceef);
    EXPECT_EQ(Stubs::instance_.sim_log_info_cnt_,
                      log_info_count_before + 5);
    EXPECT_EQ(Stubs::instance_.SIM_log_info_,
                      "Partial write to register r1: bits 11-23 <- 0xabc800");

    r1.set(0xdeadbeef);
    r1.write(write_value, 0x001ff000);
    EXPECT_EQ(r1.get(), 0xdeabceef);
    EXPECT_EQ(Stubs::instance_.sim_log_info_cnt_,
                      log_info_count_before + 6);
    EXPECT_EQ(Stubs::instance_.SIM_log_info_,
                      "Partial write to register r1: bits 12-20 <- 0xbc000");

    r1.write(write_value, 0xffffffffffffffff);
    EXPECT_EQ(r1.get(), write_value);
    EXPECT_EQ(Stubs::instance_.sim_log_info_cnt_,
                      log_info_count_before + 7);
    EXPECT_EQ(Stubs::instance_.SIM_log_info_,
                      "Write to register r1 <- 0x123456789abcdef");

    log_error_count_before = Stubs::instance_.sim_log_error_cnt_;
    r1.set(0xdeadbeef);

    // Malformed enable_bytes
    r1.write(write_value, 0x00202300);
    EXPECT_EQ(r1.get(), 0xdeadbeef);
    EXPECT_EQ(Stubs::instance_.sim_log_error_cnt_,
                      log_error_count_before + 1);
    EXPECT_EQ(Stubs::instance_.SIM_log_error_,
                      "enabled_bits(0x202300) is malformed: does not contain"
                      " consecutive ones");

    // Verify register with fields
    r1.set(0xdeadbeef);
    auto f2 = simics::ReadOnlyField(&map_obj, "b0.r1.f2");
    r1.parse_field({"f1", "", 0, 16});
    r1.parse_field({"f2", "", 16, 16});

    EXPECT_EQ(r1.read(0xffffffff), 0xdeadbeef);

    // Unmapped fields ignore write
    r1.write(0x123456789abcdef, 0xffffffffffffffff);
    EXPECT_EQ(r1.get(), 0xdeadcdef);
    EXPECT_EQ(r1.read(0xffffffffffffffff), 0xdeadcdef);

    // Read-only fields ignore write
    r1.write(0x11111111, 0xffff0000);
    EXPECT_EQ(r1.get(), 0xdeadcdef);
    EXPECT_EQ(r1.read(0xffffffffffffffff), 0xdeadcdef);

    // Test with a 2 byte register
    auto r2 = simics::Register(&map_obj, "b0.r2");
    reset_register_memory();
    r2.set_byte_pointers({pointers_[0], pointers_[1]});
    r2.init("A 2 byte register", 2, 0xffff);
    EXPECT_EQ(r2.number_of_bytes(), 2);
    EXPECT_EQ(r2.get(), 0xffff);

    // Verify read with different value of enabled_bits
    EXPECT_EQ(r2.read(0), 0);
    EXPECT_EQ(r2.read(0xffff0000), 0);
    EXPECT_EQ(r2.read(0x0000ffff), 0x0000ffff);
    EXPECT_EQ(r2.read(0x00ffff00), 0x0000ff00);

    // Verify write with different value of enabled_bits
    r2.write(write_value, 0);
    EXPECT_EQ(r2.get(), 0xffff);
    r2.write(write_value, 0xffff0000);
    EXPECT_EQ(r2.get(), 0xffff);
    r2.write(write_value, 0x0000ffff);
    EXPECT_EQ(r2.get(), 0xcdef);
    r2.set(0xffff);
    r2.write(write_value, 0x00ffff00);
    EXPECT_EQ(r2.get(), 0xcdff);
    r2.write(r2.read(0xffff), 0xffff);
    EXPECT_EQ(r2.get(), 0xcdff);

    // Add a field
    auto f1 = simics::Field(&map_obj, "b0.r2.f1");
    r2.parse_field({"f1", "", 3, 4});
    EXPECT_EQ(f1.get(), 0xf);

    // Test read
    EXPECT_EQ(r2.read(0xf), 0xf);
    f1.set(0);
    EXPECT_EQ(f1.read(0xf), 0);
    EXPECT_EQ(r2.read(0xf), 0x7);
    f1.set(1);
    EXPECT_EQ(f1.read(0xf), 1);
    EXPECT_EQ(r2.read(0xf), 0xf);

    EXPECT_EQ(r2.read(0xf0), 0x80);
    f1.set(0xa);
    EXPECT_EQ(f1.read(0xf), 0xa);
    EXPECT_EQ(f1.read(0xe), 0xa);
    EXPECT_EQ(r2.read(0xf0), 0xd0);

    // Test write
    EXPECT_EQ(f1.get(), 0xa);
    r2.write(0xf, 0xf);
    EXPECT_EQ(f1.get(), 0xb);
    r2.write(0x10, 0xf0);
    EXPECT_EQ(f1.get(), 0x3);

    // Test with a 8 byte register
    auto r3 = simics::Register(&map_obj, "b0.r3");
    reset_register_memory();
    r3.set_byte_pointers(pointers_);
    r3.init("A 8 byte register", 8, 0xffffffffffffffff);
    EXPECT_EQ(r3.number_of_bytes(), 8);
    EXPECT_EQ(r3.get(), 0xffffffffffffffff);

    // Test read
    for (size_t bit_enable : std::initializer_list<size_t>{0xf, 0xff, 0xfff,
                0xffff, 0xf'ffff, 0xff'ffff, 0xfff'ffff, 0xffff'ffff,
                0xf'ffff'ffff, 0xff'ffff'ffff, 0xfff'ffff'ffff,
                0xffff'ffff'ffff, 0xf'ffff'ffff'ffff,
                0xff'ffff'ffff'ffff, 0xfff'ffff'ffff'ffff,
                0xffff'ffff'ffff'ffff, 0xffff'0000}) {
        EXPECT_EQ(r3.read(bit_enable), bit_enable);
    }

    // Test write
    for (size_t bit_enable : std::initializer_list<size_t>{0xf, 0xff, 0xfff,
                0xffff, 0xf'ffff, 0xff'ffff, 0xfff'ffff, 0xffff'ffff,
                0xf'ffff'ffff, 0xff'ffff'ffff, 0xfff'ffff'ffff,
                0xffff'ffff'ffff, 0xf'ffff'ffff'ffff,
                0xff'ffff'ffff'ffff, 0xfff'ffff'ffff'ffff,
                0xffff'ffff'ffff'ffff, 0xffff'0000}) {
        r3.write(write_value, bit_enable);
        EXPECT_EQ(r3.get() & bit_enable, write_value & bit_enable);
    }

    r3.set(0xffff'ffff'ffff'ffff);
    // Adding a 64 bit field
    auto f3 = simics::Field(&map_obj, "b0.r3.f3");
    r3.parse_field({"f3", "", 0, 64});
    EXPECT_EQ(f3.get(), 0xffff'ffff'ffff'ffff);

    // Test read again with field
    for (size_t bit_enable : std::initializer_list<size_t>{0xf, 0xff, 0xfff,
                0xffff, 0xf'ffff, 0xff'ffff, 0xfff'ffff, 0xffff'ffff,
                0xf'ffff'ffff, 0xff'ffff'ffff, 0xfff'ffff'ffff,
                0xffff'ffff'ffff, 0xf'ffff'ffff'ffff,
                0xff'ffff'ffff'ffff, 0xfff'ffff'ffff'ffff,
                0xffff'ffff'ffff'ffff}) {
        EXPECT_EQ(r3.read(bit_enable), bit_enable);
    }

    // Test write again with field
    for (size_t bit_enable : std::initializer_list<size_t>{0xf, 0xff, 0xfff,
                0xffff, 0xf'ffff, 0xff'ffff, 0xfff'ffff, 0xffff'ffff,
                0xf'ffff'ffff, 0xff'ffff'ffff, 0xfff'ffff'ffff,
                0xffff'ffff'ffff, 0xf'ffff'ffff'ffff,
                0xff'ffff'ffff'ffff, 0xfff'ffff'ffff'ffff,
                0xffff'ffff'ffff'ffff, 0xffff'0000}) {
        r3.write(write_value, bit_enable);
        EXPECT_EQ(r3.get() & bit_enable, write_value & bit_enable);
    }
}

TEST_F(BankObjectFixture, TestRegisterStatusChangeNotify) {
    auto r1 = simics::Register(&map_obj, "b0.r1");
    r1.set_byte_pointers(pointers_);
    EXPECT_EQ(r1.get(), 0);

    auto notify_count_before = Stubs::instance_.sim_notify_cnt_;

    // Set with same value does not notify
    r1.set(r1.get());
    EXPECT_EQ(Stubs::instance_.sim_notify_cnt_, notify_count_before);

    // Set with different value does notify
    r1.set(r1.get() + 1);
    EXPECT_EQ(Stubs::instance_.sim_notify_cnt_,
                      notify_count_before + 1);

    notify_count_before = Stubs::instance_.sim_notify_cnt_;
    // Write with same value does not notify
    r1.write(r1.get(), ~0);
    EXPECT_EQ(Stubs::instance_.sim_notify_cnt_, notify_count_before);

    // Write with different value does notify
    r1.write(r1.get() - 1, ~0);
    EXPECT_EQ(Stubs::instance_.sim_notify_cnt_,
                      notify_count_before + 1);

    // Add a field
    auto f1 = simics::Field(&map_obj, "b0.r1.f1");
    r1.parse_field({"f1", "", 3, 4});
    EXPECT_EQ(f1.get(), 0);

    notify_count_before = Stubs::instance_.sim_notify_cnt_;
    // Set with same value does not notify
    f1.set(f1.get());
    EXPECT_EQ(Stubs::instance_.sim_notify_cnt_, notify_count_before);

    // Set with different value does notify
    f1.set(f1.get() + 1);
    EXPECT_EQ(Stubs::instance_.sim_notify_cnt_,
                      notify_count_before + 1);

    notify_count_before = Stubs::instance_.sim_notify_cnt_;
    // Write with same value does not notify
    f1.write(f1.get(), ~0);
    EXPECT_EQ(Stubs::instance_.sim_notify_cnt_, notify_count_before);

    // Write with different value does notify
    f1.write(f1.get() + 1, ~0);
    EXPECT_EQ(Stubs::instance_.sim_notify_cnt_,
                      notify_count_before + 1);

    notify_count_before = Stubs::instance_.sim_notify_cnt_;
    // Write outside field f1 does not notify
    r1.write(r1.get() + 1, ~0);
    EXPECT_EQ(Stubs::instance_.sim_notify_cnt_, notify_count_before);

    // Write inside field f1 does notify
    r1.write((f1.get() + 1) << 3, ~0);
    EXPECT_EQ(Stubs::instance_.sim_notify_cnt_,
                      notify_count_before + 1);
}

TEST_F(BankObjectFixture, TestRegisterFieldsInfo) {
    auto r1 = simics::Register(&map_obj, "b0.r1");
    r1.set_byte_pointers(pointers_);
    // Add a field
    const auto f1 = simics::Field(&map_obj, "b0.r1.f1");
    r1.parse_field({"f1", "field 1", 3, 4});

    const auto fields_info = r1.fields_info();
    EXPECT_EQ(fields_info.size(), 1);
    const auto first_field = fields_info[0];
    EXPECT_EQ(std::get<0>(first_field), "f1");
    EXPECT_EQ(std::get<1>(first_field), "field 1");
    EXPECT_EQ(std::get<2>(first_field), 3);
    EXPECT_EQ(std::get<3>(first_field), 4);
}

TEST_F(BankObjectFixture, TestRegisterParent) {
    auto log_error_count_before = Stubs::instance_.sim_log_error_cnt_;
    auto b_iface = reinterpret_cast<simics::BankInterface *>(0xa);
    map_obj.set_iface("b0", b_iface);
    simics::Register r {&map_obj, "b0.r1"};
    r.set_byte_pointers({pointers_[0]});

    EXPECT_EQ(r.parent(), nullptr);
    r.init("some description", 1, 0);
    EXPECT_EQ(r.parent(), b_iface);
    EXPECT_EQ(Stubs::instance_.sim_log_error_cnt_,
              log_error_count_before);
}

class TestField : public simics::Field {
  public:
    TestField(simics::MappableConfObject *dev_obj,
              const std::string &name,
              uint64_t get_return_value)
        : Field(dev_obj, name),
          get_return_value_(get_return_value) {}

    uint64_t get() const override {
        get_is_called = true;
        return get_return_value_;
    }

    void set(uint64_t value) override {
        set_is_called = true;
        set_value = value;
    }

    mutable bool get_is_called {false};
    bool set_is_called {false};
    uint64_t set_value {0};

  private:
    uint64_t get_return_value_ {0};
};

TEST_F(BankObjectFixture, TestRegisterCallsFieldGetSet) {
    {
        auto r1 = simics::Register(&map_obj, "b0.r1");
        r1.set_byte_pointers(pointers_);
        // Set all bits, test if field get() is honored in the test
        r1.set(0xffffffff);

        // f1.get() always return 0
        auto f1 = TestField(&map_obj, "b0.r1.f1", 0);
        auto f2 = TestField(&map_obj, "b0.r1.f2", 0xdeadbeef);
        r1.parse_field({"f1", "", 0, 4});
        r1.parse_field({"f2", "", 4, 28});

        EXPECT_EQ(f1.get_is_called, false);
        EXPECT_EQ(f2.get_is_called, false);
        EXPECT_EQ(r1.get(), 0xeadbeef0);
        EXPECT_EQ(f1.get_is_called, true);
        EXPECT_EQ(f2.get_is_called, true);

        EXPECT_EQ(f1.set_is_called, false);
        EXPECT_EQ(f1.set_value, 0);
        EXPECT_EQ(f2.set_is_called, false);
        EXPECT_EQ(f2.set_value, 0);
        r1.set(0xab);
        EXPECT_EQ(f1.set_is_called, true);
        EXPECT_EQ(f1.set_value, 0xb);
        EXPECT_EQ(f2.set_is_called, true);
        EXPECT_EQ(f2.set_value, 0xa);
    }
    {
        auto r1 = simics::Register(&map_obj, "b0.r1");
        reset_register_memory();
        r1.set_byte_pointers(pointers_);
        // Set all bits, test if field get() is honored in the test
        r1.set(0xffffffffffffffff);

        // f1 fills all the bits of r1
        auto f1 = TestField(&map_obj, "b0.r1.f1", 0xdeadbeefdeadbeef);
        r1.parse_field({"f1", "", 0, 64});

        EXPECT_EQ(f1.get_is_called, false);
        EXPECT_EQ(r1.get(), 0xdeadbeefdeadbeef);
        EXPECT_EQ(f1.get_is_called, true);

        EXPECT_EQ(f1.set_is_called, false);
        EXPECT_EQ(f1.set_value, 0);
        r1.set(0xab);
        EXPECT_EQ(f1.set_is_called, true);
        EXPECT_EQ(f1.set_value, 0xab);
    }
}
