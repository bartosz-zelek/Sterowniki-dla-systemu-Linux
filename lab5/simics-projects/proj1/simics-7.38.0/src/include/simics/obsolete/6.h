/*
  © 2023 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_OBSOLETE_6_H
#define SIMICS_OBSOLETE_6_H

#include <simics/util/dbuffer.h>
#include <simics/model-iface/breakpoints.h>
#include <simics/processor/generic-spr.h>

#if defined(__cplusplus)
extern "C" {
#endif

#if defined(SIMICS_6_API) || defined(SHOW_OBSOLETE_API)

#define HOST_64_BIT
#define HOST_LITTLE_ENDIAN

#ifndef PYWRAP
typedef gen_spr_user_setter_func_t ppc_spr_user_setter_func_t;
typedef gen_spr_user_getter_func_t ppc_spr_user_getter_func_t;
typedef gen_spr_ret_t ppc_spr_ret_t;
typedef gen_spr_access_type_t ppc_spr_access_type_t;

typedef int pseudo_exceptions_t;
#endif

/* <add id="breakpoint_query_interface_t">

   Deprecated; use <iface>breakpoint_query_v2</iface> instead. Classes
   implementing <iface>breakpoint_query_v2</iface> can add support for
   <iface>breakpoint_query</iface> by using
   <fun>SIM_register_compatible_interfaces</fun>.

   Objects of the <class>context</class> and
   <class>memory-space</class> classes implement the
   <iface>breakpoint_query</iface> interface. It is used by processors
   to check for breakpoints.

   The <fun>get_breakpoints</fun> function returns a set of
   breakpoints that intersect the range given in <arg>start</arg> and
   <arg>end</arg>, including both start and end in the range. Only
   breakpoints set on access types with bits set in
   <arg>read_write_execute</arg> will be returned.

   When information from <fun>get_breakpoints</fun> has been
   processed, the return value should be freed by calling the
   <fun>free_breakpoint_set</fun> function.

   <insert-until text="// ADD INTERFACE breakpoint_query_interface"/>
   </add>
   <add id="breakpoint_query_interface_exec_context">
   Cell Context for all methods.
   </add>
*/
SIM_INTERFACE(breakpoint_query) {
        breakpoint_set_t (*get_breakpoints)(conf_object_t *obj,
                                            access_t read_write_execute,
                                            generic_address_t start,
                                            generic_address_t end);
#ifndef PYWRAP
        void (*free_breakpoint_set)(conf_object_t *obj,
                                    breakpoint_set_t *set);
#endif
};

#define BREAKPOINT_QUERY_INTERFACE "breakpoint_query"
// ADD INTERFACE breakpoint_query_interface

/*
   <add id="pool_protect_interface_t">

   Deprecated; use <iface>direct_memory</iface> instead.

   <insert-until text="// ADD INTERFACE pool_protect_interface_t"/>
   </add>

   <add id="pool_protect_interface_exec_context">
   Cell Context for all methods.
   </add>
*/

SIM_INTERFACE(pool_protect) {
        int (*create_group)(conf_object_t *obj, conf_object_t *image_obj);
        void (*protect)(conf_object_t *obj, int id);
};
#define POOL_PROTECT_INTERFACE "pool_protect"
// ADD INTERFACE pool_protect_interface_t

typedef struct {
        int128 val;                          /* time in ps */
} slave_time_t;

SIM_INTERFACE(link_endpoint) {
        /* Delivery time and secondary key for the message currently being
           delivered to the device when using indirect delivery. */
        slave_time_t (*delivery_time)(conf_object_t *obj);
        uint64 (*delivery_skey)(conf_object_t *obj);
};
#define LINK_ENDPOINT_INTERFACE "link_endpoint"

typedef struct recorder recorder_t;

/* <add id="recorder_interface_t">
   This interface is <b>deprecated</b>; please use the
   <iface>recorder_v2</iface> instead.

   To initiate recording, the object should call the <fun>attach</fun> method
   with the recorder, the object itself, and an input handler as parameters.
   This is done once, typically when the object instance is created.

   When the object receives data from outside the simulator, it should call
   the <fun>input</fun> method with a <type>dbuffer_t</type> containing the
   raw data in unspecified form, along with an extra word of user data.
   The <param>data</param> parameter can be <pp>NULL</pp> if not needed.

   The <fun>input_from_recorder</fun> function is called with input data to the
   object. When using live input, it is called with data given to
   <fun>input</fun>; when playing back, it is called with recorded data.
   An object should never use its input directly but only what it receives
   from <fun>input_from_recorder</fun>.

   The <fun>playback</fun> function returns true if the recorder is currently
   playing back recorded data and false if not.

   The <type>dbuffer_t</type> parameters retain their respective ownership.
   The <fun>input_from_recorder</fun> function must not modify the contents
   without cloning it first.

   <insert-until text="// ADD INTERFACE recorder_interface"/>
   </add>
   <add id="recorder_interface_exec_context">
   Cell Context for all methods.
   </add>
*/
typedef void (*recorder_input_handler_t)(conf_object_t *obj, dbuffer_t *data,
                                         uint32 uint_data);

