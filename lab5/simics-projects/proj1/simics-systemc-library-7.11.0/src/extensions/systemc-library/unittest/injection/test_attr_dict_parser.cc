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

#include <string>

#include "environment.h"
#include "stubs.h"

using simics::systemc::injection::AttrDictParser;

BOOST_AUTO_TEST_SUITE(TestAttrDictParser)

class TestEnvironment : public Environment {
  public:
    TestEnvironment()
        : attr_(SIM_alloc_attr_dict(3)) {
        SIM_attr_dict_set_item(&attr_, 0, SIM_make_attr_string("a"),
                               SIM_make_attr_int64(1));
        SIM_attr_dict_set_item(&attr_, 1, SIM_make_attr_string("b"),
                               SIM_make_attr_uint64(2));
        SIM_attr_dict_set_item(&attr_, 2, SIM_make_attr_string("c"),
                               SIM_make_attr_int64(3));
    }
    void setAttrToValue0(const char* key, attr_value_t value) {
        SIM_attr_dict_set_item(&attr_, 0, SIM_make_attr_string(key), value);
    }
    void setAttrToValue1(const char* key, attr_value_t value) {
        SIM_attr_dict_set_item(&attr_, 1, SIM_make_attr_string(key), value);
    }
    void setAttrToValue2(const char* key, attr_value_t value) {
        SIM_attr_dict_set_item(&attr_, 2, SIM_make_attr_string(key), value);
    }

    ~TestEnvironment() {
        SIM_attr_free(&attr_);
    }

    Stubs stubs_;
    attr_value_t attr_;
};

struct MockParser : public AttrDictParser::ParserInterface {
    virtual std::string prefix() {
        return prefix_;
    }
    virtual std::set<std::string> keys() {
        std::set<std::string> k;
        k.insert("a");
        k.insert("b");
        k.insert("c");
        return k;
    }
    virtual bool parse(AttrDictParser *parser, const std::string &key,
                       attr_value_t *attr) {
        keys_.push_back(key);
        return true;
    }
    std::vector<std::string> keys_;
    std::string prefix_;
};

BOOST_FIXTURE_TEST_CASE(testParse, TestEnvironment) {
    AttrDictParser p(simulation_.simics_object(), &attr_);
    MockParser m;

    BOOST_CHECK_EQUAL(p.parse(&m), true);

    BOOST_CHECK_EQUAL(m.keys_.size(), 3);
    BOOST_CHECK_EQUAL(m.keys_[0], "a");
    BOOST_CHECK_EQUAL(m.keys_[1], "b");
    BOOST_CHECK_EQUAL(m.keys_[2], "c");
}
BOOST_FIXTURE_TEST_CASE(testParseWithPrefix, TestEnvironment) {
    setAttrToValue0("ns.a", SIM_make_attr_int64(1));
    setAttrToValue1("ns.b", SIM_make_attr_int64(2));
    setAttrToValue2("ns.c", SIM_make_attr_int64(3));

    AttrDictParser p(simulation_.simics_object(), &attr_);

    MockParser m;
    m.prefix_ = "ns.";

    BOOST_CHECK_EQUAL(p.parse(&m), true);

    BOOST_CHECK_EQUAL(m.keys_.size(), 3);
    BOOST_CHECK_EQUAL(m.keys_[0], "a");
    BOOST_CHECK_EQUAL(m.keys_[1], "b");
    BOOST_CHECK_EQUAL(m.keys_[2], "c");
}
BOOST_FIXTURE_TEST_CASE(testInitParser, TestEnvironment) {
    AttrDictParser p1(simulation_.simics_object());
    AttrDictParser p2 = p1.init(&attr_);
    MockParser m;

    BOOST_CHECK_EQUAL(p2.parse(&m), true);

    BOOST_CHECK_EQUAL(m.keys_.size(), 3);
    BOOST_CHECK_EQUAL(m.keys_[0], "a");
    BOOST_CHECK_EQUAL(m.keys_[1], "b");
    BOOST_CHECK_EQUAL(m.keys_[2], "c");
}

BOOST_FIXTURE_TEST_CASE(testReportInvalidAttrsNoError, TestEnvironment) {
    AttrDictParser p(simulation_.simics_object(), &attr_);
    MockParser m;

    BOOST_CHECK_EQUAL(p.parse(&m), true);

    BOOST_CHECK_EQUAL(p.reportInvalidAttrs(), true);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_ , 0);
}

