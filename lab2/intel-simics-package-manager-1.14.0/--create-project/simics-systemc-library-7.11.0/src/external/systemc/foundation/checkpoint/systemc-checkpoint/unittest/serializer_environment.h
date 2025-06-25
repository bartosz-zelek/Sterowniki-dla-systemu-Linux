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

#ifndef SYSTEMC_CHECKPOINT_UNITTEST_SERIALIZER_ENVIRONMENT_H
#define SYSTEMC_CHECKPOINT_UNITTEST_SERIALIZER_ENVIRONMENT_H

#include <boost/property_tree/ptree.hpp>

#include <systemc-checkpoint/iarchive.h>
#include <systemc-checkpoint/istate_tree.h>
#include <systemc-checkpoint/kernel.h>
#include <systemc-checkpoint/oarchive.h>
#include <systemc-checkpoint/ostate_tree.h>
#include <systemc-checkpoint/serialization/serializer.h>
#include <systemc-checkpoint/serialization/smd.h>

#include <unittest/simulation.h>

namespace unittest {

class SerializerEnvironment {
  public:
    void store() {
        sc_checkpoint::OStateTree ostate(&ptree);
        sc_checkpoint::OArchive oa(&ostate);
        oa & sc_checkpoint::serialization::create_smd("modules",
                        *sc_checkpoint::SerializerRegistry::Instance());
    }
    void restore() {
        sc_checkpoint::IStateTree istate(&ptree);
        sc_checkpoint::IArchive ia(&istate);
        ia & sc_checkpoint::serialization::create_smd("modules",
                        *sc_checkpoint::SerializerRegistry::Instance());
    }
    void storeWithKernel() {
        sc_checkpoint::OStateTree ostate(&ptree);
        sc_checkpoint::OArchive oa(&ostate);
        oa & sc_checkpoint::serialization::create_smd("modules",
                        *sc_checkpoint::SerializerRegistry::Instance());
        oa & sc_checkpoint::serialization::create_smd("kernel", kernel_);
    }
    void restoreWithKernel() {
        // Perform model init in accordance with Adapter implementation.
        sc_start(sc_core::SC_ZERO_TIME);

        reverseWithKernel();
    }
    void reverseWithKernel() {
        sc_checkpoint::IStateTree istate(&ptree);
        sc_checkpoint::IArchive ia(&istate);
        ia & sc_checkpoint::serialization::create_smd("modules",
                        *sc_checkpoint::SerializerRegistry::Instance());
        ia & sc_checkpoint::serialization::create_smd("kernel", kernel_);
    }

    unittest::Simulation simulation_;
    boost::property_tree::ptree ptree;
    sc_checkpoint::Kernel kernel_;
};

}  // namespace unittest

#endif
