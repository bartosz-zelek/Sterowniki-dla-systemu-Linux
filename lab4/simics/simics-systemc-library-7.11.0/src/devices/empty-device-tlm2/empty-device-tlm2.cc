/*
  © 2010 Intel Corporation
*/

#include "empty-device-tlm2.h"

#include <algorithm>
#include <string>
#include <sstream>

namespace {
const char * TAG = "intel/empty-device-tlm2";

void uint32_to_buf(unsigned char dst[], sc_dt::sc_uint<32> value) {
    dst[0] = value >> 24;
    dst[1] = (value >> 16) & 0xff;
    dst[2] = (value >> 8) & 0xff;
    dst[3] = value & 0xff;
}

sc_dt::sc_uint<32> buf_to_uint32(unsigned char src[]) {
    return src[0] << 24 | src[1] << 16 | src[2] << 8 | src[3];
}

std::string goodAccessMsg(const char *kind, sc_dt::uint64 offset,
                          unsigned size, sc_dt::sc_uint<32> value) {
    std::ostringstream os;
    os << "Good access (" << kind
       << ") of size " << size
       << " to address 0x" << std::hex << offset
       << " with value 0x" << value;
    return os.str();
}

std::string badAccessMsg(sc_dt::uint64 offset) {
    std::ostringstream os;
    os << "Invalid access to 0x" << std::hex << offset
       << " where nothing is mapped";
    return os.str();
}

}  // end namespace

void empty_device_tlm2::b_transport(tlm::tlm_generic_payload &trans,  // NOLINT
                                    sc_core::sc_time &t) {
    sc_dt::uint64 offset = trans.get_address();
    unsigned int size = trans.get_data_length();
    unsigned char *data = trans.get_data_ptr();

    if (size != 4) {
        SC_REPORT_WARNING(TAG, "Operation not four bytes in size");
        trans.set_response_status(tlm::TLM_BURST_ERROR_RESPONSE);
        return;
    }

    switch (offset) {
    case 0x00:
        // If neither read, nor write, it is an ignore command which should
        // return TLM_OK_RESPONSE if the corresponding read or write should
        // succeed (IEEE-1666-2011 14.17.i).
        trans.set_response_status(tlm::TLM_OK_RESPONSE);
        if (trans.is_write()) {
            register1_ = buf_to_uint32(data);
            SC_REPORT_INFO(TAG, goodAccessMsg("Write", offset, size,
                                              register1_).c_str());
        }
        if (trans.is_read()) {
            uint32_to_buf(data, register1_);
            SC_REPORT_INFO(TAG, goodAccessMsg("Read", offset, size,
                                              register1_).c_str());
        }
        break;

    default:
        trans.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
        SC_REPORT_WARNING(TAG, badAccessMsg(offset).c_str());
        break;
    }
}
