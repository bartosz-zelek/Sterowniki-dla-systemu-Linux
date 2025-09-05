/*
  © 2012 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include "vnc.h"
#include <pthread.h>
#include <time.h>
#include <zlib.h>

// Obtain all necessary keysym definitions.
#define XK_MISCELLANY
#define XK_LATIN1
#define XK_XKB_KEYS
#include <X11/keysymdef.h>

#include <simics/base-types.h>
#include <simics/base/iface-call.h>
#include <simics/simulator-api.h>
#include <simics/util/genrand.h>
#include <simics/util/os.h>
#include <simics/util/vect.h>
#include <simics/model-iface/sim-keys.h>
#include <simics/simulator-iface/consoles.h>
#include <simics/model-iface/external-connection.h>

#include "gfx-console.h"
#include "output.h"
#include "input.h"
#include "rect.h"
#include "gfx-inline.h"

#include "crypto-interface.h"

// VNC client states used during setup.
typedef enum {
        VNC_Client_State_Version,
        VNC_Client_State_Select_Auth,
        VNC_Client_State_Auth,
        VNC_Client_State_Auth_Reply,
        VNC_Client_State_Init,
        VNC_Client_State_Running,
} vnc_client_state_t;

// VNC commands sent from client to server.
// We read all defined commands, but CutText is a NOP.
typedef enum {
        VNC_Client_Cmd_SetPixelFormat = 0,
        VNC_Client_Cmd_SetEncodings = 2,
        VNC_Client_Cmd_UpdateRequest = 3,
        VNC_Client_Cmd_KeyEvent = 4,
        VNC_Client_Cmd_PointerEvent = 5,
        VNC_Client_Cmd_CutText = 6,
        VNC_Client_Cmd_EnableContinuousUpdates = 150,
} vnc_client_cmd_t;

// VNC commands sent from server to client.
// We only implement the frame buffer update message.
typedef enum {
        VNC_Server_Cmd_Update = 0,
        VNC_Server_Cmd_SetColourMapEntries = 1,
        VNC_Server_Cmd_EndContinuousUpdates = 150,
} vnc_server_cmd_t;

// Supported VNC authentication types.        
typedef enum {
        VNC_Auth_None = 1,
        VNC_Auth_Password = 2,
} vnc_auth_type_t;

// Supported VNC encodings
typedef enum {
        // Raw pixel data
        VNC_Encoding_Raw = 0,
        // TightVNC encoding
        VNC_Encoding_Tight = 7,
        // Compressed tiled rectangles using ZLIB
        VNC_Encoding_ZRLE = 0x10,
        // DesktopSize pseudo-encoding
        VNC_Encoding_Resize = (uint32)-223,
        // ContinuousUpdates pseudo-encoding
        VNC_Encoding_Continuous = (uint32)-313,
} vnc_client_encoding_t;

typedef struct {
        int major;
        int minor;
} vnc_version_t;
        
typedef struct vnc_client vnc_client_t;

// VNC pixel format data, for server and each client.
typedef struct {
        uint8 bpp;
        uint8 depth;
        uint8 endian;
        uint8 truecolor;
        uint16 red_max;
        uint16 green_max;
        uint16 blue_max;
        uint8 red_shift;
        uint8 green_shift;
        uint8 blue_shift;
        uint8 pad[3];
} vnc_pixelformat_t;

typedef struct {
        void (*split_rects)(vnc_client_t *cl, gfx_rect_list_t *rects);
        void (*encode_rect)(vnc_client_t *cl,
                            const uint32 *src, int x, int y, int w, int h,
                            int src_stride, vnc_pixelformat_t format);
} encoder_t;

typedef VECT(uint8) output_data_t;

// Data specific to a VNC client.
struct vnc_client {
        /* The following data is unchanged during the client lifetime */
        vnc_console_t *vc;                   /* Containing object. */

        bool connected;

        IFACE_OBJ(external_connection_ctl) server;

        /* This data is accessed from Cell Context or Global Context only. */

        // We store clients as a linked list, to avoid touching connected
        // clients data (realloc etc) when expanding the list, hence avoiding
        // threading problems.
        vnc_client_t *next;

        // ZLIB stream used for compressed encodings
        z_stream tight_strm;
        z_stream zrle_strm;

        // Encoding output buffers
        unsigned encode_pos;
        output_data_t encode_buf;
        output_data_t tight_input_buf;

        /* End of Cell Context or Global Context only data */

        /* The following data is used from Threaded Context only. */

        // Can client resize its window?
        bool can_resize;
        bool need_resize;

        // Client state, used during connection.
        vnc_client_state_t state;

        // VNC protocol version.
        vnc_version_t version;

        // Should we send a colour map?
        bool send_colour_map;

        // Authentication type used by client
        vnc_auth_type_t auth_type;

        // Authentication challenge sent to client
        uint8 challenge[16];

        // Is client authenticated?
        bool authenticated;

        // Pseudo-random state used to generate the challenge
        rand_state_t *rand_state;

        /* End of foreign context only data */

        /* The following data is used used from
           both Cell Context and Threaded Context (socket notifier callbacks),
           protected by this mutex. */
        pthread_mutex_t client_mutex;

        // Client using continuous updates?
        bool continuous_updates;
        
        // Chosen rectangle encoding
        vnc_client_encoding_t encoding;

        // Supported pixel format for this client. We convert from the server
        // format to this when sending data.
        vnc_pixelformat_t format;

        // Dirty rectangle for this client.
        // TODO Store list of rectangles to avoid bounding box?
        gfx_rect_t update_request;

        gfx_rect_t continuous_request;

        // Dirty server rectangles, not yet fetched by client.
        gfx_rect_list_t dirty;

        // Rectangle encoding function
        encoder_t encoder;

        // Client screen size, since client may not support resize.
        const uint32 *screen;
        int width;
        int height;

        // Has server resized window? (override incremental flag)
        bool resize;

        /* End of client_mutex protected data */

        // Circular socket output buffer.
        struct {
                /* This struct data is used from Cell Context and
                   output thread, hence protected by this mutex.
                   NOTE: only one (current) buffer protected. */
                pthread_mutex_t lock;
                // Double buffered output
                output_data_t bufs[2];
                int buf_num;
                // Output thread running?
                bool has_worker;

                // Exit synchronisation
                bool exiting;
                pthread_cond_t exit_cond;
        } output;
};

struct vnc_console {
        gfx_console_t *gc;             /* Sad shortcut to containing object. */

        // Linked list of connected VNC clients.
        vnc_client_t *clients;

        // Server supported pixel format.
        vnc_pixelformat_t format;

        // Screen size
        const uint32 *screen;
        int screen_width;
        int screen_height;

        // Used to send keyboard and mouse input to console.
        // FIXME: completely pointless, remove!
        const gfx_console_backend_interface_t *iface;
        
        // zlib compression level in compressed encoding
        int zlib_level;

        // Allow compressed encodings
        bool allow_compression;

        // Keysym mapping
        ht_int_table_t keymap;

        // VNC server password, or NULL
        char *password;
        IFACE_OBJ(vnc_des) des;
};

// Server to client message sent after successful client authentication.
typedef struct {
        // Initial screen size.
        uint16 width;
        uint16 height;

        // Server preferred pixel format.
        vnc_pixelformat_t format;

        // Server name, which we don't use.
        uint32 name_length;
} vnc_cmd_server_init_t;

// Need separate rect definition since VNC uses 16-bit values.
typedef struct {
        uint16 x_pos;
        uint16 y_pos;
        uint16 width;
        uint16 height;
} vnc_rect_t;

// Server-to-client command header for frame buffer update and resize messages.
typedef struct {
        uint8 type;

        uint8 pad;

        // Number of rectangles.
        uint16 num_rects;
} vnc_cmd_header_t;

typedef struct {
        // Dirty rectangle for frame buffer update, or new screen size.
        vnc_rect_t rect;

        // Encoding of rectangle contents, or DesktopSize pseudo-encoding.
        uint32 encoding;
} vnc_rect_header_t;

// Server-to-client set colour map message header.
typedef struct {
        uint8 type;
        uint8 pad;
        uint16 first_col;
        uint16 ncols;
} vnc_col_map_header_t;

// Client-to-server frame buffer update request message.
typedef struct {
        uint8 type;

        // true => the client is only interested in updates.
        // false => the client has lost the specified area.
        uint8 incremental;

        // Rectangle that the server should update.
        vnc_rect_t rect;
} vnc_fb_update_event_t;

// Client-to-server mouse pointer update message.
typedef struct {
        uint8 type;
        
        // Button state.
        uint8 mask;

        // Mouse position.
        uint16 x;
        uint16 y;
} vnc_pointer_event_t;

// Client-to-server key press/release message.
typedef struct {
        uint8 type;
        
        uint8 down;     /* If true, the key was pressed, otherwise released. */

        uint16 pad;

        // X keysym code.
        uint32 keysym;
} vnc_key_event_t;

// Client-to-server SetEncodings message data.
typedef struct {
        uint8 type;
        uint8 pad;
        uint16 num_encodings;
} vnc_num_encodings_t;

// Client-to-server CutText message data.
typedef struct {
        uint8 type;
        uint8 pad[3];
        uint32 text_len;
} vnc_client_cut_text_t;

static void send_screen_updates(vnc_client_t *cl);
static void vnc_client_update_size(vnc_console_t *vc, vnc_client_t *cl,
                                   int width, int height);

// VNC version identification strings have the same length
#define VNC_VERSION_STR_LEN 12

// Colour map for 8-bit non-true colour case.
typedef struct { uint16 r, g, b; } colour_map_t;

static vnc_console_t *
vnc_data(conf_object_t *obj)
{
        return from_obj(obj)->vnc_data;
}

static conf_object_t *
vc_obj(vnc_console_t *vc) { return to_obj(vc->gc); }

static bool
vnc_version_eq(vnc_version_t ver, int major, int minor)
{
        return ver.major == major && ver.minor == minor;
}

