// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2020 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <boost/test/unit_test.hpp>
#include <systemc>

#include <cci_configuration>
#include <cci_utils/broker.h>

#include <simics/systemc/cci_configuration.h>

#include "environment.h"
#include "simcontext_wrapper.h"
#include "stubs.h"

class A {
  public:
    A() : a_(0) {}
    A(int a) : a_(a) {} // NOLINT
    bool operator== (const A& rhs) const {
        return a_ == rhs.a_;
    }

  private:
    int a_;
};

namespace cci {
template<>
struct cci_value_converter<A> {
    typedef A type;
    static bool pack(cci_value::reference dst, type const & src) {
        return true;
    }
    static bool unpack(type & dst, cci_value::const_reference src) { // NOLINT
        return true;
    }
};
}  // namespace cci

using cci::cci_originator;
using cci::cci_param;
using cci::cci_param_handle;
using cci::CCI_ABSOLUTE_NAME;

using std::string;

BOOST_AUTO_TEST_SUITE(TestCciConfiguration)

class TestEnv : public Environment {
  public:
    TestEnv() : broker_("GLOBAL_BROKER"),
                orig_("originator") {
        setTuple("name", "\"new_value\"");
        cci::cci_register_broker(&broker_);
    }
    ~TestEnv() {
        cci::cci_unregister_brokers();
        simics::systemc::CciConfiguration::cleanCache();
    }
    void setTuple(std::string key, std::string value) {
        attr_.clear();
        attr_.push_back(std::make_pair(key, value));
    }

    Stubs stubs_;
    unittest::SimContextWrapper wrapper_;
    std::vector<std::pair<std::string, std::string>> attr_;
    cci_utils::broker broker_;
    cci_originator orig_;
};

class TestModule : public sc_core::sc_module {
  public:
    SC_CTOR(TestModule) : param_("param", 0) {}
    cci_param<int> param_;
};

class TestModuleReject : public sc_core::sc_module {
  public:
    SC_CTOR(TestModuleReject) : param_("param", 0) {
        callback_ = param_.register_pre_write_callback(
                &TestModuleReject::pre_write_reject, this);
    }
    bool pre_write_reject(const cci::cci_param_write_event<int> & ev) {
        return false;
    }
    cci_param<int> param_;
    cci::cci_callback_untyped_handle callback_;
};

BOOST_FIXTURE_TEST_CASE(testParameterSetPreset, TestEnv) {
    simics::systemc::CciConfiguration cfg;
    cfg.setPresetValues(&attr_);

    cci_param<string> p("name", "value", "test parameter", CCI_ABSOLUTE_NAME,
                        orig_);

    BOOST_CHECK_EQUAL(p.get_value(), "new_value");
}

BOOST_FIXTURE_TEST_CASE(testParameterSetPresetValueMismatch, TestEnv) {
    setTuple("name", "A");
    simics::systemc::CciConfiguration cfg;
    try {
        cfg.setPresetValues(&attr_);
    } catch (const std::exception &e) {
        BOOST_CHECK_EQUAL(e.what(),
                          "Unable to set CCI parameter name to A.");
    }

    cci_param<int> p("name", 0, "test parameter", CCI_ABSOLUTE_NAME, orig_);
}

BOOST_FIXTURE_TEST_CASE(testParameterSetPresetLocked, TestEnv) {
    broker_.lock_preset_value("name");
    simics::systemc::CciConfiguration cfg;
    try {
        cfg.setPresetValues(&attr_);
    } catch (const std::exception &e) {
        BOOST_CHECK_EQUAL(e.what(),
                          "Unable to set CCI parameter name to \"new_value\".");
    }
    cci_param<string> p("name", "value", "test parameter",
                        CCI_ABSOLUTE_NAME, orig_);
    BOOST_CHECK_EQUAL(p.get_value(), "value");
}

