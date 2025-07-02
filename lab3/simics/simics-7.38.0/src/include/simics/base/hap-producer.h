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

#ifndef SIMICS_BASE_HAP_PRODUCER_H
#define SIMICS_BASE_HAP_PRODUCER_H

#include <stdarg.h>
#include <simics/base/types.h>
#include <simics/base/attr-value.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add id="device api types">
   <name index="true">hap_type_t</name>
   <insert id="hap_type_t DOC"/>
   </add> */

/* <add id="hap_type_t DOC">
   <ndx>hap_type_t</ndx>
   <name index="true">hap_type_t</name>
   <doc>
   <doc-item name="NAME">hap_type_t</doc-item>
   <doc-item name="SYNOPSIS"><insert id="hap_type_t def"/></doc-item>
   <doc-item name="DESCRIPTION">
   This data type is used to represent hap (occurrence) types. This is
   a runtime number that may change between different Simics
   invocations. Haps are normally identified by strings, but by
   calling <fun>SIM_hap_get_number()</fun>, a lookup from such a name
   to a <type>hap_type_t</type> can be made.</doc-item>

   <doc-item name="SEE ALSO">
     <fun>SIM_get_all_hap_types</fun>, 
     <fun>SIM_hap_get_number</fun>,
     <fun>SIM_hap_add_type</fun>
   </doc-item>
   </doc></add>
*/
/* <add-type id="hap_type_t def"></add-type>
 */
typedef int hap_type_t;

EXPORTED hap_type_t SIM_hap_add_type(
        const char *NOTNULL hap, const char *NOTNULL params,
        const char *param_desc, const char *index, const char *desc,
        int unused);
EXPORTED void SIM_hap_remove_type(const char *NOTNULL hap);

EXPORTED hap_type_t SIM_hap_get_number(const char *NOTNULL hap);
EXPORTED const char *SIM_hap_get_name(hap_type_t hap);

EXPORTED bool SIM_hap_is_active(hap_type_t hap);
EXPORTED bool SIM_hap_is_active_obj(
        hap_type_t hap, conf_object_t *NOTNULL obj);
EXPORTED bool SIM_hap_is_active_obj_idx(
        hap_type_t hap, conf_object_t *NOTNULL obj, int64 index);

#if !defined(PYWRAP) /* only for C */
EXPORTED int SIM_c_hap_occurred(hap_type_t hap, conf_object_t *obj,
                                int64 value, ...);
EXPORTED int SIM_c_hap_occurred_vararg(hap_type_t hap, conf_object_t *obj,
                                       int64 value, va_list ap);
EXPORTED int SIM_c_hap_occurred_always(hap_type_t hap, conf_object_t *obj,
                                       int64 value, ...);
EXPORTED int SIM_c_hap_occurred_always_vararg(
        hap_type_t hap, conf_object_t *obj, int64 value, va_list ap);
#endif /* !PYWRAP */

EXPORTED int SIM_hap_occurred(hap_type_t hap, conf_object_t *obj,
                              int64 value, attr_value_t *NOTNULL list);
EXPORTED int SIM_hap_occurred_always(
        hap_type_t hap, conf_object_t *obj,
        int64 value, attr_value_t *NOTNULL list);

EXPORTED attr_value_t VT_hap_global_callback_ranges(hap_type_t hap);

#if defined(__cplusplus)
}
#endif

#endif