// Wrap logging so that it can be disabled.
#define CALLBACK_LOG(lvl, obj, fmt, ...)                \
        SIM_LOG_INFO(lvl, obj, Gfx_Console_Log_VNC,     \
                     fmt, ##__VA_ARGS__)

static sim_key_t
lookup_key(vnc_console_t *vc, uint32 keysym)
{
        return (sim_key_t)ht_lookup_int(&vc->keymap, keysym);
}

static void
set_default_keymap(vnc_console_t *vc)
{
        ht_insert_int(&vc->keymap, XK_BackSpace, (void *)SK_BACKSPACE);
        ht_insert_int(&vc->keymap, XK_Tab, (void *)SK_TAB);
        ht_insert_int(&vc->keymap, XK_Return, (void *)SK_ENTER);
        ht_insert_int(&vc->keymap, XK_Escape, (void *)SK_ESC);
        ht_insert_int(&vc->keymap, XK_Insert, (void *)SK_GR_INSERT);
        ht_insert_int(&vc->keymap, XK_Delete, (void *)SK_GR_DELETE);
        ht_insert_int(&vc->keymap, XK_Home, (void *)SK_GR_HOME);
        ht_insert_int(&vc->keymap, XK_End, (void *)SK_GR_END);
        ht_insert_int(&vc->keymap, XK_Page_Up, (void *)SK_GR_PG_UP);
        ht_insert_int(&vc->keymap, XK_Page_Down, (void *)SK_GR_PG_DOWN);
        ht_insert_int(&vc->keymap, XK_Left, (void *)SK_GR_LEFT);
        ht_insert_int(&vc->keymap, XK_Up, (void *)SK_GR_UP);
        ht_insert_int(&vc->keymap, XK_Right, (void *)SK_GR_RIGHT);
        ht_insert_int(&vc->keymap, XK_Down, (void *)SK_GR_DOWN);

        ht_insert_int(&vc->keymap, XK_F1, (void *)SK_F1);
        ht_insert_int(&vc->keymap, XK_F2, (void *)SK_F2);
        ht_insert_int(&vc->keymap, XK_F3, (void *)SK_F3);
        ht_insert_int(&vc->keymap, XK_F4, (void *)SK_F4);
        ht_insert_int(&vc->keymap, XK_F5, (void *)SK_F5);
        ht_insert_int(&vc->keymap, XK_F6, (void *)SK_F6);
        ht_insert_int(&vc->keymap, XK_F7, (void *)SK_F7);
        ht_insert_int(&vc->keymap, XK_F8, (void *)SK_F8);
        ht_insert_int(&vc->keymap, XK_F9, (void *)SK_F9);
        ht_insert_int(&vc->keymap, XK_F10, (void *)SK_F10);
        ht_insert_int(&vc->keymap, XK_F11, (void *)SK_F11);
        ht_insert_int(&vc->keymap, XK_F12, (void *)SK_F12);

        ht_insert_int(&vc->keymap, XK_Shift_L, (void *)SK_SHIFT_L);
        ht_insert_int(&vc->keymap, XK_Shift_R, (void *)SK_SHIFT_R);
        ht_insert_int(&vc->keymap, XK_Control_L, (void *)SK_CTRL_L);
        ht_insert_int(&vc->keymap, XK_Control_R, (void *)SK_CTRL_R);
        ht_insert_int(&vc->keymap, XK_Alt_L, (void *)SK_ALT_L);
        ht_insert_int(&vc->keymap, XK_Alt_R, (void *)SK_ALT_R);
        ht_insert_int(&vc->keymap, XK_Scroll_Lock, (void *)SK_SCROLL_LOCK);
        ht_insert_int(&vc->keymap, XK_Num_Lock, (void *)SK_NUM_LOCK);
        ht_insert_int(&vc->keymap, XK_Caps_Lock, (void *)SK_CAPS_LOCK);

        ht_insert_int(&vc->keymap, XK_Pause, (void *)SK_PAUSE);
        ht_insert_int(&vc->keymap, XK_Sys_Req, (void *)SK_SYSREQ);
        ht_insert_int(&vc->keymap, XK_Print, (void *)SK_PRNT_SCRN);
        ht_insert_int(&vc->keymap, XK_Menu, (void *)SK_LIST_BIT);

        ht_insert_int(&vc->keymap, XK_KP_Home, (void *)SK_KP_HOME);
        ht_insert_int(&vc->keymap, XK_KP_Up, (void *)SK_KP_UP);
        ht_insert_int(&vc->keymap, XK_KP_Page_Up, (void *)SK_KP_PG_UP);
        ht_insert_int(&vc->keymap, XK_KP_Page_Down, (void *)SK_KP_PG_DOWN);
        ht_insert_int(&vc->keymap, XK_KP_Left, (void *)SK_KP_LEFT);
        ht_insert_int(&vc->keymap, XK_KP_Right, (void *)SK_KP_RIGHT);
        ht_insert_int(&vc->keymap, XK_KP_Down, (void *)SK_KP_DOWN);
        ht_insert_int(&vc->keymap, XK_KP_Insert, (void *)SK_KP_INSERT);
        ht_insert_int(&vc->keymap, XK_KP_Delete, (void *)SK_KP_DELETE);
        ht_insert_int(&vc->keymap, XK_KP_Add, (void *)SK_GR_PLUS);
        ht_insert_int(&vc->keymap, XK_KP_Subtract, (void *)SK_GR_MINUS);
        ht_insert_int(&vc->keymap, XK_KP_Multiply, (void *)SK_GR_MULTIPLY);
        ht_insert_int(&vc->keymap, XK_KP_Enter, (void *)SK_ENTER);
        ht_insert_int(&vc->keymap, XK_KP_Separator, (void *)SK_KP_CENTER);
        ht_insert_int(&vc->keymap, XK_KP_Divide, (void *)SK_GR_DIVIDE);
        ht_insert_int(&vc->keymap, XK_KP_Decimal, (void *)SK_COMMA);
        ht_insert_int(&vc->keymap, XK_KP_Equal, (void *)SK_EQUAL);
        ht_insert_int(&vc->keymap, XK_KP_Space, (void *)SK_SPACE);
        ht_insert_int(&vc->keymap, XK_KP_Begin, (void *)SK_KP_CENTER);
        ht_insert_int(&vc->keymap, XK_KP_End, (void *)SK_KP_END);

        ht_insert_int(&vc->keymap, XK_Meta_L, (void *)SK_SUN_META_L);
        ht_insert_int(&vc->keymap, XK_Meta_R, (void *)SK_SUN_META_R);
        ht_insert_int(&vc->keymap, XK_Help, (void *)SK_SUN_HELP);
        ht_insert_int(&vc->keymap, XK_Find, (void *)SK_SUN_FIND);

        ht_insert_int(&vc->keymap, XK_space, (void *)SK_SPACE);
        ht_insert_int(&vc->keymap, XK_plus, (void *)SK_EQUAL);
        ht_insert_int(&vc->keymap, XK_minus, (void *)SK_MINUS);
        ht_insert_int(&vc->keymap, XK_apostrophe, (void *)SK_APOSTROPHE);
        ht_insert_int(&vc->keymap, XK_bracketleft, (void *)SK_LEFT_BRACKET);
        ht_insert_int(&vc->keymap, XK_bracketright, (void *)SK_RIGHT_BRACKET);
        ht_insert_int(&vc->keymap, XK_period, (void *)SK_PERIOD);
        ht_insert_int(&vc->keymap, XK_equal, (void *)SK_EQUAL);
        ht_insert_int(&vc->keymap, XK_slash, (void *)SK_SLASH);
        ht_insert_int(&vc->keymap, XK_semicolon, (void *)SK_SEMICOLON);
        ht_insert_int(&vc->keymap, XK_backslash, (void *)SK_BACKSLASH);
        ht_insert_int(&vc->keymap, XK_grave, (void *)SK_GRAVE);
        ht_insert_int(&vc->keymap, XK_comma, (void *)SK_COMMA);
        ht_insert_int(&vc->keymap, XK_braceleft, (void *)SK_LEFT_BRACKET);
        ht_insert_int(&vc->keymap, XK_braceright, (void *)SK_RIGHT_BRACKET);
        ht_insert_int(&vc->keymap, XK_less, (void *)SK_COMMA);
        ht_insert_int(&vc->keymap, XK_greater, (void *)SK_PERIOD);
        ht_insert_int(&vc->keymap, XK_colon, (void *)SK_SEMICOLON);
        ht_insert_int(&vc->keymap, XK_question, (void *)SK_SLASH);
        ht_insert_int(&vc->keymap, XK_asciitilde, (void *)SK_APOSTROPHE);
        ht_insert_int(&vc->keymap, XK_bar, (void *)SK_BACKSLASH);

#ifndef _WIN32
        ht_insert_int(&vc->keymap, XK_odiaeresis, (void *)SK_SEMICOLON);
        ht_insert_int(&vc->keymap, XK_Odiaeresis, (void *)SK_SEMICOLON);
        ht_insert_int(&vc->keymap, XK_adiaeresis, (void *)SK_APOSTROPHE);
        ht_insert_int(&vc->keymap, XK_Adiaeresis, (void *)SK_APOSTROPHE);
        ht_insert_int(&vc->keymap, XK_aring, (void *)SK_LEFT_BRACKET);
        ht_insert_int(&vc->keymap, XK_Aring, (void *)SK_LEFT_BRACKET);
        ht_insert_int(&vc->keymap, XK_dead_diaeresis, (void *)SK_BACKSLASH);
        ht_insert_int(&vc->keymap, XK_dead_circumflex, (void *)SK_BACKSLASH);
        ht_insert_int(&vc->keymap, XK_dead_acute, (void *)SK_EQUAL);
        ht_insert_int(&vc->keymap, XK_dead_grave, (void *)SK_EQUAL);
        ht_insert_int(&vc->keymap, XK_section, (void *)SK_GRAVE);
        ht_insert_int(&vc->keymap, XK_onehalf, (void *)SK_APOSTROPHE);
        ht_insert_int(&vc->keymap, XK_asterisk, (void *)SK_APOSTROPHE);
        ht_insert_int(&vc->keymap, XK_underscore, (void *)SK_MINUS);
        ht_insert_int(&vc->keymap, XK_currency, (void *)SK_4);
#endif
}

static sim_key_t
lookup_ascii_key(uint32 keysym)
{
        switch (keysym) {
        case '!': return SK_1;
        case '@': return SK_2;
        case '#': return SK_3;
        case '$': return SK_4;
        case '%': return SK_5;
        case '^': return SK_6;
        case '&': return SK_7;
        case '*': return SK_8;
        case '(': return SK_9;
        case ')': return SK_0;
        case '"': return SK_2;
        default:  return 0;
        }
}

static void
update_client_dirty(vnc_client_t *cl, gfx_rect_t dirty)
{
        // Add new dirty rectangle to list unless contained in existing ones
        bool new_rect = true;
        VFORT(cl->dirty, gfx_rect_t, r) {
                if (contains_rect(r, dirty)) {
                        // r contains dirty
                        new_rect = false;
                        break;
                }
        }

        gfx_rect_t cover = { 0 };
        uint64 total_area = 0;
        if (new_rect) {
                // Remove unnecessary rectangles
                for (int i = VLEN(cl->dirty) - 1; i >= 0; i--) {
                        if (!contains_rect(dirty, VGET(cl->dirty, i))) {
                                gfx_rect_t r = VGET(cl->dirty, i);
                                total_area += rect_area(r);
                                cover = bounding_box(cover, r);
                        } else
                                VREMOVE(cl->dirty, i);
                }
                VADD(cl->dirty, dirty);

                // Replace rectangle list with bounding box if cheaper
                total_area += rect_area(dirty);
                cover = bounding_box(cover, dirty);
                if (rect_area(cover) <= total_area) {
                        VCLEAR(cl->dirty);
                        VADD(cl->dirty, cover);
                }
        }
}

// Translate input keysym from VNC client to a sim_key_t and pass it to the
// console. 
static void
handle_key_event(vnc_console_t *vc, vnc_key_event_t *event)
{
        uint32 keysym = CONVERT_BE32(event->keysym);
        // See if key is a "special" key.
        sim_key_t key = lookup_key(vc, keysym);
        if (key == 0) {
                // Keysym is ASCII.
                if (keysym >= 'A' && keysym <= 'Z') {
                        key = keysym - 'A' + SK_A;
                } else if (keysym >= 'a' && keysym <= 'z') {
                        key = keysym - 'a' + SK_A;
                } else if (keysym >= '0' && keysym <= '9') {
                        key = keysym - '0' + SK_0;
                } else {
                        key = lookup_ascii_key(keysym);
                }
        }

        domain_lock_t *lock;
        SIM_ACQUIRE_CELL(vc_obj(vc), &lock);

        if (key > 0) {
                SIM_LOG_INFO(3, vc_obj(vc), Gfx_Console_Log_VNC,
                             "Sending key %u = 0x%x to console, sym %u = 0x%x",
                             key, key, keysym, keysym);
                vc->iface->kbd_event(vc_obj(vc), key, event->down);
        } else {
                SIM_LOG_INFO(2, vc_obj(vc), Gfx_Console_Log_VNC,
                             "Ignoring unknown VNC input key 0x%x",
                             keysym);
        }

        SIM_RELEASE_CELL(vc_obj(vc), &lock);
}

// Pass mouse input to the console.
static void
handle_mouse_event(vnc_console_t *vc, vnc_pointer_event_t *event)
{
        int x = CONVERT_BE16(event->x);
        int y = CONVERT_BE16(event->y);

        domain_lock_t *lock;
        SIM_ACQUIRE_CELL(vc_obj(vc), &lock);

        SIM_LOG_INFO(4, vc_obj(vc), Gfx_Console_Log_VNC,
                     "got pointer event %d %d", x, y);

        // Simics uses a different order of middle and right button
        uint8 btns = event->mask;
        vc->iface->mouse_event(
                vc_obj(vc), x, y, 0,
                (btns & ~6) | (btns & 4) >> 1 | (btns & 2) << 1);

        SIM_RELEASE_CELL(vc_obj(vc), &lock);
}

// Store client continuous update area.
static bool
handle_enable_continuous_event(vnc_client_t *cl, vnc_fb_update_event_t *event)
{        
        gfx_rect_t rect = create_rect(CONVERT_BE16(event->rect.x_pos),
                                      CONVERT_BE16(event->rect.y_pos),
                                      CONVERT_BE16(event->rect.x_pos)
                                      + CONVERT_BE16(event->rect.width) - 1,
                                      CONVERT_BE16(event->rect.y_pos)
                                      + CONVERT_BE16(event->rect.height) - 1);

        CALLBACK_LOG(4, vc_obj(cl->vc),
                     "Got EnableContinuousUpdates request (%d, %d)"
                     " size (%d, %d) enable=%d",
                     rect.x_pos, rect.y_pos, rect.width, rect.height,
                     event->incremental);
        gfx_rect_t dirty = intersect_rects(
                rect, create_rect(0, 0, cl->width - 1, cl->height - 1));
        
        pthread_mutex_lock(&cl->client_mutex);
        cl->continuous_updates = event->incremental;
        if (cl->continuous_updates) {
                update_client_dirty(cl, dirty);
                cl->continuous_request = dirty;
        } else
                cl->continuous_request = (gfx_rect_t){ 0 };
        pthread_mutex_unlock(&cl->client_mutex);
        return event->incremental;
}

static int
num_clients(vnc_console_t *vc)
{
        int nclients = 0;
        vnc_client_t *cl = vc->clients;
        while (cl) {
                nclients++;
                cl = cl->next;
        }
        return nclients;
}

static void
do_wait_for_write_thread_exit(lang_void *arg)
{
        vnc_client_t *cl = arg;
        ASSERT(cl);

        // Wait for output thread to finish
        pthread_mutex_lock(&cl->output.lock);
        cl->output.exiting = true;
        while (cl->output.has_worker)
                pthread_cond_wait(&cl->output.exit_cond,
                                  &cl->output.lock);
        pthread_mutex_unlock(&cl->output.lock);
}

static void
wait_for_write_thread_exit(vnc_client_t *cl)
{
        // Thread domains cannot be held while we wait, since this could
        // cause a deadlock if the thread we wait upon tries to log something.
        VT_run_outside_cell(do_wait_for_write_thread_exit, cl);
}

static void
close_client_cb(void *arg)
{
        vnc_client_t *cl = arg;
        vnc_console_t *vc = cl->vc;

        /* It is possible the VNC console has been released already,
           in which case all clients have been unlinked, but not released */
        if (vc) {
                // Remove client from list
                vnc_client_t **pp = &vc->clients;
                while (*pp != cl)
                        pp = &(**pp).next;
                *pp = cl->next;

                // Possibly turn off screen update events.
                if (num_clients(vc) == 0)
                        gfx_set_vnc_connected(vc->gc->input_data, false);
        }

        // Wait for output threads to finish
        wait_for_write_thread_exit(cl);

        VFREE(cl->dirty);
        VFREE(cl->output.bufs[0]);
        VFREE(cl->output.bufs[1]);

        pthread_mutex_destroy(&cl->client_mutex);
        pthread_cond_destroy(&cl->output.exit_cond);
        pthread_mutex_destroy(&cl->output.lock);
        deflateEnd(&cl->tight_strm);
        deflateEnd(&cl->zrle_strm);
        VFREE(cl->encode_buf);
        VFREE(cl->tight_input_buf);
        genrand_destroy(cl->rand_state);
        MM_FREE(cl);
}

/* Close the connection used by a client and remove it from the list.
   The client structure still needs to be destroyed afterwards. */
static void
close_client(vnc_client_t *cl)
{
        pthread_mutex_lock(&cl->output.lock);
        cl->output.exiting = true;
        bool connected = cl->connected;
        cl->connected = false;
        pthread_mutex_unlock(&cl->output.lock);

        if (connected) {
                CALL(cl->server, close)(cl);
                SIM_thread_safe_callback(close_client_cb, cl);
        }
}

static bool
client_dead(vnc_client_t *cl)
{
        pthread_mutex_lock(&cl->output.lock);
        bool connected = cl->connected;
        pthread_mutex_unlock(&cl->output.lock);
        return !connected;
}

static void
socket_write(vnc_client_t *cl, const void *data, size_t size)
{
        pthread_mutex_lock(&cl->output.lock);
        bool connected = cl->connected;
        bool exiting = cl->output.exiting;
        pthread_mutex_unlock(&cl->output.lock);
        if (connected) {
                int ret = CALL(cl->server, write)(
                        cl, (bytes_t){.data = data, .len = size});
                if (ret == -1) {
                        if (!exiting)
                                close_client(cl);
                }
        }
}

static void
write_thread_entry(void *arg)
{
        vnc_client_t *cl = arg;
        for (;;) {
                if (cl->need_resize && cl->can_resize) {
                        vnc_console_t *vc = cl->vc;
                        cl->need_resize = false;
                        vnc_client_update_size(
                                vc, cl, vc->screen_width, vc->screen_height);
                }
                if (!cl->need_resize)
                        send_screen_updates(cl);

                pthread_mutex_lock(&cl->output.lock);
                // Take current output buffer
                output_data_t *buf = &cl->output.bufs[cl->output.buf_num];
                if (VEMPTY(*buf)) {
                        cl->output.has_worker = false;
                        if (cl->output.exiting)
                                pthread_cond_signal(&cl->output.exit_cond);
                        pthread_mutex_unlock(&cl->output.lock);
                        break;
                }

                // Flip buffers for other thread
                cl->output.buf_num = !cl->output.buf_num;
                pthread_mutex_unlock(&cl->output.lock);

                socket_write(cl, VVEC(*buf), VLEN(*buf));
                VCLEAR(*buf);
        }
}

static void
start_writer_thread(vnc_client_t *cl)
{
        pthread_mutex_lock(&cl->output.lock);
        bool start_thread = !cl->output.has_worker && !cl->output.exiting;
        if (start_thread)
                cl->output.has_worker = true;
        pthread_mutex_unlock(&cl->output.lock);

        if (start_thread)
                SIM_run_in_thread(write_thread_entry, cl);
}

// Pause writer threading. The thread will not be restarted until
// resume_writer_thread has been called.
static void
pause_writer_thread(vnc_client_t *cl)
{
        wait_for_write_thread_exit(cl);
}

// Resume a paused writer thread.
static void
resume_writer_thread(vnc_client_t *cl)
{
        pthread_mutex_lock(&cl->output.lock);
        bool resume = cl->connected;
        cl->output.exiting = !resume;
        pthread_mutex_unlock(&cl->output.lock);

        if (resume)
                start_writer_thread(cl);
}

// Send specified data to the client socket. Return false on socket error.
static void
vnc_client_put_data(vnc_client_t *cl, const void *data, int len)
{
        pthread_mutex_lock(&cl->output.lock);
        output_data_t *obuf = &cl->output.bufs[cl->output.buf_num];
        size_t size = VLEN(*obuf);
        VGROW(*obuf, len);
        memcpy(VVEC(*obuf) + size, data, len);
        pthread_mutex_unlock(&cl->output.lock);

        start_writer_thread(cl);
}

// Receive at most len bytes over the client socket. Return number of bytes
// read or 0 on error
static ssize_t
vnc_client_get_data(vnc_client_t *cl, void *buf, size_t len)
{
        ssize_t num_bytes = -2;
        while (num_bytes == -2) {
                num_bytes = CALL(cl->server, read)(
                        cl, (buffer_t){.data = buf, .len = len});
        }
        if (num_bytes == -1)
                return 0;
        else
                return num_bytes;
}

// Receive at most len bytes over the client socket. Return number of bytes
// read, 0 on error, or -2 on "no data at the moment".
static ssize_t
vnc_client_get_data_non_block(vnc_client_t *cl, void *buf, size_t len)
{
        ssize_t num_bytes = CALL(cl->server, read)(
                cl, (buffer_t){.data = buf, .len = len});
        if (num_bytes == -1)
                return 0;
        else
                return num_bytes;
}

// Send a resize message to the specified client so that it changes its
// screen resolution (if the client supports it)
static void
vnc_client_update_size(vnc_console_t *vc, vnc_client_t *cl,
                       int width, int height)
{
        if (!cl->can_resize) {
                cl->need_resize = true;
                SIM_LOG_INFO(2, vc_obj(vc), Gfx_Console_Log_VNC,
                             "Client cannot be resized.");
                return;
        }

        vnc_cmd_header_t *hdr;
        vnc_rect_header_t *rect_hdr;
        uint8 data[sizeof *hdr + sizeof *rect_hdr];
        hdr = (vnc_cmd_header_t *)data;
        rect_hdr = (vnc_rect_header_t *)(data + sizeof *hdr);
        *hdr = (vnc_cmd_header_t){
                .type = VNC_Server_Cmd_Update,
                .num_rects = CONVERT_BE16(1),
        };

        *rect_hdr = (vnc_rect_header_t){
                .rect = (vnc_rect_t){
                        .x_pos = CONVERT_BE16(0),
                        .y_pos = CONVERT_BE16(0),
                        .width = CONVERT_BE16(width),
                        .height = CONVERT_BE16(height)},
                // DesktopResize pseudo encoding.
                .encoding = CONVERT_BE32(VNC_Encoding_Resize),
        };

        vnc_client_put_data(cl, data, sizeof data);
        pthread_mutex_lock(&cl->client_mutex);

        // Store new client screen size.
        cl->width = width;
        cl->height = height;
        cl->resize = true;

        // Avoid sending to large screen rectangles on next update
        gfx_rect_t screen = create_rect(0, 0, width - 1, height - 1);
        VFOREACH_T(cl->dirty, gfx_rect_t, r) {
                *r = intersect_rects(*r, screen);
        }

        update_client_dirty(cl, screen);

        // Clamp update request rectangles
        // Also done indirectly when sending, from intersection with dirty
        cl->update_request = intersect_rects(cl->update_request, screen);
        cl->continuous_request = intersect_rects(cl->continuous_request,
                                                 screen);

        pthread_mutex_unlock(&cl->client_mutex);
}

// Update all VNC clients to the specified screen size.
void
vnc_console_set_screen(vnc_console_t *vc, const uint32 *screen,
                       int width, int height)
{
        SIM_LOG_INFO(2, vc_obj(vc), Gfx_Console_Log_VNC,
                     "Resize %dx%d", width, height);

        vc->screen = screen;
        vc->screen_width = width;
        vc->screen_height = height;

        for (vnc_client_t *cl = vc->clients; cl; cl = cl->next) {
                pause_writer_thread(cl);
                cl->screen = screen;
                vnc_client_update_size(vc, cl, width, height);
                resume_writer_thread(cl);
        }
}

static uint32
rgb888_to_rgb332(uint32 rgb)
{
        return ((rgb >> 16) & 0xe0)
                | ((rgb >> 11) & 0x1c)
                | ((rgb >> 6) & 0x3);
}

static void
vnc_raw_encode_8bpp(uint8 *dst, const uint32 *src, int width, int height,
                    int src_stride, int dst_stride)
{
        for (int y = 0; y < height; y++) {
                // Convert from 32-bit to 8-bit.
                // This is also colour map index if not using truecolor.
                for (int x = 0; x < width; x++) {
                        uint32 color = rgb888_to_rgb332(src[x]);
                        dst[x] = (uint8)color;
                }
                dst += dst_stride;
                src += src_stride;
        }
}

static bool
matching_pixelformat(vnc_pixelformat_t left, vnc_pixelformat_t right)
{
        return (left.endian == right.endian
                && left.red_max == right.red_max
                && left.green_max == right.green_max
                && left.blue_max == right.blue_max
                && left.red_shift == right.red_shift
                && left.green_shift == right.green_shift
                && left.blue_shift == right.blue_shift);
}

static void
vnc_raw_encode_32bpp(uint8 *dst, const uint32 *src, int width, int height,
                     int src_stride, int dst_stride,
                     vnc_pixelformat_t client_format,
                     vnc_pixelformat_t server_format)
{
        ASSERT(client_format.truecolor);
        
        if (matching_pixelformat(client_format, server_format)) {
                for (int y = 0; y < height; y++) {
                        memcpy(dst, src, dst_stride);
                        dst += dst_stride;
                        src += src_stride;
                }
        } else {
                for (int y = 0; y < height; y++) {                        
                        uint32 *col = (uint32 *)dst;
                        for (int x = 0; x < width; x++) {
                                uint32 src_col = src[x];
                                uint8 r = (src_col >> 16)
                                          & client_format.red_max;
                                uint8 g = (src_col >> 8)
                                          & client_format.green_max;
                                uint8 b = src_col
                                          & client_format.blue_max;
                                uint32 dst_col =
                                          r << client_format.red_shift
                                        | g << client_format.green_shift
                                        | b << client_format.blue_shift;
                                col[x] = client_format.endian
                                         ? CONVERT_BE32(dst_col) : dst_col;
                        }
                        dst += dst_stride;
                        src += src_stride;
                }
        }
}

// Encode the specified rectangle of src in the format required by the
// client and append the output to buf.
static void
vnc_raw_encode_rect(vnc_client_t *cl, uint8 *dst, const uint32 *src,
                    int width, int height, int src_stride, int dst_stride,
                    vnc_pixelformat_t fmt)
{
        vnc_console_t *vc = cl->vc;
        if (cl->format.bpp == 8) {
                vnc_raw_encode_8bpp(dst, src, width, height,
                                    src_stride, dst_stride);
        } else if (cl->format.bpp == 32) {
                vnc_raw_encode_32bpp(dst, src, width, height,
                                     src_stride, dst_stride,
                                     fmt, vc->format);
        } else {
                SIM_LOG_UNIMPLEMENTED(
                        1, vc_obj(vc), Gfx_Console_Log_VNC,
                        "Unhandled bpp case %d, true-color %d",
                        fmt.bpp, fmt.truecolor);
        }
}

// Encode screen data in src to dst, using "compressed pixel" format used in
// e.g. ZRLE compression.
static void
vnc_raw_encode_compressed(uint8 *dst, const uint32 *src, int width, int height,
                          int src_stride, int dst_stride,
                          vnc_pixelformat_t client_format)
{
        for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                        uint32 src_col = src[x];
                        uint8 r = (src_col >> 16) & client_format.red_max;
                        uint8 g = (src_col >> 8) & client_format.green_max;
                        uint8 b = src_col & client_format.blue_max;
                        uint32 dst_col =
                                r << client_format.red_shift
                                | g << client_format.green_shift
                                | b << client_format.blue_shift;
                        memcpy(dst + x * 3, &dst_col, 3);
                }
                dst += dst_stride;
                src += src_stride;
        }
}


