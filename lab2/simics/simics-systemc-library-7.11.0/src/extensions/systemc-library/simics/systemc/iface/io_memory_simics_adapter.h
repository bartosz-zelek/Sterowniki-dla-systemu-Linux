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

#ifndef SIMICS_SYSTEMC_IFACE_IO_MEMORY_SIMICS_ADAPTER_H
#define SIMICS_SYSTEMC_IFACE_IO_MEMORY_SIMICS_ADAPTER_H

#include <simics/devs/io-memory.h>
#include <simics/systemc/iface/io_memory_interface.h>
#include <simics/systemc/iface/simics_adapter.h>
#include <simics/types/map_info.h>

#include <string>
#include <vector>

namespace simics {
namespace systemc {
namespace iface {

/** Adapter for Simics io_memory interface. */
template<typename TBase, typename TInterface = IoMemoryInterface>
class IoMemorySimicsAdapter : public SimicsAdapter<io_memory_interface_t> {
  public:
    IoMemorySimicsAdapter()
        : SimicsAdapter<io_memory_interface_t>(
            IO_MEMORY_INTERFACE, init_iface()) {
    }

  protected:
    static ::exception_type_t operation(conf_object_t *obj,
                                        generic_transaction_t *mem_op,
                                        ::map_info_t map_info) {
        types::map_info_t info(map_info.base,
                               map_info.start,
                               map_info.length,
                               map_info.function);
        return static_cast<::exception_type_t>(
                adapter<TBase, TInterface>(obj)->operation(mem_op, info));
    }

  private:
    std::vector<std::string> description(conf_object_t *obj,
                                       DescriptionType type) {
        return descriptionBase<TBase, TInterface>(obj, type);
    }
    io_memory_interface_t init_iface() {
        io_memory_interface_t iface = {};
        iface.operation = operation;
        return iface;
    }
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
