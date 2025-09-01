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

#ifndef __SHARED_CONSOLE_H__
#define __SHARED_CONSOLE_H__

#include <stdio.h>
#include <stdint.h>

#define shared_printf(fmt, ...) \
  do { \
    sprintf(buffer, fmt, __VA_ARGS__); \
    shared_con_put_string(buffer); \
  } while(0)


extern char buffer[1024*1024];  // will be reused from uart. Not that clean...
void shared_con_put_char(char c);
void shared_con_put_string(const char* msg);
void shared_con_put_char_nc(char c);
void shared_con_put_string_nc(const char* msg);
// Get the ID of the current subsystem, as seen by the shared console object
// i.e., based on the ID added on the path from processor to memory. 
//   0xffff_ffff_ffff_ffff is returned if no ID is found
uint64_t shared_con_get_subsystem_id(void);  

#endif