static void
encode_buf_clear(vnc_client_t *cl)
{
        cl->encode_pos = 0;
}

static unsigned
encode_buf_size(vnc_client_t *cl)
{
        return cl->encode_pos;
}

// Add (data, size) to 'encode_buf', updating 'encode_pos'
static unsigned
encode_buf_grow(vnc_client_t *cl, int size)
{
        unsigned n = VLEN(cl->encode_buf) - cl->encode_pos;
        if (n < size)
                VGROW(cl->encode_buf, size - n);
        unsigned pos = cl->encode_pos;
        cl->encode_pos += size;
        return pos;
}

// Set 'encode_buf' contents at offs to contents provided by (data, size)
static void
encode_buf_set(vnc_client_t *cl, unsigned offs, const void *data, int size)
{
        ASSERT(offs + size <= cl->encode_pos);
        ASSERT(cl->encode_pos <= VLEN(cl->encode_buf));
        memcpy(VVEC(cl->encode_buf) + offs, data, size);
}

// Append contents to 'encode_buf'
static void
encode_buf_append(vnc_client_t *cl, const void *data, int size)
{
        encode_buf_set(cl, encode_buf_grow(cl, size), data, size);
}

// Deflate (data, size) into 'encode_buf', updating 'encode_pos'
static void
encode_buf_deflate(vnc_client_t *cl, z_stream *strm,
                   const uint8 *data, int size, int flush)
{
        strm->next_in = (uint8 *)data;
        strm->avail_in = size;
        for (;;) {
                unsigned n = VLEN(cl->encode_buf) - cl->encode_pos;
                strm->next_out = VVEC(cl->encode_buf) + cl->encode_pos;
                strm->avail_out = n;

                // make sure there is some room available
                if (n > 64) {
                        int status = deflate(strm, flush);
                        ASSERT(status == Z_OK);

                        cl->encode_pos += n - strm->avail_out;

                        if (strm->avail_out != 0)
                                break;
                }
                unsigned new_size = MAX(0x10000, VLEN(cl->encode_buf) * 2);
                VRESIZE(cl->encode_buf, new_size);
        }
}

