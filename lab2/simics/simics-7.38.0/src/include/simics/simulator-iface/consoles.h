/*
  © 2015 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SIMULATOR_IFACE_CONSOLES_H
#define SIMICS_SIMULATOR_IFACE_CONSOLES_H

#include <simics/base/types.h>
#include <simics/pywrap.h>
#include <simics/base/conf-object.h>
#include <simics/model-iface/sim-keys.h>

#if defined(__cplusplus)
extern "C" {
#endif

/*
   <add id="serial_console_frontend_interface_t">

   The <iface>serial_console_frontend</iface> interface can be implemented by
   devices that want to retrieve the character stream passing through the text
   console. Objects implementing this interface can be attached to a text
   console and will receive output in the same way as a telnet connection.

   <insert-until text="// ADD INTERFACE serial_console_frontend_interface_t"/>
   </add>

   <add id="serial_console_frontend_interface_exec_context"> Cell Context
   </add>
*/
SIM_INTERFACE(serial_console_frontend) {
        void (*write)(conf_object_t *NOTNULL obj, uint8 value);
};
#define SERIAL_CONSOLE_FRONTEND_INTERFACE "serial_console_frontend"
// ADD INTERFACE serial_console_frontend_interface_t

/*
   <add id="con_input_interface_t">

   The <iface>con_input</iface> interface facilitates sending simulated input
   to the target system. It is implemented by the text console.

   Simulated input is passed through the associated <class>recorder</class>
   object and then sent to the serial device connected to the console. The
   console will not perform VT100 input handling or BS/DEL conversion.

   The <fun>input_str</fun> method allows input of a NUL-terminated string. The
   <fun>input_data</fun> allows sending a buffer, which can include NUL
   characters.

   <insert-until text="// ADD INTERFACE con_input_interface_t"/>
   </add>

   <add id="con_input_interface_exec_context"> Global Context
   </add>
*/
SIM_INTERFACE(con_input) {
        void (*input_str)(conf_object_t *NOTNULL obj, const char *str);
        void (*input_data)(conf_object_t *NOTNULL obj, bytes_t data);
};
#define CON_INPUT_INTERFACE "con_input"
// ADD INTERFACE con_input_interface_t

/*
   <add id="con_input_code_interface_t">

   The <iface>con_input_code</iface> interface facilitates sending simulated
   key events to the simulation. It is implemented by the graphics console.

   Simulated key events are passed through the associated
   <class>recorder</class> object and then sent to the keyboard connected to the
   console.

   The <fun>input</fun> method sends the key <arg>code</arg>, either a key
   press or a release, depending on the <arg>down</arg> parameter.

   <insert-until text="// ADD INTERFACE con_input_code_interface_t"/>
   </add>

   <add id="con_input_code_interface_exec_context"> Global Context
   </add>
*/
SIM_INTERFACE(con_input_code) {
        void (*input)(conf_object_t *NOTNULL obj, sim_key_t code, bool down);
};
#define CON_INPUT_CODE_INTERFACE "con_input_code"
// ADD INTERFACE con_input_code_interface_t

/* <add id="break_string_cb_t DOC">
   <ndx>break_string_cb_t</ndx>
   <name index="true">break_string_cb_t</name>
   <doc>
   <doc-item name="NAME">break_string_cb_t</doc-item>

   <doc-item name="DESCRIPTION"> Callback function used by the string
   breakpoint system, called when the associated string breakpoint has
   matched. The <arg>obj</arg> parameter is the console where the breakpoint
   matched, <arg>str</arg> is the break string that matched,
   <arg>break_id</arg> is the associated breakpoint id, returned by the
   <fun>add</fun> or <fun>add_single</fun> method in the
   <iface>break_strings</iface> interface, and <arg>arg</arg> is the callback
   data passed to that method. The return value is ignored.
   </doc-item>
   </doc></add>
*/
typedef int (*break_string_cb_t)(conf_object_t *obj,
                                 const char *str, int64 break_id,
                                 lang_void *arg);

/* Deprecated, provided for backwards compatibility. */
SIM_INTERFACE(break_strings) {
        int64 (*add)(conf_object_t *NOTNULL obj, const char *str,
                     break_string_cb_t cb, lang_void *arg);
        int64 (*add_single)(conf_object_t *NOTNULL obj, const char *str,
                            break_string_cb_t cb, lang_void *arg);
        void (*remove)(conf_object_t *NOTNULL obj, int64 bp_id);
};
#define BREAK_STRINGS_INTERFACE "break_strings"

/*
   <add id="break_strings_v2_interface_t">

   The <iface>break_strings_v2</iface> interface facilitates management of
   string breakpoints. It is implemented by the text console and the graphics
   console (but will only function when in text mode).

   The text console tries to match each break string on the stream of
   characters coming from the attached serial device, and if a match occurs,
   the given callback function will be called. If no callback is given, the
   simulation will be stopped. The graphics console behaves in the same way,
   but in this case the character stream is defined by what is sent to the
   console via the <iface>vga_text_update</iface> interface. Break strings
   only lives during a single Simics session, they are not checkpointed.

   The <fun>add</fun> method registers a breakpoint string <arg>str</arg>, and
   returns a breakpoint ID, unique during the Simics session, which is also
   passed to <arg>cb</arg> when the breakpoint matches. If <arg>cb</arg> is not
   <tt>NULL</tt>, then this function will be called on breakpoint match,
   otherwise a match stops the simulation.

   The <fun>add_single</fun> method is similar to <fun>add</fun>, but the
   breakpoint is removed automatically after the first match.

   The <fun>add_regexp</fun> method is similar to <fun>add</fun>, but the given
   string is interpreted as a regular expression. The support regular
   expression syntax is that of the Hyperscan library
   <url>https://hyperscan.io</url>.

   The <fun>remove</fun> method deactivates a previously activated breakpoint.

   <insert-until text="// ADD INTERFACE break_strings_v2_interface_t"/> </add>

   <add id="break_strings_v2_interface_exec_context"> Global Context
   for all methods
   </add>
*/
SIM_INTERFACE(break_strings_v2) {
        int64 (*add)(conf_object_t *NOTNULL obj, const char *str,
                     break_string_cb_t cb, lang_void *arg);
        int64 (*add_single)(conf_object_t *NOTNULL obj, const char *str,
                            break_string_cb_t cb, lang_void *arg);
        int64 (*add_regexp)(conf_object_t *NOTNULL obj, const char *str,
                            break_string_cb_t cb, lang_void *arg);
        void (*remove)(conf_object_t *NOTNULL obj, int64 bp_id);
};
#define BREAK_STRINGS_V2_INTERFACE "break_strings_v2"
// ADD INTERFACE break_strings_v2_interface_t
        
