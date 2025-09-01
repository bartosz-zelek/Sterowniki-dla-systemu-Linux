// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2024 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_IFACE_PCIE_MAP_EXTENSION_H
#define SIMICS_SYSTEMC_IFACE_PCIE_MAP_EXTENSION_H

#include <simics/systemc/iface/extension.h>
#include <simics/systemc/iface/pcie_map_interface.h>

namespace simics {
namespace systemc {
namespace iface {

/** Extension for Simics pcie_map interface. See base class for details. */
class PcieMapExtension : public Extension<PcieMapExtension,
                                          PcieMapInterface> {
  public:
    virtual void call(PcieMapInterface *device) {
        switch (method_.value<Method>()) {
        case ADD_MAP:
            device->add_map(
                method_input_[0].value<types::map_info_t>(),
                method_input_[1].value<types::pcie_type_t>());
            break;
        case DEL_MAP:
            device->del_map(
                method_input_[0].value<types::map_info_t::physical_address_t>(),
                method_input_[1].value<types::pcie_type_t>());
            break;
        case ADD_FUNCTION:
            device->add_function(
                method_input_[0].value<conf_object_t *>(),
                method_input_[1].value<uint16_t>());
            break;
        case DEL_FUNCTION:
            device->del_function(
                method_input_[0].value<conf_object_t *>(),
                method_input_[1].value<uint16_t>());
            break;
        case ENABLE_FUNCTION:
            device->enable_function(
                method_input_[0].value<uint16_t>());
            break;
        case DISABLE_FUNCTION:
            device->disable_function(
                method_input_[0].value<uint16_t>());
            break;
        case GET_DEVICE_ID:
            method_return_ = device->get_device_id(
                    method_input_[0].value<conf_object_t *>());
            break;
        }
    }

    virtual void add_map(types::map_info_t info, types::pcie_type_t type) {
        method_ = ADD_MAP;
        method_input_.push_back(info);
        method_input_.push_back(type);
        send();
    }

    virtual void del_map(types::map_info_t::physical_address_t base,
                         types::pcie_type_t type) {
        method_ = DEL_MAP;
        method_input_.push_back(base);
        method_input_.push_back(type);
        send();
    }

    virtual void add_function(conf_object_t *map_obj, uint16_t function_id) {
        method_ = ADD_FUNCTION;
        method_input_.push_back(map_obj);
        method_input_.push_back(function_id);
        send();
    }

    virtual void del_function(conf_object_t *map_obj, uint16_t function_id) {
        method_ = DEL_FUNCTION;
        method_input_.push_back(map_obj);
        method_input_.push_back(function_id);
        send();
    }

    virtual void enable_function(uint16_t function_id) {
        method_ = ENABLE_FUNCTION;
        method_input_.push_back(function_id);
        send();
    }

    virtual void disable_function(uint16_t function_id) {
        method_ = DISABLE_FUNCTION;
        method_input_.push_back(function_id);
        send();
    }

    virtual uint16_t get_device_id(conf_object_t *dev_obj) {
        method_ = GET_DEVICE_ID;
        method_input_.push_back(dev_obj);
        send();
        return method_return_.value<uint16_t>();
    }

  private:
    enum Method {
        ADD_MAP,
        DEL_MAP,
        ADD_FUNCTION,
        DEL_FUNCTION,
        ENABLE_FUNCTION,
        DISABLE_FUNCTION,
        GET_DEVICE_ID
    };
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
