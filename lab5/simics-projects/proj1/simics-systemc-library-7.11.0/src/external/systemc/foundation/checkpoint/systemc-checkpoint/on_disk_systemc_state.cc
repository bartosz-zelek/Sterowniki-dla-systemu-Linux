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

#include <systemc-checkpoint/iarchive.h>
#include <systemc-checkpoint/istate_tree.h>
#include <systemc-checkpoint/kernel.h>
#include <systemc-checkpoint/oarchive.h>
#include <systemc-checkpoint/on_disk_state.h>
#include <systemc-checkpoint/ostate_tree.h>
#include <systemc-checkpoint/serialization/smd.h>
#include <systemc-checkpoint/serializer_registry.h>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <iostream>
#include <string>
#include <vector>

#if __cplusplus >= 201703L || _MSVC_LANG >= 201703L
#include <filesystem>  // NOLINT(build/c++17)
using std::filesystem::path;
#else
#include <experimental/filesystem>
using std::experimental::filesystem::path;
#endif

using boost::property_tree::json_parser_error;
using boost::property_tree::read_json;
using boost::property_tree::write_json;
using std::string;
using std::vector;

namespace sc_checkpoint {

static string checkpointPath(string checkpoint_dir) {
    return (path(checkpoint_dir) / path("checkpoint.json")).string();
}

template <typename RegistryImpl, typename KernelImpl>
OnDiskSystemCState<RegistryImpl, KernelImpl>::OnDiskSystemCState(
    RegistryImpl *serializer_registry,
    KernelImpl *kernel,
    SerializeCallbackInterface *notify_callbacks,
    bool track_kernel_state) : serializer_registry_(serializer_registry),
                               kernel_(kernel),
                               notify_callbacks_(notify_callbacks),
                               track_kernel_state_(track_kernel_state) {}

template <typename RegistryImpl, typename KernelImpl>
OnDiskSystemCState<RegistryImpl, KernelImpl>::~OnDiskSystemCState()
{}

template <typename RegistryImpl, typename KernelImpl>
bool OnDiskSystemCState<RegistryImpl, KernelImpl>::write(
    const std::string &dir, bool persistent) {
    boost::property_tree::ptree ptree;
    OStateTree state_tree(&ptree);
    OArchive archive(&state_tree);

    archive << serialization::create_smd("modules", *serializer_registry_);

    notify_callbacks_->post_serialize_callback(
        SerializeCallbackInterface::State_Saving);

    // TODO(enilsson): only save persistent state
    if (track_kernel_state_) {
        archive << serialization::create_smd("kernel", *kernel_);
    }

    try {
            write_json(checkpointPath(dir), ptree);
    } catch(json_parser_error e) {
            std::cerr << e.what() << std::endl;
            return false;
    }
    return true;
}

// TODO(enilsson): at this time, the complete SystemC state is saved in all
// checkpoints, regardless whether or not they're 'full' or 'partial'. In the
// future this is likely to change; at that time the state from the entire list
// of checkpoints should be loaded, not just the latest one.
template <typename RegistryImpl, typename KernelImpl>
bool OnDiskSystemCState<RegistryImpl, KernelImpl>::read(
    const vector<string> &dirs, bool persistent) {
    boost::property_tree::ptree ptree;
    try {
            read_json(checkpointPath(dirs.back()), ptree);
    } catch(json_parser_error e) {
            std::cerr << e.what() << std::endl;
            return false;
    }

    // TODO(enilsson): only load persistent state
    IStateTree state_tree(&ptree);
    IArchive archive(&state_tree);
    archive >> serialization::create_smd("modules", *serializer_registry_);

    notify_callbacks_->post_serialize_callback(
        SerializeCallbackInterface::State_Loading);

    if (track_kernel_state_) {
        archive >> serialization::create_smd("kernel", *kernel_);
    }

    return true;
}

template
OnDiskSystemCState<SerializerRegistry, Kernel>::OnDiskSystemCState(
        SerializerRegistry *, Kernel *, SerializeCallbackInterface *, bool);

template
OnDiskSystemCState<
        SerializerRegistry, Kernel>::~OnDiskSystemCState();

template
bool OnDiskSystemCState<SerializerRegistry, Kernel>::write(
        const string &, bool);

template
bool OnDiskSystemCState<SerializerRegistry, Kernel>::read(
        const vector<string> &, bool);

}  // namespace sc_checkpoint