/* <add id="gfx_break_cb_t DOC">
   <ndx>gfx_break_cb_t</ndx>
   <name index="true">gfx_break_cb_t</name>
   <doc>
   <doc-item name="NAME">gfx_break_cb_t</doc-item>

   <doc-item name="DESCRIPTION"> Callback function used by the graphics
   breakpoint system, called when the associated graphical breakpoint has
   matched. The <arg>obj</arg> parameter is the graphics console where the
   breakpoint matched, <arg>break_id</arg> is the associated breakpoint id,
   returned by the <fun>add</fun> method in the <iface>gfx_break</iface>
   interface, and <arg>arg</arg> is the callback data passed to that method.
   </doc-item>
   </doc></add>
*/
typedef int (*gfx_break_cb_t)(conf_object_t *obj,
                              int64 break_id, lang_void *arg);

/* <add id="gbp_header_t DOC">
   <ndx>gbp_header_t</ndx>
   <name index="true">gbp_header_t</name>
   <doc>
   <doc-item name="NAME">gbp_header_t</doc-item>

   <doc-item name="DESCRIPTION"> Header for graphical breakpoint files, also
   returned by the <fun>info</fun> method of the <iface>gfx_break</iface>
   interface.

   The fields <var>magic</var> and <var>format</var> are always
   <tt>GBP_MAGIC</tt> and <tt>GBP_FMT_V3_32</tt>, respectively.

   The field <var>bytes</var> is the size of the image data in the file, not
   including the header.

   The other fields specify the breakpoint image patch location on the screen.
   </doc-item>
   </doc></add>
*/
typedef struct {
        uint32 magic;
        uint32 format;
        uint64 bytes;
        uint32 minx;
        uint32 miny;
        uint32 maxx;
        uint32 maxy;
} gbp_header_t;
        
/*
   <add id="gfx_break_interface_t">

   The <iface>gfx_break</iface> interface facilitates management of graphical
   breakpoints. It is implemented by the graphics console.

   A graphical breakpoint is defined by an image patch and a location on the
   screen, stored in a file using a bespoke format. Such a file can be created
   using the <fun>store</fun> method.

   A breakpoint can then be activated using the <fun>add</fun> method.
   The console will check if the breakpoint matches the screen data
   every <arg>interval</arg> seconds in virtual time, using the clock that
   is associated to the console. Graphical breakpoints
   are therefore deterministic.

   Note that the coordinates of the image patch is stored in the graphical
   breakpoint, and the breakpoint will only match on those coordinates.

   The <fun>store</fun> method stores the specified rectangle on the screen in
   the given file. It returns <tt>false</tt> if the rectangle is invalid or on
   I/O error.

   The <fun>add</fun> method activates a previously stored graphical
   breakpoint, and returns a breakpoint ID, which is also passed to
   <arg>cb</arg> when the breakpoint matches. The parameter <arg>name</arg> is
   the breakpoint name that will appear in log messages. If <arg>name</arg> is
   <tt>NULL</tt> then the file name is used. If <arg>oneshot</arg> is true, the
   breakpoint will be removed automatically after the first match. The
   <arg>interval</arg> parameter specifies how often the breakpoint is tested,
   in seconds of simulated time. If <arg>cb</arg> is not <tt>NULL</tt>, then
   this function will be called on breakpoint match, otherwise a match stops
   the simulation. If the breakpoint file cannot be loaded, the method returns
   <tt>-1</tt>.

   The <fun>remove</fun> method deactivates a previously activated breakpoint.

   The <fun>match</fun> method determines if a stored graphical breakpoint
   matches the current screen. It returns -1 if the breakpoint could not be
   loaded, 1 on match or 0 otherwise.

   The <fun>info</fun> method returns the header of a stored graphical
   breakpoint, including the image patch coordinates. If the given file cannot
   be read or has invalid data, then a header containing all 0's is returned.

   The <fun>export_png</fun> method converts the image data in a graphical
   breakpoint file to PNG format.

   The <fun>add_bytes</fun> method behaves as <fun>add</fun> but reads the
   breakpoint data from memory instead of a file.

   <insert-until text="// ADD INTERFACE gfx_break_interface_t"/> </add>

   <add id="gfx_break_interface_exec_context"> Global Context
   for all methods
   </add>
*/
SIM_INTERFACE(gfx_break) {
        bool (*store)(conf_object_t *NOTNULL obj,
                      const char *file,
                      int minx, int miny, int maxx, int maxy);
        int64 (*add)(conf_object_t *NOTNULL obj,
                     const char *file, const char *name,
                     bool oneshot, double interval,
                     gfx_break_cb_t cb, lang_void *arg);
        bool (*remove)(conf_object_t *NOTNULL obj, int64 break_id);
        int (*match)(conf_object_t *NOTNULL obj, const char *file);
        gbp_header_t (*info)(conf_object_t *NOTNULL obj, const char *file);
        bool (*export_png)(conf_object_t *NOTNULL obj, const char *file,
                           const char *png_file);
        int64 (*add_bytes)(conf_object_t *NOTNULL obj,
                           bytes_t data, const char *name,
                           bool oneshot, double interval,
                           gfx_break_cb_t cb, lang_void *arg);
};
#define GFX_BREAK_INTERFACE "gfx_break"
// ADD INTERFACE gfx_break_interface_t

/*
   <add id="telnet_connection_v2_interface_t">

   The <iface>telnet_connection_v2</iface> interface is used to control the text
   console telnet server.

   The text console has a built-in telnet server.

   The <fun>listening</fun> method indicates whether the server is listening
   for connections.

   The <fun>connected</fun> method indicates whether there is a connected
   telnet client.

   The <fun>disconnect</fun> method forcibly disconnects any connected client.

   <insert-until text="// ADD INTERFACE telnet_connection_v2_interface_t"/> </add>

   <add id="telnet_connection_v2_interface_exec_context"> Global Context
   for all methods
   </add>
*/
SIM_INTERFACE(telnet_connection_v2) {
        bool (*listening)(conf_object_t *NOTNULL obj);
        bool (*connected)(conf_object_t *NOTNULL obj);
        void (*disconnect)(conf_object_t *NOTNULL obj);
};
#define TELNET_CONNECTION_V2_INTERFACE "telnet_connection_v2"
// ADD INTERFACE telnet_connection_v2_interface_t

