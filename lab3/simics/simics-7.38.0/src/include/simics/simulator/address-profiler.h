/*
  © 2010 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SIMULATOR_ADDRESS_PROFILER_H
#define SIMICS_SIMULATOR_ADDRESS_PROFILER_H

#include <simics/base/types.h>
#include <simics/model-iface/branch-recorder.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define FOR_ALL_ADDRESS_TYPES(op)               \
        op(Invalid),                            \
        op(Physical),                           \
        op(Virtual)
#define ADDRESS_TYPE_ENUM_OP(s) Addr_Type_ ## s
typedef enum {
        FOR_ALL_ADDRESS_TYPES(ADDRESS_TYPE_ENUM_OP)
} addr_type_t;

/* A branch arc iterator will iterate over a specified portion of the to or
   from branch address space in order (direction address, then other address,
   then type), and return every branch arc with a given type exactly once. When
   done, it will return 0. */

#if !defined(PYWRAP) /* only for C */

/* <add id="experimental">

   <b>EXPERIMENTAL.</b> While this functionality is expected to be retained in
   future releases, the interface is likely to change.

   </add>
*/

/* <add-fun id="simics api profiling">

   <insert id="experimental"/>

   <short>Iterate over address profile counters</short>

   An address profile iterator visits some of the counters of an address
   profiler in some order. It is obtained from the <fun>iter</fun> function of
   the <iface>address_profiler</iface> interface.

   <fun>SIM_iter_next</fun> advances the address profile iterator
   <param>iter</param> to the next nonzero counter and returns the count. It
   will return 0 when there are no more counters to visit. Note that the order
   in which the counters are visited is unspecified.

   <fun>SIM_iter_addr</fun> returns the address of the counter returned by the
   most recent call to <fun>iter_next</fun>.

   When you are done with the iterator, deallocate it with
   <fun>SIM_iter_free</fun>.

   <di name="EXECUTION CONTEXT">Cell Context</di>
   </add-fun>
*/
FORCE_INLINE uint64
SIM_iter_next(addr_prof_iter_t *iter)
{
        return iter->next(iter);
}

/* <append-fun id="SIM_iter_next"/> */
FORCE_INLINE generic_address_t
SIM_iter_addr(addr_prof_iter_t *iter)
{
        return iter->addr;
}

/* <append-fun id="SIM_iter_next"/> */
FORCE_INLINE void
SIM_iter_free(addr_prof_iter_t *iter)
{
        iter->destroy(iter);
}

#endif /* !PYWRAP */

#if defined(__cplusplus)
}
#endif

#endif
