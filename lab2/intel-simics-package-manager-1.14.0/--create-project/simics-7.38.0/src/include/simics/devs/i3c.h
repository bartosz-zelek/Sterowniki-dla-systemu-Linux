/*
  © 2017 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_DEVS_I3C_H
#define SIMICS_DEVS_I3C_H

#include <simics/base/types.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/*
I3C protocol - transaction method sequence

Legacy I2C Read
M               -       S
                ->      start (R/W = 1)
acknowledge     <-
                ->      read
read_response   <-
                ->      stop

Legacy I2C Write
M               -       S
                ->      start (R/W = 0)
acknowledge     <-
                ->      legacy_write
acknowledge     <-
                ->      stop

I3C SDR Read
M               -       S
                ->      start (R/W = 1)
acknowledge     <-
                ->      read
read_response   <-
                ->      stop

I3C SDR Write
M               -       S
                ->      start (R/W = 0)
acknowledge     <-
                ->      sdr_write
                ->      stop

I3C SDR Broadcast
M               -       S
                ->      start (0x7e | R/W = 0)
acknowledge     <-
                ->      sdr_write (Broadcast CCC)
                ->      sdr_write
                ->      stop

I3C SDR Direct Read
M               -       S
                ->      start (0x7e | R/W = 0)
acknowledge     <-
                ->      sdr_write (Direct CCC)
                ->      start (R/W = 1)
acknowledge     <-
                ->      read
read_response   <-
                ->      start (R/W = 1)
acknowledge     <-
                ->      read
read_response   <-
                ->      stop

I3C SDR Direct Write
M               -       S
                ->      start (0x7e | R/W = 0)
acknowledge     <-
                ->      sdr_write (Direct CCC)
                ->      start (R/W = 0)
acknowledge     <-
                ->      sdr_write
                ->      start (R/W = 0)
acknowledge     <-
                ->      sdr_write
                ->      stop

Dynamic Address Assignment
M               -       S
                ->      start (0x7e | R/W = 0)
acknowledge     <-
                ->      sdr_write (ENTDAA CCC)
                ->      start (0x7e | R/W = 1)
acknowledge     <-
                ->      daa_read
daa_response    <-
                ->      legacy_write (assigned address)
acknowledge     <-
                ->      start (0x7e | R/W = 1)
acknowledge     <-
                ->      daa_read
daa_response    <-
                ->      legacy_write (assigned address)
acknowledge     <-
                ->      stop

I3C HDR: See interface documentation for i3c_hdr_master

Hot-Join
M                       -       S
ibi_request             <-
                        ->      ibi_start
ibi_address (0x02 | 0)  <-
                        ->      ibi_acknowledge
[Enter into DAA process]

In-Band Interrupt
M                       -       S
ibi_request             <-
                        ->      ibi_start
ibi_address (addr | 1)  <-
                        ->      ibi_acknowledge
                        ->      read (request Mandatory Data Byte)
read_response           <-
                        ->      stop

Secondary Master
M                       -       S
ibi_request             <-
                        ->      ibi_start
ibi_address (addr | 0)  <-
                        ->      ibi_acknowledge
[Enter into Direct Read process]
*/

/*

   Interfaces for the i3c architecture
   -----------------------------------

   I3C is a two-wire bidirectional serial Bus, optimized for multiple sensor
   Slave Devices and controlled by only one I3C Master Device at a time.
   I3C includes:
   • Support for many Legacy I2C Slave Devices and messages
   • I3C Single Data Rate (SDR) Mode: New I3C enhanced version of the I2C
     protocol supporting private messages, and adding two kinds of standard
     built-in messages:
     • Broadcast messages, which are sent to all I3C Slaves on the Bus
     • Direct messages, which are addressed to specific Slaves
   • I3C High Data Rate (HDR) Modes: Additional optional Modes for higher data rate.
     Interfaces i3c_hdr_master and i3c_hdr_slave are used to model HDR communication.

   All I3C communication occurs within a Frame. The Frame begins with a START,
   followed by one or more transfers, and a STOP.

   An I3C bus consists of a number of master devices and a number of slave
   devices. Both master and slave device can start a communication over the bus.
   Slave device starts communication when:
   - Hot-Join mechanism, to allow Slaves to join the I3C Bus after it is already
     configured.
   - In-Band Interrupt mechanism, for an I3C Slave to request an interrupt.
   - Secondary Master, for an I3C Slave to control of the I3C Bus, in a Master
     role.

   At any time, only one master (the one which drives the bus) can drive an
   Address onto the Bus. No arbitration in this stage.

*/

