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

#include "vt100.h"
#include <simics/base/conf-object.h>
#include <simics/base/log.h>
#include "screen.h"
#include "driver.h"
#include "telnet.h"
#include "text-console.h"
#include "text-inline.h"
#include "host-serial.h"

typedef enum {
        Parse_Ground,
        Parse_ESC,                           // ESC
        Parse_CSI,                           // ESC [
        Parse_CSI_Q,                         // ESC [ ?
        Parse_ESC_Hash,                      // ESC #
        Parse_ESC_Something,                 // ESC followed by one of %()*+-./
} parse_state_t;

/* Standard DEC terminals parse 16 parameters and ignore the rest. */
#define MAXPARAMS 16

/* The largest value that any parameter can take (the smallest is 0).
   Apparently this is what DEC terminals use. */
#define MAX_PARAM_VALUE 16383

// State of VT escape sequence parsing.
typedef struct {
        parse_state_t state;

        /* Number of parameters started so far (0..MAXPARAMS). */
        int nparams;

        /* Parameter array full; no more parameters are accepted. */
        bool overflow;

        /* Current parameters, of which the first nparams are valid. */
        int params[MAXPARAMS];
} vtparse_t;

// Cursor specific data, saved and restored together.
typedef struct {
        /* Cursor position, 0-based, absolute (independent of DECOM);
           always in the intervals 0..height-1 and 0..width-1 respectively. */
        int row;
        int col;

        /* If set, a line wrap is pending; the cursor will wrap before
           next visible character is displayed.
           (This is the Last Column Flag in DEC's internal documentation.)
           Only ever set when col = width-1. */
        bool pending_wrap;

        text_console_text_attrib_t attrib;
        bool reverse_video;
        text_console_colour_t fg_colour;
        text_console_colour_t bg_colour;

        bool rel_coord;                      /* Origin mode (DECOM). */
} cursor_data_t;

struct vt100_data {
        text_console_t *tc;             /* Sleazy shortcut to parent struct. */

        cursor_data_t cursor;                /* Current cursor data. */
        cursor_data_t saved_cursor;          /* Saved cursor data. */

        // Scrolling region (0-based, both inclusive).
        int scroll_top;
        int scroll_bottom;

        bool auto_wrap;                /* Auto-wrap mode (DECAWM). */

        bool cursor_key_appl_mode;     /* Application cursor keys (DECCKM). */
        bool keypad_appl_mode;         /* Application keypad mode (DECKPAM). */

        /* Tab stops: true marks a stop in that column. */
        VECT(bool) tabs;

        bool insert_mode;                    /* Insert mode (IRM). */

        /* If true, the backspace (or ⌫) key will send BS, otherwise DEL
           (DECBKM). */
        bool backspace_key_sends_bs;

        /* If true, use (a subset of) Microsoft's "VT100+" keyboard codes
           for input, otherwise normal VT220/XTerm. */
        bool microsoft_keyboard_mode;

        vtparse_t parser;
};

static vt100_data_t *
vt100_data(conf_object_t *obj)
{
        return from_obj(obj)->vt100_data;
}

static conf_object_t *
vt_obj(vt100_data_t *vt) { return to_obj(vt->tc); }

static screen_t *
vt_sc(vt100_data_t *vt) { return vt->tc->screen; }

static console_driver_data_t *
vt_cd(vt100_data_t *vt) { return vt->tc->driver_data; }

static telnet_data_t *
vt_td(vt100_data_t *vt) { return vt->tc->telnet_data; }

static int
screen_width(vt100_data_t *vt) { return get_screen_width(vt_sc(vt)); }

static int
screen_height(vt100_data_t *vt) { return get_screen_height(vt_sc(vt)); }

/* Reset the pending wrap state. */
static void
reset_wrap(vt100_data_t *vt)
{
        vt->cursor.pending_wrap = false;
}

/* Move the cursor down one line; scroll at margin.
   If at the bottom of the screen but below the margin, stay put.
   The frontend is not updated. */
static void
line_feed(vt100_data_t *vt)
{
        int height = screen_height(vt);
        ASSERT(vt->cursor.row < height);
        ASSERT(vt->scroll_bottom < height);
        if (vt->cursor.row == vt->scroll_bottom) {
                /* At bottom margin - scroll. */
                scroll_up_to_scrollback(vt_sc(vt),
                                        vt->scroll_top, vt->scroll_bottom,
                                        vt->cursor.bg_colour);
        } else if (vt->cursor.row < height - 1) {
                /* Not at bottom of the screen - go down. */
                vt->cursor.row++;
        }
        reset_wrap(vt);
}

/* Send the updated cursor position to the frontend. */
static void
frontend_update_cursor_pos(vt100_data_t *vt)
{
        frontend_set_cursor_pos(vt_sc(vt), vt->cursor.row, vt->cursor.col);
}

/* Display a printable character, advancing the cursor as appropriate. */
static void
insert_or_replace_char(vt100_data_t *vt, uint8 ch)
{
        if (ch < 32 || ch > 126)
                return;                      /* Only ASCII right now. */
        
        if (vt->cursor.pending_wrap
            && vt->cursor.col == screen_width(vt) - 1
            && vt->auto_wrap) {
                /* The last column flag was set: wrap the cursor first. */
                set_wrapping_line(vt_sc(vt), vt->cursor.row);
                vt->cursor.col = 0;
                line_feed(vt);
                frontend_update_cursor_pos(vt);
        }

        text_console_colour_t fg = vt->cursor.fg_colour;
        text_console_colour_t bg = vt->cursor.bg_colour;
        if (vt->cursor.reverse_video) {
                fg = vt->cursor.bg_colour;
                bg = vt->cursor.fg_colour;
        }

        if (vt->insert_mode)
                screen_insert_chars(vt_sc(vt),
                                    vt->cursor.row, vt->cursor.col,
                                    1, vt->cursor.bg_colour);
                
        set_screen_contents(vt_sc(vt), vt->cursor.row, vt->cursor.col,
                            ch, vt->cursor.attrib, fg, bg);

        if (vt->cursor.col == screen_width(vt) - 1) {
                if (vt->auto_wrap)
                        vt->cursor.pending_wrap = true;
        } else {
                vt->cursor.col++;
                frontend_update_cursor_pos(vt);
        }
}

static bool
has_remote_connection(vt100_data_t *vt)
{
        return telnet_connected(vt_td(vt)) || host_serial_connected(vt->tc);
}

// Send terminal identification string to the console associated input device.
static void
terminal_identification(vt100_data_t *vt)
{
        if (!has_remote_connection(vt)) {
                /* Identify as a VT102 (a tad conservative).
                   FIXME: Perhaps VT220 would be more accurate?
                   With attributes? */
                console_input_str(vt_cd(vt), "\33[?6c");
        }
}

