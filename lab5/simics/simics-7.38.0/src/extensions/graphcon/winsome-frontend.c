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

#include "winsome-frontend.h"
#include <simics/base/log.h>
#include <simics/base/notifier.h>
#include <simics/simulator-iface/consoles.h>
#include <simics/simulator/callbacks.h>
#include <simics/simulator/conf-object.h>
#include <simics/simulator/hap-consumer.h>
#include <simics/model-iface/sim-keys.h>

#include <pthread.h>

typedef struct {
        /* A borrowed reference to an array (width × height) of screen pixels
           to update from, or NULL if momentarily unavailable
           (typically because of size change in progress). */
        const uint32 *pixels;
        int left, top;            /* Top left corner of dirty rectangle. */
        int right, bottom;        /* Bottom right corner of dirty rectangle. */

        // Are we currently in VGA text mode?
        bool text_mode;

        // Current/new screen size
        int width;
        int height;

        // Indicates if Winsome has read the update data.
        bool winsome_in_sync;

        // Indicates if there has been non-screen related console activity.
        bool activity;

        // Indicates if the window has been resized.
        bool resize;

        // Buffer for screen data to be sent to Python.
        VECT(uint32) data;
} screen_update_t;

typedef struct {
        conf_object_t obj;

        // Handle returned by the Winsome side which identifies this console
        // object to the winsome singleton object.
        int winsome_handle;

        // The Winsome singleton.
        conf_object_t *gui;
        const gfx_console_frontend_interface_t *winsome;
        const winsome_console_interface_t *wc_iface;

        // Associated console backend.
        conf_object_t *backend;
        const gfx_console_backend_interface_t *input;

        // Console Python object pointer
        void *console_window;

        /* The following data is changed from simulation thread and
           update/GUI threads, hence protected by this lock. */
        pthread_mutex_t lock;

        // Screen update to be sent to the GUI.
        screen_update_t update;

        // Update thread state
        bool has_worker;
        bool exiting;
        pthread_cond_t exit_cond;

        // GUI informs as upon start/stop
        bool gui_running;

        /* End of lock protected data */
        
        // Console state that we store here to avoid having to query the
        // backend object.
        bool visible;

        // Backend deletion hap callback
        hap_handle_t backend_del_hap;

        // Console visibility notifier handles
        notifier_handle_t *open_notifier;
        notifier_handle_t *close_notifier;

        // LED state update
        bool update_led_state;
        gfx_console_led_t new_led_state;
} winsome_frontend_t;

// Console visibility notifiers
static notifier_type_t notify_open;
static notifier_type_t notify_close;

#define WINSOME_FRONTEND_STR SYMBOL_TO_STRING(WINSOME_FRONTEND)

static winsome_frontend_t *
from_obj(conf_object_t *obj)
{
        return (winsome_frontend_t *)obj;
}

static conf_object_t *
to_obj(winsome_frontend_t *wf)
{
        return &wf->obj;
}

static winsome_update_t
construct_gui_update(winsome_frontend_t *wf)
{
        winsome_update_t update = {0};
        screen_update_t *su = &wf->update;
        if (wf->gui_running && su->resize) {
                update.type = Winsome_Update_Resize;
                update.u.resize.width = su->width;
                update.u.resize.height = su->height;
                su->resize = false;
        } else if (wf->gui_running && !su->winsome_in_sync
                   && su->pixels != NULL) {
                int width = su->right - su->left + 1;
                int height = su->bottom - su->top + 1;
                int nelems = width * height;

                /* Create a copy of the rectangle of dirty pixels, to make
                   the code easier to reason about.

                   It would probably be worthwhile to avoid this copy (and the
                   subsequent copy when the data is converted to Python later
                   on) by passing a reference, but we would then have to be
                   very careful to handle reallocations of the original pixel
                   buffer. */
                VRESIZE(su->data, nelems);
                uint32 *dst = VVEC(su->data);
                int start = su->top * su->width + su->left;
                for (int line = 0; line < height; line++)
                        memcpy(dst + line * width,
                               su->pixels + start + line * su->width,
                               width * sizeof *dst);

                update.type = Winsome_Update_Gfx;
                update.u.gfx.data = dst;
                update.u.gfx.left = su->left;
                update.u.gfx.top = su->top;
                update.u.gfx.right = su->right;
                update.u.gfx.bottom = su->bottom;
                update.u.gfx.text_mode = su->text_mode;
                update.u.gfx.width = su->width;
                update.u.gfx.height = su->height;
                su->winsome_in_sync = true;
        } else if (wf->gui_running && su->activity) {
                update.type = Winsome_Update_Activity;
                su->activity = false;
        } else
                update.type = Winsome_Update_Nothing;

        return update;
}