/* <add id="i3c_master_interface_t">
   <insert-until text="// ADD INTERFACE i3c_master_interface"/>

   The interfaces <iface>i3c_master</iface> <iface>i3c_slave</iface>
   and <iface>i3c_daa_snoop</iface> are used together to model
   communication over an I3C bus.
   There are four kinds of devices roles on the I3C bus: Main Master,
   Secondary Master, Slave and Legacy I2C Slave. Exactly one device
   must be the Main Master; this is typically configured statically.
   Main Master should implement the <iface>i3c_master</iface> interface,
   while slave should implement the <iface>i3c_slave</iface> interface.
   Secondary Master should implement both.
   Most calls to a method in the slave device interfaces expect a response
   call to a method in the master device interfaces. One exception is the
   method <fun>sdr_write</fun> in the <fun>i3c_slave</fun> interface, master
   expects no response when calling it.
   It is up to each device to find the caller's interfaces; usually it is
   configured with attributes in both the master and slave device.

   Two I3C devices can communicate directly if one implements the
   <iface>i3c_master</iface> interface and the other one implements the
   <iface>i3c_slave</iface> interface. An I3C bus with multiple masters
   or slaves is typically modelled using an <class>i3c-link</class> object,
   where each device is connected to an endpoint object.

   An I3C bus consists of a number of <em>master</em> devices and a number
   of <em>slave</em> devices. Each I3C slave device listens to one or more
   <em>7-bit addresses</em>. The slave devices can be I3C slave devices or
   Legacy I2C slave devices. This is because I3C protocol is compatible with
   I2C protocol, so that I3C master device can communicate with I2C slave
   devices.
   For legacy I2C slave device, the address it listens to is
   pre-configured by the device, i.e. static address. For I3C slave device,
   the address it listens to can be either static address or dynamic address.
   The dynamic address is assigned by master during Dynamic Address Assignment
   process. It is an error to connect two slave devices to the same
   bus if they listen to the same address.
   In a communication, the Current Master refers to the device who now drives
   the bus. It can be Main Master, or Secondary Master.
   Usually, communication over the bus is initiated by the Current Master,
   which can communicate with one or more slave devices at a time.
   A slave device can request to initiate an communication, i.e., issue
   an ibi_request, in three cases:
   Hot-Join, In-Band Interrupt and as a Secondary Master other than the
   Current Master requesting to become Current Master.
   Only one device on a bus can communicate at a time.

   The <fun>start</fun> method starts a transfer. The <param>address</param>
   parameter is the address header combined with read/write bit encoded as an
   8-bit number. The least significant bit is the read/write bit and the other
   bits is the 7-bit address header.
   If the <fun>start</fun> method is called from master, the address header
   can be address pattern 0x7E for broadcast, or specific slave address (either
   I2C slave static address or I3C slave dynamic address).

   In I3C bus or link implementation, every start request will broadcast to
   all other devices. So does the stop request. This will monitor bus/link
   status to all devices.

   Normally, master starts a transfer, then the slave responds to
   <fun>start</fun> using the <fun>acknowledge</fun> method (implemented by
   master side interface). The <param>ack</param> parameter may be
   <const>I3C_ack</const> or <const>I3C_noack</const> if the start is acked or
   noacked, respectively.

   There are five types of different start requests:
   <dl><dt>I3C master read transaction</dt>
   <dd>If a master's start with specific slave address was acked by other
   device, and read/write bit in the <param>address</param> parameter of
   the <fun>start</fun> method was 1, then master proceeds with a sequence
   of <fun>read</fun> calls. Each call is followed by a <fun>read_response</fun>
   call from the slave. Parameter <param>data</param> in the method
   <fun>read_response</fun> is the data transferred. The procedure is no
   difference with regard to read from I3C slave or read from I2C slave.
   In hardware, the ACK/T bit is handled differently for i2c and i3c, and
   the master is supposed to know whether the slave is an i2c or i3c device.
   In the i3c case, the slave is supposed to pass the T bit in the
   <param>more</param> argument of <fun>read_response</fun>, while an i2c
   device always passes <param>more</param> as true. Further more, in the
   i2c case, a master sends an ACK bit, which is not represented explicitly
   in the interface. The value of this bit can be deduced from whether the
   following operation is START/STOP or READ. For i3c case, parameter
   <param>more</param> in method <fun>read_response</fun> indicates if
   there are still data waiting to be transferred from slave to master.
   <param>more</param> is true when there are still data left, false otherwise.
   </dd>

   <dt>I3C master write transaction (write to I3C slave)</dt>
   <dd>If a master's start with specific slave address was acked by other
   device, and the read/write bit in the <param>address</param> parameter
   of <fun>start</fun> method was 0, the master proceeds with a
   <fun>sdr_write</fun> call, no response is expected. Parameter
   <param>data</param> in method <fun>write</fun> is the data transferred.
   The parity bit is not passed explicitly in sdr_write, and that the slave
   can assume the parity bit is correctly set.
   </dd>

   <dt>I3C master write transaction (write to I2C slave)</dt>
   <dd>When the master's start request is to write to I2C slave, i.e.,
   master issues a start request to I2C slave address with read/write bit
   is 0, then master proceeds with a sequence of <fun>write</fun> calls,
   each call expects a response call to a method <fun>acknowledge</fun>.
   This is quite similar to I2C write transaction.</dd>

   <dt>I3C broadcast transaction</dt>
   <dd>When master starts a transfer with I3C Broadcast Address 0x7E which
   is passed as address header, i.e., address 0xFC passed in the
   <param>address</param> argument of method <fun>start</fun>, a typical
   I3C message transfer is initiated.

   Multiple I3C slave devices may acknowledge the start request. The master
   will conceive a request as being acknowledged if it was acknowledged by
   at least one of the slaves. Note that there will be a single
   <fun>acknowledge</fun> call in the master; the link or bus handles
   aggregation of acks.

   Master sends the I3C Commands (Common Command Code, CCC) using
   <fun>sdr_write</fun> method after receiving ACK, showing to communicate
   with either all Slaves (Broadcast CCCs) or specific individual Slaves
   (Direct CCCs). This operation expects no response, and master will proceed.

   If Broadcast CCC sent previously, master calls <fun>sdr_write</fun> method
   to transfer data to all slaves who acked the initial access.</dd>

   <dt>I3C direct transaction</dt>
   <dd>After sending a Direct CCC, master continues with a repeated start
   request along with one specific I3C slave address. The transaction goes
   on just like master is communicating with only one slave. After the
   communication with one slave, master may be issue a repeated start request
   to another slave. Further communication is routed only to the targeted
   slave device.</dd>
   </dl>

   After sending a number of reads or a number of writes, the master may
   choose to either call the <fun>stop</fun> method to issue a <em>stop
   condition</em> which terminates the transfer, or to call the <fun>start</fun>
   method again to issue a <em>repeated start condition</em>. The repeated start
   condition works like a normal start condition.

   The <fun>start</fun> method and <fun>read</fun> method in the slave
   interfaces are allowed to respond synchronously; i.e., the
   <fun>acknowledge</fun> or <fun>read</fun> method may be called before the
   corresponding method in the slave interfaces has returned. A master that
   needs to connect directly to a slave device needs to take this into
   consideration; however, when communicating via an <class>i3c-link</class>
   object, the link guarantees that all responses are asynchronous.

   The method <fun>daa_read</fun> and method <fun>daa_response</fun> are used
   in Dynamic Address Assignment process.
   Method <fun>daa_read</fun> is called from master to signal a daa read request.
   Method <fun>daa_response</fun> is called from slave to send slave specific
   8-bytes data (48-bit Unique ID, BCR, DCR) to master. The parameter
   <param>id</param>, <param>bcr</param> and <param>dcr</param> in method
   <fun>daa_response</fun> represent the slave specific daa data.
   If multiple slaves response daa data, the slave with the lowest data wins
   and it will get the assigned address this time.
   Master calls method <fun>write</fun> in slave interfaces to send assigned
   dynamic address to the slave who wins. This operation expects a response
   call to method <fun>acknowledge</fun> in master side. Parameter
   <param>data</param> in method <fun>write</fun> stores the assigned address.

   Master should implement the method <fun>stop</fun> in the interface
   <iface>i3c_slave</iface> in case there is secondary master in the
   configuration which may issue start request.

   Slave can request to start a communication through <fun>ibi_request</fun>.
   The request is sent to Current Master which drives the bus now.
   If the master chooses to continue the slave request, <fun>ibi_start</fun>
   is called, this ibi start will broadcast to all devices in the configuration.
   Then slave can send its address header with <fun>ibi_address</fun>
   argument <arg>address</arg>. The address header can be 0x02 for Hot-Join,
   or the address of the slave itself for IBI and secondary master with
   read/write bit 1 and 0 respectively.
   If more than one slave issues <fun>ibi_address</fun>, arbitration occurs,
   the slave with lowest address header wins and the winning address will be
   delivered to master by link or bus.
   At that time, master issues <fun>ibi_acknowledge</fun> to the slave who wins.
   Other slaves which do not receive <fun>ibi_acknowledge</fun> will consume it
   lost arbitration already.

   </add>
   <add id="i3c_master_interface_exec_context">
   Cell Context for all methods.
   </add>
*/

