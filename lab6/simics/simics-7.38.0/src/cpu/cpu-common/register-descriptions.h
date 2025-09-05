/*
  © 2011 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef CPU_COMMON_REGISTER_DESCRIPTIONS_H
#define CPU_COMMON_REGISTER_DESCRIPTIONS_H

#include <simics/model-iface/describe-registers.h>

#if defined(__cplusplus)
extern "C" {
#endif

struct wrap_description {
        description_t desc;

        /* Named values for the register or field.
           Terminated by a value with name == NULL. */
        const named_value_t *named_values;

        /* Children of the description.
           Terminated by a description with name == NULL. */
        const struct wrap_description *members;
};

/* common_first_child can only be used with parent != NULL. you need some way
   of finding the top level yourself. */
const description_t *common_first_child(
        conf_object_t *NOTNULL obj, const description_t *parent);
const description_t *common_next_description(
        conf_object_t *NOTNULL obj, const description_t *prev);
void common_free_description(conf_object_t *NOTNULL obj,
                             const description_t *desc);
const named_value_t *common_first_named_value(
        conf_object_t *NOTNULL obj, const description_t *parent);
const named_value_t *common_next_named_value(
        conf_object_t *NOTNULL obj, const named_value_t *prev);
void common_free_named_value(conf_object_t *NOTNULL obj,
                             const named_value_t *nv);

#if defined(__cplusplus)
}
#endif

#endif
