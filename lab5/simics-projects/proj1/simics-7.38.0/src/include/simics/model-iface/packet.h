/*
  © 2018 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_MODEL_IFACE_PACKET_H
#define SIMICS_MODEL_IFACE_PACKET_H

#include <simics/device-api.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/*
   <add id="packet_t">

   The <iface>packet</iface> interface is implemented by objects that
   transfer chunks of data without exposing the underlying
   structure of the data.

   The <fun>transfer</fun> function is called to transfer data from one device
   to another.

   <insert-until text="// ADD INTERFACE packet_t"/>

   </add>
*/

SIM_INTERFACE(packet) {
        void (*transfer)(conf_object_t *obj, bytes_t data);
};

#define PACKET_INTERFACE "packet"
// ADD INTERFACE packet_t

#if defined(__cplusplus)
}
#endif

#endif /* SIMICS_MODEL_IFACE_PACKET_H */