typedef enum {
        I3C_ack = 0,
        I3C_noack
} i3c_ack_t;


SIM_INTERFACE(i3c_master) {
        void (*acknowledge)(conf_object_t *obj, i3c_ack_t ack);
        void (*read_response)(conf_object_t *obj, uint8 value, bool more);
        void (*daa_response)(conf_object_t *obj,
                             uint64 id, uint8 bcr, uint8 dcr);
        void (*ibi_request)(conf_object_t *obj);
        void (*ibi_address)(conf_object_t *obj, uint8 address);
};

#define I3C_MASTER_INTERFACE "i3c_master"
// ADD INTERFACE i3c_master_interface

/* <add id="i3c_slave_interface_t">
   <insert-until text="// ADD INTERFACE i3c_slave_interface"/>
   </add>
   <add id="i3c_slave_interface_exec_context">
   Cell Context for all methods.
   </add>
*/
SIM_INTERFACE(i3c_slave) {
        void (*start)(conf_object_t *obj, uint8 address);
        void (*write)(conf_object_t *obj, uint8 value);
        void (*sdr_write)(conf_object_t *obj, bytes_t data);
        void (*read)(conf_object_t *obj);
        void (*daa_read)(conf_object_t *obj);
        void (*stop)(conf_object_t *obj);
        void (*ibi_start)(conf_object_t *obj);
        void (*ibi_acknowledge)(conf_object_t *obj, i3c_ack_t ack);
};

