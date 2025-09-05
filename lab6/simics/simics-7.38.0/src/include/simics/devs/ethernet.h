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

#ifndef SIMICS_DEVS_ETHERNET_H
#define SIMICS_DEVS_ETHERNET_H

#include <simics/util/dbuffer.h>
#include <simics/util/frags.h>
#include <simics/base/time.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

#if !defined(ETHER_CRC_LEN)
 #define ETHER_CRC_LEN 4
#endif
#if !defined(ETHER_MIN_LEN)
 #define ETHER_MIN_LEN 64
#endif
#if !defined(ETHER_MAX_LEN)
 #define ETHER_MAX_LEN 1518
#endif
#if !defined(ETH_ALEN)
 #define ETH_ALEN 6
#endif
#if !defined(ETHERTYPE_PUP)
 #define ETHERTYPE_PUP 0x0200      /* Xerox PUP */
#endif
#if !defined(ETHERTYPE_IP)
 #define ETHERTYPE_IP 0x0800       /* IPv4 */
#endif
#if !defined(ETHERTYPE_IPV6)
 #define ETHERTYPE_IPV6 0x86dd     /* IPv6 */
#endif
#if !defined(ETHERTYPE_ARP)
 #define ETHERTYPE_ARP 0x0806      /* Address resolution */
#endif
#if !defined(ETHERTYPE_REVARP)
 #define ETHERTYPE_REVARP 0x8035   /* Reverse ARP */
#endif

/* <add-type id="eth_frame_crc_status_t">
   <ul>
     <li><tt>Eth_Frame_CRC_Match</tt> means that the frame contents are
     correct. The CRC field in the frame should not be relied upon as its
     computation may have been skipped for optimization, and it may contain any
     value, including zero, a random value or a correctly computed CRC.</li>

     <li><tt>Eth_Frame_CRC_Mismatch</tt> means that the frame contents are
     incorrect. The CRC field in the frame must contain a CRC that does not
     match the frame contents, i.e., to send an incorrect frame on the link,
     you must make sure that the CRC field will not match when computed.</li>

     <li><tt>Eth_Frame_CRC_Unknown</tt> means that the relation between the
     frame contents and the CRC field is unknown. The relation can be
     established by computing the frame's CRC and comparing it to the frame's
     CRC field.</li>
   </ul>
   </add-type>
*/
typedef enum {
        Eth_Frame_CRC_Match,
        Eth_Frame_CRC_Mismatch,
        Eth_Frame_CRC_Unknown
} eth_frame_crc_status_t;

// Common ethernet interfaces used by all ethernet links and devices

/* <add id="ethernet_common_interface_t">
   <insert id="ethernet_common_interface_t_desc"/>

   This interface is implemented by objects that receive Ethernet frames, both
   Ethernet devices and Ethernet link endpoints.

   There is a single function <fun>frame</fun> which sends an Ethernet frame,
   without preamble nor SFD (Start Frame Delimiter), but with a CRC field.

   The <param>crc_status</param> parameter provides out-of-band information on
   the contents of the frame with regards to the CRC field using one of the
   values in the <type>eth_frame_crc_status_t</type> enum:

   <insert id="eth_frame_crc_status_t"/>

   When a device calls a link's <fun>frame</fun> function, it can set
   crc_status to any of the three values. If the link receives a
   <tt>Eth_Frame_CRC_Unknown</tt>, it will compute the CRC itself to set the
   status to <tt>Eth_Frame_CRC_Match</tt> or <tt>Eth_Frame_CRC_Mismatch</tt>.

   When a link calls a device's <tt>frame</tt> function, crc_status will be set
   to either <tt>Eth_Frame_CRC_Match</tt> or <tt>Eth_Frame_CRC_Mismatch</tt>,
   and never <tt>Eth_Frame_CRC_Unknown</tt>.

   When two devices are directly connected to each others without using a link,
   the interpretation of <tt>Eth_Frame_CRC_Unknown</tt> is up to the devices'
   implementation.
   </add>
   <add id="ethernet_common_interface_exec_context">
   <table border="false">
   <tr><td><fun>frame</fun></td><td>Cell Context</td></tr>
   </table>
   </add>
*/

/* <add id="ethernet_common_interface_t_desc">
   <insert-until text="// END ethernet_common"/>
   </add>  
*/
SIM_INTERFACE(ethernet_common) {
        void (*frame)(conf_object_t *NOTNULL obj, const frags_t *frame,
                      eth_frame_crc_status_t crc_status);
};
#define ETHERNET_COMMON_INTERFACE "ethernet_common"
// END ethernet_common

/* <add id="ethernet_cable_interface_t">
   <insert-until text="// END ethernet_cable"/>

   This interface is implemented by Ethernet devices and link endpoints that
   are interested in the link status of the peer.

   The <fun>link_status</fun> function is used to notify the peer that the link
   status at the local end is up or down. The
   <class>ethernet-cable-link</class> class propagates this information to the
   device connected at the other end.

   </add>
   <add id="ethernet_cable_interface_exec_context">
   <table border="false">
   <tr><td><fun>link_status</fun></td><td>Cell Context</td></tr>
   </table>
   </add>
*/
SIM_INTERFACE(ethernet_cable) {
        void (*link_status)(conf_object_t *NOTNULL ep, bool link_up);
};
#define ETHERNET_CABLE_INTERFACE "ethernet_cable"
// END ethernet_cable