SIM_INTERFACE(recorder) {
        recorder_t *(*attach)(conf_object_t *rec, conf_object_t *obj,
                              recorder_input_handler_t input_from_recorder);
        void (*detach)(conf_object_t *rec, recorder_t *r);
        void (*input)(conf_object_t *rec, recorder_t *r,
                      dbuffer_t *data, uint32 uint_data);
        bool (*playback)(conf_object_t *rec);
};
#define RECORDER_INTERFACE "recorder"
// ADD INTERFACE recorder_interface

/* <add id="slave_agent_interface_t">

   Deprecated, this interface has been replaced by the
   <iface>follower_agent</iface> interface.

   </add>
   <add id="slave_agent_interface_exec_context">
     Cell Context for all methods.
   </add> */
SIM_INTERFACE(slave_agent) {
        void (*accept)(conf_object_t *obj, bytes_t msg);
        void (*accept_async)(conf_object_t *obj, bytes_t msg);
};
#define SLAVE_AGENT_INTERFACE "slave_agent"

/* <add id="slaver_message_interface_t">

   Deprecated, this interface has been replaced by the
   <iface>leader_message</iface> interface.

   </add>
   <add id="slaver_message_interface_exec_context">
     Cell Context for all methods.
   </add> */
SIM_INTERFACE(slaver_message) {
        void (*send)(conf_object_t *obj,
                     slave_time_t time, uint64 skey, bytes_t msg);
        void (*send_async)(conf_object_t *obj, bytes_t msg);
};
#define SLAVER_MESSAGE_INTERFACE "slaver_message"

typedef int hap_flags_t;

#ifndef PYWRAP
EXPORTED void SIM_register_copyright(const char *NOTNULL str);
EXPORTED conf_object_t *SIM_current_processor(void);
EXPORTED bool SIM_is_loading_micro_checkpoint(conf_object_t *obj);
#endif

/* states of the i2c device (well, actually the idle state refers to the i2c
   _bus_). Names used in checkpointing. */
#define FOR_ALL_I2C_DEVICE_STATE(op)                    \
        op(I2C_idle,            "I2C Idle"),            \
        op(I2C_master_transmit, "I2C Master Transmit"), \
        op(I2C_master_receive,  "I2C Master Receive"),  \
        op(I2C_slave_transmit,  "I2C Slave Transmit"),  \
        op(I2C_slave_receive,   "I2C Slave Receive")

#define I2C_FIRST_ELEMENT(a, b) a
typedef enum i2c_device_state {
        FOR_ALL_I2C_DEVICE_STATE(I2C_FIRST_ELEMENT),
        I2C_max_states
} i2c_device_state_t;
#undef I2C_FIRST_ELEMENT

/* <add-type id="i2c_device_flag_t def">
   </add-type> */
typedef enum {
        I2C_flag_exclusive,
        I2C_flag_shared
} i2c_device_flag_t;


