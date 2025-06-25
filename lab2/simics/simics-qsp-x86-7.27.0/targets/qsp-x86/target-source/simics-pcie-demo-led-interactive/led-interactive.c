/* This Software is part of Simics. The rights to copy, distribute,
   modify, or otherwise make use of this Software may be licensed only
   pursuant to the terms of an applicable license agreement.

   Copyright 2010-2021 Intel Corporation */

//
// Demo and test program for the Simics PCIe Demo Device, using both
// the mmap and char interface to the hardware.  Is an interactive LED
// test program, as well as a performance comparison program.
//
// Implements the same function in two ways. 
//
// Device node name for the char device is hard-coded into the program
// (/dev/simics_pcie_demo_driver) - check code of main()

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
#define BUFFER_SIZE 255

// Register offsets
#define BANK_LENGTH 		    	0x32
#define LED0_OFFSET             	0x00
#define LED1_OFFSET             	0x04
#define LED2_OFFSET             	0x08
#define LED3_OFFSET             	0x0C
#define BUTTON_STATUS_OFFSET    	0x14
#define BUTTON_1_COUNT_OFFSET   	0x18
#define BUTTON_2_COUNT_OFFSET   	0x1C
#define BUTTON_INTERRUPT_CONTROL   	0x28
#define AMOUNT_WRITES_TEST		    1000000

// Global variable to hold device file handle
int gDevFile;

// Global variable to hold the pressed button. -1 means no button was pressed
// Used to communicate between the thread looking for button presses and the
// thread animating the lights.
//
// To ensure it is not cached in a register, make volatile
volatile int buttonPressed = -1;

//Global variable to hold the time between LED blinking, in microseconds
useconds_t sleepTime = 2000000; //two seconds
//pointer to the mmap memory
void *mappedMemory;

//
// Defining patterns to iterate over
//
// Global array of steps. Each step is an array with
// the LED state for the current step.  Last step 
// is represented by a NULL item
//
// Each pattern it is own global array - and we 
// then assign a global variable to the current one
//
#define NUM_LEDS 4

// dancing lights using the char_write driver function
void *dancing_lights_write(void *vargp) {
	int   onoff = 1; // start by turning lights on
	char  buf[3];
	int   ledseq[] = {0,1,3,2};  // sequence to affect LEDs
	bool  repeat = true;
	// Main loop
	while (repeat) {
		int j;
		for(j=0;j<NUM_LEDS;j++){
			// Build command string
			snprintf(buf,3,"%d%d",ledseq[j],onoff);
			write(gDevFile,buf,2);//this line will send the string to the device

			if(buttonPressed == 1) {//if we press button 1, halve the time between blinking
				sleepTime /= 2;
				buttonPressed = -1;
				printf("[Button 1] Sleep time is now %d us\n", sleepTime);
			}
			if(buttonPressed == 2) {//break if the pressed button was (two)
				buttonPressed = -1;
				repeat = false;
				break;
			}
			usleep(sleepTime);
		}
		onoff = 1 - onoff; // invert sense
	}
	return NULL;
}

// Char button reading thread
void *wait_for_button_read(void *vargp) {
	char buffer[BUFFER_SIZE];

	while( buttonPressed != 2)  {
		int ret = read(gDevFile, buffer, BUFFER_SIZE);//will block until a button is pressed
		if(ret < 0) {
			perror("Failed reading from device");
			return NULL;
		}
		int valueRead = atoi(buffer);   //convert char value to int and store in global variable
		if( (valueRead<1) || (valueRead>2)) {
		    printf("Button pressed, but value seems wrong... (%d)\n", valueRead);
		} else {
		    buttonPressed = valueRead;
		}
	}
	printf("Button reading thread terminating\n");
	// Return and terminate thread once button two gets pressed
	return NULL;
}

void *dancing_lights_mmap(void *vargp) {
	int ledOffset[] = {
						LED0_OFFSET,
						LED1_OFFSET,
						LED2_OFFSET,
						LED3_OFFSET
					 };
	uint32_t onoff = 1; // start by turning lights on
	int  ledseq[] = {0,1,3,2};  // sequence to affect LEDs
	int repeat = 1;
	// Main loop
	while (repeat) {
		int j;
		for(j=0;j<NUM_LEDS;j++){
			// Build command string
			uint32_t *address = (uint32_t*) (mappedMemory + ledOffset[ledseq[j]]);//calculate the offset
			*address = onoff;//this line will write to the corresponding register
			if(buttonPressed == 1) {//if we press button 1, halve the time between blinking
				sleepTime /= 2;
				buttonPressed = -1;
			}
			if(buttonPressed == 2) {//break if the pressed button was (two)
				buttonPressed = -1;
				repeat = 0;
				break;
			}
			usleep(sleepTime);
		}
		onoff = 1 - onoff; // invert sense
	}
	return NULL;
}


