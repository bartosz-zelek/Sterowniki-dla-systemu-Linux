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

#ifndef SIMICS_DEVS_IEEE_802_3_H
#define SIMICS_DEVS_IEEE_802_3_H

#include <simics/base/types.h>
#include <simics/pywrap.h>
#include <simics/util/frags.h>
#include <simics/util/dbuffer.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef enum {
        IEEE_no_media,      /* not connected at all */
        IEEE_media_unknown, /* media type is unknown */
        IEEE_10base2,       /* 10 Mb/s CSMA/CD LAN over RG 58 coaxial cable */
        IEEE_10base5,       /* 10 Mb/s CSMA/CD LAN over coaxial cable
                               (i.e., thicknet) */
        IEEE_10baseF,       /* 10 Mb/s CSMA/CD LAN over fiber optic cable */
        IEEE_10baseT,       /* 10 Mb/s CSMA/CD LAN over two pairs of
                               twisted-pair telephone wire */
        IEEE_100baseFX,     /* 100 Mb/s CSMA/CD LAN over two optical fibers */
        IEEE_100baseT,      /* 100 Mb/s CSMA/CD LAN */
        IEEE_100baseT2,     /* 100 Mb/s CSMA/CD LAN over two pairs of cat 3
                               or better balanced cabling */
        IEEE_100baseT4,     /* 100 Mb/s CSMA/CD LAN over four pairs of cat 3,
                               4, and 5 UTP wire */
        IEEE_100baseTX,     /* 100 Mb/s CSMA/CD LAN over two pairs of cat 5
                               UTP or STP wire */
        IEEE_100baseX,      /* 100 Mb/s CSMA/CD LAN with PMD and MDI of
                               ISO/IEC 9314 by ASC X3T12 (FDDI)   */
        IEEE_1000baseX,     /* 1000 Mb/s CSMA/CD LAN derived from
                               ANSI X3.230-1994 (FC-PH) [B20]11         */
        IEEE_1000baseCX,    /* 1000BASE-X over specially shielded balanced
                               copper jumper cable assemblies  */
        IEEE_1000baseLX,    /* 1000BASE-X using long wavelength laser over
                               multimode and single-mode fiber */
        IEEE_1000baseSX,    /* 1000BASE-X using short wavelength laser over
                               multimode fiber */
        IEEE_1000baseT,     /* 1000 Mb/s CSMA/CD LAN using four pairs of cat 5
                               balanced copper cabling     */
        IEEE_10broad36,     /* 10 Mb/s CSMA/CD LAN over single broadband
                               cable */
        IEEE_1base5         /* 1 Mb/s CSMA/CD LAN over two pairs of
                               twisted-pair telephone wire */
} ieee_802_3_media_t;

typedef enum {
        IEEE_half_duplex,
        IEEE_full_duplex
} ieee_802_3_duplex_mode_t;

/* <add-type id="ieee_802_3_link_status_t"></add-type> */
typedef enum {
        IEEE_link_unconnected,
        IEEE_link_down,
        IEEE_link_up
} ieee_802_3_link_status_t;

/* <add id="ieee_802_3_phy_interface_t">
   <insert-until text="// ADD INTERFACE ieee_802_3_phy_interface"/>

   Interface that should be implemented by 802.3 physical layers.
   Deprecated; use <iface>ieee_802_3_phy_v2</iface> instead.

   </add>
   <add id="ieee_802_3_phy_interface_exec_context">Undefined.</add>
*/
#define IEEE_802_3_PHY_INTERFACE "ieee_802_3_phy"
SIM_INTERFACE(ieee_802_3_phy) {

        int (*send_frame)(conf_object_t *obj, dbuffer_t *buf, int replace_crc);
        int (*check_tx_bandwidth)(conf_object_t *obj);
#if !defined(PYWRAP)
        void (*add_mac)(conf_object_t *obj, const uint8 *mac);
        void (*del_mac)(conf_object_t *obj, const uint8 *mac);
        void (*add_mac_mask)(conf_object_t *obj, const uint8 *mac,
                             const uint8 *mask);
        void (*del_mac_mask)(conf_object_t *obj, const uint8 *mac,
                             const uint8 *mask);
#endif
        void (*set_promiscous_mode)(conf_object_t *obj, int enable);

};
// ADD INTERFACE ieee_802_3_phy_interface

