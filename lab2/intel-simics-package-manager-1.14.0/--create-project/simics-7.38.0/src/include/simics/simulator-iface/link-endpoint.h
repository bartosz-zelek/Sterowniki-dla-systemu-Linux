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

#ifndef SIMICS_SIMULATOR_IFACE_LINK_ENDPOINT_H
#define SIMICS_SIMULATOR_IFACE_LINK_ENDPOINT_H

#include <simics/simulator/follower-time.h>
#include <simics/base/conf-object.h>
#include <simics/pywrap.h>

#ifdef __cplusplus
extern "C" {
#endif

SIM_INTERFACE(link_endpoint_v2) {
        /* Delivery time and secondary key for the message currently being
           delivered to the device when using indirect delivery. */
        follower_time_t (*delivery_time)(conf_object_t *obj);
        uint64 (*delivery_skey)(conf_object_t *obj);
};
#define LINK_ENDPOINT_V2_INTERFACE "link_endpoint_v2"

#ifdef __cplusplus
}
#endif

#endif