/* <add id="i2c_bus_interface_t">
   <insert-until text="// ADD INTERFACE i2c_bus_interface"/>

   Deprecated interface, use <iface>i2c_master_v2</iface> and
   <iface>i2c_slave_v2</iface> instead.

   The <iface>i2c_bus</iface> interface is implemented by the I2C
   bus. The interface is used by I2C devices to communicate with the
   I2C bus.

   To connect an I2C device to an I2C bus, first you call
   <fun>register_device</fun> with a 7-bit <attr>address</attr> and
   <attr>mask</attr>. The address is actually an address pattern. When
   there is traffic on the I2C bus (as initiated by a call to the bus
   interface <fun>start</fun> function), the target address is matched
   against each registered device by checking if <tt>(target_address ^
   device_address) &amp; device_mask == 0</tt>.  The
   <attr>flags</attr> attribute sets the type of connection. The
   alternatives are <const>exclusive</const> or
   <const>shared</const>. An I2C device connected
   <const>exclusive</const> can not share a transfer with another I2C
   device. An I2C device connected <const>shared</const> supports that
   other I2C devices connected <const>shared</const> to the I2C bus
   handles the same transfer. The <fun>register_device</fun> function
   returns <tt>0</tt> on success and <tt>-1</tt> on failure.

   Use <fun>unregister_device</fun> to disconnect an I2C device from
   the I2C bus. To completely remove a device use the same
   <attr>address</attr> and <attr>mask</attr> attributes as when the
   device was registered. An I2C device can also remove some part of
   the address match by unregister itself with a different mask.

   An I2C transfer is initiated by a master I2C device. The I2C device
   responding to the transfer is called slave. The master starts a
   transfer by calling the <fun>start</fun> function with
   <attr>address</attr> as argument. The <attr>address</attr> is the
   7-bit address plus a read/write bit (read/write = 0 &rarr;
   slave-receive, read/write = 1 &rarr; slave-transmit). The read/write
   bit is the least significant bit. This means that
   all odd values sent to <fun>start</fun> function initiates a
   transfer where the master is requesting data from the slave.
   The <fun>start</fun> function returns <tt>0</tt> on success and <tt>-1</tt>
   on failure.

   An I2C transfer should be terminated by a master I2C device when the bus is
   no longer needed. This is done by calling the <fun>stop</fun> function. This
   function always returns <tt>0</tt>.

   I2C devices implement the <iface>i2c_device</iface> interface. Both
   the <iface>i2c_device</iface> interface and the
   <iface>i2c_bus</iface> interface has identical <fun>read_data</fun>
   and <fun>write_data</fun> functions to transfer data over the
   bus. The <iface>i2c_device</iface> also has <fun>set_state</fun>
   function, which is used by the I2C bus to set the I2C device
   state. The states are <const>I2C_idle</const>,
   <const>I2C_slave_transmit</const>, and
   <const>I2C_slave_receive</const>. The default state is
   <const>I2C_idle</const>.

   Here are the steps for a transfer:

   <tt>1.</tt> The I2C master device calls <fun>start</fun> in the
    I2C bus with 7-bit address and 1-bit read/write flag.

   <tt>2.</tt> The I2C bus calls the <fun>set_state</fun> in the I2C
   slave device with <const>I2C_slave_transmit</const> or
   <const>I2C_slave_receive</const> as argument depending on the 1-bit
   read/write flag. The I2C slave accepts the state change by
   returning 0.

   <tt>3.</tt> I2C bus returns <tt>0</tt> to the I2C master if the
   start command in <tt>1</tt> was successful. The I2C bus can report
   failure for several reasons, there are another ongoing transfer,
   can not find any device with the address provided, I2C slave did
   not except state change etc.

   <tt>4.</tt> The I2C master calls the I2C buses
   <fun>read_data</fun> or <fun>write_data</fun> depending if it
   wants to read from or write to the I2C slave. The I2C bus relays the
   call to the I2C slave <fun>read_data</fun> or
   <fun>write_data</fun> function. The I2C slave have no way to report
   errors, the I2C master expects the I2C slave to be able to handle
   all calls without any problem. The I2C bus can do step <tt>4</tt>
   several times before terminating the transfer.

   <tt>5.</tt> The I2C master calls the <fun>stop</fun> function
   when it wants to terminate the transfer. This causes the I2C bus to
   call the <fun>set_state</fun> function in the I2C slave with
   <const>I2C_idle</const> as argument. The transfer is now completed.

   </add>
   <add id="i2c_bus_interface_exec_context">
   Cell Context for all methods.
   </add>
*/
SIM_INTERFACE(i2c_bus) {
        int (*start)(conf_object_t *i2c_bus, uint8 address);
        int (*stop)(conf_object_t *i2c_bus);
        uint8 (*read_data)(conf_object_t *i2c_bus);
        void (*write_data)(conf_object_t *i2c_bus, uint8 value);
        int (*register_device)(conf_object_t *i2c_bus,
                               conf_object_t *device,
                               uint8 address, uint8 mask,
                               i2c_device_flag_t flags);
        void (*unregister_device)(conf_object_t *i2c_bus,
                                  conf_object_t *device,
                                  uint8 address, uint8 mask);
};

#define I2C_BUS_INTERFACE "i2c_bus"
// ADD INTERFACE i2c_bus_interface


/* <add id="i2c_device_interface_t">
   <insert-until text="// ADD INTERFACE i2c_device_interface"/>
   Deprecated interface, use <iface>i2c_master_v2</iface> and
   <iface>i2c_slave_v2</iface> instead.

   The <iface>i2c_device</iface> interface is implemented by I2C
   devices. The <fun>set_state</fun> function is used to change the
   state of the I2C device. The I2C device accepts the state change by
   returning <tt>0</tt>. The I2C device must accept calls to
   <fun>read_data</fun> when its state is
   <const>I2C_slave_transmit</const> and calls to
   <fun>write_data</fun> when its state is
   <const>I2C_slave_receive</const>. See the description for the
   <iface>i2c_bus</iface> interface for more information how to use
   this interface.

   </add>
   <add id="i2c_device_interface_exec_context">
   Cell Context for all methods.
   </add>
*/
SIM_INTERFACE(i2c_device) {
        int (*set_state)(conf_object_t *device, i2c_device_state_t state,
                        uint8 address);
        uint8 (*read_data)(conf_object_t *device);
        void (*write_data)(conf_object_t *device, uint8 value);

};

#define I2C_DEVICE_INTERFACE "i2c_device"
// ADD INTERFACE i2c_device_interface


/* <add id="devs api types">
   <name index="true">i2c_status_t</name>
   <insert id="i2c_status_t DOC"/>
   </add> */

