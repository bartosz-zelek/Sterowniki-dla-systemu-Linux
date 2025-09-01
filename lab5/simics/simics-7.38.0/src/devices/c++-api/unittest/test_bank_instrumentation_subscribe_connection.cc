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

#include <simics/bank-instrumentation-subscribe-connection.h>

#include <gtest/gtest.h>
#include <simics/base/attr-value.h>  // SIM_attr_list_item
#include <simics/type/bank-access.h>

int before_read_cnt = 0;
int before_write_cnt = 0;
int after_read_cnt = 0;
int after_write_cnt = 0;
// Last received offset/size/connection
physical_address_t last_offset = 0;
physical_address_t last_size = 0;
size_t last_value = 0;
conf_object_t *last_connection = NULL;
size_t set_value = 0;

void before_read(conf_object_t *connection,
                 bank_before_read_interface_t* iface,
                 bank_access_t *access, lang_void *user_data) {
    before_read_cnt++;
    last_offset = iface->offset(access);
    last_size = iface->size(access);
    last_connection = connection;
    iface->set_offset(access, last_offset);
}
void before_write(conf_object_t *connection,
                  bank_before_write_interface_t* iface,
                  bank_access_t *access, lang_void *user_data) {
    before_write_cnt++;
    last_offset = iface->offset(access);
    last_size = iface->size(access);
    last_connection = connection;
    last_value = iface->value(access);
    iface->set_offset(access, last_offset);
    iface->set_value(access, set_value);
}
void after_read(conf_object_t *connection,
                bank_after_read_interface_t* iface,
                bank_access_t *access, lang_void *user_data) {
    after_read_cnt++;
    last_offset = iface->offset(access);
    last_size = iface->size(access);
    last_connection = connection;
    iface->set_value(access, set_value);
    iface->set_missed(access, iface->missed(access));
}
void after_write(conf_object_t *connection,
                 bank_after_write_interface_t* iface,
                 bank_access_t *access, lang_void *user_data) {
    after_write_cnt++;
    last_offset = iface->offset(access);
    last_size = iface->size(access);
    last_connection = connection;
    iface->set_missed(access, iface->missed(access));
}

TEST(TestBankInstrumentationSubscribeConnection, TestCreationAndDestruction) {
    {
        simics::BankInstrumentationSubscribeConnection connection;
        EXPECT_EQ(connection.empty(), true);
        auto num = connection.number_of_callbacks();
        EXPECT_EQ(num, 0);
        auto connections = connection.get_connections();
        EXPECT_EQ(SIM_attr_list_size(connections), 0);
    }
}

TEST(TestBankInstrumentationSubscribeConnection, test_RegisterBeforeRead) {
    before_read_cnt = 0;
    last_offset = 0;
    last_size = 0;

    simics::BankInstrumentationSubscribeConnection connection;

    uint64_t offset = 0x12;
    auto handle = connection.register_before_read(NULL, offset, 0x34,
                                                 before_read, NULL);
    EXPECT_EQ(handle, 0);
    EXPECT_EQ(connection.empty(), false);
    auto num = connection.number_of_callbacks();
    EXPECT_EQ(num, 1);

    // A transaction hit the registered callback
    simics::BankAccess access {nullptr, nullptr, false, offset, 4};
    connection.issue_callbacks(&access, simics::CallbackType::BR);
    EXPECT_EQ(before_read_cnt, 1);
    EXPECT_EQ(last_offset, 0x12);
    EXPECT_EQ(last_size, 4);

    // Missed type
    connection.issue_callbacks(&access, simics::CallbackType::AR);
    EXPECT_EQ(before_read_cnt, 1);

    // Missed offset
    access.offset = 0x48;
    connection.issue_callbacks(&access, simics::CallbackType::BR);
    EXPECT_EQ(before_read_cnt, 1);

    // Valid transaction again
    access.offset = 0x16;
    access.size = 2;
    connection.issue_callbacks(&access, simics::CallbackType::BR);
    EXPECT_EQ(before_read_cnt, 2);
    EXPECT_EQ(last_offset, 0x16);
    EXPECT_EQ(last_size, 2);
}

