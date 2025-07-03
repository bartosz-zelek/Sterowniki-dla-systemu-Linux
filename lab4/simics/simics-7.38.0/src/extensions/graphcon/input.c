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

#include "input.h"
#include <simics/util/hashtab.h>
#include <simics/base/conf-object.h>
#include <simics/base/configuration.h>
#include <simics/base/log.h>
#include <simics/base/notifier.h>
#include <simics/model-iface/kbd-interface.h>
#include <simics/model-iface/abs-pointer.h>
#include <simics/simulator/control.h>
#include <simics/simulator/hap-consumer.h>
#include <simics/simulator-iface/consoles.h>
#include "gfx-console.h"
#include "recorder.h"
#include "output.h"
#include "gfx-break.h"
#include "gfx-inline.h"

// Key stroke input data structure.
typedef struct {
        sim_key_t code;
        bool down;
} gfx_kbd_input_t;

// Data associated with absolute mouse pointer device.
typedef struct {
        // Absolute mouse pointer.
        conf_object_t *device;
        const abs_pointer_interface_t *iface;
        // Is the console using the absolute mouse?
        bool enabled;
} gfx_abs_pointer_t;

// Data associated with relative mouse positioning.
typedef struct {
        // Latest seen mouse coordinates.
        int last_x;
        int last_y;
        // True until mouse has moved at least once.
        bool first_move;
        // Should the mouse be fake moved to the centre?
        bool mouse_centered;
        // Micrometers per pixel.
        int microm_x;
        int microm_y;
} gfx_mouse_state_t;

// Data associated with console input grab.
typedef struct {
        // Mouse button to activate grab.
        gfx_console_mouse_button_t button;
        // Keyboard modifier to activate grab.
        sim_key_t modifier;
        // Grab mode active?
        bool active;
} gfx_grab_data_t;

struct gfx_input_data {
        gfx_console_t *gc;           /* Nasty shortcut to containing object. */

        // Associated keyboard device.
        conf_object_t *kbd;
        const keyboard_interface_t *kbd_int;
        // Associated mouse device.
        conf_object_t *mouse;
        const mouse_interface_t *mouse_int;
        // Recorded keyboard input not yet sent to keyboard device.
        QUEUE(gfx_kbd_input_t) kbd_input;
        gfx_grab_data_t grab_data;
        gfx_mouse_state_t rel_mouse_state;
        gfx_abs_pointer_t abs_pointer_data;

        // Is console GUI window visible?
        int visible;
        // Are VNC clients connected?
        bool vnc_connected;
        // Is Simics running?
        bool running;
};

// Maps gfx_console_mouse_button_t grab button to descriptive string
// used by attributes.
static ht_int_table_t grab_btn_to_str;
// Maps grab button descriptive string to gfx_console_mouse_button_t.
static ht_str_table_t str_to_grab_btn;
// Maps sim_key_t to grab modifier to descriptive string used by attributes.
static ht_int_table_t grab_mod_to_str;
// Maps grab modifier descriptive string to gfx_console_mouse_button_t.
static ht_str_table_t str_to_grab_mod;

// Console visibility notifiers
static notifier_type_t notify_open;
static notifier_type_t notify_close;

static gfx_input_data_t *
input_data(conf_object_t *obj)
{
        return from_obj(obj)->input_data;
}

static conf_object_t *
gi_obj(gfx_input_data_t *gi) { return to_obj(gi->gc); }

static gfx_output_data_t *
gi_go(gfx_input_data_t *gi) { return gi->gc->output_data; }

static gfx_break_data_t *
gi_gb(gfx_input_data_t *gi) { return gi->gc->gfx_break_data; }

// Convert button bitmask from frontend to bitmask used by the absolute mouse.
static abs_pointer_buttons_t
abs_buttons(int buttons)
{
        return (((buttons & Gfx_Console_Mouse_Button_Left)
                 ? Abs_Pointer_Button_Left : 0)
                | ((buttons & Gfx_Console_Mouse_Button_Right)
                   ? Abs_Pointer_Button_Right : 0)
                | ((buttons & Gfx_Console_Mouse_Button_Middle)
                   ? Abs_Pointer_Button_Middle : 0));
}

