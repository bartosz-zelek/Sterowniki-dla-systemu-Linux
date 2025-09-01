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
// Demo, test, and training program to use the Simics training device
// panel in various ways.
//
//
// Hardware access mechanism used:
//
//   Uses mmap to access both the control register bank and the device
//   local memory.   This requires mapping two BARs, and thus we do
//       that via /sys.  The user has to provide the program with the /sys
//   node that is being used as an argument, and the program then
//       appends the appropriate resource numbers.
//
// Support for debug demo:
//
//   The program has a magic instruction embedded to make it really easy
//   to grab the start of its main loop.
//

#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  // for sleep, write
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/mman.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

// Simics include
#include "simics-magic-instruction.h"
#include "big-chars.h"

// Global pointers to the register banks
volatile void *regsbank;
volatile void *localmemorybank;

// To allow some dummy loops to escape optimizations
volatile uint64_t dummy = 0;

// Wait between updating the display
// microseconds: 1000000 per second
const useconds_t gFrameTime = (1000000/5); // 1/5 second

// Just some random junk, these are not the droids
// you are looking for
const char * empty  = "#EASTER_366";
static bool eeegg   = false;

//-----------------------------------------------------------------------------------------
//
// Register map = same as found in device driver source code
//
//-----------------------------------------------------------------------------------------
#define BAR0_ENABLE                           0x0000
#define BAR0_RESET                            0x0004

#define BAR0_UPDATE_DISPLAY_REQUEST           0x0010
#define BAR0_UPDATE_DISPLAY_STATUS            0x0014
#define BAR0_UPDATE_DISPLAY_INTERRUPT_CONTROL 0x0018
#define BAR0_FRAMEBUFFER_BASE_ADDRESS         0x001c

#define FRAMEBUFFER_ADDR                    0x2000   // use a different one just to be
                                                                                                          // different

#define BAR0_ADV_COLOR_ALL                    0x0020
#define BAR0_ADV_DPROG_BASE                   0x0024
#define BAR0_ADV_DPROG_STATUS                 0x0028
#define BAR0_ADV_DPROG_CONTROL                0x002c

#define BAR0_BUTTON_INTERRUPT_CONTROL         0x0200
#define BAR0_BUTTON_STATUS_BASE               0x0204
#define BAR0_NUMBER_OF_BUTTONS                    16

#define BAR0_TOGGLE_STATUS_BASE               0x0244
#define BAR0_NUMBER_OF_BUTTONS                    16

#define BAR0_DISPLAY_I2C_BASE                 0x0800
#define BAR0_DISPLAY_WIDTH                    0x0804
#define BAR0_DISPLAY_HEIGHT                   0x0808
#define BAR0_BUTTON_I2C_ADDRESSES             0x0810
#define BAR0_TOGGLE_I2C_ADDRESSES             0x0850


//-----------------------------------------------------------------------------------------
//
// Hardware interface and activity functions
//
//-----------------------------------------------------------------------------------------
uint32_t read32(volatile void* address) {
  return *((volatile uint32_t*) address);
}
void write32(volatile void* address, uint32_t value) {
  *((volatile uint32_t*) address) = value;
}

void waitforbuttonA(void) {
  // Wait until we see button A being pressed
  // using polled reads from the device register space
  uint32_t status = 0;
  
  // Infinite loop until we see the button pressed
  while(1) {
    status = (read32( regsbank + BAR0_BUTTON_STATUS_BASE ));
    if(status!=0) break;
    // Dummy loop to reduce the amount of hardware bashing
    // int i
    //for(i=0;i<1000;i++){
    //      dummy += i;
    //}
  }
  // Clear status
  write32( regsbank + BAR0_BUTTON_STATUS_BASE, 1);
  return;
}

// Was button B pressed?
bool buttonBpressed(void) {
  uint32_t status = 0;
  status= read32( regsbank + BAR0_BUTTON_STATUS_BASE + 4*1 );
  if(status!=0) {
    write32( (regsbank + BAR0_BUTTON_STATUS_BASE + 4*1) , 1);
    return true;
  } else {
    return false;
  }
}

