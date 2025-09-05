/*
  Source code for Simics tranings and demos. Licensed for use with Simics.

  © 2018 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef HARDWARE_INTERFACE_H
#define HARDWARE_INTERFACE_H

#include <stdbool.h>

// Info
#define  DISPLAY_HEIGHT   8
#define  DISPLAY_WIDTH    8

// Check state of buttons and toggles
void      hw_wait_for_buttonA(void);
bool      hw_buttonB_pressed(void);
uint32_t  hw_get_toggle(int toggleno);

// Update display
void      hw_update_frame_buffer(uint32_t *buf);

// Request redraw of the display
void      hw_request_redraw(void);

// Open and close the device nodes 
int      hw_open_dev_nodes(const char *devnodearg);
void     hw_close_dev_nodes(void);

#endif // HARDWARE_INTERFACE_H


