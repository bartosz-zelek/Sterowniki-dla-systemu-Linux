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

#ifndef SIMICS_SYSTEMC_INJECTION_EXTENSION_INJECT_MAP_INFO_H
#define SIMICS_SYSTEMC_INJECTION_EXTENSION_INJECT_MAP_INFO_H



#include <simics/systemc/iface/map_info_extension.h>
#include <simics/systemc/injection/attr_dict_parser.h>
#include <simics/systemc/injection/extension/map_info_parser.h>
#include <simics/systemc/injection/inject_base.h>

#include <string>

namespace simics {
namespace systemc {
namespace injection {
namespace extension {

/** \internal */
template <typename TPAYLOAD>
class InjectMapInfo : public InjectBase<TPAYLOAD> {
  public:
    ATTR_DICT_PARSER_NAMESPACE("map_info")

    virtual bool setValue(AttrDictParser *parser, const std::string &key,
                          attr_value_t *attr, TPAYLOAD *gp) {
        if (key == "") {
            AttrDictParser p = parser->init(attr);
            MapInfoParser info;
            if (!p.parse(&info))
                return false;

            // The created extension is deleted in the InjectRegistry when the
            // GP is deleted.
            delete gp->set_extension(
                new iface::MapInfoExtension(info.map_info_));
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