// Read the current value of a toggle
uint32_t  gettoggle(int toggleno) {
  return ( read32( regsbank + BAR0_TOGGLE_STATUS_BASE + toggleno*4 ));
}

// Request an update and wait for it to complete
void requestredraw(void) {
  uint32_t status = 0;
  // Ask redraw to happen
  write32( (regsbank + BAR0_UPDATE_DISPLAY_REQUEST), 1);
  // Poll status until done
  // For a simulation, this is suboptimal in terms of performance
  while(1) {
    status = (read32( (regsbank + BAR0_UPDATE_DISPLAY_STATUS) ));
    if(status!=0) break;
  }
  return;
}


//-----------------------------------------------------------------------------------------
//
// Main scroll loop
// - Pointer to the scene
// - Length of the scene, in pixels, excluding the extra data at the end
//-----------------------------------------------------------------------------------------
void scene_piece_to_display(uint32_t *scene,
			    int       aplen,
			    int       x) {
  int i,j;
  // Copy from the scene at horizontal point x
  // To the display buffer in local memory
  for(i=0;i<GLYPH_HEIGHT;i++) {
    // Loop over pixels
    for(j=0;j<GLYPH_WIDTH;j++) {
      uint32_t pixel = scene[(i*aplen) + x + j];
      write32( (localmemorybank + FRAMEBUFFER_ADDR + 4*(j + i*GLYPH_WIDTH)),
	       pixel);
    }
  }
}

void scroll_loop(uint32_t *scene,
		 int       aplen    ) {
  int x = 0;
  
  while(1) {
    // Update display in memory and ask for a redraw
    scene_piece_to_display(scene, aplen, x);
    requestredraw();
    
    // move in the direction given by the top toggle
    // Note that from an appearance perspective,
    // x==0 shows the same as x==(aplen-8)
    if(gettoggle(0)==0) {
      // Toggle is "OFF", which means "normal"
      x++;
      if(x>=(aplen-8)) { // wrap around at end of actual text
        x=0;
      }
    } else {
      // Toggle is "ON", which means "reverse"
      x--;
      if(x<0) {
        x=aplen-8-1 ; // move back one glyph from buffer end,
	                 // and then one more pixel to get scrolling
      }
    }
    // Wait a suitable amount
    usleep(gFrameTime);
    
    // Did someone press "B" to quit?
    if(buttonBpressed()) break;
  }
}



//-----------------------------------------------------------------------------------------
//
// Build up the scene
//
//-----------------------------------------------------------------------------------------
void add_glyph_to_scene(uint32_t *scene,   // big pile of pixels
			int       px,      // how far in from the left, in pixels
			glyph_t  *glyph,   // symbolic glyph to add
			int       aplen) { // length of a line in pixels
  // Local vars
  int  i,j;
  char c;
  // Loop over lines
  for(i=0;i<GLYPH_HEIGHT;i++) {
    // Loop over pixels
    for(j=0;j<GLYPH_WIDTH;j++) {
      // Find the uint32_t value for the symbolic
      c = (*(glyph))[i][j];
      uint32_t p = colorcode(c);
      scene[(i*aplen) + px + j] = p;
    }
  }
}


//-----------------------------------------------------------------------------------------
//
// Main loop of the program
// Designed to allow for stepping, setting breakpoints, etc.
//
//-----------------------------------------------------------------------------------------
void display_demo(const char *displaystring ) {
  uint32_t  * scene;
  int         clen;  // char len
  int         plen, aplen;  // pixel len, actual pixel len
  int         i;
  char        c;
  glyph_t    *g;
  
  // Waiting to start...
  printf("Press button A on panel to start - scrolling text '%s'!\n", displaystring);
  
  //
  // Allocate a buffer to hold the whole set of pixels to display
  // one longer than the length of the string, to support cheating
  // wrap-around scrolling
  //
  clen = strlen(displaystring);
  plen  = clen * GLYPH_WIDTH;
  aplen = plen + GLYPH_WIDTH;
  scene = malloc(sizeof(uint32_t) * GLYPH_HEIGHT * aplen );
  
  // Create the display image in memory
  for(i=0;i<clen;i++) {
    c = displaystring[i];
    g = glyph_lookup(c);
    add_glyph_to_scene(scene, i*GLYPH_WIDTH, g, aplen);
  }
  // Add the wrap-around at the end - i has the value of
  // one beyond the end of the string
  c = displaystring[0];
  g = glyph_lookup(c);
  add_glyph_to_scene(scene, i*GLYPH_WIDTH, g, aplen);
  
  // Program hardware to use the framebuffer address we choose
  write32( (regsbank + BAR0_FRAMEBUFFER_BASE_ADDRESS), FRAMEBUFFER_ADDR);
  
  // Wait for button press by polling the hardware
  // this is not going to be fast by any means...
  waitforbuttonA();
  
  scroll_loop(scene, aplen);
}

