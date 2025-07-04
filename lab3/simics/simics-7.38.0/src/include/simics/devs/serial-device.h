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

#ifndef SIMICS_DEVS_SERIAL_DEVICE_H
#define SIMICS_DEVS_SERIAL_DEVICE_H

#include <simics/base/types.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add id="serial_device_interface_t">

   Interface used to connect serial devices together. It can be implemented
   both by devices such as UARTs and text consoles, and by links.

   A character (byte) is sent with the <fun>write()</fun> function;
   <arg>obj</arg> is the receiving device or link, and <arg>value</arg> is
   either a data byte, or the out-of-band value <tt>TTY_ABORT</tt>.

   The receiver will return the number of characters accepted; i.e. 1 on
   success and 0 if it could not handle the new byte. If the receiver returns
   0, it must later call <fun>receive_ready()</fun> in the sender's
   <iface>serial_device</iface> interface to signal that new bytes can now be
   accepted. A serial device must handle the case where the
   <fun>receive_ready()</fun> function is called although it has no more bytes
   to send.

   <insert-until text="// ADD INTERFACE serial_device_interface"/>

   </add>
   <add id="serial_device_interface_exec_context">
   Cell Context for all methods.
   </add>
*/

#define TTY_ABORT   0x100

SIM_INTERFACE(serial_device) {
        int (*write)(conf_object_t *obj, int value);
        void (*receive_ready)(conf_object_t *obj);
};

#define SERIAL_DEVICE_INTERFACE "serial_device"
// ADD INTERFACE serial_device_interface

/* <add id="extended_serial_interface_t">
   This interface extends the <iface>serial_device</iface> with
   a <fun>write_at()</fun> function. It is similar to the <fun>write()</fun>
   function of the mentioned interface, but accepts an on-screen character
   position. This interface is implemented by text consoles allowing them
   to be connected to text oriented frame buffers, such as VGA in text mode.

   <insert-until text="// ADD INTERFACE extended_serial_interface"/>

   </add>
   <add id="extended_serial_interface_exec_context">
   Cell Context for all methods.
   </add>
*/
SIM_INTERFACE(extended_serial) {
        void (*write_at)(conf_object_t *obj,
                         int value, int x, int y, int fg, int bg);
        void (*graphics_mode)(conf_object_t *obj, int in_graphics_mode);
};

#define EXTENDED_SERIAL_INTERFACE "extended_serial"
// ADD INTERFACE extended_serial_interface


/* <add id="rs232_device_interface_t">
   Currently Simics internal.

   <insert-until text="// ADD INTERFACE rs232_device_interface"/>

   </add>
   <add id="rs232_device_interface_exec_context">
   Cell Context for all methods.
   </add>
*/
SIM_INTERFACE(rs232_device) {
        /* Flow control lines */
        void (*cts)(conf_object_t *obj, int status);
        void (*dsr)(conf_object_t *obj, int status);

        /* Ring indicator */
        void (*ring)(conf_object_t *obj, int status);

        /* Carrier detect */
        void (*carrier)(conf_object_t *obj, int status);

        /* Break */
        void (*got_break)(conf_object_t *obj);

        /* Frame error */
        void (*got_frame_error)(conf_object_t *obj);
};

#define RS232_DEVICE_INTERFACE "rs232_device"
// ADD INTERFACE rs232_device_interface

/* <add id="rs232_console_interface_t">
   Currently Simics internal.

   <insert-until text="// ADD INTERFACE rs232_console_interface"/>

   </add>
   <add id="rs232_console_interface_exec_context">
   Cell Context for all methods.
   </add>
*/

typedef enum {
        Stop_Bits_1,
        Stop_Bits_1p5,
        Stop_Bits_2
} stop_bits_t;

typedef enum {
        Parity_None,
        Parity_Even,
        Parity_Odd
} parity_mode_t;


SIM_INTERFACE(rs232_console) {
        /* Set line parameters */
        void (*set_baudrate)(conf_object_t *obj, int rate, int commit);
        void (*set_data_bits)(conf_object_t *obj, int bits, int commit);
        void (*set_stop_bits)(conf_object_t *obj, stop_bits_t stop_bits,
                              int commit);
        void (*set_parity_mode)(conf_object_t *obj, parity_mode_t parity_mode,
                                int commit);

        /* Flow control lines */
        void (*set_dtr)(conf_object_t *obj, int status);
        void (*set_rts)(conf_object_t *obj, int status);

        /* Break */
        void (*set_break)(conf_object_t *obj, int status);
};

#define RS232_CONSOLE_INTERFACE "rs232_console"
// ADD INTERFACE rs232_console_interface

/* obsolete out-out-band characters */
#define TTY_NO_CHAR 0x101

#if defined(__cplusplus)
}
#endif

#endif
