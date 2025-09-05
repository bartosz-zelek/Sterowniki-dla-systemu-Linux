/*
  Source code for Simics tranings and demos. Licensed for use with Simics.

  © 2016 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

//
// Demo program that accesses PCI via the /sys file system 
//
// Intended to show how to test a PCI device from the shell,
// and how to use mmap() to memory-map a file without using the
// char aspect of the device driver. 
//
// Syntax:
//   pciop <resource> operation offset size [value]
//         <resource> = /sys/bus/pci/devices/DEV/resourceN
//         operation  = r | w
//         size       = 1 | 2 | 4
//         offset     = offset
//         value      = anything (32-bits max)
//

#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>  // for sleep, write
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

// Global variable to hold device file handle
int gPciDevFile;

// print error message on bad arguments
void printerror(char *argv0) {
    fprintf(stderr,"Incorrect program usage.\n\nUsage:\n  %s <device resource> r|w 1|2|4 <offset> [<value>]\n", argv0);
    exit(-1);
}

//main 
int main(int argc, char ** argv) {

    // Operational specifiers
	char   * devname;
	int      fno;
	int      opIsWrite = 0;  //  1 for write
	int      opSize    = 0;
	uint32_t offset = 0xDEADBEEF;;
	uint32_t value = 0xDEADBEEF;

	// Check basic argument sanity
	if(argc < 5 || argc > 6) {
	    printerror(argv[0]);
	}

	//
	// Go over the arguments with very little error checking
	//
	// 1. Device name
	devname = argv[1];
	fno = open(devname, O_RDWR);
	if(fno == -1) {
	    fprintf(stderr,"Error: Failed to open device file '%s'\n", devname);
	    return ENOENT;
	}
	gPciDevFile = fno;

	// 1.5. Get the size of the file
    struct stat filestat;
    if (stat(devname, &filestat) != 0) {
        fprintf(stderr,"Error: Failed to get size of device file '%s'\n", devname);
        return ENOENT;
    }
    size_t filesize = filestat.st_size;

	// 2. operation type
	switch(argv[2][0]) {
	case 'r':
	case 'R':
	    opIsWrite = 0;
	    break;
	case 'w':
	case 'W':
	    opIsWrite = 1;
	    break;
	default:
	    printerror(argv[0]);
	}
//	printf("OpIsWrite: %d\n",opIsWrite);

	// 3. Size
    switch(argv[3][0]) {
    case '1':
        opSize = 1;
        break;
    case '2':
        opSize = 2;
        break;
    case '4':
        opSize = 4;
        break;
    default:
        printerror(argv[0]);
    }
//    printf("OpSize: %d\n",opSize);

    // 4. Offset -- assumed to be in HEX!
    char * nextptr;
    offset = strtoul(argv[4],&nextptr,16);
//    printf("offset: 0x%x\n",offset);


    // 5. Value -- only for write!
    if(opIsWrite && (argc != 6) ) {
        printerror(argv[0]);
    }
    if(opIsWrite) {
        value = strtoul(argv[5],&nextptr,16);
//        printf("value: 0x%x\n",value);
    }

    //-------------------------------------------------------------------
    //  Starting real operation
    //-------------------------------------------------------------------
    printf("\n\n");

    // Do the mmap operation on the file
    void * mappedaddr = NULL;
    mappedaddr = mmap( NULL,      // no preferred address
                       filesize,  // the entire mapping, as given by the file size
                       PROT_READ | PROT_WRITE,
                       MAP_SHARED,
                       fno,
                       0          // Just map the whole thing, don't bother with the offset
            );
    if(mappedaddr == MAP_FAILED) {
        int err = errno;
        fprintf(stderr,"Error: mmap failed (error %d)!\n", err);
        return err;
    }
    printf("PCIe resource memory mapped to address 0x%lx, for 0x%lx bytes.\n", (unsigned long) mappedaddr, filesize);

    // Add offset to the mapped address
    void *addr = mappedaddr + offset;

    // Print arguments and do the operation:
    if(opIsWrite) {
        printf("Writing 0x%x as %d bytes to offset 0x%x\n", value, opSize, offset);
        switch(opSize) {
        case 1:
            *((uint8_t *) addr) =  ( value & 0xff );
            break;
        case 2:
            *((uint16_t *) addr) = ( value & 0xffff );
            break;
        case 4:
            *((uint32_t *) addr) = ( value );
            break;
        }
    }
    else {
        printf("Reading %d bytes from offset 0x%x \n", opSize, offset);
        switch(opSize) {
        case 1:
            value = *((uint8_t *) addr);
            break;
        case 2:
            value = *((uint16_t *) addr);
            break;
        case 4:
            value = *((uint32_t *) addr);
            break;
        }
        printf("\nResult = 0x%02x\n", value);
    }

	// Return success at end of execution
	return 0;
}