// Encode ZRLE tile (x, y) in src using zlib.
static void
vnc_zrle_encode_tile(vnc_client_t *cl, const uint32 *src,
                     int x, int y, int width, int height,
                     int src_stride, vnc_pixelformat_t fmt)
{
        bool use_cpixel = fmt.truecolor && fmt.bpp == 32 && fmt.depth < 32;
        int depth = use_cpixel ? fmt.depth >> 3 : fmt.bpp >> 3;
        int tile_data_size = width * height * depth + 1;
        uint8 tile[tile_data_size];
        const uint32 *tile_src = src + y * 64 * src_stride + x * 64;
        // Raw pixel data in tile
        tile[0] = 0;
        if (use_cpixel)
                vnc_raw_encode_compressed(tile + 1, tile_src, width, height,
                                          src_stride, width * depth,
                                          fmt);
        else
                vnc_raw_encode_rect(cl, tile + 1, tile_src, width, height,
                                    src_stride, width * depth, fmt);

        encode_buf_deflate(cl, &cl->zrle_strm,
                           tile, tile_data_size, Z_NO_FLUSH);
}

// Encode the specified rectangle of src using ZRLE compression, i.e. zlib
// compressed 64x64 tiles.
static void
vnc_zrle_encode_tiles(vnc_client_t *cl, const uint32 *src,
                      int width, int height, int src_stride,
                      vnc_pixelformat_t fmt)
{        
        int tiles_horizontal = width / 64;
        int tiles_vertical = height / 64;
        int tiles_horizontal_slack = width % 64;
        int tiles_vertical_slack = height % 64;
        int tile_y;
        int tile_x;
        SIM_LOG_INFO(4, vc_obj(cl->vc), Gfx_Console_Log_VNC,
                     "ZRLE encode rectangle, %dx%d tiles, %dx%d slack",
                     tiles_horizontal, tiles_vertical,
                     tiles_horizontal_slack, tiles_vertical_slack);
        
        for (tile_y = 0; tile_y < tiles_vertical; tile_y++) {
                for (tile_x = 0; tile_x < tiles_horizontal; tile_x++) {
                        vnc_zrle_encode_tile(cl, src, tile_x, tile_y,
                                             64, 64, src_stride, fmt);
                }
                if (tiles_horizontal_slack > 0)
                        vnc_zrle_encode_tile(cl, src, tile_x, tile_y,
                                             tiles_horizontal_slack,
                                             64, src_stride, fmt);
        }
        if (tiles_vertical_slack > 0) {
                for (tile_x = 0; tile_x < tiles_horizontal; tile_x++) {
                        vnc_zrle_encode_tile(cl, src, tile_x, tile_y,
                                             64, tiles_vertical_slack,
                                             src_stride, fmt);
                }
                if (tiles_horizontal_slack > 0)
                        vnc_zrle_encode_tile(cl, src, tile_x, tile_y,
                                             tiles_horizontal_slack,
                                             tiles_vertical_slack,
                                             src_stride, fmt);
        }
}

static void
vnc_zlib_sync(vnc_client_t *cl, z_stream *strm)
{
        encode_buf_deflate(cl, strm, NULL, 0, Z_SYNC_FLUSH);
}

// Encode and send the specified rectangle of src using ZRLE compression
static void
vnc_zrle_encode_rect(vnc_client_t *cl, const uint32 *src,
                     int x, int y, int width, int height, int src_stride,
                     vnc_pixelformat_t fmt)
{
        // 4 bytes for header for size
        uint32 header;
        size_t header_ofs = encode_buf_grow(cl, sizeof header);
        unsigned data_start = encode_buf_size(cl);

        // Divide into tiles and compress each tile
        vnc_zrle_encode_tiles(cl, src + src_stride * y + x,
                              width, height, src_stride, fmt);

        // ZRLE compression mandates that zlib should be flushed to byte
        // boundary between each rectangle
        vnc_zlib_sync(cl, &cl->zrle_strm);

        uint32 output_size = encode_buf_size(cl) - data_start;
        header = CONVERT_BE32(output_size);
        encode_buf_set(cl, header_ofs, &header, sizeof header);

        socket_write(cl, VVEC(cl->encode_buf), encode_buf_size(cl));
        encode_buf_clear(cl);
}


// Split too big rectangles, since the protocol has limits that must be
// satisfied (width < 2048 && compressed_size < 4 MB).
static void
tight_split_rects(vnc_client_t *cl, gfx_rect_list_t *rects)
{
        const int max_w = 1024;
        const int max_h = 512;

        for (int i = VLEN(*rects) - 1; i >= 0; i--) {
                gfx_rect_t r = VGET(*rects, i);
                if (r.width < 64 && r.height < 64)
                        continue;
                VREMOVE(*rects, i);
                for (int dx = 0; dx < r.width; dx += max_w) {
                        for (int dy = 0; dy < r.height; dy += max_h) {
                                gfx_rect_t rr = {
                                        .x_pos = r.x_pos + dx,
                                        .y_pos = r.y_pos + dy,
                                        .width = MIN(r.width - dx, max_w),
                                        .height = MIN(r.height - dy, max_h),
                                };
                                VADD(*rects, rr);
                        }
                }
        }
}

