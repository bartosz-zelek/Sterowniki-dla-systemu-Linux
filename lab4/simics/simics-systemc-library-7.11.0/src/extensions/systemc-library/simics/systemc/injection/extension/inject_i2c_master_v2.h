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

#ifndef SIMICS_SYSTEMC_INJECTION_EXTENSION_INJECT_I2C_MASTER_V2_H
#define SIMICS_SYSTEMC_INJECTION_EXTENSION_INJECT_I2C_MASTER_V2_H



#include <simics/systemc/iface/i2c_master_v2_extension.h>
#include <simics/types/i2c_ack.h>
#include <simics/systemc/injection/extension/extension_setter.h>
#include <simics/systemc/injection/inject_base.h>

#include <string>

namespace simics {
namespace systemc {
namespace injection {
namespace extension {

/** \internal */
template <typename TPAYLOAD>
class InjectI2cMasterV2 : public InjectBase<TPAYLOAD> {
  public:
    ATTR_DICT_PARSER_NAMESPACE("i2c_master_v2.")

    virtual bool setValue(AttrDictParser *parser, const std::string &key,
                          attr_value_t *attr, TPAYLOAD *gp) {
        if (key == "acknowledge") {
            types::i2c_ack_t ack;
            if (!i2cAckValue(parser, &ack, key))
                return false;

            ExtensionSetter<TPAYLOAD, iface::I2cMasterV2Extension>(gp)
                ->acknowledge(ack);
            return true;
        } else if (key == "read_response") {
            uint8_t value = 0;
            if (!parser->value(&value))
                return false;

            ExtensionSetter<TPAYLOAD, iface::I2cMasterV2Extension>(gp)
                ->read_response(value);
            return true;
        }

        return false;
    }
    bool i2cAckValue(AttrDictParser *parser, simics::types::i2c_ack_t *v,
                     std::string key) {
        uint64_t ack = 0;
        if (!parser->value(&ack))
            return false;

        if (ack != types::I2C_ack && ack != types::I2C_noack) {
            parser->reportError("%s must be either I2C_ack or I2C_noack",
                                key.c_str());
            return false;
        }

        *v = static_cast<types::i2c_ack_t>(ack);
        return true;
    }
};

}  // namespace extension
}  // namespace injection
}  // namespace systemc
}  // namespace simics

#endif
