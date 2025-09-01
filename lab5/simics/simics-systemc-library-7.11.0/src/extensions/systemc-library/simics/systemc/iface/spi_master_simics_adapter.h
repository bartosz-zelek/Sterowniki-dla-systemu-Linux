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

#ifndef SIMICS_SYSTEMC_IFACE_SPI_MASTER_SIMICS_ADAPTER_H
#define SIMICS_SYSTEMC_IFACE_SPI_MASTER_SIMICS_ADAPTER_H

#include <simics/devs/serial-peripheral-interface.h>
#include <simics/systemc/iface/spi_master_interface.h>
#include <simics/systemc/iface/simics_adapter.h>

#include <simics/base/types.h>
#include <simics/util/dbuffer.h>

#include <string>
#include <vector>

namespace simics {
namespace systemc {
namespace iface {

/** Adapter for Simics serial_peripheral_interface_master interface. */
template<typename TBase, typename TInterface = SpiMasterInterface>
class SpiMasterSimicsAdapter
        : public SimicsAdapter<serial_peripheral_interface_master_interface_t> {
  public:
    SpiMasterSimicsAdapter()
        : SimicsAdapter<serial_peripheral_interface_master_interface_t>(
            SERIAL_PERIPHERAL_INTERFACE_MASTER_INTERFACE, init_iface()) {
    }

  protected:
    static void spi_response(conf_object_t *obj, int bits, dbuffer_t *payload) {
        // According to reference manual, "bits defines the number of bits to
        // send, while payload defines the data to transfer. The size of the
        // payload buffer should be ceil(bits / 8) bytes."
        bytes_t data = dbuffer_bytes(payload);
        if ((bits + 7) / 8 != static_cast<int>(data.len)) {
            SIM_LOG_ERROR(obj, 0,
                          "Mismatch of the payload buffer size (%lu) %s (%d)",
                          data.len, "with ceil(bits / 8)", (bits + 7) / 8);
        }
        adapter<TBase, TInterface>(obj)->spi_response(data.data, data.len);
    }

  private:
    std::vector<std::string> description(conf_object_t *obj,
                                         DescriptionType type) {
        return descriptionBase<TBase, TInterface>(obj, type);
    }
    serial_peripheral_interface_master_interface_t init_iface() {
        serial_peripheral_interface_master_interface_t iface = {};
        iface.spi_response = spi_response;
        return iface;
    }
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
