// -*- mode: C++; c-file-style: "virtutech-c++" -*-

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

#ifndef SIMICS_SYSTEMC_INTERNALS_H
#define SIMICS_SYSTEMC_INTERNALS_H

#include <simics/cc-api.h>  // conf_object_t, conf_class_t, attr_value_t

// TODO(ah): make sure all use of internals go through this file
// TODO(ah): update this now that MB and EB are part of Base

extern "C" {
extern void SIM_break_simulation(const char *msg);
extern conf_object_t *SIM_create_object(conf_class_t *NOTNULL cls,
                                        const char *name, attr_value_t attrs);
extern int SIM_delete_object(conf_object_t *NOTNULL obj);
extern attr_value_t SIM_get_attribute(conf_object_t *NOTNULL obj,
                                      const char *name);
extern conf_object_t *SIM_get_object(const char *NOTNULL name);
extern bool SIM_get_init_arg_boolean(const char *NOTNULL name,
                                     bool default_value);
extern bool SIM_simics_is_running();
extern void SIM_thread_safe_callback(void (*NOTNULL f)(lang_void *data),
                                     lang_void *data);

extern void VT_set_delete_protection(conf_object_t *NOTNULL obj, bool on);
extern int SIM_write(const void *NOTNULL src, int length);

extern void VT_register_thread();
extern char *VT_generate_object_name();

extern conf_class_t *SIM_get_class(const char *NOTNULL name);
}  // extern "C"

#endif  // SIMICS_SYSTEMC_INTERNALS_H
