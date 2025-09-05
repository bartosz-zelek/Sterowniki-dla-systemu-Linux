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

#include <simics/systemc/sc_signal_access.h>

namespace simics {
namespace systemc {

bool ScSignalAccess::attrToValueScTime(const attr_value_t *attr,
                                       sc_core::sc_time *value) const {
    sc_core::sc_time::value_type time_value = 0;
    if (!attrToValue(attr, &time_value))
        return false;

    *value = sc_core::sc_time::from_value(time_value);
    return true;
}

attr_value_t ScSignalAccess::valueToAttrScTime(sc_core::sc_time value) const {
    sc_core::sc_time::value_type time_value = value.value();
    return valueToAttr(time_value);
}

bool ScSignalAccess::read(const sc_core::sc_object *obj,
                          attr_value_t *value) const {
    SIMICS_SYSTEMC_SIGNAL_READ(bool, obj, value, valueToAttr);
    SIMICS_SYSTEMC_SIGNAL_READ(int8_t, obj, value, valueToAttr);
    SIMICS_SYSTEMC_SIGNAL_READ(int16_t, obj, value, valueToAttr);
    SIMICS_SYSTEMC_SIGNAL_READ(int32_t, obj, value, valueToAttr);
    SIMICS_SYSTEMC_SIGNAL_READ(int64_t, obj, value, valueToAttr);
    SIMICS_SYSTEMC_SIGNAL_READ(uint8_t, obj, value, valueToAttr);
    SIMICS_SYSTEMC_SIGNAL_READ(uint16_t, obj, value, valueToAttr);
    SIMICS_SYSTEMC_SIGNAL_READ(uint32_t, obj, value, valueToAttr);
    SIMICS_SYSTEMC_SIGNAL_READ(uint64_t, obj, value, valueToAttr);
#ifndef _WIN32
    SIMICS_SYSTEMC_SIGNAL_READ(sc_core::sc_time, obj, value, valueToAttrScTime);
    // Bug 24887
#else
    __SIMICS_SYSTEMC_SIGNAL_READ_BASE(sc_core::sc_signal<sc_core::sc_time>,
                                      sc_core::sc_time, obj,
                                      value, valueToAttrScTime);
#endif
    SIMICS_SYSTEMC_SIGNAL_READ(double, obj, value, valueToAttr);

    return false;
}

bool ScSignalAccess::write(sc_core::sc_object *obj,
                           const attr_value_t *value) const {
    SIMICS_SYSTEMC_SIGNAL_WRITE(bool, obj, value, attrToValue);
    SIMICS_SYSTEMC_SIGNAL_WRITE(int8_t, obj, value, attrToValue);
    SIMICS_SYSTEMC_SIGNAL_WRITE(int16_t, obj, value, attrToValue);
    SIMICS_SYSTEMC_SIGNAL_WRITE(int32_t, obj, value, attrToValue);
    SIMICS_SYSTEMC_SIGNAL_WRITE(int64_t, obj, value, attrToValue);
    SIMICS_SYSTEMC_SIGNAL_WRITE(uint8_t, obj, value, attrToValue);
    SIMICS_SYSTEMC_SIGNAL_WRITE(uint16_t, obj, value, attrToValue);
    SIMICS_SYSTEMC_SIGNAL_WRITE(uint32_t, obj, value, attrToValue);
    SIMICS_SYSTEMC_SIGNAL_WRITE(uint64_t, obj, value, attrToValue);
#ifndef _WIN32
    SIMICS_SYSTEMC_SIGNAL_WRITE(sc_core::sc_time, obj, value,
                                attrToValueScTime);
    // Bug 24887
#else
    __SIMICS_SYSTEMC_SIGNAL_WRITE_BASE(sc_core::sc_signal<sc_core::sc_time>,
                                       sc_core::sc_time, obj, value,
                                       attrToValueScTime);
#endif
    SIMICS_SYSTEMC_SIGNAL_WRITE(double, obj, value, attrToValue);

    return false;
}


}  // namespace systemc
}  // namespace simics
