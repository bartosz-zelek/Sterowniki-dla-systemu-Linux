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

#ifndef SIMICS_SIMULATOR_IFACE_ADDRESS_PROFILER_H
#define SIMICS_SIMULATOR_IFACE_ADDRESS_PROFILER_H

#include <simics/base/types.h>
#include <simics/simulator/address-profiler.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add id="address_profiler_interface_t">

   Interface for getting statistics out of profilers. The target is some kind
   of profiler whose data can be meaningfully viewed as counts per address.

   The function <fun>num_views</fun> returns the number <math>k</math> of
   different ways you can view the data of this object. The view selection
   parameter <param>view</param> to all other functions in the interface
   accepts values between 0 and <math>k - 1</math>.

   <fun>description</fun> returns a short string that explains what the data
   means. <fun>physical_addresses</fun> returns true if the profiler works with
   physical addresses, or false if it uses virtual addresses.
   <fun>address_bits</fun> returns the number of bits in an address.

   <fun>granularity_log2</fun> returns the base 2 logarithm of the size, in
   bytes, of the address intervals that the counters are associated to. For
   example, if the data is instruction execution count and each instruction is
   4 bytes long, one would expect the granularity to be at least 4 bytes since
   that is the smallest interval containing a whole instruction (but it might
   be more, if the profiler is less fine-grained for some reason). And for a
   4-byte granularity, <fun>granularity_log2</fun> would return 2.

   <fun>sum</fun> returns the sum of all counters between <param>start</param>
   and <param>stop</param>, inclusive. <fun>max</fun> returns the maximum value
   of any counter in the range.

   <fun>iter</fun> returns an address profile iterator that will visit all
   nonzero counters in the range precisely once, in some order. In C, you can
   use the functions <fun>SIM_iter_next</fun>, <fun>SIM_iter_addr</fun> and
   <fun>SIM_iter_free</fun> to operate the iterator. In Python, it works just
   like any other iterator, and returns (count, address) pairs. Note that you
   may not continue to use the iterator after the underlying address profiler
   has been modified.

   <insert-until text="// ADD INTERFACE address_profiler_interface"/>
   </add>
   <add id="address_profiler_interface_exec_context">
   Cell Context for all methods.
   </add>
*/
SIM_INTERFACE(address_profiler) {
        addr_prof_iter_t *(*iter)(conf_object_t *prof_obj, unsigned view,
                                  generic_address_t start,
                                  generic_address_t stop);
        uint64 (*sum)(conf_object_t *prof_obj, unsigned view,
                      generic_address_t start, generic_address_t end);
        uint64 (*max)(conf_object_t *prof_obj, unsigned view,
                      generic_address_t start, generic_address_t end);
        unsigned (*granularity_log2)(conf_object_t *prof_obj, unsigned view);
        int (*address_bits)(conf_object_t *prof_obj, unsigned view);
        int (*physical_addresses)(conf_object_t *prof_obj, unsigned view);
        const char *(*description)(conf_object_t *prof_obj, unsigned view);
        unsigned (*num_views)(conf_object_t *prof_obj);
};

#define ADDRESS_PROFILER_INTERFACE "address_profiler"
// ADD INTERFACE address_profiler_interface

/* <add id="branch_arc_interface_t">
   <ndx>branch_arc_iter_t</ndx>
   <ndx>branch_arc_t</ndx>
   <ndx>branch_arc_type_t</ndx>
   <ndx>branch_recorder_direction_t</ndx>

   Interface for getting branch arcs out profilers. The target is some kind of
   profiler whose data can be meaningfully viewed as branch arcs (usually a
   branch profiler).

   <fun>iter</fun> returns a branch arc iterator that will visit all branch
   arcs in the range precisely once, in order of selected address (to or from,
   selected with <param>dir</param>), other address and type. In Python, it
   works just like any other iterator, and returns (from, to, counter, type)
   tuples. Note that you may not continue to use the iterator after the
   underlying profiler has been modified.

  <type>branch_arc_type_t</type> defines the branch types returned by a branch
  arc iterator.

  <dl>
  <dt><tt>Branch_Arc_Branch</tt></dt> <dd>Normal branch operation</dd> 
  <dt><tt>Branch_Arc_Exception</tt></dt> <dd>Branch because an exception 
  was encountered</dd>
  <dt><tt>Branch_Arc_Exception_Return</tt></dt> <dd>Branch to finish an 
  exception handler</dd>
  </dl>

   <insert id="branch_arc_type_t def"/>
   <insert id="branch_recorder_direction_t def"/>
   <insert-until text="// ADD INTERFACE branch_arc_interface"/>

   </add>
   <add id="branch_arc_interface_exec_context">
   Cell Context for all methods.
   </add>
*/

SIM_INTERFACE(branch_arc) {
        branch_arc_iter_t *(*iter)(conf_object_t *prof_obj,
                                   generic_address_t start,
                                   generic_address_t stop,
                                   branch_recorder_direction_t dir);
};

#define BRANCH_ARC_INTERFACE "branch_arc"
// ADD INTERFACE branch_arc_interface

#if defined(__cplusplus)
}
#endif

#endif
