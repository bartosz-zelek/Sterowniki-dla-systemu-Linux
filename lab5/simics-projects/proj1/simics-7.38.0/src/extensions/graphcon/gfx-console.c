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

#include "gfx-console.h"
#include <simics/base/conf-object.h>
#include <simics/base/log.h>
#include <simics/base/notifier.h>
#include <simics/simulator/conf-object.h>
#include "gfx-break.h"
#include "input.h"
#include "recorder.h"
#include "output.h"
#include "vnc.h"
#include "break-strings.h"
#include "console-frontend.h"
#include "winsome-frontend.h"
#include "gfx-inline.h"

// Log group names. Order should match Gfx_Console_Log_*
static const char *const log_groups[] = {
        "main",
        "input",
        "recorder",
        "output",
        "gfx-break",
        "vnc",
        "break-strings",
        NULL,
};
STATIC_ASSERT(ALEN(log_groups) == Gfx_Console_Log_Num + 1);

// Defines frontend object name from console object name.
#define FRONTEND_PORT "frontend"
// Default frontend name.
#define DEFAULT_FRONTEND "winsome"
// Module preference key
#define PREFS_KEY "gfx-console"

// Return frontend handler object.
conf_object_t *
get_gfx_console_frontend(conf_object_t *obj)
{
        return get_console_frontend(obj, FRONTEND_PORT);
}

static conf_object_t *
alloc(conf_class_t *cl)
{
        gfx_console_t *con = MM_ZALLOC(1, gfx_console_t);
        return to_obj(con);
}

static void *
init(conf_object_t *obj)
{
        gfx_console_t *gc = from_obj(obj);

        gc->input_data = make_gfx_input(gc);
        gc->record_data = make_gfx_recording(gc);
        gc->output_data = make_gfx_output(gc, SIM_object_name(obj));
        gc->gfx_break_data = make_gfx_break(gc);
        gc->vnc_data = make_gfx_vnc(gc);

        gc->break_str_data = make_break_strings(obj, Gfx_Console_Log_Break_Str);

        return obj;
}

static void
pre_delete_instance(conf_object_t *unused, conf_object_t *obj, void *arg)
{
        gfx_console_t *gc = from_obj(obj);
        pre_delete_gfx_break(gc->gfx_break_data);
        pre_delete_gfx_input(gc->input_data);
        pre_delete_gfx_output(gc->output_data);
}

static void
finalize(conf_object_t *obj)
{
        gfx_console_t *gc = from_obj(obj);
        conf_class_t *frontend = get_console_frontend_class(
                obj, PREFS_KEY);
        strbuf_t name = sb_newf("%s.%s", SIM_object_name(obj), FRONTEND_PORT);
        conf_object_t *frontend_obj = SIM_create_object(
                frontend, sb_str(&name), SIM_make_attr_list(0));        
        ASSERT(frontend_obj);
        sb_free(&name);

        // Make sure screen data structure is valid.
        finalise_gfx_output(gc->output_data);
        finalise_gfx_input(gc->input_data);
        SIM_set_object_configured(obj);
        finalise_gfx_break(gc->gfx_break_data);
        SIM_add_notifier(obj, Sim_Notify_Object_Delete, NULL,
                         pre_delete_instance, NULL);
}

static void
dealloc(conf_object_t *obj)
{
        gfx_console_t *gc = from_obj(obj);
        MM_FREE(gc);
}

static void
deinit(conf_object_t *obj)
{
        gfx_console_t *gc = from_obj(obj);
        destroy_break_strings(gc->break_str_data);
        destroy_gfx_vnc(gc->vnc_data);
        destroy_gfx_break(gc->gfx_break_data);
        destroy_gfx_output(gc->output_data);
        destroy_gfx_recording(gc->record_data);
        destroy_gfx_input(gc->input_data);
}

static void
init_frontends()
{
        init_console_frontends();
        conf_class_t *winsome = init_gfx_frontend_winsome();
        ASSERT(winsome);
        register_console_frontend(PREFS_KEY, "winsome", winsome);
}

static console_break_data_t * 	 
break_string_data(conf_object_t *obj) 	 
{ 	 
        return from_obj(obj)->break_str_data; 	 
}

static int64
break_strings_add(conf_object_t *obj, const char *str,
                  break_string_cb_t cb, lang_void *arg)
{
        return bs_add(break_string_data(obj), str, cb, arg);
}

static int64
break_strings_add_single(conf_object_t *obj, const char *str,
                         break_string_cb_t cb, lang_void *arg)
{
        return bs_add_single(break_string_data(obj), str, cb, arg);
}

static int64
break_strings_add_regexp(conf_object_t *obj, const char *str,
                         break_string_cb_t cb, lang_void *arg)
{
        return bs_add_regexp(break_string_data(obj), str, cb, arg);
}

static void
break_strings_remove(conf_object_t *obj, int64 bp_id)
{
        bs_remove(break_string_data(obj), bp_id);
}

static attr_value_t
get_break_strings(conf_object_t *obj)
{
        return bs_attr_break_strings(break_string_data(obj));
}

void
init_local()
{
        const class_info_t funcs = {
                .alloc = alloc,
                .init = init,
                .deinit = deinit,
                .finalize = finalize,
                .dealloc = dealloc,
                .description = "The <class>" GFX_CONSOLE_STR "</class>"
                " class implements a graphical console,"
                " representing a computer screen. Several"
                " consoles may be present at any time. It also supports input"
                " of keyboard and mouse events. The objects of the"
                " <class>" GFX_CONSOLE_STR "</class> class should be connected"
                " to a graphics card device and a keyboard/mouse device.",
                .short_desc = "graphical console"
        };
        conf_class_t *class = SIM_create_class(GFX_CONSOLE_STR, &funcs);
        ASSERT(class);

        SIM_log_register_groups(class, log_groups);
        
        register_gfx_input(class);
        register_gfx_recording(class);
        register_gfx_output(class);
        register_gfx_break(class);
        register_gfx_vnc(class);
        register_break_strings(class);

        static const break_strings_interface_t break_strings_iface = {
                .add = break_strings_add,
                .add_single = break_strings_add_single,
                .remove = break_strings_remove,
        };
        SIM_REGISTER_INTERFACE(class, break_strings, &break_strings_iface);

        static const break_strings_v2_interface_t break_strings_v2_iface = {
                .add = break_strings_add,
                .add_single = break_strings_add_single,
                .add_regexp = break_strings_add_regexp,
                .remove = break_strings_remove,
        };
        SIM_REGISTER_INTERFACE(class, break_strings_v2,
                               &break_strings_v2_iface);
        
        SIM_register_attribute(
                class, "break_strings",
                get_break_strings, 0,
                Sim_Attr_Pseudo,
                "[[sbbiib]*]",
                "List of console break strings"
                " (string, active, oneshot, hap ID, unused, regexp)");

        init_frontends();
}
