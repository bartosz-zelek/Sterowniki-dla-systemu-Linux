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

#ifndef SIMICS_SYSTEMC_INJECTION_EXTENSION_INJECT_SERIAL_DEVICE_H
#define SIMICS_SYSTEMC_INJECTION_EXTENSION_INJECT_SERIAL_DEVICE_H



#include <simics/systemc/iface/serial_device_extension.h>
#include <simics/systemc/injection/extension/extension_setter.h>
#include <simics/systemc/injection/inject_base.h>

#include <string>

namespace simics {
namespace systemc {
namespace injection {
namespace extension {

/** \internal */
template <typename TPAYLOAD>
class InjectSerialDevice : public InjectBase<TPAYLOAD> {
  public:
    ATTR_DICT_PARSER_NAMESPACE("serial_device.")

    virtual bool setValue(AttrDictParser *parser, const std::string &key,
                          attr_value_t *attr, TPAYLOAD *gp) {
        if (key == "write") {
            int v = 0;
            if (!parser->value(&v))
                return false;

            ExtensionSetter<TPAYLOAD, iface::SerialDeviceExtension>(gp)
                ->write(v);
            return true;
        } else if (key == "receive_ready") {
            ExtensionSetter<TPAYLOAD, iface::SerialDeviceExtension>(gp)
                ->receive_ready();
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
