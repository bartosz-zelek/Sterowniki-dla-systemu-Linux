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

#ifndef SIMICS_SYSTEMC_IFACE_TRANSACTION_SIMICS_ADAPTER_H
#define SIMICS_SYSTEMC_IFACE_TRANSACTION_SIMICS_ADAPTER_H

#include <simics/model-iface/transaction.h>
#include <simics/systemc/iface/transaction_interface.h>
#include <simics/systemc/iface/simics_adapter.h>

#include <string>
#include <vector>

namespace simics {
namespace systemc {
namespace iface {

/** Adapter for Simics signal interface. */
template<typename TBase, typename TInterface = TransactionInterface>
class TransactionSimicsAdapter : public SimicsAdapter<transaction_interface_t> {
  public:
    TransactionSimicsAdapter()
        : SimicsAdapter<transaction_interface_t>(TRANSACTION_INTERFACE,
                                                 init_iface()) {}

  protected:
    static ::exception_type_t issue(conf_object_t *obj, transaction_t *t,
                                    ::uint64 addr) {
        return static_cast<::exception_type_t>(
                adapter<TBase, TInterface>(obj)->issue(t, addr));
    }

  private:
    std::vector<std::string> description(conf_object_t *obj,
                                         DescriptionType type) {
        return descriptionBase<TBase, TInterface>(obj, type);
    }
    transaction_interface_t init_iface() {
        transaction_interface_t iface = {};
        iface.issue = issue;
        return iface;
    }
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