/* <add id="i2c_status_t DOC">
   <ndx>i2c_status_t</ndx>
   <doc>
   <doc-item name="NAME">i2c_status_t</doc-item>
   <doc-item name="SYNOPSIS"><insert id="i2c_status_t def"/></doc-item>
   <doc-item name="DESCRIPTION">

   The <type>i2c_status_t</type> type is used to communicate the
   results of various operations on the I2C link. The type is an enum,
   with the values <const>I2C_status_success</const>,
   <const>I2C_status_noack</const> and
   <const>I2C_status_bus_busy</const>.

   The <type>i2c_status_t</type> type typically represents an ACK bit;
   in this case <const>I2C_status_success</const> corresponds to 0,
   and <const>I2C_status_noack</const> corresponds to 1. In the
   <fun>start_response</fun> function of the <iface>i2c_master</iface>
   interface, the <type>i2c_status_t</type> parameter is additionally
   allowed to take the value <const>I2C_status_bus_busy</const>,
   meaning that the start failed since some other master is active
   using the i2c link. The value <const>I2C_status_bus_busy</const> is
   disallowed in all other function parameters in the
   <iface>i2c_link</iface>, <iface>i2c_slave</iface> and
   <iface>i2c_master</iface> interfaces.

   </doc-item>

   <doc-item name="SEE ALSO"><iface>i2c_link_interface_t</iface>,
   <iface>i2c_master_interface_t</iface>, <iface>i2c_slave_interface_t</iface>
   </doc-item>

   </doc></add>
*/
/* <add-type id="i2c_status_t def"></add-type>
 */
typedef enum {
        /* The ACK bit related to the operation was 0. This typically
           means that the operation was successful */
        I2C_status_success = 0,
        /* The ACK bit related to the operation was 1. This typically
           means that the operation was unsuccessful */
        I2C_status_noack = 1,
        /* The operation could not be carried out, because the link is
           currently in use by another master */
        I2C_status_bus_busy
} i2c_status_t;

