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

#ifndef SIMICS_SYSTEMC_IFACE_CONCURRENCY_MODE_SIMICS_ADAPTER_H
#define SIMICS_SYSTEMC_IFACE_CONCURRENCY_MODE_SIMICS_ADAPTER_H

#include <simics/model-iface/concurrency.h>
#include <simics/systemc/iface/concurrency_mode_interface.h>
#include <simics/systemc/iface/simics_adapter.h>

#include <string>
#include <vector>

namespace simics {
namespace systemc {
namespace iface {

/** Adapter for concurrency mode interface. */
template<typename TBase, typename TInterface = ConcurrencyModeInterface>
class ConcurrencyModeSimicsAdapter
    : public SimicsAdapter<concurrency_mode_interface_t> {
  public:
    ConcurrencyModeSimicsAdapter()
        : SimicsAdapter<concurrency_mode_interface_t>(
                CONCURRENCY_MODE_INTERFACE, init_iface()) {}

  protected:
    static concurrency_mode_t supported_modes(conf_object_t *NOTNULL obj) {
        return adapter<TBase, TInterface>(obj)->supported_modes();
    }
    static concurrency_mode_t current_mode(conf_object_t *NOTNULL obj) {
        return adapter<TBase, TInterface>(obj)->current_mode();
    }
    static void switch_mode(conf_object_t *NOTNULL obj,
                            concurrency_mode_t mode) {
        adapter<TBase, TInterface>(obj)->switch_mode(mode);
    }

  private:
    std::vector<std::string> description(conf_object_t *obj,
                                         DescriptionType type) {
        return descriptionBase<TBase, TInterface>(obj, type);
    }
    concurrency_mode_interface_t init_iface() {
        concurrency_mode_interface_t iface = {};
        iface.supported_modes = supported_modes;
        iface.current_mode = current_mode;
        iface.switch_mode = switch_mode;
        return iface;
    }
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
