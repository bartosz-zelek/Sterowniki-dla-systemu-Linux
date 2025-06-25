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
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <unistd.h> 
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <errno.h>

#include "hardware-interface.h"


// Global pointers to the register banks
volatile void *regsbank;
volatile void *localmemorybank;

int  g_reg_fno;
int  g_mem_fno;


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

#define FRAMEBUFFER_ADDR                      0x2000   // use a different one just to be
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

void hw_wait_for_buttonA(void) {
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
bool hw_buttonB_pressed(void) {
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
uint32_t  hw_get_toggle(int toggleno) {
  return ( read32( regsbank + BAR0_TOGGLE_STATUS_BASE + toggleno*4 ));
}

// Copy a display buffer to the device RAM
void hw_update_frame_buffer(uint32_t *buf) {
  int i;
  for(i=0; i< (DISPLAY_HEIGHT * DISPLAY_WIDTH); i++) {
    write32( (localmemorybank + FRAMEBUFFER_ADDR + 4*i), buf[i]);
  }

  // Program hardware to use the framebuffer address we choose
  write32( (regsbank + BAR0_FRAMEBUFFER_BASE_ADDRESS), FRAMEBUFFER_ADDR);
}


// Request an update and wait for it to complete
void hw_request_redraw(void) {
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

//-----------------------------------------------------------------------
//
// Open the device nodes
//
//-----------------------------------------------------------------------

// Doing a bit of overflow-resistant coding
#define MAXPATHLEN     300
#define PATHLENRESERVE 15

// Return error codes
int hw_open_dev_nodes(const char *devnodearg) {
  // files nodes
  char reg_devnode_name [MAXPATHLEN+PATHLENRESERVE];
  char mem_devnode_name [MAXPATHLEN+PATHLENRESERVE];

  // file checking
  struct stat filestat;
  size_t filesize;

  //
  // Check the devnodearg for sanity, and build up 
  // 
  int l = strlen( devnodearg );
  if((l)>=MAXPATHLEN) {
    fprintf(stderr,"The device node path was too long '%s'\n", devnodearg);
    return ENOENT;
  }
  
  //-----------------------------------------
  //      Open the register node 
  //-----------------------------------------
  strncpy(reg_devnode_name, devnodearg, MAXPATHLEN);
  strncat(reg_devnode_name, "/resource0", PATHLENRESERVE);

  g_reg_fno = open(reg_devnode_name, O_RDWR);
  if(g_reg_fno == -1) {
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
		  g_reg_fno,
		  0);
  
  printf("Device control registers mapped to address 0x%lx, for 0x%lx bytes.\n", 
         (unsigned long) regsbank, filesize);
  
  //-----------------------------------------
  //      Open the local memory bank
  //-----------------------------------------
  strncpy(mem_devnode_name, devnodearg, MAXPATHLEN);
  strncat(mem_devnode_name, "/resource3", PATHLENRESERVE);

  g_mem_fno = open(mem_devnode_name, O_RDWR);
  if(g_mem_fno == -1) {
    close(g_reg_fno);
    fprintf(stderr,"Error: Failed to open device file '%s'\n", mem_devnode_name);
    return ENOENT;
  }
  if (stat(mem_devnode_name, &filestat) != 0) {
    close(g_reg_fno);
    fprintf(stderr,"Error: Failed to get size of device file '%s'\n", mem_devnode_name);
    return ENOENT;
  }
  filesize = filestat.st_size;
  
  localmemorybank = mmap(0,
			 filesize,
			 PROT_READ | PROT_WRITE,
			 MAP_SHARED,
			 g_mem_fno,
			 0);
  
  printf("Device local memory mapped to address 0x%lx, for 0x%lx bytes.\n", 
         (unsigned long) localmemorybank, filesize);
  
  // Successful, return zero
  return 0;
}

// Call this at the end of the program
void hw_close_dev_nodes(void) {
  close(g_reg_fno);
  close(g_mem_fno);
}


