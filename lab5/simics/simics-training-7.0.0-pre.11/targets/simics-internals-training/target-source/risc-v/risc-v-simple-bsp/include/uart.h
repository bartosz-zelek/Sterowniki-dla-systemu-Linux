/*
  © 2024 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef __UART_H__
#define __UART_H__

#include <stdio.h>

#define bsp_printf(...) \
  do { \
    sprintf(buffer, __VA_ARGS__); \
    to_uart(buffer); \
  } while(0)

extern char buffer[1024*1024]; 
void init_uart();
void to_uart(const char* msg);

#endif