// Create absolute mouse device state from data sent by console frontend.
static abs_pointer_state_t
create_abs_pointer_state(gfx_input_data_t *gi, int x, int y, int z, int buttons)
{
        return (abs_pointer_state_t){
                .x = x * 0xffff / gfx_get_width(gi_go(gi)),
                .y = y * 0xffff / gfx_get_height(gi_go(gi)),
                .z = z * 0x7fff,
                .buttons = abs_buttons(buttons)};
}

// Called by keyboard device when ready for input.
static void
keyboard_ready(gfx_input_data_t *gi)
{
        // Pass queued inputs to keyboard.
        int ok = gi->kbd ? 1 : 0;
        while (!QEMPTY(gi->kbd_input) && ok) {
                gfx_kbd_input_t data = QPEEK(gi->kbd_input);
                ok = gi->kbd_int->keyboard_event(
                        gi->kbd, !data.down, data.code);
                if (ok)
                        QDROP(gi->kbd_input, 1);
        }
}

// Implementation of keyboard_console interface.
static void
keyboard_console_keyboard_ready(conf_object_t *obj)
{
        gfx_input_data_t *gi = input_data(obj);
        keyboard_ready(gi);
}

// Implementation of gfx_console_backend interface.
static void
gfx_console_backend_kbd_event(conf_object_t *obj, sim_key_t code, bool down)
{
        gfx_input_data_t *gi = input_data(obj);
        // Send data to recorder.
        gfx_record_kbd_input(gi->gc->record_data, code, down);
}

// Implementation of gfx_console_backend interface.
static void
gfx_console_backend_mouse_event(conf_object_t *obj,
                                int x, int y, int z,
                                gfx_console_mouse_button_t buttons)
{
        gfx_input_data_t *gi = input_data(obj);
        // Send data to recorder.
        gfx_record_mouse_input(gi->gc->record_data, x, y, z, buttons);
}

// Make sure the whole screen will be sent to the frontend on next draw event.
static void
request_refresh(gfx_input_data_t *gi)
{
        frontend_redraw(gi_go(gi));
        request_vga_refresh_all(gi_gb(gi));
}

// Make sure the whole screen will be sent to the frontend on next draw event.
static void
gfx_console_backend_request_refresh(conf_object_t *obj)
{
        gfx_input_data_t *gi = input_data(obj);
        request_refresh(gi);
}

// Is console GUI or VNC clients visible?
bool
console_visible(gfx_input_data_t *gi)
{
        return (gi->visible || gi->vnc_connected);
}

// Determine if device refresh events should be enabled.
static bool
enable_device_refresh(gfx_input_data_t *gi)
{
        return console_visible(gi) && gi->running;
}

// Enable/disable gfx device refresh events. Must only be called upon change
// of visibility or 
static void
adjust_gfx_refresh_status(gfx_input_data_t *gi, bool old, bool new)
{
        if (new && !old) {
                // Make sure whole screen is updated on newly visible window.
                request_refresh(gi);
                // Turn on draw events.
                enable_gfx_device_refresh(gi->gc->gfx_break_data);
        } else if (!new && old) {
                // Turn off draw events if invisible.
                disable_gfx_device_refresh(gi->gc->gfx_break_data, false);
        }        
        
}

// Core_Simulation_Stopped hap callback.
static void
simulation_stopped(lang_void *arg)
{
        bool old_status = console_visible(arg);        
        gfx_input_data_t *gi = arg;
        gi->running = false;
        if (old_status) {
                frontend_redraw(gi_go(gi));
                disable_gfx_device_refresh(gi->gc->gfx_break_data, true);
        }
}

// Core_Continuation hap callback
static void
simulation_continued(lang_void *arg)
{
        bool new_status = console_visible(arg);
        gfx_input_data_t *gi = arg;
        gi->running = true;
        if (new_status)
                enable_gfx_device_refresh(gi->gc->gfx_break_data);
}

