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

#ifndef SYSTEMC_CHECKPOINT_UNITTEST_SIMULATION_H
#define SYSTEMC_CHECKPOINT_UNITTEST_SIMULATION_H

#include <systemc>

namespace unittest {

class Simulation {
  public:
    Simulation()
        : former_context_(sc_core::sc_curr_simcontext),
          context_(new sc_core::sc_simcontext) {
        sc_core::sc_curr_simcontext = context_;
    }
    ~Simulation() {
        delete context_;
        sc_core::sc_curr_simcontext = former_context_;
    }

    sc_core::sc_simcontext *former_context_;
    sc_core::sc_simcontext *context_;
};

}  // namespace unittest

#endif
