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

#include "screen.h"
#include <simics/util/vect.h>
#include <simics/util/swabber.h>
#include <simics/base/conf-object.h>
#include <simics/base/log.h>
#include <simics/base/notifier.h>
#include <simics/base/configuration.h>
#include <simics/simulator/conf-object.h>
#include <simics/simulator/callbacks.h>
#include <simics/simulator/hap-consumer.h>
#include <simics/simulator-iface/preferences.h>
#include <simics/simulator-iface/consoles.h>
#include "driver.h"
#include "vt100.h"
#include "text-console.h"
#include "text-inline.h"

typedef char chr_type_t;
 
/* Line metadata. */
typedef struct {
        int length;    /* Length of this line. */
        bool wrap;     /* This line wraps to the next line (length = width). */
} line_info_t;

typedef struct {
        char *line;
        text_console_attrib_t *attrib;
        line_info_t info;
} sb_line_t;

typedef VECT(chr_type_t) screen_data_t;
typedef VECT(text_console_attrib_t) screen_attrib_t;
typedef QUEUE(sb_line_t) scrollback_data_t;

// Console visibility notifiers
static notifier_type_t notify_open;
static notifier_type_t notify_close;

struct screen {
        text_console_t *tc;              /* Dirty shortcut to parent struct. */

        const char *window_title;

        // Size of visible console screen, in characters.
        int width;
        int height;

        int max_sb_lines;         /* Max number of scrollback lines. */

        /* Scrollback data. */
        scrollback_data_t scrollback;
        
        /* Visible screen character data.
           The length of the vector is height * width. */
        screen_data_t screen;

        /* Screen attribute data, having the same length as the
           screen vector, and indexed in the same way. */
        screen_attrib_t attrib;

        /* Line metadata. The length of the vector is height. */
        VECT(line_info_t) line_info;

        /* Default text foreground and background colours (0xBBGGRR). */
        uint32 default_fg_col;
        uint32 default_bg_col;

        // Console frontend object.
        conf_object_t *frontend;
        const text_console_frontend_interface_t *frontend_iface;

        // Handle that identifies this console to the frontend, in case the
        // frontend object manages several consoles.
        int frontend_handle;

        // Is console GUI window visible?
        int visible;

        // Frontend deletion hap callback
        hap_handle_t frontend_del_hap;
};

static conf_object_t *
sc_obj(screen_t *sc) { return to_obj(sc->tc); }

static console_driver_data_t *
sc_sd(screen_t *sc) { return sc->tc->driver_data; }

static vt100_data_t *
sc_vt(screen_t *sc) { return sc->tc->vt100_data; }

/* Default values for the default fg/bg colours (0xBBGGRR). */
#define DEFAULT_BG_COL 0xffffff
#define DEFAULT_FG_COL 0x000000

// Invalid colour value (for initialisation)
#define INVALID_COL UINT32_MAX

// Default maximum number of scrollback lines
#define DEFAULT_MAX_SB_LINES 10000

static void
write_prefs_default_colors(uint32 fg, uint32 bg)
{
        conf_object_t *prefs = SIM_get_object("prefs");
        ASSERT(prefs);
        const preference_interface_t *iface =
                SIM_C_GET_INTERFACE(prefs, preference);
        ASSERT(iface);
        
        iface->set_preference_for_module_key(
                prefs, SIM_make_attr_uint64(fg),
                "text-console", "default-foreground");
        iface->set_preference_for_module_key(
                prefs, SIM_make_attr_uint64(bg),
                "text-console", "default-background");
}

// Size in bytes for num_lines of screen character data.
static size_t
screen_line_size(screen_t *sc, int num_lines)
{
        return num_lines * sc->width * VELEMSIZE(sc->screen);
}

// Size in bytes for num_lines of screen attribute data.
static size_t
attrib_line_size(screen_t *sc, int num_lines)
{
        return num_lines * sc->width * VELEMSIZE(sc->attrib);
}

// Pointer to line <line> of <screen>, which must have <width> columns.
static chr_type_t *
screen_vec_line(screen_data_t screen, int line, int width)
{
        return VVEC(screen) + line * width;
}

// Pointer to line <line> of <attrib>, which must have <width> columns.
static text_console_attrib_t *
attrib_vec_line(screen_attrib_t attrib, int line, int width)
{
        return VVEC(attrib) + line * width;
}

// Pointer to line <line> of console screen character data.
// <line> must be in [0, height)
static chr_type_t *
screen_line(screen_t *sc, int line)
{
        return screen_vec_line(sc->screen, line, sc->width);
}

// Pointer to line <line> of console screen attribute data.
// <line> must be in [0, height)
static text_console_attrib_t *
attrib_line(screen_t *sc, int line)
{
        return attrib_vec_line(sc->attrib, line, sc->width);
}

// Construct attribute data structure for given attribute and colours.
// See definition of text_console_attrib_t for details.
static text_console_attrib_t
attrib_data(uint8 attrib, text_console_colour_t fg, text_console_colour_t bg)
{
        return (text_console_attrib_t){ .attrib = attrib, .fg = fg, .bg = bg };
}

// Default attribute used for empty parts of the screen.
static text_console_attrib_t
default_attrib()
{
        return attrib_data(0,
                           Text_Console_Colour_Default_Foreground,
                           Text_Console_Colour_Default_Background);
}

/* Attribute used for erasing, with background colour bg. */
static text_console_attrib_t
erase_attrib(text_console_colour_t bg)
{
        return attrib_data(
                0, Text_Console_Colour_Default_Foreground, bg);
}

// Clear screen, i.e. set everything to space.
static void
clear_screen(screen_data_t *screen)
{
        VFORI(*screen, i) {
                VSET(*screen, i, ' ');
        }
}

// Clear screen, i.e. set everything to default_attrib().
static void
clear_attrib(screen_attrib_t *attrib)
{
        VFORI(*attrib, i) {
                VSET(*attrib, i, default_attrib());
        }
}

// Size of screen data (in characters)
static int
screen_size(screen_t *sc)
{
        return sc->width * sc->height;
}

static screen_t *
screen_data(conf_object_t *obj)
{
        return from_obj(obj)->screen;
}

// Send window title change to the frontend object.
static void
frontend_set_title(screen_t *sc, const char *title)
{
        sc->frontend_iface->set_title(
                sc->frontend, sc->frontend_handle, title, title);
}

