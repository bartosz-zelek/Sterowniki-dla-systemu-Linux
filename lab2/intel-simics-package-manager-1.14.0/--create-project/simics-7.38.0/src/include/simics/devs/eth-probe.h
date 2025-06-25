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

#ifndef SIMICS_DEVS_ETH_PROBE_H
#define SIMICS_DEVS_ETH_PROBE_H

#include <simics/util/frags.h>
#include <simics/device-api.h>
#include <simics/devs/ethernet.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add id="eth_probe_side_t">
   <insert-until text="// END eth_probe_side_t"/>
   </add> */
typedef enum {
        Eth_Probe_Port_A = 0,
        Eth_Probe_Port_B = 1
} eth_probe_side_t;
// END eth_probe_side_t

/* <add id="ethernet_probe_snoop_t">
   <insert-until text="// END ethernet_probe_snoop_t"/>
   </add> */
typedef void (*ethernet_probe_snoop_t)(lang_void *user_data,
                                       conf_object_t *probe,
                                       eth_probe_side_t to_side,
                                       const frags_t *frame,
                                       eth_frame_crc_status_t crc_status);
// END ethernet_probe_snoop_t

/* <add id="ethernet_probe_interface_t">
   <insert id="eth_probe_side_t"/>
   <insert id="ethernet_probe_snoop_t"/>
   <insert id="ethernet_probe_interface_t_def"/>

   This interface is implemented by <class>eth-probe</class> objects. Once a
   probe has been inserted between a device and an Ethernet link, the functions
   of this interface can be used to setup callbacks:

   <dl>

   <dt><fun>attach_snooper()</fun></dt> <dd>Attach a snooper function: the
   probe will pass each frame to the snooper function, then forward it
   unchanged where it should be going</dd>

   <dt><fun>attach_probe()</fun></dt> <dd>Attach a probe function: the probe
   will pass each frame to the probe function, and give it the responsibility
   of forwarding the frame or any number of modified or additional frames using
   the <fun>send_frame()</fun> function.</dd>

   <dt><fun>detach</fun></dt> <dd>Detach the currently registered callback from
   the probe.</dd>

   <dt><fun>send_frame</fun></dt> <dd>Send a frame via the probe, either to the
   side A or B of the probe. Which side is which can be obtained with the <cmd
   class="eth-probe">info</cmd> function.</dd>

   </dl>

   <insert id="ethernet_snoop common"/>
   </add>

   <add id="ethernet_probe_interface_exec_context">
   Cell Context for all methods.
   </add>

   <add id="ethernet_probe_interface_t_def">
   <insert-until text="// END ethernet_probe_interface_t"/>
   </add>
*/
SIM_INTERFACE(ethernet_probe) {
        void (*attach_snooper)(conf_object_t *NOTNULL probe,
                               ethernet_probe_snoop_t snoop_fun,
                               lang_void *user_data);
        void (*attach_probe)(conf_object_t *NOTNULL probe,
                             ethernet_probe_snoop_t snoop_fun,
                             lang_void *user_data);
        void (*detach)(conf_object_t *NOTNULL probe);
        void (*send_frame)(conf_object_t *NOTNULL probe,
                           eth_probe_side_t to_side,
                           const frags_t *frame,
                           eth_frame_crc_status_t crc_status);
};

#define ETHERNET_PROBE_INTERFACE "ethernet_probe"
// END ethernet_probe_interface_t

#if defined(__cplusplus)
}
#endif

#endif /* ! SIMICS_DEVS_ETH_PROBE_H */
