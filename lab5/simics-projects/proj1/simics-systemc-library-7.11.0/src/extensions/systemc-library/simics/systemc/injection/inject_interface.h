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

#ifndef SIMICS_SYSTEMC_INJECTION_INJECT_INTERFACE_H
#define SIMICS_SYSTEMC_INJECTION_INJECT_INTERFACE_H

#include <simics/systemc/injection/attr_dict_parser.h>

namespace simics {
namespace systemc {
namespace injection {

/** \internal */
template <typename TPAYLOAD>
class InjectInterface : public AttrDictParser::ParserInterface {
  public:
    virtual ~InjectInterface() {}
    virtual void attrToValue(AttrDictParser *parser, TPAYLOAD *gp) = 0;
    virtual void released(TPAYLOAD *gp) = 0;
};

}  // namespace injection
}  // namespace systemc
}  // namespace simics

#endif