/*
   <add id="vnc_server_v2_interface_t">

   The <iface>vnc_server_v2</iface> interface is used to control the graphics
   console VNC server.

   The graphics console has a built-in VNC server, supporting any number of
   connected clients.

   The <fun>listening</fun> method indicates whether the server is listening
   for connections.

   The <fun>num_clients</fun> method returns the number of connected clients.

   The <fun>disconnect</fun> method forcibly disconnects any connected client.

   <insert-until text="// ADD INTERFACE vnc_server_v2_interface_t"/> </add>

   <add id="vnc_server_v2_interface_exec_context"> Global Context
   for all methods
   </add>
*/
SIM_INTERFACE(vnc_server_v2) {
        bool (*listening)(conf_object_t *NOTNULL obj);
        int (*num_clients)(conf_object_t *NOTNULL obj);
        void (*disconnect)(conf_object_t *NOTNULL obj);
};
#define VNC_SERVER_V2_INTERFACE "vnc_server_v2"
// ADD INTERFACE vnc_server_v2_interface_t

/*
   <add id="host_serial_interface_t">

   The <iface>host_serial</iface> interface is used to control the text
   console host serial connection.

   The <fun>setup</fun> method will set up a host serial connection on a pty
   (Linux) or COM port (Windows). On Linux, <arg>name</arg> can be
   <tt>NULL</tt> in which case a new pty is opened.

   The <fun>name</fun> method returns the name of any opened pty or COM port,
   or <tt>NULL</tt>.

   The <fun>shutdown</fun> closes any opened pty or COM port.

   <insert-until text="// ADD INTERFACE host_serial_interface_t"/> </add>

   <add id="host_serial_interface_exec_context"> Global Context
   for all methods
   </add>
*/
SIM_INTERFACE(host_serial) {
        bool (*setup)(conf_object_t *NOTNULL obj, const char *name);
        const char *(*name)(conf_object_t *NOTNULL obj);
        void (*shutdown)(conf_object_t *NOTNULL obj);
};
#define HOST_SERIAL_INTERFACE "host_serial"
// ADD INTERFACE host_serial_interface_t

/*
   <add id="screenshot_interface_t">

   The <iface>screenshot</iface> interface facilitates storing screenshots. It
   is implemented by the graphics console.

   All screenshots store current screen data, using 24-bit RGB pixel format.

   <insert-until text="// ADD INTERFACE screenshot_interface_t"/> </add>

   <add id="screenshot_interface_exec_context"> Global Context
   for all methods
   </add>
*/
SIM_INTERFACE(screenshot) {
        bool (*save_png)(conf_object_t *NOTNULL obj, const char *filename);
        bool (*save_bmp)(conf_object_t *NOTNULL obj, const char *filename);
};
#define SCREENSHOT_INTERFACE "screenshot"
// ADD INTERFACE screenshot_interface_t


/* The following types and interfaces are internal and subject to change
   at any time. */

/* Text console character attributes.
   Inverse video is represented by swapping the foreground and background
   colours, so there is no bit for it here. */
typedef enum {
        Text_Console_Attrib_Bold = 1,
        Text_Console_Attrib_Underline = 2,
} text_console_text_attrib_t;

/* Text console colours.
   https://www.wikipedia.org/wiki/ANSI_escape_code#8-bit
 */
