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

#include <simics/systemc/iface/transaction_extension.h>
#include <simics/systemc/iface/transaction_pool.h>

#include <unittest/mock/tlm/mock_extension.h>

using simics::systemc::iface::TransactionPool;
using simics::systemc::iface::TransactionStatus;
using simics::systemc::iface::Transaction;

BOOST_AUTO_TEST_SUITE(TestTransactionPool)

void check_payload_default_values(tlm::tlm_generic_payload *p) {
    BOOST_CHECK_EQUAL(p->get_address(), 0U);
    BOOST_CHECK_EQUAL(p->get_command(), tlm::TLM_IGNORE_COMMAND);
    BOOST_CHECK(p->get_data_ptr() == NULL);
    BOOST_CHECK_EQUAL(p->get_data_length(), 0U);
    BOOST_CHECK_EQUAL(p->get_response_status(), tlm::TLM_INCOMPLETE_RESPONSE);
    BOOST_CHECK_EQUAL(p->is_dmi_allowed(), false);
    BOOST_CHECK(p->get_byte_enable_ptr() == NULL);
    BOOST_CHECK_EQUAL(p->get_byte_enable_length(), 0U);
    BOOST_CHECK_EQUAL(p->get_streaming_width(), 0U);
    BOOST_CHECK_EQUAL(p->get_gp_option(), tlm::TLM_MIN_PAYLOAD);
    BOOST_CHECK_EQUAL(p->has_mm(), true);
}

BOOST_AUTO_TEST_CASE(testNewAllocatedTransaction) {
    TransactionPool pool;
    Transaction t1 = pool.acquire();
    Transaction t2 = pool.acquire();

    BOOST_CHECK_NE(t1.payload(), t2.payload());
    check_payload_default_values(t1);
    check_payload_default_values(t2);
}

BOOST_AUTO_TEST_CASE(testReuseTransaction) {
    TransactionPool pool;
    unittest::MockExtension extension;
    {
        Transaction t1 = pool.acquire();
        BOOST_CHECK_EQUAL(pool.PoolSize(), 0);
        BOOST_CHECK_EQUAL(pool.active_cnt(), 1);
        t1->set_extension(&extension);
    }
    {
        Transaction t2 = pool.acquire();
        BOOST_CHECK_EQUAL(pool.PoolSize(), 0);
        BOOST_CHECK_EQUAL(pool.active_cnt(), 1);
        BOOST_CHECK_EQUAL(t2->get_extension<unittest::MockExtension>(),
                          &extension);
        t2->clear_extension<unittest::MockExtension>();
    }
    BOOST_CHECK_EQUAL(pool.PoolSize(), 1);
    BOOST_CHECK_EQUAL(pool.active_cnt(), 0);
}

BOOST_AUTO_TEST_CASE(testTransactionNullAssignment) {
    TransactionPool pool;
    {
        Transaction t1 = pool.acquire();
        BOOST_CHECK_EQUAL(pool.PoolSize(), 0);
        BOOST_CHECK_EQUAL(pool.active_cnt(), 1);

        t1 = Transaction(NULL);
        BOOST_CHECK_EQUAL(pool.PoolSize(), 1);
        BOOST_CHECK_EQUAL(pool.active_cnt(), 0);
    }
    BOOST_CHECK_EQUAL(pool.PoolSize(), 1);
    BOOST_CHECK_EQUAL(pool.active_cnt(), 0);
}

BOOST_AUTO_TEST_CASE(testTransactionAssignment) {
    TransactionPool pool;
    {
        Transaction t1 = pool.acquire();
        BOOST_CHECK_EQUAL(pool.PoolSize(), 0);
        BOOST_CHECK_EQUAL(pool.active_cnt(), 1);

        Transaction t2 = pool.acquire();
        BOOST_CHECK_EQUAL(pool.PoolSize(), 0);
        BOOST_CHECK_EQUAL(pool.active_cnt(), 2);

        t1 = t2;
        BOOST_CHECK_EQUAL(pool.PoolSize(), 1);
        BOOST_CHECK_EQUAL(pool.active_cnt(), 1);

        t1 = Transaction(NULL);
        BOOST_CHECK_EQUAL(pool.PoolSize(), 1);
        BOOST_CHECK_EQUAL(pool.active_cnt(), 1);

        t1 = t2;
        BOOST_CHECK_EQUAL(pool.PoolSize(), 1);
        BOOST_CHECK_EQUAL(pool.active_cnt(), 1);

        t2 = Transaction(NULL);
        BOOST_CHECK_EQUAL(pool.PoolSize(), 1);
        BOOST_CHECK_EQUAL(pool.active_cnt(), 1);

        t1 = Transaction(NULL);
        BOOST_CHECK_EQUAL(pool.PoolSize(), 2);
        BOOST_CHECK_EQUAL(pool.active_cnt(), 0);
    }
    BOOST_CHECK_EQUAL(pool.PoolSize(), 2);
    BOOST_CHECK_EQUAL(pool.active_cnt(), 0);
}

BOOST_AUTO_TEST_CASE(testTransactionSelfAssignment) {
    TransactionPool pool;
    {
        Transaction t1 = pool.acquire();
        BOOST_CHECK_EQUAL(pool.PoolSize(), 0);
        BOOST_CHECK_EQUAL(pool.active_cnt(), 1);

        Transaction *t2 = &t1;
        t1 = *t2;
        BOOST_CHECK_EQUAL(pool.PoolSize(), 0);
        BOOST_CHECK_EQUAL(pool.active_cnt(), 1);
    }
    BOOST_CHECK_EQUAL(pool.PoolSize(), 1);
    BOOST_CHECK_EQUAL(pool.active_cnt(), 0);
}

BOOST_AUTO_TEST_CASE(testTransactionCopy) {
    TransactionPool pool;
    {
        Transaction t1 = pool.acquire();
        BOOST_CHECK_EQUAL(pool.PoolSize(), 0);
        BOOST_CHECK_EQUAL(pool.active_cnt(), 1);

        Transaction t2(t1);
        BOOST_CHECK_EQUAL(pool.PoolSize(), 0);
        BOOST_CHECK_EQUAL(pool.active_cnt(), 1);

        t1 = Transaction(NULL);
        BOOST_CHECK_EQUAL(pool.PoolSize(), 0);
        BOOST_CHECK_EQUAL(pool.active_cnt(), 1);
    }
    BOOST_CHECK_EQUAL(pool.PoolSize(), 1);
    BOOST_CHECK_EQUAL(pool.active_cnt(), 0);
}

BOOST_AUTO_TEST_CASE(testTransactionDefaultConstructor) {
    TransactionPool pool;
    Transaction t1 = pool.acquire();
    BOOST_CHECK_EQUAL(pool.PoolSize(), 0);
    BOOST_CHECK_EQUAL(pool.active_cnt(), 1);

    tlm::tlm_generic_payload *p = t1;
    Transaction t2;
    t2 = Transaction(p);

    t1 = Transaction(NULL);
    BOOST_CHECK_EQUAL(pool.PoolSize(), 0);
    BOOST_CHECK_EQUAL(pool.active_cnt(), 1);

    t2 = Transaction(NULL);
    BOOST_CHECK_EQUAL(pool.PoolSize(), 1);
    BOOST_CHECK_EQUAL(pool.active_cnt(), 0);
}

BOOST_AUTO_TEST_CASE(testTransactionFreeAfterPoolDeleted) {
    Transaction t1;
    {
        TransactionPool pool;
        t1 = pool.acquire();
    }
    check_payload_default_values(t1);
}

BOOST_AUTO_TEST_SUITE_END()
