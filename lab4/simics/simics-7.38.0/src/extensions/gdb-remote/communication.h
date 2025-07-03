/*
  communication.h - Remote GDB connectivity via TCP/IP

  © 2010 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include "gdb-remote.h"

int socket_write(gdb_remote_t *gdb, const void *buf, int len);
void deactivate_gdb_notifier(gdb_remote_t *gdb);
void activate_gdb_notifier(gdb_remote_t *gdb);
void gdb_disconnect(gdb_remote_t *gdb);
void send_packet(gdb_remote_t *gdb, const char *cmd);
void send_packet_no_log(gdb_remote_t *gdb, const char *cmd);
void read_gdb_data(void *param);

#endif
