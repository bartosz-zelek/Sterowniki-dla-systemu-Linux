/*
  © 2021 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef EXTERNAL_CONNECTION_INIT_H
#define EXTERNAL_CONNECTION_INIT_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
void init_pipe_server();
#else
void init_unix_socket_server();
#endif

void init_tcp_server();

#ifdef __cplusplus
}
#endif

#endif