static void
send_gui_update(winsome_frontend_t *wf, winsome_update_t *wu)
{
        switch (wu->type) {
        case Winsome_Update_Activity:
                wf->wc_iface->activity(wf->gui, wf->console_window);
                break;
        case Winsome_Update_Resize:
                wf->wc_iface->resize(wf->gui, wf->console_window,
                                     wu->u.resize.width, wu->u.resize.height);
                break;
        case Winsome_Update_Gfx: {
                size_t width = wu->u.gfx.right - wu->u.gfx.left + 1;
                size_t height = wu->u.gfx.bottom - wu->u.gfx.top + 1;
                wf->wc_iface->gfx(
                        wf->gui, wf->console_window,
                        wu->u.gfx.left, wu->u.gfx.top,
                        wu->u.gfx.right, wu->u.gfx.bottom,
                        (bytes_t){
                                .data = (uint8 *)wu->u.gfx.data,
                                .len = (width * height
                                        * sizeof *wu->u.gfx.data) },
                        wu->u.gfx.text_mode);
                break;
        }
        default:
                ASSERT(0);
        }
}

static void
update_gui(void *arg)
{
        winsome_frontend_t *wf = arg;
        pthread_mutex_lock(&wf->lock);
        for (;;) {
                // Get latest screen update
                winsome_update_t update = construct_gui_update(wf);
                // Exit thread if no current updates
                if (update.type == Winsome_Update_Nothing) {
                        wf->has_worker = false;
                        if (wf->exiting)
                                pthread_cond_signal(&wf->exit_cond);
                        break;
                }

                // Update LED state if necessary
                bool update_led_state = wf->update_led_state;
                wf->update_led_state = false;
                pthread_mutex_unlock(&wf->lock);
                ASSERT(update.type != Winsome_Update_Nothing);

                if (update_led_state)
                        wf->winsome->set_keyboard_leds(
                                wf->gui, wf->winsome_handle,
                                wf->new_led_state);

                // Send screen update to Python
                send_gui_update(wf, &update);
                pthread_mutex_lock(&wf->lock);
        }

        pthread_mutex_unlock(&wf->lock);
}

static void
acquire_work_lock(winsome_frontend_t *wf)
{
        pthread_mutex_lock(&wf->lock);
}

static void
release_work_lock(winsome_frontend_t *wf)
{
        // Start Winsome update thread if necessary
        bool worker_active = wf->has_worker;
        wf->has_worker = true;
        pthread_mutex_unlock(&wf->lock);

        if (!worker_active)
                SIM_run_in_thread(update_gui, wf);
}

static void
backend_del_cb(lang_void *data, conf_object_t *backend)
{
        winsome_frontend_t *wf = data;
        wf->backend = NULL;
        SIM_hap_delete_callback_obj_id("Core_Conf_Object_Delete", backend,
                                       wf->backend_del_hap);
}

static void
frontend_set_visible(winsome_frontend_t *wf, bool visible)
{
        wf->visible = visible;
}

static void
console_open_notify(conf_object_t *obj, conf_object_t *backend,
                    lang_void *data)
{
        frontend_set_visible(from_obj(obj), true);
}

static void
console_close_notify(conf_object_t *obj, conf_object_t *backend,
                     lang_void *data)
{
        frontend_set_visible(from_obj(obj), false);
}

// Implementation of the gui_console_backend interface.
static void
gui_console_backend_start(conf_object_t *obj, void *console)
{
        winsome_frontend_t *wf = from_obj(obj);
        pthread_mutex_lock(&wf->lock);
        wf->gui_running = true;
        wf->console_window = console;
        pthread_mutex_unlock(&wf->lock);
}


