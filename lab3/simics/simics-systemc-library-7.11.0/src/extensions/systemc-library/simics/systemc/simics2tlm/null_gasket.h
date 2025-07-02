// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2014 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_SIMICS2TLM_NULL_GASKET_H
#define SIMICS_SYSTEMC_SIMICS2TLM_NULL_GASKET_H

#include <simics/systemc/simics2tlm/gasket_interface.h>
#include <simics/systemc/instance_counter.h>

#include <string>

namespace simics {
namespace systemc {
namespace simics2tlm {

/** \internal
 * Utility class that counts the number of instances. This class is used to
 * validate that all gaskets have been assigned before running the
 * simulation.
 */
class NullGasket : public GasketInterface,
                   public simics::systemc::InstanceCounter<NullGasket> {
  public:
    virtual bool trigger(iface::Transaction *transaction) {
        return false;
    }
    virtual simics::ConfObjectRef &simics_obj() {
        return obj_;
    }
    virtual DmiDataTable *get_dmi_data_table() {
        return &dmi_data_table_;
    }
    virtual void set_type(ClassType *type) {}
    virtual ClassType *type() {
        return NULL;
    }
    virtual sc_core::sc_object *get_target_socket() {
        return NULL;
    }
    virtual void set_dmi(bool enable) {
    }
    virtual bool is_dmi_enabled() {
        return false;
    }
    std::string gasket_name() const {
        return "";
    }

    // deprecated
    virtual tlm::tlm_generic_payload &payload() {
        return transaction_;
    }
    virtual bool trigger_transaction() {
        return false;
    }
    virtual void set_inquiry(bool inquiry) {
    }

  private:
    simics::ConfObjectRef obj_;
    tlm::tlm_generic_payload transaction_;
    DmiDataTable dmi_data_table_;
};

}  // namespace simics2tlm
}  // namespace systemc
}  // namespace simics

#endif