// Send visible screen size change to the frontend object.
static void
frontend_set_size(screen_t *sc, int width, int height)
{
        sc->frontend_iface->set_size(sc->frontend, sc->frontend_handle,
                                     width, height);
}

// Send screen rectangle update to the frontend object.
static void
frontend_set_contents(screen_t *sc, int top, int left, int bottom, int right)
{
        sc->frontend_iface->set_contents(
                sc->frontend, sc->frontend_handle, top, left, bottom, right,
                VVEC(sc->screen), VVEC(sc->attrib));
}

// Send an append line message to the frontend object.
static void
frontend_append_line(screen_t *sc)
{
        // The console data structure has already been updated, so provide
        // pointer to this last line.
        sc->frontend_iface->append_text(
                sc->frontend, sc->frontend_handle, 1,
                screen_line(sc, sc->height - 1),
                attrib_line(sc, sc->height - 1));
}

// Send new cursor position to the frontend object.
void
frontend_set_cursor_pos(screen_t *sc, int row, int col)
{
        sc->frontend_iface->set_cursor_pos(
                sc->frontend, sc->frontend_handle, row, col);
}

// Send new maximum scrollback size to the frontend object.
static void
frontend_set_scrollback_size(screen_t *sc, int num_lines)
{
        sc->frontend_iface->set_max_scrollback_size(
                sc->frontend, sc->frontend_handle, num_lines);
}

// Make sure frontend updates the whole screen (maximum scrollback and
// visible screen) with the current console data.
static void
frontend_update_screen(screen_t *sc)
{
        chr_type_t *sb_text = MM_MALLOC(QLEN(sc->scrollback) * sc->width,
                                        chr_type_t);
        text_console_attrib_t *sb_attrib =
                MM_MALLOC(QLEN(sc->scrollback) * sc->width,
                          text_console_attrib_t);
        for (int i = 0; i < QLEN(sc->scrollback); i++) {
                memcpy(sb_text + i * sc->width,
                       QGET(sc->scrollback, i).line,
                       sc->width * sizeof *sb_text);
                memcpy(sb_attrib + i * sc->width,
                       QGET(sc->scrollback, i).attrib,
                       sc->width * sizeof *sb_attrib);
        }
        
        sc->frontend_iface->refresh_screen(
                sc->frontend, sc->frontend_handle, 
                VVEC(sc->screen), VVEC(sc->attrib), sb_text, sb_attrib,
                QLEN(sc->scrollback));

        MM_FREE(sb_text);
        MM_FREE(sb_attrib);
}

// Change number of rows of the visible part of the screen.
// New lines appear at the bottom, filled with space and default attribute.
// Only updates console data structure, not frontend.
static void
change_screen_height(screen_t *sc, int height)
{
        // Construct a new local VECT with the new dimensions.
        screen_data_t screen = VNULL;
        VRESIZE(screen, sc->width * height);
        ASSERT(VECT_ELEMS_PRESENT(screen));
        clear_screen(&screen);
        screen_attrib_t attrib = VNULL;
        VRESIZE(attrib, VLEN(screen));
        clear_attrib(&attrib);
        STATIC_ASSERT(VELEMSIZE(screen) == VELEMSIZE(sc->screen));
        STATIC_ASSERT(VELEMSIZE(attrib) == VELEMSIZE(sc->attrib));
        // Must change line_info structure as well.
        VECT(line_info_t) line_info = VNULL;
        VRESIZE(line_info, height);
        memset(VVEC(line_info), 0,
               VLEN(line_info) * VELEMSIZE(line_info));
        STATIC_ASSERT(VELEMSIZE(line_info) == VELEMSIZE(sc->line_info));

        // Copy relevant data from console to local VECT.
        // If screen is shrunk, we remove lines at the top of the scrollback,
        // i.e. the oldest (or yet unused) part of the scrollback.
        memcpy(screen_vec_line(screen, 0, sc->width),
               screen_line(sc, MAX(0, sc->height - height)),
               screen_line_size(sc, MIN(sc->height, height)));
        memcpy(attrib_vec_line(attrib, 0, sc->width),
               attrib_line(sc, MAX(0, sc->height - height)),
               attrib_line_size(sc, MIN(sc->height, height)));
        memcpy(VVEC(line_info),
               VVEC(sc->line_info) + MAX(0, sc->height - height),
               VELEMSIZE(line_info) * MIN(sc->height, height));

        // Replace console data with local VECT.
        VCOPY(sc->screen, screen);
        VCOPY(sc->attrib, attrib);
        VCOPY(sc->line_info, line_info);
        VFREE(screen);
        VFREE(attrib);
        VFREE(line_info);
}

// Increase number of columns in the scrollback data.
// Added columns are filled with space and default attribute.
static void
increase_scrollback_width(screen_t *sc, int width)
{
        // Nothing needs to be be done if columns are removed.
        ASSERT(width > sc->width);
        int num_lines = QLEN(sc->scrollback);
        for (int line = 0; line < num_lines; line++) {
                sb_line_t sb = QREMOVE(sc->scrollback);
                sb.line = MM_REALLOC(sb.line, width + 1, chr_type_t);
                sb.attrib = MM_REALLOC(sb.attrib, width,
                                       text_console_attrib_t);

                for (int i = sc->width; i < width; i++) {
                        sb.line[i] = ' ';
                        sb.attrib[i] = default_attrib();
                }
                sb.line[width] = '\0';
                QADD(sc->scrollback, sb);
        }
}

// Change number of columns of the screen.
// Columns are removed from the right.
// Added columns are filled with space and default attribute.
// Only updates console data structure, not frontend.
static void
change_screen_width(screen_t *sc, int width)
{
        // Construct a new local VECT with the new dimensions.
        screen_data_t screen = VNULL;
        VRESIZE(screen, width * sc->height);
        clear_screen(&screen);
        screen_attrib_t attrib = VNULL;
        VRESIZE(attrib, VLEN(screen));
        clear_attrib(&attrib);
        STATIC_ASSERT(VELEMSIZE(screen) == VELEMSIZE(sc->screen));
        STATIC_ASSERT(VELEMSIZE(attrib) == VELEMSIZE(sc->attrib));

        // Copy relevant data from console to local VECT.
        for (int row = 0; row < sc->height; row++) {
                memcpy(screen_vec_line(screen, row, width),
                       screen_line(sc, row),
                       VELEMSIZE(screen) * MIN(width, sc->width));
                memcpy(attrib_vec_line(attrib, row, width),
                       attrib_line(sc, row),
                       VELEMSIZE(attrib) * MIN(width, sc->width));
        }
        // Replace console data with local VECT.
        VCOPY(sc->screen, screen);
        VCOPY(sc->attrib, attrib);
        VFREE(screen);
        VFREE(attrib);
}

