// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2016 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#if INTC_EXT && USE_SIMICS_CHECKPOINTING

#include <simics/systemc/adapter_log_groups.h>
#include <simics/systemc/checkpoint.h>
#include <simics/systemc/checkpoint_control.h>
#include <simics/systemc/context.h>
#include <simics/systemc/internals.h>
#include <simics/systemc/kernel_state_modifier.h>

#if __cplusplus >= 201703L || _MSVC_LANG >= 201703L
#include <filesystem>  // NOLINT(build/c++17)
using std::filesystem::path;
#else
#include <experimental/filesystem>
using std::experimental::filesystem::path;
#endif

#include <string>
#include <utility>
#include <vector>

namespace simics {
namespace systemc {

Checkpoint::Checkpoint(iface::SimulationInterface *simulation)
    : simulation_(simulation), control_(NULL) {}

Checkpoint::~Checkpoint() {
    delete control_;
}

// CheckpointInterface
void Checkpoint::save(const char *sim_chkpt) {
    assert(control_);

    Context context(simulation_);
    assert(std::string(sim_chkpt) != "");

    std::string device_name = simulation_->simics_object().name();

    std::vector<std::string> checkpoints = checkpoints_;
    checkpoints.push_back(
        (sim_chkpt / path("systemc") / path(device_name)).string());

    // TODO(enilsson): determine whether or not we should write only the
    // persistent state or all state
    bool success = checkpoints.size() == 1
        ? control_->write_standalone(checkpoints.back(), false)
        : control_->write(checkpoints.back(), false);
    if (!success) {
        SIM_LOG_ERROR(simulation_->simics_object().object(), Log_Configuration,
                      "Could not write state to disk for %s!",
                      device_name.c_str());
    } else {
        checkpoints_ = std::move(checkpoints);
    }
}

void Checkpoint::finish(int success) {}

int Checkpoint::has_persistent_data() {
    return 0;
}

// TemporalStateInterface
lang_void *Checkpoint::save() {
    assert(control_);
    Context context(simulation_);
    lang_void *p = NULL;
    if (!control_->save(&p)) {
        SIM_LOG_ERROR(simulation_->simics_object().object(), Log_Configuration,
                      "Could not save temporal state for %s!",
                      simulation_->simics_object().name().c_str());
    }
    return p;
}

void Checkpoint::merge(lang_void *prev, lang_void *killed) {
    assert(control_);
    Context context(simulation_);
    if (!control_->merge(prev, killed)) {
        SIM_LOG_ERROR(simulation_->simics_object().object(), Log_Configuration,
                      "Could not merge temporal states for %s!",
                      simulation_->simics_object().name().c_str());
    }
}

void Checkpoint::prepare_restore() {}

void Checkpoint::finish_restore(lang_void *state) {
    assert(control_);
    Context context(simulation_);
    KernelStateModifier m(simulation_);
    if (!control_->restore(state)) {
        SIM_LOG_ERROR(simulation_->simics_object().object(), Log_Configuration,
                      "Could not restore temporal state for %s!",
                      simulation_->simics_object().name().c_str());
    }
}

// The following two methods are only invoked by Adapter::finalize() so we can
// assume the correct context has already been set
void Checkpoint::set_time_information(int64_t high, uint64_t low) {
    assert(control_);
    control_->set_time_information(high, low);
}
void Checkpoint::time_information(int64_t *high, uint64_t *low) {
    assert(control_);
    control_->time_information(high, low);
}

std::vector<std::string> Checkpoint::systemcState() const {
    return checkpoints_;
}

void Checkpoint::setSystemcState(const std::vector<std::string> &checkpoints) {
    // TODO(enilsson): The attribute setter only make sense when invoked from
    // checkpoint restore, and it looks like an error to update checkpoints_
    // from CLI/script as that will have an impact on the
    // checkpoint-to-be-written. I.e. there is an assumption that the state of
    // checkpoints_ is sane and either zero (starting fresh) or non-zero
    // (coming from a series of checkpoint save/restore).
    if (SIM_object_is_configured(simulation_->simics_object())
        && !SIM_is_restoring_state(simulation_->simics_object())) {
        // TODO(enilsson): Maybe support setting same value?
        const char *msg = ("The state attribute may not be changed once"
                           " the object has been instantiated.");
        throw std::runtime_error(msg);
    }

    checkpoints_ = checkpoints;
}

void Checkpoint::initializeCheckpoint(CheckpointControl *control) {
    control_ = control;
}

void Checkpoint::readCheckpointFromDisk() {
    assert(control_);
    std::string device_name = simulation_->simics_object().name();
    // TODO(enilsson): determine whether or not to read only persistent state
    Context context(simulation_);
    KernelStateModifier m(simulation_);
    if (!control_->read(checkpoints_, false)) {
        SIM_LOG_ERROR(simulation_->simics_object().object(), Log_Configuration,
                      "Could not read state from disk for %s!",
                      device_name.c_str());
    }
}

}  // namespace systemc
}  // namespace simics

#endif
