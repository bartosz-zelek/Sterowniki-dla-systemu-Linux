// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2018 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_IFACE_FREQUENCY_SIMICS_ADAPTER_H
#define SIMICS_SYSTEMC_IFACE_FREQUENCY_SIMICS_ADAPTER_H

#include <simics/devs/frequency.h>
#include <simics/systemc/iface/frequency_interface.h>
#include <simics/systemc/iface/simics_adapter.h>

#include <string>
#include <vector>

namespace simics {
namespace systemc {
namespace iface {

/** \internal */
template<typename TBase, typename TInterface = FrequencyInterface>
class FrequencySimicsAdapter : public SimicsAdapter<frequency_interface_t> {
  public:
    FrequencySimicsAdapter()
        : SimicsAdapter<frequency_interface_t>(FREQUENCY_INTERFACE,
                                                init_iface()) {
    }

  protected:
    static double get(conf_object_t *obj) {
        return adapter<TBase, TInterface>(obj)->get();
    }

  private:
    std::vector<std::string> description(conf_object_t *obj,
                                         DescriptionType type) {
        return descriptionBase<TBase, TInterface>(obj, type);
    }
    frequency_interface_t init_iface() {
        frequency_interface_t iface = {};
        iface.get = get;
        return iface;
    }
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
