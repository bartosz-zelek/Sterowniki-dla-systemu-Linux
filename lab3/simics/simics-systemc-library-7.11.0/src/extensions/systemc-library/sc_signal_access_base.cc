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

#include <simics/systemc/sc_signal_access_base.h>

namespace simics {
namespace systemc {

#define __SIMICS_SYSTEMC_SIGNAL_DEFINE_TRANSFORM(type, is_type,    \
                                                 to_type, to_attr) \
bool ScSignalAccessBase::attrToValue(const attr_value_t *attr,     \
                                     type *value) const {          \
    if (!attr) {                                                   \
        return false;                                              \
    }                                                              \
    if (!is_type(*attr)) {                                         \
        return false;                                              \
    }                                                              \
    *value = to_type(*attr);                                       \
    return true;                                                   \
}                                                                  \
                                                                   \
attr_value_t ScSignalAccessBase::valueToAttr(type value) const {   \
    return to_attr(value);                                         \
}

__SIMICS_SYSTEMC_SIGNAL_DEFINE_TRANSFORM(bool, SIM_attr_is_boolean,
                                         SIM_attr_boolean,
                                         SIM_make_attr_boolean)
__SIMICS_SYSTEMC_SIGNAL_DEFINE_TRANSFORM(int64_t, SIM_attr_is_int64,
                                         SIM_attr_integer, SIM_make_attr_int64)
__SIMICS_SYSTEMC_SIGNAL_DEFINE_TRANSFORM(int32_t, SIM_attr_is_int64,
                                         SIM_attr_integer, SIM_make_attr_int64)
__SIMICS_SYSTEMC_SIGNAL_DEFINE_TRANSFORM(int16_t, SIM_attr_is_int64,
                                         SIM_attr_integer, SIM_make_attr_int64)
__SIMICS_SYSTEMC_SIGNAL_DEFINE_TRANSFORM(int8_t, SIM_attr_is_int64,
                                         SIM_attr_integer, SIM_make_attr_int64)
#ifndef _WIN32
__SIMICS_SYSTEMC_SIGNAL_DEFINE_TRANSFORM(long long unsigned int,  // NOLINT
                                         SIM_attr_is_uint64,
                                         SIM_attr_integer, SIM_make_attr_uint64)
#endif
__SIMICS_SYSTEMC_SIGNAL_DEFINE_TRANSFORM(uint64_t, SIM_attr_is_uint64,
                                         SIM_attr_integer, SIM_make_attr_uint64)
__SIMICS_SYSTEMC_SIGNAL_DEFINE_TRANSFORM(uint32_t, SIM_attr_is_uint64,
                                         SIM_attr_integer, SIM_make_attr_uint64)
__SIMICS_SYSTEMC_SIGNAL_DEFINE_TRANSFORM(uint16_t, SIM_attr_is_uint64,
                                         SIM_attr_integer, SIM_make_attr_uint64)
__SIMICS_SYSTEMC_SIGNAL_DEFINE_TRANSFORM(uint8_t, SIM_attr_is_uint64,
                                         SIM_attr_integer, SIM_make_attr_uint64)
__SIMICS_SYSTEMC_SIGNAL_DEFINE_TRANSFORM(double, SIM_attr_is_floating,
                                         SIM_attr_floating,
                                         SIM_make_attr_floating)
__SIMICS_SYSTEMC_SIGNAL_DEFINE_TRANSFORM(const char *, SIM_attr_is_string,
                                         SIM_attr_string, SIM_make_attr_string)

}  // namespace systemc
}  // namespace simics