// Encode and send the specified rectangle of src using Tight compression
static void
vnc_tight_encode_rect(vnc_client_t *cl, const uint32 *src,
                      int x, int y, int width, int height, int src_stride,
                      vnc_pixelformat_t fmt)
{
        bool use_cpixel = fmt.truecolor && fmt.bpp == 32 && fmt.depth < 32;
        int depth = use_cpixel ? fmt.depth >> 3 : fmt.bpp >> 3;
        int input_size = width * height * depth;
        src += src_stride * y + x;

        // Reserve space for input
        VRESIZE(cl->tight_input_buf, input_size);
        uint8 *input = VVEC(cl->tight_input_buf);

        // Encode input, to be compressed
        if (use_cpixel) {
                vnc_raw_encode_compressed(input, src, width, height,
                                          src_stride, width * depth,
                                          fmt);
        } else {
                vnc_raw_encode_rect(cl, input, src, width, height,
                                    src_stride, width * depth, fmt);
        }
        if (input_size < 12) {
                // Copy contents
                unsigned offs = encode_buf_grow(cl, input_size + 1);
                uint8 header = 0;
                encode_buf_set(cl, offs, &header, 1);
                encode_buf_set(cl, offs + 1, input, input_size);

                socket_write(cl, VVEC(cl->encode_buf), encode_buf_size(cl));
        } else {
                // Reserve 4-byte header (might be reduced later)
                unsigned header_offs = encode_buf_grow(cl, 4);

                unsigned start_offs = encode_buf_size(cl);

                // Compress contents
                encode_buf_deflate(cl, &cl->tight_strm,
                                   input, input_size, Z_NO_FLUSH);
        
                // Flush to byte boundary between each rectangle
                vnc_zlib_sync(cl, &cl->tight_strm);

                unsigned output_size = encode_buf_size(cl) - start_offs;
                SIM_LOG_INFO(4, vc_obj(cl->vc), Gfx_Console_Log_VNC,
                             "Compressed rectangle size %u, ratio %f",
                             output_size, (float)input_size / output_size);

                uint8 *header = VVEC(cl->encode_buf) + header_offs;
                header[0] = 0;
                unsigned skip;
                if (output_size <= 0x7f) {
                        header[1] = output_size;
                        skip = 2;
                } else if (output_size <= 0x3fff) {
                        header[1] = 0x80 | (output_size & 0x7f);
                        header[2] = (output_size >> 7);
                        skip = 1;
                } else if (output_size < (1 << 22)) {
                        header[1] = 0x80 | (output_size & 0x7f);
                        header[2] = 0x80 | ((output_size >> 7) & 0x7f);
                        header[3] = output_size >> 14;   /* 8 bits */
                        skip = 0;
                } else {
                        /* too big size; encoding not possible */
                        ASSERT(0);
                }
                // Remove holes from the header
                uint8 *p = VVEC(cl->encode_buf);
                if (skip != 0)
                        memmove(p + skip, p, start_offs - skip);
                socket_write(cl, p + skip, encode_buf_size(cl) - skip);
        }
        encode_buf_clear(cl);
}

// Encode and send the specified rectangle of src using Raw encoding
static void
vnc_raw_rect_encoder(vnc_client_t *cl, const uint32 *src,
                     int x, int y, int width, int height, int src_stride,
                     vnc_pixelformat_t fmt)
{
        int dst_stride = width * fmt.bpp >> 3;
        size_t data_len = height * dst_stride;

        size_t ofs = encode_buf_grow(cl, data_len);
        vnc_raw_encode_rect(cl, VVEC(cl->encode_buf) + ofs,
                            // Move to the start of the rectangle.
                            src + src_stride * y + x,
                            width, height,
                            src_stride, dst_stride, fmt);

        socket_write(cl, VVEC(cl->encode_buf), encode_buf_size(cl));
        encode_buf_clear(cl);
}

// Send a frame buffer update message to the client, updating the specified
// screen rectangles from src, which must point to the start of the console
// scren data.
static void
vnc_client_update_blocks(vnc_client_t *cl, gfx_rect_list_t *rects,
                        const uint32 *src)
{
        pthread_mutex_lock(&cl->client_mutex);
        vnc_pixelformat_t fmt = cl->format;
        vnc_client_encoding_t encoding = cl->encoding;
        encoder_t encoder = cl->encoder;
        pthread_mutex_unlock(&cl->client_mutex);

        SIM_LOG_INFO(4, vc_obj(cl->vc), Gfx_Console_Log_VNC,
                     "Send %d rectangles", VLEN(*rects));

        // Encoder might need to split some rectangles into smaller ones
        if (encoder.split_rects)
                encoder.split_rects(cl, rects);

        // Encode buffer should always be empty at this point
        ASSERT(encode_buf_size(cl) == 0);

        vnc_cmd_header_t hdr = (vnc_cmd_header_t){
                .type = VNC_Server_Cmd_Update,
                .num_rects = CONVERT_BE16(VLEN(*rects)),
        };
        encode_buf_append(cl, &hdr, sizeof hdr);

        VFORT(*rects, gfx_rect_t, rect) {
                SIM_LOG_INFO(4, vc_obj(cl->vc), Gfx_Console_Log_VNC,
                             "update %d,%d (%d x %d)", rect.x_pos, rect.y_pos,
                             rect.width, rect.height);

                // Header for each rectangle
                vnc_rect_header_t r = {
                        .rect = (vnc_rect_t){
                                .x_pos = CONVERT_BE16(rect.x_pos),
                                .y_pos = CONVERT_BE16(rect.y_pos),
                                .width = CONVERT_BE16(rect.width),
                                .height = CONVERT_BE16(rect.height)},
                        .encoding = CONVERT_BE32(encoding),
                };
                encode_buf_append(cl, &r, sizeof r);

                // Encode rectangle data
                encoder.encode_rect(cl, src, rect.x_pos, rect.y_pos,
                                    rect.width, rect.height, cl->width, fmt);
        }
}

static void
construct_update_rects(vnc_client_t *cl, gfx_rect_list_t *updates)
{
        // Only update parts that client has requested.
        // Skip loop completely if empty rectangle.
        if (!empty_rect(cl->update_request)
            || !empty_rect(cl->continuous_request)) {
                for (int i = VLEN(cl->dirty) - 1; i >= 0; i--) {
                        gfx_rect_t rect = intersect_rects(
                                cl->update_request, VGET(cl->dirty, i));
                        // Only remove dirty server rectangles when "fetched"
                        if (!empty_rect(rect)) {
                                VADD(*updates, rect);
                                VREMOVE(cl->dirty, i);
                        } else {
                                rect = intersect_rects(
                                        cl->continuous_request,
                                        VGET(cl->dirty, i));
                                if (!empty_rect(rect)) {
                                        VADD(*updates, rect);
                                        VREMOVE(cl->dirty, i);
                                }
                        }
                }
        }
}

static void
send_screen_updates(vnc_client_t *cl)
{
        gfx_rect_list_t updates = VNULL;
        pthread_mutex_lock(&cl->client_mutex);

        // Construct list of rectangles to send
        construct_update_rects(cl, &updates);

        if (!VEMPTY(updates)) {
                // Do not send data again until client requests it
                cl->update_request = (gfx_rect_t){ 0 };
        }
        pthread_mutex_unlock(&cl->client_mutex);

        if (!VEMPTY(updates))
                vnc_client_update_blocks(cl, &updates, cl->screen);
        VFREE(updates);
}

// Notify the VNC server that the rectangle 'dirty' has changed and
// needs to be sent to clients.
void 
vnc_screen_rect_dirty(vnc_console_t *vc, gfx_rect_t dirty)
{
        for (vnc_client_t *cl = vc->clients; cl; cl = cl->next) {
                pthread_mutex_lock(&cl->client_mutex);
                update_client_dirty(cl, dirty);
                pthread_mutex_unlock(&cl->client_mutex);

                start_writer_thread(cl);
        }
}

static void 
vnc_client_send_eocu(vnc_client_t *cl)
{
        uint8 message = VNC_Server_Cmd_EndContinuousUpdates;
        vnc_client_put_data(cl, &message, sizeof message);
}

// Send data to the given client, depending on the client state.
// Used during client connection setup, not after it starts running.
static void
vnc_client_write(void *arg)
{
	vnc_client_t *cl = arg;
        if (client_dead(cl))
                return;

        vnc_console_t *vc = cl->vc;
        
	switch (cl->state) {
	case VNC_Client_State_Version: {
                // Send expected version to client.
                const char verstr[VNC_VERSION_STR_LEN] = "RFB 003.008\n";
		vnc_client_put_data(cl, verstr, VNC_VERSION_STR_LEN);
		break;
	}
	case VNC_Client_State_Select_Auth: {
                // Authentication structure depends on version.
                CALLBACK_LOG(2, vc_obj(cl->vc), "Send auth info to client");
                if (vnc_version_eq(cl->version, 3, 3)) {
			uint8 buf[4] = { 0, 0, 0, vc->password
                                         ? 0 : VNC_Auth_None };
			vnc_client_put_data(cl, buf, sizeof buf);
                        if (vc->password) {
                                const char msg[] = "Password authentication"
                                        " not supported for protocol"
                                        " version 3.3";
                                uint32 len = sizeof msg;
                                vnc_client_put_data(cl, &len, sizeof len);
                                vnc_client_put_data(cl, msg, len);
                                close_client(cl);
                        }
		} else {
                        // 3.7 and 3.8
                        uint8 buf[2] = { 1,
                                vc->password
                                ? VNC_Auth_Password
                                : VNC_Auth_None };
                        vnc_client_put_data(cl, buf, sizeof buf);
		}
		break;
	}
	case VNC_Client_State_Auth:
                // Generate and send challenge
                CALLBACK_LOG(2, vc_obj(cl->vc), "Send auth challenge");
                for (int i = 0; i < sizeof cl->challenge; i += 8)
                        *(uint64 *)&cl->challenge[i] =
                                genrand_uint64(cl->rand_state);
                vnc_client_put_data(cl, cl->challenge, sizeof cl->challenge);
                break;
	case VNC_Client_State_Auth_Reply: {
                CALLBACK_LOG(2, vc_obj(cl->vc), "Send auth reply");
                if (cl->auth_type == VNC_Auth_None || cl->authenticated) {
                        uint8 buf[4] = { 0, 0, 0, 0 }; // CAR32   0 == OK
                        vnc_client_put_data(cl, buf, sizeof buf);
                } else {
                        const char msg[] = "Incorrect password";
                        uint32 len = sizeof msg;
                        vnc_client_put_data(cl, &len, sizeof len);
                        vnc_client_put_data(cl, msg, len);
                        close_client(cl);
                }
		break;
	}
	case VNC_Client_State_Init: {
                // Send server information to client.
                const char *s = SIM_object_name(vc_obj(vc));
		vnc_cmd_server_init_t init = {
                        // Initial screen size.
                        .width = CONVERT_BE16(cl->width),
                        .height = CONVERT_BE16(cl->height),

                        // Server pixel format.
                        .format.bpp = vc->format.bpp,
                        .format.depth = vc->format.depth,
                        .format.endian = vc->format.endian,
                        .format.truecolor = vc->format.truecolor,
                        .format.red_max = CONVERT_BE16(vc->format.red_max),
                        .format.green_max = CONVERT_BE16(vc->format.green_max),
                        .format.blue_max = CONVERT_BE16(vc->format.blue_max),
                        .format.red_shift = vc->format.red_shift,
                        .format.green_shift = vc->format.green_shift,
                        .format.blue_shift = vc->format.blue_shift,

                        // We do not set a server name.
                        .name_length = CONVERT_BE32(strlen(s)),
                };
		vnc_client_put_data(cl, &init, sizeof init);
		vnc_client_put_data(cl, s, strlen(s));
                // Client connection setup is now completed.
		cl->state = VNC_Client_State_Running;

                // Make sure we send complete screen immediately.
                pthread_mutex_lock(&cl->client_mutex);
                update_client_dirty(cl, create_rect(0, 0, cl->width - 1,
                                                    cl->height - 1));
                pthread_mutex_unlock(&cl->client_mutex);
                break;
	}
	case VNC_Client_State_Running: 
		break;
	}
}

