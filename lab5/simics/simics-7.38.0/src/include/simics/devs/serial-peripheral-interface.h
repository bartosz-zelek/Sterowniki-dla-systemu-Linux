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

#ifndef SIMICS_DEVS_SERIAL_PERIPHERAL_INTERFACE_H
#define SIMICS_DEVS_SERIAL_PERIPHERAL_INTERFACE_H

#include <simics/base/types.h>
#include <simics/pywrap.h>
#include <simics/util/dbuffer.h>

#if defined(__cplusplus)
extern "C" {
#endif

/*

   Interfaces for the SPI bus architecture
   -----------------------------------------

   SPI is a bidirectional bus, where a single master is connected to
   multiple slaves.  The master initiates a transfer, sends a number
   of bits, and closes the transfer.  The communication is always
   full-duplex, so the master receives as many bits as it sends,
   although often both the slave and master device ignores some
   incoming bits. To simulate half-duplex the master sends zeros
   to the slave when master is receiving and ignores incoming data
   when the master is transmitting. The protocol doesn't dictate
   any separation of data into words, though a word size is usually
   defined.

   A master which is connected to multiple slave devices, selects
   which slave to talk to by signaling it on a separate pin, called
   Slave Select (SS). The master has a separate Slave Select pin
   connected to each slave device. In Simics, this is modeled by
   letting the master implement one SPI interface port for each
   outgoing Slave Select pin.

   SPI is not a well-defined standard, and there are different
   de-facto conventions on e.g. whether a pin should be high or low to
   represent a 1.  This means that the results are undefined if two
   connected SPI devices use different conventions.

   In Simics, the acronym SPI is consistently expanded to
   serial_peripheral_interface, to distinguish it from the System
   Packet Interface.
*/

/* <add id="devs api types">
   <name index="true">serial_peripheral_interface_flags_t</name>
   <insert id="serial_peripheral_interface_flags_t DOC"/>
   </add> */

/* <add id="serial_peripheral_interface_flags_t DOC">
   <ndx>serial_peripheral_interface_flags_t</ndx>
   <doc>
   <doc-item name="NAME">serial_peripheral_interface_flags_t</doc-item>
   <doc-item name="SYNOPSIS">
     <insert id="serial_peripheral_interface_flags_t def"/>
   </doc-item>
   <doc-item name="DESCRIPTION">

   The <type>serial_peripheral_interface_flags_t</type> type is used
   to describe some properties of an SPI connection.  The type is a
   bitfield, currently defining two values, CPOL and CPHA.  If a
   master device connects to a slave using a CPOL/CPHA combination
   incompatible with the slave device, then the results of any SPI
   transfer are undefined.

   The <tt>SPI_Flags_CPOL</tt> bit defines the <em>polarity</em> of
   the clock pin (SCK): A value of zero means that the pin is low when
   the bus is idle, while a value of one means that the pin is high
   when the bus is idle.

   The <tt>SPI_Flags_CPHA</tt> bit defines the <em>phase</em> of the
   clock pin. If the CPHA and CPOL bits are equal, data bits are read
   on the falling edge of the SCK pin and changed on the rising edge
   of the pin; if the bits are not equal, data bits are read on the
   rising edge and changed on the falling edge of the SCK pin.

   </doc-item>

   <doc-item name="SEE ALSO">
     <iface>serial_peripheral_interface_slave_interface_t</iface>
   </doc-item>

   </doc></add>
*/
/* <add-type id="serial_peripheral_interface_flags_t def"></add-type>
 */

typedef enum serial_peripheral_interface_flags {
        SPI_Flags_CPHA = 0x1,
        SPI_Flags_CPOL = 0x2
} serial_peripheral_interface_flags_t;


/* <add id="serial_peripheral_interface_master_interface_t">
   <insert-until text="// ADD INTERFACE serial_peripheral_interface_master_interface"/>

   The <iface>serial_peripheral_interface_master</iface> interface is
   implemented by all SPI master devices. The interface contains a
   single function <fun>spi_response</fun>, called by the slave device
   in response to a <fun>spi_request</fun> call. The parameters
   <param>bits</param> and <param>payload</param> have the same
   meaning as in the <fun>spi_request</fun> call, and a SPI response
   should always have the same length in bits as the corresponding request.

   <di name="SEE ALSO">
     <iface>serial_peripheral_interface_slave_interface_t</iface>
   </di>
   </add>
   <add id="serial_peripheral_interface_master_interface_exec_context">
   Cell Context for all methods.

   </add>
*/

SIM_INTERFACE(serial_peripheral_interface_master) {
        void (*spi_response)(conf_object_t *obj, int bits, dbuffer_t *payload);
};

#define SERIAL_PERIPHERAL_INTERFACE_MASTER_INTERFACE \
        "serial_peripheral_interface_master"
// ADD INTERFACE serial_peripheral_interface_master_interface

/* <add id="serial_peripheral_interface_slave_interface_t">
   <insert-until text="// ADD INTERFACE serial_peripheral_interface_slave_interface"/>

   The <iface>serial_peripheral_interface_slave</iface> interface is
   implemented by all SPI slave devices.

   The <fun>connect_master</fun> and <fun>disconnect_master</fun>
   functions are used to select which master device is connected. At
   most one master device can be connected to a slave device at once.
   The <param>flags</param> parameter to <fun>connect_master</fun>
   should be set according to the documentation of the
   <tt>serial_peripheral_interface_flags_t</tt> type.

   An SPI transfer consists of a number of consecutive calls to the
   <fun>spi_request</fun> function. The parameters
   <param>first</param> and <param>last</param> represent the rise and
   fall of the Slave Select (SS) pin: <param>first</param> is
   <const>true</const> on the first <fun>spi_request</fun> call of a
   transfer, while <param>last</param> is <const>true</const> on the
   last call of a transfer.

   The <param>bits</param> and <param>payload</param> parameters
   describe the data sent by the master device. <param>bits</param>
   defines the number of bits to send, while <param>payload</param>
   defines the data to transfer. The size of the
   <param>payload</param> buffer should be <tt>ceil(bits / 8)</tt>
   bytes. The first byte corresponds to the first 8 sent bits, and the
   least significant bit in each byte corresponds to the first sent
   bit.  For example, the 11-bit sequence (first) <tt>11011111101</tt>
   (last) will be represented as two bytes, <tt>0xfb</tt> followed by
   <tt>0x5</tt>.

   <doc-item name="SEE ALSO">
     <iface>serial_peripheral_interface_master_interface_t</iface>,
     <type>serial_peripheral_interface_flags_t</type>
   </doc-item>

   </add>
   <add id="serial_peripheral_interface_slave_interface_exec_context">
   Cell Context for all methods.
   </add>
*/

SIM_INTERFACE(serial_peripheral_interface_slave) {
        void (*spi_request)(conf_object_t *obj, int first, int last,
                            int bits, dbuffer_t *payload);
        void (*connect_master)(conf_object_t *obj, conf_object_t *master,
                               const char *port,
                               serial_peripheral_interface_flags_t flags);
        void (*disconnect_master)(conf_object_t *obj, conf_object_t *master);
};

#define SERIAL_PERIPHERAL_INTERFACE_SLAVE_INTERFACE \
        "serial_peripheral_interface_slave"
// ADD INTERFACE serial_peripheral_interface_slave_interface

#if defined(__cplusplus)
}
#endif

#endif