typedef enum {
        Text_Console_Colour_Black = 0,
        Text_Console_Colour_Red = 1,
        Text_Console_Colour_Green = 2,
        Text_Console_Colour_Yellow = 3,
        Text_Console_Colour_Blue = 4,
        Text_Console_Colour_Magenta = 5,
        Text_Console_Colour_Cyan = 6,
        Text_Console_Colour_White = 7,

        Text_Console_Colour_Black_Bright = 8,
        Text_Console_Colour_Red_Bright = 9,
        Text_Console_Colour_Green_Bright = 10,
        Text_Console_Colour_Yellow_Bright = 11,
        Text_Console_Colour_Blue_Bright = 12,
        Text_Console_Colour_Magenta_Bright = 13,
        Text_Console_Colour_Cyan_Bright = 14,
        Text_Console_Colour_White_Bright = 15,

        Text_Console_Colour_Cube_16 = 16,
        Text_Console_Colour_Cube_17 = 17,
        Text_Console_Colour_Cube_18 = 18,
        Text_Console_Colour_Cube_19 = 19,
        Text_Console_Colour_Cube_20 = 20,
        Text_Console_Colour_Cube_21 = 21,
        Text_Console_Colour_Cube_22 = 22,
        Text_Console_Colour_Cube_23 = 23,
        Text_Console_Colour_Cube_24 = 24,
        Text_Console_Colour_Cube_25 = 25,
        Text_Console_Colour_Cube_26 = 26,
        Text_Console_Colour_Cube_27 = 27,
        Text_Console_Colour_Cube_28 = 28,
        Text_Console_Colour_Cube_29 = 29,
        Text_Console_Colour_Cube_30 = 30,
        Text_Console_Colour_Cube_31 = 31,
        Text_Console_Colour_Cube_32 = 32,
        Text_Console_Colour_Cube_33 = 33,
        Text_Console_Colour_Cube_34 = 34,
        Text_Console_Colour_Cube_35 = 35,
        Text_Console_Colour_Cube_36 = 36,
        Text_Console_Colour_Cube_37 = 37,
        Text_Console_Colour_Cube_38 = 38,
        Text_Console_Colour_Cube_39 = 39,
        Text_Console_Colour_Cube_40 = 40,
        Text_Console_Colour_Cube_41 = 41,
        Text_Console_Colour_Cube_42 = 42,
        Text_Console_Colour_Cube_43 = 43,
        Text_Console_Colour_Cube_44 = 44,
        Text_Console_Colour_Cube_45 = 45,
        Text_Console_Colour_Cube_46 = 46,
        Text_Console_Colour_Cube_47 = 47,
        Text_Console_Colour_Cube_48 = 48,
        Text_Console_Colour_Cube_49 = 49,
        Text_Console_Colour_Cube_50 = 50,
        Text_Console_Colour_Cube_51 = 51,
        Text_Console_Colour_Cube_52 = 52,
        Text_Console_Colour_Cube_53 = 53,
        Text_Console_Colour_Cube_54 = 54,
        Text_Console_Colour_Cube_55 = 55,
        Text_Console_Colour_Cube_56 = 56,
        Text_Console_Colour_Cube_57 = 57,
        Text_Console_Colour_Cube_58 = 58,
        Text_Console_Colour_Cube_59 = 59,
        Text_Console_Colour_Cube_60 = 60,
        Text_Console_Colour_Cube_61 = 61,
        Text_Console_Colour_Cube_62 = 62,
        Text_Console_Colour_Cube_63 = 63,
        Text_Console_Colour_Cube_64 = 64,
        Text_Console_Colour_Cube_65 = 65,
        Text_Console_Colour_Cube_66 = 66,
        Text_Console_Colour_Cube_67 = 67,
        Text_Console_Colour_Cube_68 = 68,
        Text_Console_Colour_Cube_69 = 69,
        Text_Console_Colour_Cube_70 = 70,
        Text_Console_Colour_Cube_71 = 71,
        Text_Console_Colour_Cube_72 = 72,
        Text_Console_Colour_Cube_73 = 73,
        Text_Console_Colour_Cube_74 = 74,
        Text_Console_Colour_Cube_75 = 75,
        Text_Console_Colour_Cube_76 = 76,
        Text_Console_Colour_Cube_77 = 77,
        Text_Console_Colour_Cube_78 = 78,
        Text_Console_Colour_Cube_79 = 79,
        Text_Console_Colour_Cube_80 = 80,
        Text_Console_Colour_Cube_81 = 81,
        Text_Console_Colour_Cube_82 = 82,
        Text_Console_Colour_Cube_83 = 83,
        Text_Console_Colour_Cube_84 = 84,
        Text_Console_Colour_Cube_85 = 85,
        Text_Console_Colour_Cube_86 = 86,
        Text_Console_Colour_Cube_87 = 87,
        Text_Console_Colour_Cube_88 = 88,
        Text_Console_Colour_Cube_89 = 89,
        Text_Console_Colour_Cube_90 = 90,
        Text_Console_Colour_Cube_91 = 91,
        Text_Console_Colour_Cube_92 = 92,
        Text_Console_Colour_Cube_93 = 93,
        Text_Console_Colour_Cube_94 = 94,
        Text_Console_Colour_Cube_95 = 95,
        Text_Console_Colour_Cube_96 = 96,
        Text_Console_Colour_Cube_97 = 97,
        Text_Console_Colour_Cube_98 = 98,
        Text_Console_Colour_Cube_99 = 99,
        Text_Console_Colour_Cube_100 = 100,
        Text_Console_Colour_Cube_101 = 101,
        Text_Console_Colour_Cube_102 = 102,
        Text_Console_Colour_Cube_103 = 103,
        Text_Console_Colour_Cube_104 = 104,
        Text_Console_Colour_Cube_105 = 105,
        Text_Console_Colour_Cube_106 = 106,
        Text_Console_Colour_Cube_107 = 107,
        Text_Console_Colour_Cube_108 = 108,
        Text_Console_Colour_Cube_109 = 109,
        Text_Console_Colour_Cube_110 = 110,
        Text_Console_Colour_Cube_111 = 111,
        Text_Console_Colour_Cube_112 = 112,
        Text_Console_Colour_Cube_113 = 113,
        Text_Console_Colour_Cube_114 = 114,
        Text_Console_Colour_Cube_115 = 115,
        Text_Console_Colour_Cube_116 = 116,
        Text_Console_Colour_Cube_117 = 117,
        Text_Console_Colour_Cube_118 = 118,
        Text_Console_Colour_Cube_119 = 119,
        Text_Console_Colour_Cube_120 = 120,
        Text_Console_Colour_Cube_121 = 121,
        Text_Console_Colour_Cube_122 = 122,
        Text_Console_Colour_Cube_123 = 123,
        Text_Console_Colour_Cube_124 = 124,
        Text_Console_Colour_Cube_125 = 125,
        Text_Console_Colour_Cube_126 = 126,
        Text_Console_Colour_Cube_127 = 127,
        Text_Console_Colour_Cube_128 = 128,
        Text_Console_Colour_Cube_129 = 129,
        Text_Console_Colour_Cube_130 = 130,
        Text_Console_Colour_Cube_131 = 131,
        Text_Console_Colour_Cube_132 = 132,
        Text_Console_Colour_Cube_133 = 133,
        Text_Console_Colour_Cube_134 = 134,
        Text_Console_Colour_Cube_135 = 135,
        Text_Console_Colour_Cube_136 = 136,
        Text_Console_Colour_Cube_137 = 137,
        Text_Console_Colour_Cube_138 = 138,
        Text_Console_Colour_Cube_139 = 139,
        Text_Console_Colour_Cube_140 = 140,
        Text_Console_Colour_Cube_141 = 141,
        Text_Console_Colour_Cube_142 = 142,
        Text_Console_Colour_Cube_143 = 143,
        Text_Console_Colour_Cube_144 = 144,
        Text_Console_Colour_Cube_145 = 145,
        Text_Console_Colour_Cube_146 = 146,
        Text_Console_Colour_Cube_147 = 147,
        Text_Console_Colour_Cube_148 = 148,
        Text_Console_Colour_Cube_149 = 149,
        Text_Console_Colour_Cube_150 = 150,
        Text_Console_Colour_Cube_151 = 151,
        Text_Console_Colour_Cube_152 = 152,
        Text_Console_Colour_Cube_153 = 153,
        Text_Console_Colour_Cube_154 = 154,
        Text_Console_Colour_Cube_155 = 155,
        Text_Console_Colour_Cube_156 = 156,
        Text_Console_Colour_Cube_157 = 157,
        Text_Console_Colour_Cube_158 = 158,
        Text_Console_Colour_Cube_159 = 159,
        Text_Console_Colour_Cube_160 = 160,
        Text_Console_Colour_Cube_161 = 161,
        Text_Console_Colour_Cube_162 = 162,
        Text_Console_Colour_Cube_163 = 163,
        Text_Console_Colour_Cube_164 = 164,
        Text_Console_Colour_Cube_165 = 165,
        Text_Console_Colour_Cube_166 = 166,
        Text_Console_Colour_Cube_167 = 167,
        Text_Console_Colour_Cube_168 = 168,
        Text_Console_Colour_Cube_169 = 169,
        Text_Console_Colour_Cube_170 = 170,
        Text_Console_Colour_Cube_171 = 171,
        Text_Console_Colour_Cube_172 = 172,
        Text_Console_Colour_Cube_173 = 173,
        Text_Console_Colour_Cube_174 = 174,
        Text_Console_Colour_Cube_175 = 175,
        Text_Console_Colour_Cube_176 = 176,
        Text_Console_Colour_Cube_177 = 177,
        Text_Console_Colour_Cube_178 = 178,
        Text_Console_Colour_Cube_179 = 179,
        Text_Console_Colour_Cube_180 = 180,
        Text_Console_Colour_Cube_181 = 181,
        Text_Console_Colour_Cube_182 = 182,
        Text_Console_Colour_Cube_183 = 183,
        Text_Console_Colour_Cube_184 = 184,
        Text_Console_Colour_Cube_185 = 185,
        Text_Console_Colour_Cube_186 = 186,
        Text_Console_Colour_Cube_187 = 187,
        Text_Console_Colour_Cube_188 = 188,
        Text_Console_Colour_Cube_189 = 189,
        Text_Console_Colour_Cube_190 = 190,
        Text_Console_Colour_Cube_191 = 191,
        Text_Console_Colour_Cube_192 = 192,
        Text_Console_Colour_Cube_193 = 193,
        Text_Console_Colour_Cube_194 = 194,
        Text_Console_Colour_Cube_195 = 195,
        Text_Console_Colour_Cube_196 = 196,
        Text_Console_Colour_Cube_197 = 197,
        Text_Console_Colour_Cube_198 = 198,
        Text_Console_Colour_Cube_199 = 199,
        Text_Console_Colour_Cube_200 = 200,
        Text_Console_Colour_Cube_201 = 201,
        Text_Console_Colour_Cube_202 = 202,
        Text_Console_Colour_Cube_203 = 203,
        Text_Console_Colour_Cube_204 = 204,
        Text_Console_Colour_Cube_205 = 205,
        Text_Console_Colour_Cube_206 = 206,
        Text_Console_Colour_Cube_207 = 207,
        Text_Console_Colour_Cube_208 = 208,
        Text_Console_Colour_Cube_209 = 209,
        Text_Console_Colour_Cube_210 = 210,
        Text_Console_Colour_Cube_211 = 211,
        Text_Console_Colour_Cube_212 = 212,
        Text_Console_Colour_Cube_213 = 213,
        Text_Console_Colour_Cube_214 = 214,
        Text_Console_Colour_Cube_215 = 215,
        Text_Console_Colour_Cube_216 = 216,
        Text_Console_Colour_Cube_217 = 217,
        Text_Console_Colour_Cube_218 = 218,
        Text_Console_Colour_Cube_219 = 219,
        Text_Console_Colour_Cube_220 = 220,
        Text_Console_Colour_Cube_221 = 221,
        Text_Console_Colour_Cube_222 = 222,
        Text_Console_Colour_Cube_223 = 223,
        Text_Console_Colour_Cube_224 = 224,
        Text_Console_Colour_Cube_225 = 225,
        Text_Console_Colour_Cube_226 = 226,
        Text_Console_Colour_Cube_227 = 227,
        Text_Console_Colour_Cube_228 = 228,
        Text_Console_Colour_Cube_229 = 229,
        Text_Console_Colour_Cube_230 = 230,
        Text_Console_Colour_Cube_231 = 231,

        Text_Console_Colour_Grey_232 = 232,
        Text_Console_Colour_Grey_233 = 233,
        Text_Console_Colour_Grey_234 = 234,
        Text_Console_Colour_Grey_235 = 235,
        Text_Console_Colour_Grey_236 = 236,
        Text_Console_Colour_Grey_237 = 237,
        Text_Console_Colour_Grey_238 = 238,
        Text_Console_Colour_Grey_239 = 239,
        Text_Console_Colour_Grey_240 = 240,
        Text_Console_Colour_Grey_241 = 241,
        Text_Console_Colour_Grey_242 = 242,
        Text_Console_Colour_Grey_243 = 243,
        Text_Console_Colour_Grey_244 = 244,
        Text_Console_Colour_Grey_245 = 245,
        Text_Console_Colour_Grey_246 = 246,
        Text_Console_Colour_Grey_247 = 247,
        Text_Console_Colour_Grey_248 = 248,
        Text_Console_Colour_Grey_249 = 249,
        Text_Console_Colour_Grey_250 = 250,
        Text_Console_Colour_Grey_251 = 251,
        Text_Console_Colour_Grey_252 = 252,
        Text_Console_Colour_Grey_253 = 253,
        Text_Console_Colour_Grey_254 = 254,
        Text_Console_Colour_Grey_255 = 255,

        /* These values represent the chosen default colours. */
        Text_Console_Colour_Default_Foreground = 256,
        Text_Console_Colour_Default_Background = 257,
} text_console_colour_t;