void
gfx_set_visible(gfx_input_data_t *gi, bool visible)
{
        bool old_status = enable_device_refresh(gi);
        gi->visible = visible;
        bool new_status = enable_device_refresh(gi);
        adjust_gfx_refresh_status(gi, old_status, new_status);
        if (gi->visible) {
                // Make sure whole screen is updated on newly visible window.
                request_refresh(gi);
                gfx_enqueue_redraw(gi_go(gi));
                SIM_notify(gi_obj(gi), notify_open);
        } else
                SIM_notify(gi_obj(gi), notify_close);
}

void
gfx_set_vnc_connected(gfx_input_data_t *gi, bool connected)
{
        bool old_status = enable_device_refresh(gi);
        gi->vnc_connected = connected;
        bool new_status = enable_device_refresh(gi);
        adjust_gfx_refresh_status(gi, old_status, new_status);
        if (connected)
                request_vga_refresh_all(gi_gb(gi));
}

// Implementation of gfx_console_backend interface.
static void
gfx_console_backend_set_visible(conf_object_t *obj, bool visible)
{
        gfx_input_data_t *gi = input_data(obj);
        gfx_set_visible(gi, visible);
}

// Implementation of gfx_console_backend interface.
static gfx_console_screen_text_t
gfx_console_backend_text_data(conf_object_t *obj)
{
        gfx_input_data_t *gi = input_data(obj);
        return gfx_get_text_data(gi_gb(gi));
}

// Implementation of gfx_console_backend interface.
static void
gfx_console_backend_got_grab_keys(conf_object_t *obj)
{
        gfx_input_data_t *gi = input_data(obj);
        if (!gi->abs_pointer_data.enabled) {
                // Grab mode activated.
                gi->grab_data.active = !gi->grab_data.active;
                frontend_set_grab_mode(gi_go(gi), gi->grab_data.active);
                set_window_title_with_grab_info(gi);
                if (gi->grab_data.active)
                        gi->rel_mouse_state.first_move = true;
        }
}

static void
abs_pointer_enable(gfx_input_data_t *gi)
{
        if (gi->abs_pointer_data.device != NULL) {
                gi->abs_pointer_data.enabled = true;
                // Remove grab info from window title.
                set_window_title_with_grab_info(gi);
        } else
                SIM_LOG_ERROR(gi_obj(gi), 0, "Cannot enable abs_pointer"
                              " without a abs_pointer object.");
}

// Implementation of abs_pointer_activate interface.
static void
abs_pointer_activate_enable(conf_object_t *obj)
{
        gfx_input_data_t *gi = input_data(obj);
        abs_pointer_enable(gi);
}

static void
abs_pointer_disable(gfx_input_data_t *gi)
{
        gi->abs_pointer_data.enabled = false;
        // Add grab info to window title.
        set_window_title_with_grab_info(gi);
}

// Implementation of abs_pointer_activate interface.
static void
abs_pointer_activate_disable(conf_object_t *obj)
{
        gfx_input_data_t *gi = input_data(obj);
        abs_pointer_disable(gi);
}

// Enqueue given keyboard event to be sent to the keyboard device.
void
gfx_kbd_input(gfx_input_data_t *gi, sim_key_t code, bool down)
{
        SIM_LOG_INFO(3, gi_obj(gi), Gfx_Console_Log_Input,
                     "Got input code %d down %d",
                     code, (int)down);

        gfx_kbd_input_t data = {.code = code, .down = down};
        // Keyboard device can in general not handle SK_ILLEGAL.
        if (data.code != SK_ILLEGAL) {
                QADD(gi->kbd_input, data);
                // TODO Shouldn't the keyboard device call this?
                keyboard_ready(gi);
        }
}