// Set dimensions of console visible screen. Does not update frontend.
// Return true if the console data structure was updated
// i.e. if the given dimensions are different from the current ones.
static bool
resize_screen(screen_t *sc, int width, int height)
{
        bool updated = false;
        if (height != sc->height) {
                change_screen_height(sc, height);
                sc->height = height;
                updated = true;
        }
        if (width != sc->width) {
                change_screen_width(sc, width);
                if (width > sc->width)
                        increase_scrollback_width(sc, width);
                sc->width = width;        
                updated = true;
        }
        return updated;
}

// Set dimensions of console visible screen and also update frontend.
static void
resize_screen_and_frontend(screen_t *sc, int width, int height)
{
        ASSERT(width > 0 && height > 0);
        int old_height = sc->height;
        if (resize_screen(sc, width, height)) {
                // Update VT100 parser state, e.g. cursor position.
                screen_size_change(sc_vt(sc), old_height, height);
                // Make sure frontend object exists
                if (SIM_object_is_configured(sc_obj(sc)))
                        frontend_set_size(sc, sc->width, sc->height);
        }
}

// Set width of console screen (and update frontend)
void
set_screen_width(screen_t *sc, int width)
{
        resize_screen_and_frontend(sc, width, sc->height);
}

// Set console screen size (and update frontend)
void
update_screen_size(screen_t *sc, int width, int height)
{
        resize_screen_and_frontend(sc, width, height);
}

/* Clear lines [top,top+n) by filling them with spaces of background colour
   bg, and reset line info data for those lines. */
static void
clear_lines(screen_t *sc, int top, int num_lines,
            text_console_colour_t bg)
{
        text_console_attrib_t attr = erase_attrib(bg);
        for (int row = 0; row < num_lines; row++) {
                for (int col = 0; col < sc->width; col++) {
                        VSET(sc->screen, sc->width * (top + row) + col, ' ');
                        VSET(sc->attrib,
                             sc->width * (top + row) + col,
                             attr);
                }
                VSET(sc->line_info, top + row, (line_info_t){0});
        }
}

// Empty region left..left+size-1 of line row of console screen data.
// i.e. fill character data with space, attribute data with default attributes.
static void
clear_region(screen_t *sc, int row, int left, int size,
             text_console_colour_t bg)
{
        ASSERT(left + size <= sc->width);
        text_console_attrib_t attr = erase_attrib(bg);
        for (int col = left; col < left + size; col++) {
                VSET(sc->screen, sc->width * row + col, ' ');
                VSET(sc->attrib, sc->width * row + col, attr);
        }
}

/* Clear the screen between (top, left) and (bottom, right) inclusive,
   wrapping around the (horizontal) edges as necessary.
   Cleared cells are filled with space of background colour bg. */
void
screen_erase(screen_t *sc,
             int top, int left, int bottom, int right, text_console_colour_t bg)
{
        ASSERT(left >= 0 && right >= 0
               && right < sc->width && left < sc->width);
        ASSERT(top >= 0 && bottom >= top && bottom < sc->height);
        
        if (top < bottom) {
                clear_region(sc, top, left, sc->width - left, bg);
                if (top < bottom - 1)
                        clear_lines(sc, top + 1, bottom - top - 1, bg);
                clear_region(sc, bottom, 0, right + 1, bg);
                frontend_set_contents(sc, top, 0, bottom, sc->width - 1);
        } else {
                clear_region(sc, top, left, right - left + 1, bg);
                frontend_set_contents(sc, top, left, bottom, right);
        }
}

/* Move num_lines lines of the console screen data from src_row to
   dst_row (0-based line numbers). */
static void
screen_move_lines(screen_t *sc, int dst_row, int src_row, int num_lines)
{
        ASSERT(dst_row >= 0 && src_row >= 0
               && dst_row + num_lines <= sc->height
               && src_row + num_lines <= sc->height);
        memmove(screen_line(sc, dst_row),
                screen_line(sc, src_row),
                screen_line_size(sc, num_lines));
        memmove(attrib_line(sc, dst_row),
                attrib_line(sc, src_row),
                attrib_line_size(sc, num_lines));
        memmove(VVEC(sc->line_info) + dst_row,
                VVEC(sc->line_info) + src_row,
                num_lines * VELEMSIZE(sc->line_info));
}

/* Move n characters and attributes from src_col to dst_col on row
   (0-based row and columns). */
static void
screen_move(screen_t *sc, int row, int dst_col, int src_col, int n)
{
        ASSERT(row >= 0 && row <= sc->height
               && src_col >= 0 && dst_col >= 0
               && src_col + n <= sc->width && dst_col + n <= sc->width);
        memmove(screen_line(sc, row) + dst_col,
                screen_line(sc, row) + src_col,
                n * VELEMSIZE(sc->screen));
        memmove(attrib_line(sc, row) + dst_col,
                attrib_line(sc, row) + src_col,
                n * VELEMSIZE(sc->attrib));
}

/* Delete num_chars characters to the right, at position (row, col) on the
   visible console screen. This is similar as when pressing Del, so characters
   to the right are moved leftwards. Only line row can be affected, hence
   num_chars must be at most width - col. */
void
screen_delete_chars(screen_t *sc,
                    int row, int col, int num_chars, text_console_colour_t bg)
{
        ASSERT(num_chars >= 0);
        ASSERT(col + num_chars <= sc->width);

        int move_chars = sc->width - num_chars - col;

        // Move data to the left.
        screen_move(sc, row, col, col + num_chars, move_chars);

        // Adjust line length.
        line_info_t info = VGET(sc->line_info, row);
        info.length -= num_chars;
        info.wrap = false;
        VSET(sc->line_info, row, info);

        // New empty region at the end of the line.
        clear_region(sc, row, col + move_chars,
                     sc->width - move_chars - col, bg);
        frontend_set_contents(sc, row, col, row, sc->width - 1);
}

