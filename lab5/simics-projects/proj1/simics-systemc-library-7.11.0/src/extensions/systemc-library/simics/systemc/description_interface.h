// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2020 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_DESCRIPTION_INTERFACE_H
#define SIMICS_SYSTEMC_DESCRIPTION_INTERFACE_H

#include <string>
#include <vector>

namespace simics {
namespace systemc {

enum DescriptionType {
    DESCRIPTION_TYPE_SIMICS2TLM,
    DESCRIPTION_TYPE_TLM2SIMICS,
    DESCRIPTION_TYPE_SIMICS2SYSTEMC,
    DESCRIPTION_TYPE_SYSTEMC2SIMICS
};

/** \internal */
template<typename TBase>
class DescriptionInterface {
  public:
    virtual ~DescriptionInterface() {}
    virtual std::vector<std::string> description(DescriptionType type) = 0;
};

}  // namespace systemc
}  // namespace simics

#endif