// Update console relative mouse state using the given incoming mouse event
// with coordinates, wheel and button bitmask.
static void
update_relative_mouse_state(gfx_input_data_t *gi,
                            int x, int y, int z, int buttons)
{
        int width = gfx_get_width(gi_go(gi));
        int height = gfx_get_height(gi_go(gi));

        // To avoid jumping cursor, set it first time.
        if (gi->rel_mouse_state.first_move) {
                gi->rel_mouse_state.last_x = x;
                gi->rel_mouse_state.last_y = y;
                // Avoid sending grab button press to mouse device
                buttons &= ~gi->grab_data.button;
                gi->rel_mouse_state.first_move = false;
        }
        
        if (gi->rel_mouse_state.mouse_centered
            && x == width >> 1
            && y == height >> 1) {
                // Ignore this event since it's our own creation.
                gi->rel_mouse_state.mouse_centered = false;
        } else {
                // Is the mouse near the window edges?
                if (!gi->rel_mouse_state.mouse_centered
                    && (x < 10 || x > width - 64
                        || y < 10 || y > height - 64)) {
                        // Warp pointer to center of window.
                        frontend_warp_mouse(gi_go(gi),
                                            width >> 1, height >> 1);
                        // Flag faked move.
                        gi->rel_mouse_state.mouse_centered = true;  
                }
                
                int xmicro = gi->rel_mouse_state.microm_x
                        * (x - gi->rel_mouse_state.last_x);
                int ymicro = -gi->rel_mouse_state.microm_y
                        * (y - gi->rel_mouse_state.last_y);
                
                SIM_LOG_INFO(
                        4, gi_obj(gi), Gfx_Console_Log_Input,
                        "Send mouse event (x, y, z) = (%d, %d, %d) buttons %d",
                        xmicro, ymicro, z, buttons);
                gi->mouse_int->mouse_event(
                        gi->mouse, xmicro, ymicro, z, buttons);
        }
        gi->rel_mouse_state.last_x = x;
        gi->rel_mouse_state.last_y = y;
}

// Enqueue given mouse data to be sent to the keyboard device.
// Also update internal mouse state if using relative mouse.
void
gfx_mouse_input(gfx_input_data_t *gi, int x, int y, int z, int buttons)
{
        SIM_LOG_INFO(4, gi_obj(gi), Gfx_Console_Log_Input,
                     "Got mouse event (x, y, z) = (%d, %d, %d) buttons %d",
                     x, y, z, buttons);

        if (gi->abs_pointer_data.enabled) {
                gi->abs_pointer_data.iface->set_state(
                        gi->abs_pointer_data.device,
                        create_abs_pointer_state(gi, x, y, z, buttons));
        } else {
                if (gi->grab_data.active)
                        update_relative_mouse_state(gi, x, y, z, buttons);
        }
}

// Return descriptive string of current grab mouse button.
static const char *
get_grab_btn_str(gfx_input_data_t *gi)
{
        return ht_lookup_int(&grab_btn_to_str, gi->grab_data.button);
}

// Return descriptive string of current grab keyboard modifier.
static const char *
get_grab_mod_str(gfx_input_data_t *gi)
{
        return ht_lookup_int(&grab_mod_to_str, gi->grab_data.modifier);
}

// Set console frontend GUI window title, possibly with a text about grab
// buttons, if not using absolute mouse.
void
set_window_title_with_grab_info(gfx_input_data_t *gi)
{
        strbuf_t title = sb_new(gfx_get_window_title(gi_go(gi)));
        if (!gi->abs_pointer_data.enabled) {
                strbuf_t shortcut = sb_newf(
                        "%s and %s button",
                        get_grab_mod_str(gi),
                        get_grab_btn_str(gi));

                if (gi->grab_data.active)
                        sb_addfmt(&title, " - Press %s to leave window",
                                  sb_str(&shortcut));
                else
                        sb_addfmt(&title,
                                  " - Press %s to grab additional input",
                                  sb_str(&shortcut));
                sb_free(&shortcut);

        }
        frontend_set_window_title(gi_go(gi), gfx_get_window_title(gi_go(gi)),
                                  sb_str(&title));
        sb_free(&title);
}

static set_error_t
set_keyboard(conf_object_t *obj, attr_value_t *val)
{
        gfx_input_data_t *gi = input_data(obj);
        conf_object_t *dev = SIM_attr_object_or_nil(*val);
        const keyboard_interface_t *iface = NULL;
        if (dev) {
                iface = SIM_C_GET_INTERFACE(dev, keyboard);
                if (!iface)
                        return Sim_Set_Interface_Not_Found;
        }
        gi->kbd = dev;
        gi->kbd_int = iface;
        return Sim_Set_Ok;
}

static attr_value_t
get_keyboard(conf_object_t *obj)
{
        return SIM_make_attr_object(input_data(obj)->kbd);
}