// Implementation of the gui_console_backend interface.
static void
gui_console_backend_stop(conf_object_t *obj)
{
        winsome_frontend_t *wf = from_obj(obj);
        pthread_mutex_lock(&wf->lock);
        wf->gui_running = false;
        pthread_mutex_unlock(&wf->lock);
}

// Implementation of the gfx_console_frontend interface.
static void
gfx_console_frontend_set_size(conf_object_t *obj, int handle,
                              int width, int height)
{
        winsome_frontend_t *wf = from_obj(obj);
        if (wf->gui) {
                SIM_LOG_INFO(2, obj, 0, "Resize Winsome frontend 0x%x to %dx%d",
                             handle, width, height);
                acquire_work_lock(wf);
                wf->update.width = width;
                wf->update.height = height;
                wf->update.resize = true;

                // After a resize, the old dirty rectangle is no longer valid
                // since it applied to the previous geometry. We consider the
                // entire screen to be dirty now.
                wf->update.left = 0;
                wf->update.top = 0;
                wf->update.right = wf->update.width - 1;
                wf->update.bottom = wf->update.height - 1;
                release_work_lock(wf);
        }
}

// Implementation of the gfx_console_frontend interface.
static void
gfx_console_frontend_invalidate_contents(conf_object_t *obj, int handle)
{
        winsome_frontend_t *wf = from_obj(obj);
        if (wf->gui) {
                pthread_mutex_lock(&wf->lock);
                wf->update.pixels = NULL;
                pthread_mutex_unlock(&wf->lock);
        }
}

// Implementation of the gfx_console_frontend interface.
static void
gfx_console_frontend_set_visible(conf_object_t *obj, int handle, bool visible)
{
        winsome_frontend_t *wf = from_obj(obj);
        if (wf->gui) {
                ASSERT(wf->winsome);
                frontend_set_visible(wf, visible);
                wf->winsome->set_visible(wf->gui, handle, visible);
        }
}

// Implementation of the gfx_console_frontend interface.
static void
gfx_console_frontend_set_title(conf_object_t *obj, int handle,
                  const char *short_title, const char *long_title)
{
        winsome_frontend_t *wf = from_obj(obj);
        if (wf->gui)
                wf->winsome->set_title(wf->gui, handle,
                                       short_title, long_title);
}

// Implementation of the gfx_console_frontend interface.
static void
gfx_console_frontend_set_grab_mode(
        conf_object_t *obj, int handle, bool active)
{
        winsome_frontend_t *wf = from_obj(obj);
        if (wf->gui)
                wf->winsome->set_grab_mode(wf->gui, handle, active);
}

// Implementation of the gfx_console_frontend interface.
static void
gfx_console_frontend_set_grab_modifier(
        conf_object_t *obj, int handle, sim_key_t modifier)
{
        winsome_frontend_t *wf = from_obj(obj);
        if (wf->gui)
                wf->winsome->set_grab_modifier(wf->gui, handle, modifier);
}

// Implementation of the gfx_console_frontend interface.
static void
gfx_console_frontend_set_grab_button(
        conf_object_t *obj, int handle, gfx_console_mouse_button_t button)
{
        winsome_frontend_t *wf = from_obj(obj);
        if (wf->gui)
                wf->winsome->set_grab_button(wf->gui, handle, button);
}

// Implementation of the gfx_console_frontend interface.
static void
gfx_console_frontend_set_mouse_pos(
        conf_object_t *obj, int handle, int x, int y)
{
        winsome_frontend_t *wf = from_obj(obj);
        if (wf->gui)
                wf->winsome->set_mouse_pos(wf->gui, handle, x, y);
}

// Implementation of the gfx_console_frontend interface.
static void
gfx_console_frontend_set_keyboard_leds(
        conf_object_t *obj, int handle, gfx_console_led_t led_state)
{
        winsome_frontend_t *wf = from_obj(obj);

        acquire_work_lock(wf);
        wf->update.activity = true;
        wf->update_led_state = true;
        wf->new_led_state = led_state;
        release_work_lock(wf);
}

