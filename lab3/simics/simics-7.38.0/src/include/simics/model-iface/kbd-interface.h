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

#ifndef SIMICS_MODEL_IFACE_KBD_INTERFACE_H
#define SIMICS_MODEL_IFACE_KBD_INTERFACE_H

#include <simics/base/types.h>
#include <simics/base/attr-value.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add id="keyboard_interface_t">

   Interface implemented by keyboard controllers. Used by consoles to send
   keyboard events to the controller.

   The function <fun>keyboard_event()</fun> takes the keyboard controller as
   its first argument <arg>obj</arg>. The <arg>key_up</arg> argument specifies
   whether the event is a key release (1) or a key press (0). The
   <arg>key</arg> argument is the Simics internal keycode, as defined in
   the <type>sim_key_t</type> enum.

   If the return value is 1 the keyboard controller accepted the event. If
   return value is 0 the keyboard controller did not accept the event, and the
   console should buffer the event until it gets a <fun>keyboard_ready()</fun>
   call from the keyboard controller.

   <insert-until text="// ADD INTERFACE keyboard_interface"/>

   </add>
   <add id="keyboard_interface_exec_context">
   Cell Context for all methods.
   </add>
*/

SIM_INTERFACE(keyboard) {
        int (*keyboard_event)(conf_object_t *obj, int key_up, uint8 key);
};

#define KEYBOARD_INTERFACE "keyboard"
// ADD INTERFACE keyboard_interface

/* <add id="keyboard_console_interface_t">

   Interface implemented by consoles, to receive notifications from keyboard
   controllers.

   The function <fun>keyboard_ready()</fun>, which takes the console as its
   first argument <arg>obj</arg>, must be called by the keyboard controller
   when it is ready to receive keyboard events again after having rejected a
   keyboard event. Note that this function may be called even though no
   keyboard event has been rejected, and that the console must not assume that
   keyboard controller will accept an event just because the
   <fun>keyboard_ready()</fun> function has been called.

   <fun>keyboard_ready</fun> must not be called while the keyboard controller
   is handling a <fun>keyboard_event()</fun> call.

   <insert-until text="// ADD INTERFACE keyboard_console_interface"/>
   </add>
   <add id="keyboard_console_interface_exec_context">
   Cell Context for all methods.
   </add>
*/

SIM_INTERFACE(keyboard_console) {
        void (*keyboard_ready)(conf_object_t *obj);
};

#define KEYBOARD_CONSOLE_INTERFACE "keyboard_console"
// ADD INTERFACE keyboard_console_interface

/* Having interfaces with different name and typename is no longer supported,
   but we have to keep this for compatibility.
*/
SIM_INTERFACE(kbd_console) {
        void (*keyboard_ready)(conf_object_t *obj);
};

/* <add id="mouse_interface_t">
   Interface used to send mouse events to a mouse device. The function
   <fun>mouse_event()</fun> takes the destination device as first argument
   in <arg>obj</arg>. The <arg>xmicro</arg> and <arg>ymicro</arg> arguments
   specified the relative mouse movement in micro-meters. If the mouse
   supports a wheel, the wheel movement is supplied in <arg>z</arg>, as
   number of steps up or down. The last argument <arg>buttons</arg> is a
   bit-mask with the state of the mouse buttons. The mapping of mouse
   buttons to bits is defined in the header file
   <file>&lt;simics/model-iface/sim-keys.h&gt;</file>.

   <insert-until text="// ADD INTERFACE mouse_interface"/>

   </add>
   <add id="mouse_interface_exec_context">
   Cell Context for all methods.
   </add>
*/

SIM_INTERFACE(mouse) {
        void (*mouse_event)(conf_object_t *obj,
                            int xmicro,
                            int ymicro,
                            int z,
                            int buttons);
};

#define MOUSE_INTERFACE "mouse"
// ADD INTERFACE mouse_interface

#if defined(__cplusplus)
}
#endif

#endif