TEST(TestBankInstrumentationSubscribeConnection, test_RegisterBeforeWrite) {
    before_write_cnt = 0;
    last_offset = 0;
    last_size = 0;
    last_value = 0;

    simics::BankInstrumentationSubscribeConnection connection;

    conf_object_t conf_obj;
    auto handle = connection.register_before_write(&conf_obj, 0, 0x34,
                                                  before_write, NULL);
    EXPECT_EQ(handle, 0);
    EXPECT_EQ(connection.empty(), false);
    auto num = connection.number_of_callbacks();
    EXPECT_EQ(num, 1);

    // A transaction hit the registered callback
    simics::BankAccess access {nullptr, nullptr, false, 0x12, 8};
    access.value = 0x1234567890abcdef;
    connection.issue_callbacks(&access, simics::CallbackType::BW);
    EXPECT_EQ(before_write_cnt, 1);
    EXPECT_EQ(last_offset, 0x12);
    EXPECT_EQ(last_size, 8);
    EXPECT_EQ(last_value, 0x1234567890abcdef);
    EXPECT_EQ(last_connection, &conf_obj);

    // Missed function
    connection.issue_callbacks(&access, simics::CallbackType::AW);
    EXPECT_EQ(before_write_cnt, 1);

    // Missed offset
    access.offset = 0x48;
    connection.issue_callbacks(&access, simics::CallbackType::BW);
    EXPECT_EQ(before_write_cnt, 1);
    access.offset = 0x16;

    // Valid transaction again
    access.size = 2;
    connection.issue_callbacks(&access, simics::CallbackType::BW);
    EXPECT_EQ(before_write_cnt, 2);
    EXPECT_EQ(last_offset, 0x16);
    EXPECT_EQ(last_size, 2);
    EXPECT_EQ(last_connection, &conf_obj);
}

TEST(TestBankInstrumentationSubscribeConnection, test_RegisterAfterRead) {
    after_read_cnt = 0;
    last_offset = 0;
    last_size = 0;
    set_value = 0x12345678;

    simics::BankInstrumentationSubscribeConnection connection;

    auto handle = connection.register_after_read(NULL, 0x12, 0x4,
                                                after_read, NULL);
    EXPECT_EQ(handle, 0);
    EXPECT_EQ(connection.empty(), false);
    auto num = connection.number_of_callbacks();
    EXPECT_EQ(num, 1);

    // A transaction hit the registered callback
    simics::BankAccess access {nullptr, nullptr, false, 0x12, 4};
    connection.issue_callbacks(&access, simics::CallbackType::AR);
    EXPECT_EQ(after_read_cnt, 1);
    EXPECT_EQ(last_offset, 0x12);
    EXPECT_EQ(last_size, 4);
    EXPECT_EQ(access.value, set_value);

    // Missed function
    connection.issue_callbacks(&access, simics::CallbackType::AW);
    EXPECT_EQ(after_read_cnt, 1);

    // Missed offset
    access.offset = 0x48;
    connection.issue_callbacks(&access, simics::CallbackType::AR);
    EXPECT_EQ(after_read_cnt, 1);
    access.offset = 0x12;

    // Valid transaction again
    access.size = 2;
    connection.issue_callbacks(&access, simics::CallbackType::AR);
    EXPECT_EQ(after_read_cnt, 2);
    EXPECT_EQ(last_offset, 0x12);
    EXPECT_EQ(last_size, 2);
}

TEST(TestBankInstrumentationSubscribeConnection, test_RegisterAfterWrite) {
    after_write_cnt = 0;
    last_offset = 0;
    last_size = 0;

    simics::BankInstrumentationSubscribeConnection connection;

    // Test both offset and size set to 0
    auto handle = connection.register_after_write(NULL, 0, 0,
                                                 after_write, NULL);
    EXPECT_EQ(handle, 0);
    EXPECT_EQ(connection.empty(), false);
    auto num = connection.number_of_callbacks();
    EXPECT_EQ(num, 1);

    // A transaction hit the registered callback
    simics::BankAccess access {nullptr, nullptr, false, 0x12, 4};
    connection.issue_callbacks(&access, simics::CallbackType::AW);
    EXPECT_EQ(after_write_cnt, 1);
    EXPECT_EQ(last_offset, 0x12);
    EXPECT_EQ(last_size, 4);

    // Missed function
    connection.issue_callbacks(&access, simics::CallbackType::AR);
    EXPECT_EQ(after_write_cnt, 1);

    // Missed offset (Callback is called since both offset and size are 0)
    access.offset = 0x48;
    connection.issue_callbacks(&access, simics::CallbackType::AW);
    EXPECT_EQ(after_write_cnt, 2);
    EXPECT_EQ(last_offset, 0x48);
    EXPECT_EQ(last_size, 4);

    // Valid transaction again
    access.offset = 0x16;
    access.size = 2;
    connection.issue_callbacks(&access, simics::CallbackType::AW);
    EXPECT_EQ(after_write_cnt, 3);
    EXPECT_EQ(last_offset, 0x16);
    EXPECT_EQ(last_size, 2);
}

