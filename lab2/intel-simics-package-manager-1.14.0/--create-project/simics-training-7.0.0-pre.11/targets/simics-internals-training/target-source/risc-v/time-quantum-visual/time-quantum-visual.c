/* 

  © 2024 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include "risc-v-simple-bsp.h"
#include "magic-instruction.h"
#include <stdint.h>
#include <string.h>

// Aim for 20 - 4 active units on an 80-wide field
#define visualization_length 20

void main_loop(const char * message, int m_length) {
    char     c = ' ';
    uint64_t count = 0;
    uint64_t index = 0;
    // Arbitrary large loop count - makes the program terminate 
    // if left alone for too long, but does not interfere with labs 
    // by terminating too early. 
    while( count < 20000000) {
        // To shared output: message string char by char
        // Which gets colorized by the magic of the shared output device
        c = message[index];
        shared_con_put_char(c);
        // Increase the counter
        count++;
        // Increase and wrap the index -- make this into something
        // slightly complicated to make the loop take a little bit
        // more time - instead of a modulo operation
        index++;
        if(index >= m_length) {
            index = 0;
        }
    }
}

int main() {

    init_board();

    // Retrieve the ID for this subsystem
    uint64_t my_ss_id = shared_con_get_subsystem_id();

    // Sign-on to local serial port
    bsp_printf("Starting time quantum visualizer [subsystem ID=0x%x] \n\r", my_ss_id);

    if(my_ss_id > 0xF) {
        bsp_printf("That ID number is too large for this lab!\n\r");
        return 0;
    }

    // Generate string including ID
    char visualization_message[visualization_length+1];  // +1 to allow for the ending zero 
    //snprintf(visualization_message, visualization_length+1,
    //        "|PrintFromSubSys%03x.",my_ss_id);
    //    //   0123401234012340 1234
    
    // Repeatedly print the ID of this node - so that the scheduling is
    // obvious even if someone cannot tell the colors apart
    char c = "0123456789ABCDEF"[my_ss_id];
    memset(visualization_message,c,visualization_length);
    // terminating zero to be nice to debugger inspection 
    visualization_message[visualization_length]=0;   

    // Wait for all subsystems to start and reach the beginning point
    // -- this is expected to provide a single notification in the simulated
    //    hardware when all nodes have hit the sync.
    sync_wait_for_start();

    // Magic breakpoint
    // -- Allows capturing that this node specifically is going into
    //    its main loop. 
    MAGIC(0);

    main_loop(visualization_message,visualization_length);
    
    bsp_printf("Time quantum visualizer finished\n\r");

    return 0;
}