BOOST_FIXTURE_TEST_CASE(testUnconsumedParameters, TestEnv) {
    setTuple("unconsumed", "\"A\"");
    simics::systemc::CciConfiguration cfg;

    cfg.setPresetValues(&attr_);

    cci_param<string> p("name", "value", "test parameter", CCI_ABSOLUTE_NAME,
                        orig_);
    conf_object_t conf_;
    simics::ConfObjectRef simics_obj(&conf_);
    cfg.logUnconsumedPresetValues(simics_obj);

    BOOST_CHECK_EQUAL(stubs_.instance_.SIM_log_info_,
                      "The SystemC CCI configuration contains unconsumed "
                      "preset values (unconsumed). "
                      "Preset values for static objects and objects created "
                      "outside 'virtual void elaborate()' are not supported.");
}

BOOST_FIXTURE_TEST_CASE(testParametersGet, TestEnv) {
    TestModule module1("module1");
    TestModule module2("module2");
    simics::systemc::CciConfiguration cfg;
    std::vector <cci_param_handle> parameters = cfg.getParameters(&module1);

    BOOST_REQUIRE_EQUAL(parameters.size(), 1);
    BOOST_CHECK_EQUAL(parameters[0].get_cci_value().to_json(), "0");

    parameters[0].set_cci_value(cci::cci_value::from_json("1"));

    BOOST_CHECK_EQUAL(module1.param_.get_cci_value().to_json(), "1");
}

BOOST_FIXTURE_TEST_CASE(testGlobalParametersGet, TestEnv) {
    cci_param<string> p("name", "value", "test parameter", CCI_ABSOLUTE_NAME,
                        orig_);

    simics::systemc::CciConfiguration cfg;
    std::vector <cci_param_handle> parameters = cfg.getParameters(NULL);

    BOOST_REQUIRE_EQUAL(parameters.size(), 1);
    BOOST_CHECK_EQUAL(parameters[0].get_cci_value().to_json(), "\"value\"");
}

BOOST_FIXTURE_TEST_CASE(testParameterGet, TestEnv) {
    TestModule module("module");
    simics::systemc::CciConfiguration cfg;
    cci_param_handle parameter = cfg.getParameter("module.param");

    BOOST_REQUIRE_EQUAL(parameter.is_valid(), true);
    BOOST_CHECK_EQUAL(parameter.get_cci_value().to_json(), "0");

    parameter.set_cci_value(cci::cci_value::from_json("1"));

    BOOST_CHECK_EQUAL(module.param_.get_cci_value().to_json(), "1");
}

BOOST_FIXTURE_TEST_CASE(testParameterGetWrongName, TestEnv) {
    TestModule module("module");
    simics::systemc::CciConfiguration cfg;
    cci_param_handle parameter = cfg.getParameter("module.param1");

    BOOST_CHECK_EQUAL(parameter.is_valid(), false);
}

