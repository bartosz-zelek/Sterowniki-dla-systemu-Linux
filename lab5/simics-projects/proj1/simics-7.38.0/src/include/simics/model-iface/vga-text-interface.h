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

#ifndef SIMICS_MODEL_IFACE_VGA_TEXT_INTERFACE_H
#define SIMICS_MODEL_IFACE_VGA_TEXT_INTERFACE_H

#include <simics/pywrap.h>
#include <simics/base-types.h>
#include <simics/base/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

SIM_INTERFACE(vga_text) {
        int (*add_string_notification)(conf_object_t *obj, char *substring,
                                       double sample_interval);
};
#define VGA_TEXT_INTERFACE "vga_text"

/*
   <add id="vga_text_info_interface_t">

   The <iface>vga_text_info</iface> interface facilitates the graphics console
   to obtain information about displayed text from its attached VGA device,
   when the video mode is a VGA text mode. This interface must be implemented
   by VGA devices that are attached to the graphics console.

   The <fun>text_mode</fun> method indicates whether the current video mode is
   a VGA text mode.

   If the current video mode is not a VGA text mode, all other methods have no
   effect, and will return <tt>false</tt>. Otherwise they return <tt>true</tt>
   and behaves as follows:

   The <fun>font_size</fun> method sets <arg>width</arg> and <arg>height</arg>
   to the current font size.

   The <fun>screen_size</fun> method sets <arg>columns</arg> and
   <arg>rows</arg> to the current screen size.

   The <fun>text</fun> method retrieves the screen text data and line
   lengths. The <arg>text</arg> parameter must be a buffer with size at least
   <tt>columns * rows</tt>, as given by <fun>screen_size</fun>. Similarly, the
   <arg>line_length</arg> parameter must be a buffer of length <tt>rows</tt>.

   <insert-until text="// ADD INTERFACE vga_text_info_interface_t"/>
   </add>

   <add id="vga_text_info_interface_exec_context"> Cell Context
   for all methods.
   </add>
*/
SIM_INTERFACE(vga_text_info) {
        bool (*text_mode)(conf_object_t *NOTNULL obj);
        bool (*font_size)(conf_object_t *NOTNULL obj, int *width, int *height);
        bool (*screen_size)(conf_object_t *NOTNULL obj,
                            int *columns, int *rows);
        bool (*text)(conf_object_t *NOTNULL obj,
                     uint8 *text, uint8 *line_lengths);
};
#define VGA_TEXT_INFO_INTERFACE "vga_text_info"
// ADD INTERFACE vga_text_info_interface_t

/*
   <add id="vga_text_update_interface_t">

   The <iface>vga_text_update</iface> interface facilitates defining an input
   character stream for graphics consoles. The video device associated to a
   graphics console can use this interface to send a stream of characters to
   the console. This stream is used by the graphics console break strings. The
   stream should match the displayed VGA text whenever possible.

   <insert-until text="// ADD INTERFACE vga_text_update_interface_t"/>
   </add>

   <add id="vga_text_update_interface_exec_context"> Cell Context
   </add>
*/
SIM_INTERFACE(vga_text_update) {
        void (*write)(conf_object_t *NOTNULL obj, char value);
};
#define VGA_TEXT_UPDATE_INTERFACE "vga_text_update"
// ADD INTERFACE vga_text_update_interface_t

#if defined(__cplusplus)
}
#endif

#endif