// Implementation of the gfx_console_frontend interface.
static void
gfx_console_frontend_set_contents(
        conf_object_t *obj, int handle,
        int left, int top, int right, int bottom,
        const uint32 *data)
{
        winsome_frontend_t *wf = from_obj(obj);
        if (wf->gui && wf->visible) {
                ASSERT(data);
                acquire_work_lock(wf);
                if (!wf->update.winsome_in_sync) {
                        // Update has not yet been fetched by Winsome and
                        // contains a valid dirty rectangle: take bounding box.
                        wf->update.left = MIN(wf->update.left, left);
                        wf->update.top = MIN(wf->update.top, top);
                        wf->update.right = MAX(wf->update.right, right);
                        wf->update.bottom = MAX(wf->update.bottom, bottom);
                } else {
                        // Set new dirty rectangle.
                        wf->update.left = left;
                        wf->update.top = top;
                        wf->update.right = right;
                        wf->update.bottom = bottom;
                }
                wf->update.pixels = data;
                wf->update.winsome_in_sync = false;
                release_work_lock(wf);
        }
}

// Implementation of the gfx_console_frontend interface.
static void
gfx_console_frontend_set_text_mode(
        conf_object_t *obj, int handle, bool text_mode)
{
        winsome_frontend_t *wf = from_obj(obj);        
        if (wf->gui && wf->visible) {
                acquire_work_lock(wf);
                wf->update.text_mode = text_mode;
                release_work_lock(wf);
        }
}

// Implementation of the gfx_console_frontend interface.
static void
gfx_console_frontend_signal_text_update(conf_object_t *obj, int handle)
{
        winsome_frontend_t *wf = from_obj(obj);        
        if (wf->gui) {
                acquire_work_lock(wf);
                // Notify Winsome update thread for activity indicator purposes.
                wf->update.activity = true;
                release_work_lock(wf);
        }
}

static void
init_gui(conf_object_t *obj)
{
        winsome_frontend_t *wf = from_obj(obj);
        if (wf->gui) {
                SIM_LOG_WARNING(obj, 0,
                                "Re-initializing an existing GUI is not"
                                " supported, for console %s",
                                SIM_object_name(obj));
        } else {
                wf->gui = VT_get_object_by_name("sim.gui");
        }
        SIM_LOG_INFO(2, obj, 0, "Query Winsome object for console %s",
                     SIM_object_name(obj));
        // We may be running without the Simics GUI, e.g. batch-mode.
        if (wf->gui) {
                SIM_LOG_INFO(2, obj, 0, "Winsome object found");
                wf->winsome = SIM_C_GET_INTERFACE(
                        wf->gui, gfx_console_frontend);
                ASSERT(wf->winsome);
                wf->wc_iface = SIM_C_GET_INTERFACE(wf->gui,
                                                   winsome_console);
                ASSERT(wf->wc_iface);
        }
}

// Implementation of the gfx_console_frontend interface.
static int
gfx_console_frontend_start(conf_object_t *obj, conf_object_t *backend)
{
        winsome_frontend_t *wf = from_obj(obj);
        init_gui(obj);
        if (wf->gui) {
                ASSERT(backend);
                wf->backend = backend;
                wf->input = SIM_C_GET_INTERFACE(
                        wf->backend, gfx_console_backend);
                ASSERT(wf->input);
                ASSERT(wf->winsome);
                wf->winsome_handle = wf->winsome->start(wf->gui, obj);
                wf->update.winsome_in_sync = true;

                wf->backend_del_hap = SIM_hap_add_callback_obj(
                        "Core_Conf_Object_Delete", wf->backend, 0,
                        (obj_hap_func_t)backend_del_cb, wf);
                wf->open_notifier = SIM_add_notifier(
                        backend, notify_open, obj,
                        console_open_notify, NULL);
                wf->close_notifier = SIM_add_notifier(
                        backend, notify_close, obj,
                        console_close_notify, NULL);

                return wf->winsome_handle;
        } else
                return 0;  
}

