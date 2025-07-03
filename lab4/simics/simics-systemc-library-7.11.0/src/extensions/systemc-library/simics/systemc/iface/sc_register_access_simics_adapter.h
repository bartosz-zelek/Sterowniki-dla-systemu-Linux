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

#ifndef SIMICS_SYSTEMC_IFACE_SC_REGISTER_ACCESS_SIMICS_ADAPTER_H
#define SIMICS_SYSTEMC_IFACE_SC_REGISTER_ACCESS_SIMICS_ADAPTER_H

#include <simics/systemc/iface/simics_adapter.h>
#include <simics/systemc/iface/sc_register_access_interface.h>
#include <simics/types/buffer.h>
#include <simics/types/bytes.h>

#include <systemc-interfaces.h>

#include <string>
#include <vector>

namespace simics {
namespace systemc {
namespace iface {

/** \internal */
template<typename TBase, typename TInterface = ScRegisterAccessInterface>
class ScRegisterAccessSimicsAdapter
    : public SimicsAdapter<sc_register_access_interface_t> {
  public:
    ScRegisterAccessSimicsAdapter()
        : SimicsAdapter<sc_register_access_interface_t>(
            SC_REGISTER_ACCESS_INTERFACE, init_iface()) {
    }

  protected:
    static ::exception_type_t read(conf_object_t *obj, ::uint64 address,
                                   buffer_t value) {
        types::buffer_t v = {value.data, value.len};
        return static_cast<::exception_type_t>(
                adapter<TBase, TInterface>(obj)->read(address, v));
    }
    static ::exception_type_t write(conf_object_t *obj, ::uint64 address,
                                    bytes_t value) {
        types::bytes_t v = {value.data, value.len};
        return static_cast<::exception_type_t>(
                adapter<TBase, TInterface>(obj)->write(address, v));
    }

  private:
    sc_register_access_interface_t init_iface() {
        sc_register_access_interface_t iface = {};
        iface.read = read;
        iface.write = write;
        return iface;
    }
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