/* <add id="ieee_802_3_phy_v2_interface_t">
   <insert-until text="// ADD INTERFACE ieee_802_3_phy_v2_interface"/>

   Deprecated; use <iface>ieee_802_3_phy_v3</iface> instead.

   Interface that should be implemented by 802.3 physical layers.

   The <fun>send_frame</fun> function is used by a device to send an
   Ethernet frame. The frame should be a
   <type><idx>dbuffer_t</idx></type> containing a complete Ethernet frame.
   excluding the preamble and SFD, but including the CRC.  The
   <param>replace_crc</param> flag indicates whether the CRC is
   not actually calculated yet. The passed <param>buf</param> should
   not be modified by the called function.
   If the function return 0, the frame was sent to the link; In case
   -1 is returned, there was not enough bandwidth available right now,
   and the frame could not be sent. The PHY should call the
   <fun>tx_bandwidth_available</fun> in the <iface>ieee_802_3_mac</iface>
   interface at the MAC, when the frame can be sent.

   The <fun>check_tx_bandwidth</fun> can also be used to check that there
   is bandwidth available, without sending a frame. It returns 0 if there
   is no bandwidth available, and a positive value if the frame can be
   sent right away.

   <fun>add_mac</fun>, <fun>del_mac</fun> and <fun>set_promiscous_mode</fun>
   have the same functionality as the equivalent
   functions in <iface>ethernet_link</iface> interface. They do nothing
   if the PHY is connected to the new-style links using the
   <iface>ethernet_common</iface> interface.

   The word "promiscuous" is misspelled in the interface.

   </add>
   <add id="ieee_802_3_phy_v2_interface_exec_context">
   Cell Context for all methods.
   </add>
*/
#define IEEE_802_3_PHY_V2_INTERFACE "ieee_802_3_phy_v2"
SIM_INTERFACE(ieee_802_3_phy_v2) {
        int (*send_frame)(conf_object_t *obj, dbuffer_t *buf, int replace_crc);
        int (*check_tx_bandwidth)(conf_object_t *obj);
        void (*add_mac)(conf_object_t *obj, byte_string_t mac);
        void (*del_mac)(conf_object_t *obj, byte_string_t mac);
        void (*add_mac_mask)(conf_object_t *obj, byte_string_t mac,
                             byte_string_t mask);
        void (*del_mac_mask)(conf_object_t *obj, byte_string_t mac,
                             byte_string_t mask);
        void (*set_promiscous_mode)(conf_object_t *obj, int enable);
};
// ADD INTERFACE ieee_802_3_phy_v2_interface

/* <add id="ieee_802_3_phy_v3_interface_t">
   <insert-until text="// ADD INTERFACE ieee_802_3_phy_v3_interface"/>

   Interface that should be implemented by 802.3 physical layers.

   The <fun>send_frame</fun> function is used by a device to send an Ethernet
   frame. The frame should be a <type><idx>frags_t</idx></type> containing a
   complete Ethernet frame, excluding the preamble and SFD, but including space
   for the CRC field. The passed <param>frame</param> must not be modified by
   the called function.

   The <param>replace_crc</param> flag indicates whether the CRC field contents
   can be modified by the implementing object: if <param>replace_crc</param> is
   not set, the implementing object will leave the CRC field untouched; if
   <param>replace_crc</param> is set, the implementing object is free to
   rewrite the CRC field according to the link constraints. Note that in many
   cases, setting <param>replace_crc</param> to true will allow the link to
   assume the CRC field to be matching the frame contents, thus skipping CRC
   calculation and improving simulation performance. <param>replace_crc</param>
   should only be set to false when the device wants to send a frame with a CRC
   field not matching the frame contents.

   If the function return 0, the frame was sent to the link; In case
   -1 is returned, there was not enough bandwidth available right now,
   and the frame could not be sent. The PHY should call the
   <fun>tx_bandwidth_available</fun> in the <iface>ieee_802_3_mac_v3</iface>
   interface at the MAC, when the frame can be sent.

   The <fun>check_tx_bandwidth</fun> can also be used to check that there
   is bandwidth available, without sending a frame. It returns 0 if there
   is no bandwidth available, and a positive value if the frame can be
   sent right away.

   </add>
   <add id="ieee_802_3_phy_v3_interface_exec_context">
   Cell Context for all methods.
   </add>
*/
#define IEEE_802_3_PHY_V3_INTERFACE "ieee_802_3_phy_v3"
SIM_INTERFACE(ieee_802_3_phy_v3) {
        int (*send_frame)(
                conf_object_t *obj, const frags_t *frame, int replace_crc);
        int (*check_tx_bandwidth)(conf_object_t *obj);
};
// ADD INTERFACE ieee_802_3_phy_v3_interface

