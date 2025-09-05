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

#ifndef SIMICS_SYSTEMC_INJECTION_ATTR_DMI_H
#define SIMICS_SYSTEMC_INJECTION_ATTR_DMI_H



#include <simics/systemc/injection/attr_dict_parser.h>

#include <tlm>

#include <string>

namespace simics {
namespace systemc {
namespace injection {

/** \internal */
class AttrDmi : public AttrDictParser::ParserInterface {
  public:
    explicit AttrDmi(tlm::tlm_dmi *dmi) : dmi_(dmi) {}
    ATTR_DICT_PARSER_NAMESPACE("dmi.")

    virtual bool parse(AttrDictParser *parser, const std::string &key,
                       attr_value_t *attr) {
        if (key == "dmi_ptr") {
            uint64_t v;
            if (!parser->value(&v))
                return false;

            dmi_->set_dmi_ptr(reinterpret_cast<unsigned char*> (v));
        } else if (key == "start_address") {
            uint64_t v;
            if (!parser->value(&v))
                return false;

            dmi_->set_start_address(v);
        } else if (key == "end_address") {
            uint64_t v;
            if (!parser->value(&v))
                return false;

            dmi_->set_end_address(v);
        } else if (key == "read_latency") {
            uint64_t v;
            if (!parser->value(&v))
                return false;

            dmi_->set_read_latency(sc_core::sc_time::from_value(v));
        } else if (key == "write_latency") {
            uint64_t v;
            if (!parser->value(&v))
                return false;

            dmi_->set_write_latency(sc_core::sc_time::from_value(v));
        } else if (key == "granted_access") {
            uint64_t v;
            if (!parser->value(&v))
                return false;

            dmi_->set_granted_access(static_cast<tlm::tlm_dmi::dmi_access_e>(
                v));
        } else {
            return false;
        }

        return true;
    }

  private:
    tlm::tlm_dmi *dmi_;
};

}  // namespace injection
}  // namespace systemc
}  // namespace simics

#endif