TEST(TestBankInstrumentationSubscribeConnection,
     test_MultipleRegisterBeforeRead) {
    before_read_cnt = 0;
    last_offset = 0;
    last_size = 0;

    simics::BankInstrumentationSubscribeConnection connection;

    auto handle = connection.register_before_read(NULL, 0, 0,
                                                 before_read, NULL);
    EXPECT_EQ(handle, 0);
    handle = connection.register_before_read(NULL, 0x12, 0x34,
                                             before_read, NULL);
    EXPECT_EQ(handle, 1);
    EXPECT_EQ(connection.empty(), false);
    auto num = connection.number_of_callbacks();
    EXPECT_EQ(num, 2);

    // A transaction hit the registered callback
    simics::BankAccess access {nullptr, nullptr, false, 0x12, 4};
    connection.issue_callbacks(&access, simics::CallbackType::BR);
    EXPECT_EQ(before_read_cnt, 2);
    EXPECT_EQ(last_offset, 0x12);
    EXPECT_EQ(last_size, 4);
}

TEST(TestBankInstrumentationSubscribeConnection, test_MixedRegisterCallbacks) {
    after_write_cnt = 0;
    before_read_cnt = 0;
    last_offset = 0;
    last_size = 0;

    simics::BankInstrumentationSubscribeConnection connection;

    auto handle = connection.register_after_write(NULL, 0, 0,
                                                 after_write, NULL);
    EXPECT_EQ(handle, 0);
    handle = connection.register_before_read(NULL, 0x12, 0x34,
                                             before_read, NULL);
    EXPECT_EQ(handle, 1);
    EXPECT_EQ(connection.empty(), false);
    auto num = connection.number_of_callbacks();
    EXPECT_EQ(num, 2);

    // A transaction hit the registered callback
    simics::BankAccess access {nullptr, nullptr, false, 0x12, 4};
    connection.issue_callbacks(&access, simics::CallbackType::AW);
    EXPECT_EQ(after_write_cnt, 1);
    EXPECT_EQ(before_read_cnt, 0);
    EXPECT_EQ(last_offset, 0x12);
    EXPECT_EQ(last_size, 4);

    // Another transaction hit another callback
    connection.issue_callbacks(&access, simics::CallbackType::BR);
    EXPECT_EQ(after_write_cnt, 1);
    EXPECT_EQ(before_read_cnt, 1);
}

TEST(TestBankInstrumentationSubscribeConnection,
     test_MixedConnectionCallbacks) {
    after_write_cnt = 0;
    last_offset = 0;
    last_size = 0;
    last_connection = NULL;

    simics::BankInstrumentationSubscribeConnection connection;
    conf_object_t conf_obj;
    auto handle = connection.register_after_write(&conf_obj, 0x12, 0x34,
                                                 after_write, NULL);
    EXPECT_EQ(handle, 0);
    handle = connection.register_after_write(NULL, 0x12, 0x34,
                                             after_write, NULL);
    EXPECT_EQ(handle, 1);
    EXPECT_EQ(connection.empty(), false);
    auto num = connection.number_of_callbacks();
    EXPECT_EQ(num, 2);

    // A transaction hit the registered callback
    simics::BankAccess access {nullptr, nullptr, false, 0x12, 4};
    connection.issue_callbacks(&access, simics::CallbackType::AW);
    EXPECT_EQ(after_write_cnt, 2);
    EXPECT_EQ(last_offset, 0x12);
    EXPECT_EQ(last_size, 0x4);
    // NULL connection callbacks called first
    EXPECT_EQ(last_connection, &conf_obj);
}