/* <add id="i2c_link_interface_t">
   <insert-until text="// ADD INTERFACE i2c_link_interface"/>
   Deprecated interface, use <iface>i2c_master_v2</iface> and
   <iface>i2c_slave_v2</iface> instead.

   The <iface>i2c_link</iface> interface is implemented by the I2C
   link. The interface is used by I2C devices to communicate with the
   I2C link.

   An I2C device implements either the <iface>i2c_slave</iface> or the
   <iface>i2c_master</iface> interface, or both. You don't need to do
   anything to connect an I2C master device to a link.

   To connect an I2C slave device to an I2C link, call
   <fun>register_slave_address</fun> with an <em>address pattern</em>
   consisting of the <param>address</param> and <param>mask</param>
   parameters. When there is traffic on the I2C link (as initiated by a
   call to the link interface <fun>start_request</fun> function), the
   target address is matched against each registered device by
   checking if <tt>(target_address ^ device_address) &amp; device_mask
   == 0</tt>; i.e., the bits that are set to 1 in the mask indicates
   which bits in the address that must match.

   There are three different <em>addressing modes</em> defined by this
   protocol: <em>7-bit addressing</em>, <em>General call</em>, and
   <em>10-bit addressing</em>. 7-bit addressing is by far the most
   common addressing mode, and at the moment the only one supported by
   the official <iface>i2c_link</iface> device. When 7-bit addressing is
   used, the address fits into the lower 8 bits of the
   <param>address</param> parameter; in other addressing modes more
   than 8 bits can be used, but the addressing mode can always be
   deduced from the lower 8 address bits.

   In 7-bit addressing mode, the address of an I2C device is encoded
   as an even 8-bit number, in the range <tt>0x10</tt> -
   <tt>0xef</tt>.  In other words, address ranges <tt>0x00</tt> -
   <tt>0x0f</tt> and <tt>0xf0</tt> - <tt>0xff</tt> are reserved for
   other addressing modes, and the 7-bit address is defined by bits 7
   to 1 (little-endian) of the 8-bit encoded address. Bit 0 is reserved as a
   read/write bit for the <fun>start_request</fun> function, and
   should be 0 in both <param>address</param> and 0 in
   <param>mask</param>, while bits 8 to 31 should be 0 in
   <param>address</param> and 1 in <param>mask</param>.

   It is an error to register two I2C slave devices to the same 7-bit
   address.

   Use <fun>disconnect_device</fun> to completely disconnect an I2C
   slave device from the I2C link. If you just intend to disable
   the I2C functionality of the device (without disconnecting the
   wire), use <fun>unregister_slave_address</fun>, and use the same
   <param>address</param> and <param>mask</param> attributes as when
   the device was registered. An I2C slave device can also remove
   some part of the address match by unregistering itself with a
   different mask.

   An I2C transfer is initiated by a master I2C device. The I2C device
   responding to the transfer is called slave. The master starts a
   transfer by calling the <fun>start_request</fun> function with
   <attr>address</attr> as argument. In 7-bit addressing mode, the
   <attr>address</attr> is the 7-bit address plus a read-write bit
   (read-write = 0 &rarr; slave-receive, read-write = 1 &rarr;
   slave-transmit). The read-write bit is the least significant
   bit. This means that all odd values sent to <fun>start</fun>
   function initiates a transfer where the master is requesting data
   from the slave.

   I2C slave devices implement the <iface>i2c_slave</iface>
   interface. The <iface>i2c_slave</iface> interface and the
   <iface>i2c_link</iface> interface have identical
   <fun>start_request</fun>, <fun>read_request</fun>,
   <fun>ack_read_request</fun> and <fun>write_request</fun> functions
   to transfer data over the link. See the <iface>i2c_slave</iface>
   interface for the definitions of these functions.

   I2C master devices implement the <iface>i2c_master</iface>
   interface, which has the functions <fun>start_response</fun>,
   <fun>read_response</fun>, <fun>ack_read_response</fun> and
   <fun>write_response</fun> in common with the
   <iface>i2c_link</iface> interface. The only difference in
   definition is that the <var>I2C_status_bus_busy</var> value is not
   allowed for the <param>status</param> parameter of
   <iface>start_response</iface> in the <iface>i2c_link</iface>
   interface, while it is allowed in the <iface>i2c_master</iface>
   interface. The definitions of these functions can be found in the
   documentation of the <iface>i2c_master</iface> interface.

   Here are the steps for a transfer:

   <ol>
     <li>The I2C master device calls <fun>start_request</fun> in
     the I2C link with 7-bit address and 1-bit read/write flag.
     </li>

     <li>The I2C link calls <fun>start_request</fun> in the I2C slave
     device, forwarding the address given by the master. If there is
     no slave device on the given address, or if the link is busy with
     another connection, the I2C link instead terminates the connection
     by calling <fun>start_response</fun> in the master device with
     <var>I2C_status_noack</var> or <var>I2C_status_bus_busy</var>
     as status code.
     </li>

     <li>The I2C slave device calls <fun>start_response</fun> in
     the I2C link, with either <var>I2C_status_success</var> or
     <var>I2C_status_noack</var> as status code.
     </li>

     <li>The I2C link calls <fun>start_response</fun> in the I2C
     master device. This terminates the transfer if the status wasn't
     <var>I2C_status_success</var>.
     </li>

     <li>The I2C master calls the <fun>read_request</fun> or
     <fun>write_request</fun> function in the I2C link, depending on
     the read/write bit given in the start_request message. The I2C
     link relays the call to the I2C slave <fun>read_request</fun> or
     <fun>write_request</fun> function. The slave responds with a call
     to <fun>read_response</fun> or <fun>write_response</fun>, which
     the link relays to the I2C master device.  If it's a read
     transfer, the master calls the <fun>ack_read_request</fun>
     function in the I2C link, which is relayed to the slave. The
     slave responds with a call to the <fun>ack_read_response</fun> in
     the I2C link, which is relayed to the master.  This step can be
     repeated any number of times.
     </li>

     <li> The I2C master ends the transfer either by calling
     <fun>start_request</fun> again (possibly with a different slave
     address), or by calling <fun>stop</fun> in the I2C link. The link
     will respond by calling <fun>stop</fun> in the slave device (if
     it wasn't a repeated start with the same device as slave), and if
     it was a start command, relay it to the slave device as in step 2
     above.  Before a master device is allowed to call
     <fun>start_request</fun> or <fun>stop</fun>, it must wait for any
     pending <fun>read_response</fun> or <fun>write_response</fun>
     call from a slave device.  In a read transfer, the last call to
     the function <fun>ack_read_request</fun> should pass
     <var>I2C_status_noack</var> in the <param>ack</param> parameter.
     </li>
   </ol>

   In total, an I2C link can be in nine different states:

   <ol>

     <li>Idle, waiting for <fun>start_request</fun> call from any master
     device</li>

     <li>Waiting for a <fun>start_response</fun> call from the slave
     device (write mode)</li>

     <li>Waiting for a <fun>write_request</fun> (or <fun>stop</fun> or
     repeated <fun>start_request</fun>) call from the current master
     device</li>

     <li>Waiting for a <fun>write_response</fun> call from the slave
     device</li>

     <li>Waiting for <fun>start_response</fun> call from the slave
     device (read mode)</li>

     <li>Waiting for a <fun>read_request</fun> (or <fun>stop</fun> or
     repeated <fun>start_request</fun>) call from the current master
     device</li>

     <li>Waiting for a <fun>read_response</fun> call from the slave
     device</li>

     <li>Waiting for an <fun>ack_read_request</fun> call from the
     current master device</li>

     <li>Waiting for an <fun>ack_read_response</fun> call from the slave
     device</li>
   </ol>

   During a transfer, the actual data is delivered by the
   <fun>write_request</fun> and <fun>read_response</fun> function
   calls. The data is always delivered one byte at a time.

   There are two addressing modes apart from the 7-bit addressing mode
   described above:

   <ul>

     <li>General call address: This is a kind of broadcast signal; an
     I2C master device writes data to the link, that any I2C device
     can listen to. A General call is issued by calling
     <fun>request_start</fun> with <param>address</param> = 0. To
     listen to General call, call <fun>register_slave_address</fun>
     with <param>addr</param> = <tt>0</tt> and <param>mask</param> =
     <tt>0xffffffff</tt>. Multiple slave devices can register to the
     General call address; calls to <fun>start_request</fun> and
     <fun>write_request</fun> will be relayed to each registered slave
     device. The <fun>start_response</fun> and
     <fun>write_response</fun> functions in the calling master's
     <iface>i2c_master</iface> interface will be called when
     <em>all</em> slave devices have called the corresponding function
     in the <iface>i2c_link</iface> interface.</li>

     <li>10-bit addresses: This is an extension to the protocol, where
     two bytes are sent as address, of which 10 bits are interpreted
     as address bits. The address sent to
     <fun>register_slave_address</fun> consists of the two bytes that
     are sent, with the first sent byte as the least significant
     byte. This means that the bits of the <param>address</param>
     parameter are defined as follows:

     <ul>
       <li>Bit 0 = 0</li>
       <li>Bits 1-2 correspond to bits 8-9 of the 10-bit address.</li>
       <li>Bits 3 = 0</li>
       <li>Bits 4-7 = all 1</li>
       <li>Bits 8-15 correspond to bits 0-7 of the 10-bit address.</li>
       <li>Bits 16-31 = all 0</li>
     </ul>

     Similarly, the <param>mask</param> parameter should have bit 0
     set to 0 and bits 3-7 and 16-31 set to 1.

     A transmission from a master to a 10-bit addressed slave must
     start with a write transaction, which optionally is followed by a
     read transaction from the same slave. The write transaction is
     started by a <fun>start_request</fun> call, whose
     <param>address</param> parameter is a 16-bit number, composed in
     the same way as the <param>address</param> parameter of
     <fun>register_slave_address</fun> described above, with bit 0 set
     to 0. After writing the desired number of bytes, a repeated start
     command can be issued to start the read transaction to the same
     slave device. The repeated start command is issued by calling
     <fun>start_request</fun>, now with an 8-bit
     <param>address</param> parameter, which consists of the lower 8
     bits from the <param>address</param> parameter of the previous
     <fun>start_request</fun> call, with bit 0 flipped to 1.  </li>

     It is an error to register two I2C slave devices to the same
     10-bit address.
   </ul>

   Note that a single call to <fun>register_slave_address</fun> may
   only register addresses from <em>one</em> addressing mode; it is an
   error to supply an address pattern that matches addresses from
   different addressing modes.

   During write operations, ACK bits are sent in the
   <param>status</param> parameter of the <fun>write_response</fun>
   functions. During read operations, the master device sends the ACK
   bit with the <fun>ack_read_request</fun> function after the
   <fun>read_response</fun> function has been called by the slave. The
   slave should respond with a call to the
   <fun>ack_read_response</fun> function, which is relayed to the
   master.  The value of the ACK bit controls which functions that are
   allowed as the next operation by the master device: If the ACK bit
   is <tt>0</tt> (i.e., <const>I2C_status_success</const>), the next
   operation must be a <fun>read_request</fun>, while ACK = <tt>1</tt>
   (<const>I2C_status_noack</const>) means that the next operation
   must be <fun>stop</fun> or a repeated <fun>start_request</fun>. It
   is an error to violate this rule: If the master device's software
   attempts to issue a STOP or START condition after a read with
   ACK=0, the master device should catch the error and log a
   <const>spec_violation</const> message.

   The <fun>register_bridge</fun> function is used to register a
   device that implements the <iface>i2c_bridge</iface> interface. The
   I2C link device will use the <iface>i2c_bridge</iface> interface to
   keep the I2C bridge device updated with which slave addresses on
   the link that have something registered on them; this is mainly
   useful when implementing a bridge between i2c links. For more
   information, see the documentation on the <nref
   label="__rm_interface_i2c_bridge">i2c_bridge</nref> interface.

   The <fun>disconnect_device</fun> function completely disconnects a
   device (slave, master, bridge or any combination thereof) from the
   link. The link checks which i2c interfaces the device implements, and
   cleanly removes all its references to the device. This mainly has
   following effects:

   <ul>
     <li>The link is prevented from issuing pending
     <fun>response</fun>-like calls to a disconnected master or
     bridge device</li>

     <li>If the device is currently involved in a data transfer, the
     transfer will be aborted as cleanly as possible.</li>

     <li>If the device was registered with the <fun>register_bridge</fun>
     function, its <fun>address_removed</fun> function is called for each
     currently registered i2c slave address.</li>

     <li>The disconnected device is erased from any internal caches;
     this prevents certain memory corruption errors which can occur
     if one device is destroyed and another one is created on exactly
     the same memory location.</li>
   </ul>

   Hence, <fun>disconnect_device</fun> an I2C device which has had any
   kind of interaction with an I2C link, must always use
   <fun>disconnect_device</fun> when disconnecting.
   
   </add>
   <add id="i2c_link_interface_exec_context">
   Cell Context for all methods.
   </add>
*/
SIM_INTERFACE(i2c_link) {
        void (*register_slave_address)(conf_object_t *i2c_link,
                                       conf_object_t *slave,
                                       uint32 address, uint32 mask);
        void (*unregister_slave_address)(conf_object_t *i2c_link,
                                  conf_object_t *slave,
                                  uint32 address, uint32 mask);

        void (*register_bridge)(conf_object_t *i2c_link,
                                conf_object_t *bridge);

        void (*disconnect_device)(conf_object_t *i2c_link,
                                  conf_object_t *device);

        void (*start_request)(conf_object_t *i2c_link, conf_object_t *master,
                              uint32 address);
        void (*read_request)(conf_object_t *i2c_link, conf_object_t *master);
        void (*ack_read_request)(conf_object_t *i2c_link, conf_object_t *master,
                                 i2c_status_t ack);
        void (*write_request)(conf_object_t *i2c_link, conf_object_t *master,
                              uint8 value);

        void (*read_response)(conf_object_t *i2c_link, conf_object_t *slave,
                                 uint8 value);
        void (*ack_read_response)(conf_object_t *i2c_link,
                                  conf_object_t *slave);
        void (*write_response)(conf_object_t *i2c_link, conf_object_t *slave,
                                  i2c_status_t status);
        void (*start_response)(conf_object_t *i2c_link, conf_object_t *slave,
                                  i2c_status_t status);
        void (*stop)(conf_object_t *i2c_link, conf_object_t *master);
};

