/*
  © 2015 Intel Corporation
*/

//:: pre sc-device.cc {{
#include "sc-device.h"

namespace {
const char *const TAG = "intel/empty-device-sc-only/info";
}

void DeviceModel::run_test() {
    while (--count_ >= 0) {
        SC_REPORT_INFO(TAG, "Looping...");
        wait(sc_core::sc_time(500, sc_core::SC_NS));
    }
}

sc_core::sc_module *setup(int argc, char *argv[]) {
    DeviceModel *top = new DeviceModel("top");
    if (argc > 1) {
        int count = atoi(argv[1]);
        top->set_count(count);
    }

    return top;
}

void teardown(sc_core::sc_module *top_level) {
    delete top_level;
}
// }}