static set_error_t
set_mouse(conf_object_t *obj, attr_value_t *val)
{
        gfx_input_data_t *gi = input_data(obj);
        conf_object_t *dev = SIM_attr_object_or_nil(*val);
        const mouse_interface_t *iface = NULL;
        if (dev) {
                iface = SIM_C_GET_INTERFACE(dev, mouse);
                if (!iface)
                        return Sim_Set_Interface_Not_Found;
        }
        gi->mouse = dev;
        gi->mouse_int = iface;
        return Sim_Set_Ok;
}

static attr_value_t
get_mouse(conf_object_t *obj)
{
        return SIM_make_attr_object(input_data(obj)->mouse);
}

static attr_value_t
get_abs_mouse(conf_object_t *obj)
{
        gfx_input_data_t *gi = input_data(obj);
        return SIM_make_attr_object(gi->abs_pointer_data.device);
}

static set_error_t
set_abs_mouse(conf_object_t *obj, attr_value_t *val)
{
        gfx_input_data_t *gi = input_data(obj);
        conf_object_t *dev = SIM_attr_object_or_nil(*val);
        const abs_pointer_interface_t *iface = NULL;
        if (!dev) {
                // Must disable abs-pointer if no object.
                if (gi->abs_pointer_data.enabled)
                        abs_pointer_disable(gi);
        } else {
                iface = SIM_C_GET_INTERFACE(dev, abs_pointer);
                if (!iface)
                        return Sim_Set_Interface_Not_Found;
        }
        gi->abs_pointer_data.device = dev;
        gi->abs_pointer_data.iface = iface;
        return Sim_Set_Ok;
}

static attr_value_t
get_abs_pointer_enabled(conf_object_t *obj)
{
        gfx_input_data_t *gi = input_data(obj);
        return SIM_make_attr_boolean(gi->abs_pointer_data.enabled);
}

static set_error_t
set_abs_pointer_enabled(conf_object_t *obj, attr_value_t *val)
{
        gfx_input_data_t *gi = input_data(obj);
        bool new_value = SIM_attr_boolean(*val);
        if (new_value) {
                if (!gi->abs_pointer_data.device) {
                        SIM_attribute_error(
                                "Enabling absolute pointer functionality"
                                " requires the abs_mouse attribute to be set.");
                        return Sim_Set_Illegal_Value;
                } else
                        abs_pointer_enable(gi);
        } else {
                abs_pointer_disable(gi);
        }
        return Sim_Set_Ok;
}

static set_error_t
set_pending_kbd_input(conf_object_t *obj, attr_value_t *val)
{
        gfx_input_data_t *gi = input_data(obj);
        QCLEAR(gi->kbd_input);
        for (int i = 0; i < SIM_attr_list_size(*val); i++) {
                attr_value_t a = SIM_attr_list_item(*val, i);
                gfx_kbd_input_t ki = {
                        .code = SIM_attr_integer(SIM_attr_list_item(a, 0)),
                        .down = SIM_attr_boolean(SIM_attr_list_item(a, 1)),
                };
                QADD(gi->kbd_input, ki);
        }
        return Sim_Set_Ok;
}

static attr_value_t
get_pending_kbd_input(conf_object_t *obj)
{
        gfx_input_data_t *gi = input_data(obj);
        int size = QLEN(gi->kbd_input);
        attr_value_t ret = SIM_alloc_attr_list(size);
        for (int i = 0; i < size; i++) {
                gfx_kbd_input_t ki = QGET(gi->kbd_input, i);
                SIM_attr_list_set_item(
                        &ret, i,
                        SIM_make_attr_list(2,
                                           SIM_make_attr_uint64(ki.code),
                                           SIM_make_attr_boolean(ki.down)));
        }
        return ret;
}

static set_error_t
set_mouse_state(conf_object_t *obj, attr_value_t *val)
{
        gfx_input_data_t *gi = input_data(obj);
        gi->rel_mouse_state.last_x =
                SIM_attr_integer(SIM_attr_list_item(*val, 0));
        gi->rel_mouse_state.last_y =
                SIM_attr_integer(SIM_attr_list_item(*val, 1));
        gi->rel_mouse_state.mouse_centered =
                SIM_attr_boolean(SIM_attr_list_item(*val, 2));
        gi->rel_mouse_state.first_move =
                SIM_attr_boolean(SIM_attr_list_item(*val, 3));
        return Sim_Set_Ok;
}

