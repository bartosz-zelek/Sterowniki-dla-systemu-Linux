// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2016 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <boost/test/unit_test.hpp>

#include <simics/systemc/instrumentation/tool_controller.h>
#include <simics/systemc/instrumentation/tool_connection_interface.h>
#include <simics/systemc/iface/instrumentation/connection_interface.h>

#include "environment.h"
#include "mock/awareness/mock_proxy.h"

using simics::systemc::instrumentation::ToolController;
using simics::systemc::instrumentation::ToolConnectionInterface;
using simics::systemc::iface::instrumentation::ConnectionInterface;
using unittest::awareness::MockProxy;

BOOST_AUTO_TEST_SUITE(TestToolController)

class MockInterface {
  public:
    bool f0(bool p) {
        ++call_cnt_;
        if (p)
            ++true_cnt_;
        return p;
    }
    void f1() {
        ++call_cnt_;
    }

    static int call_cnt_;
    static int true_cnt_;
};

int MockInterface::call_cnt_ = 0;
int MockInterface::true_cnt_ = 0;

class MockTool : public simics::ConfObject, public MockInterface {
  public:
    explicit MockTool(simics::ConfObjectRef o) : simics::ConfObject(o) {}
};

class MockToolController
    : public ToolController,
      public ToolController::CallbackInterface,
      public MockProxy {
  public:
    MockToolController()
        : ToolController(this), tool_controller_init_cnt_(0),
          node_list_updated_cnt_(0), mock_state_(FIRST_ELEMENT_ADDED) {}
    virtual void tool_controller_init(ToolController *controller) {
        ++tool_controller_init_cnt_;
    }
    void connection_list_updated(ConnectionListState state) {
        ++node_list_updated_cnt_;
        mock_state_ = state;
    }

    int tool_controller_init_cnt_;
    int node_list_updated_cnt_;
    ConnectionListState mock_state_;
};

class MockToolConnection
    : public ToolConnectionInterface,
      public ConnectionInterface {
  public:
    MockToolConnection() : enabled_(true), controller_(NULL) {}
    virtual void set_functions(std::vector<std::string> *functions) {
        functions_ = *functions;
    }
    virtual const std::vector<std::string> &functions() const {
        return functions_;
    }
    virtual bool enabled() const {
        return enabled_;
    }
    virtual void set_tool(simics::ConfObjectRef tool) {
        tool_ = tool;
    }
    virtual simics::ConfObjectRef tool() const {
        return tool_;
    }
    virtual bool enable() {
        enabled_ = true;
        return true;
    }
    virtual bool disable() {
        enabled_ = false;
        return true;
    }
    virtual void set_controller(simics::ConfObjectRef controller) {
        controller_ = controller;
    }
    virtual simics::ConfObjectRef controller() const {
        return controller_;
    }

    std::vector<std::string> functions_;
    bool enabled_;
    simics::ConfObjectRef tool_;
    simics::ConfObjectRef controller_;
};

BOOST_AUTO_TEST_CASE(TestInsert) {
    MockToolController controller;
    MockToolConnection c0, c1, c2;
    controller.insert(&c0, -1);
    controller.insert(&c1, -1);
    controller.insert(&c2, -1);

    std::vector<ToolConnectionInterface *> connections =
        controller.get_connections();

    BOOST_REQUIRE_EQUAL(connections.size(), 3);
    BOOST_CHECK(connections[0] == &c0);
    BOOST_CHECK(connections[1] == &c1);
    BOOST_CHECK(connections[2] == &c2);
}

BOOST_AUTO_TEST_CASE(TestRemove) {
    MockToolController controller;
    MockToolConnection c0, c1, c2;

    controller.insert(&c0, -1);
    controller.insert(&c1, -1);
    controller.insert(&c2, -1);

    controller.remove(&c1);

    // Test for invalid handle
    controller.remove(&c1);

    std::vector<ToolConnectionInterface *> connections =
        controller.get_connections();

    BOOST_REQUIRE_EQUAL(connections.size(), 2);
    BOOST_CHECK(connections[0] == &c0);
    BOOST_CHECK(connections[1] == &c2);

    controller.remove(&c0);

    connections = controller.get_connections();
    BOOST_REQUIRE_EQUAL(connections.size(), 1);
    BOOST_CHECK(connections[0] == &c2);

    controller.remove(&c2);

    BOOST_REQUIRE_EQUAL(controller.get_connections().size(), 0);
}