BOOST_FIXTURE_TEST_CASE(testSetAttribute, TestEnv) {
    cci_param<int8> p1("int8", '0', "int8", CCI_ABSOLUTE_NAME, orig_);
    cci_param<uint8> p2("uint8", '0', "uint8", CCI_ABSOLUTE_NAME, orig_);

    cci_param<int16> p3("int16", 0, "int16", CCI_ABSOLUTE_NAME, orig_);
    cci_param<uint16> p4("uint16", 0, "uint16", CCI_ABSOLUTE_NAME, orig_);

    cci_param<int32> p5("int32", 0, "int32", CCI_ABSOLUTE_NAME, orig_);
    cci_param<uint32> p6("uint32", 0, "uint32", CCI_ABSOLUTE_NAME, orig_);

    cci_param<int64> p7("int64", 0, "int64", CCI_ABSOLUTE_NAME, orig_);
    cci_param<uint64> p8("uint64", 0, "uint64", CCI_ABSOLUTE_NAME, orig_);

    cci_param<long> p9("long", 0, "long", CCI_ABSOLUTE_NAME, orig_);  // NOLINT
    cci_param<unsigned long>  // NOLINT
        p10("ulong", 0, "ulong", CCI_ABSOLUTE_NAME, orig_);

    cci_param<float> p11("float", 0.0f, "float", CCI_ABSOLUTE_NAME, orig_);
    cci_param<double> p12("double", 0.0, "double", CCI_ABSOLUTE_NAME, orig_);

    cci_param<string> p13("string", "0", "string", CCI_ABSOLUTE_NAME, orig_);

    cci_param<sc_core::sc_time> p14("time", sc_core::sc_time::from_value(0),
                                    "time", CCI_ABSOLUTE_NAME, orig_);

    cci_param<char> p15("char", '0', "char", CCI_ABSOLUTE_NAME, orig_);

    simics::systemc::CciConfiguration cfg;

    BOOST_CHECK_EQUAL(cfg.setAttribute(cfg.getParameter("int8"),
                                       SIM_make_attr_string("1")), true);
    BOOST_CHECK_EQUAL(p1.get_value(), '1');
    BOOST_CHECK_EQUAL(cfg.setAttribute(cfg.getParameter("uint8"),
                                       SIM_make_attr_string("1")), true);
    BOOST_CHECK_EQUAL(p2.get_value(), '1');

    BOOST_CHECK_EQUAL(cfg.setAttribute(cfg.getParameter("int16"),
                                       SIM_make_attr_int64(1)), true);
    BOOST_CHECK_EQUAL(p3.get_value(), 1);
    BOOST_CHECK_EQUAL(cfg.setAttribute(cfg.getParameter("uint16"),
                                       SIM_make_attr_int64(1)), true);
    BOOST_CHECK_EQUAL(p4.get_value(), 1);

    BOOST_CHECK_EQUAL(cfg.setAttribute(cfg.getParameter("int32"),
                                       SIM_make_attr_int64(1)), true);
    BOOST_CHECK_EQUAL(p5.get_value(), 1);
    BOOST_CHECK_EQUAL(cfg.setAttribute(cfg.getParameter("uint32"),
                                       SIM_make_attr_int64(1)), true);
    BOOST_CHECK_EQUAL(p6.get_value(), 1);

    BOOST_CHECK_EQUAL(cfg.setAttribute(cfg.getParameter("int64"),
                                       SIM_make_attr_int64(1)), true);
    BOOST_CHECK_EQUAL(p7.get_value(), 1);
    BOOST_CHECK_EQUAL(cfg.setAttribute(cfg.getParameter("uint64"),
                                       SIM_make_attr_int64(1)), true);
    BOOST_CHECK_EQUAL(p8.get_value(), 1);

    BOOST_CHECK_EQUAL(cfg.setAttribute(cfg.getParameter("long"),
                                       SIM_make_attr_int64(1)), true);
    BOOST_CHECK_EQUAL(p9.get_value(), 1);
    BOOST_CHECK_EQUAL(cfg.setAttribute(cfg.getParameter("ulong"),
                                       SIM_make_attr_int64(1)), true);
    BOOST_CHECK_EQUAL(p10.get_value(), 1);

    BOOST_CHECK_EQUAL(cfg.setAttribute(cfg.getParameter("float"),
                                       SIM_make_attr_floating(1.0f)), true);
    BOOST_CHECK_EQUAL(p11.get_value(), 1.0f);

    BOOST_CHECK_EQUAL(cfg.setAttribute(cfg.getParameter("double"),
                                       SIM_make_attr_floating(1.0)), true);
    BOOST_CHECK_EQUAL(p12.get_value(), 1.0);

    BOOST_CHECK_EQUAL(cfg.setAttribute(cfg.getParameter("string"),
                                       SIM_make_attr_string("1")), true);
    BOOST_CHECK_EQUAL(p13.get_value(), "1");

    BOOST_CHECK_EQUAL(cfg.setAttribute(cfg.getParameter("time"),
                                       SIM_make_attr_uint64(1)), true);
    BOOST_CHECK_EQUAL(p14.get_value(), sc_core::sc_time::from_value(1));

    BOOST_CHECK_EQUAL(cfg.setAttribute(cfg.getParameter("char"),
                                       SIM_make_attr_string("1")), true);
    BOOST_CHECK_EQUAL(cfg.setAttribute(cfg.getParameter("char"),
                                       SIM_make_attr_string("00")), false);
    BOOST_CHECK_EQUAL(p15.get_value(), '1');
}

