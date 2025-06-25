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

#ifndef SIMICS_SIMULATOR_CMD_LINE_H
#define SIMICS_SIMULATOR_CMD_LINE_H

#include <simics/base/types.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef int cmd_line_id_t;

/* This enum must be kept in sync with the array simicsToSwt in
 * SimicsKeyTranslator.java, otherwise the Eclipse Console view
 * will break. */
typedef enum {
        Cmd_Line_Key_Left,
        Cmd_Line_Key_Right,
        Cmd_Line_Key_Up,
        Cmd_Line_Key_Down,
        Cmd_Line_Key_PgUp,
        Cmd_Line_Key_PgDn,
        Cmd_Line_Key_Home,
        Cmd_Line_Key_End,
        Cmd_Line_Key_Back,
        Cmd_Line_Key_Tab,
        Cmd_Line_Key_Enter,
        Cmd_Line_Key_Del,
        Cmd_Line_Key_Ins,
        Cmd_Line_Key_F1,
        Cmd_Line_Key_F2,
        Cmd_Line_Key_F3,
        Cmd_Line_Key_F4,
        Cmd_Line_Key_F5,
        Cmd_Line_Key_F6,
        Cmd_Line_Key_F7,
        Cmd_Line_Key_F8,
        Cmd_Line_Key_F9,
        Cmd_Line_Key_F10,
        Cmd_Line_Key_F11,
        Cmd_Line_Key_F12
        // followed by ASCII 32 - 126
} cmd_line_key_t;

typedef enum {
        Cmd_Line_Mod_None,
        Cmd_Line_Mod_Shift = 1,
        Cmd_Line_Mod_Ctrl = 2,
        Cmd_Line_Mod_Alt = 4
} cmd_line_mod_t;

/* Internal use only */
SIM_INTERFACE(cmd_line_frontend) {
        /* write a character on the command line at the current cursor
           position, overwriting any existing character there */
        void (*write)(conf_object_t *obj, const char *NOTNULL str);

        /* deletes the line from current position to the end of the line */
        void (*delete_line)(conf_object_t *obj);

        void (*disconnect)(conf_object_t *obj);

        /* moves the cursor <param>num</param> characters to the left. */
        void (*cursor_left)(conf_object_t *obj, int num);

        /* moves the cursor <param>num</param> characters to the right. */
        void (*cursor_right)(conf_object_t *obj, int num);

        /* clears the screen and moves the cursor to position 0. Optional,
           may be NULL if not implemented. */
        void (*clear_screen)(conf_object_t *obj);

        /* called when the prompt has been printed with the position on
           the line of the first non-prompt character. Optional, may be
           null if not implemented */
        void (*prompt_end)(conf_object_t *obj, int pos);

        /* optional */
        void (*bell)(conf_object_t *obj);
};

/* internal interface for now */
#define CMD_LINE_FRONTEND_INTERFACE "cmd_line_frontend"

/* only when mirroring selection/clipboard (internal use only) */
SIM_INTERFACE(cmd_line_selection) {
        void (*new_selection)(conf_object_t *obj, int x, int y);
        void (*to_clipboard)(conf_object_t *obj, const char *NOTNULL str);
};

/* internal interface for now */
#define CMD_LINE_SELECTION_INTERFACE "cmd_line_selection"

/* The following functions are internal and will probably be replaced
   by a "cmd_line" interface. */

EXPORTED cmd_line_id_t VT_command_line_create(
        conf_object_t *obj, bool interactive, bool primary);
EXPORTED void VT_command_line_delete(cmd_line_id_t id);

EXPORTED void VT_command_line_new_position(cmd_line_id_t id, int pos);
EXPORTED void VT_command_line_new_selection(
        cmd_line_id_t id, int left, int right);
EXPORTED void VT_command_line_to_clipboard(cmd_line_id_t id, const char *str);
EXPORTED void VT_command_line_key(cmd_line_id_t id,
                                  cmd_line_mod_t mod, cmd_line_key_t key);
EXPORTED void VT_command_line_set_size(cmd_line_id_t, int cols, int rows);

SIM_INTERFACE(terminal_server) {
        void (*write)(conf_object_t *obj, const char *NOTNULL str);
        void (*set_size)(conf_object_t *obj, int left, int right);
        void (*disconnect)(conf_object_t *obj);
};

/* internal interface for now */
#define TERMINAL_SERVER_INTERFACE "terminal_server"

/* internal use only */
SIM_INTERFACE(terminal_client) {
        void (*write)(conf_object_t *obj, int id, const char *str);
        void (*disconnect)(conf_object_t *obj, int id);
};

/* internal interface for now */
#define TERMINAL_CLIENT_INTERFACE "terminal_client"

typedef char *(*prompt_customizer_t)(const char *);

EXPORTED void VT_set_prompt_customizer(prompt_customizer_t NOTNULL cb);

#if defined(__cplusplus)
}
#endif

#endif
