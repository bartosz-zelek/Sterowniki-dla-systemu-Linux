// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2017 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <boost/test/unit_test.hpp>

#include <simics/systemc/injection/attr_dict_parser.h>
#include <simics/systemc/injection/inject_gp.h>
#include <simics/systemc/injection/inject_registry.h>

#include <string>

#include "environment.h"
#include "stubs.h"

using simics::systemc::injection::InjectGp;
using simics::systemc::injection::InjectRegistry;
using simics::systemc::injection::AttrDictParser;

class MyExtension : public tlm::tlm_extension<MyExtension> {
  public:
    MyExtension() : value_(0) {}
    virtual tlm::tlm_extension_base *clone() const {
        return new MyExtension(*this);
    }
    virtual void copy_from(tlm::tlm_extension_base const &ext) {
        *this = static_cast<const MyExtension &>(ext);
    }
    void set_extension_value(uint64 value) {
        value_ = value;
    }
    uint64 get_extension_value() {
        return value_;
    }

  private:
    uint64 value_;
};

template <typename TPAYLOAD>
class MyExtensionInjector
    : public simics::systemc::injection::InjectBase<TPAYLOAD> {
  public:
    MyExtensionInjector() : gp_(NULL), data_(NULL) {}
    virtual std::string prefix() {
        return "MyExtension.";
    }
    virtual std::set<std::string> keys() {
        std::set<std::string> keys;
        keys.insert("extension-attribute-1");
        return keys;
    }

    virtual bool setValue(AttrDictParser *parser, const std::string &key,
                          attr_value_t *attr, TPAYLOAD *gp) {
        if (key == "extension-attribute-1") {
            MyExtension *extension = new MyExtension;
            gp->set_extension(extension);

            uint64_t value;
            if (!parser->value(&value))
                return false;

            extension->set_extension_value(value);
            return true;
        } else if (key == "extension-attribute-2") {
            if (!parser->value(&data_))
                return false;

            return true;
        }

        return false;
    }
    virtual void released(TPAYLOAD *gp) {
        gp_ = gp;
    }
    TPAYLOAD *gp_;
    const unsigned char *data_;
};

BOOST_AUTO_TEST_SUITE(TestInjectRegistry)

class TestEnvironment : public Environment {
  public:
    TestEnvironment() {
        attr_ = SIM_alloc_attr_dict(2);
        payload_ = NULL;
        // Necessary until bug 24864 is resolved.
        setAttrToValue1("", SIM_make_attr_uint64(0));
    }
    ~TestEnvironment() {
        if (payload_)
            payload_->release();
        SIM_attr_free(&attr_);
    }
    void setAttrToValue0(const char* key, attr_value_t value) {
        SIM_attr_dict_set_item(&attr_, 0, SIM_make_attr_string(key), value);
    }
    void setAttrToValue1(const char* key, attr_value_t value) {
        SIM_attr_dict_set_item(&attr_, 1, SIM_make_attr_string(key), value);
    }
    void testGp(const char* key, attr_value_t value, bool expected) {
        SIM_attr_dict_set_item(&attr_, 0, SIM_make_attr_string(key), value);
        payload_ = registry_.attrToPayload(simulation_.simics_object(), &attr_);

        BOOST_CHECK_EQUAL(payload_ != NULL, expected);
    }
    Stubs stubs_;
    attr_value_t attr_;
    InjectGp<tlm::tlm_generic_payload> inject_;
    tlm::tlm_generic_payload *payload_;
    InjectRegistry<tlm::tlm_generic_payload> registry_;
};

BOOST_FIXTURE_TEST_CASE(testGpAllAttributesHandled, TestEnvironment) {
    setAttrToValue1("gp.command", SIM_make_attr_uint64(tlm::TLM_WRITE_COMMAND));
    testGp("gp.address", SIM_make_attr_uint64(1234), true);

    BOOST_CHECK_EQUAL(payload_->get_command(), tlm::TLM_WRITE_COMMAND);
    BOOST_CHECK_EQUAL(payload_->get_address(), 1234);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 0);
}

BOOST_FIXTURE_TEST_CASE(testGpAttributeValueInvalid, TestEnvironment) {
    testGp("gp.command", SIM_make_attr_int64(-1), false);

    BOOST_CHECK(Stubs::instance_.SIM_log_error_count_ > 0);
}

BOOST_FIXTURE_TEST_CASE(testGpUnhandledAttribute, TestEnvironment) {
    setAttrToValue1("gp.address", SIM_make_attr_uint64(1234));
    testGp("gp.unknown-attribute", SIM_make_attr_uint64(1), false);

    BOOST_CHECK(Stubs::instance_.SIM_log_error_count_ > 0);
}

BOOST_FIXTURE_TEST_CASE(testGpWithExtensionAttribute, TestEnvironment) {
    MyExtensionInjector <tlm::tlm_generic_payload> extension_injector;
    setAttrToValue1("gp.address", SIM_make_attr_uint64(1));
    testGp("MyExtension.extension-attribute-1", SIM_make_attr_uint64(9876),
           true);

    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 0);

    MyExtension *extension;
    payload_->get_extension(extension);

    BOOST_REQUIRE(extension);
    BOOST_CHECK_EQUAL(extension->get_extension_value(), 9876);
}

BOOST_FIXTURE_TEST_CASE(testGpDataRelease, TestEnvironment) {
    MyExtensionInjector <tlm::tlm_generic_payload> extension_injector;
    setAttrToValue1("gp.address", SIM_make_attr_uint64(1));
    testGp("MyExtension.extension-attribute-1", SIM_make_attr_uint64(2345),
           true);

    BOOST_CHECK(extension_injector.gp_ == 0);

    payload_->release();

    BOOST_CHECK_EQUAL(extension_injector.gp_, payload_);

    payload_ = NULL;
}

BOOST_FIXTURE_TEST_CASE(testAttrDataValidUntilGpReleased, TestEnvironment) {
    MyExtensionInjector <tlm::tlm_generic_payload> extension_injector;
    testGp("MyExtension.extension-attribute-2", SIM_make_attr_data(4, "abc"),
           true);

    setAttrToValue0("MyExtension.extension-attribute-2",
                    SIM_make_attr_data(4, "cba"));
    BOOST_CHECK_EQUAL(reinterpret_cast<const char*>(extension_injector.data_),
                      "abc");
}

BOOST_AUTO_TEST_SUITE_END()
