// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2015 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_AWARENESS_SC_INITIATOR_GASKET_H
#define SIMICS_SYSTEMC_AWARENESS_SC_INITIATOR_GASKET_H



#include <simics/systemc/iface/sc_initiator_gasket_interface.h>
#include <simics/systemc/simics2tlm/gasket_interface.h>
#include <simics/systemc/adapter_log_groups.h>

namespace simics {
namespace systemc {
namespace awareness {

/** \internal */
class ScInitiatorGasket : public iface::ScInitiatorGasketInterface {
  public:
    explicit ScInitiatorGasket(simics::ConfObjectRef obj)
       : gasket_(NULL), simics_obj_(obj) {}
    void init(sc_core::sc_object *obj) {
        gasket_ = dynamic_cast<simics2tlm::GasketInterface*>(obj);
    }
    virtual void set_dmi(bool enable) {
        if (!gasket_) {
            SIM_LOG_CRITICAL(simics_obj_, Log_TLM,
                             "Unexpected error, object is no gasket.");
            sc_core::sc_stop();
            return;
        }
        gasket_->set_dmi(enable);
    }
    virtual bool is_dmi_enabled() {
        if (!gasket_) {
            SIM_LOG_CRITICAL(simics_obj_, Log_TLM,
                             "Unexpected error, object is no gasket.");
            sc_core::sc_stop();
            return false;
        }
        return gasket_->is_dmi_enabled();
    }
    virtual char *print_dmi_table() {
        if (!gasket_) {
            SIM_LOG_CRITICAL(simics_obj_, Log_TLM,
                             "Unexpected error, object is no gasket.");
            sc_core::sc_stop();
            return NULL;
        }
        return MM_STRDUP(gasket_->get_dmi_data_table()->print().c_str());
    }

  protected:
    simics2tlm::GasketInterface *gasket_;
    simics::ConfObjectRef simics_obj_;
};

}  // namespace awareness
}  // namespace systemc
}  // namespace simics

#endif