#define I3C_SLAVE_INTERFACE "i3c_slave"
// ADD INTERFACE i3c_slave_interface

/* <add id="i3c_daa_snoop_interface_t">
   <insert-until text="// ADD INTERFACE i3c_daa_snoop_interface"/>

   During DAA process, the assigned address information will be monitored
   to devices who implement the interface <iface>i3c_daa_snoop</iface>.
   Whenever an address is assigned to a slave, the method
   <fun>assigned_address</fun> is called in all devices that implement the
   interface. The parameter <param>id</param>, <param>bcr</param> and
   <param>dcr</param> in method <fun>assigned_address</fun> is slave specific
   daa data which indicates who gets the assigned address. Parameter
   <param>address</param> stores the assigned address. The 7-bit address
   is represented as 8-bit number with the highest bit set to 0.

   Interface <iface>i3c_daa_snoop</iface> is mainly implemented by slave who
   may become secondary master later and needs to know the information about
   slave addresses.
   </add>
   <add id="i3c_daa_snoop_interface_exec_context">
   Cell Context for all methods.
   </add>
*/
SIM_INTERFACE(i3c_daa_snoop) {
        void (*assigned_address)(conf_object_t *obj, uint64 id,
                                 uint8 bcr, uint8 dcr, uint8 address);
};

