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

#include <simics/systemc/registry.h>

using simics::systemc::Registry;
using simics::systemc::Registrant;

BOOST_AUTO_TEST_SUITE(TestRegistry)

class MockInterface {
  public:
    virtual ~MockInterface() {}
    virtual void called() = 0;
};

class Mock : public Registrant<MockInterface> {
  public:
    Mock() : called_(0) {}
    void called() {
        ++called_;
    }
    int called_;
};

class MockWithRemove : public Registrant<MockInterface> {
  public:
    MockWithRemove() : called_(0) {}
    void called() {
        ++called_;
        Registry<MockInterface> *registry = Registry<MockInterface>::instance();
        registry->remove(this);
    }
    int called_;
};

struct InvokeCalledOnceFunctor {
    InvokeCalledOnceFunctor() : mock_(NULL) {}
    bool operator()(MockInterface* mock) {
        mock_ = mock;
        mock->called();
        return true;
    }
    MockInterface* mock_;
};

struct InvokeCalledTwiceFunctor {
    bool operator()(MockInterface* mock) {
        if (mocks_.size() >= 2)
            return true;

        mock->called();
        mocks_.push_back(mock);
        return false;
    }
    std::vector<MockInterface*> mocks_;
};

struct InvokeCalledAllFunctor {
    bool operator()(MockInterface* mock) {
        mock->called();
        return false;
    }
};

BOOST_AUTO_TEST_CASE(testRegistryIterateOnce) {
    Registry<MockInterface> *registry = Registry<MockInterface>::instance();
    InvokeCalledOnceFunctor f1;

    BOOST_CHECK_EQUAL(registry->iterate(&f1), false);

    Mock mock1;

    BOOST_CHECK_EQUAL(mock1.called_ , 0);
    BOOST_CHECK_EQUAL(registry->iterate(&f1), true);
    BOOST_CHECK_EQUAL(mock1.called_ , 1);
    BOOST_CHECK_EQUAL(f1.mock_, &mock1);
}

BOOST_AUTO_TEST_CASE(testCopiedRegistrant) {
    Registry<MockInterface> *registry = Registry<MockInterface>::instance();
    InvokeCalledAllFunctor f1;

    registry->iterate(&f1);

    std::vector<Mock> mocks;
    mocks.resize(1);
    {
        Mock mock2 = mocks.back();

        registry->iterate(&f1);
        BOOST_CHECK_EQUAL(mock2.called_ , 1);
        BOOST_CHECK_EQUAL(mocks.back().called_ , 1);
    }

    registry->iterate(&f1);
    BOOST_CHECK_EQUAL(mocks.back().called_ , 2);
}

BOOST_AUTO_TEST_CASE(testRegistryUnregister) {
    Registry<MockInterface> *registry = Registry<MockInterface>::instance();
    InvokeCalledOnceFunctor f1;

    BOOST_CHECK_EQUAL(registry->iterate(&f1), false);

    {
        Mock mock1;
    }

    BOOST_CHECK_EQUAL(registry->iterate(&f1), false);
}

BOOST_AUTO_TEST_CASE(testRegistryReverseIterateOnce) {
    Registry<MockInterface> *registry = Registry<MockInterface>::instance();
    InvokeCalledOnceFunctor f1;

    BOOST_CHECK_EQUAL(registry->reverseIterate(&f1), false);

    Mock mock1;

    BOOST_CHECK_EQUAL(mock1.called_ , 0);
    BOOST_CHECK_EQUAL(registry->reverseIterate(&f1), true);
    BOOST_CHECK_EQUAL(mock1.called_ , 1);
    BOOST_CHECK_EQUAL(f1.mock_, &mock1);
}

BOOST_AUTO_TEST_CASE(testRegistryIterateTwice) {
    Registry<MockInterface> *registry = Registry<MockInterface>::instance();
    InvokeCalledTwiceFunctor f2;

    Mock mock1;
    Mock mock2;
    Mock mock3;

    BOOST_CHECK_EQUAL(registry->iterate(&f2), true);
    BOOST_CHECK_EQUAL(mock1.called_ , 1);
    BOOST_CHECK_EQUAL(mock2.called_ , 1);
    BOOST_CHECK_EQUAL(mock3.called_ , 0);
    BOOST_REQUIRE_EQUAL(f2.mocks_.size(), 2);
    BOOST_CHECK_EQUAL(f2.mocks_[0], &mock1);
    BOOST_CHECK_EQUAL(f2.mocks_[1], &mock2);
}

BOOST_AUTO_TEST_CASE(testRegistryReverseIterateTwice) {
    Registry<MockInterface> *registry = Registry<MockInterface>::instance();
    InvokeCalledTwiceFunctor f2;

    Mock mock1;
    Mock mock2;
    Mock mock3;

    BOOST_CHECK_EQUAL(registry->reverseIterate(&f2), true);
    BOOST_CHECK_EQUAL(mock1.called_ , 0);
    BOOST_CHECK_EQUAL(mock2.called_ , 1);
    BOOST_CHECK_EQUAL(mock3.called_ , 1);
    BOOST_REQUIRE_EQUAL(f2.mocks_.size(), 2);
    BOOST_CHECK_EQUAL(f2.mocks_[0], &mock3);
    BOOST_CHECK_EQUAL(f2.mocks_[1], &mock2);
}

BOOST_AUTO_TEST_CASE(testRegistryIterateWithRemove) {
    Registry<MockInterface> *registry = Registry<MockInterface>::instance();
    InvokeCalledAllFunctor f;

    MockWithRemove mock1;
    MockWithRemove mock2;
    MockWithRemove mock3;

    BOOST_CHECK_EQUAL(registry->iterate(&f), false);
    BOOST_CHECK_EQUAL(mock1.called_ , 1);
    BOOST_CHECK_EQUAL(mock2.called_ , 1);
    BOOST_CHECK_EQUAL(mock3.called_ , 1);
}

BOOST_AUTO_TEST_CASE(testRegistryReverseIterateWithRemove) {
    Registry<MockInterface> *registry = Registry<MockInterface>::instance();
    InvokeCalledAllFunctor f;

    MockWithRemove mock1;
    MockWithRemove mock2;
    MockWithRemove mock3;

    BOOST_CHECK_EQUAL(registry->reverseIterate(&f), false);
    BOOST_CHECK_EQUAL(mock1.called_ , 1);
    BOOST_CHECK_EQUAL(mock2.called_ , 1);
    BOOST_CHECK_EQUAL(mock3.called_ , 1);
}

BOOST_AUTO_TEST_SUITE_END()