//-----------------------------------------------------------------------------------------
//
// main program entry point
//
//-----------------------------------------------------------------------------------------

// Doing a bit of overflow-resistant coding
#define MAXPATHLEN     300
#define PATHLENRESERVE 15

// Entry point
int main(int argc, char ** argv) {
  // device node names
  char reg_devnode_name [MAXPATHLEN+PATHLENRESERVE];
  char mem_devnode_name [MAXPATHLEN+PATHLENRESERVE];
  int  reg_fno;
  int  mem_fno;
  // file checking
  struct stat filestat;
  size_t filesize;
  // string to scroll
  const char * scrollstring = NULL;

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
  int l = strlen(argv[2]);
  if((l)>=MAXPATHLEN) {
    fprintf(stderr,"The device node path was too long '%s'\n", argv[2]);
    exit(-1);
  }
  strncpy(reg_devnode_name, argv[2], MAXPATHLEN);
  strncat(reg_devnode_name, "/resource0", PATHLENRESERVE);
  
  //
  // string to display
  //
  scrollstring = argv[1];

  //
  // Open and map the nodes
  //
  reg_fno = open(reg_devnode_name, O_RDWR);
  if(reg_fno == -1) {
    fprintf(stderr,"Error: Failed to open device file '%s'\n", reg_devnode_name);
    return ENOENT;
  }
  // Get the size of the file to map the appropriate amount of memory without constants
  // in this code
  if (stat(reg_devnode_name, &filestat) != 0) {
    fprintf(stderr,"Error: Failed to get size of device file '%s'\n", reg_devnode_name);
    return ENOENT;
  }
  filesize = filestat.st_size;
  
  regsbank = mmap(0,
		  filesize,
		  PROT_READ | PROT_WRITE,
		  MAP_SHARED,
		  reg_fno,
		  0);
  
  printf("Device control registers mapped to address 0x%lx, for 0x%lx bytes.\n", (unsigned long) regsbank, filesize);
  
  //-----------------------------------------
  //      Open the local memory bank
  //-----------------------------------------
  strncpy(mem_devnode_name, argv[2], MAXPATHLEN);
  strncat(mem_devnode_name, "/resource3", PATHLENRESERVE);
  if(eeegg) scrollstring = empty;

  //
  // Open and map the nodes
  //
  mem_fno = open(mem_devnode_name, O_RDWR);
  if(mem_fno == -1) {
    fprintf(stderr,"Error: Failed to open device file '%s'\n", mem_devnode_name);
    return ENOENT;
  }
  if (stat(mem_devnode_name, &filestat) != 0) {
    fprintf(stderr,"Error: Failed to get size of device file '%s'\n", mem_devnode_name);
    return ENOENT;
  }
  filesize = filestat.st_size;
  
  localmemorybank = mmap(0,
			 filesize,
			 PROT_READ | PROT_WRITE,
			 MAP_SHARED,
			 mem_fno,
			 0);
  
  
  printf("Device local memory mapped to address 0x%lx, for 0x%lx bytes.\n", (unsigned long) localmemorybank, filesize);
  
  //-----------------------------------------
  //      Get the demo going
  //-----------------------------------------
  display_demo(scrollstring);
  
  //
  // Close before finishing
  //
  close(reg_fno);
  close(mem_fno);
  return 0;
}