#define I2C_LINK_INTERFACE "i2c_link"
// ADD INTERFACE i2c_link_interface


/* <add id="i2c_slave_interface_t">
   <insert-until text="// ADD INTERFACE i2c_slave_interface"/>
   Deprecated interface, use <iface>i2c_master_v2</iface> and
   <iface>i2c_slave_v2</iface> instead.

   The <iface>i2c_slave</iface> interface is implemented by devices
   that may act as slaves on an I2C link. The <fun>start_request</fun>
   function is called by the I2C link to start a transaction.  Bits 7
   down to 1 of the <param>address</param> parameter are the address of
   the slave, and bit 0 is the write bit. The started transaction is a
   write transaction if the write bit is <tt>0</tt>, and a read
   transaction otherwise. The I2C slave device accepts the transaction
   by calling the <fun>start_response</fun> function in the I2C
   link. The transaction consists of a number of calls to
   <fun>write_request</fun> (or <fun>read_request</fun>, depending on
   the write bit). The slave responds to each of these with a call to
   <fun>write_response</fun> (or <fun>read_response</fun>) in the I2C
   link. The <fun>ack_read_request</fun> function is called by the I2C
   link as response to the <fun>read_response</fun> function. The
   transfer can end in three ways:

   <ol>
     <li>If a <em>stop</em> condition occurs (i.e., a master calls
     <fun>stop</fun>), <tt>stop(0)</tt> is called in the slave device.
     </li>

     <li>If a <em>repeated start</em> condition occurs, with the same
     device as slave, then the start message is relayed to the slave
     device as usual, and the first transmission is ended by this new
     call to the slave's <fun>start_request</fun> function.  </li>

     <li>If a <em>repeated start</em> condition occurs, with a
     different device as slave, then the first transmission is ended by
     a <tt>stop(1)</tt> call in the first slave device. </li>
   </ol>

   Note that a call to <fun>stop</fun> doesn't necessarily represent
   an I2C stop condition: If the <param>repeated_start</param>
   parameter is <tt>1</tt>, the call actually represents an I2C start
   condition, whose effect is similar to that of a stop condition.

   Note that a call to <fun>ack_read_request</fun> always is followed
   by a call to <fun>read_request</fun> if <param>ack</param> is
   <const>I2C_status_success</const>, or by a call to either
   <fun>stop</fun> or <fun>start_request</fun> if <param>ack</param>
   is <const>I2C_status_noack</const>. Moreover, the interface calls
   to an i2c slave device always follow the regular expression
   <tt>(((R(ra)*rA)|(Ww*))+P)*</tt>, where <tt>R</tt> and <tt>W</tt>
   are calls to <fun>start_request</fun> with odd and even addresses,
   respectively; <tt>r</tt> is <fun>read_request</fun>; <tt>w</tt> is
   <fun>write_request</fun>; <tt>a</tt> and <tt>A</tt> are
   <fun>ack_read_request</fun> calls with <param>ack</param> set to
   <const>I2C_status_success</const> and
   <const>I2C_status_noack</const>, respectively; and <tt>P</tt> is a
   call to <fun>stop</fun>.

   See the description for the <iface>i2c_link</iface> interface for
   more information how to call the <fun>*_response</fun> functions.

   </add>
   <add id="i2c_slave_interface_exec_context">
   Cell Context for all methods.
   </add>
*/
SIM_INTERFACE(i2c_slave) {
        void (*start_request)(conf_object_t *device, uint32 address);
        void (*read_request)(conf_object_t *device);
        void (*ack_read_request)(conf_object_t *device, i2c_status_t ack);
        void (*write_request)(conf_object_t *device, uint8 value);
        void (*stop)(conf_object_t *device, uint8 repeated_start);
};

