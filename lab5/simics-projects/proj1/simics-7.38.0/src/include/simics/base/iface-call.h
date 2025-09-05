/*
  © 2016 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_BASE_IFACE_CALL_H
#define SIMICS_BASE_IFACE_CALL_H

#include <simics/base/types.h>

#if defined(__cplusplus)
extern "C" {
#endif
        
/* convenience functions to make it easier to use interfaces */
#ifndef PYWRAP
static inline bool _priv_iface_as_bool(const void *i) { return (bool)i; }

#define IFACE_OBJ(name) \
        struct { conf_object_t *obj; const name##_interface_t *i; }

#define CALL(x, method) \
        ((x).i->method)((x).obj CALL_PRIV_

#define CALL_PRIV_(...) \
        , ## __VA_ARGS__)

#define INIT_IFACE(x, iface, _obj)                                        \
        ((x)->obj = (_obj),                                               \
         (x)->i = (x)->obj ? SIM_C_GET_INTERFACE((x)->obj, iface) : NULL, \
         _priv_iface_as_bool((x)->i))

#define CLEAR_IFACE(x)                         \
        ((x)->obj = NULL,                       \
         (x)->i = NULL)

#define GET_IFACE_OBJ(x) \
        ((x).obj)

#define HAS_IFACE(x) \
        _priv_iface_as_bool((x).i)

#define INIT_REQUIRED_IFACE(...) \
        ASSERT(INIT_IFACE(__VA_ARGS__))

#endif

#if defined(__cplusplus)
}
#endif

#endif
