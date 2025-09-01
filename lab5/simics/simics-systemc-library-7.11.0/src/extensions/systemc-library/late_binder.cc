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

#include <simics/systemc/late_binder.h>
#include <simics/systemc/traverser.h>

#if INTC_EXT
#include <sysc/intc/communication/unconnected_signal.h>
#include <tlm_core/tlm_2/intc/tlm_sockets/unconnected_sockets.h>

#include <sysc/communication/sc_signal_ports.h>  // for dynamic casts
#include <tlm_utils/multi_socket_bases.h>  // for dynamic casts
#include <tlm_core/tlm_2/tlm_2_interfaces/tlm_fw_bw_ifs.h>  // for TYPES

#include <vector>
#include <string>

namespace simics {
namespace systemc {

LateBinder::~LateBinder() {
    std::vector<intc::UnconnectedBase*>::iterator it;
    for (it = unconnected_objects_.begin();
         it != unconnected_objects_.end(); ++it) {
        delete (*it);
    }
}

template <typename T, typename P>
void LateBinder::bind_unconnected_port(P *port) {
    T *o = new T(port);
    port->bind(*o);
    unconnected_objects_.push_back(o);
}

template <typename T>
bool LateBinder::signal_bind(sc_core::sc_port_base *port) {
    typename sc_core::sc_in<T>::this_type *sc_in_port
        = dynamic_cast<typename sc_core::sc_in<T>::this_type *>(port);
    if (sc_in_port) {
        bind_unconnected_port<intc::UnconnectedSignal<T> >(sc_in_port);
        return true;
    }
    typename sc_core::sc_out<T>::this_type *sc_out_port
        = dynamic_cast<typename sc_core::sc_out<T>::this_type *>(port);
    if (sc_out_port) {
        bind_unconnected_port<intc::UnconnectedSignal<T> >(sc_out_port);
        return true;
    }
    typename sc_core::sc_inout<T>::this_type *sc_inout_port
        = dynamic_cast<typename sc_core::sc_inout<T>::this_type *>(port);
    if (sc_inout_port) {
        bind_unconnected_port<intc::UnconnectedSignal<T> >(sc_inout_port);
        return true;
    }

    return false;
}

template <unsigned int BUSWIDTH, typename TYPES>
class BindTraverser : public Traverser {
  public:
    BindTraverser(typename tlm_utils::multi_target_base<
                  BUSWIDTH, TYPES>::port_type *target_port) {
        std::string pn(target_port->name());
        std::size_t pos = pn.find_last_of("_port");
        // it's safe to assume '_port' is found in port name, as this is
        // hard-coded in the SystemC kernel for all target socket ports
        assert(pos != std::string::npos);
        target_socket_name_ = pn.substr(0, pos - 5);
    }

    virtual void applyOn(sc_core::sc_object *obj) {
        typename tlm_utils::multi_target_base<
            BUSWIDTH, TYPES>::export_type *test
            = dynamic_cast<typename tlm_utils::multi_target_base<
                BUSWIDTH, TYPES>::export_type *>(obj);
        if (test && target_socket_name_.compare(obj->name()) == 0) {
            // To bypass the internal sanity checks made by the
            // multi_passthrough_target_socket::end_of_elaboration() callback,
            // we exclude this object from the kernel callbacks. This is OK,
            // since we have already bound the port and the export's BW_IF is
            // not to be used without someone calling the port's FW_IF first.
            sc_core::sc_get_curr_simcontext()->late_binder_insert_export(test);
        }
    }
    virtual void done() {
    }

  private:
    std::string target_socket_name_;
};

template <unsigned int BUSWIDTH, typename TYPES>
bool LateBinder::socket_bind(sc_core::sc_port_base *port) {
    sc_core::sc_port<tlm::tlm_fw_transport_if<TYPES> > *fw_port
        = dynamic_cast<sc_core::sc_port<tlm::tlm_fw_transport_if<TYPES> > *>(
                port);
    if (fw_port) {
        bind_unconnected_port<intc::UnconnectedFwTransport<TYPES> >(fw_port);
        return true;
    }
    sc_core::sc_port<tlm::tlm_bw_transport_if<TYPES> > *bw_port
        = dynamic_cast<sc_core::sc_port<tlm::tlm_bw_transport_if<TYPES> > *>(
                port);
    if (bw_port) {
        bind_unconnected_port<intc::UnconnectedBwTransport<TYPES> >(bw_port);
        return true;
    }
    // Multi init works a bit differently
    typename tlm_utils::multi_init_base<
        BUSWIDTH, TYPES>::port_type *multi_init_port
        = dynamic_cast<typename tlm_utils::multi_init_base<
            BUSWIDTH, TYPES>::port_type *>(port);
    if (multi_init_port) {
        bind_unconnected_port<
            intc::UnconnectedFwTransport<TYPES> >(multi_init_port);
        return true;
    }
    // Multi target also works a bit differently
    typename tlm_utils::multi_target_base<
        BUSWIDTH, TYPES>::port_type *multi_target_port
        = dynamic_cast<typename tlm_utils::multi_target_base<
            BUSWIDTH, TYPES>::port_type *>(port);
    if (multi_target_port) {
        bind_unconnected_port<
            intc::UnconnectedBwTransport<TYPES> >(multi_target_port);
        // The BindTraverser ignores the
        // multi_passthrough_target_socket::end_of_elaboration() callback,
        // where additional sanity checks are run which are not valid if
        // sockets are not bound.
        BindTraverser<BUSWIDTH, TYPES> t(multi_target_port);
        t.traverseAll();
        return true;
    }

    return false;
}

// the callback
void LateBinder::bind(sc_core::sc_port_base *port) {
    // sc_in/sc_out/sc_inout for standard types
    if (signal_bind<bool>(port)) return;
    if (signal_bind<int>(port)) return;
    if (signal_bind<double>(port)) return;
    if (signal_bind<sc_dt::uint64>(port)) return;
    if (signal_bind<sc_dt::sc_logic>(port)) return;

    // FW or BW IF for sockets
    if (socket_bind<32, tlm::tlm_base_protocol_types>(port)) return;
    if (socket_bind<64, tlm::tlm_base_protocol_types>(port)) return;

#ifndef SYSTEMC_IGNORE_UNCONNECTED_PORTS
    SC_REPORT_ERROR("/intel/LateBinder",
                    "port type not supported => left unconnected");
#endif
}

}  // namespace systemc
}  // namespace simics
#endif
