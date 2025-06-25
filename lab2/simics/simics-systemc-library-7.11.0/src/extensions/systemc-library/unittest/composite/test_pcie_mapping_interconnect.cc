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

#include <simics/systemc/iface/extension_dispatcher.h>
#include <simics/systemc/composite/pcie_mapping_interconnect.h>

#include <vector>

#include "environment.h"
#include "stubs.h"

BOOST_AUTO_TEST_SUITE(TestPcieMappingInterconnect)

class ExtensionSenderModule : public sc_core::sc_module {
  public:
    explicit ExtensionSenderModule(
            sc_core::sc_module_name = "ExtensionSenderModule") {
        target.register_b_transport(
                this, &ExtensionSenderModule::b_transport);
        init.bind(target);
    }

    void b_transport(tlm::tlm_generic_payload &trans,  // NOLINT
                     sc_core::sc_time &t) {  // NOLINT
        ++b_transport_cnt_;
    }

    tlm_utils::simple_initiator_socket<ExtensionSenderModule> init;
    tlm_utils::simple_target_socket<ExtensionSenderModule> target;
    int b_transport_cnt_ {0};
};

BOOST_FIXTURE_TEST_CASE(testExtensionSender, Environment) {
    ExtensionSenderModule m;
    simics::systemc::composite::PcieMappingInterconnectExtensionSender<
        tlm_utils::simple_initiator_socket<ExtensionSenderModule>> es;
    es.init(&m.init);

    BOOST_CHECK_EQUAL(m.b_transport_cnt_, 0);
    auto t = es.transaction();
    es.send_extension(&t);
    BOOST_CHECK_EQUAL(m.b_transport_cnt_, 1);

    BOOST_CHECK_EQUAL(es.failed_transaction_.payload(), nullptr);
    BOOST_CHECK_THROW(es.send_failed(&t), std::exception);
    BOOST_CHECK_EQUAL(es.failed_transaction_.payload(), t.payload());
}

BOOST_FIXTURE_TEST_CASE(testInterconnectCreation, Environment) {
    {
        simics::systemc::composite::PcieMappingInterconnect<
            32, tlm::tlm_base_protocol_types> m;
        BOOST_CHECK_EQUAL(m.name(), "PcieMappingInterconnect");
    }

    {
        simics::systemc::composite::PcieMappingInterconnect<
            64, tlm::tlm_base_protocol_types> m;
        BOOST_CHECK_EQUAL(m.name(), "PcieMappingInterconnect");
    }
}

class TestInputModule : public sc_core::sc_module {
  public:
    explicit TestInputModule(
            sc_core::sc_module_name = "TestInputModule") {}

    sc_core::sc_signal<bool> warm_reset_signal;
    tlm_utils::simple_initiator_socket<ExtensionSenderModule> init;
};