/* Insert num_chars space characters at position (row, col) on the visible
   console screen. This is similar to pressing Space, so characters are moved
   to the right (but the cursor is not moved). Only line row can be affected,
   hence num_chars must be at most width - col. */
void
screen_insert_chars(screen_t *sc,
                    int row, int col, int num_chars, text_console_colour_t bg)
{
        ASSERT(num_chars >= 0);
        ASSERT(col + num_chars <= sc->width);

        // Move data to the right.
        screen_move(sc, row, col + num_chars, col, sc->width - col - num_chars);

        // Adjust line length.
        line_info_t info = VGET(sc->line_info, row);
        info.length += num_chars;
        VSET(sc->line_info, row, info);

        // New empty region at (row, col)
        clear_region(sc, row, col, num_chars, bg);
        frontend_set_contents(sc, row, col, row, sc->width - 1);
}

/* Scroll the region top..bottom (inclusive) n lines down,
   filling new empty lines with the colour bg. */
void
screen_scroll_down(screen_t *sc,
                   int top, int bottom, int n, text_console_colour_t bg)
{
        /* How many lines to move down. */
        int nmove = MAX(0, bottom + 1 - n - top);
        if (nmove > 0)
                screen_move_lines(sc, top + n, top, nmove);

        /* How many lines to clear. */
        int nclear = bottom + 1 - top - nmove;
        clear_lines(sc, top, nclear, bg);

        frontend_set_contents(sc, top, 0, bottom, sc->width - 1);
}

/* Scroll the region top..bottom (inclusive) n lines up,
   filling new empty lines with colour bg.
   Lines are not added to the scrollback. */
void
screen_scroll_up(screen_t *sc,
                 int top, int bottom, int n, text_console_colour_t bg)
{
        /* How many lines to move up. */
        int nmove = MAX(0, bottom + 1 - n - top);
        if (nmove > 0)
                screen_move_lines(sc, top, top + n, nmove);

        /* How many lines to clear. */
        int nclear = bottom + 1 - top - nmove;
        clear_lines(sc, bottom + 1 - nclear, nclear, bg);
                
        frontend_set_contents(sc, top, 0, bottom, sc->width - 1);
}

static void
ensure_max_scrollback_size(screen_t *sc)
{
        if (sc->max_sb_lines > 0) {
                for (int i = sc->max_sb_lines; i < QLEN(sc->scrollback); i++) {
                        sb_line_t data = QREMOVE(sc->scrollback);
                        MM_FREE(data.line);
                        MM_FREE(data.attrib);
                }
        }
}

static void
add_scrollback_line(screen_t *sc, const chr_type_t *chars,
                    const text_console_attrib_t *attr, line_info_t info)
{
        chr_type_t *line = MM_MALLOC(sc->width + 1, chr_type_t);
        memcpy(line, chars, sc->width * sizeof *line);
        line[sc->width] = '\0';
        text_console_attrib_t *attrib = MM_MALLOC(
                sc->width, text_console_attrib_t);
        memcpy(attrib, attr, sc->width * sizeof *attrib);
        
        sb_line_t data = {.line = line, .attrib = attrib, .info = info};
        QADD(sc->scrollback, data);
        ensure_max_scrollback_size(sc);
}

/* Scroll the region top..bottom (inclusive) one line up,
   filling new empty lines with colour bg.
   If the region is the entire screen, the topmost line is added to the
   scrollback. */
void
scroll_up_to_scrollback(screen_t *sc,
                        int top, int bottom, text_console_colour_t bg)
{
        if (top != 0 || bottom != sc->height - 1) {
                /* No scrollback addition. */
                screen_scroll_up(sc, top, bottom, 1, bg);
                return;
        }

        add_scrollback_line(sc, VVEC(sc->screen), VVEC(sc->attrib),
                            VGET(sc->line_info, 0));
        screen_move_lines(sc, 0, 1, sc->height - 1);        
        clear_lines(sc, sc->height - 1, 1, bg);

        /* Tell the frontend that the line should be moved to the
           scrollback. */
        frontend_append_line(sc);
}

// Return number of rows of the visible console screen.
int
get_screen_height(screen_t *sc)
{
        return sc->height;
}

// Return number of columns of the visible console screen.
int
get_screen_width(screen_t *sc)
{
        return sc->width;
}

/* Set character and attribute data at position (row, col) of the
   visible console screen.
   The text attribute should be a bitmask of text_console_text_attrib_t. */
void
set_screen_contents(screen_t *sc,
                    int row, int col, uint8 ch, uint8 attrib,
                    text_console_colour_t fg_colour,
                    text_console_colour_t bg_colour)
{
        chr_type_t output = ch;

        SIM_LOG_INFO(4, sc_obj(sc), Text_Console_Log_Screen,
                     "Set char 0x%x at (%d, %d) colours 0x%x 0x%x",
                     (int)output, row, col, fg_colour, bg_colour);
        VSET(sc->screen, row * sc->width + col, output);
        VSET(sc->attrib, row * sc->width + col,
             attrib_data(attrib, fg_colour, bg_colour));

        // Adjust line length.
        ASSERT(row < VLEN(sc->line_info));
        line_info_t info = VGET(sc->line_info, row);
        info.length = MAX(info.length, col + 1);
        VSET(sc->line_info, row, info);
        
        frontend_set_contents(sc, row, col, row, col);
}

// Send character to the visual console, passing through VT100 parser
// and update frontend.
void
text_console_output(screen_t *sc, uint8 value)
{
        // Strip away the high bit, because we neither interpret 8-bit VT
        // control sequences (that nobody uses), nor display anything but ASCII
        // at the moment. The latter may change.
        uint8 ch = value & 0x7f;
        vt100_output(sc->tc->vt100_data, ch);
}

// Mark specified line on the visible console screen as wrapping on
// to the next line.
void
set_wrapping_line(screen_t *sc, int line_num)
{
        ASSERT(line_num >= 0);
        ASSERT(line_num < VLEN(sc->line_info));
        line_info_t info = VGET(sc->line_info, line_num);
        info.wrap = true;
        VSET(sc->line_info, line_num, info);
}