// Read version from client socket and return true if the server supports
// the version. Return false on error or if the version is unsupported.
static bool
vnc_client_set_version(vnc_client_t *cl)
{
        char version[VNC_VERSION_STR_LEN + 1];
        ssize_t len = VNC_VERSION_STR_LEN;
        if (vnc_client_get_data(cl, version, len) < VNC_VERSION_STR_LEN)
                return false;

        version[VNC_VERSION_STR_LEN] = '\0';
                
        CALLBACK_LOG(3, vc_obj(cl->vc),
                     "Got client version: %s", version);
        int major, minor;
        if (sscanf(version, "RFB %d.%d\n", &major, &minor) != 2)
                return false;
        cl->version = (vnc_version_t){major, minor};
        return true;
}

static void
create_colour_map(colour_map_t *colour_map)
{
        // Use rgb332 value as colour map index.
        for (int r = 0; r <= 7; r++)
                for (int g = 0; g <= 7; g++)
                        for (int b = 0; b <= 3; b++) {
                                int c = b | (g << 2) | (r << 5);
                                // Scale up rgb332 to 16-bit values.
                                colour_map[c].r = CONVERT_BE16(r * 0xffff / 7);
                                colour_map[c].g = CONVERT_BE16(g * 0xffff / 7);
                                colour_map[c].b = CONVERT_BE16(b * 0xffff / 3);
                        }
}

// Send server-to-client SetColourMapEntries message.
static bool
vnc_client_set_colour_map(vnc_client_t *cl)
{
	ASSERT(cl->format.bpp == 8 && !cl->format.truecolor);
        // We only use colour map (palette) with 8-bit depth.
        int colour_map_size = 256;

        vnc_col_map_header_t *map;
        colour_map_t *colour_map;
        uint8 out[sizeof *map + sizeof *colour_map * colour_map_size];
        map = (vnc_col_map_header_t *)out;
        colour_map = (colour_map_t *)(out + sizeof *map);

        *map = (vnc_col_map_header_t){
                .type = VNC_Server_Cmd_SetColourMapEntries,
                .first_col = CONVERT_BE16(0),
                .ncols = CONVERT_BE16((uint16)colour_map_size),
        };
        create_colour_map(colour_map);
        
        CALLBACK_LOG(2, vc_obj(cl->vc), "Send colour map");
        // Send all colour map entries
        vnc_client_put_data(cl, out, sizeof out);
        cl->send_colour_map = false;
        return true;
}

// Handle client-to-server SetPixelFormat message. Return false on error.
static bool
vnc_client_set_pixel_format(vnc_client_t *cl)
{
        vnc_pixelformat_t format;
        ssize_t len;
        STATIC_ASSERT(sizeof format >= 3);
        
        // Read padding.
        len = 3;
        if (vnc_client_get_data(cl, &format, len) < len)
                return false;

        // Now read pixel format.
        len = sizeof format;
        if (vnc_client_get_data(cl, &format, len) < len)
                return false;

        pthread_mutex_lock(&cl->client_mutex);
        cl->format.bpp = format.bpp;
        cl->format.depth = format.depth;
        cl->format.endian = format.endian;
        cl->format.truecolor = format.truecolor;
        cl->format.red_max = CONVERT_BE16(format.red_max);
        cl->format.green_max = CONVERT_BE16(format.green_max);
        cl->format.blue_max = CONVERT_BE16(format.blue_max);
        cl->format.red_shift = format.red_shift;
        cl->format.green_shift = format.green_shift;
        cl->format.blue_shift = format.blue_shift;
        
        // Make sure we send complete screen after pixel format change.
        update_client_dirty(cl, create_rect(0, 0, cl->width - 1,
                                           cl->height - 1));
        pthread_mutex_unlock(&cl->client_mutex);

        CALLBACK_LOG(2, vc_obj(cl->vc),
                     "set format: %d bpp (%d) %d",
                     cl->format.bpp, cl->format.truecolor, cl->format.depth);

        if (!cl->format.truecolor) {
                if (cl->format.bpp != 8) {
                        CALLBACK_LOG(
                                1, vc_obj(cl->vc), 
                                "Unhandled bpp case %d, true-color %d",
                                cl->format.bpp, cl->format.truecolor);
                        return false;
                }
                cl->send_colour_map = true;
        }
                
        return true;
}

static void *
zlib_alloc(void *opaque, unsigned items, unsigned size)
{
        return MM_ZALLOC(items * size, char);
}

static void
zlib_free(void *opaque, void *ptr)
{
        return MM_FREE(ptr);
}

static void
init_zlib_stream(z_stream *strm, int zlib_level)
{
        strm->zalloc = zlib_alloc;
        strm->zfree = zlib_free;
        int status = deflateInit(strm, zlib_level);
        ASSERT(status == Z_OK);
}

// Handle client-to-server SetEncodings message. Return false on error.
static bool
vnc_client_set_encodings(vnc_client_t *cl)
{
        vnc_num_encodings_t msg;
        ssize_t len = sizeof msg - 1;   // Leading message type already fetched.
        if (vnc_client_get_data(cl, (uint8 *)&msg + 1, len) < len)
                return false;
                
        unsigned num_encodings = CONVERT_BE16(msg.num_encodings);
        CALLBACK_LOG(2, vc_obj(cl->vc),
                     "Expect %d encodings", num_encodings);

        // Read all encoding types.
        uint32 encodings[num_encodings];
        if (vnc_client_get_data(cl, encodings, sizeof encodings) < len)
                return false;

        // Chosen rectangle encoding
        vnc_client_encoding_t encoding = 0;
        encoder_t encoder = {0};

        ASSERT(num_encodings < 0xffff);
        for (unsigned i = 0; i < num_encodings; i++) {
                uint32 val = CONVERT_BE32(encodings[i]);
                CALLBACK_LOG(2, vc_obj(cl->vc),
                             "Got encoding %#x", val);
                switch (val) {
                case VNC_Encoding_Resize:
                        cl->can_resize = true;
                        break;
                case VNC_Encoding_Continuous:
                        CALLBACK_LOG(2, vc_obj(cl->vc),
                                     "Client supports continuous updates");
                        vnc_client_send_eocu(cl);
                        break;
                case VNC_Encoding_Raw:
                        if (!encoder.encode_rect) {
                                encoder.encode_rect = vnc_raw_rect_encoder;
                                encoding = val;
                        }
                        break;
                case VNC_Encoding_ZRLE:
                        CALLBACK_LOG(2, vc_obj(cl->vc),
                                     "Client supports ZRLE encoding");
                        if (cl->vc->allow_compression && !encoder.encode_rect) {
                                encoder.encode_rect = vnc_zrle_encode_rect;
                                encoding = val;
                        }
                        break;
                case VNC_Encoding_Tight:
                        CALLBACK_LOG(2, vc_obj(cl->vc),
                                     "Client supports Tight encoding");
                        if (cl->vc->allow_compression && !encoder.encode_rect) {
                                encoder.encode_rect = vnc_tight_encode_rect;
                                encoder.split_rects = tight_split_rects;
                                encoding = val;
                        }
                        break;
                }                
        }
        if (!encoder.encode_rect) {
                encoding = VNC_Encoding_Raw;
                encoder.encode_rect = vnc_raw_rect_encoder;
        }
        const char *s = (encoding == VNC_Encoding_ZRLE) ? "ZRLE"
                : (encoding == VNC_Encoding_Tight) ? "Tight" : "Raw";
        CALLBACK_LOG(2, vc_obj(cl->vc), "VNC encoding '%s' selected", s);

        pthread_mutex_lock(&cl->client_mutex);
        cl->encoder = encoder;
        cl->encoding = encoding;
        pthread_mutex_unlock(&cl->client_mutex);
        return true;
}

// Store client frame buffer update request for later processing.
static void
handle_fb_update_event(vnc_client_t *cl, vnc_fb_update_event_t *event)
{
        gfx_rect_t rect = create_rect(CONVERT_BE16(event->rect.x_pos),
                                      CONVERT_BE16(event->rect.y_pos),
                                      CONVERT_BE16(event->rect.x_pos)
                                      + CONVERT_BE16(event->rect.width) - 1,
                                      CONVERT_BE16(event->rect.y_pos)
                                      + CONVERT_BE16(event->rect.height) - 1);

        CALLBACK_LOG(4, vc_obj(cl->vc),
                     "Got update request (%d, %d) size (%d, %d) incremental=%d",
                     rect.x_pos, rect.y_pos, rect.width, rect.height,
                     event->incremental);
        // If client does not support resize, it may have a
        // smaller resolution.
        gfx_rect_t dirty = intersect_rects(
                rect, create_rect(0, 0, cl->width - 1, cl->height - 1));

        // In this Threaded Context callback we just store the
        // dirty rectangle. Screen data is pushed to the clients from
        // the console real-time update events.
        pthread_mutex_lock(&cl->client_mutex);
        if (!cl->continuous_updates || !event->incremental)
                cl->update_request = bounding_box(cl->update_request, dirty);

        // Ignore incremental flag in the first update request after a resize
        // This works around the problem that the dirty data has already been
        // processed before the client sends an update request.
        gfx_rect_t r = cl->update_request;
        if (cl->resize && r.x_pos == 0 && r.y_pos == 0
            && r.width == cl->width && r.height == cl->height) {
                cl->resize = false;
                event->incremental = false;
        }
        if (!event->incremental)
                update_client_dirty(cl, dirty);
        pthread_mutex_unlock(&cl->client_mutex);

        // If client has lost the data, make sure the console will send it.
        start_writer_thread(cl);
}

// Handle client-to-server FrameBufferUpdateRequest message.
// Return false on error.
static bool
vnc_client_update_request(vnc_client_t *cl)
{
        vnc_fb_update_event_t event;
        ssize_t len = sizeof event - 1;
        if (vnc_client_get_data(cl, (uint8 *)&event + 1, len) < len)
                return false;

        handle_fb_update_event(cl, &event);

        // SetColourMapEntries message should not be sent until
        // server has received at least one FrameBufferUpdateRequest.
        if (cl->send_colour_map)
                vnc_client_set_colour_map(cl);
        return true;
}

