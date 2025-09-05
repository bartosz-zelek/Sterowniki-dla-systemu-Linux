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

#include "big-chars.h"

//-----------------------------------------------------------------------
//
// Color mapping:
// - Produce a little-endian uint32_t [RR, GG, BB, Alpha] => AlBBGGRR
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

//----------------------------------------------------------------------
//
// Character shapes
//
//----------------------------------------------------------------------
const glyph_map_t glyphs[] = {
  
  {'A', {"        ",
	 "  CCCC  ",
	 " CC  CC ",
	 " CC  CC ",
	 " CC  CC ",
	 " CCCCCC ",
	 " CC  CC ",
	 "        "}},
  
  {'B', {"        ",
	 " GGGGG  ",
	 " GG  GG ",
	 " GGGGG  ",
	 " GG  GG ",
	 " GG  GG ",
	 " GGGGG  ",
	 "        "}},
  
  {'C', {"        ",
	 "  YYYY  ",
	 " YY  YY ",
	 " YY     ",
	 " YY     ",
	 " YY  YY ",
	 "  YYYY  ",
	 "        "}},

  {'D', {"        ",
	 " YYYYY  ",
	 " YY  YY ",
	 " YY  YY ",
	 " YY  YY ",
	 " YY  YY ",
	 " YYYYY  ",
	 "        "}},

  {'E', {"        ",
	 " CCCCCC ",
	 " CC     ",
	 " CCCCCC ",
	 " CC     ",
	 " CC     ",
	 " CCCCCC ",
	 "        "}},

  {'F', {"        ",
	 " CCCCCC ",
	 " CC     ",
	 " CC     ",
	 " CCCCCC ",
	 " CC     ",
	 " CC     ",
	 "        "}},

  {'G', {"        ",
	 "  YYYY  ",
	 " YY     ",
	 " YY     ",
	 " YY YYY ",
	 " YY  YY ",
	 "  YYYY  ",
	 "        "}},

  {'H', {"        ",
	 " RR  RR ",
	 " RR  RR ",
	 " RRRRRR ",
	 " RR  RR ",
	 " RR  RR ",
	 " RR  RR ",
	 "        "}},

  {'I', {"        ",
	 " MMMMMM ",
	 "   MM   ",
	 "   MM   ",
	 "   MM   ",
	 "   MM   ",
	 " MMMMMM ",
	 "        "}},

  {'J', {"        ",
	 " MMMMMM ",
	 "     MM ",
	 "     MM ",
	 "     MM ",
	 " MM  MM ",
	 "  MMMM  ",
	 "        "}},

  {'K', {"        ",
	 " YY  YY ",
	 " YY  YY ",
	 " YYYYY  ",
	 " YY  YY ",
	 " YY  YY ",
	 " YY  YY ",
	 "        "}},

  {'L', {"        ",
	 " GG     ",
	 " GG     ",
	 " GG     ",
	 " GG     ",
	 " GG     ",
	 " GGGGGG ",
	 "        "}},

  {'M', {"        ",
	 " G    G ",
	 " GG  GG ",
	 " GGGGGG ",
	 " GG  GG ",
	 " GG  GG ",
	 " GG  GG ",
	 "        "}},

  {'N', {"        ",
	 " RR  RR ",
	 " RRR RR ",
	 " RRRRRR ",
	 " RR RRR ",
	 " RR  RR ",
	 " RR  RR ",
	 "        "}},

  {'O', {"        ",
	 "  RRRR  ",
	 " RR  RR ",
	 " RR  RR ",
	 " RR  RR ",
	 " RR  RR ",
	 "  RRRR  ",
	 "        "}},

  {'P', {"        ",
	 " RRRRR  ",
	 " RR  RR ",
	 " RR  RR ",
	 " RRRRR  ",
	 " RR     ",
	 " RR     ",
	 "        "}},

  {'Q', {"        ",
	 "  CCCC  ",
	 " CC  CC ",
	 " CC  CC ",
	 " CC  CC ",
	 " CC CCC ",
	 "  CCCC  ",
	 "    CCC "}},

  {'R', {"        ",
	 " RRRRR  ",
	 " RR  RR ",
	 " RR  RR ",
	 " RRRRR  ",
	 " RR  RR ",
	 " RR  RR ",
	 "        "}},

  {'S', {"        ",
	 "  YYYY  ",
	 " YY     ",
	 " YYYYY  ",
	 "     YY ",
	 " YY  YY ",
	 "  YYYY  ",
	 "        "}},

  {'T', {"        ",
	 " GGGGGG ",
	 "   GG   ",
	 "   GG   ",
	 "   GG   ",
	 "   GG   ",
	 "   GG   ",
	 "        "}},

  {'U', {"        ",
	 " RR  RR ",
	 " RR  RR ",
	 " RR  RR ",
	 " RR  RR ",
	 " RR  RR ",
	 "  RRRR  ",
	 "        "}},

  {'V', {"        ",
	 " BB  BB ",
	 " BB  BB ",
	 " BB  BB ",
	 " BB  BB ",
	 "  BBBB  ",
	 "   BB   ",
	 "        "}},

  {'W', {"        ",
	 " GG  GG ",
	 " GG  GG ",
	 " GG  GG ",
	 " GGGGGG ",
	 " GG  GG ",
	 " G    G ",
	 "        "}},

  {'X', {"        ",
	 " GG  GG ",
	 " GG  GG ",
	 "  GGGG  ",
	 " GG  GG ",
	 " GG  GG ",
	 " GG  GG ",
	 "        "}},

  {'Y', {"        ",
	 " YY  YY ",
	 " YY  YY ",
	 " YY  YY ",
	 "  YYYY  ",
	 "   YY   ",
	 "   YY   ",
	 "        "}},

  {'Z', {"        ",
	 " BBBBBB ",
	 "    BB  ",
	 "   BB   ",
	 "  BB    ",
	 " BB     ",
	 " BBBBBB ",
	 "        "}},

  //// Signs ///////////////
  
  {'.', {"        ",
	 "        ",
	 "  WWWW  ",
	 "  WGGW  ",
	 "  WGGW  ",
	 "  WWWW  ",
	 "        ",
	 "        "}},

  {':', {"        ",
	 "   YY   ",
	 "   YY   ",
	 "        ",
	 "        ",
	 "   YY   ",
	 "   YY   ",
	 "        "}},

  {';', {"        ",
	 "   GG   ",
	 "   GG   ",
	 "        ",
	 "   GG   ",
	 "   GG   ",
	 "  GG    ",
	 "        "}},
  
  {'!', {"        ",
	 "   CC   ",
	 "   CC   ",
	 "   CC   ",
	 "   CC   ",
	 "        ",
	 "   GG   ",
	 "        "}},
  
  {'?', {"        ",
	 "  MMMM  ",
	 "     MM ",
	 "     MM ",
	 "   MMM  ",
	 "        ",
	 "   BB   ",
	 "        "}},

  {'#', {"        ",
	 "  B  B  ",
	 " RMRRMR ",
	 "  B  B  ",
	 "  B  B  ",
	 " RMRRMR ",
	 "  B  B  ",
	 "        "}},

  {'_', {"        ",
	 "        ",
	 "        ",
	 "        ",
	 "        ",
	 "        ",
	 "        ",
	 "BCBCBCB "}},

  {'-', {"        ",
	 "        ",
	 "        ",
	 " YYYYYY ",
	 " RRRRRR ",
	 "        ",
	 "        ",
	 "        "}},

  {' ', {"        ",
     "        ",
     "        ",
     "        ",
     "        ",
     "        ",
     "        ",
     "        "}},



  //// Numbers /////////////
  
  {'0', {"........",
	 ".BBBBBB.",
	 ".BB..BB.",
	 ".BB.BBB.",
	 ".BBB.BB.",
	 ".BB..BB.",
	 ".BBBBBB.",
	 "........"}},
  
  {'1', {"........",
	 "...BB...",
	 "...BB...",
	 "...BB...",
	 "...BB...",
	 "...BB...",
	 "...BB...",
	 "........"}},
  
  {'2', {"........",
	 ".BBBBBB.",
	 ".....BB.",
	 ".BBBBBB.",
	 ".BB.....",
	 ".BB.....",
	 ".BBBBBB.",
	 "........"}},

  {'3', {"........",
	 ".BBBBBB.",
	 ".....BB.",
	 ".BBBBBB.",
	 ".....BB.",
	 ".....BB.",
	 ".BBBBBB.",
	 "........"}},

  {'4', {"........",
	 ".BB..BB.",
	 ".BB..BB.",
	 ".BB..BB.",
	 ".BBBBBB.",
	 ".....BB.",
	 ".....BB.",
	 "........"}},

  {'5', {"........",
	 ".BBBBBB.",
	 ".BB.....",
	 ".BBBBBB.",
	 ".....BB.",
	 ".....BB.",
	 ".BBBBBB.",
	 "........"}},

  {'6', {"........",
	 ".BBBBBB.",
	 ".BB.....",
	 ".BBBBBB.",
	 ".BB..BB.",
	 ".BB..BB.",
	 ".BBBBBB.",
	 "........"}},
  
  {'7', {"........",
	 ".BBBBBB.",
	 ".....BB.",
	 ".....BB.",
	 ".....BB.",
	 ".....BB.",
	 ".....BB.",
	 "........"}},

  {'8', {"........",
	 ".BBBBBB.",
	 ".BB..BB.",
	 ".BBBBBB.",
	 ".BB..BB.",
	 ".BB..BB.",
	 ".BBBBBB.",
	 "........"}},

  {'9', {"........",
	 ".BBBBBB.",
	 ".BB..BB.",
	 ".BB..BB.",
	 ".BBBBBB.",
	 ".....BB.",
	 ".BBBBBB.",
	 "........"}},

  // end marker and "bad character" glyph
  { 0, {"YYYYRRRR",
	"YYYYRRRR",
	"YYYYRRRR",
	"YYYYRRRR",
	"GGGGWWWW",
	"GGGGWWWW",
	"GGGGWWWW",
	"GGGGWWWW"}}
};


//
// Scan for a match in the array
//
glyph_t *glyph_lookup(const char c) {
  int i=0;
  while( glyphs[i].key != 0) {
    // scan for an exact match
    if(glyphs[i].key == c) break;
    // keep going
    i++;
  }
  // this is safe since the end marker has a designated glyph too!
  return &(glyphs[i].glyph);
}


