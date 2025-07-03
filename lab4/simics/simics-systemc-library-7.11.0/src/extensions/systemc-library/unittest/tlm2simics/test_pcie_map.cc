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

#include <simics/devs/pci.h>
#include <simics/systemc/iface/pcie_map_extension.h>
#include <simics/systemc/tlm2simics/pcie_map.h>

#include "test_gasket_environment.h"
#include "stubs.h"

BOOST_AUTO_TEST_SUITE(TestTlm2SimicsPcieMap)

class MockPcieMapSimicsInterfaceImpl : public pcie_map_interface_t {
  public:
    MockPcieMapSimicsInterfaceImpl() {
        instance_ = this;
        add_map = mock_add_map;
        del_map = mock_del_map;
        add_function = mock_add_function;
        del_function = mock_del_function;
        enable_function = mock_enable_function;
        disable_function = mock_disable_function;
        get_device_id = mock_get_device_id;
    }
    ~MockPcieMapSimicsInterfaceImpl() {
    }
    static void mock_add_map(conf_object_t *obj, conf_object_t *map_obj,
                             map_info_t info, pcie_type_t type) {
        instance_->add_map_called_ = true;

        instance_->obj_ = obj;
        instance_->map_obj_ = map_obj;
        instance_->info_ = info;
        instance_->type_ = type;
    }
    static void mock_del_map(conf_object_t *obj, conf_object_t *map_obj,
                             physical_address_t base, pcie_type_t type) {
        instance_->del_map_called_ = true;

        instance_->obj_ = obj;
        instance_->map_obj_ = map_obj;
        instance_->base_ = base;
        instance_->type_ = type;
    }
    static void mock_add_function(conf_object_t *obj, conf_object_t *map_obj,
                                  uint16 function_id) {
        instance_->add_function_called_ = true;

        instance_->obj_ = obj;
        instance_->map_obj_ = map_obj;
        instance_->function_id_ = function_id;
    }
    static void mock_del_function(conf_object_t *obj, conf_object_t *map_obj,
                                  uint16 function_id) {
        instance_->del_function_called_ = true;

        instance_->obj_ = obj;
        instance_->map_obj_ = map_obj;
        instance_->function_id_ = function_id;
    }
    static void mock_enable_function(conf_object_t *obj, uint16 function_id) {
        instance_->enable_function_called_ = true;

        instance_->obj_ = obj;
        instance_->function_id_ = function_id;
    }
    static void mock_disable_function(conf_object_t *obj, uint16 function_id) {
        instance_->disable_function_called_ = true;

        instance_->obj_ = obj;
        instance_->function_id_ = function_id;
    }
    static uint16 mock_get_device_id(conf_object_t *obj,
                                     conf_object_t *map_obj) {
        instance_->get_device_id_called_ = true;

        instance_->obj_ = obj;
        instance_->map_obj_ = map_obj;
        return 4711;
    }

    static MockPcieMapSimicsInterfaceImpl *instance_;

    bool add_map_called_ {false};
    bool del_map_called_ {false};
    bool add_function_called_ {false};
    bool del_function_called_ {false};
    bool enable_function_called_ {false};
    bool disable_function_called_ {false};
    bool get_device_id_called_ {false};

    conf_object_t *obj_ {nullptr};
    conf_object_t *map_obj_ {nullptr};
    map_info_t info_;
    pcie_type_t type_ {::PCIE_Type_Not_Set};
    physical_address_t base_ {0};
    uint16 function_id_ {0};
};

MockPcieMapSimicsInterfaceImpl *
    MockPcieMapSimicsInterfaceImpl::instance_ = NULL;

class TestEnv
    : public TestGasketEnvironment<simics::systemc::tlm2simics::PcieMap,
                                   simics::systemc::iface::PcieMapExtension,
                                   MockPcieMapSimicsInterfaceImpl> {
  public:
    TestEnv() : TestGasketEnvironment(PCIE_MAP_INTERFACE) {}
};

