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

#ifndef SIMICS_SYSTEMC_IFACE_SIMICS_ADAPTER_INTERFACE_H
#define SIMICS_SYSTEMC_IFACE_SIMICS_ADAPTER_INTERFACE_H

#include <simics/base/conf-object.h>
#include <simics/systemc/description_interface.h>

#include <string>
#include <vector>

namespace simics {
namespace systemc {
namespace iface {

class SimicsAdapterInterface {
  public:
    virtual ~SimicsAdapterInterface() {}
    virtual void set_simics_class(conf_class_t *conf_class) = 0;
    virtual conf_class_t *simics_class() const = 0;
    virtual void set_map_adapter(bool map) = 0;
    virtual bool map_adapter() const = 0;
    virtual std::vector<std::string> description(conf_object_t *obj,
                                                 DescriptionType type) {
        return {};
    }
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
