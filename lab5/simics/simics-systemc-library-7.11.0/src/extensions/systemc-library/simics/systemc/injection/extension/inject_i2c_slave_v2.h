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

#ifndef SIMICS_SYSTEMC_INJECTION_EXTENSION_INJECT_I2C_SLAVE_V2_H
#define SIMICS_SYSTEMC_INJECTION_EXTENSION_INJECT_I2C_SLAVE_V2_H



#include <simics/systemc/iface/i2c_slave_v2_extension.h>
#include <simics/systemc/injection/extension/extension_setter.h>
#include <simics/systemc/injection/inject_base.h>

#include <string>

namespace simics {
namespace systemc {
namespace injection {
namespace extension {

/** \internal */
template <typename TPAYLOAD>
class InjectI2cSlaveV2 : public InjectBase<TPAYLOAD> {
  public:
    ATTR_DICT_PARSER_NAMESPACE("i2c_slave_v2.")

    virtual bool setValue(AttrDictParser *parser, const std::string &key,
                          attr_value_t *attr, TPAYLOAD *gp) {
        if (key == "start") {
            uint8_t address = 0;
            if (!parser->value(&address))
                return false;

            ExtensionSetter<TPAYLOAD, iface::I2cSlaveV2Extension>(gp)
                ->start(address);
            return true;
        } else if (key == "read") {
            ExtensionSetter<TPAYLOAD, iface::I2cSlaveV2Extension>(gp)
                ->read();
            return true;
        } else if (key == "write") {
            uint8_t value = 0;
            if (!parser->value(&value))
                return false;

            ExtensionSetter<TPAYLOAD, iface::I2cSlaveV2Extension>(gp)
                ->write(value);
            return true;
        } else if (key == "stop") {
            ExtensionSetter<TPAYLOAD, iface::I2cSlaveV2Extension>(gp)
                ->stop();
            return true;
        } else if (key == "addresses") {
            ExtensionSetter<TPAYLOAD, iface::I2cSlaveV2Extension>(gp)
                ->addresses();
            return true;
        }

        return false;
    }
};

}  // namespace extension
}  // namespace injection
}  // namespace systemc
}  // namespace simics

#endif
