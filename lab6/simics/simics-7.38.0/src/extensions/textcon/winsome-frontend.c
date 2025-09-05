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

#include <simics/util/os.h>
#include <simics/base/log.h>
#include <simics/base/notifier.h>
#include <simics/simulator/callbacks.h>
#include <simics/simulator/conf-object.h>
#include <simics/simulator/hap-consumer.h>
#include <simics/simulator-iface/consoles.h>

#include <pthread.h>

// Linked list of pending updates to the Winsome side.
typedef struct winsome_update_list winsome_update_list_t;
struct winsome_update_list {
        winsome_update_t update;
        winsome_update_list_t *next;
};

typedef struct {
        conf_object_t obj;

        // Handle returned by the Winsome side which identifies this console
        // object to the winsome singleton object.
        int winsome_handle;

        // The Winsome singleton.
        conf_object_t *gui;
        const text_console_frontend_interface_t *winsome;
        const winsome_console_interface_t *wc_iface;

        // Associated console backend.
        conf_object_t *backend;
        const text_console_backend_interface_t *input;

        // Console Python object pointer
        void *console_window;

        /* The following data is changed from simulation thread and
           update/GUI threads, hence protected by this lock. */
        pthread_mutex_t lock;

        // Winsome update queue.
        winsome_update_list_t *updates;

        // We need O(1) access to the last node of the queue.
        winsome_update_list_t *last_update;

        // Indicates if there has been non-screen related console activity.
        bool activity;

        // Update thread state
        bool has_worker;
        bool exiting;
        pthread_cond_t exit_cond;

        // GUI informs as upon start/stop
        bool gui_running;

        // Console state that we store here to avoid having to query the
        // backend object.
        int width;
        int height;
        int cursor_x;
        int cursor_y;

        /* End of lock protected data */

        bool visible;

        // Backend deletion hap callback
        hap_handle_t backend_del_hap;

        // Console visibility notifier handles
        notifier_handle_t *open_notifier;
        notifier_handle_t *close_notifier;
} winsome_frontend_t;

// Console visibility notifiers
static notifier_type_t notify_open;
static notifier_type_t notify_close;

typedef char chr_type_t;

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


// Allocate and append a node at the end of the Winsome update queue,
// and return the node.
static winsome_update_list_t *
append_update_node(winsome_frontend_t *wf)
{
        winsome_update_list_t *node = MM_ZALLOC(1, winsome_update_list_t);
        if (wf->last_update) {
                // Point to new last node.
                wf->last_update->next = node;
                wf->last_update = node;
        } else {
                // Single element list.
                wf->updates = wf->last_update = node;
        }
        return node;
}

// Remove and free the first node of the Winsome update queue.
// Return the update message the node contains.
static winsome_update_t
remove_first_node(winsome_frontend_t *wf)
{
        winsome_update_list_t *update = wf->updates;
        ASSERT(update);
        winsome_update_t data = update->update;
        wf->updates = update->next;
        if (!wf->updates)
                wf->last_update = NULL;
        MM_FREE(update);
        return data;
}

static void
free_update_list_node(winsome_update_list_t *list)
{
        if (list->update.type == Winsome_Update_Text_Cursor) {
                MM_FREE(list->update.u.text.text);
                MM_FREE(list->update.u.text.attrib);
        } else if (list->update.type == Winsome_Update_Refresh) {
                MM_FREE(list->update.u.refresh.text);
                MM_FREE(list->update.u.refresh.attrib);
                MM_FREE(list->update.u.refresh.sb_text);
                MM_FREE(list->update.u.refresh.sb_attrib);
        }
        MM_FREE(list);
}

static winsome_update_list_t *
truncate_update_list_tail(winsome_update_list_t *list)
{
        if (list) {
                winsome_update_list_t *last =
                        truncate_update_list_tail(list->next);
                if (!last) {
                        if (list->update.type != Winsome_Update_Resize) {
                                free_update_list_node(list);
                                return NULL;
                        } else {
                                list->next = NULL;
                                return list;
                        }
                } else
                        return last;
        } else
                return NULL;
}

