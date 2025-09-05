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

#ifndef SIMICS_SYSTEMC_CHECKPOINT_H
#define SIMICS_SYSTEMC_CHECKPOINT_H

#include <simics/systemc/iface/checkpoint_interface.h>
#include <simics/systemc/iface/simulation_interface.h>
#include <simics/systemc/iface/temporal_state_interface.h>
#include <systemc-checkpoint/time_information_interface.h>

#include <string>
#include <vector>

namespace simics {
namespace systemc {

class CheckpointControl;

class Checkpoint : public iface::CheckpointInterface,
                   public iface::TemporalStateInterface,
                   public sc_checkpoint::ExternalTimeInformationInterface {
  public:
    explicit Checkpoint(iface::SimulationInterface *simulation);
    Checkpoint(const Checkpoint&) = delete;
    Checkpoint& operator=(const Checkpoint&) = delete;
    virtual ~Checkpoint();

    // CheckpointInterface
    virtual void save(const char *path);
    virtual void finish(int success);
    virtual int has_persistent_data();

    // TemporalStateInterface
    virtual lang_void *save();
    virtual void merge(lang_void *prev, lang_void *killed);
    virtual void prepare_restore();
    virtual void finish_restore(lang_void *state);

    // ExternalTimeInformationInterface
    virtual void set_time_information(int64_t high, uint64_t low);
    virtual void time_information(int64_t *high, uint64_t *low);

    std::vector<std::string> systemcState() const;
    void setSystemcState(const std::vector<std::string> &checkpoints);

    void readCheckpointFromDisk();

  protected:
    void initializeCheckpoint(CheckpointControl *control);

  private:
    iface::SimulationInterface *simulation_;
    CheckpointControl *control_;

    std::vector<std::string> checkpoints_;
};

}  // namespace systemc
}  // namespace simics

#endif  // SIMICS_SYSTEMC_CHECKPOINT_H
