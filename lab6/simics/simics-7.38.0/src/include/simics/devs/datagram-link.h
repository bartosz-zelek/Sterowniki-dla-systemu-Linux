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

#ifndef SIMICS_DEVS_DATAGRAM_LINK_H
#define SIMICS_DEVS_DATAGRAM_LINK_H

#include <simics/pywrap.h>
#include <simics/base/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

/*
  <add id="datagram_link_interface_t">
  <insert id="datagram_link_interface_t_desc"/>

  This interface is implemented by objects that receive messages from a
  datagram-link, and by the datagram-link connection endpoints themselves.

  There is a single function <fun>receive()</fun>, which is used to pass the
  message <param>msg</param> to the object <param>obj</param>. 

  The message <param>msg</param> is treated as a series of bytes with no
  special format or meaning expected. If <param>obj</param> is a datagram-link
  endpoint, it will forward the message to all other endpoints registered on
  the link except the sender, effectively broadcasting the message on the
  link. If <param>obj</param> is a device, it will simply receive the message
  as sent by the original sender.

  Note that the symmetry of the interface allows two devices to be connected
  directly to each other and talk as if connected via a datagram-link. This is
  however not supported by the default datagram-link component, so a special
  connector must be created for this purpose. Additionally, the standard link
  features, such as multicell configurations and latency, will not be available
  in that setup.

  </add>
  
  <add id="datagram_link_interface_exec_context">
  <table border="false">
  <tr><td><fun>receive</fun></td><td>Cell Context</td></tr>
  </table>
  </add>

  <add id="datagram_link_interface_t_desc">
  <insert-until text="// END datagram_link"/>
  </add>
*/
SIM_INTERFACE(datagram_link) {
        /* Transmit a message to the object. */
        void (*receive)(conf_object_t *NOTNULL obj, bytes_t msg);
};

#define DATAGRAM_LINK_INTERFACE "datagram_link"
// END datagram_link

#if defined(__cplusplus)
}
#endif

#endif