// Remove all updates after last resize
static void
truncate_update_list(winsome_frontend_t *wf)
{
        wf->last_update = truncate_update_list_tail(wf->updates);
        if (!wf->last_update)
                wf->updates = NULL;
}

// Free all data associated with the Winsome update queue.
static void
free_update_list(winsome_frontend_t *wf)
{
        while (wf->updates) {
                winsome_update_list_t *node = wf->updates->next;
                free_update_list_node(wf->updates);
                wf->updates = node;
        }
        wf->last_update = NULL;
}

static void
send_gui_update(winsome_frontend_t *wf, winsome_update_t *wu)
{
        switch (wu->type) {
        case Winsome_Update_Activity:
                wf->wc_iface->activity(wf->gui, wf->console_window);
                break;
        case Winsome_Update_Resize:
                wf->wc_iface->resize(
                        wf->gui, wf->console_window,
                        wu->u.resize.width, wu->u.resize.height);
                break;
        case Winsome_Update_Refresh: {
                size_t len = wu->u.refresh.width * wu->u.refresh.height;
                size_t sb = wu->u.refresh.width * wu->u.refresh.sb_size;
                wf->wc_iface->refresh(
                        wf->gui, wf->console_window,
                        (bytes_t){
                                .data = (uint8 *)wu->u.refresh.text,
                                .len = len * sizeof *wu->u.refresh.text },
                        (bytes_t){
                                .data = (uint8 *)wu->u.refresh.attrib,
                                .len = len * sizeof *wu->u.refresh.attrib },
                        (bytes_t){
                                .data = (uint8 *)wu->u.refresh.sb_text,
                                .len = sb * sizeof *wu->u.refresh.sb_text },
                        (bytes_t){
                                .data = (uint8 *)wu->u.refresh.sb_attrib,
                                .len = sb * sizeof *wu->u.refresh.sb_attrib },
                        wu->u.refresh.sb_size,
                        wu->u.refresh.cursor_x, wu->u.refresh.cursor_y);
                MM_FREE(wu->u.refresh.text);
                MM_FREE(wu->u.refresh.attrib);
                MM_FREE(wu->u.refresh.sb_text);
                MM_FREE(wu->u.refresh.sb_attrib);
                break;
        }
        case Winsome_Update_Append:
                wf->wc_iface->append(
                        wf->gui, wf->console_window,
                        wu->u.sb.num_lines,
                        (bytes_t){
                                .data = (uint8 *)wu->u.sb.text,
                                .len = (wu->u.sb.text_len
                                        * sizeof *wu->u.sb.text) },
                        (bytes_t){
                                .data = (uint8 *)wu->u.sb.attrib,
                                .len = (wu->u.sb.text_len
                                        * sizeof *wu->u.sb.attrib) },
                        wu->u.sb.cursor_x, wu->u.sb.cursor_y);
                MM_FREE(wu->u.sb.text);
                MM_FREE(wu->u.sb.attrib);
                break;
        case Winsome_Update_Text_Cursor: {
                bytes_t text = { 0 }, attrib = { 0 };
                if (wu->u.text.left >= 0) {
                        size_t len = (wu->u.text.right - wu->u.text.left + 1)
                                * (wu->u.text.bottom - wu->u.text.top + 1);
                        text = (bytes_t){
                                .data = (uint8 *)wu->u.text.text,
                                .len = len * sizeof *wu->u.text.text };
                        attrib = (bytes_t){
                                .data = (uint8 *)wu->u.text.attrib,
                                .len = len * sizeof *wu->u.text.attrib };
                }
                wf->wc_iface->text(
                        wf->gui, wf->console_window,
                        wu->u.text.left, wu->u.text.top,
                        wu->u.text.right, wu->u.text.bottom,
                        text, attrib,
                        wu->u.text.cursor_x, wu->u.text.cursor_y);
                MM_FREE(wu->u.text.text);
                MM_FREE(wu->u.text.attrib);
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
                // Exit thread if no current updates
                if (!wf->gui_running || wf->updates == NULL) {
                        wf->has_worker = false;
                        if (wf->exiting)
                                pthread_cond_signal(&wf->exit_cond);
                        break;
                }

                // Get latest screen updates
                winsome_update_t data = remove_first_node(wf);
                if (data.type == Winsome_Update_Activity)
                        wf->activity = false;
                pthread_mutex_unlock(&wf->lock);
                ASSERT(data.type != Winsome_Update_Nothing);
                // Send screen update to Python
                send_gui_update(wf, &data);
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
enqueue_activity_update(winsome_frontend_t *wf)
{
        // Avoid sending multiple activity events
        if (!wf->activity) {
                acquire_work_lock(wf);
                winsome_update_list_t *node = append_update_node(wf);
                winsome_update_t update = {0};
                update.type = Winsome_Update_Activity;
                node->update = update;
                wf->activity = true;
                release_work_lock(wf);
        }

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

// Implementation of the text_console_frontend interface.
static void
text_console_frontend_set_title(conf_object_t *obj, int handle,
                                  const char *short_title,
                                  const char *long_title)
{
        winsome_frontend_t *wf = from_obj(obj);
        if (wf->gui) {
                ASSERT(wf->winsome);
                wf->winsome->set_title(wf->gui, handle,
                                        short_title, long_title);
        }
}

// Implementation of the text_console_frontend interface.
static void
text_console_frontend_set_size(conf_object_t *obj, int handle,
                                 int width, int height)
{
        winsome_frontend_t *wf = from_obj(obj);
        if (wf->gui) {
                acquire_work_lock(wf);
                winsome_update_list_t *node = append_update_node(wf);
                winsome_update_t update = {0};
                update.type = Winsome_Update_Resize;
                update.u.resize.width = width;
                update.u.resize.height = height;
                node->update = update;

                wf->width = width;
                wf->height = height;

                release_work_lock(wf);
        }
}

// Implementation of the text_console_frontend interface.
static void
text_console_frontend_set_visible(conf_object_t *obj,
                                    int handle, bool visible)
{
        winsome_frontend_t *wf = from_obj(obj);
        if (wf->gui) {                
                ASSERT(wf->winsome);
                frontend_set_visible(wf, visible);
                wf->winsome->set_visible(wf->gui, handle, visible);
        }
}

// Implementation of the text_console_frontend interface.
static void
text_console_frontend_set_contents(
        conf_object_t *obj, int handle,
        int top, int left, int bottom, int right,
        const char *text, const text_console_attrib_t *attrib)
{
        winsome_frontend_t *wf = from_obj(obj);
        // For performance purposes, do not add to the update queue
        // if the frontend is invisible.
        if (wf->gui && wf->visible) {
                acquire_work_lock(wf);
                winsome_update_list_t *node = wf->last_update;
                
                if (node && node->update.type == Winsome_Update_Text_Cursor) {
                        // Last node has correct type.
                        if (node->update.u.text.left >= 0) {
                                // Node already has a valid dirty region,
                                // hence take bounding box.
                                node->update.u.text.left =
                                        MIN(node->update.u.text.left, left);
                                node->update.u.text.top =
                                        MIN(node->update.u.text.top, top);
                                node->update.u.text.right =
                                        MAX(node->update.u.text.right, right);
                                node->update.u.text.bottom =
                                        MAX(node->update.u.text.bottom, bottom);
                        } else {
                                // Node was only used for cursor update.
                                node->update.u.text.left = left;
                                node->update.u.text.right = right;
                                node->update.u.text.top = top;
                                node->update.u.text.bottom = bottom;
                        }
                } else if (node && node->update.type == Winsome_Update_Append
                           && (bottom - top + 1) <= node->update.u.sb.num_lines
                           && top >= wf->height - node->update.u.sb.num_lines) {
                        // For performance purposes, modify append nodes that
                        // have not yet been fetched by Winsome, if the dirty
                        // region is inside the appended lines, to avoid
                        // creating new nodes.
                        ASSERT(top < wf->height);
                        int line_len = right - left + 1;
                        for (int line = top; line <= bottom; line++) {
                                int append_line =
                                        line - (wf->height
                                                - node->update.u.sb.num_lines);
                                ASSERT(append_line >= 0);
                                int idx = append_line * wf->width + left;
                                int start = wf->width * line + left;
                                ASSERT(idx < node->update.u.sb.text_len);
                                memcpy(node->update.u.sb.text + idx,
                                       text + start,
                                       line_len * sizeof *text);
                                memcpy(node->update.u.sb.attrib + idx,
                                       attrib + start,
                                       line_len * sizeof *attrib);
                        }
                } else {
                        // We can end up here if the update queue is empty.
                        node = append_update_node(wf);
                        winsome_update_t update = {0};
                        update.type = Winsome_Update_Text_Cursor;
                        update.u.text.left = left;
                        update.u.text.right = right;
                        update.u.text.top = top;
                        update.u.text.bottom = bottom;
                        // Mark cursor update data as invalid.
                        update.u.text.cursor_x = -1;
                        node->update = update;
                }

                if (node->update.type == Winsome_Update_Text_Cursor) {
                        // We must store dirty part of screen and attributes
                        // in the update queue, to obtain a consistent
                        // frontend display.
                        int width = node->update.u.text.right
                                    - node->update.u.text.left + 1;
                        int height = node->update.u.text.bottom
                                     - node->update.u.text.top + 1;
                        int nelems = width * height;
                        node->update.u.text.text =
                                MM_REALLOC(node->update.u.text.text,
                                           nelems, chr_type_t);
                        node->update.u.text.attrib =
                                MM_REALLOC(node->update.u.text.attrib,
                                           nelems, text_console_attrib_t);
                        
                        int start = node->update.u.text.top * wf->width
                                    + node->update.u.text.left;
                        for (int line = 0; line < height; line++) {
                                memcpy(node->update.u.text.text + line * width,
                                       text + start + line * wf->width,
                                       width * sizeof *text);
                                memcpy(node->update.u.text.attrib
                                       + line * width,
                                       attrib + start + line * wf->width,
                                       width * sizeof *attrib);
                        }
                }
                release_work_lock(wf);
        } else if (wf->gui)
                enqueue_activity_update(wf);
}

// Implementation of the text_console_frontend interface.
static void
text_console_frontend_refresh_screen(
        conf_object_t *obj, int handle,
        const chr_type_t *text, const text_console_attrib_t *attrib,
        const chr_type_t *sb_text, const text_console_attrib_t *sb_attrib,
        int scrollback_size)
{
        winsome_frontend_t *wf = from_obj(obj);
        // Refresh screen happens even when frontend is invisible.
        // Generally, it is only done when the frontend becomes visible.
        if (wf->gui) {
                // Since we refresh the whole screen, we can empty the queue
                // after the last resize message.
                acquire_work_lock(wf);
                truncate_update_list(wf);

                winsome_update_list_t *node = append_update_node(wf);
                winsome_update_t update;
                update.type = Winsome_Update_Refresh;
                update.u.refresh.sb_size = scrollback_size;
                update.u.refresh.width = wf->width;
                update.u.refresh.height = wf->height;
                update.u.refresh.cursor_x = wf->cursor_x;
                update.u.refresh.cursor_y = wf->cursor_y;
                node->update = update;

                int screen_size = wf->height * wf->width;
                node->update.u.refresh.text = MM_MALLOC(
                        screen_size, chr_type_t);
                memcpy(node->update.u.refresh.text, text,
                       screen_size * sizeof *text);      
                node->update.u.refresh.attrib =
                        MM_MALLOC(screen_size, text_console_attrib_t);
                memcpy(node->update.u.refresh.attrib, attrib,
                       screen_size * sizeof *attrib);

                int sb_size = scrollback_size * wf->width;
                node->update.u.refresh.sb_text = MM_MALLOC(sb_size, chr_type_t);
                memcpy(node->update.u.refresh.sb_text, sb_text,
                       sb_size * sizeof *sb_text);       
                node->update.u.refresh.sb_attrib =
                        MM_MALLOC(sb_size, text_console_attrib_t);
                memcpy(node->update.u.refresh.sb_attrib, sb_attrib,
                       sb_size * sizeof *sb_attrib);

                release_work_lock(wf);
        }
}

// Implementation of the text_console_frontend interface.
static void
text_console_frontend_append_text(
        conf_object_t *obj, int handle, int num_lines, const char *text,
        const text_console_attrib_t *attrib)
{
        winsome_frontend_t *wf = from_obj(obj);
        // For performance purposes, do not add to the update queue
        // if the frontend is invisible.
        if (wf->gui && wf->visible) {
                acquire_work_lock(wf);
                winsome_update_list_t *node = wf->last_update;
                
                if (!node || node->update.type != Winsome_Update_Append) {
                        // Must add a new update node.
                        node = append_update_node(wf);
                        winsome_update_t update = {0};
                        update.type = Winsome_Update_Append;
                        // Mark cursor update data as invalid.
                        update.u.sb.cursor_x = -1;
                        update.u.sb.num_lines = num_lines;
                        node->update = update;
                } else {
                        // Last node already an append, just add more lines.
                        ASSERT(node
                               && node->update.type == Winsome_Update_Append);
                        node->update.u.sb.num_lines += num_lines;
                }
                ASSERT(node->update.type == Winsome_Update_Append);
                int new_text_len = num_lines * wf->width;
                node->update.u.sb.text_len += new_text_len;

                // We must store the append lines in the update queue,
                // to obtain a consistent frontend display.
                node->update.u.sb.text = MM_REALLOC(
                        node->update.u.sb.text,
                        node->update.u.sb.text_len, char);
                node->update.u.sb.attrib = MM_REALLOC(
                        node->update.u.sb.attrib,
                        node->update.u.sb.text_len,
                        text_console_attrib_t);
                memcpy(node->update.u.sb.text
                       + node->update.u.sb.text_len - new_text_len,
                       text, new_text_len * sizeof *text);
                memcpy(node->update.u.sb.attrib
                       + node->update.u.sb.text_len - new_text_len,
                       attrib, new_text_len * sizeof *attrib);

                release_work_lock(wf);
        } else if (wf->gui)
                enqueue_activity_update(wf);
}

// Implementation of the text_console_frontend interface.
static void
text_console_frontend_set_cursor_pos(conf_object_t *obj,
                                       int handle, int row, int col)
{
        winsome_frontend_t *wf = from_obj(obj);
        wf->cursor_x = col;
        wf->cursor_y = row;
        // For performance purposes, do not add to the update queue
        // if the frontend is invisible.
        if (wf->gui && wf->visible) {
                acquire_work_lock(wf);
                winsome_update_list_t *node = wf->last_update;
                // For performance purposes, we store cursor data in every
                // update message, to avoid adding extra nodes in the queue.
                if (node && node->update.type == Winsome_Update_Text_Cursor) {
                        // Only need to send latest cursor data.
                        node->update.u.text.cursor_x = col;
                        node->update.u.text.cursor_y = row;
                } else if (node && node->update.type
                                   == Winsome_Update_Refresh) {
                        // Only need to send latest cursor data.
                        node->update.u.refresh.cursor_x = col;
                        node->update.u.refresh.cursor_y = row;
                } else if (node && node->update.type == Winsome_Update_Append) {
                        // Only need to send latest cursor data.
                        node->update.u.sb.cursor_x = col;
                        node->update.u.sb.cursor_y = row;
                } else {
                        node = append_update_node(wf);
                        winsome_update_t update = {0};
                        update.type = Winsome_Update_Text_Cursor;
                        update.u.text.cursor_x = col;
                        update.u.text.cursor_y = row;
                        // Mark invalid update region.
                        update.u.text.left = -1;
                        node->update = update;
                }

                release_work_lock(wf);
        } else if (wf->gui)
                enqueue_activity_update(wf);
}

// Implementation of the text_console_frontend interface.
static void
text_console_frontend_set_max_scrollback_size(conf_object_t *obj,
                                            int handle, int num_lines)
{
        winsome_frontend_t *wf = from_obj(obj);
        if (wf->gui) {
                ASSERT(wf->winsome);
                wf->winsome->set_max_scrollback_size(wf->gui, handle,
                                                     num_lines);
        }
}

// Implementation of the text_console_frontend interface.
static void
text_console_frontend_set_default_colours(
        conf_object_t *obj, int handle,
        uint32 default_fg_col, uint32 default_bg_col)
{
        winsome_frontend_t *wf = from_obj(obj);
        if (wf->gui) {
                ASSERT(wf->winsome);
                wf->winsome->set_default_colours(
                        wf->gui, handle, default_fg_col, default_bg_col);
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
        SIM_LOG_INFO(2, obj, 0,
                     "Query Winsome object for console %s",
                     SIM_object_name(obj));

        // We may be running without the Simics GUI, e.g. batch-mode.
        if (wf->gui) {
                SIM_LOG_INFO(2, obj, 0, "Winsome object found");
                wf->winsome = SIM_C_GET_INTERFACE(
                        wf->gui, text_console_frontend);
                ASSERT(wf->winsome);
                wf->wc_iface = SIM_C_GET_INTERFACE(wf->gui,
                                                   winsome_console);
                ASSERT(wf->wc_iface);
        }
}

// Implementation of the text_console_frontend interface.
static int
text_console_frontend_start(conf_object_t *obj, conf_object_t *backend)
{
        winsome_frontend_t *wf = from_obj(obj);
        init_gui(obj);
        if (wf->gui) {
                SIM_LOG_INFO(2, obj, 0, "Winsome frontend open for console %s",
                             SIM_object_name(backend));
                ASSERT(backend);
                wf->backend = backend;
                wf->input = SIM_C_GET_INTERFACE(
                        wf->backend, text_console_backend);
                ASSERT(wf->winsome);
                wf->winsome_handle = wf->winsome->start(wf->gui, obj);

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

// Implementation of the text_console_frontend interface.
static void
text_console_frontend_stop(conf_object_t *obj, int handle)
{
        winsome_frontend_t *wf = from_obj(obj);
        if (wf->gui) {
                ASSERT(wf->winsome);
                SIM_delete_notifier(wf->backend, wf->open_notifier);
                SIM_delete_notifier(wf->backend, wf->close_notifier);

                // Empty Winsome update queue and exit thread
                pthread_mutex_lock(&wf->lock);
                free_update_list(wf);
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
        MM_FREE(wf);
        return 0;
}

conf_class_t *
init_text_frontend_winsome()
{
        const class_data_t cdata = {
                .alloc_object = alloc_object,
                .init_object = init_object,
                .finalize_instance = finalize_instance,
                .pre_delete_instance = pre_delete_instance,
                .delete_instance = delete_instance,
                .class_desc = "text-frontend-winsome",
                .description =
                    "The <class>text-frontend-winsome</class> class.",
        };
        conf_class_t *class = SIM_register_class("text-frontend-winsome",
                                                 &cdata);
        ASSERT(class);
        
        static const text_console_frontend_interface_t vcf_iface = {
                .set_title = text_console_frontend_set_title,
                .set_size = text_console_frontend_set_size,
                .set_contents = text_console_frontend_set_contents,
                .set_cursor_pos = text_console_frontend_set_cursor_pos,
                .set_max_scrollback_size =
                        text_console_frontend_set_max_scrollback_size,
                .set_visible = text_console_frontend_set_visible,
                .append_text = text_console_frontend_append_text,
                .start = text_console_frontend_start,
                .stop = text_console_frontend_stop,
                .refresh_screen = text_console_frontend_refresh_screen,
                .set_default_colours =
                        text_console_frontend_set_default_colours,
        };
        SIM_REGISTER_INTERFACE(class, text_console_frontend, &vcf_iface);

        static const gui_console_backend_interface_t gui_iface = {
                .start = gui_console_backend_start,
                .stop = gui_console_backend_stop,
        };
        SIM_REGISTER_INTERFACE(class, gui_console_backend, &gui_iface);
        
        notify_open = SIM_notifier_type(SYMBOL_TO_STRING(NOTIFY_OPEN));
        notify_close = SIM_notifier_type(SYMBOL_TO_STRING(NOTIFY_CLOSE));
        return class;
}
