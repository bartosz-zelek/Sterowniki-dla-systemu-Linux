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

#include "sample-linux-tracker-params.h"
#include "sample-linux-tracker.h"

#define ADD_DEFAULT_PARAM(field, default_value)                               \
        params->field = default_value;

params_t *
default_parameters()
{
        params_t *params = MM_MALLOC(1, params_t);
        FOR_PARAMS(ADD_DEFAULT_PARAM);
        params->name = MM_STRDUP("Linux");
        return params;
}

#undef ADD_DEFAULT_PARAM

#define ADD_PARAM_TO_DICT(field, default_value)                               \
        SIM_attr_dict_resize(&dict, index + 1);                               \
        SIM_attr_dict_set_item(&dict, index, SIM_make_attr_string(#field),    \
                               SIM_make_attr_uint64(params->field));          \
        index++;

attr_value_t
attr_dict_from_params(params_t *params)
{
        attr_value_t dict = SIM_alloc_attr_dict(0);
        int index = 0;
        FOR_PARAMS(ADD_PARAM_TO_DICT);
        SIM_attr_dict_resize(&dict, index + 1);
        SIM_attr_dict_set_item(&dict, index, SIM_make_attr_string("name"),
                               SIM_make_attr_string(params->name));
        return dict;
}

#undef ADD_PARAM_TO_DICT

#define ADD_PARAM_FROM_DICT(field, default_value)        \
        if (strcmp(key, #field) == 0)                    \
                params->field = value;

#define PARAM_IS_SET(field, default_value)       \
        if (params->field == 0)                  \
                all_params_set = false;

/* This function only does very basic verification of the parameters. */
params_t *
attr_dict_to_params(attr_value_t attr_params)
{
        bool name_set = false;
        params_t *params = MM_ZALLOC(1, params_t);
        for (int i = 0; i < SIM_attr_dict_size(attr_params); i++) {
                const char *key = SIM_attr_string(
                        SIM_attr_dict_key(attr_params, i));
                attr_value_t elem = SIM_attr_dict_value(attr_params, i);
                if (strcmp(key, "name") == 0 && SIM_attr_is_string(elem)) {
                        name_set = true;
                        params->name = MM_STRDUP(SIM_attr_string(elem));
                } else {
                        if (!SIM_attr_is_uint64(elem)) {
                                free_params(params);
                                return NULL;
                        }
                        uint64 value = SIM_attr_integer(
                                SIM_attr_dict_value(attr_params, i));
                        FOR_PARAMS(ADD_PARAM_FROM_DICT);
                }
        }
        bool all_params_set = name_set;
        FOR_PARAMS(PARAM_IS_SET);
        if (!all_params_set) {
                free_params(params);
                return NULL;
        }
        return params;
}

#undef ADD_PARAM_FROM_DICT
#undef PARAM_IS_SET

void
free_params(params_t *params)
{
        MM_FREE(params->name);
        MM_FREE(params);
}