/* <add-type id="ethernet_link_snoop_t"></add-type> */
typedef void (*ethernet_link_snoop_t)(lang_void *user_data,
                                      conf_object_t *clock,
                                      const frags_t *packet,
                                      eth_frame_crc_status_t crc_status);

/* <add id="ethernet_snoop common">

   This interface should only be used for inspection, and never as part of the
   actual simulation. The snoop functions must not affect the simulation in any
   way.

   The <param>clock</param> parameter tells the link on which clock to post the
   events that call the snoop function. The snoop function will be called at
   the delivery time of the network packet, which means that it will be called
   at the same time as any Ethernet devices attached to the same clock that
   receives packets from the same link.

   Snooped frames with a matching CRC will contain the correct frame check
   sequence.

   The <param>user_data</param> parameter is passed to the snoop function every
   time it is called.
   </add>
*/

/* <add id="ethernet_snoop_interface_t">
   <insert id="ethernet_link_snoop_t"/>
   <insert-until text="// END ethernet_snoop"/>

   This interface is implemented by Ethernet link objects. It is used to attach
   snoop functions to the link. The snoop function will receive all traffic
   going over the link.

   <insert id="ethernet_snoop common"/>

   </add>
   <add id="ethernet_snoop_interface_exec_context">
   <table border="false">
   <tr><td><fun>attach</fun></td><td>Global Context</td></tr>
   </table>
   </add>
*/
SIM_INTERFACE(ethernet_snoop) {
        conf_object_t *(*attach)(conf_object_t *NOTNULL link,
                                 conf_object_t *clock,
                                 ethernet_link_snoop_t snoop_fun,
                                 lang_void *user_data);
};
#define ETHERNET_SNOOP_INTERFACE "ethernet_snoop"
// END ethernet_snoop

/* Attaching to an Ethernet VLAN network. (Use the b
ethernet-common interface to send traffic.) */

/* <add id="ethernet_vlan_snoop_interface_t">
   <insert id="ethernet_link_snoop_t"/>
   <insert-until text="// END ethernet_vlan_snoop"/>

   This interface is implemented by Ethernet VLAN switch link objects. It is
   used to attach snoop functions to the link. The snoop function will receive
   all traffic going over the link, either on a single VLAN, or on all of them.

   <insert id="ethernet_snoop common"/>

   The <param>vlan_id</param> indicates on which VLAN the snoop function should
   be attached (as its native VLAN).

   The <param>is_vlan_trunk</param> flag indicates whether the snoop function
   should also receive the traffic on all other VLANs, tagged with an 802.1Q
   tag.

   </add>
   <add id="ethernet_vlan_snoop_interface_exec_context">
   <table border="false">
   <tr><td><fun>attach</fun></td><td>Global Context</td></tr>
   </table>
   </add>
*/
SIM_INTERFACE(ethernet_vlan_snoop) {
        conf_object_t *(*attach)(
                conf_object_t *NOTNULL link, conf_object_t *clock,
                ethernet_link_snoop_t snoop_fun, lang_void *user_data,
                uint16 vlan_id, bool is_vlan_trunk);
};
#define ETHERNET_VLAN_SNOOP_INTERFACE "ethernet_vlan_snoop"
// END ethernet_vlan_snoop

/* <add id="break_net_cb_t">
   Callback function used by network breakpoint system, called when associated
   criteria has matched. The <arg>obj</arg> parameter is the ethernet link id
   where the breakpoint matched, <arg>msg</arg> is the message that is given
   back to the manager, <arg>break_id</arg> is the associated
   breakpoint id, returned by the <fun>add</fun> method in the
   <iface>network_breakpoint</iface> interface. The return value is ignored.
   </add>
 */
typedef int (*break_net_cb_t)(conf_object_t *obj, bytes_t data,
                              int len, int64 break_id);
// END break_net_cb_t

/* <add id="network_breakpoint_interface_t">

   The <iface>network_breakpoint</iface> interface is implemented by
   objects that handles ethernet links to have breakpoints.
   The <fun>add</fun> is used to set a breakpoint on a given src mac
   address or gived dst mac address or by ether type. If they are combined
   then the criteria is that all given has to match.
   The <fun>remove</fun> is used to remove an existing
   breakpoint.
   <insert-until text="// END network_breakpoint"/> </add>
   <add id="network_breakpoint_interface_exec_context"> Global Context
   for all methods
   </add>
*/
SIM_INTERFACE(network_breakpoint) {
        int64 (*add)(conf_object_t *NOTNULL obj, bytes_t src_mac_addr,
                     bytes_t dst_mac_addr, int ether_type,
                     break_net_cb_t cb, bool once, int64 bp_id);
        void (*remove)(conf_object_t *NOTNULL obj, int64 bp_id);
};
#define NETWORK_BREAKPOINT_INTERFACE "network_breakpoint"
// END network_breakpoint

#if defined(__cplusplus)
}
#endif

#endif
