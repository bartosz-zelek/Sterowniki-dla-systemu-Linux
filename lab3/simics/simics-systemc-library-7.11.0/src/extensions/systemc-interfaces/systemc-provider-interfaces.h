/*
  © 2016 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SYSTEMC_INTERFACES_SYSTEMC_PROVIDER_INTERFACES_H
#define SYSTEMC_INTERFACES_SYSTEMC_PROVIDER_INTERFACES_H

#include <simics/device-api.h>
#include <simics/pywrap.h>

#ifdef __cplusplus
extern "C" {
#endif
        
SIM_INTERFACE(sc_provider_controller) {
    bool (*insert)(conf_object_t *obj, conf_object_t *conn, int pos);
    void (*remove)(conf_object_t *obj, conf_object_t *conn);
};
#define SC_PROVIDER_CONTROLLER_INTERFACE "sc_provider_controller"

SIM_INTERFACE(sc_tool_connection) {
    attr_value_t (*tool)(conf_object_t *obj);
    attr_value_t (*controller)(conf_object_t *obj);
};
#define SC_TOOL_CONNECTION_INTERFACE "sc_tool_connection"
        
#ifdef __cplusplus
}
#endif

#endif
