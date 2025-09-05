// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2014 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <boost/test/unit_test.hpp>

#include <simics/base/types.h>


#include <simics/systemc/simics2tlm/io_memory.h>
#include <simics/systemc/simics2tlm/io_memory_gasket_adapter.h>
#include <simics/systemc/iface/io_memory_interface.h>
#include <simics/systemc/iface/io_memory_simics_adapter.h>
#include <simics/systemc/iface/map_info_extension.h>

#include "mock/iface/mock_io_memory.h"
#include "mock/mock_simulation.h"
#include "mock/simics2tlm/mock_gasket.h"
#include "environment.h"
#include "stubs.h"

BOOST_AUTO_TEST_SUITE(TestIoMemory)

using simics::systemc::simics2tlm::GasketInterface;

class MockDevice : public simics::ConfObject,
                   public unittest::iface::MockIoMemory,
                   public unittest::MockSimulation {
  public:
    explicit MockDevice(simics::ConfObjectRef objectRef)
        : simics::ConfObject(objectRef) {
    }
};

class MockIoGasket : public unittest::simics2tlm::MockGasket {
  public:
    MockIoGasket() : function_(-1), transaction_(NULL) {}
    virtual bool trigger(simics::systemc::iface::Transaction *transaction) {
        function_ = (*transaction)->get_extension<
            simics::systemc::iface::MapInfoExtension>()->function();
        transaction_ = *transaction;
        return MockGasket::trigger(transaction);
    }
    int function_;
    simics::systemc::iface::Transaction transaction_;
};

class TestEnvironment : public Environment {
  public:
    TestEnvironment()
        : object_(&conf_), device_(object_) {
        conf_.instance_data = &device_;
    }
    conf_object_t conf_;
    simics::ConfObjectRef object_;
    MockDevice device_;
};

BOOST_AUTO_TEST_CASE(testIoMemoryMap) {
    simics::systemc::simics2tlm::IoMemory io_memory;
    io_memory.set_gasket(GasketInterface::Ptr(
            new unittest::simics2tlm::MockGasket));

    map_info_t info = {};

    BOOST_CHECK_EQUAL(io_memory._deprecated_map(Sim_Addr_Space_Memory, info),
                      0);
}

BOOST_AUTO_TEST_CASE(testIoMemoryOperation) {
    unittest::simics2tlm::MockGasket *gasket =
        new unittest::simics2tlm::MockGasket;
    simics::systemc::simics2tlm::IoMemory io_memory;
    io_memory.set_gasket(GasketInterface::Ptr(gasket));

    simics::types::map_info_t info;
    generic_transaction_t trans = {};

    // Normal access, no error
    BOOST_CHECK_EQUAL(io_memory.operation(&trans, info), Sim_PE_No_Exception);
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 1);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_info_count_, 0);

    // Inquiry access, no error
    SIM_set_mem_op_inquiry(&trans, true);
    BOOST_CHECK_EQUAL(io_memory.operation(&trans, info), Sim_PE_No_Exception);
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 2);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_info_count_, 0);

    // Normal access, error
    gasket->transaction_result_ = false;
    SIM_set_mem_op_inquiry(&trans, false);
    BOOST_CHECK_EQUAL(io_memory.operation(&trans, info), Sim_PE_IO_Error);
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 3);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_info_count_, 0);

    // Inquiry access, error
    SIM_set_mem_op_inquiry(&trans, true);
    BOOST_CHECK_EQUAL(io_memory.operation(&trans, info),
                      Sim_PE_Inquiry_Unhandled);
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 4);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_info_count_, 0);

    // Normal access, unmapped
    gasket->transaction_status_ = tlm::TLM_ADDRESS_ERROR_RESPONSE;
    SIM_set_mem_op_inquiry(&trans, false);
    BOOST_CHECK_EQUAL(io_memory.operation(&trans, info), Sim_PE_IO_Not_Taken);
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 5);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_info_count_, 0);

    // Inquiry access, unmapped
    SIM_set_mem_op_inquiry(&trans, true);
    BOOST_CHECK_EQUAL(io_memory.operation(&trans, info),
                      Sim_PE_Inquiry_Outside_Memory);
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 6);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_info_count_, 0);
}

