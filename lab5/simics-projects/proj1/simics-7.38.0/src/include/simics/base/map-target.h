/*
  © 2015 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_BASE_MAP_TARGET_H
#define SIMICS_BASE_MAP_TARGET_H

#include <simics/base/memory.h>
#include <simics/base/direct-memory.h>
#include <simics/base/memory-transaction.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add id="device api types">
   <name index="true">map_target_t</name>
   <insert id="map_target_t DOC"/>
   </add> */

/* <add id="map_target_t DOC">
   <ndx>map_target_t</ndx>
   <name index="true">map_target_t</name>
   <doc>
   <di name="NAME">map_target_t</di>
   <di name="SYNOPSIS">
   <smaller><insert id="map_target_t def"/></smaller>
   </di>
   <di name="DESCRIPTION">
   A map target can be viewed as an opaque representation of
   an object/interface pair which can function either as an endpoint
   for a memory transaction or as an address space where a
   memory transaction can be performed. To create a <type>map_target_t</type>
   object one should use the <fun>SIM_new_map_target</fun> function.
   The <fun>SIM_free_map_target</fun> function frees
   a <type>map_target_t</type> object. In order to get better performance,
   it is better to allocate a map target once and reuse it rather
   than to allocate and free it every time.

   Examples of map targets include IO banks, RAM, ROM, memory spaces,
   port spaces, translators and bridges.

   For certain targets, e.g. bridges or translators, the map target also
   holds information about a chained, or default, target.
   </di>

   <di name="PYTHON SPECIFICS">
   In Python, it is allowed to use arguments of the <type>conf_object_t</type>
   type with Simics API functions that have parameters of
   the <type>map_target_t</type> type. The arguments of
   the <type>conf_object_t</type> type will be converted to
   <type>map_target_t</type> values automatically, via the call to
   the <fun>SIM_new_map_target</fun> function. Here is an example where
   <param>memory_space</param> is a Simics object, i.e. has
   the <type>conf_object_t</type> type:
   <pre>
   t = transaction_t(...)
   SIM_issue_transaction(memory_space, t, addr)</pre>

   In Python, the objects of the <type>map_target_t</type> type have
   read-only <tt>obj</tt>, <tt>port</tt>, and <tt>target</tt> attributes.
   These attributes correspond to the arguments given to
   <fun>SIM_new_map_target</fun> that created
   a <type>map_target_t</type> object.
   </di>

   <di name="SEE ALSO">
   <fun>SIM_new_map_target</fun>, <fun>SIM_free_map_target</fun>,
   <fun>SIM_map_target_object</fun>, <fun>SIM_map_target_port</fun>,
   <fun>SIM_map_target_target</fun>, <iface>translator_interface_t</iface>
   </di>
   </doc>
   </add> */

/* <add-type id="map_target_t def"></add-type> */
typedef struct map_target map_target_t;

EXPORTED map_target_t *SIM_new_map_target(conf_object_t *NOTNULL obj,
                                          const char *port,
                                          const map_target_t *chained_target);
EXPORTED void SIM_free_map_target(map_target_t *mt);

EXPORTED conf_object_t *SIM_map_target_object(
        const map_target_t *NOTNULL mt);
EXPORTED const char *SIM_map_target_port(
        const map_target_t *NOTNULL mt);
EXPORTED const map_target_t *SIM_map_target_target(
        const map_target_t *NOTNULL mt);

EXPORTED exception_type_t VT_map_target_access(const map_target_t *NOTNULL mt,
                                               generic_transaction_t *mop);

EXPORTED direct_memory_lookup_t VT_map_target_dm_lookup(
        const map_target_t *NOTNULL mt, conf_object_t *requester,
        physical_address_t addr, uint64 size, access_t access);

EXPORTED bool SIM_map_target_flush(const map_target_t *NOTNULL mt,
                                   uint64 base, uint64 size, access_t access);

#if defined(__cplusplus)
}
#endif

#endif   /* SIMICS_BASE_MAP_TARGET_H */
