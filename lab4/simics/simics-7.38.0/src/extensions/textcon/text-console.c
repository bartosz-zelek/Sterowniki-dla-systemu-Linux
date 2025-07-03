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

#include "text-console.h"
#include <simics/util/hashtab.h>
#include <simics/base/conf-object.h>
#include <simics/base/log.h>
#include <simics/base/notifier.h>
#include <simics/simulator/conf-object.h>
#include "driver.h"
#include "recorder.h"
#include "break-strings.h"
#include "screen.h"
#include "vt100.h"
#include "telnet.h"
#include "host-serial.h"
#include "console-frontend.h"
#include "winsome-frontend.h"
#include "text-inline.h"

// Log group names. Order should match Text_Console_Log_*
static const char *const log_groups[] = {
        "main",
        "break-strings",
        "recorder",
        "vt100",
        "screen",
        "telnet",
        "pty",
        "output-line",
        NULL,
};
STATIC_ASSERT(ALEN(log_groups) == Text_Console_Log_Num + 1);

// Defines frontend object name from console object name.
#define FRONTEND_PORT "frontend"

// Module preference key
#define PREFS_KEY "text-console"

conf_object_t *
get_text_console_frontend(conf_object_t *obj)
{
        return get_console_frontend(obj, FRONTEND_PORT);
}

static conf_object_t *
alloc(conf_class_t *cl)
{
        text_console_t *con = MM_ZALLOC(1, text_console_t);
        return to_obj(con);
}

static void *
init(conf_object_t *obj)
{
        text_console_t *tc = from_obj(obj);
        tc->driver_data = make_driver(tc);
        tc->record_data = make_text_recording(tc);
        // Init break strings in console-common, providing log group.
        tc->break_data = make_break_strings(obj, Text_Console_Log_Break);
        tc->screen = make_screen(tc, SIM_object_name(obj));
        tc->vt100_data = make_vt100(tc);
        tc->telnet_data = make_telnet(tc);
        tc->host_serial_data = make_host_serial(tc);
        return obj;
}

static void
pre_delete_instance(conf_object_t *unused, conf_object_t *obj, void *arg)
{
        text_console_t *tc = from_obj(obj);
        pre_delete_screen(tc->screen);
}

static void
finalize(conf_object_t *obj)
{
        text_console_t *tc = from_obj(obj);
        conf_class_t *frontend = get_console_frontend_class(obj, PREFS_KEY);
        strbuf_t name = sb_newf("%s.%s", SIM_object_name(obj), FRONTEND_PORT);
        conf_object_t *frontend_obj = SIM_create_object(
                frontend, sb_str(&name), SIM_make_attr_list(0));        
        ASSERT(frontend_obj);
        sb_free(&name);
        finalise_driver(tc->driver_data);
        finalise_screen(tc->screen);
        finalise_vt100(tc->vt100_data);
        SIM_add_notifier(obj, Sim_Notify_Object_Delete, NULL,
                         pre_delete_instance, NULL);
}

static void
dealloc(conf_object_t *obj)
{
        text_console_t *tc = from_obj(obj);
        MM_FREE(tc);
}

static void
deinit(conf_object_t *obj)
{
        text_console_t *tc = from_obj(obj);
        destroy_host_serial(tc->host_serial_data);
        destroy_telnet(tc->telnet_data);
        destroy_vt100(tc->vt100_data);
        destroy_screen(tc->screen);
        destroy_break_strings(tc->break_data);
        destroy_text_recording(tc->record_data);
        destroy_driver(tc->driver_data);
}

static void
init_frontends()
{
        init_console_frontends();
        conf_class_t *winsome = init_text_frontend_winsome();
        ASSERT(winsome);
        register_console_frontend(PREFS_KEY, "winsome", winsome);
}

static console_break_data_t * 	 
break_string_data(conf_object_t *obj) 	 
{ 	 
        return from_obj(obj)->break_data; 	 
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
        const class_info_t cdata = {
                .alloc = alloc,
                .init = init,
                .finalize = finalize,
                .deinit = deinit,
                .dealloc = dealloc,
                .description = "The <class>textcon</class> class provides"
                " a text user interface to a simulated machine. The console is"
                " typically connected to a simulated serial device.",
                .short_desc = "text console"
        };
        conf_class_t *class = SIM_create_class("textcon", &cdata);
        ASSERT(class);
        
        SIM_log_register_groups(class, log_groups);
        
        register_driver(class);
        register_text_recording(class);
        register_break_strings(class);
        register_screen(class);
        register_vt100(class);
        register_telnet(class);
        register_host_serial(class);

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