BOOST_FIXTURE_TEST_CASE(testInterconnectInput, Environment) {
    simics::systemc::composite::PcieMappingInterconnect<
        32, tlm::tlm_base_protocol_types> m;
    TestInputModule ti;

    // Test send signal to warm_reset_pin, since reset_ is nullptr,
    // nothing should happen
    m.warm_reset_pin.bind(ti.warm_reset_signal);
    ti.warm_reset_signal = true;
    ti.warm_reset_signal = false;

    // Test send transaction to transaction_target_socket
    sc_core::sc_time t {sc_core::SC_ZERO_TIME};
    ti.init.bind(m.transaction_target_socket);
    {
        // Empty GP
        tlm::tlm_generic_payload trans;
        // report error about PcieTlmExtension is missing
        BOOST_CHECK_THROW(ti.init->b_transport(trans, t), std::exception);
        BOOST_CHECK_EQUAL(trans.get_response_status(),
                          tlm::TLM_GENERIC_ERROR_RESPONSE);
    }

    {
        // GP with empty PcieTlmExtension
        tlm::tlm_generic_payload trans;
        simics::systemc::PcieTlmExtension ext;
        ext.type = simics::types::PCIE_Type_Other;
        trans.set_extension<simics::systemc::PcieTlmExtension>(&ext);
        ti.init->b_transport(trans, t);
        // Pcie type not supported
        BOOST_CHECK_EQUAL(trans.get_response_status(),
                          tlm::TLM_GENERIC_ERROR_RESPONSE);
        trans.clear_extension(&ext);
    }

    {
        // GP with empty PcieTlmExtension
        tlm::tlm_generic_payload trans;
        simics::systemc::PcieTlmExtension ext;
        trans.set_extension<simics::systemc::PcieTlmExtension>(&ext);
        ti.init->b_transport(trans, t);
        // Pcie type not set
        BOOST_CHECK_EQUAL(trans.get_response_status(),
                          tlm::TLM_GENERIC_ERROR_RESPONSE);
        trans.clear_extension(&ext);
    }

    {
        // GP with PcieTlmExtension, type = MEM/IO
        tlm::tlm_generic_payload trans;
        simics::systemc::PcieTlmExtension ext;
        trans.set_extension<simics::systemc::PcieTlmExtension>(&ext);

        std::array<simics::types::pcie_type_t, 2> types {
            simics::types::PCIE_Type_Mem,
            simics::types::PCIE_Type_IO
        };
        for (simics::types::pcie_type_t type : types) {
            ext.type = type;
            ti.init->b_transport(trans, t);
            // Address 0 not mapped
            BOOST_CHECK_EQUAL(trans.get_response_status(),
                              tlm::TLM_ADDRESS_ERROR_RESPONSE);
        }
        trans.clear_extension(&ext);
    }

    {
        // GP with PcieTlmExtension, type = MSG
        tlm::tlm_generic_payload trans;
        simics::systemc::PcieTlmExtension ext;
        trans.set_extension<simics::systemc::PcieTlmExtension>(&ext);
        ext.type = simics::types::PCIE_Type_Msg;
        ti.init->b_transport(trans, t);
        BOOST_CHECK_EQUAL(trans.get_response_status(),
                          tlm::TLM_GENERIC_ERROR_RESPONSE);
        trans.clear_extension(&ext);
    }

    {
        // GP with PcieTlmExtension, type = CFG
        tlm::tlm_generic_payload trans;
        simics::systemc::PcieTlmExtension ext;
        trans.set_extension<simics::systemc::PcieTlmExtension>(&ext);
        ext.type = simics::types::PCIE_Type_Cfg;
        ext.device_id = 0;
        ext.device_id_set = true;
        ti.init->b_transport(trans, t);
        BOOST_CHECK_EQUAL(trans.get_response_status(),
                          tlm::TLM_GENERIC_ERROR_RESPONSE);
        trans.clear_extension(&ext);
    }
}

class FakeUpstreamTarget : public sc_core::sc_module,
                           public simics::systemc::iface::PcieMapInterface {
  public:
    explicit FakeUpstreamTarget(
            sc_core::sc_module_name = "FakeUpstreamTarget") {
        target.register_b_transport(
                this, &FakeUpstreamTarget::b_transport);
        dispatcher_.subscribe(
                simics::systemc::iface::PcieMapExtension::createReceiver(this));
    }

    void b_transport(tlm::tlm_generic_payload &trans,  // NOLINT
                     sc_core::sc_time &t) {  // NOLINT
        dispatcher_.handle(&trans);
    }

    void add_map(simics::types::map_info_t info,
                 simics::types::pcie_type_t type) override {}
    void del_map(simics::types::map_info_t::physical_address_t base,
                 simics::types::pcie_type_t type) override {}
    void add_function(conf_object_t *map_obj, uint16_t function_id) override {
        add_function_ids_.push_back(function_id);
    }
    void del_function(conf_object_t *map_obj, uint16_t function_id) override {
        del_function_ids_.push_back(function_id);
    }
    void enable_function(uint16_t function_id) override {}
    void disable_function(uint16_t function_id) override {}
    uint16_t get_device_id(conf_object_t *dev_obj) {return 0;}

    simics::systemc::iface::ExtensionDispatcher dispatcher_;
    tlm_utils::simple_target_socket<FakeUpstreamTarget> target;

    std::vector<uint16_t> add_function_ids_ {};
    std::vector<uint16_t> del_function_ids_ {};
};