#define I2C_SLAVE_INTERFACE "i2c_slave"
// ADD INTERFACE i2c_slave_interface

/* <add id="i2c_master_interface_t">
   <insert-until text="// ADD INTERFACE i2c_master_interface"/>
   Deprecated interface, use <iface>i2c_master_v2</iface> and
   <iface>i2c_slave_v2</iface> instead.

   The <iface>i2c_master</iface> interface should be implemented
   by devices that intend to act as a master on an I2C link.

   The functions <fun>start_response</fun>, <fun>read_response</fun>,
   <fun>ack_read_response</fun> and <fun>write_response</fun> are
   called as a response to the corresponding <fun>request_*</fun>
   calls to the I2C link.

   The <param>status</param> parameter to the
   <fun>start_response</fun> function can have three values:
   <dl>
     <dt>I2C_status_success</dt>
     <dd>The transfer was successfully started</dd>

     <dt>I2C_status_noack</dt>
     <dd>No I2C slave device accepted the transfer, likely
     because there was no I2C slave device listening to the given
     address</dd>

     <dt>I2C_status_bus_busy</dt>
     <dd>An existing transfer blocks the I2C link. When the existing
     transfer is completed (i.e., on the next STOP condition), the link
     will call the <fun>bus_freed</fun> function in the master device,
     if it's implemented. This makes it possible to avoid periodical
     polling of a busy link.</dd>
   </dl>

   The <param>status</param> parameter to the
   <fun>write_response</fun> function can have two values,
   <var>I2C_status_success</var> for success, or
   <var>I2C_status_noack</var> if the slave device did not ack the
   written byte.

   </add>
   <add id="i2c_master_interface_exec_context">
   Cell Context for all methods.
   </add>
*/
SIM_INTERFACE(i2c_master) {
        void (*bus_freed)(conf_object_t *device);
        void (*read_response)(conf_object_t *device, uint8 value);
        void (*ack_read_response)(conf_object_t *device);
        void (*write_response)(conf_object_t *device, i2c_status_t status);
        void (*start_response)(conf_object_t *device, i2c_status_t status);
};

