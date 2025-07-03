//               -*- c++ -*-

/*
  © 2015 Intel Corporation
*/

//:: pre simics-main.cc {{
#include <simics/systemc/sc_factory.h>
#include "sc-device.h"

#define CLASS_NAME "empty_device_sc_only"
#define CLASS_DESC "example SystemC device"
#define CLASS_DOC  "The <class>" CLASS_NAME "</class>" \
    " class runs a SystemC-only device in Simics."

simics::systemc::RegisterModel model(CLASS_NAME, CLASS_DESC, CLASS_DOC,
                                     setup, teardown);
// }}
