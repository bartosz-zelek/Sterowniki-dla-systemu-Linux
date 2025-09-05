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

#ifndef SIMICS_SYSTEMC_TLM2SIMICS_PCIE_MAP_H
#define SIMICS_SYSTEMC_TLM2SIMICS_PCIE_MAP_H

#include <simics/systemc/iface/pcie_map_extension.h>
#include <simics/systemc/iface/pcie_map_interface.h>
#include <simics/systemc/iface/receiver_interface.h>
#include <simics/systemc/interface_provider.h>
#include <simics/systemc/tlm2simics/transaction_handler.h>

namespace simics {
namespace systemc {
namespace tlm2simics {

class PcieMap : public InterfaceProvider,
                public TransactionHandler,
                public iface::PcieMapInterface {
  public:
    PcieMap() : InterfaceProvider("pcie_map"),
                TransactionHandler(
                        this,
                        iface::PcieMapExtension::createIgnoreReceiver()),
                receiver_(iface::PcieMapExtension::createReceiver(this)),
                device_(nullptr) {}
    virtual ~PcieMap();
    PcieMap(const PcieMap &other) = delete;
    PcieMap& operator=(const PcieMap &other) = delete;

    // TransactionHandler
    iface::ReceiverInterface *receiver() override;

    // iface::PcieMapInterface
    void add_map(types::map_info_t info, types::pcie_type_t type) override;
    void del_map(types::map_info_t::physical_address_t base,
                 types::pcie_type_t type) override;
    void add_function(conf_object_t *map_obj, uint16_t function_id) override;
    void del_function(conf_object_t *map_obj, uint16_t function_id) override;
    void enable_function(uint16_t function_id) override;
    void disable_function(uint16_t function_id) override;
    uint16_t get_device_id(conf_object_t *dev_obj) override;

  private:
    // TransactionHandler
    tlm::tlm_response_status simics_transaction(
        ConfObjectRef &simics_obj,
        tlm::tlm_generic_payload *trans) override;

    iface::ReceiverInterface *receiver_;
    conf_object_t *device_;
};

}  // namespace tlm2simics
}  // namespace systemc
}  // namespace simics

#endif
