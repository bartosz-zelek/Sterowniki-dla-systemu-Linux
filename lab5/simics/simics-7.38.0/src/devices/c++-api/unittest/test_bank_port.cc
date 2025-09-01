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

#include <simics/bank-port.h>

#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "mock/gtest_extensions.h"  // EXPECT_PRED_THROW
#include "mock/mock-bank.h"
#include "mock/mock-object.h"
#include "mock/stubs.h"

namespace {
bool checkInvalidName(const std::exception &ex) {
    auto message = std::string(ex.what());
    EXPECT_EQ(message.substr(0, message.rfind(':')),
                      "Invalid bank port name (foo.bar)");
    return true;
}

bool checkInvalidBankPointer(const std::exception &ex) {
    auto message = std::string(ex.what());
    EXPECT_EQ(message.substr(0, message.rfind(':')),
                      "Bank pointer cannot be nullptr");
    return true;
}
}  // namespace

// Test cases for BankPort
class BankPortTest : public ::testing::Test {
  protected:
    BankPortTest() {}

    void SetUp() override {
        Stubs::instance_.sim_port_object_parent_ret_ = dev_obj.obj();
        Stubs::instance_.sim_object_descendant_ret_ = port_obj.obj();
        Stubs::instance_.sim_object_data_ret_ = &map_obj;
        sim_log_info_cnt_ = Stubs::instance_.sim_log_info_cnt_;
        sim_log_error_cnt_ = Stubs::instance_.sim_log_error_cnt_;
    }

    void TearDown() override {
        Stubs::instance_.sim_port_object_parent_ret_ = nullptr;
        Stubs::instance_.sim_object_data_ret_ = nullptr;
        Stubs::instance_.sim_object_descendant_ret_ = nullptr;
        Stubs::instance_.sim_log_info_cnt_ = 0;
        delete port_obj.obj();
    }

    MockObject dev_obj {
        reinterpret_cast<conf_object_t *>(0x1234), "foo"
    };
    simics::MappableConfObject map_obj {
        dev_obj.obj()
    };
    MockObject port_obj {
        new conf_object_t, "foo.bank.bar"
    };
    size_t sim_log_info_cnt_;
    size_t sim_log_error_cnt_;
};

TEST_F(BankPortTest, TestCreation) {
    // Test that passing a nullptr ConfObjectRef to the BankPort
    //  constructor throws an exception
    simics::ConfObjectRef null_ref(nullptr);

    EXPECT_THROW(
        {
            try {
                simics::Port<MockObject> port(null_ref);
            } catch (const std::invalid_argument &e) {
                // Verify the exception message
                EXPECT_STREQ(e.what(),
                    "ConfObjectRef passed to Port constructor is null");
                throw;
            }
        },
        std::invalid_argument);

    {
        // Test invalid bank name
        MockObject mock_obj {reinterpret_cast<conf_object_t *>(0xc0ffee),
                "foo.bar"};
        EXPECT_PRED_THROW(
                simics::BankPort<simics::MappableConfObject> bp(mock_obj.obj()),
                std::invalid_argument, checkInvalidName);
    }

    {
        // Test invalid bank pointer
        EXPECT_PRED_THROW(
                simics::BankPort<simics::MappableConfObject> bp(port_obj.obj(),
                                                                nullptr),
                std::invalid_argument, checkInvalidBankPointer);
    }

    {
        simics::BankPort<simics::MappableConfObject> bp(port_obj.obj());

        // No logging
        EXPECT_EQ(Stubs::instance_.sim_log_info_cnt_, sim_log_info_cnt_);
        EXPECT_EQ(bp.bank_name(), "bar");
        EXPECT_EQ(bp.dev_obj(), &map_obj);
    }

    const simics::bank_t b {"bar", "a bank named bar", {/*No register*/}};

    {
        // Test construct it with a a const bank_t *, and the bank is not
        // in the map yet
        simics::BankPort<simics::MappableConfObject> bp {port_obj.obj(), &b};

        EXPECT_EQ(Stubs::instance_.sim_log_info_cnt_, ++sim_log_info_cnt_);
        // Since no Bank "bar" in the map, a default bank for bar is created
        EXPECT_EQ(Stubs::instance_.SIM_log_info_,
                          "Created a new default bank for bar");
        EXPECT_EQ(bp.bank_name(), "bar");
        EXPECT_EQ(bp.dev_obj(), &map_obj);
        EXPECT_EQ(bp.big_endian_bitorder(), false);
        EXPECT_EQ(bp.number_of_registers(), 0);
    }

    {
        MockBank bank;
        bank.name_ = "test";
        // Need a new bank name since bank "bar" is already initialized
        // thus cannot be modified
        map_obj.set_iface<simics::BankInterface>("bar3", &bank);
        MockObject port_obj2 {reinterpret_cast<conf_object_t *>(0xc0ffee),
            "foo.bank.bar3"};

        // Test construct it with a a const bank_t *, and the bank is already
        // in the map
        simics::BankPort<simics::MappableConfObject> bp {port_obj2.obj(), &b};

        EXPECT_EQ(Stubs::instance_.sim_log_info_cnt_, ++sim_log_info_cnt_);
        // Reuse the already defined bank
        EXPECT_EQ(Stubs::instance_.SIM_log_info_,
                          "Used user defined bank for bar3");
        EXPECT_EQ(bp.bank_name(), "bar3");
        EXPECT_EQ(bp.dev_obj(), &map_obj);
        EXPECT_EQ(bp.big_endian_bitorder(), false);
        EXPECT_EQ(bp.number_of_registers(), 0);
    }
}

