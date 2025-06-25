// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2019 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef TLM2SIMICS_TRANSACTION_H
#define TLM2SIMICS_TRANSACTION_H

#include <simics/systemc/interface_provider.h>
#include <simics/systemc/tlm2simics/dmi_transaction_handler.h>
#include <vector>

namespace simics {
namespace systemc {
namespace tlm2simics {

//:: pre tlm2simics_Transaction {{
/** Protocol specific transaction handler for Simics transaction interface.
 */
class Transaction : public InterfaceProvider,
                    public DmiTransactionHandler {
  public:
    Transaction();
    virtual ~Transaction();

    // DmiTransactionHandler
    void set_gasket(GasketInterface::Ptr gasketInterface) override;
  private:
    tlm::tlm_response_status simics_transaction(
            ConfObjectRef &simics_obj,
            tlm::tlm_generic_payload *trans) override;
    unsigned int debug_transaction(ConfObjectRef &simics_obj,
                                   tlm::tlm_generic_payload *trans) override;

    unsigned int transaction(ConfObjectRef &simics_obj,  // NOLINT
                             tlm::tlm_generic_payload *trans, bool inquiry);

    /*
     * Update the Simics transaction atoms before sending it to the Simics side
     * By default, it is empty since the basic required atoms are already filled.
     * It can be used to update the existing atoms or add new customized atoms.
     *
     * @param tlm_transaction the TLM transaction received from SystemC side
     * @param atoms the atoms used in the Simics transaction sent to the Simics
     *              side. No need to add ATOM_LIST_END here.
     */
    virtual void add_custom_atoms(
            const tlm::tlm_generic_payload *tlm_transaction,
            std::vector<atom_t> *atoms) {}

    class UpdateTarget : public InterfaceProvider::TargetUpdateListener {
      public:
        UpdateTarget() : map_target_(NULL) {}
        virtual ~UpdateTarget() {
            if (map_target_)
                SIM_free_map_target(map_target_);
        }

        // InterfaceProvider::TargetUpdateListener
        void update_target(ConfObjectRef old_target,
                           ConfObjectRef new_target) override {
            if (map_target_) {
                SIM_free_map_target(map_target_);
                map_target_ = nullptr;
            }

            if (new_target) {
                map_target_ = SIM_new_map_target(new_target.object(),
                                                 NULL, NULL);
            }
        }

        map_target_t *map_target() {
            return map_target_;
        }

      private:
        map_target_t *map_target_;
    };

    UpdateTarget update_target_;
    // The proxy object for the target_socket used in this gasket
    conf_object_t *target_socket_proxy_obj_ {nullptr};
};
// }}

}  // namespace tlm2simics
}  // namespace systemc
}  // namespace simics

#endif
