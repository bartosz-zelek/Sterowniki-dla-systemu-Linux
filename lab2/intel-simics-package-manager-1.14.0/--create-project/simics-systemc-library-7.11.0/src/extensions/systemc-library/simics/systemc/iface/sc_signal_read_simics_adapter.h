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

#ifndef SIMICS_SYSTEMC_IFACE_SC_SIGNAL_READ_SIMICS_ADAPTER_H
#define SIMICS_SYSTEMC_IFACE_SC_SIGNAL_READ_SIMICS_ADAPTER_H

#include <simics/systemc/iface/sc_signal_read_interface.h>
#include <simics/systemc/iface/simics_adapter.h>

#include <systemc-interfaces.h>

#include <string>
#include <vector>

namespace simics {
namespace systemc {
namespace iface {

/** \internal */
template<typename TBase,
         typename TInterface = ScSignalReadInterface>
class ScSignalReadSimicsAdapter
    : public SimicsAdapter<sc_signal_read_interface_t> {
  public:
    ScSignalReadSimicsAdapter()
        : SimicsAdapter<sc_signal_read_interface_t>(
            SC_SIGNAL_READ_INTERFACE, init_iface()) {
    }

  protected:
    static attr_value_t read(conf_object_t *obj) {
        return adapter<TBase, TInterface>(obj)->read();
    }

  private:
    std::vector<std::string> description(conf_object_t *obj,
                                         DescriptionType type) {
        return descriptionBase<TBase, TInterface>(obj, type);
    }
    sc_signal_read_interface_t init_iface() {
        sc_signal_read_interface_t iface = {};
        iface.read = read;
        return iface;
    }
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
