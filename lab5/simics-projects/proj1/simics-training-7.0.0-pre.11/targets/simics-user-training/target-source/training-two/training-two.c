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

//
// Program to demonstrate the Simics debugger in action,
// while accessing the training device for visual effects. 
//
// Borrows the hardware access pieces from the demo-one program.
//
// The input string is of the format:
//   String of color codes (WRGBCMYK)
//   Optionally following each code with *NNN to repeat 
//                              an additional NNN times
//
// To have something to show, always fills the entire buffer
// with a striped pattern before inserting the given string
//

#include <stdint.h>
#include <stdio.h>
#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>

// Simics include
#include "simics-magic-instruction.h"

// More pieces of this program
#include "color-code.h"
#include "hardware-interface.h"



//-----------------------------------------------------------------------------------------
//
// Interpret input string
// -- Call with the input string, assumed to be validly zero-terminated
// -- And a buffer that can hold buflen words (linear frame buffer)
//
//-----------------------------------------------------------------------------------------
void decode_input( const char *instr, uint32_t *buf, int buflen ) {
  int      s = 0; // index into input string
  int      i = 0; // index into buf
  int      j = 0; // subloop index
  int      r = 0; // repeat count
  char     num [10] = "";  // big enough for any number, right?
  uint32_t colorword = 0x80808080;  // last word seen in the input
  
  while(true) {
    // Parse next character
    switch (instr[s]) {
    //
    // Termination of the string
    //
    case '\0':
      return;
    //
    // Regular color codes
    //
    case 'W':
    case 'R':
    case 'G':
    case 'B':
    case 'C':
    case 'M':
    case 'Y':
    case 'K':
    case ' ':
    case '.':
      colorword = colorcode(instr[s]);
      s++;
      buf[i]=colorword;
      i++;
      if(i==buflen) {
        // done!
        return;
      }
      break;
    //
    // Repeat code
    //
    case '*':
      // Scan the numeric code after the star:
      j = 0;
      s++;
      while (isdigit(instr[s])) {
        num[j]=instr[s];
        s++;
        j++;
      }
      num[j]=0; // terminate string
      // Now we have a number in hand
      r=atoi(num);
      // Fill in r repetitions of the number
      while(true) {
        buf[i]=colorword;
        i++;
        r--;
        if(r==0) break;
        if(i==buflen){
          // avoid clobbering beyond the end of the buffer
          return;
        }
      }
      break;
    default:
      // skip forward
      s++;
      fprintf(stderr, "Unexpected character ('%c') in input '%s', skipping.", instr[s], instr);
    }
    // Loop the loop back to top
  } 
}

//-----------------------------------------------------------------------------------------
//
// Interpret string and put the pattern on the LEDs
//
//-----------------------------------------------------------------------------------------
void display_string(const char *instr) {
  uint32_t   display_buffer[(DISPLAY_HEIGHT * DISPLAY_WIDTH)];
  const int  buflen = (DISPLAY_HEIGHT * DISPLAY_WIDTH);
  int        j;

  // Clear the character display to base pattern of blue-cyan lines
  for(j=0;j<buflen;j++) {
    display_buffer[j] = (j&0x01) ? 0x00FF0000 : 0x00FFFF00;
  }

  // Convert input string to pixel values in the buffer
  decode_input(instr, display_buffer, buflen);

  // Update device memory
  hw_update_frame_buffer(display_buffer);

  // Tell the hardware to update
  hw_request_redraw();
}

//-----------------------------------------------------------------------------------------
//
// main program entry point
//
//-----------------------------------------------------------------------------------------

// Entry point
int main(int argc, char ** argv) {
  int err;

  // Allow Simics to catch the start
  MAGIC(0);
  
  // Check argument count
  if(argc != 3) {
    fprintf(stderr, "Illegal number of arguments.\nUsage: %s <string> <devnode> \n", argv[0]);
    exit(-1);
  }
  
  //-----------------------------------------
  //      Open the device register bank
  //-----------------------------------------
  err = hw_open_dev_nodes(argv[2]);
  if( err ) {
    // exit on error
    return err;
  }

  //-----------------------------------------
  //      Interpret input and display
  //-----------------------------------------
  display_string(argv[1]);

  //-----------------------------------------
  //      Close the device nodes on exit
  //-----------------------------------------
  hw_close_dev_nodes();

  return 0;
}