TEST(TestBankPort, TestAddBankProperties) {
    Stubs::instance_.a_conf_class_ = reinterpret_cast<conf_class_t*>(
            uintptr_t{0xdeadbeef});
    auto ret = simics::make_class<MockObject>("test_add_bank_properties",
                                              "short_desc", "description");

    auto sim_register_interface_cnt_ = \
        Stubs::instance_.sim_register_interface_cnt_;
    Stubs::instance_.sim_register_interface_names_.clear();
    simics::BankPort<simics::MappableConfObject>::addBankProperties(ret.get());
    EXPECT_EQ(Stubs::instance_.sim_register_interface_cnt_,
              sim_register_interface_cnt_ + 6);

    std::vector<std::string> expected_interface_names = {
        "transaction", "register_view", "register_view_read_only",
        "register_view_catalog", "bank_instrumentation_subscribe",
        "instrumentation_order"
    };
    EXPECT_EQ(Stubs::instance_.sim_register_interface_names_,
              expected_interface_names);
}

TEST_F(BankPortTest, TestBankPortInterface) {
    simics::BankPort<simics::MappableConfObject> bp {port_obj.obj()};

    EXPECT_EQ(bp.bank_name(), "bar");
    EXPECT_EQ(bp.bank_iface(), nullptr);
    EXPECT_EQ(bp.dev_obj(), &map_obj);

    bp.set_bank({"bar", "test description", {}});
    EXPECT_STREQ(bp.description(), "test description");
    EXPECT_EQ(Stubs::instance_.sim_log_info_cnt_,
              ++sim_log_info_cnt_);
    EXPECT_EQ(Stubs::instance_.SIM_log_info_,
        "Created a new default bank for bar");
    EXPECT_NE(bp.bank_iface(), nullptr);

    // Bank interface can only be set once
    bp.set_bank({"bar", "test description 2", {}});
    EXPECT_EQ(Stubs::instance_.sim_log_error_cnt_, ++sim_log_error_cnt_);
    EXPECT_EQ(Stubs::instance_.SIM_log_error_,
        "bank iface can only be set once");

    simics::BankPort<simics::MappableConfObject> bp2 {port_obj.obj()};
    // Add a bank with a register
    bp2.set_bank({"bar", "test description",
                  {{"r", "a register with default value 42",
                   0, 4, 42, {}}},
                 });
}

TEST_F(BankPortTest, TestTransactionInterface) {
    simics::BankPort<simics::MappableConfObject> bp {port_obj.obj()};

    EXPECT_EQ(bp.issue(nullptr, 0), Sim_PE_IO_Not_Taken);
    EXPECT_EQ(Stubs::instance_.sim_log_error_cnt_,
              ++sim_log_error_cnt_);
    EXPECT_EQ(Stubs::instance_.SIM_log_error_,
              "BankPort should have one bank");

    bp.set_bank({"bar", "test description", {}});
    EXPECT_EQ(bp.issue(nullptr, 0), Sim_PE_IO_Not_Taken);
    EXPECT_EQ(Stubs::instance_.SIM_log_spec_violation_,
              "0 byte transaction ignored");
}

