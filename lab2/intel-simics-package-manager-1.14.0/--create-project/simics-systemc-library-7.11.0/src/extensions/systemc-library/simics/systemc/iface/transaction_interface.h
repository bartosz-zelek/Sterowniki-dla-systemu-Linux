// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2019 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_IFACE_TRANSACTION_INTERFACE_H
#define SIMICS_SYSTEMC_IFACE_TRANSACTION_INTERFACE_H

#include <simics/model-iface/transaction.h>

namespace simics {
namespace systemc {
namespace iface {

/** Simics transaction interface. */
class TransactionInterface {
  public:
    virtual exception_type_t issue(transaction_t *t, uint64 addr) = 0;
    virtual ~TransactionInterface() {}
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