struct MockParserOnlyB : MockParser {
    virtual bool parse(AttrDictParser *parser, const std::string &key,
                       attr_value_t *attr) {
        if (key == "b")
            return true;

        return false;
    }
};

BOOST_FIXTURE_TEST_CASE(testReportInvalidAttrsError, TestEnvironment) {
    AttrDictParser p(simulation_.simics_object(), &attr_);
    MockParserOnlyB m;

    BOOST_CHECK_EQUAL(p.parse(&m), false);

    BOOST_CHECK_EQUAL(p.reportInvalidAttrs(), false);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_ , 1);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_ ,
                      "Attribute key a, c is invalid");
}

BOOST_FIXTURE_TEST_CASE(testLookUp, TestEnvironment) {
    AttrDictParser p(simulation_.simics_object(), &attr_);

    int32_t a = 0;
    BOOST_CHECK_EQUAL(p.lookUp("a", &a), true);
    BOOST_CHECK_EQUAL(a, 1);

    uint32_t b = 0;
    BOOST_CHECK_EQUAL(p.lookUp("b", &b), true);
    BOOST_CHECK_EQUAL(b, 2);

    int32_t c = 0;
    BOOST_CHECK_EQUAL(p.lookUp("c", &c), true);
    BOOST_CHECK_EQUAL(c, 3);

    BOOST_CHECK_EQUAL(p.lookUp("d", &c), false);
    BOOST_CHECK_EQUAL(c, 3);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_ , 1);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_ ,
                      "Attribute key d not found");
}

struct MockParserMixedMode : public MockParser {
    virtual bool parse(AttrDictParser *parser, const std::string &key,
                       attr_value_t *attr) {
        if (key == "b")
            parser->lookUp("a", &a_);

        keys_.push_back(key);
        return true;
    }
    int32_t a_;
};

BOOST_FIXTURE_TEST_CASE(testLookUpInParse, TestEnvironment) {
    AttrDictParser p(simulation_.simics_object(), &attr_);
    MockParserMixedMode m;

    BOOST_CHECK_EQUAL(p.parse(&m), true);

    BOOST_CHECK_EQUAL(m.a_, 1);
    BOOST_CHECK_EQUAL(m.keys_.size(), 3);
    BOOST_CHECK_EQUAL(m.keys_[0], "a");
    BOOST_CHECK_EQUAL(m.keys_[1], "b");
    BOOST_CHECK_EQUAL(m.keys_[2], "c");
}

template<typename T>
struct MockListParser : public MockParser {
    virtual bool parse(AttrDictParser *parser, const std::string &key,
                       attr_value_t *attr) {
        if (key == "a") {
            if (parser->value(&list_))
                return true;
        }
        return MockParser::parse(parser, key, attr);
    }
    std::vector<T> list_;
};

BOOST_FIXTURE_TEST_CASE(testParseAttrList, TestEnvironment) {
    attr_value_t list = SIM_make_attr_list(2, SIM_make_attr_boolean(1),
                                           SIM_make_attr_string("test"));
    setAttrToValue0("a", list);
    AttrDictParser p(simulation_.simics_object(), &attr_);
    MockListParser<attr_value_t> m;

    BOOST_CHECK_EQUAL(p.parse(&m), true);

    BOOST_CHECK_EQUAL(m.list_.size(), 2);
    BOOST_CHECK(SIM_attr_boolean(m.list_[0]));
    BOOST_CHECK(SIM_attr_string(m.list_[1]));
}

BOOST_FIXTURE_TEST_CASE(testParseAttrListUInt8, TestEnvironment) {
    attr_value_t list = SIM_make_attr_list(2, SIM_make_attr_uint64(1),
                                           SIM_make_attr_uint64(2));
    setAttrToValue0("a", list);
    AttrDictParser p(simulation_.simics_object(), &attr_);
    MockListParser<u_int8_t> m;

    BOOST_CHECK_EQUAL(p.parse(&m), true);

    BOOST_CHECK_EQUAL(m.list_.size(), 2);
    BOOST_CHECK_EQUAL(m.list_[0], 1);
    BOOST_CHECK_EQUAL(m.list_[1], 2);
}

BOOST_AUTO_TEST_SUITE_END()
