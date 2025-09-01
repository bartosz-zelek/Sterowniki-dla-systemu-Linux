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

#include "../cpu-common/register-descriptions.h"
#include <simics/simulator-api.h>
#include <stddef.h>

static const struct wrap_description *
find_wrapper(const description_t *d)
{
        return (const struct wrap_description *)(
                (char *)d - offsetof(struct wrap_description, desc));
}

const description_t *
common_first_child(conf_object_t *NOTNULL obj,
                   const description_t *parent)
{
        ASSERT(parent != NULL);
        const struct wrap_description *wd = find_wrapper(parent);
        const struct wrap_description *wm = wd->members;
        if (wm == NULL) return NULL;
        if (wm->desc.name == NULL) return NULL;
        return &wm->desc;
}

const description_t *
common_next_description(conf_object_t *NOTNULL obj,
                        const description_t *prev)
{
        ASSERT(prev != NULL);
        const struct wrap_description *wp = find_wrapper(prev);
        const struct wrap_description *wn = wp + 1;
        if (wn->desc.name == NULL) return NULL;

        /* Older tcf-agent crashes on registers wider than XMM, hide them */
        static int build_id = 0;
        if (!build_id) {
                build_id = SIM_attr_integer(
                        SIM_get_attribute(SIM_get_object("sim"), "build_id"));
        }
        if (build_id < 5150 && !strncmp(wn->desc.name, "YMM", 4) ) {
                return NULL;
        }

        return &wn->desc;
}

void
common_free_description(conf_object_t *NOTNULL obj,
                        const description_t *desc)
{
        /* static data, nothing to free */
}

const named_value_t *
common_first_named_value(conf_object_t *NOTNULL obj,
                         const description_t *parent)
{
        ASSERT(parent != NULL);
        const struct wrap_description *wd = find_wrapper(parent);
        const named_value_t *nv = wd->named_values;
        if (nv == NULL || nv->name == NULL) return NULL;
        return nv;
}

const named_value_t *
common_next_named_value(conf_object_t *NOTNULL obj,
                        const named_value_t *prev)
{
        ASSERT(prev != NULL);
        const named_value_t *next = prev + 1;
        if (next->name == NULL) return NULL;
        return next;
}

void
common_free_named_value(conf_object_t *NOTNULL obj,
                        const named_value_t *nv)
{
        /* static data, nothing to free */
}
