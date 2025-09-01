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

//
// Demos for particular hardware features
// 
void sync_demo() {
    // After the startup, see if all units reach this point 
    // -- which can take interesting amounts of time in case the processors 
    //    are set to varying clock speeds. Of course things will immediately 
    //    diverge again, but at least we have a hook point. 
    sync_wait_for_start();
    bsp_printf("\033[38;5;10mSynchronization accomplished, all units at the same point\033[38;5;7m\n\r");
    MAGIC(0);

}

void print_demo() {
    const char* my_message="Hello from me again!!\n\r";
    unsigned int count = 0;
    while(count < 42) {
        count++;
        bsp_printf("count %d\n\r", count);
        shared_con_put_string(my_message);
    }
}

void event_counter_demo() {
    bsp_printf("Event device is current running? %d\n\r", local_counter_is_running());

    local_counter_start(10000); //that's 100 wait_a_while units
    bsp_printf("Event device is now running? %d\n\r", local_counter_is_running());
    uint64_t ev_counter = local_counter_get_counter();
    while(ev_counter < 42) {
        wait_a_while(10);
        to_uart("Wake up\n\r");
        if (ev_counter != local_counter_get_counter()) {
            ev_counter = local_counter_get_counter();
            bsp_printf("Event count changed to %d\n\r", ev_counter);
        }
    }
    local_counter_stop();
}

int mailbox_demo() {  
  if (mailbox_get_far_end_id()) {  // we are sender
      bsp_printf("Sleeping for one second now (%llu)\n\r", get_mtime());
      wait_a_while(1000000); //sleep for 1 second
      uint64_t data = 0xbadc0ffedeadbeefULL;
      bsp_printf("Now sending data 0x%llx (%llu)\n\r", data, get_mtime());
      mailbox_send_request_data(data);
      data = mailbox_get_response_data(data);
      bsp_printf("Got response data 0x%llx (%llu)\n\r", data, get_mtime());
  } else {  // we are received
      bsp_printf("Waiting for incoming message now (%llu)\n\r", get_mtime());
      mailbox_wait_for_request();
      uint64_t rcv_time= get_mtime();
      uint64_t data = mailbox_get_request_data();
      bsp_printf("Received data 0x%llx (%llu)\n\r", data, rcv_time);
      bsp_printf("Sleeping for one second now (%llu)\n\r", get_mtime());
      wait_a_while(1000000); //sleep for 1 second
      data += 1;
      bsp_printf("Now sending response 0x%llx (%llu)\n\r", data, get_mtime());   
      mailbox_set_response_data(data);
  }
}


int main() {

    init_board();

    // Feature 1: reading my ID
    // Retrieve the ID for this subsystem
    uint64_t my_ss_id = shared_con_get_subsystem_id();

    // Feature 2: reading time and waiting for a second
    uint64_t mtime_start = get_mtime();
    wait_a_while(1000000); //sleep for 1 second
    uint64_t mtime_end = get_mtime();
    bsp_printf("Hello World from <\033[38;5;11munit %d\033[38;5;7m>!  Time at start was: %llu\n\r", my_ss_id, mtime_start);
    bsp_printf("and now the time (after a wait call is) is: %llu\n\r", mtime_end);

    // Feature 3: synchronization 
    sync_demo();

    // Feature 4: printing to local and shared console
    print_demo();

    // Feature 5: the event counter devices
    event_counter_demo();

    // Feature 6: the mailboxes
    mailbox_demo();

}
