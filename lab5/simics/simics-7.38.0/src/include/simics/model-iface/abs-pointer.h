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

#ifndef SIMICS_MODEL_IFACE_ABS_POINTER_H
#define SIMICS_MODEL_IFACE_ABS_POINTER_H

#include <simics/pywrap.h>
#include <simics/base-types.h>
#include <simics/base/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add id="abs_pointer_interface_t">

   Interface implemented by tablet devices. Used by consoles to send touchpad
   events to the controller. The <fun>set_state</fun> function is called when
   something changes in the console. The coordinates are given as scaled
   absolute scaled values, where (0, 0) is the upper-left corner and (0xffff,
   0xffff) is the lower-right corner.

   <insert-until text="// ADD INTERFACE abs_pointer_interface"/>

   </add>
   <add id="abs_pointer_interface_exec_context">
   Cell Context for all methods.
   </add>
*/
typedef enum {
        Abs_Pointer_Button_Left   = 0x20,
        Abs_Pointer_Button_Right  = 0x10,
        Abs_Pointer_Button_Middle = 0x08
} abs_pointer_buttons_t;

typedef struct {
        abs_pointer_buttons_t buttons;
        uint16 x;
        uint16 y;
        uint16 z;
} abs_pointer_state_t;

SIM_INTERFACE(abs_pointer) {
        void (*set_state)(conf_object_t *obj, abs_pointer_state_t state);
};

#define ABS_POINTER_INTERFACE "abs_pointer"
// ADD INTERFACE abs_pointer_interface

/* <add id="abs_pointer_activate_interface_t">

   Interface used by tablet controllers to temporary turn off and on the
   tracking of absolute pointer locations. Implemented by consoles. When
   disabled, no calls will be made to the controller's
   <iface>abs_pointer</iface> interface.

   <insert-until text="// ADD INTERFACE abs_pointer_activate_interface"/>

   </add>
   <add id="abs_pointer_activate_interface_exec_context">
   Cell Context for all methods.
   </add>
*/

SIM_INTERFACE(abs_pointer_activate) {
        void (*enable)(conf_object_t *obj);
        void (*disable)(conf_object_t *obj);
};

#define ABS_POINTER_ACTIVATE_INTERFACE "abs_pointer_activate"
// ADD INTERFACE abs_pointer_activate_interface

#if defined(__cplusplus)
}
#endif

#endif
