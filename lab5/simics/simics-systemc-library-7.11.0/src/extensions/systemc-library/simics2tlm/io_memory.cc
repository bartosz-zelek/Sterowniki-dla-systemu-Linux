// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2013 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <simics/base/log.h>
#include <simics/systemc/simics2tlm/io_memory.h>
#include <simics/systemc/iface/map_info_extension.h>
#include <simics/systemc/adapter_log_groups.h>

#include <memory>
#include <vector>

namespace simics {
namespace systemc {
namespace simics2tlm {

exception_type_t IoMemory::operation(generic_transaction_t *mop,
                                     const types::map_info_t &info) {
    // Get required Simics transaction data
    uint64 offset = (SIM_get_mem_op_physical_address(mop)
                     + info.start - info.base);
    unsigned int size = SIM_get_mem_op_size(mop);
    int id = info.function;
    GasketInterface::Ptr gasket = findGasket(id);
    if (!MultiGasketOwner::hasId(id) && !MultiGasketOwner::empty()) {
        SIM_LOG_INFO(3, gasket->simics_obj(), Log_TLM,
                     "IoMemory::operation, warning: no gasket for function"
                     " id=%d found, using gasket with lowest ID", id);
    }
    iface::Transaction t = pool_.acquire();
    unsigned char *data_ptr = mop->real_address;

    // Handle inversed endianness
    std::unique_ptr<unsigned char[]> data_ptr_inverse_endian;
    if (mop->inverse_endian && size != 0) {
        data_ptr_inverse_endian = std::make_unique<unsigned char[]>(size);
        data_ptr = data_ptr_inverse_endian.get();
        if (SIM_mem_op_is_write(mop)) {
            SIM_c_get_mem_op_value_buf(mop, data_ptr);
        }
    }

    // Specific to the TLM-2.0 device
    t->set_data_ptr(data_ptr);
    t->set_address(offset);
    t->set_data_length(size);
    t->set_streaming_width(size);

    if (SIM_mem_op_is_read(mop)) {
        // Read operation
        t->set_read();
    } else {
        // Write operation
        t->set_write();
    }

    iface::MapInfoExtension ext(info);
    t->set_extension<iface::MapInfoExtension>(&ext);
    t.extension()->set_transport_debug(SIM_get_mem_op_inquiry(mop));
    bool success = gasket->trigger(&t);
    t->clear_extension<iface::MapInfoExtension>();

    if (!success) {
        SIM_set_mem_op_value_le(mop, 0);

        if (t->get_response_status() == tlm::TLM_ADDRESS_ERROR_RESPONSE) {
            return SIM_get_mem_op_inquiry(mop)
                ? Sim_PE_Inquiry_Outside_Memory
                : Sim_PE_IO_Not_Taken;
        } else {
            return SIM_get_mem_op_inquiry(mop)
                ? Sim_PE_Inquiry_Unhandled
                : Sim_PE_IO_Error;
        }
    }

    if (mop->inverse_endian && SIM_mem_op_is_read(mop)) {
        SIM_c_set_mem_op_value_buf(mop, data_ptr_inverse_endian.get());
    }

    return Sim_PE_No_Exception;
}

}  // namespace simics2tlm
}  // namespace systemc
}  // namespace simics