BOOST_FIXTURE_TEST_CASE(testSetAttributeLocked, TestEnv) {
    cci_param<int8> p1("int8", '0', "int8", CCI_ABSOLUTE_NAME, orig_);
    p1.lock();

    simics::systemc::CciConfiguration cfg;

    BOOST_CHECK_EQUAL(cfg.setAttribute(cfg.getParameter("int8"),
                                       SIM_make_attr_string("1")), false);
    BOOST_CHECK_EQUAL(p1.get_value(), '0');
}

BOOST_FIXTURE_TEST_CASE(testSetAttributeImmutable, TestEnv) {
    cci_param<int8, cci::CCI_IMMUTABLE_PARAM> p1("int8", '0', "int8",
                                                 CCI_ABSOLUTE_NAME, orig_);

    simics::systemc::CciConfiguration cfg;

    BOOST_CHECK_EQUAL(cfg.setAttribute(cfg.getParameter("int8"),
                                       SIM_make_attr_string("1")), false);
    BOOST_CHECK_EQUAL(p1.get_value(), '0');
}

BOOST_FIXTURE_TEST_CASE(testSetAttributeReject, TestEnv) {
    TestModuleReject module("module");

    simics::systemc::CciConfiguration cfg;

    BOOST_CHECK_EQUAL(cfg.setAttribute(cfg.getParameter("module.param"),
                                       SIM_make_attr_int64(1)), false);
    BOOST_CHECK_EQUAL(module.param_.get_value(), 0);
}

