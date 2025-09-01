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

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>


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
    
    void *rawptr = malloc (memorysize);
    if(rawptr==NULL) {
        fprintf(stderr, "\nFailed to allocate %ld bytes, try something smaller\n", memorysize);
        return ENOMEM;
    }

    printf("Progress:\n\n" );

    // Work through memory
    char *ptr = (char *) rawptr;

    // Think through stepping
    uint64_t steps = (memorysize/stride);
    int      h = 16;  // vertical display
    int      b = 32;  // horizontal display
    uint64_t report_steps = steps/(h*b);  // never mind if this is a bit uneven
    setbuf(stdout, NULL);
    const char * offset = "  ";
    printf(offset);

    // Iterate
    uint64_t i;
    int      row_counter = 0;
    int      col_counter = 0;
    int      step_counter = 0;
    for(i=0;i<steps;i++) {
        *((uint64_t*)(ptr + (stride * i))) = value;

        // Progress indicator
        step_counter ++;
        if(step_counter >= report_steps) {
            step_counter = 0;
            printf("\033[38;5;33m* ");
            col_counter++;
            if(col_counter>=b) {
                col_counter =0;
                row_counter++;
                // Print total memory touched so far
                printf("\033[38;5;34m- %ld MiB", mibibytes((i+1) * stride));
                // Start next row
                printf("\n%s", offset);
            }
        }
    }
    printf("\033[0m\n"); // reset screen colors

    return 0;
}


//-----------------------------------------------------------------------------------------
//
// main program entry point
//
//-----------------------------------------------------------------------------------------
int main(int argc, char ** argv) {
  
    // Check argument count
    if(argc != 4) {
        fprintf(stderr, "\nUsage: %s <memorysize> <stride> <value> \n", argv[0]);
        fprintf(stderr, "  memorysize = total memory amount to allocate (MiB)\n");    
        fprintf(stderr, "  stride = address stride for value writing (KiB)\n");
        fprintf(stderr, "  value = value to write to each address (8 bytes hex)\n");
        fprintf(stderr, "\nFor example:\n");
        fprintf(stderr, "  %s 16384 1 0xc0ffee\n\n", argv[0]);    
        exit(-1);
    }

    //-----------------------------------------
    //      Parse arguments
    //-----------------------------------------

    // Variables for arguments
    uint64_t arg_memorysize = 0;
    uint64_t arg_stride = 0;
    uint64_t arg_value = 0;
    char * tempnextptr;
    const char * arg;

    // Variables for values to use
    size_t memorysize = 0;
    uint64_t stride = 0;
    uint64_t value = 0;

    //   Could in principle break out this parsing to a function.
    //   But that would not save all that much code. Since it would require
    //   error reporting plus value return.  

    // Parse argument - memorysize
    errno = 0;
    arg = argv[1];
    arg_memorysize = strtoull(arg, &tempnextptr, 10);
    if (tempnextptr == arg || *tempnextptr != '\0' || errno) {
        fprintf(stderr,"Error: Memory size '%s' is not valid\n", arg);
        return ENOENT;
    }
    memorysize = arg_memorysize * 1024 * 1024;

    // Parse argument - stride
    errno = 0;
    arg = argv[2];
    arg_stride = strtoull(arg, &tempnextptr, 10);
    if (tempnextptr == arg || *tempnextptr != '\0' || errno) {
        fprintf(stderr,"Error: Stride '%s' is not valid\n", arg);
        return ENOENT;
    }
    stride = arg_stride * 1024 ;

    // Parse argument - value
    errno = 0;
    arg = argv[3];
    arg_value = strtoull(arg, &tempnextptr, 16);
    if (tempnextptr == arg || *tempnextptr != '\0' || errno) {
        fprintf(stderr,"Error: '%s' for 'value' is not valid\n", arg);
        return ENOENT;
    }
    value = arg_value;

    printf( "\nParameters:\n"  
            "\n  Memory size: %ld (%ld MiB)"  
            "\n       Stride: %ld (%ld KiB)"  
            "\n        Value: 0x%lx\n\n", 
            memorysize, mibibytes(memorysize), 
            stride, kibibytes(stride), value);

    //-----------------------------------------
    //      Do work
    //-----------------------------------------
    int result;
    result = allocate_and_touch(memorysize, stride, value);

    return result;
}