BOOST_FIXTURE_TEST_CASE(testAddMap, TestEnv) {
    simics::types::map_info_t info;
    info.base = 4;
    simics::types::pcie_type_t type = simics::types::PCIE_Type_Msg;
    conf_object_t map_obj;
    Stubs::instance_.SIM_object_descendant_ = &map_obj;

    extension_.add_map(info, type);
    BOOST_CHECK_EQUAL(response_, tlm::TLM_OK_RESPONSE);

    BOOST_CHECK(interface_.add_map_called_);
    BOOST_CHECK_EQUAL(interface_.obj_, simulation_.simics_object().object());
    BOOST_CHECK_EQUAL(interface_.map_obj_, &map_obj);
    BOOST_CHECK_EQUAL(interface_.info_.base, 4);
    BOOST_CHECK_EQUAL(interface_.type_,
                      static_cast<int>(simics::types::PCIE_Type_Msg));
}

BOOST_FIXTURE_TEST_CASE(testDelMap, TestEnv) {
    simics::types::map_info_t::physical_address_t base;
    base = 3;
    simics::types::pcie_type_t type = simics::types::PCIE_Type_Mem;
    conf_object_t map_obj;
    Stubs::instance_.SIM_object_descendant_ = &map_obj;

    extension_.del_map(base, type);
    BOOST_CHECK_EQUAL(response_, tlm::TLM_OK_RESPONSE);

    BOOST_CHECK(interface_.del_map_called_);
    BOOST_CHECK_EQUAL(interface_.obj_, simulation_.simics_object().object());
    BOOST_CHECK_EQUAL(interface_.map_obj_, &map_obj);
    BOOST_CHECK_EQUAL(interface_.base_, 3);
    BOOST_CHECK_EQUAL(interface_.type_,
                      static_cast<int>(simics::types::PCIE_Type_Mem));
}

BOOST_FIXTURE_TEST_CASE(testAddFunction, TestEnv) {
    uint16_t function_id {1};
    conf_object_t map_obj;

    extension_.add_function(&map_obj, function_id);
    BOOST_CHECK_EQUAL(response_, tlm::TLM_OK_RESPONSE);

    BOOST_CHECK(interface_.add_function_called_);
    BOOST_CHECK_EQUAL(interface_.obj_, simulation_.simics_object().object());
    // interface_.map_obj_ is not equal to &map_obj since Simics 22414
    BOOST_CHECK_EQUAL(interface_.function_id_, 1);
}

BOOST_FIXTURE_TEST_CASE(testDelFunction, TestEnv) {
    uint16_t function_id {2};
    conf_object_t map_obj;

    extension_.del_function(&map_obj, function_id);
    BOOST_CHECK_EQUAL(response_, tlm::TLM_OK_RESPONSE);

    BOOST_CHECK(interface_.del_function_called_);
    BOOST_CHECK_EQUAL(interface_.obj_, simulation_.simics_object().object());
    BOOST_CHECK_EQUAL(interface_.map_obj_, &map_obj);
    BOOST_CHECK_EQUAL(interface_.function_id_, 2);
}

BOOST_FIXTURE_TEST_CASE(testEnableFunction, TestEnv) {
    uint16_t function_id {3};

    extension_.enable_function(function_id);
    BOOST_CHECK_EQUAL(response_, tlm::TLM_OK_RESPONSE);

    BOOST_CHECK(interface_.enable_function_called_);
    BOOST_CHECK_EQUAL(interface_.obj_, simulation_.simics_object().object());
    BOOST_CHECK_EQUAL(interface_.function_id_, 3);
}

BOOST_FIXTURE_TEST_CASE(testDisableFunction, TestEnv) {
    uint16_t function_id {4};

    extension_.disable_function(function_id);
    BOOST_CHECK_EQUAL(response_, tlm::TLM_OK_RESPONSE);

    BOOST_CHECK(interface_.disable_function_called_);
    BOOST_CHECK_EQUAL(interface_.obj_, simulation_.simics_object().object());
    BOOST_CHECK_EQUAL(interface_.function_id_, 4);
}

BOOST_FIXTURE_TEST_CASE(testGetDeviceId, TestEnv) {
    conf_object_t map_obj;
    auto device_id = extension_.get_device_id(&map_obj);
    BOOST_CHECK_EQUAL(response_, tlm::TLM_OK_RESPONSE);

    BOOST_CHECK(interface_.get_device_id_called_);
    BOOST_CHECK_EQUAL(interface_.obj_, simulation_.simics_object().object());
    BOOST_CHECK_EQUAL(interface_.map_obj_, &map_obj);
    BOOST_CHECK_EQUAL(device_id, 4711);
}

BOOST_AUTO_TEST_SUITE_END()
