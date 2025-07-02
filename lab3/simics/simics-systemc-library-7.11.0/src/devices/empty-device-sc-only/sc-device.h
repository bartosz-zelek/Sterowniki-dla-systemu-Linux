//               -*- c++ -*-

/*
  © 2015 Intel Corporation
*/

//:: pre sc-device.h {{
#ifndef EMPTY_DEVICE_SC_ONLY_SC_DEVICE_H
#define EMPTY_DEVICE_SC_ONLY_SC_DEVICE_H

#include <systemc>

SC_MODULE(DeviceModel) {
  public:
    SC_CTOR(DeviceModel) : count_(10) {
        SC_THREAD(run_test);
    }

    void set_count(int count) { count_ = count; }
    int get_count() const { return count_; }

 private:
    void run_test();

    int count_;
};

sc_core::sc_module *setup(int argc, char *argv[]);
void teardown(sc_core::sc_module *top_level);

#endif  // EMPTY_DEVICE_SC_ONLY_SC_DEVICE_H
// }}
