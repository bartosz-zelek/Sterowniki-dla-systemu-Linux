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

#include <systemc-checkpoint/checkpoint_control.h>

#include <boost/filesystem.hpp>

#include <string>
#include <vector>

#include "doc-systemc-checkpoint.h"

boost::filesystem::path create_scratch_directory() {
        boost::filesystem::path scratch_directory = "scratch";
        boost::filesystem::remove_all(scratch_directory);
        boost::filesystem::create_directory(scratch_directory);
        return scratch_directory;
}

void write_read(sc_checkpoint::CheckpointControl &control) {
        boost::filesystem::path scratch = create_scratch_directory();

        std::vector<std::string> checkpoints;
        checkpoints.push_back((scratch / "write").string());
        assert(control.write_standalone(checkpoints.back(), false));
        assert(control.read(checkpoints, false));
}

void save_load(sc_checkpoint::CheckpointControl &control) {
        void *handle = NULL;
        assert(control.save(&handle));
        assert(control.restore(handle));
}

int sc_main(int argc, char *argv[]) {
        Top device("dut");
        sc_checkpoint::CheckpointControl control;

        write_read(control);
        save_load(control);

        return 0;
}