BOOST_AUTO_TEST_CASE(testIoMemoryOperationMulti) {
    MockIoGasket *gasket[2] = { new MockIoGasket,
                                new MockIoGasket };
    simics::systemc::simics2tlm::IoMemory io_memory;
    io_memory.addGasket(1, GasketInterface::Ptr(gasket[0]));
    io_memory.addGasket(255, GasketInterface::Ptr(gasket[1]));

    generic_transaction_t trans = {};
    simics::types::map_info_t info;
    info.function = 1;
    BOOST_CHECK_EQUAL(io_memory.operation(&trans, info), Sim_PE_No_Exception);
    BOOST_CHECK_EQUAL(gasket[0]->function_, 1);
    BOOST_CHECK_EQUAL(gasket[0]->trigger_transaction_, 1);
    BOOST_CHECK_EQUAL(gasket[1]->trigger_transaction_, 0);
    info.function = 255;
    BOOST_CHECK_EQUAL(io_memory.operation(&trans, info), Sim_PE_No_Exception);
    BOOST_CHECK_EQUAL(gasket[1]->function_, 255);
    BOOST_CHECK_EQUAL(gasket[0]->trigger_transaction_, 1);
    BOOST_CHECK_EQUAL(gasket[1]->trigger_transaction_, 1);
    info.function = 0;
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_info_count_, 0);
    BOOST_CHECK_EQUAL(io_memory.operation(&trans, info), Sim_PE_No_Exception);
    BOOST_CHECK_EQUAL(gasket[0]->function_, 0);
    BOOST_CHECK_EQUAL(gasket[0]->trigger_transaction_, 2);
    BOOST_CHECK_EQUAL(gasket[1]->trigger_transaction_, 1);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_info_count_, 1);
    // extensions must be cleared
    BOOST_CHECK(gasket[0]->transaction_->get_extension<
                simics::systemc::iface::MapInfoExtension>() == NULL);
}

BOOST_FIXTURE_TEST_CASE(testIoMemorySimicsAdapterMap, TestEnvironment) {
    simics::systemc::iface::IoMemorySimicsAdapter<MockDevice> simicsAdapter;
    const io_memory_interface_t *interface =
        static_cast<const io_memory_interface_t *>(simicsAdapter.cstruct());

    BOOST_CHECK(interface->_deprecated_map == NULL);
}

BOOST_FIXTURE_TEST_CASE(testIoMemorySimicsAdapterOperation, TestEnvironment) {
    generic_transaction_t trans = {};
    ::map_info_t info;

    simics::systemc::iface::IoMemorySimicsAdapter<MockDevice> simicsAdapter;
    const io_memory_interface_t *interface =
        static_cast<const io_memory_interface_t *>(simicsAdapter.cstruct());

    BOOST_CHECK_EQUAL(interface->operation(&conf_, &trans, info),
                      Sim_PE_No_Exception);
    BOOST_CHECK_EQUAL(device_.operation_, 1);
    BOOST_CHECK_EQUAL(interface->operation(&conf_, &trans, info),
                      Sim_PE_No_Exception);
    BOOST_CHECK_EQUAL(device_.operation_, 2);
}

BOOST_FIXTURE_TEST_CASE(testIoMemoryGasketAdapterOperation, TestEnvironment) {
    generic_transaction_t trans = {};
    simics::types::map_info_t info;

    simics::systemc::simics2tlm::IoMemoryGasketAdapter adapter(&device_,
                                                               &simulation_);
    BOOST_CHECK_EQUAL(adapter.operation(&trans, info), Sim_PE_No_Exception);
    BOOST_CHECK_EQUAL(device_.operation_, 1);
}

BOOST_AUTO_TEST_CASE(testIoMemoryType) {
    simics::systemc::simics2tlm::IoMemory io_memory;
    simics::systemc::ClassType *type = &io_memory;

    BOOST_CHECK(*type == io_memory);
    BOOST_CHECK(*type != simics::systemc::ClassType());
    BOOST_CHECK(simics::systemc::ClassType() ==
                simics::systemc::ClassType());
}

BOOST_AUTO_TEST_SUITE_END()
