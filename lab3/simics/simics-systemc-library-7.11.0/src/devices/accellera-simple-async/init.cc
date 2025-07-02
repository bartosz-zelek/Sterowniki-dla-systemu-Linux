/*
  © 2021 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <simics/systemc/sc_factory.h>

#include "simple_async_top.h"

#define CLASS_NAME "simple_async"
#define CLASS_DESC "test device for async signal"
#define CLASS_DOC  "The <class>" CLASS_NAME "</class>" \
    " runs a pure SystemC device in Simics."

struct Simulation {
    watchDog w_ {"WatchDog"};
    activity a_ {"Activity"};
};

void *setup_simulation(int argc, char *argv[]) {
    SC_REPORT_INFO("simple_async", "Start SystemC ");
    return new Simulation();
}

void teardown_simulation(void *arg) {
    Simulation *sim = static_cast<Simulation*>(arg);
    delete sim;

    SC_REPORT_INFO("simple_async", "Program completed");
}

simics::systemc::RegisterModel model(CLASS_NAME, CLASS_DESC, CLASS_DOC,
                                     setup_simulation, teardown_simulation);