TEST(TestBankInstrumentationSubscribeConnection, test_RemoveCallbacks) {
    after_write_cnt = 0;
    last_offset = 0;
    last_size = 0;
    last_connection = NULL;

    simics::BankInstrumentationSubscribeConnection connection;
    conf_object_t conf_obj1;
    connection.register_after_write(&conf_obj1, 0x12, 0x34,
                                    after_write, NULL);
    auto handle = connection.register_after_write(NULL, 0x12, 0x34,
                                                 after_write, NULL);
    EXPECT_EQ(handle, 1);
    EXPECT_EQ(connection.empty(), false);
    auto num = connection.number_of_callbacks();
    EXPECT_EQ(num, 2);

    // Remove by handle
    connection.remove_callback(handle);
    num = connection.number_of_callbacks();
    EXPECT_EQ(num, 1);

    // A transaction hit the remaining registered callback
    simics::BankAccess access {nullptr, nullptr, false, 0x12, 4};
    connection.issue_callbacks(&access, simics::CallbackType::AW);
    EXPECT_EQ(after_write_cnt, 1);
    EXPECT_EQ(last_offset, 0x12);
    EXPECT_EQ(last_size, 0x4);
    // This checks that handle with id 0 is not removed
    EXPECT_EQ(last_connection, &conf_obj1);

    // Add a new connection callback to test correct one will be
    // removed
    conf_object_t conf_obj2;
    connection.register_after_write(&conf_obj2, 0x12, 0x34,
                                    after_write, NULL);

    // Remove by connection conf_obj1
    connection.remove_connection_callbacks(&conf_obj1);
    num = connection.number_of_callbacks();
    EXPECT_EQ(num, 1);
    connection.issue_callbacks(&access, simics::CallbackType::AW);
    // Connection conf_obj2 is still there
    EXPECT_EQ(last_connection, &conf_obj2);

    // Remove by connection conf_obj2
    connection.remove_connection_callbacks(&conf_obj2);
    num = connection.number_of_callbacks();
    EXPECT_EQ(num, 0);
    EXPECT_EQ(connection.empty(), true);
}

TEST(TestBankInstrumentationSubscribeConnection, test_ReorderCallbacks) {
    simics::BankInstrumentationSubscribeConnection connection;

    // Add a non-NULL connection
    conf_object_t *con1 = reinterpret_cast<conf_object_t*>(0xdead);
    connection.register_before_read(con1, 0x12, 0x34, before_read, NULL);
    EXPECT_EQ(connection.empty(), false);
    auto connections = connection.get_connections();
    EXPECT_EQ(SIM_attr_list_size(connections), 1);
    EXPECT_EQ(SIM_attr_object(SIM_attr_list_item(connections, 0)),
                      con1);

    // NULL connection by default insert in the first place
    connection.register_before_read(NULL, 0xa0, 0x4, before_read, NULL);
    connections = connection.get_connections();
    EXPECT_EQ(SIM_attr_list_size(connections), 2);
    EXPECT_EQ(SIM_attr_is_nil(SIM_attr_list_item(connections, 0)),
                      true);
    EXPECT_EQ(SIM_attr_object(SIM_attr_list_item(connections, 1)),
                      con1);

    // Non-NULL connection insert at the back
    conf_object_t *con2 = reinterpret_cast<conf_object_t*>(0xbeef);
    connection.register_before_write(con2, 0x10, 0x20, before_write, NULL);
    connections = connection.get_connections();
    EXPECT_EQ(SIM_attr_list_size(connections), 3);
    EXPECT_EQ(SIM_attr_is_nil(SIM_attr_list_item(connections, 0)),
                      true);
    EXPECT_EQ(SIM_attr_object(SIM_attr_list_item(connections, 1)),
                      con1);
    EXPECT_EQ(SIM_attr_object(SIM_attr_list_item(connections, 2)),
                      con2);

    // move before itself is a nop
    connection.move_before(con1, con1);
    connections = connection.get_connections();
    EXPECT_EQ(SIM_attr_list_size(connections), 3);
    EXPECT_EQ(SIM_attr_is_nil(SIM_attr_list_item(connections, 0)),
                      true);
    EXPECT_EQ(SIM_attr_object(SIM_attr_list_item(connections, 1)),
                      con1);
    EXPECT_EQ(SIM_attr_object(SIM_attr_list_item(connections, 2)),
                      con2);

    // Move the con1 to the end
    connection.move_before(con1, nullptr);
    connections = connection.get_connections();
    EXPECT_EQ(SIM_attr_is_nil(SIM_attr_list_item(connections, 0)),
                      true);
    EXPECT_EQ(SIM_attr_object(SIM_attr_list_item(connections, 1)),
                      con2);
    EXPECT_EQ(SIM_attr_object(SIM_attr_list_item(connections, 2)),
                      con1);

    // Move the NULL connection before con1
    connection.move_before(nullptr, con1);
    connections = connection.get_connections();
    EXPECT_EQ(SIM_attr_object(SIM_attr_list_item(connections, 0)),
                      con2);
    EXPECT_EQ(SIM_attr_is_nil(SIM_attr_list_item(connections, 1)),
                      true);
    EXPECT_EQ(SIM_attr_object(SIM_attr_list_item(connections, 2)),
                      con1);
}
