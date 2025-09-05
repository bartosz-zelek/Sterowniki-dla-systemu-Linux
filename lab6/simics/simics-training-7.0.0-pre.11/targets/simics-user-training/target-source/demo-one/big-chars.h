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

#ifndef BIG_CHARS_H
#define BIG_CHARS_H

// Color mapping:
//  Red
//  Green
//  Blue
//  Yellow
//  Cyan
//  Magenta
//  White
//  blacK
uint32_t colorcode(const char c);

// Size as constants
#define  GLYPH_HEIGHT   8
#define  GLYPH_WIDTH    8

// Character map for building display contents
typedef const char glyph_t[GLYPH_WIDTH][GLYPH_HEIGHT];  // 8x8 - standing up

typedef struct glyph_map {
  char	 key;                                // character to match
  glyph_t  glyph;								// how it looks
} glyph_map_t;

glyph_t *glyph_lookup(const char c);

#endif // BIG_CHARS_H