static attr_value_t
get_mouse_state(conf_object_t *obj)
{
        gfx_input_data_t *gi = input_data(obj);
        return SIM_make_attr_list(
                4,
                SIM_make_attr_uint64(gi->rel_mouse_state.last_x),
                SIM_make_attr_uint64(gi->rel_mouse_state.last_y),
                SIM_make_attr_boolean(gi->rel_mouse_state.mouse_centered),
                SIM_make_attr_boolean(gi->rel_mouse_state.first_move));
}

static set_error_t
set_microm_per_pixel(conf_object_t *obj, attr_value_t *val)
{
        gfx_input_data_t *gi = input_data(obj);
        gi->rel_mouse_state.microm_x =
                SIM_attr_integer(SIM_attr_list_item(*val, 0));
        gi->rel_mouse_state.microm_y =
                SIM_attr_integer(SIM_attr_list_item(*val, 1));
        return Sim_Set_Ok;
}

static attr_value_t
get_microm_per_pixel(conf_object_t *obj)
{
        gfx_input_data_t *gi = input_data(obj);
        return SIM_make_attr_list(
                2,
                SIM_make_attr_uint64(gi->rel_mouse_state.microm_x),
                SIM_make_attr_uint64(gi->rel_mouse_state.microm_y));
}

static attr_value_t
get_grab_modifier(conf_object_t *obj)
{
        gfx_input_data_t *gi = input_data(obj);
        // Return descriptive string of grab modifier.
        return SIM_make_attr_string(get_grab_mod_str(gi));
}

static set_error_t
set_grab_modifier(conf_object_t *obj, attr_value_t *val)
{
        gfx_input_data_t *gi = input_data(obj);
        const char *str = SIM_attr_string(*val);
        // Convert from descriptive string.
        void *elt = ht_lookup_str(&str_to_grab_mod, str);
        if (elt) {
                gi->grab_data.modifier = (sim_key_t)elt;
                SIM_LOG_INFO(3, gi_obj(gi), Gfx_Console_Log_Input,
                             "Set mouse grab modifier code %d",
                             gi->grab_data.modifier);
                set_window_title_with_grab_info(gi);
                frontend_set_grab_modifier(gi_go(gi), gi->grab_data.modifier);
                return Sim_Set_Ok;
        } else {
                strbuf_t msg = sb_newf("Illegal grab modifier: %s", str);
                SIM_attribute_error(sb_str(&msg));
                sb_free(&msg);
                return Sim_Set_Illegal_Value;
        }
}

static attr_value_t
get_grab_button(conf_object_t *obj)
{
        gfx_input_data_t *gi = input_data(obj);
        // Return descriptive string of grab button.
        return SIM_make_attr_string(get_grab_btn_str(gi));
}

static set_error_t
set_grab_button(conf_object_t *obj, attr_value_t *val)
{
        gfx_input_data_t *gi = input_data(obj);
        const char *str = SIM_attr_string(*val);
        // Convert from descriptive string.
        void *elt = ht_lookup_str(&str_to_grab_btn, str);
        if (elt) {
                gi->grab_data.button = (gfx_console_mouse_button_t)elt;
                SIM_LOG_INFO(3, gi_obj(gi), Gfx_Console_Log_Input,
                             "Set mouse grab button code %d",
                             gi->grab_data.button);
                set_window_title_with_grab_info(gi);
                frontend_set_grab_button(gi_go(gi), gi->grab_data.button);
                return Sim_Set_Ok;
        } else {
                strbuf_t msg = sb_newf("Illegal grab button: %s", str);
                SIM_attribute_error(sb_str(&msg));
                sb_free(&msg);
                return Sim_Set_Illegal_Value;
        }
}

static attr_value_t
get_visible(conf_object_t *obj)
{
        gfx_input_data_t *gi = input_data(obj);
        return SIM_make_attr_boolean(gi->visible == -1 ? false : gi->visible);
}