// Send cursor position string to the console associated input device.
static void
cursor_position_report(vt100_data_t *vt)
{
        if (!has_remote_connection(vt)) {
                char report[64];
                int row = vt->cursor.row;
                if (vt->cursor.rel_coord)
                        row -= vt->scroll_top;
                sprintf(report, "\33[%d;%dR", row + 1, vt->cursor.col + 1);
                console_input_str(vt_cd(vt), report);
        }
}

// Set default text attributes and colour on the specified cursor data.
static void
reset_text_attributes(cursor_data_t *cursor)
{
        cursor->attrib = 0;
        cursor->reverse_video = false;
        cursor->fg_colour = Text_Console_Colour_Default_Foreground;
        cursor->bg_colour = Text_Console_Colour_Default_Background;
}

// Move current cursor position to (x, y), stopping at screen edges.
static void
set_cursor_pos(vt100_data_t *vt, int x, int y)
{
        vt->cursor.row = MIN(screen_height(vt) - 1, MAX(0, y));
        vt->cursor.col = MIN(screen_width(vt) - 1, MAX(0, x));
        frontend_update_cursor_pos(vt);
}

// Move current cursor position to (x, y), possibly relative to the current
// scrolling region, depending on the coordinate mode.
static void
set_cursor_pos_rel(vt100_data_t *vt, int x, int y)
{
        if (vt->cursor.rel_coord) {
                y += vt->scroll_top;
                if (y > vt->scroll_bottom)
                        y = vt->scroll_bottom;
        }
        set_cursor_pos(vt, x, y);
}

// Reset tab stops to default 8 characters.
static void
reset_tabs(vt100_data_t *vt)
{
        int width = screen_width(vt);
        VRESIZE(vt->tabs, width);
        VFORI(vt->tabs, i)
                VSET(vt->tabs, i, (i & 7) == 0);
        reset_wrap(vt);
}

// Set the whole console screen to space and default attributes.
static void
clear_screen(vt100_data_t *vt)
{
        screen_erase(vt_sc(vt), 0, 0,
                     screen_height(vt) - 1, screen_width(vt) - 1,
                     vt->cursor.bg_colour);
        reset_wrap(vt);
}

// Screen size was changed by the user.
void
screen_size_change(vt100_data_t *vt, int prev_rows, int new_rows)
{
        // Update to valid scroll region.
        if (vt->scroll_bottom == prev_rows - 1)
                vt->scroll_bottom = new_rows - 1;
        
        // Move cursor to valid position.
        // If not yet configured, then cursor position must already be valid.
        // (unless a checkpoint has been manually edited)
        if (SIM_object_is_configured(vt_obj(vt)))
            set_cursor_pos(vt, vt->cursor.col, vt->cursor.row);
}

// Reset all terminal properties to defaults.
static void
terminal_reset(vt100_data_t *vt)
{
        vt->cursor = (cursor_data_t){
                .row = 0,
                .col = 0,
                .pending_wrap = false,
        };
        reset_text_attributes(&vt->cursor);
        vt->saved_cursor = vt->cursor;
        vt->scroll_top = 0;
        vt->scroll_bottom = screen_height(vt) - 1;
        vt->cursor_key_appl_mode = false;
        vt->keypad_appl_mode = false;
        vt->cursor.rel_coord = false;
        vt->auto_wrap = true;
        vt->backspace_key_sends_bs = false;
        reset_tabs(vt);
}

/* BS (Backspace). */
static void
do_bs(vt100_data_t *vt)
{
        if (vt->cursor.col > 0) {
                vt->cursor.col--;
                reset_wrap(vt);
        }
                
        frontend_update_cursor_pos(vt);
}

/* CR (Carriage Return). */
static void
do_cr(vt100_data_t *vt)
{
        vt->cursor.col = 0;
        reset_wrap(vt);
        frontend_update_cursor_pos(vt);
}

/* HT (Horizontal Tab). */
static void
do_ht(vt100_data_t *vt)
{
        int width = screen_width(vt);
        int col = vt->cursor.col + 1;
        while (col < width && !VGET(vt->tabs, col))
                col++;
        vt->cursor.col = MIN(col, width - 1);

        /* A real VT220 resets the wrap state on TAB, and DEC STD-070 mandates
           that behaviour, but we don't, following VT510 (and xterm).
           Resetting wrap on TAB will cause characters to be lost if long lines
           containing TABs are printed in DECAWM mode. */

        frontend_update_cursor_pos(vt);
}

/* DECSC (Save Cursor) */
static void
do_decsc(vt100_data_t *vt)
{
        vt->saved_cursor = vt->cursor;
}

/* DECRC (Restore Cursor) */
static void
do_decrc(vt100_data_t *vt)
{
        vt->cursor = vt->saved_cursor;
        frontend_update_cursor_pos(vt);
}

/* DECKPAM (Keypad Application Mode) */
static void
do_deckpam(vt100_data_t *vt)
{
        vt->keypad_appl_mode = true;
}

/* DECKPNM (Keypad Normal Mode) */
static void
do_deckpnm(vt100_data_t *vt)
{
        vt->keypad_appl_mode = false;
}

/* IND (Index) */
static void
do_ind(vt100_data_t *vt)
{
        line_feed(vt);
        frontend_update_cursor_pos(vt);
}

/* LF (Linefeed). */
static void
do_nl(vt100_data_t *vt)
{
        /* Same as IND, in absence of automatic newline mode. */
        do_ind(vt);
}

/* NEL (Next Line) */
static void
do_nel(vt100_data_t *vt)
{
        line_feed(vt);
        vt->cursor.col = 0;
        frontend_update_cursor_pos(vt);
}

/* RI (Reverse Index) */
static void
do_ri(vt100_data_t *vt)
{
        if (vt->cursor.row == vt->scroll_top) {
                /* At top margin - scroll. */
                screen_scroll_down(vt_sc(vt),
                                   vt->scroll_top, vt->scroll_bottom,
                                   1, vt->cursor.bg_colour);
        } else if (vt->cursor.row > 0) {
                /* Not at top of the screen - go up. */
                vt->cursor.row--;
        }
        reset_wrap(vt);
        frontend_update_cursor_pos(vt);
}

/* HTS (Horizontal Tab Set) */
static void
do_hts(vt100_data_t *vt)
{
        VSET(vt->tabs, vt->cursor.col, true);
}

/* DECID (Return Terminal ID) */
static void
do_decid(vt100_data_t *vt)
{
        terminal_identification(vt);
}

/* RIS (Full Reset) */
static void
do_ris(vt100_data_t *vt)
{
        terminal_reset(vt);
        clear_screen(vt);
        frontend_update_cursor_pos(vt);
}

/* Return the n:th parameter (starting at 0), 
   using default_value if missing or 0. */
static int
param(vt100_data_t *vt, int n, int default_value)
{
        if (n < vt->parser.nparams && vt->parser.params[n])
                return vt->parser.params[n];
        else
                return default_value;
}