static void
get_pos_string(screen_t *sc, strbuf_t *buf, int idx)
{
        chr_type_t c = VGET(sc->screen, idx);
        text_console_attrib_t a = VGET(sc->attrib, idx);

        if (a.fg == Text_Console_Colour_Default_Foreground)
                sb_addfmt(buf, "\x1b[39m");
        else
                sb_addfmt(buf, "\x1b[38;5;%um", a.fg);

        if (a.bg == Text_Console_Colour_Default_Background)
                sb_addfmt(buf, "\x1b[49m");
        else
                sb_addfmt(buf, "\x1b[48;5;%um", a.bg);

        // Add attributes and character
        sb_addfmt(buf, "\x1b[%um\x1b[%um%c",
                  a.attrib & Text_Console_Attrib_Bold ? 1 : 22,
                  a.attrib & Text_Console_Attrib_Underline ? 4 : 24, c);
}

// Set buf to VT100-parseable string of the current screen contents
void
get_screen_string(screen_t *sc, strbuf_t *buf)
{
        int cursor_x, cursor_y;
        get_cursor_pos(sc_vt(sc), &cursor_x, &cursor_y);

        for (int y = 0; y < cursor_y; y++) {
                line_info_t info = VGET(sc->line_info, y);

                for (int x = 0; x < info.length; x++)
                        get_pos_string(sc, buf, y * sc->width + x);
                // Ignore line wrapping for now since client side might be wider
                sb_addstr(buf, "\r\n");
        }

        for (int x = 0; x < cursor_x; x++)
                get_pos_string(sc, buf, cursor_y * sc->width + x);
}

// Implementation of text_console_backend interface.
static void
text_console_backend_input(conf_object_t *obj, text_console_key_t key,
                             text_console_modifier_t modifiers)
{        
        screen_t *sc = screen_data(obj);
        SIM_LOG_INFO(4, sc_obj(sc), Text_Console_Log_Screen,
                     "Frontend sent char 0x%x with modifiers %d",
                     key, modifiers);

        vt100_input(sc->tc->vt100_data, key, modifiers);
}

// Implementation of text_console_backend interface.
static void
text_console_backend_request_refresh(conf_object_t *obj)
{
        // We cannot refresh frontend immediately (in the same thread)
        if (SIM_object_is_configured(obj)) {
                screen_t *sc = screen_data(obj);
                frontend_update_screen(sc);
        }
}

static void
frontend_set_visible(screen_t *sc, bool visible)
{
        if (sc->frontend) {
                sc->frontend_iface->set_visible(
                        sc->frontend, sc->frontend_handle, visible);
        }
}

// Set visibility of console GUI window.
static void
console_set_visible(screen_t *sc, bool visible)
{
        sc->visible = visible;
        if (visible) {
                console_visible(sc_sd(sc));
                text_console_backend_request_refresh(sc_obj(sc));
                SIM_notify(sc_obj(sc), notify_open);
        } else
                SIM_notify(sc_obj(sc), notify_close);
}

// Implementation of text_console_backend interface.
static void
text_console_backend_set_visible(conf_object_t *obj, bool visible)
{
        screen_t *sc = screen_data(obj);
        console_set_visible(sc, visible);
}

// Implementation of text_console_backend interface.
static int
text_console_backend_line_length(conf_object_t *obj, int line_num)
{
        screen_t *sc = screen_data(obj);
        if (line_num >= 0) {
                // Visible screen line.
                ASSERT(line_num < VLEN(sc->line_info));
                return VGET(sc->line_info, line_num).length;
        } else {
                // Scrollback line.
                ASSERT(-line_num <= QLEN(sc->scrollback));
                return QGET(sc->scrollback,
                            line_num + QLEN(sc->scrollback)).info.length;
        }
}

// Implementation of text_console_backend interface.
static bool
text_console_backend_line_wrap(conf_object_t *obj, int line_num)
{
        screen_t *sc = screen_data(obj);
        if (line_num >= 0) {
                // Visible screen line.
                ASSERT(line_num < VLEN(sc->line_info));
                return VGET(sc->line_info, line_num).wrap;
        } else {
                // Scrollback line.
                ASSERT(-line_num <= QLEN(sc->scrollback));
                return QGET(sc->scrollback,
                            line_num + QLEN(sc->scrollback)).info.wrap;
        }
}

// Implementation of text_console_backend interface.
static void
text_console_backend_set_size(conf_object_t *obj, int width, int height)
{
        screen_t *sc = screen_data(obj);
        if (width > 0 && height > 0) {
                resize_screen_and_frontend(sc, width, height);
                if (SIM_object_is_configured(obj))
                        frontend_update_screen(sc);
        }
}

// Implementation of text_console_backend interface.
static void
text_console_backend_set_default_colours(conf_object_t *obj,
                                           uint32 fg, uint32 bg)
{
        screen_t *sc = screen_data(obj);
        sc->default_fg_col = fg;
        sc->default_bg_col = bg;
        if (SIM_object_is_configured(obj)) {
                ASSERT(sc->frontend);
                sc->frontend_iface->set_default_colours(
                        sc->frontend, sc->frontend_handle,
                        sc->default_fg_col, sc->default_bg_col);
                write_prefs_default_colors(sc->default_fg_col,
                                           sc->default_bg_col);
        }
}

static set_error_t
set_window_title(conf_object_t *obj, attr_value_t *val)
{
        const char *str = SIM_attr_string(*val);
        screen_t *sc = screen_data(obj);
        MM_FREE((char *)sc->window_title);
        sc->window_title = MM_STRDUP(str);
        
        if (SIM_object_is_configured(obj))
                frontend_set_title(sc, sc->window_title);
        return Sim_Set_Ok;
}

static attr_value_t
get_window_title(conf_object_t *obj)
{
        screen_t *sc = screen_data(obj);
        return SIM_make_attr_string(sc->window_title);
}

static set_error_t
set_screen_size(conf_object_t *obj, attr_value_t *val)
{
        int width = SIM_attr_integer(SIM_attr_list_item(*val, 0));
        int height = SIM_attr_integer(SIM_attr_list_item(*val, 1));
        if (width <= 0 || height <= 0)
                return Sim_Set_Illegal_Value;
        text_console_backend_set_size(obj, width, height);
        return Sim_Set_Ok;
}

