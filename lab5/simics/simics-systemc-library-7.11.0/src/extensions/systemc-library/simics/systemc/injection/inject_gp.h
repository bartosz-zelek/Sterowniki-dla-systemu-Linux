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

#ifndef SIMICS_SYSTEMC_INJECTION_INJECT_GP_H
#define SIMICS_SYSTEMC_INJECTION_INJECT_GP_H



#include <simics/systemc/injection/attr_dict_parser.h>
#include <simics/systemc/injection/inject_base.h>

#include <tlm>

#include <string>

namespace simics {
namespace systemc {
namespace injection {

/** \internal */
template <typename TPAYLOAD>
class InjectGp : public InjectBase<TPAYLOAD> {
  public:
    ATTR_DICT_PARSER_NAMESPACE("gp.")

    virtual bool setValue(AttrDictParser *parser, const std::string &key,
                          attr_value_t *attr, TPAYLOAD *gp) {
        if (key == "command") {
            uint64_t v;
            if (!parser->value(&v))
                return false;

            if (v > tlm::TLM_IGNORE_COMMAND) {
                parser->reportError("command must be in range [%i - %i]",
                                    tlm::TLM_READ_COMMAND,
                                    tlm::TLM_IGNORE_COMMAND);
                return false;
            }

            gp->set_command(static_cast<tlm::tlm_command>(v));
        } else if (key == "address") {
            uint64_t v;
            if (!parser->value(&v))
                return false;

            gp->set_address(v);
        } else if (key == "data_ptr") {
            const uint8_t *v = NULL;
            if (!parser->value(&v))
                return false;

            gp->set_data_ptr(const_cast<unsigned char*>(v));
            gp->set_data_length(SIM_attr_data_size(*attr));
            gp->set_streaming_width(SIM_attr_data_size(*attr));
        } else if (key == "response_status") {
            int64_t v;
            if (!parser->value(&v))
                return false;

            if (v > tlm::TLM_OK_RESPONSE ||
                v < tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE) {
                parser->reportError("response_status value must be in range "
                                    "[%i - %i]",
                                    tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE,
                                    tlm::TLM_OK_RESPONSE);
                return false;
            }

            gp->set_response_status(static_cast<tlm::tlm_response_status>(
                v));
        } else if (key == "streaming_width") {
            uint64_t v;
            if (!parser->value(&v))
                return false;

            gp->set_streaming_width(v);
        } else if (key == "byte_enable_ptr") {
            uint64_t v;
            if (!parser->value(&v))
                return false;

            gp->set_byte_enable_ptr(reinterpret_cast<unsigned char *>(v));
        } else if (key == "byte_enable_length") {
            uint64_t v;
            if (!parser->value(&v))
                return false;

            gp->set_byte_enable_length(v);
        } else if (key == "gp_option") {
            uint64_t v;
            if (!parser->value(&v))
                return false;

            if (v > tlm::TLM_FULL_PAYLOAD_ACCEPTED) {
                parser->reportError("gp_option must be in range [%i - %i]",
                                    tlm::TLM_MIN_PAYLOAD,
                                    tlm::TLM_FULL_PAYLOAD_ACCEPTED);
                return false;
            }

            gp->set_gp_option(static_cast<tlm::tlm_gp_option>(v));
        } else if (key == "dmi_allowed") {
            bool v;
            if (!parser->value(&v))
                return false;

            gp->set_dmi_allowed(v);
        } else {
            return false;
        }

        return true;
    }
};

}  // namespace injection
}  // namespace systemc
}  // namespace simics

#endif
