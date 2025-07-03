//               -*- c++ -*-

/*
  © 2015 Intel Corporation
*/

//:: pre sc-main.cc {{
#include "sc-device.h"

int sc_main(int argc, char *argv[]) {
    sc_core::sc_module *top_level = setup(argc, argv);
    // coverity[fun_call_w_exception]
    sc_core::sc_start();
    teardown(top_level);

    return 0;
}
// }}