static attr_value_t
get_screen_size(conf_object_t *obj)
{
        screen_t *sc = screen_data(obj);
        return SIM_make_attr_list(
                2,
                SIM_make_attr_uint64(sc->width),
                SIM_make_attr_uint64(sc->height));
}

static set_error_t
set_screen_data(conf_object_t *obj, attr_value_t *val)
{
        screen_t *sc = screen_data(obj);
        attr_value_t chars = SIM_attr_list_item(*val, 0);
        attr_value_t attrs = SIM_attr_list_item(*val, 1);
        attr_value_t info = SIM_attr_list_item(*val, 2);

        if (SIM_attr_list_size(chars) != sc->width * sc->height
            || SIM_attr_list_size(attrs) != sc->width * sc->height
            || SIM_attr_list_size(info) != sc->height)
                return Sim_Set_Illegal_Value;

        // Set screen data.
        VRESIZE(sc->screen, sc->width * sc->height);
        VFORI(sc->screen, i) {
                VSET(sc->screen, i,
                     SIM_attr_integer(SIM_attr_list_item(chars, i)));
        }        

        // Set screen attributes.
        VRESIZE(sc->attrib, sc->width * sc->height);
        VFORI(sc->attrib, i) {
                attr_value_t elt = SIM_attr_list_item(attrs, i);
                VSET(sc->attrib, i,
                     attrib_data(SIM_attr_integer(
                                         SIM_attr_list_item(elt, 0)),
                                 SIM_attr_integer(
                                         SIM_attr_list_item(elt, 1)),
                                 SIM_attr_integer(
                                         SIM_attr_list_item(elt, 2))));
        }

        // Set line info.
        VRESIZE(sc->line_info, sc->height);
        VFORI(sc->line_info, i) {
                attr_value_t elt = SIM_attr_list_item(info, i);
                line_info_t info = {
                        .length = SIM_attr_integer(SIM_attr_list_item(elt, 0)),
                        .wrap = SIM_attr_boolean(SIM_attr_list_item(elt, 1))};
                VSET(sc->line_info, i, info);
        }

        if (SIM_object_is_configured(obj))
                frontend_update_screen(sc);
        return Sim_Set_Ok;
}

static attr_value_t
get_screen_data(conf_object_t *obj)
{
        screen_t *sc = screen_data(obj);

        attr_value_t chars = SIM_alloc_attr_list(VLEN(sc->screen));
        attr_value_t attrs = SIM_alloc_attr_list(VLEN(sc->attrib));
        attr_value_t info = SIM_alloc_attr_list(VLEN(sc->line_info));

        // Return screen data.
        VFORI(sc->screen, i) {
                SIM_attr_list_set_item(
                        &chars, i, SIM_make_attr_uint64(VGET(sc->screen, i)));
        }

        // Return screen attributes.
        VFORI(sc->attrib, i) {
                text_console_attrib_t attrib = VGET(sc->attrib, i);
                SIM_attr_list_set_item(
                        &attrs, i,
                        SIM_make_attr_list(
                                3,
                                SIM_make_attr_uint64(attrib.attrib),
                                SIM_make_attr_uint64(attrib.fg),
                                SIM_make_attr_uint64(attrib.bg)));
        }

        // Return line info.
        VFORI(sc->line_info, i) {
                line_info_t elt = VGET(sc->line_info, i);
                SIM_attr_list_set_item(
                        &info, i,
                        SIM_make_attr_list(
                                2,
                                SIM_make_attr_uint64(elt.length),
                                SIM_make_attr_boolean(elt.wrap)));
        }

        return SIM_make_attr_list(3, chars, attrs, info);
}

static void
clear_scrollback(screen_t *sc)
{
        for (int i = 0; i < QLEN(sc->scrollback); i++) {
                sb_line_t sb = QGET(sc->scrollback, i);
                ASSERT(sb.line);
                ASSERT(sb.attrib);
                MM_FREE(sb.line);
                MM_FREE(sb.attrib);
        }
        QCLEAR(sc->scrollback);
}

static set_error_t
set_scrollback_data(conf_object_t *obj, attr_value_t *val)
{
        screen_t *sc = screen_data(obj);
        attr_value_t chars = SIM_attr_list_item(*val, 0);
        attr_value_t attrs = SIM_attr_list_item(*val, 1);
        attr_value_t cols = SIM_attr_list_item(*val, 2);
        attr_value_t info = SIM_attr_list_item(*val, 3);

        // We must have a well-defined number of scrollback lines.
        if (SIM_attr_data_size(chars) % sc->width != 0
            || SIM_attr_data_size(attrs) % sc->width != 0
            || (SIM_attr_data_size(chars) / sc->width
                != SIM_attr_data_size(attrs) / sc->width)
            || (SIM_attr_data_size(chars) / sc->width
                != SIM_attr_data_size(cols) / (4 * sc->width))
            || (SIM_attr_data_size(chars) / sc->width
                != SIM_attr_list_size(info)))
                return Sim_Set_Illegal_Value;

        int sb_lines = SIM_attr_list_size(info);
        clear_scrollback(sc);
        const chr_type_t *lines = (const chr_type_t *)SIM_attr_data(chars);
        const uint8 *attribs = (const uint8 *)SIM_attr_data(attrs);
        const uint16 *colours = (const uint16 *)SIM_attr_data(cols);
        text_console_attrib_t attrib[sc->width];
        
        for (int i = 0; i < sb_lines; i++) {
                attr_value_t elt = SIM_attr_list_item(info, i);
                line_info_t info = {
                        .length = SIM_attr_integer(SIM_attr_list_item(elt, 0)),
                        .wrap = SIM_attr_boolean(SIM_attr_list_item(elt, 1))};
                for (int j = 0; j < sc->width; j++) {
                        attrib[j].attrib = attribs[i * sc->width + j];
                        attrib[j].fg = colours[2 * (i * sc->width + j)];
                        attrib[j].bg = colours[2 * (i * sc->width + j) + 1];
                }
                add_scrollback_line(sc, lines + i * sc->width,
                                    &attrib[0], info);
        }

        if (SIM_object_is_configured(obj))
                frontend_update_screen(sc);
        return Sim_Set_Ok;
}

