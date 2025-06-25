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

#ifndef SIMICS_DEVS_ARINC_429_H
#define SIMICS_DEVS_ARINC_429_H

/* Arinc-429 is a two-wire serial protocol used in avionics. The bus
   consists of one transmitter and 1-20 receivers. The rate is either
   12.5 or 100 kbps. The lines have three states: NULL, high, low, and
   is self-clocking.
   Each data packet is 32 bits. Bit 31 is the parity bit.
   The whole word should be of odd parity. The rest of the data is
   up to the sensor type, a list of sensors and the data encoding can
   be found at
   http://www.condoreng.com/support/downloads/tutorials/ARINCTutorial.pdf
*/

#include <simics/pywrap.h>
#include <simics/base/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add id="arinc429_bus_interface_t">
   <insert-until text="// ADD INTERFACE arinc429_bus_interface"/>

   Interface to Arinc-429 serial buses.

   The <fun>send_word</fun> function is used by a device to send an
   Arinc-429 formatted word. The most significant bit (bit 31) is the
   parity bit, but if the device have checksumming built in, it can
   set <param>parity_ok</param> to tell the bus to ignore bit 31 of the data.

   If the parity bit is already calculated, pass -1 as <param>parity_ok</param>.

   </add>

   <add id="arinc429_bus_interface_exec_context">
   Cell Context for all methods.
   </add>
*/

SIM_INTERFACE(arinc429_bus) {
    void (*send_word)(conf_object_t *bus, uint32 word, int parity_ok);
};
// ADD INTERFACE arinc429_bus_interface

/* <add id="arinc429_receiver_interface_t">
   <insert-until text="// ADD INTERFACE arinc429_receiver_interface"/>

   Interface to Arinc-429 compatible receivers.

   The <fun>receive_word</fun> is called when there is traffic on the bus.
   <param>word</param> contains the data received (with valid parity bit),
   and the <param>parity_ok</param> tells whether the parity of the word
   is correct or not.

   </add>

   <add id="arinc429_receiver_interface_exec_context">
   Cell Context for all methods.
   </add>
*/
SIM_INTERFACE(arinc429_receiver) {
    void (*receive_word)(conf_object_t *dev, uint32 word, int parity_ok);
};
// ADD INTERFACE arinc429_receiver_interface

#define ARINC429_BUS_INTERFACE "arinc429_bus"
#define ARINC429_RECEIVER_INTERFACE "arinc429_receiver"

#if defined(__cplusplus)
}
#endif

#endif