BOOST_AUTO_TEST_CASE(TestDispatch) {
    Environment e;
    MockInterface mock;
    e.register_interface("MockInterface", &mock);
    MockInterface::call_cnt_ = 0;
    MockInterface::true_cnt_ = 0;

    MockToolController controller;
    MockToolController *controller_p = NULL;
    MockToolConnection c0;
    MockToolConnection c1;
    MockToolConnection c2;

    conf_object_t conf;
    simics::ConfObjectRef obj(&conf);
    MockTool tool(obj);
    conf.instance_data = & tool;

    c0.set_tool(obj);
    c1.set_tool(obj);
    c2.set_tool(obj);
    std::vector<std::string> functions0 {"f0"};
    std::vector<std::string> functions1 {"f1"};
    c0.set_functions(&functions0);
    c1.set_functions(&functions1);
    c2.set_functions(&functions0);
    controller.insert(&c0, -1);
    controller.insert(&c1, -1);
    controller.insert(&c2, -1);

    // Test for NULL controller
    DISPATCH_TOOL_CHAIN(controller_p, MockInterface, f0, true);

    controller_p = &controller;

    DISPATCH_TOOL_CHAIN(controller_p, MockInterface, f0, true);

    BOOST_CHECK_EQUAL(MockInterface::call_cnt_, 2);
    BOOST_CHECK_EQUAL(MockInterface::true_cnt_, 2);

    MockInterface::call_cnt_ = 0;
    MockInterface::true_cnt_ = 0;

    DISPATCH_TOOL_CHAIN(controller_p, MockInterface, f0, false);

    BOOST_CHECK_EQUAL(MockInterface::call_cnt_, 2);
    BOOST_CHECK_EQUAL(MockInterface::true_cnt_, 0);

    MockInterface::call_cnt_ = 0;
    c0.disable();

    DISPATCH_TOOL_CHAIN(controller_p, MockInterface, f0, true);

    BOOST_CHECK_EQUAL(MockInterface::call_cnt_, 1);
    BOOST_CHECK_EQUAL(MockInterface::true_cnt_, 1);

    MockInterface::call_cnt_ = 0;
    MockInterface::true_cnt_ = 0;

    DISPATCH_TOOL_CHAIN(controller_p, MockInterface, f1);
    BOOST_CHECK_EQUAL(MockInterface::call_cnt_, 1);
}

BOOST_AUTO_TEST_CASE(TestToolControllerInit) {
    MockToolController controller;
    MockToolConnection c0;

    BOOST_CHECK_EQUAL(controller.tool_controller_init_cnt_, 0);

    controller.insert(&c0, 0);

    BOOST_CHECK_EQUAL(controller.tool_controller_init_cnt_, 1);

    controller.insert(&c0, 1);

    BOOST_CHECK_EQUAL(controller.tool_controller_init_cnt_, 1);
}

BOOST_AUTO_TEST_CASE(TestNodeListUpaded) {
    MockToolController controller;
    MockToolConnection c0;

    controller.insert(&c0, -1);

    BOOST_CHECK_EQUAL(controller.node_list_updated_cnt_, 1);
    BOOST_CHECK_EQUAL(controller.mock_state_,
                      ToolController::FIRST_ELEMENT_ADDED);

    controller.remove(NULL);

    BOOST_CHECK_EQUAL(controller.node_list_updated_cnt_, 2);
    BOOST_CHECK_EQUAL(controller.mock_state_,
                      ToolController::ONE_OR_MORE_ELEMENTS);

    controller.remove(&c0);

    BOOST_CHECK_EQUAL(controller.node_list_updated_cnt_, 3);
    BOOST_CHECK_EQUAL(controller.mock_state_, ToolController::EMPTY);

    controller.remove(NULL);

    BOOST_CHECK_EQUAL(controller.node_list_updated_cnt_, 4);
    BOOST_CHECK_EQUAL(controller.mock_state_, ToolController::EMPTY);
}

BOOST_AUTO_TEST_SUITE_END()
