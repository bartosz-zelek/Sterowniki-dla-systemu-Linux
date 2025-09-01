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

#ifndef SIMICS_SIMULATOR_CONF_OBJECT_H
#define SIMICS_SIMULATOR_CONF_OBJECT_H

#include <simics/base/types.h>
#include <simics/base/attr-value.h>
#include <simics/base/conf-object.h>

#include <simics/simulator/sim-get-class.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* Get attribute attribute. */
EXPORTED attr_attr_t SIM_get_attribute_attributes(conf_class_t *NOTNULL cls,
                                                  const char *NOTNULL attr);

/* return 1 if the class has an attribute with name attr */
EXPORTED bool SIM_class_has_attribute(conf_class_t *NOTNULL cls,
                                      const char *NOTNULL attr);

EXPORTED conf_class_t *SIM_class_port(const conf_class_t *NOTNULL cls,
                                      const char *NOTNULL portname);

EXPORTED attr_value_t VT_get_port_classes(conf_class_t *NOTNULL cls);
EXPORTED const char *VT_get_port_obj_desc(conf_class_t *NOTNULL cls,
                                          const char *NOTNULL name);

EXPORTED const char *VT_get_class_description(const conf_class_t *NOTNULL cls);
EXPORTED const char *VT_get_class_short_desc(const conf_class_t *NOTNULL cls);

EXPORTED class_kind_t VT_get_class_kind(const conf_class_t *NOTNULL cls);


/* Get object (returns NULL if object not found). */
EXPORTED conf_object_t *SIM_get_object(const char *NOTNULL name);
EXPORTED conf_object_t *VT_get_object_by_name(const char *NOTNULL name);

/* Delete object. */
EXPORTED int SIM_delete_object(conf_object_t *NOTNULL obj);
EXPORTED int SIM_delete_objects(attr_value_t object_list);

/* Get attribute from object. */
EXPORTED attr_value_t SIM_get_attribute(conf_object_t *NOTNULL obj,
                                        const char *NOTNULL name);
EXPORTED attr_value_t SIM_get_attribute_idx(conf_object_t *NOTNULL obj,
                                            const char *NOTNULL name,
                                            attr_value_t *NOTNULL index);

/* Get attribute from class. */
EXPORTED attr_value_t SIM_get_class_attribute(conf_class_t *NOTNULL cls,
                                              const char *NOTNULL name);
EXPORTED attr_value_t SIM_get_class_attribute_idx(conf_class_t *NOTNULL cls,
                                                  const char *NOTNULL name,
                                                  attr_value_t *NOTNULL index);

/* Set attribute in object. Returns error code on failure (0 == ok). */
EXPORTED set_error_t SIM_set_attribute(conf_object_t *NOTNULL obj,
                                       const char *NOTNULL name,
                                       attr_value_t *NOTNULL value);

EXPORTED set_error_t SIM_set_attribute_idx(conf_object_t *NOTNULL obj,
                                           const char *NOTNULL name,
                                           attr_value_t *NOTNULL index,
                                           attr_value_t *NOTNULL value);

EXPORTED set_error_t SIM_set_attribute_default(conf_object_t *NOTNULL obj,
                                               const char *NOTNULL name,
                                               attr_value_t value);

/* Set attribute in class. Returns error code on failure (0 == ok). */
EXPORTED set_error_t SIM_set_class_attribute(conf_class_t *NOTNULL cls,
                                             const char *NOTNULL name,
                                             attr_value_t *NOTNULL value);

EXPORTED set_error_t SIM_set_class_attribute_idx(conf_class_t *NOTNULL cls,
                                                 const char *NOTNULL name,
                                                 attr_value_t *NOTNULL index,
                                                 attr_value_t *NOTNULL value);

EXPORTED attr_value_t VT_get_attributes(conf_class_t *NOTNULL cls);
EXPORTED attr_value_t VT_get_interfaces(conf_class_t *NOTNULL cls);
EXPORTED attr_value_t VT_get_port_interfaces(conf_class_t *NOTNULL cls);
EXPORTED attr_value_t VT_get_class_extensions(conf_class_t *NOTNULL cls);

EXPORTED const char *VT_get_attribute_type(conf_class_t *NOTNULL cls,
                                           const char *NOTNULL attr);

EXPORTED void VT_rename_object(conf_object_t *NOTNULL obj,
                               const char *NOTNULL newname);

EXPORTED void VT_set_object_checkpointable(conf_object_t *NOTNULL obj,
                                           bool checkpointable);
EXPORTED bool VT_object_checkpointable(conf_object_t *NOTNULL obj);

EXPORTED void VT_set_delete_protection(conf_object_t *NOTNULL obj, bool on);

EXPORTED void VT_add_permanent_object(conf_object_t *NOTNULL obj);

EXPORTED conf_object_t *SIM_create_object(conf_class_t *NOTNULL cls,
                                          const char *name,
                                          attr_value_t attrs);

/* Return vector with all objects. */
EXPORTED attr_value_t SIM_get_all_objects(void);
EXPORTED attr_value_t SIM_get_all_classes(void);

/* Return vector with objects implementing all the ifaces */
EXPORTED attr_value_t VT_get_all_objects_impl(attr_value_t ifaces);
EXPORTED attr_value_t VT_get_all_instances(conf_class_t *NOTNULL cls); 

EXPORTED int DBG_check_typing_system(const char *type,
                                     attr_value_t *NOTNULL val);

#if defined(__cplusplus)
}
#endif

#endif