// Visual console character attributes, stored for every screen and scrollback
// character.
#pragma pack(1)
typedef struct {
        // Bitmask of text_console_text_attrib_t
        uint8 attrib;

        // text_console_colour_t specifying fg colour
        uint16 fg;
        // text_console_colour_t specifying bg colour.
        uint16 bg;
} text_console_attrib_t;
#pragma pack()

/* This interface should be implemented by a text console frontend and defines
   the output part of the interaction between the text console (the backend)
   and this frontend. The input part is defined by the text_console_backend
   interface.

   A single frontend object can service multiple backend objects.

   Character and attribute data are encoded as two-dimensional arrays
   of bytes and text_console_attrib_t respectively. The width of these
   arrays is the console width, set in calls to set_size.

   As the text console screen is manipulated, the backend informs the frontend
   of changes. The important functions are set_contents, which changes existing
   parts of the screen, set_cursor_pos for moving the cursor within the screen,
   and append_text, which adds text to the bottom of the screen and implicitly
   moves all text upwards, hence increasing the size of the used scrollback.

   If the frontend has indicated that it is invisible, by calling
   iface.text_console_backend.set_visible, the backend may avoid calling
   set_contents, set_cursor_pos and append_text until the frontend indicates
   that it is visible again.
*/
SIM_INTERFACE(text_console_frontend) {
        /* Called when the console is finalised.
           'backend' is the calling text console. */
        int (*start)(conf_object_t *NOTNULL obj, conf_object_t *backend);

        /* Called when the console is being destroyed. */
        void (*stop)(conf_object_t *NOTNULL obj, int handle);

        /* Set the window title.
           Two title strings are given, where the long title is
           meant for a window title bar and the short for a window list.
           The long title string is at least as long as the short. */
        void (*set_title)(conf_object_t *NOTNULL obj, int handle,
                          const char *short_title, const char *long_title);

        /* Called when the screen size of the console changes.
           The width (columns) and height (rows) are given in characters. */
        void (*set_size)(conf_object_t *NOTNULL obj, int handle,
                         int width, int height);

        /* Specify the maximum scrollback size (in lines). */
        void (*set_max_scrollback_size)(conf_object_t *NOTNULL obj, int handle,
                                    int num_lines);

        /* Specify new default text colours for the console.
           These are used for characters whose colours were set to
           Text_Console_Colour_Default_Background or
           Text_Console_Colour_Default_Foreground. */
        void (*set_default_colours)(conf_object_t *NOTNULL obj, int handle,
                                    uint32 default_fg_col,
                                    uint32 default_bg_col);

        /* Show/hide the console window. */
        void (*set_visible)(conf_object_t *NOTNULL obj, int handle,
                            bool visible);

#ifndef PYWRAP
        /* Update characters and attributes in a rectangle in the console.
           (top, left) is the line and column of the upper left corner;
           (bottom, right) is the line and column of the lower right corner
                           (inclusive).
           Lines and columns are 0-based. Lines are counted from the top of
           the visible screen.

           'text' and 'attrib' are the text and attribute arrays, respectively,
           into which the lines and columns are used as indices.
           For example, the start of the rectangle in the text array is
           &text[width * top + left], where width was set by set_size. */
        void (*set_contents)(
                conf_object_t *NOTNULL obj, int handle,
                int top, int left, int bottom, int right,
                const char *text, const text_console_attrib_t *attrib);

        /* Move the cursor to a new position.
           row is the new cursor row (0-based), within the visible screen.
           col is the new cursor column (0-based). */
        void (*set_cursor_pos)(conf_object_t *NOTNULL obj, int handle,
                               int row, int col);

        /* Append text at the bottom of the screen of the console,
           implicitly scrolling the text upwards.
           num_lines is the number of lines to append_scroll.
           'text' and 'attrib' are the start of the newly scrolled-in data,
           representing the num_lines bottommost lines of the screen after
           scrolling. */
        void (*append_text)(
                conf_object_t *NOTNULL obj, int handle,
                int num_lines,
                const char *text, const text_console_attrib_t *attrib);

        /* Replace the screen and scrollback data with new information.
           'text' and 'attrib' each contain
             width * height elements,
            where width and height were set by set_size.
           'sb_text' and 'sb_attrib' each contain
             width * scrollback_size elements. */
        void (*refresh_screen)(
                conf_object_t *NOTNULL obj, int handle,
                const char *text, const text_console_attrib_t *attrib,
                const char *sb_text, const text_console_attrib_t *sb_attrib,
                int scrollback_size);
#endif
};
#define TEXT_CONSOLE_FRONTEND_INTERFACE "text_console_frontend"

