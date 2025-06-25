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

#ifndef SIMICS_SYSTEMC_SIMICS2TLM_GASKET_ADAPTER_H
#define SIMICS_SYSTEMC_SIMICS2TLM_GASKET_ADAPTER_H

#include <simics/systemc/description_interface.h>
#include <simics/systemc/simics2tlm/gasket_interface.h>
#include <simics/systemc/simics2tlm/gasket_owner.h>
#include <simics/systemc/simics2tlm/multi_gasket_owner.h>

#include <set>
#include <string>
#include <vector>

#define GASKET_ADAPTER_PORT_CLASS(name, gasket_adapter)                      \
class name                                                                   \
    : public simics::systemc::simics2tlm::gasket_adapter,                    \
      public simics::systemc::simics2tlm::GasketAdapter<name> {              \
  public:                                                                    \
    using gasket_adapter::gasket_adapter;                                    \
    virtual ~name() = default;                                               \
    simics::systemc::simics2tlm::GasketOwner *gasket_owner() const override {\
        return gasket_adapter::gasket_owner();                               \
    }                                                                        \
};

namespace simics {
namespace systemc {
namespace simics2tlm {

template<typename TBase>
class GasketAdapter : public DescriptionInterface<TBase> {
  public:
    virtual ~GasketAdapter() {}

    std::vector<std::string> description(DescriptionType type) override {
        if (type != DESCRIPTION_TYPE_SIMICS2TLM)
            return {};

        GasketOwner *owner = gasket_owner();
        auto *multi_owner = dynamic_cast<MultiGasketOwner *>(owner);
        if (multi_owner) {
            std::set<int> keys = multi_owner->keys();
            if (keys.empty())
                return description(owner->gasket());

            std::vector<std::string> result;
            for (auto i = keys.begin(); i != keys.end(); ++i) {
                std::vector<std::string> r =
                        description(multi_owner->findGasket(*i));
                if (r.empty())
                    continue;

                result.push_back(r[0]);
                result.push_back(r[1]);
            }

            return result;
        }

        if (!owner)
            return {};

        return description(owner->gasket());
    }
    virtual simics2tlm::GasketOwner *gasket_owner() const = 0;

  protected:
    void append(std::string *result, std::string s) {
        if (result->empty()) {
            *result = s;
        } else {
            *result += ';';
            *result += s;
        }
    }
    std::vector<std::string> description(GasketInterface::Ptr gasket_ptr) {
        if (!gasket_ptr)
            return {};

        return {gasket_ptr->gasket_name(),
                gasket_ptr->get_target_socket()->name()};
    }
};

}  // namespace simics2tlm
}  // namespace systemc
}  // namespace simics

#endif
