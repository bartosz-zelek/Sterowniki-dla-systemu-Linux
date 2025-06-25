// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2013 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_SIMICS2TLM_GASKET_INTERFACE_H
#define SIMICS_SYSTEMC_SIMICS2TLM_GASKET_INTERFACE_H

#include <simics/conf-object.h>  // ConfObjectRef
#include <simics/systemc/class_type.h>
#include <simics/systemc/iface/transaction.h>
#include <simics/systemc/simics2tlm/dmi_data_table.h>

#include <tlm>
#include <systemc>

#include <memory>
#include <string>

namespace simics {
namespace systemc {
namespace simics2tlm {

/** Interface used by simics2tlm gaskets, implemented by Gasket base class. */
class GasketInterface {
  public:
    typedef std::shared_ptr<GasketInterface> Ptr;
    virtual bool trigger(iface::Transaction *transaction) = 0;
    virtual ConfObjectRef &simics_obj() = 0;
    virtual DmiDataTable *get_dmi_data_table() = 0;
    virtual void set_type(ClassType *type) = 0;
    virtual ClassType *type() = 0;
    virtual sc_core::sc_object *get_target_socket() = 0;
    virtual void set_dmi(bool enable) = 0;
    virtual bool is_dmi_enabled() = 0;
    virtual std::string gasket_name() const = 0;
    virtual ~GasketInterface() {}

    /** Deprecated, use the TransactionPool::acquire() instead. */
    virtual tlm::tlm_generic_payload &payload() = 0;
    /** Deprecated, use the trigger(iface::Transaction *transaction) instead. */
    virtual bool trigger_transaction() = 0;
    /** Deprecated, use the TransactionExtension::set_transport_debug(bool)
        instead. */
    virtual void set_inquiry(bool inquiry) = 0;
};

}  // namespace simics2tlm
}  // namespace systemc
}  // namespace simics

#endif
