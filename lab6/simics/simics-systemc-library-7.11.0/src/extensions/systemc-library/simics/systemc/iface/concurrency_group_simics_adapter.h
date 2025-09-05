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

#ifndef SIMICS_SYSTEMC_IFACE_CONCURRENCY_GROUP_SIMICS_ADAPTER_H
#define SIMICS_SYSTEMC_IFACE_CONCURRENCY_GROUP_SIMICS_ADAPTER_H

#include <simics/model-iface/concurrency.h>
#include <simics/systemc/iface/concurrency_group_interface.h>
#include <simics/systemc/iface/simics_adapter.h>

#include <string>
#include <vector>

namespace simics {
namespace systemc {
namespace iface {

/** Adapter for concurrency mode interface. */
template<typename TBase, typename TInterface = ConcurrencyGroupInterface>
class ConcurrencyGroupSimicsAdapter
    : public SimicsAdapter<concurrency_group_interface_t> {
  public:
    ConcurrencyGroupSimicsAdapter()
        : SimicsAdapter<concurrency_group_interface_t>(
                CONCURRENCY_GROUP_INTERFACE, init_iface()) {}

  protected:
    static attr_value_t serialized_memory_group(conf_object_t *NOTNULL obj,
                                                unsigned group_index) {
        std::vector<conf_object_t *> v = adapter<TBase, TInterface>(
                obj)->serialized_memory_group(group_index);
        return vectorToList(v);
    }
    static attr_value_t execution_group(conf_object_t *NOTNULL obj,
                                        unsigned group_index) {
        std::vector<conf_object_t *> v = adapter<TBase, TInterface>(
                obj)->execution_group(group_index);
        return vectorToList(v);
    }

  private:
    static attr_value_t vectorToList(const std::vector<conf_object_t *> &v) {
        if (v.size() == 0)
            return SIM_make_attr_nil();

        attr_value_t list = SIM_alloc_attr_list(v.size());
        for (unsigned i = 0; i < v.size(); ++i)
            SIM_attr_list_set_item(&list, i, SIM_make_attr_object(v[i]));

        return list;
    }
    std::vector<std::string> description(conf_object_t *obj,
                                         DescriptionType type) {
        return descriptionBase<TBase, TInterface>(obj, type);
    }
    concurrency_group_interface_t init_iface() {
        concurrency_group_interface_t iface = {};
        iface.serialized_memory_group = serialized_memory_group;
        iface.execution_group = execution_group;
        return iface;
    }
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