/* ICH (Insert Character) */
static void
do_ich(vt100_data_t *vt)
{
        screen_insert_chars(vt_sc(vt), vt->cursor.row, vt->cursor.col,
                            MIN(param(vt, 0, 1), 
                                screen_width(vt) - vt->cursor.col),
                            vt->cursor.bg_colour);
        reset_wrap(vt);
}

/* CUU (Cursor Up) */
static void
do_cuu(vt100_data_t *vt)
{
        int row_limit = vt->cursor.row < vt->scroll_top ? 0 : vt->scroll_top;
        vt->cursor.row = MAX(vt->cursor.row - param(vt, 0, 1), row_limit);
        reset_wrap(vt);
        frontend_update_cursor_pos(vt);
}

/* CUD (Cursor Down) */
static void
do_cud(vt100_data_t *vt)
{
        int row_limit = vt->cursor.row > vt->scroll_bottom
                        ? screen_height(vt) - 1 : vt->scroll_bottom;
        vt->cursor.row = MIN(vt->cursor.row + param(vt, 0, 1), row_limit);
        reset_wrap(vt);
        frontend_update_cursor_pos(vt);
}

/* CUF (Cursor Forward) */
static void
do_cuf(vt100_data_t *vt)
{
        set_cursor_pos(vt, vt->cursor.col + param(vt, 0, 1), vt->cursor.row);
        reset_wrap(vt);
}

/* CUB (Cursor Backward) */
static void
do_cub(vt100_data_t *vt)
{
        set_cursor_pos(vt, vt->cursor.col - param(vt, 0, 1), vt->cursor.row);
        reset_wrap(vt);
}

/* CUP (Cursor Position) */
static void
do_cup(vt100_data_t *vt)
{
        set_cursor_pos_rel(vt, param(vt, 1, 1) - 1, param(vt, 0, 1) - 1);
        reset_wrap(vt);
}

/* HPA (Horizontal Position Absolute) */
static void
do_hpa(vt100_data_t *vt)
{
        set_cursor_pos(vt, param(vt, 0, 1) - 1, vt->cursor.row);
        reset_wrap(vt);
}

/* VPA (Vertical Position Absolute) */
static void
do_vpa(vt100_data_t *vt)
{
        int row = param(vt, 0, 1) - 1;

        /* We assume that VPA is affected by DECOM just like CUP is. */
        if (vt->cursor.rel_coord) {
                row += vt->scroll_top;
                if (row > vt->scroll_bottom)
                        row = vt->scroll_bottom;
        }
        set_cursor_pos(vt, vt->cursor.col, row);
}

/* ED (Erase in Display) */
static void
do_ed(vt100_data_t *vt)
{
        switch (param(vt, 0, 0)) {
        case 0:
                screen_erase(vt_sc(vt),
                             vt->cursor.row, vt->cursor.col,
                             screen_height(vt) - 1, screen_width(vt) - 1,
                             vt->cursor.bg_colour);
                reset_wrap(vt);
                break;
        case 1:
                screen_erase(vt_sc(vt),
                             0, 0,
                             vt->cursor.row, vt->cursor.col,
                             vt->cursor.bg_colour);
                reset_wrap(vt);
                break;
        case 2:
                clear_screen(vt);
                reset_wrap(vt);
                break;
        }
}

/* EL (Erase in Line) */
static void
do_el(vt100_data_t *vt)
{
        switch (param(vt, 0, 0)) {
        case 0:
                screen_erase(vt_sc(vt),
                             vt->cursor.row, vt->cursor.col,
                             vt->cursor.row, screen_width(vt) - 1,
                             vt->cursor.bg_colour);
                reset_wrap(vt);
                break;
        case 1:
                screen_erase(vt_sc(vt),
                             vt->cursor.row, 0,
                             vt->cursor.row, vt->cursor.col,
                             vt->cursor.bg_colour);
                reset_wrap(vt);
                break;
        case 2:
                screen_erase(vt_sc(vt),
                             vt->cursor.row, 0,
                             vt->cursor.row, screen_width(vt) - 1,
                             vt->cursor.bg_colour);
                reset_wrap(vt);
                break;
        }
}

/* IL (Insert Line) */
static void
do_il(vt100_data_t *vt)
{
        if (vt->cursor.row >= vt->scroll_top
            && vt->cursor.row <= vt->scroll_bottom) {
                /* Inside the region. */
                screen_scroll_down(vt_sc(vt),
                                   vt->cursor.row, vt->scroll_bottom,
                                   param(vt, 0, 1), vt->cursor.bg_colour);
        }
        vt->cursor.col = 0;
        reset_wrap(vt);
        frontend_update_cursor_pos(vt);
}
        

/* DL (Delete Line) */
static void
do_dl(vt100_data_t *vt)
{
        if (vt->cursor.row >= vt->scroll_top
            && vt->cursor.row <= vt->scroll_bottom) {
                /* Inside the region. */
                screen_scroll_up(vt_sc(vt), vt->cursor.row, vt->scroll_bottom,
                                 param(vt, 0, 1), vt->cursor.bg_colour);
        }
        vt->cursor.col = 0;
        reset_wrap(vt);
        frontend_update_cursor_pos(vt);
}
        
/* DCH (Delete Character) */
static void
do_dch(vt100_data_t *vt)
{
        screen_delete_chars(vt_sc(vt), vt->cursor.row, vt->cursor.col,
                            MIN(param(vt, 0, 1), 
                                screen_width(vt) - vt->cursor.col),
                            vt->cursor.bg_colour);
        reset_wrap(vt);
}

/* ECH (Erase Character) */
static void
do_ech(vt100_data_t *vt)
{
        screen_erase(vt_sc(vt),
                     vt->cursor.row, vt->cursor.col,
                     vt->cursor.row, MIN(vt->cursor.col + param(vt, 0, 1),
                                         screen_width(vt)) - 1,
                     vt->cursor.bg_colour);
        reset_wrap(vt);
}

/* Primary DA (Send Device Attributes) */
static void
do_primary_da(vt100_data_t *vt)
{
        switch (param(vt, 0, 0)) {
        case 0:
                terminal_identification(vt);
                break;
        }
}

/* TBC (Tab Clear) */
static void
do_tbc(vt100_data_t *vt)
{
        switch (param(vt, 0, 0)) {
        case 0:
                VSET(vt->tabs, vt->cursor.col, false);
                break;
        case 3:
                VFORI(vt->tabs, i)
                        VSET(vt->tabs, i, false);
                break;
        }        
}

/* SM (Set Mode) */
static void
do_sm(vt100_data_t *vt)
{
        for (int i = 0; i < vt->parser.nparams; i++)
                switch (vt->parser.params[i]) {
                case 4:                      /* IRM (Insert Mode) */
                        vt->insert_mode = true;
                        break;
                }
}

/* RM (Reset Mode) */
static void
do_rm(vt100_data_t *vt)
{
        for (int i = 0; i < vt->parser.nparams; i++)
                switch (vt->parser.params[i]) {
                case 4:                      /* IRM (Replace Mode) */
                        vt->insert_mode = false;
                        break;
                }
}

