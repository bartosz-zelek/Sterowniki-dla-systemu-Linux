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

#include <simics/systemc/tlm2simics/pcie_map.h>
#include <simics/devs/pci.h>

#include <string>

namespace simics {
namespace systemc {
namespace tlm2simics {

namespace {
::map_info_t convertToSimics(types::map_info_t info) {
    ::map_info_t map_info = {};
    map_info.base = info.base;
    map_info.start = info.start;
    map_info.length = info.length;
    map_info.function = info.function;
    return map_info;
}

std::string NamePcieMapHelper(types::pcie_type_t type) {
    std::string name = "port.";
    switch (type) {
    case types::PCIE_Type_Cfg:
        name += "cfg";
        break;
    case types::PCIE_Type_Mem:
        name += "mem";
        break;
    case types::PCIE_Type_IO:
        name += "io";
        break;
    case types::PCIE_Type_Msg:
        name += "msg";
        break;
    default:
        // Unsupported type, caller needs to check the name
        break;
    }
    return name;
}
}  // namespace

void PcieMap::add_map(types::map_info_t info, types::pcie_type_t type) {
    std::string helper_name = NamePcieMapHelper(type);
    if (helper_name == "port.") {
        SIM_LOG_ERROR(device_, 0, "add_map: Unsupported PCIe type");
        return;
    }
    auto *helper = SIM_object_descendant(device_, helper_name.c_str());
    if (helper == nullptr) {
        get_interface<pcie_map_interface_t>()->add_map(
                target().object(), device_, convertToSimics(info),
                static_cast<::pcie_type_t>(type));
    } else {
        get_interface<pcie_map_interface_t>()->add_map(
                target().object(), helper, convertToSimics(info),
                static_cast<::pcie_type_t>(type));
    }
}

void PcieMap::del_map(types::map_info_t::physical_address_t base,
                      types::pcie_type_t type) {
    std::string helper_name = NamePcieMapHelper(type);
    if (helper_name == "port.") {
        SIM_LOG_ERROR(device_, 0, "del_map: Unsupported PCIe type");
        return;
    }
    auto *helper = SIM_object_descendant(device_, helper_name.c_str());
    if (helper == nullptr) {
        get_interface<pcie_map_interface_t>()->del_map(
                target().object(), device_,
                static_cast<::physical_address_t>(base),
                static_cast<::pcie_type_t>(type));
    } else {
        get_interface<pcie_map_interface_t>()->del_map(
                target().object(), helper,
                static_cast<::physical_address_t>(base),
                static_cast<::pcie_type_t>(type));
    }
}

void PcieMap::add_function(conf_object_t *map_obj, uint16_t function_id) {
    get_interface<pcie_map_interface_t>()->add_function(
            target().object(), map_obj, function_id);

    // Simics-22414 pcie-downstream-port maps same object to both CFG/MSG space
    // delete the wrong map and remap with correct type object
    types::map_info_t info;
    info.base = function_id;
    info.base <<= 48;
    info.length = 1ULL << 48;
    get_interface<pcie_map_interface_t>()->del_map(
                target().object(), map_obj,
                static_cast<::physical_address_t>(info.base),
                static_cast<::pcie_type_t>(types::PCIE_Type_Msg));
    add_map(info, types::PCIE_Type_Msg);
}

void PcieMap::del_function(conf_object_t *map_obj, uint16_t function_id) {
    get_interface<pcie_map_interface_t>()->del_function(
            target().object(), map_obj, function_id);
}

void PcieMap::enable_function(uint16_t function_id) {
    get_interface<pcie_map_interface_t>()->enable_function(
            target().object(), function_id);
}

void PcieMap::disable_function(uint16_t function_id) {
    get_interface<pcie_map_interface_t>()->disable_function(
            target().object(), function_id);
}

uint16_t PcieMap::get_device_id(conf_object_t *dev_obj) {
    // The device object is needed to get the map helper which is a
    // port object of it (required by SIM_object_descendant)
    if (device_ == nullptr) {
        device_ = dev_obj;
    }
    return get_interface<pcie_map_interface_t>()->get_device_id(
            target().object(), dev_obj);
}

iface::ReceiverInterface *PcieMap::receiver() {
    return receiver_;
}

PcieMap::~PcieMap() {
    delete receiver_;
}

tlm::tlm_response_status PcieMap::simics_transaction(
        ConfObjectRef &simics_obj,
        tlm::tlm_generic_payload *trans) {
    if (receiver_->handle(trans)) {
        return tlm::TLM_OK_RESPONSE;
    }

    SIM_LOG_SPEC_VIOLATION(1, simics_obj, 0, "Expected PcieMapExtension");
    return tlm::TLM_COMMAND_ERROR_RESPONSE;
}

}  // namespace tlm2simics
}  // namespace systemc
}  // namespace simics
