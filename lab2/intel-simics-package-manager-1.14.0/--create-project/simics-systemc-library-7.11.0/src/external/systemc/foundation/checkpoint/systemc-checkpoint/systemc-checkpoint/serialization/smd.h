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

#ifndef SYSTEMC_CHECKPOINT_SERIALIZATION_SMD_H
#define SYSTEMC_CHECKPOINT_SERIALIZATION_SMD_H

#include <systemc-checkpoint/serialization/serializable_meta_data.h>

#include <cci/serialization/base_object.hpp>

#define SMD(var) (sc_checkpoint::serialization::create_smd(#var, var))

#define SMD_BASE_OBJECT(var) (sc_checkpoint::serialization::create_smd( \
        #var, cci::serialization::base_object<var>(*this)))

#endif  // SYSTEMC_CHECKPOINT_SERIALIZATION_SMD_H