/* SGR (Set Graphic Rendition) */
static void
do_sgr(vt100_data_t *vt)
{
        for (int i = 0; i < vt->parser.nparams; i++) {
                int p = vt->parser.params[i];
                switch (p) {
                case 0:
                        reset_text_attributes(&vt->cursor);
                        break;
                case 1:
                        vt->cursor.attrib |= Text_Console_Attrib_Bold;
                        break;
                case 4:
                        vt->cursor.attrib |= Text_Console_Attrib_Underline;
                        break;
                case 7:
                        vt->cursor.reverse_video = true;
                        break;
                case 22:
                        vt->cursor.attrib &= ~Text_Console_Attrib_Bold;
                        break;
                case 24:
                        vt->cursor.attrib &= ~Text_Console_Attrib_Underline;
                        break;
                case 27:
                        vt->cursor.reverse_video = false;
                        break;
                case 38:
                        if (i + 2 < vt->parser.nparams
                            && vt->parser.params[i + 1] == 5)
                                vt->cursor.fg_colour
                                        = vt->parser.params[i + 2];
                        i += 2;
                        break;
                case 39:
                        vt->cursor.fg_colour =
                                Text_Console_Colour_Default_Foreground;
                        break;
                case 48:
                        if (i + 2 < vt->parser.nparams
                            && vt->parser.params[i + 1] == 5)
                                vt->cursor.bg_colour
                                        = vt->parser.params[i + 2];
                        i += 2;
                        break;
                case 49:
                        vt->cursor.bg_colour = 
                                Text_Console_Colour_Default_Background;
                        break;
                default:
                        if (p >= 30 && p <= 37)
                                vt->cursor.fg_colour = p - 30;
                        else if (p >= 40 && p <= 47)
                                vt->cursor.bg_colour = p - 40;
                        break;
                }
        }
}

/* DSR (Device Status Report) */
static void
do_dsr(vt100_data_t *vt)
{
        switch (param(vt, 0, 0)) {
        case 6:                              /* CPR (Report Cursor Position) */
                cursor_position_report(vt);
                break;
        }
}

/* DECSTBM (Set Scrolling Region) */
static void
do_decstbm(vt100_data_t *vt)
{
        int top = param(vt, 0, 1) - 1;
        int bot = param(vt, 1, screen_height(vt)) - 1;
        
        if (top < bot) {
                vt->scroll_top =
                        MIN(screen_height(vt) - 1, MAX(0, top));
                vt->scroll_bottom =
                        MIN(screen_height(vt) - 1, MAX(top, bot));
                set_cursor_pos_rel(vt, 0, 0);
        }
        reset_wrap(vt);
}

/* DECSET (DEC Private Mode Set) */
static void
do_decset(vt100_data_t *vt)
{
        for (int i = 0; i < vt->parser.nparams; i++)
                switch (vt->parser.params[i]) {
                case 1:               /* DECCKM (Application Cursor Keys) */
                        vt->cursor_key_appl_mode = true;
                        break;
                case 3:               /* DECCOLM (132 Column Mode) */
                        set_screen_width(vt_sc(vt), 132);
                        set_cursor_pos_rel(vt, 0, 0);
                        clear_screen(vt);
                        reset_tabs(vt);
                        reset_wrap(vt);
                        break;
                case 6:               /* DECOM (Origin Mode) */
                        vt->cursor.rel_coord = true;
                        set_cursor_pos_rel(vt, 0, 0);
                        reset_wrap(vt);
                        break;
                case 7:               /* DECAWM (Wrap-around Mode) */
                        vt->auto_wrap = true;
                        break;
                case 67:              /* DECBKM (Backarrow Key Mode) */
                        vt->backspace_key_sends_bs = true;
                        break;
                }
}

/* DECRST (DEC Private Mode Reset) */
static void
do_decrst(vt100_data_t *vt)
{
        for (int i = 0; i < vt->parser.nparams; i++)
                switch (vt->parser.params[i]) {
                case 1:               /* DECCKM (Normal Cursor Keys) */
                        vt->cursor_key_appl_mode = false;
                        break;
                case 3:               /* DECCOLM (80 Column Mode) */
                        set_screen_width(vt_sc(vt), 80);
                        set_cursor_pos_rel(vt, 0, 0);
                        clear_screen(vt);
                        reset_tabs(vt);
                        reset_wrap(vt);
                        break;
                case 6:               /* DECOM (Normal Cursor Mode) */
                        vt->cursor.rel_coord = false;
                        set_cursor_pos_rel(vt, 0, 0);
                        reset_wrap(vt);
                        break;
                case 7:               /* DECAWM (No Wrap-around Mode) */
                        vt->auto_wrap = false;
                        reset_wrap(vt);
                        break;
                case 67:              /* DECBKM (Backarrow Key Mode) */
                        vt->backspace_key_sends_bs = false;
                        break;
                }
}

/* DECALN (DEC Screen Alignment Test */
static void
do_decaln(vt100_data_t *vt)
{
        /* STD-070 does not say how DECALN affects or is affected by
           colours, only that the other attributes are turned off first.
           We follow XTerm, in that we keep the set colours, but use the
           default colours for the test pattern. */
        vt->cursor.attrib = 0;
        vt->cursor.reverse_video = false;
        for (int row = 0; row < screen_height(vt); row++)
                for (int col = 0; col < screen_width(vt); col++)
                        set_screen_contents(
                                vt_sc(vt), row, col, 'E',
                                vt->cursor.attrib,
                                Text_Console_Colour_Default_Foreground,
                                Text_Console_Colour_Default_Background);
        reset_wrap(vt);
}

/* Handle received characters in the range 0..31. */
static void
control_char(vt100_data_t *vt, uint8 ch)
{
        switch (ch) {
        case '\b':
                do_bs(vt);
                break;
        case '\r':
                do_cr(vt);
                break;
        case '\n':
        case '\v':
        case '\f':
                do_nl(vt);
                break;
        case '\t':
                do_ht(vt);
                break;
        case 27:
                vt->parser.state = Parse_ESC;
                break;
        }
}

/* Start parsing of a new parameter. */
static void
next_param(vt100_data_t *vt)
{
        if (vt->parser.nparams < MAXPARAMS) {
                vt->parser.params[vt->parser.nparams] = 0;
                vt->parser.nparams++;
        } else
                vt->parser.overflow = true;
}

/* Add a digit to the last parameter. */
static void
param_digit(vt100_data_t *vt, int digit)
{
        if (!vt->parser.overflow) {
                int p = vt->parser.nparams - 1;
                vt->parser.params[p] = MIN(vt->parser.params[p] * 10 + digit,
                                           MAX_PARAM_VALUE);
        }
}