/* <add id="ieee_802_3_mac_interface_t">
   <insert-until text="// ADD INTERFACE ieee_802_3_mac_interface"/>

   <insert id="ieee_802_3_link_status_t"/>

   Deprecated; use <iface>ieee_802_3_mac_v3</iface> instead.

   Interface that should be implemented by 802.3 media access control layers.

   The <fun>receive_frame</fun> function is called when a frame has
   been received by the phy.  The frame is passed as a
   <type><idx>dbuffer_t</idx></type> that may not be modified without
   cloning it first. The return value have no meaning, callers should
   ignore it, and new implementations should return 0.

   The <fun>tx_bandwidth_available</fun> is called by the PHY when a
   previous call to <fun>send_frame</fun> or <fun>check_tx_bandwidth</fun>
   in the <iface>ieee_802_3_phy</iface> interface have returned no bandwidth
   available.

   <fun>link_status_changed</fun> is called when the phy detects a change
   of the link status.

   The <param>phy</param> parameter is a number that identifies this particular
   PHY, in configurations with several PHYs connected to the same MAC.

   </add>
   <add id="ieee_802_3_mac_interface_exec_context">
   Cell Context for all methods.

   </add>
*/
#define IEEE_802_3_MAC_INTERFACE "ieee_802_3_mac"
SIM_INTERFACE(ieee_802_3_mac) {
        int (*receive_frame)(conf_object_t *obj, int phy,
                             dbuffer_t *buf, int crc_ok);
        void (*tx_bandwidth_available)(conf_object_t *obj, int phy);
        void (*link_status_changed)(conf_object_t *obj, int phy,
                                    ieee_802_3_link_status_t status);
};
// ADD INTERFACE ieee_802_3_mac_interface

/* <add id="ieee_802_3_mac_v3_interface_t">
   <insert-until text="// ADD INTERFACE ieee_802_3_mac_v3_interface"/>

   <insert id="ieee_802_3_link_status_t"/>

   Interface that should be implemented by 802.3 media access control layers.

   The <fun>receive_frame</fun> function is called when a frame has been
   received by the phy.  The frame is passed as a
   <type><idx>frags_t</idx></type> that must not be modified. 

   The <param>crc_ok</param> flag is set to indicate that the frame is valid
   with regards to CRC calculations. If the <param>crc_ok</param> flag is not
   set, the frame is considered invalid. Note that in general,
   <param>crc_ok</param> does not indicate whether or not the CRC field in the
   frame can be relied upon, or whether its computation has been skipped to
   improve simulation performance. When using new-style links,
   <param>crc_ok</param> set to false indicates that the CRC field contains
   valid information that is not matching with the frame contents.

   The <fun>tx_bandwidth_available</fun> is called by the PHY when a
   previous call to <fun>send_frame</fun> or <fun>check_tx_bandwidth</fun>
   in the <iface>ieee_802_3_phy_v3</iface> interface have returned no bandwidth
   available.

   <fun>link_status_changed</fun> is called when the phy detects a change
   of the link status.

   The <param>phy</param> parameter is deprecated and should be ignored.

   </add>
   <add id="ieee_802_3_mac_v3_interface_exec_context">
   Cell Context for all methods.

   </add>
*/
#define IEEE_802_3_MAC_V3_INTERFACE "ieee_802_3_mac_v3"
SIM_INTERFACE(ieee_802_3_mac_v3) {
        void (*receive_frame)(
                conf_object_t *obj, int phy, const frags_t *frame, int crc_ok);
        void (*tx_bandwidth_available)(conf_object_t *obj, int phy);
        void (*link_status_changed)(
                conf_object_t *obj, int phy, ieee_802_3_link_status_t status);
};
// ADD INTERFACE ieee_802_3_mac_v3_interface

#if defined(__cplusplus)
}
#endif

#endif