// Handle client-to-server KeyEvent message. Return false on error.
static bool
vnc_client_key_event(vnc_client_t *cl)
{
        vnc_key_event_t event;
        ssize_t len = sizeof event - 1;
        if (vnc_client_get_data(cl, (uint8 *)&event + 1, len) < len)
                return false;

        handle_key_event(cl->vc, &event);
        return true;
}

// Handle client-to-server PointerEvent message. Return false on error.
static bool
vnc_client_pointer_event(vnc_client_t *cl)
{
        vnc_pointer_event_t event;
        ssize_t len = sizeof event - 1;
        if (vnc_client_get_data(cl, (uint8 *)&event + 1, len) < len)
                return false;

        handle_mouse_event(cl->vc, &event);
        return true;
}

// Read and ignore client-to-server CutText message. Return false on error.
static bool
vnc_client_cut_text(vnc_client_t *cl)
{
        vnc_client_cut_text_t msg;

        // Leading message type already fetched.
        ssize_t len = sizeof msg - 1;
        if (vnc_client_get_data(cl, (uint8 *)&msg + 1, len) < len)
                return false;
        uint32 text_len = CONVERT_BE32(msg.text_len);
        CALLBACK_LOG(2, vc_obj(cl->vc),
                     "Got cut text event length %u", text_len);
        uint8 *text = MM_MALLOC(text_len, uint8);
        len = text_len;
        bool ok = (vnc_client_get_data(cl, text, len) >= len);
        
        // Ignore cut text
        MM_FREE(text);
        return ok;
}

// Handle client-to-server EnableContinuousUpdates message.
// Return false on error.
static bool
vnc_client_enable_continuous(vnc_client_t *cl)
{
        vnc_fb_update_event_t event;
        ssize_t len = sizeof event - 1;
        if (vnc_client_get_data(cl, (uint8 *)&event + 1, len) < len)
                return false;

        bool continuous = handle_enable_continuous_event(cl, &event);
        // Acknowledge to client that continuous updates are now off.
        if (!continuous)
                vnc_client_send_eocu(cl);
        return true;
}

// Read and check VNC password
static bool
vnc_client_check_auth(vnc_client_t *cl)
{
        uint8 cipher[16];
        if (vnc_client_get_data(cl, cipher, sizeof cipher) < sizeof cipher)
                return false;

        uint8 plain[16];
        uint8 key[8] = {0};
        if (strlen(cl->vc->password) <= sizeof key)
                memcpy(key, cl->vc->password, strlen(cl->vc->password));
        else
                memcpy(key, cl->vc->password, sizeof key);

        CALL(cl->vc->des, set_key)((bytes_t){.data = key,
                                             .len = sizeof key}, true);
        CALL(cl->vc->des, des)((buffer_t){.data = plain, .len = sizeof plain},
                               (bytes_t){.data = cipher, .len = sizeof cipher});

        STATIC_ASSERT(sizeof plain == sizeof cl->challenge);
        cl->authenticated = (memcmp(plain, cl->challenge, sizeof plain) == 0);
        return true;
}

// The given client is running and data has arrived on the client socket.
// Read and handle any client-to-server message, or disconnect.
static void
vnc_client_update_running(vnc_client_t *cl)
{
        uint8 message_type;
        ssize_t len = sizeof message_type;

        // No data means client disconnected.
        ssize_t ret = vnc_client_get_data_non_block(cl, &message_type, len);
        if (ret <= 0) {
                if (ret == 0) {
                        CALLBACK_LOG(2, vc_obj(cl->vc), "Client disconnected");
                        close_client(cl);
                }
                return;
        }
        do {
                CALLBACK_LOG(4, vc_obj(cl->vc),
                             "Got client cmd: %d", (int)message_type);
        
                switch ((vnc_client_cmd_t)message_type) {
                case VNC_Client_Cmd_SetPixelFormat:
                        if (!vnc_client_set_pixel_format(cl)) {
                                CALLBACK_LOG(2, vc_obj(cl->vc),
                                             "Error handling SetPixelFormat");
                                close_client(cl);
                        }
                        break;
                case VNC_Client_Cmd_SetEncodings:
                        if (!vnc_client_set_encodings(cl)) {
                                CALLBACK_LOG(2, vc_obj(cl->vc),
                                             "Error handling SetEncodings");
                                close_client(cl);
                        }
                        break;
                case VNC_Client_Cmd_UpdateRequest:
                        if (!vnc_client_update_request(cl)) {
                                CALLBACK_LOG(2, vc_obj(cl->vc),
                                             "Error reading"
                                             " FrameBufferUpdateRequest");
                                close_client(cl);
                        }
                        break;
                case VNC_Client_Cmd_KeyEvent:
                        if (!vnc_client_key_event(cl)) {
                                CALLBACK_LOG(2, vc_obj(cl->vc),
                                             "Error reading KeyEvent");
                                close_client(cl);
                        }
                        break;
                case VNC_Client_Cmd_PointerEvent:
                        if (!vnc_client_pointer_event(cl)) {
                                CALLBACK_LOG(2, vc_obj(cl->vc),
                                             "Error reading PointerEvent");
                                close_client(cl);
                        }
                        break;
                case VNC_Client_Cmd_CutText:
                        if (!vnc_client_cut_text(cl)) {
                                CALLBACK_LOG(2, vc_obj(cl->vc),
                                             "Error handling ClientCutText");
                                close_client(cl);
                        }
                        break;
                case VNC_Client_Cmd_EnableContinuousUpdates:
                        if (!vnc_client_enable_continuous(cl)) {
                                CALLBACK_LOG(2, vc_obj(cl->vc),
                                             "Error reading"
                                             " EnableContinuousUpdates");
                                close_client(cl);
                        }
                        break;
                default:
                        close_client(cl);
                }
        } while (!client_dead(cl)
                 && vnc_client_get_data_non_block(cl, &message_type, len) > 0);
        CALLBACK_LOG(4, vc_obj(cl->vc), "No more cmds");
}

// Called when there is data ready to be read on the connection.
// If in connection phase, act accordingly, or handle client to server
// messages if running.
static void
external_connection_events_on_input(conf_object_t *obj, void *arg)
{
	vnc_client_t *cl = arg;
	int old_state = cl->state;

	switch (cl->state) {
	case VNC_Client_State_Version: {
                if (!vnc_client_set_version(cl)) {
			CALLBACK_LOG(2, vc_obj(cl->vc),
                                     "Could not read client version.");
                        close_client(cl);
                } else
                        cl->state = VNC_Client_State_Select_Auth;
		break;		
	}
	case VNC_Client_State_Select_Auth:
                /* In version 3.3 the client cannot choose authentication. */
		if (!vnc_version_eq(cl->version, 3, 3)) {
                        uint8 buf;
                        ssize_t len = sizeof buf;
                        ssize_t ret = vnc_client_get_data(cl, &buf, len);
			if (ret < len) {
                                if (ret >= 0) {
                                        CALLBACK_LOG(
                                                2, vc_obj(cl->vc),
                                                "Could not read client"
                                                " auth info");
                                        close_client(cl);
                                }
                        } else {
                                cl->auth_type = (vnc_auth_type_t)buf;
                                cl->state = (cl->auth_type == VNC_Auth_Password)
                                        ? VNC_Client_State_Auth
                                        : VNC_Client_State_Auth_Reply;
                        }
			break;
		} else {
                        ASSERT(!cl->vc->password);
                        cl->auth_type = VNC_Auth_None;
                }
                /* Fallthrough */
	case VNC_Client_State_Auth:
                if (cl->auth_type == VNC_Auth_Password) {
                        if (!vnc_client_check_auth(cl)) {
                                CALLBACK_LOG(2, vc_obj(cl->vc),
                                             "Could not read auth response");
                                close_client(cl);
                        } else {
                                cl->state = VNC_Client_State_Auth_Reply;
                        }
                        break;
                } else {
                        ASSERT(vnc_version_eq(cl->version, 3, 3));
                        // No authentication necessary.
                        // Fall through
                }
        case VNC_Client_State_Auth_Reply:
                cl->state = VNC_Client_State_Init;
                /* Fallthrough */
	case VNC_Client_State_Init: {
                uint8 buf;
                ssize_t len = sizeof buf;
                ssize_t ret = vnc_client_get_data(cl, &buf, len);
		if (ret < len) {
                        if (ret == 0) {
                                CALLBACK_LOG(2, vc_obj(cl->vc),
                                             "Could not read client init data");
                                close_client(cl);
                        }
                } else {
                        // We ignore the client init data.
                        if (buf == 0)
                                CALLBACK_LOG(2, vc_obj(cl->vc),
                                             "Client wants exclusive rights.");
                        else
                                CALLBACK_LOG(2, vc_obj(cl->vc),
                                             "Client can share desktop.");
		}
		break;
	}
	case VNC_Client_State_Running:
                // Process client to server messages.
                vnc_client_update_running(cl);
		break;
	}

        // Send response to client.
	if (!client_dead(cl) && old_state != cl->state)
		vnc_client_write(cl);
}

// Define server preferred pixel format.
static vnc_pixelformat_t
vnc_server_pixel_format()
{
	return (vnc_pixelformat_t){
                .bpp = 32, .depth = 32, .endian = 0, .truecolor = 1,
                .red_max = (1 << 8) - 1, .green_max = (1 << 8) - 1,
                .blue_max = (1 << 8) - 1,
                .red_shift = 16, .green_shift = 8, .blue_shift = 0};
}

// Setup VNC client data structure given connected client socket fd.
static vnc_client_t *
vnc_console_new_client(vnc_console_t *vc)
{
	vnc_client_t *cl = MM_ZALLOC(1, vnc_client_t);
	cl->state = VNC_Client_State_Version;
	cl->vc = vc;
        cl->connected = true;

        // Dirty rectangle is empty until connection established.
        pthread_mutex_init(&cl->client_mutex, NULL);
        pthread_mutex_lock(&cl->client_mutex);
        cl->width = vc->screen_width;
        cl->height = vc->screen_height;
        cl->screen = vc->screen;
        cl->format = vnc_server_pixel_format();
        cl->update_request = (gfx_rect_t){ 0 };
        VINIT(cl->dirty);
        cl->continuous_updates = false;
        cl->continuous_request = (gfx_rect_t){ 0 };
        pthread_mutex_unlock(&cl->client_mutex);

        // Init output buffer.
        VINIT(cl->encode_buf);
        pthread_mutex_init(&cl->output.lock, NULL);
        pthread_cond_init(&cl->output.exit_cond, NULL);
        VINIT(cl->output.bufs[0]);
        VINIT(cl->output.bufs[1]);
        
        // Add client at start of list.
	cl->next = vc->clients;
	vc->clients = cl;

        init_zlib_stream(&cl->tight_strm, vc->zlib_level);
        init_zlib_stream(&cl->zrle_strm, vc->zlib_level);

        // Default encoding
        cl->encoder.encode_rect = vnc_raw_rect_encoder;
        cl->encoding = VNC_Encoding_Raw;

        uint64 t = os_current_time();
        uint32 seed[] = {t >> 32, t & 0xffffffff};
        cl->rand_state = genrand_init_array(seed, sizeof seed / sizeof *seed);
	return cl;
}

