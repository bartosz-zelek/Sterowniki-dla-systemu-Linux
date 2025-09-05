// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2022 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_TYPE_BANK_TYPE_H
#define SIMICS_TYPE_BANK_TYPE_H

#include <cstdint>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "simics/type/common-types.h"
#include "simics/type/register-type.h"

namespace simics {

/*
 * Type alias represents a type containing bank information
 *
 * It is a tuple consists of following members,
 *
 * 1. A name of the bank. The name should follow the Simics naming rules
 * 2. A human-readable description of the bank
 * 3. The contained registers on the bank
 */
using bank_t = std::tuple<Name, Description, std::vector<register_t>>;

/*
 * Type alias represents a type for bank memory
 *
 * It is an unordered_map consists of following members,
 *
 * Key is byte offset relative to this bank,
 * Value is a 8 bits integer
 */
using bank_memory_t = std::unordered_map<Offset, uint8_t>;
}  // namespace simics

#endif