#define I3C_DAA_SNOOP_INTERFACE "i3c_daa_snoop"
// ADD INTERFACE i3c_daa_snoop_interface

/* <add id="i3c_hdr_slave_interface_t">
   <insert-until text="// ADD INTERFACE i3c_hdr_slave"/>
   </add>
   <add id="i3c_hdr_slave_interface_exec_context">
   Cell Context for all methods.
   </add>
*/
SIM_INTERFACE(i3c_hdr_slave) {
        void (*hdr_write)(conf_object_t *obj, bytes_t data);
        void (*hdr_read)(conf_object_t *obj, uint32 max_len);
        void (*hdr_restart)(conf_object_t *obj);
        void (*hdr_exit)(conf_object_t *obj);
};

#define I3C_HDR_SLAVE_INTERFACE "i3c_hdr_slave"
// ADD INTERFACE i3c_hdr_slave_interface

/* <add id="i3c_hdr_master_interface_t">
   <insert-until text="// ADD INTERFACE i3c_hdr_master"/>
   The interfaces <iface>i3c_hdr_slave</iface> and <iface>i3c_hdr_master</iface>
   are used together to model I3C HDR communication over the bus.
   HDR modes are entered by sending one of the ENTHDRx broadcast CCCs on the bus
   through normal SDR communication.
   Slaves that support HDR shall now internally switch to HDR mode.
   The master can exit HDR mode by calling <fun>hdr_exit</fun> in
   the <iface>i3c_hdr_slave</iface> interface and
   then follow it with a <fun>stop</fun> call. All slaves that support HDR
   shall now internally switch back to SDR mode.

   All write transfers in HDR mode are done using the <fun>hdr_write</fun>
   method. Slaves must always respond through method <fun>hdr_acknowledge</fun>
   to either ACK or NACK the write. Devices not supporting active acknowledgement
   for a certain write transfer block must always respond with an ACK as it
   represents a passive acknowledgement according to MIPI I3C V1.1.1 spec.
   
   All read transfers in HDR mode are done using the <fun>hdr_read</fun> method.
   Parameter <param>max_len</param> in method <fun>hdr_read</fun> is the maximum
   number of bytes that the slave is allowed to send in response.
   The slave shall respond with an <fun>hdr_read_response</fun>
   call after each <fun>hdr_read</fun> call. The parameter <param>more</param>
   indicates if the slave has more data to send.
   The slave is allowed to send back less than <param>max_len</param> and at the same time
   set <param>more</param> to true. This feature is an optimization in order to:
      1. Minimize the number of <fun>hdr_read</fun> calls needed to read a large amount of data.
      2. Avoid underflow on the slave side by allowing the slave respond with less data.
   
   If <param>more</param> is set to false, the master has to ends it current transfer
   by either calling <fun>hdr_restart</fun> or exit HDR through the procedure described above.


   Examples of HDR write transactions:
   <figure>
   <pre>
HDR Private Write 256 bytes followed by a 64 byte write:
+-------------+-----------------+--------------------------------------------------------+
| Master      | Slave           | Arguments                                              |
+-------------+-----------------+--------------------------------------------------------+
| hdr_write   |                 | 2 bytes: RnW @ 15 |  CC @ 14:8 | Target Address @ 8:1  |
+-------------+-----------------+--------------------------------------------------------+
|             | hdr_acknowledge | ACK                                                    |
+-------------+-----------------+--------------------------------------------------------+
| hdr_write   |                 | 100-bytes                                              |
+-------------+-----------------+--------------------------------------------------------+
|             | hdr_acknowledge | ACK                                                    |
+-------------+-----------------+--------------------------------------------------------+
| hdr_write   |                 | 100-bytes                                              |
+-------------+-----------------+--------------------------------------------------------+
|             | hdr_acknowledge | ACK                                                    |
+-------------+-----------------+--------------------------------------------------------+
| hdr_write   |                 | 56-bytes                                               |
+-------------+-----------------+--------------------------------------------------------+
|             | hdr_acknowledge | ACK                                                    |
+-------------+-----------------+--------------------------------------------------------+
| hdr_restart |                 |                                                        |
+-------------+-----------------+--------------------------------------------------------+
| hdr_write   |                 | 2 bytes: RnW @ 15 |  CC @ 14:8 | Target Address @ 8:1  |
+-------------+-----------------+--------------------------------------------------------+
|             | hdr_acknowledge | ACK                                                    |
+-------------+-----------------+--------------------------------------------------------+
| hdr_write   |                 | 64-bytes                                               |
+-------------+-----------------+--------------------------------------------------------+
|             | hdr_acknowledge | ACK                                                    |
+-------------+-----------------+--------------------------------------------------------+
| hdr_exit    |                 |                                                        |
+-------------+-----------------+--------------------------------------------------------+
| stop        |                 |                                                        |
+-------------+-----------------+--------------------------------------------------------+
   </pre>
   </figure>

   The sequence of hdr_write calls could be made differently. Master can send smaller or larger
   chunks of data in each hdr_write call. The main concerns are to minimize the number
   of interface calls and to avoid overflow on the slave side. A balance between the
   two should be considered in the implementation of the master.

   <figure>
   <pre>
HDR Private Read 256 bytes:
+-----------+-------------------+-------------------------------------------------------+
| Master    | Slave             | Arguments                                             |
+-----------+-------------------+-------------------------------------------------------+
| hdr_write |                   | 2 bytes: RnW @ 15 |  CC @ 14:8 | Target Address @ 8:1 |
+-----------+-------------------+-------------------------------------------------------+
|           | hdr_acknowledge   | ACK                                                   |
+-----------+-------------------+-------------------------------------------------------+
| hdr_read  |                   | max_len = 128                                         |
+-----------+-------------------+-------------------------------------------------------+
|           | hdr_read_response | bytes.len = 64, more = true                           |
+-----------+-------------------+-------------------------------------------------------+
| hdr_read  |                   | max_len = 128                                         |
+-----------+-------------------+-------------------------------------------------------+
|           | hdr_read_response | bytes.len = 64, more = true                           |
+-----------+-------------------+-------------------------------------------------------+
| hdr_read  |                   | max_len = 128                                         |
+-----------+-------------------+-------------------------------------------------------+
|           | hdr_read_response | bytes.len = 64, more = true                           |
+-----------+-------------------+-------------------------------------------------------+
| hdr_read  |                   | max_len = 64                                          |
+-----------+-------------------+-------------------------------------------------------+
|           | hdr_read_response | bytes.len = 64, more = false                          |
+-----------+-------------------+-------------------------------------------------------+
| hdr_exit  |                   |                                                       |
+-----------+-------------------+-------------------------------------------------------+
| stop      |                   |                                                       |
+-----------+-------------------+-------------------------------------------------------+
</pre>
</figure>

   The sequence of <fun>hdr_read</fun> calls could be made differently. Master can read smaller
   chunks of data in each hdr_read call. The main concerns are to minimize the number
   of interface calls and to avoid underflow on the slave side. A balance between the
   two should be considered in the implementation of the master.

   Note: This interface is in tech-preview and may change without
   notice.

   </add>
   <add id="i3c_hdr_master_interface_exec_context">
   Cell Context for all methods.
   </add>
*/
SIM_INTERFACE(i3c_hdr_master) {
        void (*hdr_read_response)(conf_object_t *obj, bytes_t bytes, bool more);
        void (*hdr_acknowledge)(conf_object_t *obj, i3c_ack_t ack);
};

#define I3C_HDR_MASTER_INTERFACE "i3c_hdr_master"
// ADD INTERFACE i3c_hdr_master_interface

#ifdef __cplusplus
}
#endif

#endif