// Callback when there is new connection.
// Hence called when a telnet client connects.
// Will setup vnc_client data structure.
static void
external_connection_events_on_accept(
        conf_object_t *obj,
        conf_object_t *server,
        uint64 id)
{
	vnc_console_t *vc = vnc_data(obj);
        vnc_client_t *cl = vnc_console_new_client(vc);
        SIM_LOG_INFO(1, vc_obj(vc), Gfx_Console_Log_VNC,
                     "New VNC connection established");
        INIT_REQUIRED_IFACE(&cl->server, external_connection_ctl, server);
        CALL(cl->server, accept)(id, cl, false);

        // Send initial data to client.
	SIM_register_work(vnc_client_write, cl);

        // Make sure console will not ignore screen updates.
        gfx_set_vnc_connected(vc->gc->input_data, true);

        CALL(cl->server, notify)(cl, Sim_NM_Read,
                                 Sim_EM_Thread, true);
}

enum {
        Server_TCP,
        Server_UNIX,
        Server_Pipe,
};

typedef struct {
        const char *port;
        const char *class;
        const char *desc;
} server_info_t;

#ifdef _WIN32
static const server_info_t servers[] = {
        [Server_TCP] = {.port = "tcp", .class = "tcp-server",
                        .desc = "VNC TCP server"},
        [Server_Pipe] = {.port = "named_pipe",
                         .class = "named-pipe-server",
                         .desc = "VNC Windows named pipe server"},
};
#else
static const server_info_t servers[] = {
        [Server_TCP] = {.port = "tcp", .class = "tcp-server",
                        .desc = "VNC TCP server"},
        [Server_UNIX] = {.port = "unix_socket",
                         .class = "unix-socket-server",
                         .desc = "VNC Unix domain socket server"},
};
#endif

// Implementation of vnc_server_v2 interface.
static bool
vnc_server_listening(conf_object_t *obj)
{
        attr_value_t port = SIM_get_attribute(
                SIM_object_descendant(obj, servers[Server_TCP].port),
                "port");
        bool ok = SIM_attr_is_integer(port);
#ifndef _WIN32
        attr_value_t socket = SIM_get_attribute(
                SIM_object_descendant(obj, servers[Server_UNIX].port),
                "socket_name");
        ok = ok || SIM_attr_is_string(socket);
#else
        attr_value_t pipe = SIM_get_attribute(
                SIM_object_descendant(obj, servers[Server_Pipe].port),
                "pipe_name");
        ok = ok || SIM_attr_is_string(pipe);
#endif
        return ok;
}

static void
vnc_server_v2_disconnect(conf_object_t *obj)
{
        vnc_console_t *vc = vnc_data(obj);
        while (vc->clients) {
                vnc_client_t *client = vc->clients;
                vc->clients = client->next;

                wait_for_write_thread_exit(client);

                // Clear the backpointer, since the vc struct will typically
                // be released before the client pointer. */
                client->vc = NULL;

                // This will actually perform a deferred free of the client.
                client->next = NULL;
                close_client(client);
        }
}

static void
server_shutdown(vnc_console_t *vc)
{
        vnc_server_v2_disconnect(vc_obj(vc));
        conf_object_t *port = SIM_object_descendant(vc_obj(vc),
                                                    servers[Server_TCP].port);
        attr_value_t attr = SIM_make_attr_nil();
        SIM_set_attribute(port, "port", &attr);
        (void)SIM_clear_exception();
        SIM_free_attribute(attr);

#ifndef _WIN32
        port = SIM_object_descendant(vc_obj(vc), servers[Server_UNIX].port);
        attr = SIM_make_attr_nil();
        SIM_set_attribute(port, "socket_name", &attr);
        (void)SIM_clear_exception();
        SIM_free_attribute(attr);
#endif
}

static int
vnc_server_num_clients(conf_object_t *obj)
{
        vnc_console_t *vc = vnc_data(obj);
        return num_clients(vc);
}


static conf_object_t *
set_auth_object(conf_object_t *obj)
{
        conf_class_t *des_cl = SIM_get_class("crypto_engine_vnc_des");
        if (!des_cl) {
                (void)SIM_clear_exception();
                return NULL;
        }
        vnc_console_t *vc = vnc_data(obj);
        strbuf_t des_name = sb_newf("%s.auth", SIM_object_name(obj));
        conf_object_t *des = SIM_get_object(sb_str(&des_name));
        if (!des) {
                (void)SIM_clear_exception();
                attr_value_t empty = SIM_alloc_attr_list(0);
                des = SIM_create_object(des_cl, sb_str(&des_name), empty);
                SIM_attr_free(&empty);
                ASSERT(des);
                INIT_REQUIRED_IFACE(&vc->des, vnc_des, des);
        }
        sb_free(&des_name);
        return des;
}

static attr_value_t
get_password(conf_object_t *obj)
{
        vnc_console_t *vc = vnc_data(obj);
        return SIM_make_attr_string(vc->password);
}

static set_error_t
set_password(conf_object_t *obj, attr_value_t *val)
{
        vnc_console_t *vc = vnc_data(obj);
        const char *password = SIM_attr_is_string(*val)
                ? SIM_attr_string(*val) : NULL;
        if (password) {
                if (strlen(password) > 8 || strlen(password) == 0) {
                        SIM_attribute_error("VNC passwords must be"
                                            " 1 to 8 characters");
                        return Sim_Set_Illegal_Value;
                }

                if (!set_auth_object(obj)) {
                        SIM_attribute_error(
                                "VNC password authentication requires the"
                                " Crypto-Engine package to be installed.");
                        return Sim_Set_Illegal_Value;
                }

                MM_FREE(vc->password);
                vc->password = MM_STRDUP(password);
        } else {
                MM_FREE(vc->password);
                vc->password = NULL;
        }
        return Sim_Set_Ok;
}

static attr_value_t
get_allow_compression(conf_object_t *obj)
{
        vnc_console_t *vc = vnc_data(obj);
        return SIM_make_attr_boolean(vc->allow_compression);
}

static set_error_t
set_allow_compression(conf_object_t *obj, attr_value_t *val)
{
        vnc_console_t *vc = vnc_data(obj);
        vc->allow_compression = SIM_attr_boolean(*val);
        return Sim_Set_Ok;
}

static attr_value_t
get_zlib_level(conf_object_t *obj)
{
        vnc_console_t *vc = vnc_data(obj);
        return SIM_make_attr_uint64(vc->zlib_level);
}

static set_error_t
set_zlib_level(conf_object_t *obj, attr_value_t *val)
{
        vnc_console_t *vc = vnc_data(obj);
        int level = SIM_attr_integer(*val);
        if (level >= 0 && level <= 9) {
                vc->zlib_level = level;
                return Sim_Set_Ok;
        } else {
                strbuf_t msg = sb_newf("Invalid zlib compression level %d",
                                       level);
                SIM_attribute_error(sb_str(&msg));
                sb_free(&msg);
                return Sim_Set_Illegal_Value;
        }
}

static int
uint128_cmp(const void *a, const void *b)
{
        __uint128_t x = *(const __uint128_t *)a;
        __uint128_t y = *(const __uint128_t *)b;
        if (x != y)
                return x < y ? -1 : 1;
        else
                return 0;
}

static attr_value_t
get_keymap(conf_object_t *obj)
{
        vnc_console_t *vc = vnc_data(obj);
        VECT(__uint128_t) map = VNULL;
        HT_FOREACH_INT(&vc->keymap, it) {
                VADD(map, (((__uint128_t)ht_iter_int_key(it) << 64)
                           | (uint64)ht_iter_int_value(it)));
        }
        VSORT(map, uint128_cmp);

        attr_value_t ret = SIM_alloc_attr_list(VLEN(map));
        int i = 0;
        VFORT(map, __uint128_t, e) {
                SIM_attr_list_set_item(
                        &ret, i++,
                        SIM_make_attr_list(
                                2, SIM_make_attr_uint64(e >> 64),
                                SIM_make_attr_uint64(e & UINT64_MAX)));
        }
        VFREE(map);
        return ret;
}

static set_error_t
set_keymap(conf_object_t *obj, attr_value_t *val)
{
        vnc_console_t *vc = vnc_data(obj);
        ht_clear_int_table(&vc->keymap, false);
        for (int i = 0; i < SIM_attr_list_size(*val); i++) {
                attr_value_t pair = SIM_attr_list_item(*val, i);
                ht_insert_int(&vc->keymap,
                              SIM_attr_integer(SIM_attr_list_item(pair, 0)),
                              (void *)SIM_attr_integer(
                                      SIM_attr_list_item(pair, 1)));
        }
        return Sim_Set_Ok;
}

vnc_console_t *
make_gfx_vnc(gfx_console_t *gc)
{
        vnc_console_t *vc = MM_ZALLOC(1, vnc_console_t);
        vc->gc = gc;
        vc->format = vnc_server_pixel_format();
        vc->iface = SIM_C_GET_INTERFACE(vc_obj(vc), gfx_console_backend);
        vc->zlib_level = 6;
        vc->allow_compression = true;
        ht_init_int_table(&vc->keymap);
        set_default_keymap(vc);
        for (int i = 0; i < ALEN(servers); i++) {
                if (servers[i].port) {
                        SIM_set_attribute_default(
                                SIM_object_descendant(vc_obj(vc),
                                                      servers[i].port),
                                "client", SIM_make_attr_object(vc_obj(vc)));
                }
        }
        return vc;
}

void
destroy_gfx_vnc(vnc_console_t *vc)
{
        server_shutdown(vc);
        ht_delete_int_table(&vc->keymap, false);
        MM_FREE(vc);
}

void
register_gfx_vnc(conf_class_t *cl)
{
        static const vnc_server_v2_interface_t vnc_server_v2_iface = {
                .listening = vnc_server_listening,
                .num_clients = vnc_server_num_clients,
                .disconnect = vnc_server_v2_disconnect,
        };
        SIM_REGISTER_INTERFACE(cl, vnc_server_v2, &vnc_server_v2_iface);

        for (int i = 0; i < ALEN(servers); i++) {
                if (servers[i].port) {
                        SIM_register_port(cl, servers[i].port,
                                          SIM_get_class(servers[i].class),
                                          servers[i].desc);
                }
        }

        SIM_register_attribute(
                cl, "vnc_allow_compression", 
                get_allow_compression,
                set_allow_compression,
                Sim_Attr_Optional,
                "b",
                "If TRUE, then compressed encodings are used if the VNC client"
                " requests it, otherwise only raw encoding is used."
                " Default is TRUE. Supported encodings are ZRLE and Tight.");

        SIM_register_attribute(
                cl, "vnc_zlib_level", 
                get_zlib_level,
                set_zlib_level,
                Sim_Attr_Optional,
                "i",
                "VNC zlib compression level, in range 0..9, default 6.");

        SIM_register_attribute(
                cl, "vnc_keymap",
                get_keymap, set_keymap,
                Sim_Attr_Optional,
                "[[ii]*]",
                "VNC keyboard mapping for special keys: a list of pairs,"
                " keysym -> sim_key_t.");

        SIM_register_attribute(
                cl, "vnc_password",
                get_password, set_password,
                Sim_Attr_Optional,
                "s|n",
                "VNC server password. If this is set to a string, which must"
                " be non-empty, then the server will use the VNC"
                " authentication mechanism and require clients to provide"
                " the password on connect.");

        static const external_connection_events_interface_t ext_iface = {
                .on_accept = external_connection_events_on_accept,
                .on_input = external_connection_events_on_input,
        };
        SIM_REGISTER_INTERFACE(cl, external_connection_events, &ext_iface);
}
