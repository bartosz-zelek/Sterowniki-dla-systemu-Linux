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

#ifndef SIMICS_SYSTEMC_INJECTION_EXTENSION_MAP_INFO_PARSER_H
#define SIMICS_SYSTEMC_INJECTION_EXTENSION_MAP_INFO_PARSER_H

#include <simics/base/types.h>

#include <simics/systemc/injection/attr_dict_parser.h>

#include <string>

namespace simics {
namespace systemc {
namespace injection {
namespace extension {

/** \internal */
class MapInfoParser : public AttrDictParser::ParserInterface {
  public:
    virtual std::string prefix() {return "";}
    virtual bool parse(AttrDictParser *parser, const std::string &key,
                       attr_value_t *attr) {
        uint64_t addr = 0;
        if (key == "base") {
            if (!parser->value(&addr))
                return false;

            map_info_.base = addr;
        } else if (key == "start") {
            if (!parser->value(&addr))
                return false;

            map_info_.start = addr;
        } else if (key == "length") {
            if (!parser->value(&addr))
                return false;

            map_info_.length = addr;
        } else if (key == "function") {
            int64_t function = 0;
            if (!parser->value(&function))
                return false;

            map_info_.function = function;
        } else {
            return false;
        }

        return true;
    }
    types::map_info_t map_info_;
};

}  // namespace extension
}  // namespace injection
}  // namespace systemc
}  // namespace simics

#endif
