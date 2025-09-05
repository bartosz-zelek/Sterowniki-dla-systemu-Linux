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

#ifndef SIMICS_SIMULATOR_HAP_CONSUMER_H
#define SIMICS_SIMULATOR_HAP_CONSUMER_H

#include <simics/base/types.h>
#include <simics/base/attr-value.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add-type id="hap_handle_t def"><ndx>hap_handle_t</ndx></add-type> */
typedef int hap_handle_t;

#if defined (PYWRAP) || defined(GULP_EXTERN_DECL) || defined(GULP_API_HELP) || defined(MAKE_GULP_HAPPY) || (!defined(__cplusplus) && __STDC_VERSION__ < 202000)
typedef void (*obj_hap_func_t)();
#else
typedef void (*obj_hap_func_t)(...);
#endif

EXPORTED attr_value_t SIM_get_all_hap_types(void);

EXPORTED hap_handle_t SIM_hap_add_callback(
        const char *NOTNULL hap, NOTNULL obj_hap_func_t func, lang_void *data);
EXPORTED hap_handle_t SIM_hap_add_callback_index(
        const char *NOTNULL hap, NOTNULL obj_hap_func_t func,
        lang_void *data, int64 index);
EXPORTED hap_handle_t SIM_hap_add_callback_range(
        const char *NOTNULL hap, NOTNULL obj_hap_func_t func,
        lang_void *data, int64 start, int64 end);
EXPORTED hap_handle_t SIM_hap_add_callback_obj(
        const char *NOTNULL hap, conf_object_t *NOTNULL obj, int flags,
        NOTNULL obj_hap_func_t func, lang_void *data);
EXPORTED hap_handle_t SIM_hap_add_callback_obj_index(
        const char *NOTNULL hap, conf_object_t *NOTNULL obj,
        int flags, NOTNULL obj_hap_func_t func,
        lang_void *data, int64 index);
EXPORTED hap_handle_t SIM_hap_add_callback_obj_range(
        const char *NOTNULL hap, conf_object_t *NOTNULL obj,
        int flags, NOTNULL obj_hap_func_t func,
        lang_void *data, int64 start, int64 end);

EXPORTED void SIM_hap_delete_callback(
        const char *NOTNULL hap, NOTNULL obj_hap_func_t func, lang_void *data);
EXPORTED void SIM_hap_delete_callback_obj(
        const char *NOTNULL hap, conf_object_t *NOTNULL obj,
        NOTNULL obj_hap_func_t func, lang_void *data);
EXPORTED void SIM_hap_delete_callback_id(
        const char *NOTNULL hap, hap_handle_t handle);
EXPORTED void SIM_hap_delete_callback_obj_id(
        const char *NOTNULL hap, conf_object_t *NOTNULL obj,
        hap_handle_t handle);

#if defined(__cplusplus)
}
#endif

#endif