#define I2C_MASTER_INTERFACE "i2c_master"
// ADD INTERFACE i2c_master_interface

/* <add id="i2c_bridge_interface_t">
   <insert-until text="// ADD INTERFACE i2c_bridge_interface"/>
   Deprecated interface, use <iface>i2c_master_v2</iface> and
   <iface>i2c_slave_v2</iface> instead.

   This interface is implemented by any device that is registered in
   an I2C link with the <fun>register_bridge</fun>
   function. <fun>register_bridge</fun> will call
   <fun>address_added</fun>, possibly multiple times, to tell the
   bridge which slave addresses that are currently registered in the
   link. Until the bridge device disconnects from the link (using
   <fun>disconnect_device</fun>), the link will continuously call the
   functions <fun>address_added</fun> and <fun>address_removed</fun>
   whenever a slave device registers or unregisters from the link.
   In addition, when an i2c bridge is disconnected from a link, the
   <fun>address_removed</fun> function is called for all currently registered
   slave addresses.

   The <param>addr</param> and <param>mask</param> parameters are
   interpreted in the same way as in the function
   <fun>register_slave_address</fun> in the <iface>i2c_link</iface>
   interface.

   The <iface>i2c_bridge</iface> interface is mainly useful when
   implementing bridges between I2C links: When a bridge forwards
   traffic from one link to another, it needs the information provided
   via this interface to correctly forward all relevant traffic
   between the links, while avoiding to register twice to the same
   address.

   </add>
   <add id="i2c_bridge_interface_exec_context">
   Cell Context for all methods.
   </add>
*/
SIM_INTERFACE(i2c_bridge) {
        void (*address_added)(conf_object_t *device,
                              uint32 addr, uint32 mask);
        void (*address_removed)(conf_object_t *device,
                                uint32 addr, uint32 mask);
};
#define I2C_BRIDGE_INTERFACE "i2c_bridge"
// ADD INTERFACE i2c_bridge_interface

#endif /* 6 or SHOW_OBSOLETE_API */

#if defined(__cplusplus)
}
#endif

#endif
