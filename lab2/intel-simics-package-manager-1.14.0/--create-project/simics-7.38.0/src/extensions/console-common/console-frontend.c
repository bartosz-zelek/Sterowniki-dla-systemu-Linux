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

#include "console-frontend.h"
#include <simics/util/hashtab.h>
#include <simics/util/strbuf.h>
#include <simics/simulator/conf-object.h>
#include <simics/simulator-iface/preferences.h>

// Maps frontend name to frontend class.
static ht_str_table_t frontends;

#define DEFAULT_FRONTEND "winsome"

void
init_console_frontends()
{
        if (ht_num_entries_str(&frontends) == 0)
                ht_init_str_table(&frontends, true);
}

void
register_console_frontend(const char *console, const char *name,
                          conf_class_t *frontend)
{
        ASSERT(console && name && frontend);
        strbuf_t key = sb_newf("%s-%s", console, name);
        ht_insert_str(&frontends, sb_detach(&key), frontend);
}


conf_object_t *
get_console_frontend(conf_object_t *obj, const char *port)
{
        ASSERT(obj && port);
        strbuf_t name = sb_newf("%s", port);
        conf_object_t *frontend = SIM_object_descendant(obj, sb_str(&name));
        sb_free(&name);
        return frontend;
}

// Lookup chosen frontend name from module preferences.
conf_class_t *
get_console_frontend_class(conf_object_t *obj, const char *console)
{
        ASSERT(obj && console);
        conf_object_t *prefs = SIM_get_object("prefs");
        ASSERT(prefs);
        SIM_require_object(prefs);
        const preference_interface_t *iface =
                SIM_C_GET_INTERFACE(prefs, preference);
        ASSERT(iface);
        attr_value_t frontend_name =
                iface->get_preference_for_module_key(
                        prefs, console, "frontend");
        conf_class_t *frontend;
        if (SIM_attr_is_string(frontend_name)) {
                strbuf_t key = sb_newf("%s-%s", console,
                                       SIM_attr_string(frontend_name));
                frontend = ht_lookup_str(&frontends, sb_str(&key));
                if (!frontend) {
                        sb_fmt(&key, "%s-%s", console, DEFAULT_FRONTEND);
                        frontend = ht_lookup_str(&frontends, sb_str(&key));
                }
                sb_free(&key);
        } else {
                strbuf_t key = sb_newf("%s-%s", console, DEFAULT_FRONTEND);
                frontend = ht_lookup_str(&frontends, sb_str(&key));
                sb_free(&key);
        }
        
        iface->set_preference_for_module_key(
                prefs, SIM_make_attr_string(SIM_get_class_name(frontend)),
                console, "frontend");
        return frontend;
}