/* Text console keys sent from the GUI to the console. */
typedef enum {
        Text_Console_Key_Tab = 9,
        Text_Console_Key_Return = 13,
        Text_Console_Key_Escape = 27,

        /* 32-126 are for (printable) ASCII characters. */

        /* Backspace has a special code (not 8) to distinguish it from ^H. */
        Text_Console_Key_Backspace = 128,

        Text_Console_Key_Left,
        Text_Console_Key_Up,
        Text_Console_Key_Right,
        Text_Console_Key_Down,

        Text_Console_Key_Home,
        Text_Console_Key_End,
        Text_Console_Key_Ins,
        Text_Console_Key_Del,
        Text_Console_Key_Pgup,
        Text_Console_Key_Pgdn,

        Text_Console_Key_F1,
        Text_Console_Key_F2,
        Text_Console_Key_F3,
        Text_Console_Key_F4,
        Text_Console_Key_F5,
        Text_Console_Key_F6,
        Text_Console_Key_F7,
        Text_Console_Key_F8,
        Text_Console_Key_F9,
        Text_Console_Key_F10,
        Text_Console_Key_F11,
        Text_Console_Key_F12,

        /* Numeric keypad keys. */
        Text_Console_Key_KP_0,
        Text_Console_Key_KP_1,
        Text_Console_Key_KP_2,
        Text_Console_Key_KP_3,
        Text_Console_Key_KP_4,
        Text_Console_Key_KP_5,
        Text_Console_Key_KP_6,
        Text_Console_Key_KP_7,
        Text_Console_Key_KP_8,
        Text_Console_Key_KP_9,

        Text_Console_Key_KP_Plus,
        Text_Console_Key_KP_Minus,
        Text_Console_Key_KP_Mul,
        Text_Console_Key_KP_Div,
        Text_Console_Key_KP_Dot,
        Text_Console_Key_KP_Enter,
} text_console_key_t;

// Text console input key modifiers.
typedef enum {
        Text_Console_Modifier_Alt   = 1,
        Text_Console_Modifier_Ctrl  = 2,
        Text_Console_Modifier_Shift = 4,
} text_console_modifier_t;

/* This interface is implemented by the text console (backend) and defines
   the input part of the interaction between the text console and a text
   console frontend. The output part is defined by the text_console_frontend
   interface.

   A text console frontend should use this interface to communicate with the
   backend, most importantly to pass on user input.
*/
SIM_INTERFACE(text_console_backend) {
        /* Send a key press to the backend. */
        void (*input)(conf_object_t *NOTNULL obj,
                      text_console_key_t key,
                      text_console_modifier_t modifiers);

        /* Request that the backend should refresh the whole console screen.
           Shortly after this call, the frontend can expect a call to
           text_console_frontend.refresh_screen. */
        void (*request_refresh)(conf_object_t *NOTNULL obj);

        /* Indicate to the backend whether the frontend is visible, e.g. if
           the console window is hidden by other windows or minimised.
           The backend may then choose not to call any text or cursor update
           functions in the text_console_frontend interface. */
        void (*set_visible)(conf_object_t *NOTNULL obj, bool visible);

        /* Returns 1 + the maximum column on the given line
           with a printable character. The frontend may use this information
           for e.g. user marking and copying text.
           line_num is 0-based and indexes over the screen for positive values
           and negative values to indicate scrollback lines. */
        int (*line_length)(conf_object_t *NOTNULL obj, int line_num);

        /* Indicate whether the specified line was not finished with an explicit
           carriage return by the user, and hence wraps to the next line.
           The frontend may use this information for e.g. user marking
           and copying text.
           line_num is 0-based and indexes over the screen for positive values
           and negative values to indicate scrollback lines. */
        bool (*line_wrap)(conf_object_t *NOTNULL obj, int line_num);

        /* Tell the console to change the screen size. Note that this is
           visible to the target software this call therefore impacts the
           simulation. An immediate call to text_console_frontend.set_size
           will follow a call to this function. */
        void (*set_size)(conf_object_t *NOTNULL obj, int width, int height);

        /* Tell the console to change the default text colours. An immediate
           call to text_console_frontend.set_default_colours will follow a
           call to this function. */
        void (*set_default_colours)(
                conf_object_t *NOTNULL obj,
                uint32 default_fg_col, uint32 default_bg_col);
};
#define TEXT_CONSOLE_BACKEND_INTERFACE "text_console_backend"

// Keyboard LED status bitmask.
typedef enum {
        Gfx_Console_Led_None    = 0,
        Gfx_Console_Led_Caps    = 1,
        Gfx_Console_Led_Num     = 2,
        Gfx_Console_Led_Scroll  = 4,
} gfx_console_led_t;