// Update the VT parse state using the incoming character ch, and take
// appropriate screen actions.
void
vt100_output(vt100_data_t *vt, uint8 ch)
{
        if (ch < 32) {
                /* Control characters are acted upon in any parse state. */
                control_char(vt, ch);
                return;
        }

        switch (vt->parser.state) {
        case Parse_Ground:
                insert_or_replace_char(vt, ch);
                break;

        case Parse_ESC:
                switch (ch) {
                case '[':
                        vt->parser.state = Parse_CSI;
                        vt->parser.nparams = 1;
                        vt->parser.params[0] = 0;
                        vt->parser.overflow = false;
                        return;
                case '#':
                        vt->parser.state = Parse_ESC_Hash;
                        return;
                case '%':
                case '(':
                case ')':
                case '*':
                case '+':
                case '-':
                case '.':
                case '/':
                        vt->parser.state = Parse_ESC_Something;
                        return;

                case '7':                    /* Save Cursor */
                        do_decsc(vt);
                        break;
                case '8':                    /* Restore Cursor */
                        do_decrc(vt);
                        break;
                case '=':                    /* Application Keypad */
                        do_deckpam(vt);
                        break;
                case '>':                    /* Normal Keypad */
                        do_deckpnm(vt);
                        break;
                case 'D':                    /* Index */
                        do_ind(vt);
                        break;
                case 'E':                    /* Next Line */
                        do_nel(vt);
                        break;
                case 'M':                    /* Reverse Index */
                        do_ri(vt);
                        break;
                case 'H':                    /* Tab Set */
                        do_hts(vt);
                        break;
                case 'Z':                    /* Return Terminal ID */
                        do_decid(vt);
                        break;
                case 'c':                    /* Full Reset */
                        do_ris(vt);
                        break;
                }
                break;

        case Parse_CSI:
                switch (ch) {
                case '?':
                        vt->parser.state = Parse_CSI_Q;
                        return;
                case ';':
                        next_param(vt);
                        return;

                case '@':                    /* Insert Character */
                        do_ich(vt);
                        break;
                case 'A':                    /* Cursor Up */
                        do_cuu(vt);
                        break;
                case 'B':                    /* Cursor Down */
                        do_cud(vt);
                        break;
                case 'C':                    /* Cursor Forward */
                        do_cuf(vt);
                        break;
                case 'D':                    /* Cursor Backward */
                        do_cub(vt);
                        break;
                case 'G':                    /* Cursor Horizontal Absolute */
                        do_hpa(vt);          /* Same as HPA */
                        break;
                case 'H':                    /* Cursor Position */
                        do_cup(vt);
                        break;
                case 'J':                    /* Erase in Display */
                        do_ed(vt);
                        break;
                case 'K':                    /* Erase in Line */
                        do_el(vt);
                        break;
                case 'L':                    /* Insert Line */
                        do_il(vt);
                        break;
                case 'M':                    /* Delete Line */
                        do_dl(vt);
                        break;
                case 'P':                    /* Delete Character */
                        do_dch(vt);
                        break;
                case 'X':                    /* Erase Character */
                        do_ech(vt);
                        break;
                case '`':                    /* Horizontal Position Absolute */
                        do_hpa(vt);
                        break;
                case 'c':                    /* Send Device Attributes */
                        do_primary_da(vt);
                        break;
                case 'd':                    /* Vertical Position Absolute */
                        do_vpa(vt);
                        break;
                case 'f':                    /* Horizontal and Vertical Pos */
                        do_cup(vt);          /* Same as CUP */
                        break;
                case 'g':                    /* Tab Clear */
                        do_tbc(vt);
                        break;
                case 'h':                    /* Set Mode */
                        do_sm(vt);
                        break;
                case 'l':                    /* Reset Mode */
                        do_rm(vt);
                        break;
                case 'm':                    /* Set Graphic Rendition */
                        do_sgr(vt);
                        break;
                case 'n':                    /* Device Status Report */
                        do_dsr(vt);
                        break;
                case 'r':                    /* Set Scrolling Region */
                        do_decstbm(vt);
                        break;
                        
                default:
                        if (ch >= '0' && ch <= '9') {
                                param_digit(vt, ch - '0');
                                return;
                        }
                        break;
                }
                break;

        case Parse_CSI_Q:
                switch (ch) {
                case ';':
                        next_param(vt);
                        return;

                case 'h':                    /* DEC Private Mode Set */
                        do_decset(vt);
                        break;
                case 'l':                    /* DEC Private Mode Reset */
                        do_decrst(vt);
                        break;

                default:
                        if (ch >= '0' && ch <= '9') {
                                param_digit(vt, ch - '0');
                                return;
                        }
                        break;
                }
                break;

        case Parse_ESC_Hash:
                switch (ch) {
                case '8':                    /* DEC Screen Alignment Test */
                        do_decaln(vt);
                        break;
                }
                break;

        case Parse_ESC_Something:            /* Designate Character Set */
                /* All ignored right now. */
                break;

        }

        vt->parser.state = Parse_Ground;
}

/* The Microsoft "VT100+" escape sequence for a key, or NULL if there
   is no special translation for that key. */
static const char *
ms_key_sequence(text_console_key_t key)
{
        switch (key) {
                /* Microsoft's docs don't say anything about arrow keys so we
                   assume that it's all right to use VT100 sequences for
                   them. */
        case Text_Console_Key_Up:    return "\033[A";
        case Text_Console_Key_Down:  return "\033[B";
        case Text_Console_Key_Left:  return "\033[D";
        case Text_Console_Key_Right: return "\033[C";

                /* The following sequences are from Microsoft's
                   documentation. */
        case Text_Console_Key_Home:  return "\033h";
        case Text_Console_Key_End:   return "\033k";
        case Text_Console_Key_Ins:   return "\033+";
        case Text_Console_Key_Del:   return "\033-";
        case Text_Console_Key_Pgup:  return "\033?";
        case Text_Console_Key_Pgdn:  return "\033/";

        case Text_Console_Key_F1:    return "\0331";
        case Text_Console_Key_F2:    return "\0332";
        case Text_Console_Key_F3:    return "\0333";
        case Text_Console_Key_F4:    return "\0334";
        case Text_Console_Key_F5:    return "\0335";
        case Text_Console_Key_F6:    return "\0336";
        case Text_Console_Key_F7:    return "\0337";
        case Text_Console_Key_F8:    return "\0338";
        case Text_Console_Key_F9:    return "\0339";
        case Text_Console_Key_F10:   return "\0330";
        case Text_Console_Key_F11:   return "\033!";
        case Text_Console_Key_F12:   return "\033@";
        default:
                return NULL;
        }
}

/* For keys that should be translated according to Microsoft's "VT100+" rules,
   send the corresponding sequence to the device and return true.
   For other keys, return false. */