static set_error_t
set_visible(conf_object_t *obj, attr_value_t *val)
{
        gfx_input_data_t *gi = input_data(obj);
        if (SIM_attr_is_boolean(*val)) {
                bool visible = SIM_attr_boolean(*val);
                // -no-win should override this, unless explicitly set
                if (!SIM_object_is_configured(obj)
                    || SIM_is_restoring_state(obj))
                        visible = visible && !VT_get_hide_consoles_flag();
                gfx_set_visible(gi, visible);
                frontend_set_visible(gi_go(gi), visible);
        }
        return Sim_Set_Ok;
}

gfx_input_data_t *
make_gfx_input(gfx_console_t *gc)
{
        gfx_input_data_t *gi = MM_ZALLOC(1, gfx_input_data_t);
        gi->gc = gc;
        gi->grab_data.button = Gfx_Console_Mouse_Button_Right;
        gi->grab_data.modifier = SK_SHIFT_L;
        gi->rel_mouse_state.microm_x = 300;
        gi->rel_mouse_state.microm_y = 300;
        // Unset => console will be shown automatically upon
        // startup if it is unique.
        gi->visible = -1;
        return gi;
}

void
finalise_gfx_input(gfx_input_data_t *gi)
{
        SIM_hap_add_callback("Core_Simulation_Stopped",
                             (obj_hap_func_t)simulation_stopped, gi);
        SIM_hap_add_callback("Core_Continuation",
                             (obj_hap_func_t)simulation_continued, gi);
        // If visibility set explicitly, inform frontend.
        if (gi->visible != -1) {
                gfx_set_visible(gi, gi->visible);
                frontend_set_visible(gi_go(gi), gi->visible);
        } else
                gi->visible = false;
        
        frontend_set_grab_modifier(gi_go(gi), gi->grab_data.modifier);
        frontend_set_grab_button(gi_go(gi), gi->grab_data.button);
}

void
pre_delete_gfx_input(gfx_input_data_t *gi)
{
        SIM_hap_delete_callback("Core_Simulation_Stopped",
                                (obj_hap_func_t)simulation_stopped, gi);
        SIM_hap_delete_callback("Core_Continuation",
                                (obj_hap_func_t)simulation_continued, gi);
}

void
destroy_gfx_input(gfx_input_data_t *gi)
{
        MM_FREE(gi);
}

static void
init_grab_button_lists()
{
        ht_init_int_table(&grab_btn_to_str);
        ht_init_int_table(&grab_mod_to_str);
        ht_init_str_table(&str_to_grab_btn, false);
        ht_init_str_table(&str_to_grab_mod, false);
        ht_insert_int(&grab_btn_to_str,
                      Gfx_Console_Mouse_Button_Left, "left");
        ht_insert_int(&grab_btn_to_str,
                      Gfx_Console_Mouse_Button_Middle, "middle");
        ht_insert_int(&grab_btn_to_str,
                      Gfx_Console_Mouse_Button_Right, "right");
        ht_insert_int(&grab_mod_to_str, SK_CTRL_L, "control");
        ht_insert_int(&grab_mod_to_str, SK_ALT_L, "alt");
        ht_insert_int(&grab_mod_to_str, SK_SHIFT_L, "shift");
        ht_insert_str(&str_to_grab_btn, "left",
                      (void *)Gfx_Console_Mouse_Button_Left);
        ht_insert_str(&str_to_grab_btn, "middle",
                      (void *)Gfx_Console_Mouse_Button_Middle);
        ht_insert_str(&str_to_grab_btn, "right",
                      (void *)Gfx_Console_Mouse_Button_Right);
        ht_insert_str(&str_to_grab_mod, "control", (void *)SK_CTRL_L);
        ht_insert_str(&str_to_grab_mod, "alt", (void *)SK_ALT_L);
        ht_insert_str(&str_to_grab_mod, "shift", (void *)SK_SHIFT_L);
}