static attr_value_t
get_scrollback_data(conf_object_t *obj)
{
        screen_t *sc = screen_data(obj);

        int sb_len = sc->width * QLEN(sc->scrollback);
        chr_type_t *lines = MM_MALLOC(sb_len, chr_type_t);
        uint8 *attribs = MM_MALLOC(sb_len, uint8);
        uint16 *colours = MM_MALLOC(2 * sb_len, uint16);
        
        attr_value_t info = SIM_alloc_attr_list(QLEN(sc->scrollback));

        for (int i = 0; i < QLEN(sc->scrollback); i++) {
                sb_line_t data = QGET(sc->scrollback, i);
                memcpy(lines + i * sc->width, data.line,
                       sc->width * sizeof *lines);
                for (int j = 0; j < sc->width; j++) {
                        attribs[i * sc->width + j] = data.attrib[j].attrib;
                        colours[2 * (i * sc->width + j)] = data.attrib[j].fg;
                        colours[2 * (i * sc->width + j) + 1]
                                = data.attrib[j].bg;
                }
                SIM_attr_list_set_item(
                        &info, i,
                        SIM_make_attr_list(
                                2,
                                SIM_make_attr_uint64(data.info.length),
                                SIM_make_attr_boolean(data.info.wrap)));
        }

        return SIM_make_attr_list(
                4,
                SIM_make_attr_data_adopt(sb_len * sizeof *lines, lines),
                SIM_make_attr_data_adopt(sb_len * sizeof *attribs, attribs),
                SIM_make_attr_data_adopt(2 * sb_len * sizeof *colours, colours),
                info);
}

static set_error_t
set_max_scrollback_size(conf_object_t *obj, attr_value_t *val)
{
        screen_t *sc = screen_data(obj);
        int lines = SIM_attr_integer(*val);
        if (lines < 0)
                return Sim_Set_Illegal_Value;
        sc->max_sb_lines = lines;
        ensure_max_scrollback_size(sc);
        if (SIM_object_is_configured(obj))
                frontend_set_scrollback_size(sc, sc->max_sb_lines);
        return Sim_Set_Ok;
}

static attr_value_t
get_max_scrollback_size(conf_object_t *obj)
{
        screen_t *sc = screen_data(obj);
        return SIM_make_attr_uint64(sc->max_sb_lines);
}

static set_error_t
set_default_colours(conf_object_t *obj, attr_value_t *val)
{
        uint32 fg = SIM_attr_integer(SIM_attr_list_item(*val, 0));
        uint32 bg = SIM_attr_integer(SIM_attr_list_item(*val, 1));
        if (fg > 0xffffff || bg > 0xffffff) {
                SIM_attribute_error("Invalid RGB values.");
                return Sim_Set_Illegal_Value;
        } else {
                // Convert to internal xBGR format
                text_console_backend_set_default_colours(
                        obj, reverse_colour(fg), reverse_colour(bg));
                return Sim_Set_Ok;
        }
}

static attr_value_t
get_default_colours(conf_object_t *obj)
{
        screen_t *sc = screen_data(obj);
        return SIM_make_attr_list(
                2,
                SIM_make_attr_uint64(reverse_colour(sc->default_fg_col)),
                SIM_make_attr_uint64(reverse_colour(sc->default_bg_col)));
}

static void
init_frontend(screen_t *sc)
{
        sc->frontend_iface->set_default_colours(
                sc->frontend, sc->frontend_handle,
                sc->default_fg_col, sc->default_bg_col);
        frontend_set_title(sc, sc->window_title);
        frontend_set_size(sc, sc->width, sc->height);
        frontend_set_scrollback_size(sc, sc->max_sb_lines);
        // If visibility set explicitly, inform frontend.
        if (sc->visible != -1) {
                console_set_visible(sc, sc->visible);
                frontend_set_visible(sc, sc->visible);
        } else
                sc->visible = false;
}

static attr_value_t
get_frontend_handle(conf_object_t *obj)
{
        screen_t *sc = screen_data(obj);
        return SIM_make_attr_uint64((unsigned)sc->frontend_handle);
}

static set_error_t
set_frontend_handle(conf_object_t *obj, attr_value_t *val)
{
        screen_t *sc = screen_data(obj);
        sc->frontend_handle = SIM_attr_integer(*val);
        init_frontend(sc);
        return Sim_Set_Ok;
}

static attr_value_t
get_visible_attr(conf_object_t *obj)
{
        screen_t *sc = screen_data(obj);
        return SIM_make_attr_boolean(sc->visible == -1 ? false : sc->visible);
}

static set_error_t
set_visible_attr(conf_object_t *obj, attr_value_t *val)
{
        screen_t *sc = screen_data(obj);
        if (SIM_attr_is_boolean(*val)) {
                bool visible = SIM_attr_boolean(*val);
                // -no-win should override this, unless explicitly set
                if (!SIM_object_is_configured(obj)
                    || SIM_is_restoring_state(obj))
                        visible = visible && !VT_get_hide_consoles_flag();
                console_set_visible(sc, visible);
                frontend_set_visible(sc, visible);
        }
        return Sim_Set_Ok;
}

// Lookup default text colour from module preferences
static void
read_prefs_default_colours(uint32 *fg, uint32 *bg)
{
        conf_object_t *prefs = SIM_get_object("prefs");
        ASSERT(prefs);
        SIM_require_object(prefs);
        const preference_interface_t *iface =
                SIM_C_GET_INTERFACE(prefs, preference);
        ASSERT(iface);
        attr_value_t default_fg =
                iface->get_preference_for_module_key(
                        prefs, "text-console", "default-foreground");
        attr_value_t default_bg =
                iface->get_preference_for_module_key(
                        prefs, "text-console", "default-background");
        if (*fg == INVALID_COL) {
                if (SIM_attr_is_integer(default_fg))
                        *fg = SIM_attr_integer(default_fg);
                else
                        *fg = DEFAULT_FG_COL;
        }
        if (*bg == INVALID_COL) {
                if (SIM_attr_is_integer(default_bg))
                        *bg = SIM_attr_integer(default_bg);
                else
                        *bg = DEFAULT_BG_COL;
        }
}

