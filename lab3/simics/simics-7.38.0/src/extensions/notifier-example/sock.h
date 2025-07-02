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

#ifndef SOCK_H
#define SOCK_H

#ifdef _WIN32
 #include <ws2tcpip.h>
 #include <windows.h>
 typedef SOCKET sock_t;
 #define INVALID_SOCK INVALID_SOCKET
#else
 typedef int sock_t;
 #define INVALID_SOCK (-1)
#endif

#include <simics/base/conf-object.h>

void close_sock(sock_t s);
sock_t open_passive_socket(conf_object_t *obj, int port);
int get_local_socket_port(conf_object_t *obj, sock_t s);
sock_t sock_accept(sock_t passive, uint8 *addr);
int sock_read(sock_t s, void *buf, int len);
int sock_errno();

#endif
