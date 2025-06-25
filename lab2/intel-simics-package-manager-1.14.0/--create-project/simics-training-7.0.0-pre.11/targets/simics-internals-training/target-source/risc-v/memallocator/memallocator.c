/*
  Source code for Simics tranings and demos. Licensed for use with Simics.

  © 2023 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

//
// Program to exercise Simics image memory management
//
// Allocate large amounts of RAM
// Touch pages to quickly cause Simics to quickly ramp image usage
//
// Arguments:
// - total memory size to allocate 
// - stride of accesses
// - value to write (to allow exercising zero-value page detection)
//
// To build on host so that it works on QSP:
//   $ gcc memallocator.c -o memallocator -static
// But really it should be built on the target using its preferred
// gcc and libc
#include "risc-v-simple-bsp.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>

#include "magic-instruction.h"

uint64_t kibibytes(uint64_t bytes) {
    return bytes >> 10;
}

uint64_t mibibytes(uint64_t bytes) {
    return bytes >> 20;
}

uint64_t gibibytes(uint64_t bytes) {
    return bytes >> 30;
}

//-----------------------------------------------------------------------------------------
//
// do the work 
//
//-----------------------------------------------------------------------------------------
int allocate_and_touch(size_t memorysize, 
                        uint64_t stride, 
                        uint64_t value) {
    
    bsp_printf("Progress:\n\n\r" );

    // Work through memory
    char *ptr = (char *)0x100000000ULL;

    // Think through stepping
    uint64_t steps = (memorysize/stride);
    int      h = 16;  // vertical display
    int      b = 32;  // horizontal display
    uint64_t report_steps = steps/(h*b);  // never mind if this is a bit uneven

    const char * offset = "  ";
    bsp_printf(offset);

    // Iterate
    uint64_t i;
    int      row_counter = 0;
    int      col_counter = 0;
    int      step_counter = 0;
    MAGIC(33);
    for(i=0;i<steps;i++) {
        *((uint64_t*)(ptr + (stride * i))) = value;
        MAGIC(42);
        // Progress indicator
        step_counter ++;
        if(step_counter >= report_steps) {
            step_counter = 0;
            bsp_printf("\033[38;5;33m* ");
            col_counter++;
            if(col_counter>=b) {
                col_counter =0;
                row_counter++;
                // Print total memory touched so far
                bsp_printf("\033[38;5;34m- %ld MiB", mibibytes((i+1) * stride));
                // Start next row
                bsp_printf("\n\r%s", offset);
            }
        }
    }
    bsp_printf("\033[0m\n\r"); // reset screen colors

    return 0;
}


//-----------------------------------------------------------------------------------------
//
// main program entry point
//
//-----------------------------------------------------------------------------------------
int main(int argc, char ** argv) {
    //-----------------------------------------
    //      Do work
    //-----------------------------------------
    int result;
    init_board();
    result = allocate_and_touch(0x400000000, 0x400, 0xdeadbeefbadc0feeULL);

    return result;
}