BOOST_FIXTURE_TEST_CASE(testGetAttribute, TestEnv) {
    cci_param<int8> p1("int8", '0', "int8", CCI_ABSOLUTE_NAME, orig_);
    cci_param<uint8> p2("uint8", '0', "uint8", CCI_ABSOLUTE_NAME, orig_);

    cci_param<int16> p3("int16", 0, "int16", CCI_ABSOLUTE_NAME, orig_);
    cci_param<uint16> p4("uint16", 0, "uint16", CCI_ABSOLUTE_NAME, orig_);

    cci_param<int32> p5("int32", 0, "int32", CCI_ABSOLUTE_NAME, orig_);
    cci_param<uint32> p6("uint32", 0, "uint32", CCI_ABSOLUTE_NAME, orig_);

    cci_param<int64> p7("int64", 0, "int64", CCI_ABSOLUTE_NAME, orig_);
    cci_param<uint64> p8("uint64", 0, "uint64", CCI_ABSOLUTE_NAME, orig_);

    cci_param<long> p9("long", 0, "long", CCI_ABSOLUTE_NAME, orig_);  // NOLINT
    cci_param<unsigned long>  // NOLINT
        p10("ulong", 0, "ulong", CCI_ABSOLUTE_NAME, orig_);

    cci_param<float> p11("float", 0.0f, "float", CCI_ABSOLUTE_NAME, orig_);
    cci_param<double> p12("double", 0.0, "double", CCI_ABSOLUTE_NAME, orig_);

    cci_param<string> p13("string", "0", "string", CCI_ABSOLUTE_NAME, orig_);

    cci_param<sc_core::sc_time> p14("time", sc_core::sc_time::from_value(0),
                                    "time", CCI_ABSOLUTE_NAME, orig_);

    cci_param<char> p15("char", '0', "char", CCI_ABSOLUTE_NAME, orig_);

    simics::systemc::CciConfiguration cfg;

    cci_param_handle p = cfg.getParameter("int8");
    BOOST_CHECK(SIM_attr_is_string(cfg.getAttribute(p)));
    BOOST_CHECK_EQUAL(SIM_attr_string(cfg.getAttribute(p)), "0");

    p = cfg.getParameter("uint8");
    BOOST_CHECK(SIM_attr_is_string(cfg.getAttribute(p)));
    BOOST_CHECK_EQUAL(SIM_attr_string(cfg.getAttribute(p)), "0");

    p = cfg.getParameter("int16");
    BOOST_CHECK(SIM_attr_is_integer(cfg.getAttribute(p)));
    BOOST_CHECK_EQUAL(SIM_attr_integer(cfg.getAttribute(p)), 0);

    p = cfg.getParameter("uint16");
    BOOST_CHECK(SIM_attr_is_integer(cfg.getAttribute(p)));
    BOOST_CHECK_EQUAL(SIM_attr_integer(cfg.getAttribute(p)), 0);

    p = cfg.getParameter("int32");
    BOOST_CHECK(SIM_attr_is_integer(cfg.getAttribute(p)));
    BOOST_CHECK_EQUAL(SIM_attr_integer(cfg.getAttribute(p)), 0);

    p = cfg.getParameter("uint32");
    BOOST_CHECK(SIM_attr_is_integer(cfg.getAttribute(p)));
    BOOST_CHECK_EQUAL(SIM_attr_integer(cfg.getAttribute(p)), 0);

    p = cfg.getParameter("int64");
    BOOST_CHECK(SIM_attr_is_integer(cfg.getAttribute(p)));
    BOOST_CHECK_EQUAL(SIM_attr_integer(cfg.getAttribute(p)), 0);

    p = cfg.getParameter("uint64");
    BOOST_CHECK(SIM_attr_is_integer(cfg.getAttribute(p)));
    BOOST_CHECK_EQUAL(SIM_attr_integer(cfg.getAttribute(p)), 0);

    p = cfg.getParameter("long");
    BOOST_CHECK(SIM_attr_is_integer(cfg.getAttribute(p)));
    BOOST_CHECK_EQUAL(SIM_attr_integer(cfg.getAttribute(p)), 0);

    p = cfg.getParameter("ulong");
    BOOST_CHECK(SIM_attr_is_integer(cfg.getAttribute(p)));
    BOOST_CHECK_EQUAL(SIM_attr_integer(cfg.getAttribute(p)), 0);

    p = cfg.getParameter("float");
    BOOST_CHECK(SIM_attr_is_floating(cfg.getAttribute(p)));
    BOOST_CHECK_EQUAL(SIM_attr_floating(cfg.getAttribute(p)), 0.0f);

    p = cfg.getParameter("double");
    BOOST_CHECK(SIM_attr_is_floating(cfg.getAttribute(p)));
    BOOST_CHECK_EQUAL(SIM_attr_floating(cfg.getAttribute(p)), 0.0);

    p = cfg.getParameter("string");
    BOOST_CHECK(SIM_attr_is_string(cfg.getAttribute(p)));
    BOOST_CHECK_EQUAL(SIM_attr_string(cfg.getAttribute(p)), "0");

    p = cfg.getParameter("time");
    BOOST_CHECK(SIM_attr_is_integer(cfg.getAttribute(p)));
    BOOST_CHECK_EQUAL(SIM_attr_integer(cfg.getAttribute(p)), 0);

    p = cfg.getParameter("char");
    BOOST_CHECK(SIM_attr_is_string(cfg.getAttribute(p)));
    BOOST_CHECK_EQUAL(SIM_attr_string(cfg.getAttribute(p)), "0");
}