TEST_F(BankPortTest, TestRegisterViewInterface) {
    simics::BankPort<simics::MappableConfObject> bp {port_obj.obj()};

    EXPECT_EQ(bp.description(), nullptr);
    EXPECT_EQ(Stubs::instance_.sim_log_error_cnt_,
              ++sim_log_error_cnt_);
    EXPECT_EQ(Stubs::instance_.SIM_log_error_,
              "BankPort should have one bank");

    EXPECT_EQ(bp.big_endian_bitorder(), false);

    EXPECT_EQ(bp.number_of_registers(), 0);
    EXPECT_EQ(Stubs::instance_.sim_log_error_cnt_,
              ++sim_log_error_cnt_);
    EXPECT_EQ(Stubs::instance_.SIM_log_error_,
              "BankPort should have one bank");

    EXPECT_TRUE(SIM_attr_is_nil(bp.register_info(0)));
    EXPECT_EQ(Stubs::instance_.sim_log_error_cnt_,
              ++sim_log_error_cnt_);
    EXPECT_EQ(Stubs::instance_.SIM_log_error_,
              "BankPort should have one bank");

    EXPECT_EQ(bp.get_register_value(0), 0);
    EXPECT_EQ(Stubs::instance_.sim_log_error_cnt_,
              ++sim_log_error_cnt_);
    EXPECT_EQ(Stubs::instance_.SIM_log_error_,
              "BankPort should have one bank");

    bp.set_register_value(0, 0xdead);
    EXPECT_EQ(Stubs::instance_.sim_log_error_cnt_,
              ++sim_log_error_cnt_);
    EXPECT_EQ(Stubs::instance_.SIM_log_error_,
              "BankPort should have one bank");

    // Add a bank with a register
    bp.set_bank({"bar", "test description",
                 {{"r", "a register with default value 42",
                  0, 4, 42, {}}},
                });

    EXPECT_STREQ(bp.description(), "test description");

    EXPECT_EQ(bp.number_of_registers(), 1);
    EXPECT_EQ(Stubs::instance_.sim_log_error_cnt_,
              sim_log_error_cnt_);

    EXPECT_TRUE(SIM_attr_is_list(bp.register_info(0)));
    EXPECT_EQ(Stubs::instance_.sim_log_error_cnt_,
              sim_log_error_cnt_);

    EXPECT_EQ(bp.get_register_value(0), 42);
    EXPECT_EQ(Stubs::instance_.sim_log_error_cnt_,
              sim_log_error_cnt_);

    bp.set_register_value(0, 0);
    EXPECT_EQ(bp.get_register_value(0), 0);
    EXPECT_EQ(Stubs::instance_.sim_log_error_cnt_,
              sim_log_error_cnt_);
}

TEST_F(BankPortTest, TestRegisterViewReadOnlyInterface) {
    simics::BankPort<simics::MappableConfObject> bp {port_obj.obj()};

    EXPECT_EQ(bp.is_read_only(0), false);
    EXPECT_EQ(Stubs::instance_.sim_log_error_cnt_,
              ++sim_log_error_cnt_);
    EXPECT_EQ(Stubs::instance_.SIM_log_error_,
              "BankPort should have one bank");

    // Add a bank with a register
    bp.set_bank({"bar", "test description",
                 {{"r", "a register with default value 42",
                  0, 4, 42, {}}},
                });

    EXPECT_EQ(bp.is_read_only(0), false);
    EXPECT_EQ(Stubs::instance_.sim_log_error_cnt_,
              sim_log_error_cnt_);
}

TEST_F(BankPortTest, TestRegisterViewCatalogInterface) {
    simics::BankPort<simics::MappableConfObject> bp {port_obj.obj()};

    EXPECT_TRUE(SIM_attr_is_nil(bp.register_names()));
    EXPECT_EQ(Stubs::instance_.sim_log_error_cnt_,
              ++sim_log_error_cnt_);
    EXPECT_EQ(Stubs::instance_.SIM_log_error_,
              "BankPort should have one bank");

    EXPECT_TRUE(SIM_attr_is_nil(bp.register_offsets()));
    EXPECT_EQ(Stubs::instance_.sim_log_error_cnt_,
              ++sim_log_error_cnt_);
    EXPECT_EQ(Stubs::instance_.SIM_log_error_,
              "BankPort should have one bank");

    // Add a bank with a register
    bp.set_bank({"bar", "test description",
                 {{"r", "a register with default value 42",
                  0, 4, 42, {}}},
                });

    EXPECT_TRUE(SIM_attr_is_list(bp.register_names()));
    EXPECT_TRUE(SIM_attr_is_list(bp.register_offsets()));
    EXPECT_EQ(Stubs::instance_.sim_log_error_cnt_,
              sim_log_error_cnt_);
}

class TestPortBank {
  public:
    TestPortBank(simics::BankPortInterface *port_iface,
                 simics::Description desc) {
        port_iface->set_bank({port_iface->bank_name().data(), desc, {}});
    }
};

class TestPortBankWithArgs {
  public:
    TestPortBankWithArgs(simics::BankPortInterface *port_iface,
                         simics::Description desc, std::string suffix) {
        port_iface->set_bank({port_iface->bank_name().data(),
                              std::string(desc) + suffix, {}});
    }
};

TEST_F(BankPortTest, TestBankPortSimpleBankPort) {
    simics::SimpleBankPort<TestPortBank> bp {port_obj.obj()};

    EXPECT_EQ(bp.bank_name(), "bar");
    EXPECT_EQ(bp.dev_obj(), &map_obj);

    EXPECT_STREQ(bp.description(),
                 "A bank created through the SimicsBankPort utility class");

    simics::SimpleBankPort<TestPortBankWithArgs, std::string> bpa {
        port_obj.obj(), " with args"
    };

    EXPECT_EQ(bpa.bank_name(), "bar");
    EXPECT_EQ(bpa.dev_obj(), &map_obj);

    EXPECT_STREQ(bpa.description(),
                 "A bank created through the SimicsBankPort utility class"
                 " with args");
}