// Identifiers for graphics console mouse input.
typedef enum {
        Gfx_Console_Mouse_Button_Left = MOUSE_LEFT_DOWN,
        Gfx_Console_Mouse_Button_Right = MOUSE_RIGHT_DOWN,
        Gfx_Console_Mouse_Button_Middle = MOUSE_MIDDLE_DOWN,
} gfx_console_mouse_button_t;

/* This interface should be implemented by a graphics console frontend and
   defines the output part of the interaction between the graphics console (the
   backend) and this frontend. The input part is defined by the
   gfx_console_backend interface.

   A single frontend object can service multiple backend objects.

   The backend typically calls set_contents with a certain real-time frequency,
   such as 50 Hz. If the frontend has indicated that it is invisible, using
   iface.gfx_console_backend.set_visible, then the backend may stop these calls.

   The graphics console can be in either graphics or text mode, corresponding to
   different VGA modes. To facilitate the frontend implementing text mark and
   copy when in text mode, the backend sends the text data using the set_text
   function.

   When the graphics console is in text mode, the backend will call set_text
   after each call to set_contents, to notify the frontend about the text data.

   When the backend receives characters from the associated graphics device
   using the vga_text_update interface, it will also notify the frontend using
   the signal_text_update function. This will always be called even if the
   frontend is invisible, which facilitates some form of activity indicator in
   the frontend.

   The frontend can assume that the console is in text mode after a call to
   set_text, until the next call to set_size.

   Depending on the configuration, the graphics console should send absolute or
   relative mouse coordinates to attached devices. In the latter case, the
   backend normally ignores mouse input from the frontend, since the host and
   target mouse pointer positions may not match. When a certain "grab" key
   combination is pressed, it starts processing mouse events, and informs the
   frontend using set_grab_mode.

   When grab mode is active, the frontend should hide the host mouse cursor
   and capture the mouse (receive all mouse input from the host system).
   In this situation, the backend may also call set_mouse_pos to explicitly
   move the mouse. */
SIM_INTERFACE(gfx_console_frontend) {
        // Called when the backend is finalised. Should return a unique handle
        // identifying the given backend object to the frontend.
        int (*start)(conf_object_t *NOTNULL obj, conf_object_t *backend);

        // Called when the backend is being destroyed.
        void (*stop)(conf_object_t *NOTNULL obj, int handle);

        // Set window title of the frontend associated to the specified backend.
        // Two title strings are given, where the long title is
        // meant for a window title bar and the short for a window list.
        // The long title string is at least as long as the short.
        void (*set_title)(conf_object_t *NOTNULL obj,
                          int handle, const char *short_title,
                          const char *long_title);

        // Called when the screen size of the specified backend changes.
        void (*set_size)(conf_object_t *NOTNULL obj, int handle,
                         // Width and height in pixels.
                         int width, int height);

        // Tell the frontend to show/hide its GUI window.
        void (*set_visible)(conf_object_t *NOTNULL obj, int handle,
                            bool visible);
#ifndef PYWRAP
        /* Update the screen contents in the rectangle between (left, top)
           and (right, bottom), inclusive.
           'data' is the start of the screen pixel array, so the first
           pixel of the rectangle is at &data[width * top + left],
           where width was set by set_size. */
        void (*set_contents)(
                conf_object_t *NOTNULL obj, int handle,
                int left, int top, int right, int bottom,
                const uint32 *data);

        // Tell the frontend that the data argument passed to set_contents
        // is no longer valid.
        // This must be called upon a screen size change, before set_size.
        void (*invalidate_contents)(conf_object_t *NOTNULL obj, int handle);

        // Inform the frontend whether or not we are currently in VGA text mode.
        void (*set_text_mode)(conf_object_t *NOTNULL obj, int handle,
                              bool text_mode);
#endif
        // Tell the frontend that text arrived to the backend from the attached
        // device, using vga_text_update.
        void (*signal_text_update)(conf_object_t *NOTNULL obj, int handle);

        // Inform the frontend that the grab mode of the specified backend
        // has changed.
        void (*set_grab_mode)(conf_object_t *NOTNULL obj,
                              int handle, bool active);

        /* Tell the frontend to set the host mouse cursor to (x, y),
           with ranges [0, width - 1] and [0, height - 1], where
           width and height were set by set_size. */
        void (*set_mouse_pos)(conf_object_t *NOTNULL obj, int handle,
                              int x, int y);

        /* Inform frontend that the keyboard LED state have been updated.
           The new state of the LEDs is given as a bitmask. */
        void (*set_keyboard_leds)(conf_object_t *NOTNULL obj, int handle,
                                  gfx_console_led_t led_state);

        /* Inform frontend that the grab modifier has changed. */
        void (*set_grab_modifier)(conf_object_t *NOTNULL obj, int handle,
                                  sim_key_t modifier);

        /* Inform frontend that the grab button has changed. */
        void (*set_grab_button)(conf_object_t *NOTNULL obj, int handle,
                                gfx_console_mouse_button_t button);
};
#define GFX_CONSOLE_FRONTEND_INTERFACE "gfx_console_frontend"

// VGA text mode data.
// Stored in this file since we have the VGA device pointer here.
typedef struct {
        /* Screen text (text_width × text_height), in some 8-bit ASCII
           superset (we assume CP437). */
        bytes_t text;
        int rows, columns;            /* Text screen size in character cells. */
        int font_width, font_height;  /* Text cell size in pixels. */

        /* Length of each line (text_height elements), beyond which there
           is only empty space. Each element is in [0, columns]. */
        bytes_t line_lengths;
} gfx_console_screen_text_t;

/* This interface is implemented by the graphics console (backend) and defines
   the input part of the interaction between the graphics console and a graphics
   console frontend. The output part is defined by the gfx_console_frontend
   interface.

   A graphics console frontend should use this interface to communicate with the
   backend, most importantly to pass on user input. */
SIM_INTERFACE(gfx_console_backend) {
        // Indicate to the backend that a key has been pressed or released.
        void (*kbd_event)(conf_object_t *NOTNULL obj,
                          sim_key_t code, bool down);

        /* Indicate to the backend that the mouse state has changed.
           (x, y) are in screen coordinates (pixels).
           z is the mouse wheel: 1 if the wheel has rolled upwards, -1 for
           downwards, 0 for no change.
           'buttons' is the current button state. */
        void (*mouse_event)(
                conf_object_t *NOTNULL obj, int x, int y, int z,
                gfx_console_mouse_button_t buttons);

        // Request that the backend should refresh the whole console screen.
        // Shortly after this call, the frontend can expect a call to
        // gfx_console_frontend.set_contents, updating the whole screen.
        void (*request_refresh)(conf_object_t *NOTNULL obj);

        // Indicate to the backend whether the frontend is visible, e.g. if
        // the console window is hidden by other windows or minimised.
        // If the frontend is invisible, the backend may choose not to call
        // gfx_console_frontend.set_contents.
        void (*set_visible)(conf_object_t *NOTNULL obj, bool visible);

        // Obtain VGA text data from console backend. If the console is not in
        // VGA text mode, the result is undefined.
        gfx_console_screen_text_t (*text_data)(conf_object_t *NOTNULL obj);

        // Inform the backed that the grab mode keys were pressed.
        void (*got_grab_keys)(conf_object_t *NOTNULL obj);
};
#define GFX_CONSOLE_BACKEND_INTERFACE "gfx_console_backend"
// ADD INTERFACE gfx_console_backend_interface