BOOST_FIXTURE_TEST_CASE(testSimicsTypes, TestEnv) {
    cci_param<int8> p1("int8", '0', "int8", CCI_ABSOLUTE_NAME, orig_);
    cci_param<uint8> p2("uint8", '0', "uint8", CCI_ABSOLUTE_NAME, orig_);

    cci_param<int16> p3("int16", 0, "int16", CCI_ABSOLUTE_NAME, orig_);
    cci_param<uint16> p4("uint16", 0, "uint16", CCI_ABSOLUTE_NAME, orig_);

    cci_param<int32> p5("int32", 0, "int32", CCI_ABSOLUTE_NAME, orig_);
    cci_param<uint32> p6("uint32", 0, "uint32", CCI_ABSOLUTE_NAME, orig_);

    cci_param<int64> p7("int64", 0, "int64", CCI_ABSOLUTE_NAME, orig_);
    cci_param<uint64> p8("uint64", 0, "uint64", CCI_ABSOLUTE_NAME, orig_);

    cci_param<long> p9("long", 0, "long", CCI_ABSOLUTE_NAME, orig_);  // NOLINT
    cci_param<unsigned long>  // NOLINT
        p10("ulong", 0, "ulong", CCI_ABSOLUTE_NAME, orig_);

    cci_param<string> p11("string", "0", "string", CCI_ABSOLUTE_NAME, orig_);
    cci_param<float> p12("float", 1.0f, "float", CCI_ABSOLUTE_NAME, orig_);
    cci_param<double> p13("double", 1.0, "double", CCI_ABSOLUTE_NAME, orig_);
    cci_param<char> p14("char", '0', "char", CCI_ABSOLUTE_NAME, orig_);
    cci_param<sc_core::sc_time> p15("time", sc_core::sc_time::from_value(0),
                                   "time", CCI_ABSOLUTE_NAME, orig_);
    cci_param<A> p16("A", 0, "A", CCI_ABSOLUTE_NAME, orig_);

    simics::systemc::CciConfiguration cfg;
    BOOST_CHECK_EQUAL(cfg.simicsType(cfg.getParameter("int8")), "s");
    BOOST_CHECK_EQUAL(cfg.simicsType(cfg.getParameter("uint8")), "s");
    BOOST_CHECK_EQUAL(cfg.simicsType(cfg.getParameter("int16")), "i");
    BOOST_CHECK_EQUAL(cfg.simicsType(cfg.getParameter("uint16")), "i");
    BOOST_CHECK_EQUAL(cfg.simicsType(cfg.getParameter("int32")), "i");
    BOOST_CHECK_EQUAL(cfg.simicsType(cfg.getParameter("uint32")), "i");
    BOOST_CHECK_EQUAL(cfg.simicsType(cfg.getParameter("int64")), "i");
    BOOST_CHECK_EQUAL(cfg.simicsType(cfg.getParameter("uint64")), "i");
    BOOST_CHECK_EQUAL(cfg.simicsType(cfg.getParameter("long")), "i");
    BOOST_CHECK_EQUAL(cfg.simicsType(cfg.getParameter("ulong")), "i");
    BOOST_CHECK_EQUAL(cfg.simicsType(cfg.getParameter("string")), "s");
    BOOST_CHECK_EQUAL(cfg.simicsType(cfg.getParameter("float")), "f");
    BOOST_CHECK_EQUAL(cfg.simicsType(cfg.getParameter("double")), "f");
    BOOST_CHECK_EQUAL(cfg.simicsType(cfg.getParameter("char")), "s");
    BOOST_CHECK_EQUAL(cfg.simicsType(cfg.getParameter("time")), "i");
    BOOST_CHECK_EQUAL(cfg.simicsType(cfg.getParameter("A")),
                      static_cast<const char*>(0));
}

BOOST_FIXTURE_TEST_CASE(testSimicsName, TestEnv) {
    TestModule module("module");
    simics::systemc::CciConfiguration cfg;
    cci_param_handle parameter = cfg.getParameter("module.param");

    BOOST_CHECK_EQUAL(cfg.simicsName(parameter), "param");
}

BOOST_AUTO_TEST_SUITE_END()