static bool
handle_microsoft_keys(vt100_data_t *vt,
                      text_console_key_t key, text_console_modifier_t modifiers)
{
        const char *s = ms_key_sequence(key);
        if (!s)
                return false;
        if (modifiers & Text_Console_Modifier_Shift)
                console_input_str(vt_cd(vt), "\033\023");
        if (modifiers & Text_Console_Modifier_Ctrl)
                console_input_str(vt_cd(vt), "\033\003");
        if (modifiers & Text_Console_Modifier_Alt)
                console_input_str(vt_cd(vt), "\033\001");
        console_input_str(vt_cd(vt), s);
        return true;
}

/* The sequence to emit for a function key, or NULL if not a function key. */
static const char *
vt_function_key_seq(vt100_data_t *vt,
                    text_console_key_t key, text_console_modifier_t modifiers)
{
        /* FIXME: This code does not take modifiers into account; it should.
           They should affect the base sequence in an XTerm-like way
           (cross-check with the Linux console). */
           
        bool ca = vt->cursor_key_appl_mode;
        bool ka = vt->keypad_appl_mode;
        switch (key) {
        case Text_Console_Key_Left:     return ca ? "\033OD" : "\033[D";
        case Text_Console_Key_Up:       return ca ? "\033OA" : "\033[A";
        case Text_Console_Key_Right:    return ca ? "\033OC" : "\033[C";
        case Text_Console_Key_Down:     return ca ? "\033OB" : "\033[B";

        case Text_Console_Key_KP_0:     return ka ? "\033Op" : "0";     
        case Text_Console_Key_KP_1:     return ka ? "\033Oq" : "1";     
        case Text_Console_Key_KP_2:     return ka ? "\033Or" : "2";     
        case Text_Console_Key_KP_3:     return ka ? "\033Os" : "3";     
        case Text_Console_Key_KP_4:     return ka ? "\033Ot" : "4";     
        case Text_Console_Key_KP_5:     return ka ? "\033Ou" : "5";     
        case Text_Console_Key_KP_6:     return ka ? "\033Ov" : "6";     
        case Text_Console_Key_KP_7:     return ka ? "\033Ow" : "7";     
        case Text_Console_Key_KP_8:     return ka ? "\033Ox" : "8";     
        case Text_Console_Key_KP_9:     return ka ? "\033Oy" : "9";     
        case Text_Console_Key_KP_Plus : return ka ? "\033Ol" : "+";     
        case Text_Console_Key_KP_Minus: return ka ? "\033Om" : "-";     
        case Text_Console_Key_KP_Mul:   return ka ? "\033Oj" : "*";     
        case Text_Console_Key_KP_Div:   return ka ? "\033Oo" : "/";     
        case Text_Console_Key_KP_Dot:   return ka ? "\033On" : ".";     
        case Text_Console_Key_KP_Enter: return ka ? "\033OM" : "\r";    

        case Text_Console_Key_Home:     return "\033[1~";
        case Text_Console_Key_Ins:      return "\033[2~";
        case Text_Console_Key_Del:      return "\033[3~";
        case Text_Console_Key_End:      return "\033[4~";
        case Text_Console_Key_Pgup:     return "\033[5~";
        case Text_Console_Key_Pgdn:     return "\033[6~";

        case Text_Console_Key_F1:       return "\033OP";
        case Text_Console_Key_F2:       return "\033OQ";
        case Text_Console_Key_F3:       return "\033OR";
        case Text_Console_Key_F4:       return "\033OS";
        case Text_Console_Key_F5:       return "\033[15~";
        case Text_Console_Key_F6:       return "\033[17~";
        case Text_Console_Key_F7:       return "\033[18~";
        case Text_Console_Key_F8:       return "\033[19~";
        case Text_Console_Key_F9:       return "\033[20~";
        case Text_Console_Key_F10:      return "\033[21~";
        case Text_Console_Key_F11:      return "\033[23~";
        case Text_Console_Key_F12:      return "\033[24~";
                
        default:
                return NULL;
        }
}

/* Emit the sequence for a VT function key and return true;
   return false for other keys. */
static bool
handle_vt_function_key(vt100_data_t *vt,
                       text_console_key_t key,
                       text_console_modifier_t modifiers)
{
        const char *seq = vt_function_key_seq(vt, key, modifiers);
        if (seq) {
                console_input_str(vt_cd(vt), seq);
                return true;
        } else
                return false;
}

/* Convert a key (with modifiers) to a byte sequence which is sent back to the
   device. */
void
vt100_input(vt100_data_t *vt,
            text_console_key_t key, text_console_modifier_t modifiers)
{ 
        if (vt->microsoft_keyboard_mode
            && handle_microsoft_keys(vt, key, modifiers))
                return;
                
        if (handle_vt_function_key(vt, key, modifiers))
                return;

        if (key == Text_Console_Key_Backspace)
                key = vt->backspace_key_sends_bs ? 8 : 127;

        if (key >= 0 && key <= 127) {
                if (modifiers & Text_Console_Modifier_Alt)
                        console_input(vt_cd(vt), 27);  // ESC
                if (modifiers & Text_Console_Modifier_Ctrl) {
                        // Convert to control character.
                        if (key >= 64 && key <= 126)
                                key &= 0x1f;
                        else if (key == 63)
                                key = 127;  /* ^? = DEL   */
                        else if (key == 32)
                                key = 0;    /* ^SPC = NUL */
                }
                
                console_input(vt_cd(vt), key);
        } else
                SIM_LOG_INFO(1, vt_obj(vt), Text_Console_Log_VT100,
                             "Ignoring non-ASCII input char"
                             " 0x%x mod %d",
                             key, modifiers);
}

void
get_vt100_reset_string(vt100_data_t *vt, strbuf_t *buf)
{
        // Full reset
        sb_addstr(buf, "\x1b" "c");
}

void
get_vt100_state_string(vt100_data_t *vt, strbuf_t *buf)
{
        // Set scrolling region (DECSTBM)
        sb_addfmt(buf, "\x1b[%d;%dr", vt->scroll_top, vt->scroll_bottom);

        // DECCOLM
        if (screen_width(vt) == 132)
                sb_addstr(buf, "\x1b[?3h");
        else
                sb_addstr(buf, "\x1b[?3l");

        // DECOM
        if (vt->cursor.rel_coord)
                sb_addstr(buf, "\x1b[?6h");
        else
                sb_addstr(buf, "\x1b[?6l");

        // DECAWM
        if (vt->auto_wrap)
                sb_addstr(buf, "\x1b[?7h");
        else
                sb_addstr(buf, "\x1b[?7l");

        // DECBKM
        if (vt->backspace_key_sends_bs)
                sb_addstr(buf, "\x1b[?67h");
        else
                sb_addstr(buf, "\x1b[?67l");

        // DECCKM
        if (vt->cursor_key_appl_mode)
                sb_addstr(buf, "\x1b[?1h");
        else
                sb_addstr(buf, "\x1b[?1l");

        // Set saved cursor

        // Cursor position (CUP)
        sb_addfmt(buf, "\x1b[%d;%dH",
                  vt->saved_cursor.row - vt->scroll_top + 1,
                  vt->saved_cursor.col + 1);
        // Save cursor (DECSC)
        sb_addstr(buf, "\x1b" "7");

        // Cursor position (CUP)
        sb_addfmt(buf, "\x1b[%d;%dH",
                  vt->cursor.row - vt->scroll_top + 1,
                  vt->cursor.col + 1);

        // Default text colour (SGR)
        sb_addstr(buf, "\x1b[39;49m");

        // Keypad mode
        if (vt->keypad_appl_mode)
                sb_addstr(buf, "\x1b=");
        else
                sb_addstr(buf, "\x1b>");
}

