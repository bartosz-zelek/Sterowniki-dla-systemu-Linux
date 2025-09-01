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
#include "shared_console.h"

extern void* shared_con_base;

inline static void shared_con_put_char_common(char c, char nc) {
  volatile char* con = (volatile char*)shared_con_base;
  if (nc)
    con += 4;
  *con = c;
}

void shared_con_put_char(char c) {
  shared_con_put_char_common(c, 0);
}

void shared_con_put_char_nc(char c) {
  shared_con_put_char_common(c, 1);
}

inline static void shared_con_put_string_common(const char* msg, char nc) {
  volatile char* con = (volatile char*)shared_con_base;
  if (nc)
    con += 4;
  for(unsigned int i=0; i<strlen(msg); i++)
      *con = msg[i];
}

void shared_con_put_string(const char* msg) {
  shared_con_put_string_common(msg, 0);
}

void shared_con_put_string_nc(const char* msg) {
  shared_con_put_string_common(msg, 1);
}

uint64_t shared_con_get_subsystem_id(void) {
  // address of register 
  volatile uint8_t* idreg_address = (shared_con_base) + 8 ; 
  // read it - given 
  volatile uint64_t* idreg = (volatile uint64_t*)idreg_address;
  return *idreg;
}