// Implementation of the gfx_console_frontend interface.
static void
gfx_console_frontend_stop(conf_object_t *obj, int handle)
{
        winsome_frontend_t *wf = from_obj(obj);
        if (wf->gui) {
                ASSERT(wf->winsome);
                SIM_delete_notifier(wf->backend, wf->open_notifier);
                SIM_delete_notifier(wf->backend, wf->close_notifier);

                // Disable updates and exit thread
                pthread_mutex_lock(&wf->lock);
                wf->update.activity = false;
                wf->update.winsome_in_sync = true;
                wf->exiting = true;
                while (wf->has_worker)
                        pthread_cond_wait(&wf->exit_cond, &wf->lock);
                pthread_mutex_unlock(&wf->lock);
                // Stop GUI part
                wf->winsome->stop(wf->gui, handle);
        }
}

static conf_object_t *
alloc_object(void *arg)
{
        winsome_frontend_t *wf = MM_ZALLOC(1, winsome_frontend_t);
        return to_obj(wf);
}

static void *
init_object(conf_object_t *obj, void *arg)
{
        VT_set_object_checkpointable(obj, false);
        return obj;
}

static void
finalize_instance(conf_object_t *obj)
{
        winsome_frontend_t *wf = from_obj(obj);
        pthread_mutex_init(&wf->lock, NULL);
        pthread_cond_init(&wf->exit_cond, NULL);
        VINIT(wf->update.data);

        conf_object_t *gui = VT_get_object_by_name("sim.gui");
        if (gui)
                SIM_require_object(gui);
}

static void
pre_delete_instance(conf_object_t *obj)
{
        winsome_frontend_t *wf = from_obj(obj);
        if (wf->backend)
                SIM_delete_object(wf->backend);
}

static int
delete_instance(conf_object_t *obj)
{
        winsome_frontend_t *wf = from_obj(obj);

        // Now safe to destroy mutex and condition variable used by
        // Winsome update thread.
        pthread_cond_destroy(&wf->exit_cond);
        pthread_mutex_destroy(&wf->lock);

        VFREE(wf->update.data);
        MM_FREE(wf);
        return 0;
}

conf_class_t *
init_gfx_frontend_winsome()
{
        const class_data_t cdata = {
                .alloc_object = alloc_object,
                .init_object = init_object,
                .finalize_instance = finalize_instance,
                .pre_delete_instance = pre_delete_instance,
                .delete_instance = delete_instance,
                .class_desc = "a " WINSOME_FRONTEND_STR,
                .description = "The <class>"
                WINSOME_FRONTEND_STR "</class> class."
        };
        conf_class_t *class = SIM_register_class(WINSOME_FRONTEND_STR, &cdata);
        ASSERT(class);
        
        static const gfx_console_frontend_interface_t gcf_iface = {
                .set_title = gfx_console_frontend_set_title,
                .set_size = gfx_console_frontend_set_size,
                .set_contents = gfx_console_frontend_set_contents,
                .invalidate_contents = gfx_console_frontend_invalidate_contents,
                .set_visible = gfx_console_frontend_set_visible,
                .start = gfx_console_frontend_start,
                .stop = gfx_console_frontend_stop,
                .set_grab_mode = gfx_console_frontend_set_grab_mode,
                .set_grab_modifier = gfx_console_frontend_set_grab_modifier,
                .set_grab_button = gfx_console_frontend_set_grab_button,
                .set_mouse_pos = gfx_console_frontend_set_mouse_pos,
                .set_text_mode = gfx_console_frontend_set_text_mode,
                .signal_text_update = gfx_console_frontend_signal_text_update,
                .set_keyboard_leds = gfx_console_frontend_set_keyboard_leds,
        };
        SIM_REGISTER_INTERFACE(class, gfx_console_frontend, &gcf_iface);

        static const gui_console_backend_interface_t gui_iface = {
                .start = gui_console_backend_start,
                .stop = gui_console_backend_stop,
        };
        SIM_REGISTER_INTERFACE(class, gui_console_backend, &gui_iface);

        notify_open = SIM_notifier_type(SYMBOL_TO_STRING(NOTIFY_OPEN));
        notify_close = SIM_notifier_type(SYMBOL_TO_STRING(NOTIFY_CLOSE));
        return class;
}