void *wait_for_button_mmap(void *vargp) {
	uint32_t *address = (uint32_t*) (mappedMemory + BUTTON_STATUS_OFFSET);
	*address = 0;
	while(1)  {//poll to see if the register value has changed
		if(*address > 0) {
			buttonPressed = *address;
			*address = 3;//clear the button status register
		}
		if(buttonPressed == 2)//break if the button (two) was pressed
			break;
		usleep(10000);//sleep 10ms
	}
	return NULL;
}

void charTest() {
	int i;
	char buf[3] ;
	printf("Writing %d times using char driver\n", AMOUNT_WRITES_TEST);
	clock_t startTime = clock();
	for(i = 0; i < AMOUNT_WRITES_TEST; i++) {
	    // Go through all the LEDs, and turn them alternatingly on
	    // and off
		snprintf(buf,3,"%d%d", i % 4, ( (i>>2) & 0x01 ) );
		write(gDevFile,buf,2);
	}
	clock_t finishTime = clock();
	double execTime = (double)(finishTime - startTime) / CLOCKS_PER_SEC;
	printf("Execution time: %lf \n", execTime);
}

void mmapTest() {
	int i;
	volatile uint32_t *address = (volatile uint32_t*) (mappedMemory + LED0_OFFSET);
	printf("Writing %d times using mmap\n", AMOUNT_WRITES_TEST);
	clock_t startTime = clock();
	for(i = 0; i < AMOUNT_WRITES_TEST; i++) {
		*(address + i%4) = ( (i>>2) & 0x01 );
	}
	clock_t finishTime = clock();
	double execTime = (double)(finishTime - startTime) / CLOCKS_PER_SEC;
	printf("Execution time: %lf \n", execTime);
	
}

//
// main program entry point
//
int main(int argc, char ** argv) {
  char *devname;
  int   fno;
  int   isMmap = 0;
  int   isTest = 0;
  
  // Check argument count
  if(argc != 2) {
    perror("Illegal number of arguments.\nUsage: %s [mmap|char]");
    exit(-1);
  }
  
  // Check first argument - are we to use mmap or char mode?
  if(strcmp(argv[1], "mmap") == 0)
    isMmap = 1;
  else if(strcmp(argv[1], "char") == 0)
    isMmap = 0;
  else if (strcmp(argv[1], "test") == 0) {
    isTest = 1;
  } else{
    perror("Illegal argument. Supported values are \'mmap\', \'char\' and \'test\' \n");
    exit(-1);
  }
  devname = "/dev/simics_pcie_demo_driver";
  // Open device for writing
  fno = open(devname, O_RDWR);
  if(fno == -1) {
    fprintf(stderr,"Error: Failed to open device file '%s'\n", devname);
    return ENOENT;
  }
  gDevFile = fno;
  
  //We need two threads: One to poll for button presses and other to blink the leds
  pthread_t idDancingLeds;
  pthread_t idWaitForButton;
  if (isMmap) {
    //we get the pointer to the regs bank
    mappedMemory = mmap(0, BANK_LENGTH, PROT_READ | PROT_WRITE,
			MAP_SHARED, gDevFile, 0);
    //we get the offset of the interrupt control register
    uint32_t *interruptControlReg = (uint32_t*) (mappedMemory + BUTTON_INTERRUPT_CONTROL);
    //we write 0 to disable interrupts
    *interruptControlReg = 0x0;
    pthread_create(&idDancingLeds, NULL, dancing_lights_mmap, NULL);
    pthread_create(&idWaitForButton, NULL, wait_for_button_mmap, NULL);
    pthread_join(idDancingLeds, NULL);
    //we re-enable interrupts. Write 1 to clear the register fields
    *interruptControlReg = 0x3;
  } else if(isTest) {
    charTest();
    mappedMemory = mmap(NULL, BANK_LENGTH, PROT_READ | PROT_WRITE,
			MAP_SHARED, gDevFile, 0);
    mmapTest();
  }
  else {
    pthread_create(&idDancingLeds, NULL, dancing_lights_write, NULL);
    pthread_create(&idWaitForButton, NULL, wait_for_button_read, NULL);
    pthread_join(idWaitForButton, NULL);
  }
  // Return success at end of execution
  return 0;
}

