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

#include <string.h>
#include "uart.h"

extern void* bsp_uart_base;
char buffer[1024*1024]; //1MiB for strings should be enough, right?

void to_uart(const char* msg) {
  volatile char* uart = (volatile char*)bsp_uart_base;
  for(unsigned int i=0; i<strlen(msg); i++)
      *uart = msg[i];
}

void init_uart() {
  volatile char* uart_cfg = (volatile char*)(bsp_uart_base + 0x3);
  *uart_cfg = 0x3; //enable 8 bit mode
}