/* Below are internal data structures and interfaces used by the
   Simics GUI console frontends. */

// Possible update messages that can be sent to Winsome in a winsome_update_t.
typedef enum {
        Winsome_Update_Nothing = 0,
        Winsome_Update_Text_Cursor = 1,
        Winsome_Update_Append = 2,
        Winsome_Update_Refresh = 3,
        Winsome_Update_Gfx = 4,
        Winsome_Update_Resize = 5,
        Winsome_Update_Activity = 6,
} winsome_update_type_t;

/* Main structure for sending console updates to the Winsome GUI.
   This is converted to a Python tuple with data corresponding to the
   different parts of the union. Data buffers such as screen and attribute data
   are converted to Python strings. */
typedef struct {
        // Indicates which part of the union is used.
        winsome_update_type_t type;
        union {
                /* Data for a message containing text console screen update
                   and cursor movement. This corresponds to calls to
                   set_contents and set_cursor_pos in text_console_frontend,
                   accumulated in the obvious way: take the bounding box of
                   the updated rectangles and the last cursor position. */
                struct {
                        // Heap-allocated contents of new screen rectangle.
                        // Each array has
                        // (right - left + 1) * (bottom - top + 1)
                        // elements.
                        char *text;
                        text_console_attrib_t *attrib;

                        // Coordinates on screen of new data.
                        int left;
                        int right;
                        int top;
                        int bottom;

                        // New cursor position.
                        int cursor_x;
                        int cursor_y;
                } text;
                /* Data for a message containing text console append and cursor
                   movement. This corresponds to calls to append_text and
                   set_cursor_pos in text_console_frontend, accumulated in the
                   obvious way: append the text lines and take the last cursor
                   position. */
                struct {
                        // Number of lines to append.
                        uint64 num_lines;

                        // Pointers to start of text at new lines.
                        char *text;
                        text_console_attrib_t *attrib;

                        // Length of buffers in characters.
                        uint64 text_len;

                        // New cursor position.
                        int cursor_x;
                        int cursor_y;
                } sb;
                /* Data for a message containing text console screen refresh.
                   This corresponds to calls to refresh_screen
                   in text_console_frontend. */
                struct {
                        // Heap-allocated contents of visible screen rectangle.
                        // Each array has width * height elements.
                        char *text;
                        text_console_attrib_t *attrib;

                        // Heap-allocated contents of scrollback.
                        // Each array has width * sb_size elements.
                        char *sb_text;
                        text_console_attrib_t *sb_attrib;

                        // Screen size.
                        int width;
                        int height;

                        // Number of scrollback lines.
                        int sb_size;

                        // New cursor position.
                        int cursor_x;
                        int cursor_y;
                } refresh;
                /* Data for a message containing text console screen resize.
                   This corresponds to calls to set_size
                   in text_console_frontend. */
                struct {
                        // New screen size.
                        int width;
                        int height;
                } resize;
                /* Data for a message containing graphics console updates.
                   This corresponds to calls to set_contents in
                   gfx_console_frontend, accumulated in the
                   obvious way: take the bounding box of the updated
                   rectangles. */
                struct {
                        // New pixels for the dirty rectangle (owned).
                        uint32 *data;

                        // Dirty rectangle
                        int left;
                        int right;
                        int top;
                        int bottom;

                        // Current/new screen size
                        int width;
                        int height;

                        // Are we currently in VGA text mode?
                        bool text_mode;
                } gfx;
        } u;
} winsome_update_t;

/* Interface implemented by the Python side of the Winsome console frontends.
   Used by the C side to send console updates.

   The C side of the Winsome console frontend maintains a queue of console
   updates coming from the console backend. The updates are sent to the
   Python side as soon as it can. */
SIM_INTERFACE(winsome_console) {
        void (*gfx)(
                conf_object_t *NOTNULL obj, lang_void *console,
                // Dirty rectangle
                int left, int right, int top, int bottom,
                // New pixels for the dirty rectangle (owned).
                bytes_t data,
                // Are we currently in VGA text mode?
                bool text_mode);
        void (*resize)(
                conf_object_t *NOTNULL obj, lang_void *console,
                // New screen size
                int width, int height);
        void (*refresh)(
                conf_object_t *NOTNULL obj, lang_void *console,
                // Heap-allocated contents of visible screen rectangle.
                // Each array has width * height elements.
                bytes_t text, bytes_t attrib,
                // Heap-allocated contents of scrollback.
                // Each array has width * sb_size elements.
                bytes_t sb_text, bytes_t sb_attrib,
                // Number of scrollback lines.
                int sb_size,
                // New cursor position.
                int cursor_x, int cursor_y);
        void (*append)(
                conf_object_t *NOTNULL obj, lang_void *console,
                // Number of lines to append.
                uint64 num_lines,
                // Pointers to start of text at new lines.
                bytes_t text, bytes_t attrib,
                // New cursor position.
                int cursor_x, int cursor_y);
        void (*text)(
                conf_object_t *NOTNULL obj, lang_void *console,
                // Coordinates on screen of new data.
                int left, int right, int top, int bottom,
                // Heap-allocated contents of new screen rectangle.
                // Each array has
                // (right - left + 1) * (bottom - top + 1)
                // elements.
                bytes_t text, bytes_t attrib,
                // New cursor position.
                int cursor_x, int cursor_y);
        void (*activity)(
                conf_object_t *NOTNULL obj, lang_void *console);
};
#define WINSOME_CONSOLE_INTERFACE "winsome_console"

/* Interface implemented by C backend of console GUI. The Python side uses this
   to inform the backend when the GUI is ready/shutdown. */
SIM_INTERFACE(gui_console_backend) {
        void (*start)(conf_object_t *NOTNULL obj, lang_void *console);
        void (*stop)(conf_object_t *NOTNULL obj);
};
#define GUI_CONSOLE_BACKEND_INTERFACE "gui_console_backend"

EXPORTED int VT_get_hide_consoles_flag(void);

#if defined(__cplusplus)
}
#endif

#endif
