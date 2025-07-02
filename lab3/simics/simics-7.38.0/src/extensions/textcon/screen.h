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

#ifndef TEXTCON_SCREEN_H
#define TEXTCON_SCREEN_H

#include <simics/base/conf-object.h>
#include <simics/base/types.h>
#include <simics/simulator-iface/consoles.h>
#include "text-console.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct screen screen_t;

void text_console_output(screen_t *sc, uint8 value);

void frontend_set_cursor_pos(screen_t *sc, int row, int col);

void scroll_up_to_scrollback(screen_t *sc, int top, int bottom,
                             text_console_colour_t bg);
void screen_scroll_down(screen_t *sc, int top, int bottom, int n,
                        text_console_colour_t bg);
void screen_scroll_up(screen_t *sc, int top, int bottom, int n,
                      text_console_colour_t bg);
        
void screen_erase(screen_t *sc,
                  int top, int left, int bottom, int right,
                  text_console_colour_t bg);
void screen_insert_chars(screen_t *sc,
                         int row, int col, int num_chars,
                         text_console_colour_t bg);
void screen_delete_chars(screen_t *sc,
                         int row, int col, int num_chars,
                         text_console_colour_t bg);

int get_screen_width(screen_t *sc);
int get_screen_height(screen_t *sc);
void set_screen_width(screen_t *sc, int width);
void update_screen_size(screen_t *sc, int width, int height);

void set_screen_contents(screen_t *sc, int row, int col,
                         uint8 ch, uint8 attrib,
                         text_console_colour_t fg_colour,
                         text_console_colour_t bg_colour);
void set_wrapping_line(screen_t *sc, int line_num);

int text_console_get_frontend_handle(screen_t *sc);

void get_screen_string(screen_t *sc, strbuf_t *buf);

screen_t *make_screen(text_console_t *tc, const char *name);
void finalise_screen(screen_t *sc);
void pre_delete_screen(screen_t *sc);
void destroy_screen(screen_t *sc);
void register_screen(conf_class_t *cl);

#if defined(__cplusplus)
}
#endif
        
#endif
