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

#ifndef SIMICS_MODEL_IFACE_GFX_CONSOLE_H
#define SIMICS_MODEL_IFACE_GFX_CONSOLE_H

#include <simics/base/types.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/*
   <add id="vga_update_interface_t">

   The <iface>vga_update</iface> interface facilitates the graphics console to
   request screen redraw from its attached video device, which typically
   happens on every frame update event. This interface must be implemented by
   video devices that are attached to the graphics console. The implementation
   should call functions in the <iface>gfx_con</iface> interface, e.g. a
   sequence of put_block() calls followed by redraw(), which is implemented by
   the graphics console.

   The <fun>refresh</fun> method requests the video device to redraw dirty
   parts of the screen. The <fun>refresh_all</fun> method requests the video
   device to redraw the whole screen.

   <insert-until text="// ADD INTERFACE vga_update_interface_t"/>
   </add>

   <add id="vga_update_interface_exec_context"> Cell Context for
   all methods
   </add>
*/
SIM_INTERFACE(vga_update) {
        void (*refresh)(conf_object_t *NOTNULL obj);
        void (*refresh_all)(conf_object_t *NOTNULL obj);
};
#define VGA_UPDATE_INTERFACE "vga_update"
// ADD INTERFACE vga_update_interface_t

/* <add id="gfx_con_pixel_fmt_t DOC">
   <ndx>gfx_con_pixel_fmt_t</ndx>
   <name index="true">gfx_con_pixel_fmt_t</name>
   <doc>

   <doc-item name="NAME">gfx_con_pixel_fmt_t</doc-item>

   <doc-item name="DESCRIPTION"> These constants are used when calling the
   <fun>put_block</fun> method in the <iface>gfx_con</iface>, to specify the
   pixel format of the provided buffer.

   Specifying <tt>GFX_8BIT_INDEXED</tt> means that the buffer uses VGA style
   8-bit per pixel indexed colors, and that the console palette, specified by
   the <fun>set_color</fun> method, should be used.

   Specifying <tt>GFX_RGB_565</tt> means that the buffer uses 16 bits per
   pixel, with format <tt>0bRRRRRGGGGGGBBBBB</tt> (in little endian).

   Specifying <tt>GFX_RGB_888</tt> means that the buffer uses 24 bits per
   pixel, with format <tt>0xRRGGBB</tt> (in little endian).

   Specifying <tt>GFX_xRGB_8888</tt> means that the buffer uses 32 bits per
   pixel, with format <tt>0xXXRRGGBB</tt> (in little endian), where <tt>X</tt>
   is unused.
   </doc-item>
   </doc></add>
*/
typedef enum {
        GFX_8BIT_INDEXED = 8,       /* 0xII */
        GFX_RGB_565      = 16,      /* 0bRRRRRGGGGGGBBBBB */
        GFX_RGB_888      = 24,      /* 0xRRGGBB */
        GFX_xRGB_8888    = 32,      /* 0xXXRRGGBB (X=unused) */
} gfx_con_pixel_fmt_t;

/*
   <add id="gfx_con_interface_t">

   The <iface>gfx_con</iface> is implemented by graphics consoles of class
   <class>graphcon</class> and is used by attached video devices to update the
   screen.

   The <fun>set_color</fun> method sets the specified indexed palette
   color. The return value has no meaning.

   The <fun>set_size</fun> method sets the size of the displayed screen.

   The <fun>put_pixel</fun>method sets the pixel at (<arg>x</arg>,
   <arg>y</arg>) to the specified palette color. The change will only be
   visible after the next call to <fun>redraw</fun>.

   The <fun>put_pixel_rgb</fun> method sets the pixel at (<arg>x</arg>,
   <arg>y</arg>) to the color <arg>rgb</arg>, which should be in
   <tt>GFX_xRGB_8888</tt> format. The change will only be visible after the
   next call to <fun>redraw</fun>.

   The <fun>put_pixel_col</fun> method sets the pixel at (<arg>x</arg>,
   <arg>y</arg>) to the color defined by <arg>r</arg>, <arg>g</arg> and
   <arg>b</arg>. The change will only be visible after the next call to
   <fun>redraw</fun>.

   The <fun>put_block</fun> method draws pixels from a memory block to the
   screen rectangle described by (<arg>minx</arg>, <arg>miny</arg>,
   <arg>maxx</arg>, <arg>maxy</arg>); the upper bounds are inclusive. The
   <arg>block</arg> parameter refers to the source memory block, whose rows
   should be <arg>src_stride</arg> bytes long. Memory on each row outside the
   rectangle is not accessed. The <arg>src_fmt</arg> parameter specifies the
   pixel format in <arg>block</arg>. The change will only be visible
   after the next call to <fun>redraw</fun>.

   The <fun>redraw</fun> method updates the changed parts of the console screen
   window, so should typically be called at the end of every frame update.

   The <fun>update_keyboard_leds</fun> method changes the keyboard LEDs.
   The <arg>led_change</arg> parameter must be one of the <tt>KBD_</tt>
   constants from <file>simics/model-iface/sim-keys.h</file>.

   <insert-until text="// ADD INTERFACE gfx_con_interface_t"/>
   </add>

   <add id="gfx_con_interface_exec_context"> Cell Context for all methods
   </add>
*/
SIM_INTERFACE(gfx_con) {
        int (*set_color)(conf_object_t *obj,
                         uint8 index, uint8 r, uint8 g, uint8 b);
        void (*set_size)(conf_object_t *obj, int width, int height);
        void (*put_pixel)(conf_object_t *obj, int x, int y, uint8 index);
        void (*put_pixel_rgb)(conf_object_t *obj, int x, int y, uint32 rgb);
#ifndef PYWRAP
        void (*put_block_old)(conf_object_t *obj,
                              uint8 *src, int minx, int miny,
                              int maxx, int maxy, int src_fmt,
                              int src_stride, int unused);
#endif
        void (*redraw)(conf_object_t *obj);
        void (*update_keyboard_leds)(conf_object_t *obj, int led_change);
        void (*put_pixel_col)(conf_object_t *obj, int x, int y,
                              int r, int g, int b);
        void (*put_block)(conf_object_t *obj, bytes_t block, int minx, int miny,
                          int maxx, int maxy, gfx_con_pixel_fmt_t src_fmt,
                          int src_stride);
};

#define GFX_CON_INTERFACE "gfx_con"
// ADD INTERFACE gfx_con_interface_t

#if defined(__cplusplus)
}
#endif

#endif /* _GFX_CONSOLE_H */