void
get_cursor_pos(vt100_data_t *vt, int *x, int *y)
{
        *x = vt->cursor.col;
        *y = vt->cursor.row;
}

static set_error_t
attr_set_cursor_data(vt100_data_t *vt, cursor_data_t *cursor,
                     int x, int y, bool pending_wrap, bool rel_coord)
{
        if (x >= 0 && x < screen_width(vt)
            && y >= 0 && y < screen_height(vt)
            && (!pending_wrap || x == screen_width(vt) - 1)) {
                cursor->col = x;
                cursor->row = y;
                cursor->pending_wrap = pending_wrap;
                cursor->rel_coord = rel_coord;

                if (SIM_object_is_configured(vt_obj(vt)))
                        frontend_update_cursor_pos(vt);
                return Sim_Set_Ok;
        } else {
                SIM_attribute_error("bad cursor position");
                return Sim_Set_Illegal_Value;
        }
}

static attr_value_t
attr_get_cursor_data(const cursor_data_t *cursor)
{
        return SIM_make_attr_list(
                4,
                SIM_make_attr_uint64(cursor->col),
                SIM_make_attr_uint64(cursor->row),
                SIM_make_attr_boolean(cursor->pending_wrap),
                SIM_make_attr_boolean(cursor->rel_coord));
}
        
static set_error_t
set_cursor_data(conf_object_t *obj, attr_value_t *val)
{
        vt100_data_t *vt = vt100_data(obj);
        return attr_set_cursor_data(
                vt, &vt->cursor,
                SIM_attr_integer(SIM_attr_list_item(*val, 0)),
                SIM_attr_integer(SIM_attr_list_item(*val, 1)),
                SIM_attr_boolean(SIM_attr_list_item(*val, 2)),
                SIM_attr_boolean(SIM_attr_list_item(*val, 3)));
}

static attr_value_t
get_cursor_data(conf_object_t *obj)
{
        vt100_data_t *vt = vt100_data(obj);
        return attr_get_cursor_data(&vt->cursor);
}

static set_error_t
set_saved_cursor_data(conf_object_t *obj, attr_value_t *val)
{
        vt100_data_t *vt = vt100_data(obj);
        return attr_set_cursor_data(
                vt, &vt->saved_cursor,
                SIM_attr_integer(SIM_attr_list_item(*val, 0)),
                SIM_attr_integer(SIM_attr_list_item(*val, 1)),
                SIM_attr_boolean(SIM_attr_list_item(*val, 2)),
                SIM_attr_boolean(SIM_attr_list_item(*val, 3)));
}

static attr_value_t
get_saved_cursor_data(conf_object_t *obj)
{
        vt100_data_t *vt = vt100_data(obj);
        return attr_get_cursor_data(&vt->saved_cursor);
}

static attr_value_t
get_decawm(conf_object_t *obj)
{
        vt100_data_t *vt = vt100_data(obj);
        return SIM_make_attr_boolean(vt->auto_wrap);
}

static set_error_t
set_decawm(conf_object_t *obj, attr_value_t *val)
{
        vt100_data_t *vt = vt100_data(obj);
        vt->auto_wrap = SIM_attr_boolean(*val);
        return Sim_Set_Ok;
}

static set_error_t
set_scroll_region(conf_object_t *obj, attr_value_t *val)
{
        vt100_data_t *vt = vt100_data(obj);
        int top = SIM_attr_integer(SIM_attr_list_item(*val, 0));
        int bottom = SIM_attr_integer(SIM_attr_list_item(*val, 1));
        if (top >= 0 && top < bottom && bottom < screen_height(vt)) {
                vt->scroll_top = top;
                vt->scroll_bottom = bottom;
                return Sim_Set_Ok;
        } else {
                return Sim_Set_Illegal_Value;
        }
}

static attr_value_t
get_scroll_region(conf_object_t *obj)
{
        vt100_data_t *vt = vt100_data(obj);
        return SIM_make_attr_list(
                2,
                SIM_make_attr_uint64(vt->scroll_top),
                SIM_make_attr_uint64(vt->scroll_bottom));
}

static set_error_t
set_text_attrib(conf_object_t *obj, attr_value_t *val)
{
        vt100_data_t *vt = vt100_data(obj);
        int fg = SIM_attr_integer(SIM_attr_list_item(*val, 1));
        int bg = SIM_attr_integer(SIM_attr_list_item(*val, 2));
        if (fg >= 0 && fg <= Text_Console_Colour_Default_Foreground
            && bg >= 0 && bg <= Text_Console_Colour_Default_Background
            && bg != Text_Console_Colour_Default_Foreground) {
                vt->cursor.fg_colour = fg;
                vt->cursor.bg_colour = bg;
                vt->cursor.attrib =
                        SIM_attr_integer(SIM_attr_list_item(*val, 0));
                vt->cursor.reverse_video =
                        SIM_attr_boolean(SIM_attr_list_item(*val, 3));
                return Sim_Set_Ok;
        } else
                return Sim_Set_Illegal_Value;
}

static attr_value_t
get_text_attrib(conf_object_t *obj)
{
        vt100_data_t *vt = vt100_data(obj);
        return SIM_make_attr_list(
                4,
                SIM_make_attr_uint64(vt->cursor.attrib),
                SIM_make_attr_uint64(vt->cursor.fg_colour),
                SIM_make_attr_uint64(vt->cursor.bg_colour),
                SIM_make_attr_boolean(vt->cursor.reverse_video));
}

static set_error_t
set_modes(conf_object_t *obj, attr_value_t *val)
{
        vt100_data_t *vt = vt100_data(obj);
        vt->cursor_key_appl_mode =
                SIM_attr_boolean(SIM_attr_list_item(*val, 0));
        vt->keypad_appl_mode = SIM_attr_boolean(SIM_attr_list_item(*val, 1));
        vt->insert_mode = SIM_attr_boolean(SIM_attr_list_item(*val, 2));
        return Sim_Set_Ok;
}

static attr_value_t
get_modes(conf_object_t *obj)
{
        vt100_data_t *vt = vt100_data(obj);
        return SIM_make_attr_list(
                3,
                SIM_make_attr_boolean(vt->cursor_key_appl_mode),
                SIM_make_attr_boolean(vt->keypad_appl_mode),
                SIM_make_attr_boolean(vt->insert_mode));
}

