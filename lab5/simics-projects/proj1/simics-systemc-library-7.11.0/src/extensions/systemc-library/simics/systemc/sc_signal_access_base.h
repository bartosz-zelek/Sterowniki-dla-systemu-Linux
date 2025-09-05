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

#ifndef SIMICS_SYSTEMC_SC_SIGNAL_ACCESS_BASE_H
#define SIMICS_SYSTEMC_SC_SIGNAL_ACCESS_BASE_H

#include <simics/systemc/registry.h>
#include <simics/systemc/sc_signal_access_interface.h>

namespace simics {
namespace systemc {

#define __SIMICS_SYSTEMC_SIGNAL_DECLARE_TRANSFORM(type)        \
bool attrToValue(const attr_value_t *attr, type *value) const; \
attr_value_t valueToAttr(type value) const;

#define __SIMICS_SYSTEMC_SIGNAL_COMMA ,

#define __SIMICS_SYSTEMC_SIGNAL_WRITE_BASE(signal, type, obj, \
                                           value, set_func)   \
do {                                                          \
    signal *s = dynamic_cast<signal *>(obj);                  \
    if (s) {                                                  \
        type v;                                               \
        if (set_func(value, &v)) {                            \
            *s = v;                                           \
            return true;                                      \
        }                                                     \
    }                                                         \
} while (0)

#define SIMICS_SYSTEMC_SIGNAL_WRITE(type, obj, value, set_func)      \
__SIMICS_SYSTEMC_SIGNAL_WRITE_BASE(sc_core::sc_signal<type>,         \
                                   type, obj, value, set_func);      \
__SIMICS_SYSTEMC_SIGNAL_WRITE_BASE(sc_core::sc_signal<type           \
                                       __SIMICS_SYSTEMC_SIGNAL_COMMA \
                                       sc_core::SC_MANY_WRITERS>,    \
                                   type, obj, value, set_func);      \
__SIMICS_SYSTEMC_SIGNAL_WRITE_BASE(sc_core::sc_out<type>,            \
                                   type, obj, value, set_func);      \
__SIMICS_SYSTEMC_SIGNAL_WRITE_BASE(sc_core::sc_inout<type>,          \
                                   type, obj, value, set_func);

#define __SIMICS_SYSTEMC_SIGNAL_READ_BASE(signal, type, obj, \
                                          value, get_func)   \
do {                                                         \
    const signal *s = dynamic_cast<const signal *>(obj);     \
    if (s) {                                                 \
        *value = get_func(s->read());                        \
        return true;                                         \
    }                                                        \
} while (0)

#define SIMICS_SYSTEMC_SIGNAL_READ(type, obj, value, get_func)      \
__SIMICS_SYSTEMC_SIGNAL_READ_BASE(sc_core::sc_signal<type>,         \
                                  type, obj, value, get_func);      \
__SIMICS_SYSTEMC_SIGNAL_READ_BASE(sc_core::sc_signal<type           \
                                      __SIMICS_SYSTEMC_SIGNAL_COMMA \
                                      sc_core::SC_MANY_WRITERS>,    \
                                  type, obj, value, get_func);      \
__SIMICS_SYSTEMC_SIGNAL_READ_BASE(sc_core::sc_in<type>,             \
                                  type, obj, value, get_func);      \
__SIMICS_SYSTEMC_SIGNAL_READ_BASE(sc_core::sc_inout<type>,          \
                                  type, obj, value, get_func);

class ScSignalAccessBase : public Registrant<ScSignalAccessInterface> {
  public:
    __SIMICS_SYSTEMC_SIGNAL_DECLARE_TRANSFORM(bool)
    __SIMICS_SYSTEMC_SIGNAL_DECLARE_TRANSFORM(int64_t)
    __SIMICS_SYSTEMC_SIGNAL_DECLARE_TRANSFORM(int32_t)
    __SIMICS_SYSTEMC_SIGNAL_DECLARE_TRANSFORM(int16_t)
    __SIMICS_SYSTEMC_SIGNAL_DECLARE_TRANSFORM(int8_t)
#ifndef _WIN32
    __SIMICS_SYSTEMC_SIGNAL_DECLARE_TRANSFORM(long long unsigned int)  // NOLINT
#endif
    __SIMICS_SYSTEMC_SIGNAL_DECLARE_TRANSFORM(uint64_t)
    __SIMICS_SYSTEMC_SIGNAL_DECLARE_TRANSFORM(uint32_t)
    __SIMICS_SYSTEMC_SIGNAL_DECLARE_TRANSFORM(uint16_t)
    __SIMICS_SYSTEMC_SIGNAL_DECLARE_TRANSFORM(uint8_t)
    __SIMICS_SYSTEMC_SIGNAL_DECLARE_TRANSFORM(double)
    __SIMICS_SYSTEMC_SIGNAL_DECLARE_TRANSFORM(const char *)
};

}  // namespace systemc
}  // namespace simics

#endif
