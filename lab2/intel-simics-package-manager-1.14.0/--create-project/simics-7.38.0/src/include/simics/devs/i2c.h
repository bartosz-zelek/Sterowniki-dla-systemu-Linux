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

#ifndef SIMICS_DEVS_I2C_H
#define SIMICS_DEVS_I2C_H

#include <simics/base/attr-value.h>
#include <simics/base/types.h>
#include <simics/pywrap.h>
#include <simics/obsolete/6.h>

#if defined(__cplusplus)
extern "C" {
#endif

/*

   Interfaces for the i2c architecture
   -----------------------------------

   The i2c bidirectional 2-wire bus and data transmission protocol was
   developed in the early 1980's by Philips semiconductors. The i2c is
   an acronym for inter-IC (iic), and this name literally explains the
   purpose; to connect integrated circuits to a common bus.

   A device that sends data to the bus is defined as a transmitter,
   and a device receiving data from the bus is defined as a receiver.
   The device that controls the transfer is called a master, and
   devices that are controlled by the master are called slaves. This
   gives us a total of four different modes for device operations:

   - master/transmitter
   - master/receiver
   - slave/transmitter
   - slave/receiver

   Note that not all devices can operate as a bus master, and that not
   all devices can operate as a bus slave.

   The transfer must be initiated by the master, through a start (or
   repeat-start) condition. All data are transferred one byte at a
   time, and the first byte contains the slave address (with the least
   significant bit as a read/write flag).

*/

/* <add id="i2c_slave_v2_interface_t">
   <insert-until text="// ADD INTERFACE i2c_slave_v2_interface"/>

   The <iface>i2c_slave_v2</iface> interface is used together with the <nref
   label="__rm_interface_i2c_master_v2"><iface>i2c_master_v2</iface></nref>
   interface to model communication over an I2C bus. Most calls to a method in
   the <iface>i2c_slave_v2</iface> interface expect a response call to a method
   in the <iface>i2c_master_v2</iface> interface. It is up to each device to
   find the caller's interface; usually it is configured with an attribute in
   both the master and slave device.

   The <iface>i2c_slave_v2</iface> and <iface>i2c_master_v2</iface> interfaces
   replace the obsolete interfaces <iface>i2c_master</iface>,
   <iface>i2c_slave</iface>, <iface>i2c_link</iface>, and
   <iface>i2c_bridge</iface>.

   An I2C bus consists of a number of <em>master</em> devices and a number of
   <em>slave</em> devices.  Each slave device listens to one or more <em>7-bit
   addresses</em> or <em>10-bit addresses</em>.  It is an error to connect two
   slave devices to the same bus if they listen to the same address. 
   Communication over the bus is initiated by a master, which can communicate
   with one slave device at a time.  Only one master on a bus can communicate
   at a time.

   Two I2C devices can communicate directly if one implements the
   <iface>i2c_master_v2</iface> interface and the other one implements the
   <iface>i2c_slave_v2</iface> interface.  An I2C bus with multiple masters or
   slaves is typically modeled using an <class>i2c-link-v2</class> object,
   where each device is connected to an endpoint object.

   The <fun>start</fun> method starts a transfer.  The
   <param>address</param> parameter is the address of the slave device, encoded
   as an 8-bit number.  To communicate with 7-bit address slave device, using 
   little-endian bit numbering, bit 0 is a read/write bit (0 &rarr; master
   writes, 1 &rarr; master reads) and bits 1 to 7 is the 7-bit address of
   the slave device.  The <param>address</param> parameter should normally
   be in the (inclusive) range <tt>0x10 - 0xef</tt>; the ranges <tt>0x00 -
   0x0f</tt> and <tt>0xf0 - 0xff</tt> are reserved for special addressing modes.
   To communicate with 10-bit address slave device, using little-endian bit 
   numbering, bit 0 is a read/write bit (same as 7-bit address), bits 1 to 2
   is part of 10-bit address, and bits 3 to 7 is 10-bit address mode 0b11110.

   The slave responds to <fun>start</fun> using the
   <fun>acknowledge</fun> method.  The <param>ack</param> parameter may be
   <const>I2C_ack</const> or <const>I2C_noack</const> if the start is acked
   or noacked, respectively. In a multi-master configuration, the slave may
   instead respond by calling the <fun>start</fun> method in the
   <iface>i2c_slave_v2</iface> interface of the master device; this signals
   that the master lost the bus arbitration. This is discussed further below.

   If a start was acked by a slave device, read/write bit in the
   <fun>start</fun> method was 1, the master proceeds with a sequence
   of <fun>read</fun> calls.  Each call is followed by a
   <fun>read_response</fun> call from the slave.  If the read/write bit in
   the <fun>start</fun> was 0, the master instead proceeds with a
   sequence of <fun>write</fun> calls, each one followed by an
   <fun>acknowledge</fun> call. The <param>ack</param> parameter of the
   <fun>acknowledge</fun> method may be either <const>I2C_ack</const> or
   <const>I2C_noack</const>, representing ack and noack, respectively.

   After sending a number of reads or writes, the master may choose to
   either call the <fun>stop</fun> method to issue a <em>stop condition</em>
   which terminates the transfer, or to call the <fun>start</fun>
   method again to issue a <em>repeated start condition</em>.  The repeated
   start condition works like a normal start condition.

   When 0 is passed as the <param>address</param> parameter of the
   <fun>start</fun> method, a <em>General Call</em> transfer is initiated. A
   General Call transfer works like a normal write transfer, except that
   multiple slave devices may acknowledge the start and write requests.  The
   master will conceive a request as acknowledged if it was acknowledged by at
   least one of the slaves.

   When 10-bit address pattern is passed as the <param>address</param>
   parameter of the <fun>start</fun> method, a <em>10-bit address</em>
   transaction is initiated. A 10-bit address transaction begins with write
   status (read/write bit should be 0). As mentioned before, bits 1 to 2 are
   the most significant bits of 10-bit address. Then following is a call
   to the <fun>write</fun> method with the last 8 bits of 10-bit address.
   If the transaction is slave-receiver, the next transfer works like a
   normal write transfer. If the transaction is slave-transmitter, then
   the <fun>start</fun> method is called again with the same address pattern
   as before, but with bit 0 (the read/write bit) set to 1. This is followed
   by calls to <fun>read</fun> like in a normal read transaction.

   The <fun>addresses</fun> method is used to retrieve the set of addresses a
   device currently listens to.  The method is called by the
   <class>i2c-link-v2</class> to provide an overview of the bus topology, and
   to detect address collisions early.  The method may not have any
   side-effects on the simulation.  The return value is a list containing all
   the different values of the <param>address</param> parameter in a
   <fun>start</fun> call that the slave would respond to with
   <const>I2C_ack</const>.  The list may also contain an element
   <tt>Nil</tt> to mark that the device might respond to other addresses as
   well.  If a device is configured to listen to a specific address but is
   currently disabled, then that address may still be included in the list.

   For example, if a slave device listens to the single 7-bit address
   <tt>0x55</tt>, the return value of <fun>addresses</fun> would be
   <tt>[0xaa, 0xab]</tt>, using Python list syntax.

   The specification of the return value may be extended in the future, to
   allow other values than <tt>Nil</tt> and 8-bit integers. Therefore, callers
   should not make any assumptions on how the elements are formatted in the
   return value.

   For most I2C devices it is sufficient to implement only one of the
   <iface>i2c_slave_v2</iface> and <iface>i2c_master_v2</iface> interfaces.  In
   some cases it may be useful for a device to implement both interfaces,
   either because the device can act both as a master and as a slave on the
   bus, or because it needs to use the <fun>start</fun> and
   <fun>stop</fun> methods to monitor start and stop conditions on the bus.
   I2C link endpoints also implement both interfaces.

   If multiple master devices are connected to the same i2c link, then all
   master devices need to implement the <iface>i2c_slave_v2</iface> interface
   in addition to <iface>i2c_master_v2</iface>, in order to handle bus
   arbitration correctly:

   <ul>
     <li>A master device should monitor calls to the <fun>start</fun> and
     <fun>stop</fun> functions: After a call to <fun>start</fun> in a master
     device, the master should not call <fun>start</fun> in the slave until
     after a subsequent call to <fun>stop</fun> in the master.</li>

     <li>When a master calls <fun>start</fun> in a link endpoint device, the
     endpoint may respond with a call to <fun>start</fun> instead of
     <fun>acknowledge</fun>; this means that there was bus arbitration, which
     the master lost to the given <fun>start</fun> request.  Note that this
     case can not happen on a latency-free connection, since all arbitration is
     handled by the master's monitoring of <fun>start</fun> and <fun>stop</fun>
     calls. Note also that user-written slave devices are required to respond
     with <fun>acknowledge</fun> to a <fun>start</fun> call; only the i2c link
     endpoint (and other i2c infrastructure objects from the Simics-Base
     package) are allowed to respond with a <fun>start</fun> call.</li>
   </ul>

   Note that there is no need for a master device to implement the
   <iface>i2c_slave_v2</iface> interface if the device only will be used in
   single-master configurations.

   The <fun>start</fun>, <fun>write</fun> and <fun>read</fun> methods in the
   <iface>i2c_slave_v2</iface> interface are allowed to respond synchronously;
   i.e., the <fun>acknowledge</fun> or <fun>read_response</fun> method may be
   called before the corresponding method in the slave interface has returned.
   A master that needs to connect directly to a slave device needs to take this
   into consideration; however, when communicating via an
   <class>i2c-link-v2</class> object, the link guarantees that all responses
   are asynchronous.  </add>

   <add id="i2c_slave_v2_interface_exec_context">
   <table border="false">
   <tr><td><tt>addresses</tt></td><td>Global Context</td></tr>
   <tr><td>All other methods</td><td>Cell Context</td></tr>
   </table>
   </add>
*/
SIM_INTERFACE(i2c_slave_v2) {
        void (*start)(conf_object_t *device, uint8 address);
        void (*read)(conf_object_t *device);
        void (*write)(conf_object_t *device, uint8 value);
        void (*stop)(conf_object_t *device);
        attr_value_t (*addresses)(conf_object_t *device);
};
#define I2C_SLAVE_V2_INTERFACE "i2c_slave_v2"
// ADD INTERFACE i2c_slave_v2_interface

/* <add id="i2c_master_v2_interface_t">
   <insert-until text="// ADD INTERFACE i2c_master_v2_interface"/>

   The <iface>i2c_master_v2</iface> interface should be implemented
   by devices that intend to act as a master on an I2C link.

   The function <fun>acknowledge</fun> is called in response to a
   <fun>start</fun> or <fun>write</fun> call in the
   <iface>i2c_slave_v2</iface> interface of the slave device.
   <fun>read_response</fun> is called as response to a
   <fun>read</fun> call.  More details can be found in the
   documentation of the <nref
   label="__rm_interface_i2c_slave_v2">i2c_slave_v2</nref> interface.

   </add>
   <add id="i2c_master_v2_interface_exec_context">
   Cell Context for all methods.
   </add>
*/

typedef enum {
        I2C_ack = 0,
        I2C_noack = 1
} i2c_ack_t;

SIM_INTERFACE(i2c_master_v2) {
        void (*acknowledge)(conf_object_t *device, i2c_ack_t ack);
        void (*read_response)(conf_object_t *device, uint8 value);
};

#define I2C_MASTER_V2_INTERFACE "i2c_master_v2"
// ADD INTERFACE i2c_master_v2_interface

#if defined(__cplusplus)
}
#endif

#endif
