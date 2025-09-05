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

#include <stdint.h>

#include "color-code.h"

//-----------------------------------------------------------------------
//
// Color mapping:
// - Produce a little-endian uint32_t [RR, GG, BB, Alpha] => AABBGGRR
//
//-----------------------------------------------------------------------
#define RED    0x000000FF
#define BLUE   0x00FF0000
#define GREEN  0x0000FF00

uint32_t colorcode(const char c) {
  switch(c) {
  case ' ':
    return 0x00000000;
  case 'K':
    return 0x00000000;
  case 'R':
    return RED;
  case 'G':
    return GREEN;
  case 'B':
    return BLUE;
  case 'C':
    return BLUE+GREEN;
  case 'Y':
    return GREEN+RED;
  case 'M':
    return BLUE+RED;
  case '.':
    return RED+BLUE+GREEN;
  case 'W':
    return RED+BLUE+GREEN;
  }
  return 0x80808080;  // easy to spot in memory,
                     // even though the display will just
                    // show it as white
}