class TestModule
    : public sc_core::sc_module,
      public simics::systemc::iface::PcieDeviceQueryInterface,
      public simics::systemc::iface::PcieBaseAddressRegisterQueryInterface,
      public simics::systemc::iface::PcieResetInterface {
  public:
    explicit TestModule(sc_core::sc_module_name = "TestModule") {}

    // simics::systemc::iface::PcieDeviceQueryInterface
    sc_core::sc_object *getConfigTargetSocket() override {
        ++getConfigTargetSocket_called_;
        return &target;
    }
    sc_core::sc_object *getPcieMapInitiatorSocket() override {
        ++getPcieMapInitiatorSocket_called_;
        return &initiator;
    }
    sc_core::sc_object *getMsgTargetSocket() override {
        ++getMsgTargetSocket_called_;
        return &msg_target;
    }

    // simics::systemc::iface::PcieBaseAddressRegisterQueryInterface
    std::vector<
        simics::systemc::iface::PcieBaseAddressRegisterQueryInterface::PCIeBar>
    getBarInfo() override {
        ++getBarInfo_called_;
        return getBarInfo_return_;
    }

    // simics::systemc::iface::PcieResetInterface
    void functionLevelReset(int function_number) override {
        ++flr_reset_cnt_;
    };
    void warmReset() override {
        ++warm_reset_cnt_;
    };
    void hotReset() override {
        ++hot_reset_cnt_;
    };

    // sockets
    tlm_utils::simple_initiator_socket<TestModule> initiator;
    tlm_utils::simple_target_socket<TestModule> target;
    tlm_utils::simple_target_socket<TestModule> bar0_target;
    tlm_utils::simple_target_socket<TestModule> bar1_target;
    tlm_utils::simple_target_socket<TestModule> msg_target;

    int getConfigTargetSocket_called_{0};
    int getPcieMapInitiatorSocket_called_{0};
    int getMsgTargetSocket_called_{0};
    int getPcieWarmResetPin_called_{0};
    int getBarInfo_called_{0};
    std::vector<
        simics::systemc::iface::
        PcieBaseAddressRegisterQueryInterface::PCIeBar> getBarInfo_return_ {};
    int flr_reset_cnt_ {0};
    int warm_reset_cnt_ {0};
    int hot_reset_cnt_ {0};
};

BOOST_FIXTURE_TEST_CASE(testPcieDeviceEmptyBarInfo, Environment) {
    simics::systemc::composite::PcieMappingInterconnect<
        32, tlm::tlm_base_protocol_types> m;
    FakeUpstreamTarget up;
    m.pcie_map_initiator_socket.bind(up.target);
    conf_object_t conf_;
    Stubs::instance_.SIM_object_descendant_ = &conf_;
    up.add_function_ids_.clear();

    m.connected(0xa0);
    std::vector<uint16_t> expected_function_ids = {0xa0};
    BOOST_CHECK_EQUAL_COLLECTIONS(up.add_function_ids_.begin(),
                                  up.add_function_ids_.end(),
                                  expected_function_ids.begin(),
                                  expected_function_ids.end());
    // When connected, it deletes function first before adds it
    BOOST_CHECK_EQUAL_COLLECTIONS(up.del_function_ids_.begin(),
                                  up.del_function_ids_.end(),
                                  expected_function_ids.begin(),
                                  expected_function_ids.end());
    // Function 0 is always required
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_object_descendant_relname_,
                      "port.cfg0");

    m.disconnected(0xa0);
    expected_function_ids = {0xa0, 0xa0};
    BOOST_CHECK_EQUAL_COLLECTIONS(up.del_function_ids_.begin(),
                                  up.del_function_ids_.end(),
                                  expected_function_ids.begin(),
                                  expected_function_ids.end());

    m.hotReset();
}

BOOST_FIXTURE_TEST_CASE(testPcieDeviceMultiFunction, Environment) {
    simics::systemc::composite::PcieMappingInterconnect<
        32, tlm::tlm_base_protocol_types> m;
    FakeUpstreamTarget up;
    m.pcie_map_initiator_socket.bind(up.target);
    conf_object_t conf_;
    Stubs::instance_.SIM_object_descendant_ = &conf_;
    TestModule tb;
    simics::ConfObjectRef simics_obj(&conf_);
    up.add_function_ids_.clear();

    simics::systemc::iface::PcieBaseAddressRegisterQueryInterface::PCIeBar bar0;
    bar0.function = 0;
    bar0.offset = 0x10;
    bar0.is_memory = true;
    bar0.is_64bit = true;
    bar0.size_bits = 16;  // 64KB
    bar0.target_socket = &tb.bar0_target;
    simics::systemc::iface::PcieBaseAddressRegisterQueryInterface::PCIeBar bar1;
    bar1.function = 5;
    bar1.offset = 0x18;
    bar1.is_memory = false;
    bar1.size_bits = 10;  // 1KB
    bar1.target_socket = &tb.bar1_target;
    tb.getBarInfo_return_ = {bar0, bar1};

    BOOST_CHECK_EQUAL(tb.getBarInfo_called_, 0);
    m.connect(&tb, &tb, &tb, simics_obj);
    BOOST_CHECK_EQUAL(tb.getBarInfo_called_, 1);

    // In Simics this is called automatically, but in the test
    // it needs to be called manually
    m.before_end_of_elaboration();

    m.connected(0xa0);
    std::vector<uint16_t> expected_function_ids = {0xa0, 0xa5};
    BOOST_CHECK_EQUAL_COLLECTIONS(up.add_function_ids_.begin(),
                                  up.add_function_ids_.end(),
                                  expected_function_ids.begin(),
                                  expected_function_ids.end());
    // When connected, it deletes function first before adds it
    BOOST_CHECK_EQUAL_COLLECTIONS(up.del_function_ids_.begin(),
                                  up.del_function_ids_.end(),
                                  expected_function_ids.begin(),
                                  expected_function_ids.end());
    // The last accessed function is 5
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_object_descendant_relname_,
                      "port.cfg5");

    m.disconnected(0xa0);
    expected_function_ids = {0xa0, 0xa5, 0xa0, 0xa5};
    BOOST_CHECK_EQUAL_COLLECTIONS(up.del_function_ids_.begin(),
                                  up.del_function_ids_.end(),
                                  expected_function_ids.begin(),
                                  expected_function_ids.end());
    m.hotReset();
}