void
register_gfx_input(conf_class_t *cl)
{
        static const keyboard_console_interface_t kc_iface = {
                .keyboard_ready = keyboard_console_keyboard_ready
        };
        SIM_REGISTER_INTERFACE(cl, keyboard_console, &kc_iface);

        static const gfx_console_backend_interface_t gcb_iface = {
                .kbd_event = gfx_console_backend_kbd_event,
                .mouse_event = gfx_console_backend_mouse_event,
                .request_refresh = gfx_console_backend_request_refresh,
                .set_visible = gfx_console_backend_set_visible,
                .text_data = gfx_console_backend_text_data,
                .got_grab_keys = gfx_console_backend_got_grab_keys,
        };
        SIM_REGISTER_INTERFACE(cl, gfx_console_backend, &gcb_iface);

        static const abs_pointer_activate_interface_t apa_iface = {
                .enable  = abs_pointer_activate_enable,
                .disable = abs_pointer_activate_disable
        };
        SIM_REGISTER_INTERFACE(cl, abs_pointer_activate, &apa_iface);
        
        SIM_register_attribute(
                cl, "keyboard",
                get_keyboard,
                set_keyboard,
                Sim_Attr_Optional,
                "o|n",
                "Keyboard device the console is connected to.");

        SIM_register_attribute(
                cl, "mouse",
                get_mouse,
                set_mouse,
                Sim_Attr_Optional,
                "o|n",
                "Mouse device the console is connected to.");

        SIM_register_attribute(
                cl, "abs_mouse",
                get_abs_mouse,
                set_abs_mouse,
                Sim_Attr_Optional,
                "o|n",
                "Absolute positioning pointer device connected to the"
                " console. Must implement the " ABS_POINTER_INTERFACE
                " interface");

        SIM_register_attribute(
                cl, "abs_pointer_enabled",
                get_abs_pointer_enabled,
                set_abs_pointer_enabled,
                Sim_Attr_Optional,
                "b",
                "Absolute positioning pointer enabled.");
        
        SIM_register_attribute(
                cl, "pending_kbd_input",
                get_pending_kbd_input,
                set_pending_kbd_input,
                Sim_Attr_Optional,
                "[[ib]*]",
                "Queued keyboard input waiting to be sent to the"
                " attached keyboard device. Each element is a key code"
                " and a make/break flag (TRUE on make).");

        SIM_register_attribute(
                cl, "mouse_state",
                get_mouse_state,
                set_mouse_state,
                Sim_Attr_Optional,
                "[iibb]",
                "Mouse state, only used when not using an"
                " absolute positioning pointer.");
        
        SIM_register_attribute(
                cl, "microm_per_pixel",
                get_microm_per_pixel,
                set_microm_per_pixel,
                Sim_Attr_Pseudo,
                "[ii]",
                "Micrometers per (horizontal, vertical) pixel for converting"
                " mouse movements. only used when not using an"
                " absolute positioning pointer.");

        SIM_register_attribute(
                cl, "grab_modifier",
                get_grab_modifier,
                set_grab_modifier,
                Sim_Attr_Optional,
                "s",
                "A string in (control, alt, shift), specifying"
                " the modifier key used for grabbing and ungrabbing input from"
                " the console. Currently only the left side"
                " modifier are interpreted.");

        SIM_register_attribute(
                cl, "grab_button",
                get_grab_button,
                set_grab_button,
                Sim_Attr_Optional,
                "s",
                "One of left, middle and right"
                " The grab button specifies which mouse button that is used to"
                " grab and ungrab input for the console.");

        SIM_register_attribute(
                cl, "visible",
                get_visible,
                set_visible,
                Sim_Attr_Optional, "b|n",
                "Show/hide console GUI window. Setting to NIL only has effect"
                " before instantiation, in which case it leads to the default"
                " visibility behaviour of showing the console window if it is"
                " the unique console in the configuration.");
        
        if (ht_num_entries_int(&grab_btn_to_str) == 0)
                init_grab_button_lists();

        notify_open = SIM_notifier_type(SYMBOL_TO_STRING(NOTIFY_OPEN));
        notify_close = SIM_notifier_type(SYMBOL_TO_STRING(NOTIFY_CLOSE));
        SIM_register_notifier(
                cl, notify_open,
                "Notifier that is triggered when a graphics console"
                " is opened.");
        SIM_register_notifier(
                cl, notify_close,
                "Notifier that is triggered when a graphics console"
                " is closed.");
}
