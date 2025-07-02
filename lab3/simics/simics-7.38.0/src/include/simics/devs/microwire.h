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

#ifndef SIMICS_DEVS_MICROWIRE_H
#define SIMICS_DEVS_MICROWIRE_H

#include <simics/base/types.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add id="microwire_interface_t">
   This interface is used to communicate with a 3-wire
   (microwire) serial EEPROM device, for example, the 93Cxx
   series, via its pins. To set the values of the CS, SK, and
   DI pins, use the <fun>set_cs()</fun>, <fun>set_sk()</fun>,
   and <fun>set_di()</fun> functions, respectively. To read the
   value output on the DO pin, use the <fun>get_do()</fun>
   function. Zero represents low signal and non-zero high
   signal. The <fun>read_word()</fun> and <fun>write_word()</fun>
   functions are provided to shortcut high-level operations.
   They operate on 8-bit or 16-bit entities depending on the
   width of the eeprom.

   <insert-until text="// END INTERFACE microwire"/>

   </add>
   <add id="microwire_interface_exec_context">
   Cell Context for all methods.
   </add> */

#define MICROWIRE_INTERFACE "microwire"

SIM_INTERFACE(microwire) {
        void (*set_cs)(conf_object_t *obj, int cs);
        void (*set_sk)(conf_object_t *obj, int sk);
        void (*set_di)(conf_object_t *obj, int di);
        int  (*get_do)(conf_object_t *obj);
        uint16 (*read_word)(conf_object_t *obj, int offset);
        void (*write_word)(conf_object_t *obj, int offset, uint16 value);
};

// END INTERFACE microwire

#if defined(__cplusplus)
}
#endif

#endif