screen_t *
make_screen(text_console_t *tc, const char *name)
{
        screen_t *sc = MM_ZALLOC(1, screen_t);
        sc->window_title = MM_STRDUP(name);

        sc->width = 80;
        sc->height = 24;
        sc->max_sb_lines = DEFAULT_MAX_SB_LINES;
        // Initially invalid values, set by attribute or prefs.
        sc->default_fg_col = INVALID_COL;
        sc->default_bg_col = INVALID_COL;
        VINIT(sc->screen);
        VRESIZE(sc->screen, screen_size(sc));
        clear_screen(&sc->screen);
        VINIT(sc->attrib);
        VRESIZE(sc->attrib, VLEN(sc->screen));
        clear_attrib(&sc->attrib);
        sc->frontend = NULL;
        sc->frontend_iface = NULL;
        VINIT(sc->line_info);
        VRESIZE(sc->line_info, sc->height);
        memset(VVEC(sc->line_info), 0,
               VLEN(sc->line_info) * VELEMSIZE(sc->line_info));
        // Unset => console will be shown automatically upon
        // startup if it is unique.
        sc->visible = -1;        
        sc->tc = tc;
        return sc;
}

static void
frontend_del_cb(lang_void *data, conf_object_t *frontend)
{
        screen_t *sc = data;
        sc->frontend = NULL;
        SIM_hap_delete_callback_obj_id("Core_Conf_Object_Delete", frontend,
                                       sc->frontend_del_hap);
}

void
finalise_screen(screen_t *sc)
{
        read_prefs_default_colours(&sc->default_fg_col, &sc->default_bg_col);
        write_prefs_default_colors(sc->default_fg_col, sc->default_bg_col);
        sc->frontend = get_text_console_frontend(sc_obj(sc));
        ASSERT(sc->frontend);
        sc->frontend_iface =
                SIM_C_GET_INTERFACE(sc->frontend, text_console_frontend);
        ASSERT(sc->frontend_iface);
        SIM_require_object(sc->frontend);
        sc->frontend_handle = sc->frontend_iface->start(
                sc->frontend, sc_obj(sc));
        init_frontend(sc);
        sc->frontend_del_hap = SIM_hap_add_callback_obj(
                "Core_Conf_Object_Delete", sc->frontend, 0,
                (obj_hap_func_t)frontend_del_cb, sc);
}

void
pre_delete_screen(screen_t *sc)
{
        ASSERT(sc->frontend_iface);
        sc->frontend_iface->stop(sc->frontend, sc->frontend_handle);
        SIM_delete_object(sc->frontend);
}

void
destroy_screen(screen_t *sc)
{
        VFREE(sc->screen);
        VFREE(sc->attrib);
        VFREE(sc->line_info);
        clear_scrollback(sc);
        QFREE(sc->scrollback);
        MM_FREE((char *)sc->window_title);
        MM_FREE(sc);
}

void
register_screen(conf_class_t *cl)
{
        static const text_console_backend_interface_t iface = {
                .input = text_console_backend_input,
                .request_refresh = text_console_backend_request_refresh,
                .set_visible = text_console_backend_set_visible,
                .line_length = text_console_backend_line_length,
                .line_wrap = text_console_backend_line_wrap,
                .set_size = text_console_backend_set_size,
                .set_default_colours =
                        text_console_backend_set_default_colours,
        };
        SIM_REGISTER_INTERFACE(cl, text_console_backend, &iface);

        SIM_register_attribute(
                cl, "window_title",
                get_window_title,
                set_window_title,
                Sim_Attr_Optional,
                "s",
                "Console GUI window title.");

        SIM_register_attribute(
                cl, "screen_size",
                get_screen_size,
                set_screen_size,
                Sim_Attr_Optional,
                "[ii]",
                "Console visible screen size (columns, rows).");
        
        SIM_register_attribute(
                cl, "max_scrollback_size",
                get_max_scrollback_size,
                set_max_scrollback_size,
                Sim_Attr_Optional,
                "i",
                "Maximum number of scrollback lines. Set to 0 for no maximum."
                " Default is " SYMBOL_TO_STRING(DEFAULT_MAX_SB_LINES) ".");

        SIM_register_attribute(
                cl, "screen_data",
                get_screen_data,
                set_screen_data,
                Sim_Attr_Optional,
                "[[i*][[iii]*][[ib]*]]",
                "Console screen contents (characters, attributes, line_info)."
                " Characters and attributes lists have length width * height."
                " Attributes list contains (attribute, fg colour, bg colour)"
                " for every screen character."
                " line_info is a list with meta data for every line:"
                " (line length, line wrap)."
                );

        SIM_register_attribute(
                cl, "scrollback_data",
                get_scrollback_data,
                set_scrollback_data,
                Sim_Attr_Optional,
                "[ddd[[ib]*]]",
                "Console scrollback contents"
                " (characters, attributes, colours, line_info)."
                " line_info has the same format as for the"
                " <attr>screen_data</attr> attribute."
                " The character data also has the same format but is given as"
                " binary for faster checkpointing. The attribute data is a list"
                " of text_console_attrib_t.attrib values for each scrollback"
                " character, and the colour data is a list of"
                " text_console_colour_t values, alternating foreground and"
                " background colours (hence the colour data is 4 times long"
                " as the character data).");

        
        SIM_register_attribute(
                cl, "default_colours",
                get_default_colours,
                set_default_colours,
                Sim_Attr_Optional,
                "[ii]",
                "Default text colours (foreground, background)"
                " in 0xRRGGBB format.");

        SIM_register_attribute(
                cl, "visible",
                get_visible_attr,
                set_visible_attr,
                Sim_Attr_Optional, "b|n",
                "Show/hide console GUI window. Setting to NIL only has effect"
                " before instantiation, in which case it leads to the default"
                " visibility behaviour of showing the console window if it is"
                " the unique console in the configuration.");

        SIM_register_attribute(
                cl, "frontend_handle",
                get_frontend_handle,
                set_frontend_handle,
                Sim_Attr_Pseudo | Sim_Attr_Internal, "i",
                "GUI frontend handle.");
        
        SIM_ensure_partial_attr_order(cl, "screen_size", "screen_data");
        SIM_ensure_partial_attr_order(cl, "screen_size", "scrollback_data");

        notify_open = SIM_notifier_type(SYMBOL_TO_STRING(NOTIFY_OPEN));
        notify_close = SIM_notifier_type(SYMBOL_TO_STRING(NOTIFY_CLOSE));
        SIM_register_notifier(
                cl, notify_open,
                "Notifier that is triggered when a text console is opened.");
        SIM_register_notifier(
                cl, notify_close,
                "Notifier that is triggered when a text console is closed.");
}