static set_error_t
set_tab_stops(conf_object_t *obj, attr_value_t *val)
{
        vt100_data_t *vt = vt100_data(obj);
        
        /* FIXME: Can we depend on having the screen width at this point? */
        int width = screen_width(vt);
        VRESIZE(vt->tabs, width);
        VFORI(vt->tabs, i)
                VSET(vt->tabs, i, false);
        int ntabs = SIM_attr_list_size(*val);
        for (int i = 0; i < ntabs; i++) {
                int c = SIM_attr_integer(SIM_attr_list_item(*val, i));
                if (c < 1 || c > width)
                        return Sim_Set_Illegal_Value;
                VSET(vt->tabs, c - 1, true);
        }
        return Sim_Set_Ok;
}

static attr_value_t
get_tab_stops(conf_object_t *obj)
{
        vt100_data_t *vt = vt100_data(obj);

        int ntabs = 0;
        VFORI(vt->tabs, i)
                ntabs += VGET(vt->tabs, i);

        attr_value_t ret = SIM_alloc_attr_list(ntabs);
        int j = 0;
        VFORI(vt->tabs, i)
                if (VGET(vt->tabs, i))
                        SIM_attr_list_set_item(&ret, j++,
                                               SIM_make_attr_uint64(i + 1));

        return ret;
}

static set_error_t
set_parser_state(conf_object_t *obj, attr_value_t *val)
{
        vt100_data_t *vt = vt100_data(obj);

        int state = SIM_attr_integer(SIM_attr_list_item(*val, 0));
        bool overflow = SIM_attr_boolean(SIM_attr_list_item(*val, 1));
        attr_value_t params = SIM_attr_list_item(*val, 2);
        int nparams = SIM_attr_list_size(params);
        if (nparams > MAXPARAMS)
                return Sim_Set_Illegal_Value;

        vt->parser.state = state;
        vt->parser.nparams = nparams;
        vt->parser.overflow = overflow;
        for (int i = 0; i < nparams; i++)
                vt->parser.params[i] =
                        SIM_attr_integer(SIM_attr_list_item(params, i));
        return Sim_Set_Ok;
}

static attr_value_t
get_parser_state(conf_object_t *obj)
{
        vt100_data_t *vt = vt100_data(obj);
        attr_value_t params = SIM_alloc_attr_list(vt->parser.nparams);
        for (int i = 0; i < vt->parser.nparams; i++)
                SIM_attr_list_set_item(
                        &params, i,
                        SIM_make_attr_uint64(vt->parser.params[i]));

        return SIM_make_attr_list(
                3,
                SIM_make_attr_uint64(vt->parser.state),
                SIM_make_attr_boolean(vt->parser.overflow),
                params);
}

static set_error_t
set_backspace_sends_del(conf_object_t *obj, attr_value_t *val)
{
        vt100_data_t *vt = vt100_data(obj);
        vt->backspace_key_sends_bs = !SIM_attr_boolean(*val);
        return Sim_Set_Ok;
}

static attr_value_t
get_backspace_sends_del(conf_object_t *obj)
{
        vt100_data_t *vt = vt100_data(obj);
        return SIM_make_attr_boolean(!vt->backspace_key_sends_bs);
}

static attr_value_t
get_input_mode(conf_object_t *obj)
{
        vt100_data_t *vt = vt100_data(obj);
        return SIM_make_attr_string(vt->microsoft_keyboard_mode
                                    ? "microsoft" : "standard");
}

static set_error_t
set_input_mode(conf_object_t *obj, attr_value_t *val)
{
        vt100_data_t *vt = vt100_data(obj);
        if (strcmp(SIM_attr_string(*val), "standard") == 0)
                vt->microsoft_keyboard_mode = false;
        else if (strcmp(SIM_attr_string(*val), "microsoft") == 0)
                vt->microsoft_keyboard_mode = true;
        else
                return Sim_Set_Illegal_Value;
        return Sim_Set_Ok;
}

vt100_data_t *
make_vt100(text_console_t *tc)
{
        vt100_data_t *vt = MM_ZALLOC(1, vt100_data_t);
        vt->tc = tc;
        vt->parser.state = Parse_Ground;
        vt->parser.nparams = 0;
        VINIT(vt->tabs);
        terminal_reset(vt);
        return vt;
}

void
finalise_vt100(vt100_data_t *vt)
{
        frontend_update_cursor_pos(vt);
}

void
destroy_vt100(vt100_data_t *vt)
{
        VFREE(vt->tabs);
        MM_FREE(vt);
}

void
register_vt100(conf_class_t *cl)
{
        SIM_register_attribute(
                cl, "cursor_data",
                get_cursor_data,
                set_cursor_data,
                Sim_Attr_Optional,
                "[iibb]",
                "Current cursor data"
                " (column, row, pending wrap, relative coordinates).");

        SIM_register_attribute(
                cl, "saved_cursor_data",
                get_saved_cursor_data,
                set_saved_cursor_data,
                Sim_Attr_Optional,
                "[iibb]",
                "Saved cursor data"
                " (column, row, pending wrap, relative coordinates).");

        SIM_register_attribute(
                cl, "DECAWM",
                get_decawm,
                set_decawm,
                Sim_Attr_Optional,
                "b",
                "Auto-wrap mode.");

        SIM_register_attribute(
                cl, "scrolling_region",
                get_scroll_region,
                set_scroll_region,
                Sim_Attr_Optional,
                "[ii]",
                "Scrolling region line numbers (top, bottom), 0-based.");

        SIM_register_attribute(
                cl, "text_attributes",
                get_text_attrib,
                set_text_attrib,
                Sim_Attr_Optional,
                "[iiib]",
                "Current text attributes"
                 " (attribute, fg col, bg col, reverse).");

        SIM_register_attribute(
                cl, "modes",
                get_modes,
                set_modes,
                Sim_Attr_Optional,
                "[bbb]",
                "Current terminal modes (cursor, keypad, insert).");

        SIM_register_attribute(
                cl, "tab_stops",
                get_tab_stops,
                set_tab_stops,
                Sim_Attr_Optional,
                "[i*]",
                "Current tab stop positions (1-based).");
        
        SIM_register_attribute(
                cl, "parser_state",
                get_parser_state,
                set_parser_state,
                Sim_Attr_Optional,
                "[ib[i*]]",
                "VT parser state");

        SIM_register_attribute(
                cl, "backspace_sends_del",
                get_backspace_sends_del,
                set_backspace_sends_del,
                Sim_Attr_Optional,
                "b",
                "If TRUE, the backspace key sends DEL (127), otherwise BS (8)."
                " Default is TRUE.");

        SIM_register_attribute(
                cl, "input_mode",
                get_input_mode,
                set_input_mode,
                Sim_Attr_Optional,
                "s",
                "What terminal type to emulate for keyboard inputs."
                " Possible values are <tt>\"standard\"</tt> (VT220/XTerm),"
                " which is the default, and <tt>\"microsoft\"</tt>"
                " (Microsoft's \"VT100+\").");

}
