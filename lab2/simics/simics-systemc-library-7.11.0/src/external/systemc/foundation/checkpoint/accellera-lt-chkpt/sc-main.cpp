// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*****************************************************************************

  Licensed to Accellera Systems Initiative Inc. (Accellera) under one or
  more contributor license agreements.  See the NOTICE file distributed
  with this work for additional information regarding copyright ownership.
  Accellera licenses this file to you under the Apache License, Version 2.0
  (the "License"); you may not use this file except in compliance with the
  License.  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
  implied.  See the License for the specific language governing
  permissions and limitations under the License.

 *****************************************************************************/

#define REPORT_DEFINE_GLOBALS

#include "lt/include/lt_top.h"

#include <common/include/reporting.h>
#include <systemc-checkpoint/checkpoint_control.h>
#include <tlm.h>

#include <exception>
#include <sstream>
#include <string>
#include <vector>

void write_checkpoint() {
    sc_checkpoint::CheckpointControl control;
    if(!control.write_standalone("accellera-lt-checkpoint", false)) {
        throw std::runtime_error("Could not write checkpoint to disk.");
    }
}

void read_checkpoint() {
    std::vector<std::string> checkpoints;
    checkpoints.push_back("accellera-lt-checkpoint");

    sc_checkpoint::CheckpointControl control;
    if(!control.read(checkpoints, false)) {
        throw std::runtime_error("Could not load checkpoint from disk.");
    }
}

int numeric_arg(int argc, char *argv[]) {
    int numeric_arg = -1;
    if (argc >= 2) {
        std::istringstream arg(argv[1]);
        if (!(arg >> numeric_arg) || !arg.eof()) {
            return -1;
        }
    }
    return numeric_arg;
}

int sc_main(int argc, char *argv[]) {
    REPORT_ENABLE_ALL_REPORTING();

    lt_top device("dut");
    switch(numeric_arg(argc, argv)) {
    case 0:
        sc_core::sc_start(1000, sc_core::SC_PS);
        break;
    case 1:
        sc_core::sc_start(500, sc_core::SC_PS);
        write_checkpoint();
        break;
    case 2:
        read_checkpoint();
        sc_core::sc_start(500, sc_core::SC_PS);
        break;
    default:
        throw std::runtime_error(
            "Please provide one of the following arguments:\n"
            "0 - Instantiate device and run for 1000 PS\n"
            "1 - Instantiate device, run for 500 PS, and save a checkpoint\n"
            "2 - Instantiate device, load the checkpoint, and run for 500 PS");
    }

    return 0;
}
