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

#ifndef SIMPLE_CACHE_INTERFACE_H
#define SIMPLE_CACHE_INTERFACE_H

#include <simics/base/time.h>
#include <simics/arch/x86.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define FOR_ACCESS_TYPE(op)                     \
        op(Write)                               \
        op(Write_Back)                          \
        op(Read)                                \
        op(Read_For_Ownership)                  \
        op(Execute)                             \
        op(Prefetch)                            \
        op(Prefetch_Read_For_Ownership)         \
        op(Num)

#define ACCESS_TYPE_ENUM(e) \
        Access_Type_ ## e,

typedef enum {
        FOR_ACCESS_TYPE(ACCESS_TYPE_ENUM)
} access_type_t;

typedef enum {
        Bus_Op_Rd,
        Bus_Op_Rd_X,
} bus_op_t;

typedef enum {
        Cache_Propagate_None,
        Cache_Propagate_Next,
        Cache_Propagate_Prev
} cache_propagate_t;

SIM_INTERFACE(simple_cache) {
        cycles_t (*read)(conf_object_t *obj, conf_object_t *initiator,
                         ini_type_t ini_type,
                         physical_address_t pa, unsigned size,
                         access_type_t type);
        cycles_t (*write)(conf_object_t *obj, conf_object_t *initiator,
                          ini_type_t ini_type,
                          physical_address_t pa, unsigned size,
                          access_type_t access);
        cycles_t (*mesi_snoop)(conf_object_t *obj, physical_address_t pa,
                            bus_op_t busop);
        cycles_t (*invalidate)(conf_object_t *obj, physical_address_t pa,
                               cache_propagate_t propagate);
        cycles_t (*invalidate_all)(conf_object_t *obj,
                                   cache_propagate_t propagate);
};

#define SIMPLE_CACHE_INTERFACE "simple_cache"

#if defined(__cplusplus)
}
#endif
        
#endif
