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

#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <simics/base/conf-object.h>

/* Interface implemented by the tcp_server user:  */
typedef struct {
        /* Data received from TCP connection */
        void (*received)(conf_object_t *obj, bytes_t data);
} tcp_served_interface_t;

void init_tcp_server();

#endif
