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

#ifndef SIMICS_SYSTEMC_IFACE_REGISTER_VIEW_SIMICS_ADAPTER_H
#define SIMICS_SYSTEMC_IFACE_REGISTER_VIEW_SIMICS_ADAPTER_H

#include <simics/model-iface/register-view.h>
#include <simics/systemc/iface/register_view_interface.h>
#include <simics/systemc/iface/simics_adapter.h>

#include <string>
#include <vector>

namespace simics {
namespace systemc {
namespace iface {

/** \internal */
template<typename TBase,
         typename TInterface = RegisterViewInterface>
class RegisterViewSimicsAdapter
    : public SimicsAdapter<register_view_interface_t> {
  public:
    RegisterViewSimicsAdapter()
        : SimicsAdapter<register_view_interface_t>(
            REGISTER_VIEW_INTERFACE, init_iface()) {
    }

  protected:
    static const char *description(conf_object_t *obj) {
        return adapter<TBase, TInterface>(obj)->description();
    }
    static bool big_endian_bitorder(conf_object_t *obj) {
        return adapter<TBase, TInterface>(obj)->big_endian_bitorder();
    }
    static unsigned number_of_registers(conf_object_t *obj) {
        return adapter<TBase, TInterface>(obj)->number_of_registers();
    }
    static attr_value_t register_info(conf_object_t *obj, unsigned reg) {
        return adapter<TBase, TInterface>(obj)->register_info(reg);
    }
    static ::uint64 get_register_value(conf_object_t *obj, unsigned reg) {
        return adapter<TBase, TInterface>(obj)->get_register_value(reg);
    }
    static void set_register_value(conf_object_t *obj, unsigned reg,
                                   ::uint64 val) {
        adapter<TBase, TInterface>(obj)->set_register_value(reg, val);
    }

  private:
    std::vector<std::string> description(conf_object_t *obj,
                                         DescriptionType type) {
        return descriptionBase<TBase, TInterface>(obj, type);
    }
    register_view_interface_t init_iface() {
        register_view_interface_t iface = {};
        iface.description = description;
        iface.big_endian_bitorder = big_endian_bitorder;
        iface.number_of_registers = number_of_registers;
        iface.register_info = register_info;
        iface.get_register_value = get_register_value;
        iface.set_register_value = set_register_value;
        return iface;
    }
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