BOOST_FIXTURE_TEST_CASE(testPcieDeviceARIFunction, Environment) {
    simics::systemc::composite::PcieMappingInterconnect<
        32, tlm::tlm_base_protocol_types> m;
    FakeUpstreamTarget up;
    m.pcie_map_initiator_socket.bind(up.target);
    conf_object_t conf_;
    Stubs::instance_.SIM_object_descendant_ = &conf_;
    TestModule tb;
    simics::ConfObjectRef simics_obj(&conf_);
    up.add_function_ids_.clear();

    simics::systemc::iface::PcieBaseAddressRegisterQueryInterface::PCIeBar bar0;
    bar0.function = 0;
    bar0.offset = 0x10;
    bar0.is_memory = true;
    bar0.is_64bit = true;
    bar0.size_bits = 16;  // 64KB
    bar0.target_socket = &tb.bar0_target;
    simics::systemc::iface::PcieBaseAddressRegisterQueryInterface::PCIeBar bar1;
    // Function can be 0-255 when ARI is enabled
    bar1.function = 255;
    bar1.offset = 0x18;
    bar1.is_memory = false;
    bar1.size_bits = 10;  // 1KB
    bar1.target_socket = &tb.bar1_target;
    tb.getBarInfo_return_ = {bar0, bar1};

    BOOST_CHECK_EQUAL(tb.getBarInfo_called_, 0);
    m.connect(&tb, &tb, &tb, simics_obj);
    BOOST_CHECK_EQUAL(tb.getBarInfo_called_, 1);

    // In Simics this is called automatically, but in the test
    // it needs to be called manually
    m.before_end_of_elaboration();

    m.connected(0);
    std::vector<uint16_t> expected_function_ids = {0, 0xff};
    BOOST_CHECK_EQUAL_COLLECTIONS(up.add_function_ids_.begin(),
                                  up.add_function_ids_.end(),
                                  expected_function_ids.begin(),
                                  expected_function_ids.end());
    // When connected, it deletes function first before adds it
    BOOST_CHECK_EQUAL_COLLECTIONS(up.del_function_ids_.begin(),
                                  up.del_function_ids_.end(),
                                  expected_function_ids.begin(),
                                  expected_function_ids.end());
    // The last accessed function is 255
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_object_descendant_relname_,
                      "port.cfg255");

    m.disconnected(0);
    expected_function_ids = {0, 0xff, 0, 0xff};
    BOOST_CHECK_EQUAL_COLLECTIONS(up.del_function_ids_.begin(),
                                  up.del_function_ids_.end(),
                                  expected_function_ids.begin(),
                                  expected_function_ids.end());
}

BOOST_FIXTURE_TEST_CASE(testConnect, Environment) {
    simics::systemc::composite::PcieMappingInterconnect<
        32, tlm::tlm_base_protocol_types> m;
    conf_object_t conf_;
    simics::ConfObjectRef simics_obj(&conf_);
    TestModule tb;
    Stubs::instance_.created_obj_ = &conf_;

    BOOST_CHECK_EQUAL(tb.getConfigTargetSocket_called_, 0);
    BOOST_CHECK_EQUAL(tb.getPcieMapInitiatorSocket_called_, 0);
    BOOST_CHECK_EQUAL(tb.getMsgTargetSocket_called_, 0);
    BOOST_CHECK_EQUAL(tb.getBarInfo_called_, 0);
    m.connect(&tb, &tb, &tb, simics_obj);
    BOOST_CHECK_EQUAL(tb.getConfigTargetSocket_called_, 1);
    BOOST_CHECK_EQUAL(tb.getPcieMapInitiatorSocket_called_, 1);
    BOOST_CHECK_EQUAL(tb.getMsgTargetSocket_called_, 1);
    BOOST_CHECK_EQUAL(tb.getBarInfo_called_, 1);
}

BOOST_AUTO_TEST_SUITE_END()
