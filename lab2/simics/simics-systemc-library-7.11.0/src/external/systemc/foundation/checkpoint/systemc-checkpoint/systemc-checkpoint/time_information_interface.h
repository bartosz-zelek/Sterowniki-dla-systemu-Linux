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

#ifndef SYSTEMC_CHECKPOINT_TIME_INFORMATION_INTERFACE_H
#define SYSTEMC_CHECKPOINT_TIME_INFORMATION_INTERFACE_H

#include <stdint.h>

namespace sc_checkpoint {

class ExternalTimeInformationInterface {
  public:
    virtual ~ExternalTimeInformationInterface() {}
    virtual void set_time_information(int64_t high, uint64_t low) = 0;
    virtual void time_information(int64_t *high, uint64_t *low) = 0;
};

}  // namespace sc_checkpoint

#endif
