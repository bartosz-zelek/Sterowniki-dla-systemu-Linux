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

#ifndef SIMICS_SYSTEMC_TLM2SIMICS_GASKET_FACTORY_H
#define SIMICS_SYSTEMC_TLM2SIMICS_GASKET_FACTORY_H


#include <tlm_utils/multi_passthrough_initiator_socket.h>

#include <simics/systemc/registry.h>
#include <simics/systemc/tlm2simics/gasket.h>
#include <simics/systemc/tlm2simics/gasket_dispatcher.h>

#include <algorithm>
#include <string>

namespace simics {
namespace systemc {
namespace tlm2simics {

namespace internal {

template<unsigned int BUSWIDTH, typename TYPES>
static Gasket<BUSWIDTH, TYPES> *createGasket(
        const std::string& socket_name,
        const simics::ConfObjectRef &simics_obj) {
    std::string name = "gasket_" + socket_name;
    std::replace(name.begin(), name.end(), '.', '_');

    const char* char_name = name.c_str();
    if (sc_core::sc_find_object(char_name))
        char_name = sc_core::sc_gen_unique_name(char_name);

    // The raw pointer is eventually managed by shared_ptr
    return new Gasket<BUSWIDTH, TYPES>(char_name, simics_obj);
}
}  // namespace internal

/// \addtogroup Factories
/// @{

/** Factory for creating a Gasket and bind it hierarchically. */
template<unsigned int BUSWIDTH, typename TYPES,
         int N, sc_core::sc_port_policy POL>
static GasketInterface::Ptr createGasket(
        tlm::tlm_target_socket<BUSWIDTH, TYPES, N, POL> *target_socket,
        const simics::ConfObjectRef &simics_obj) {
    FATAL_ERROR_IF(target_socket->name() == NULL,
                   "target socket not fully initialized, name missing");
    auto gasket = internal::createGasket<BUSWIDTH, TYPES>(
            target_socket->name(), simics_obj);
    gasket->bind(*target_socket);
    return GasketInterface::Ptr(gasket);
}

/** Factory for creating a Gasket and bind it to an initiator socket. */
template<unsigned int BUSWIDTH, typename TYPES,
         int N, sc_core::sc_port_policy POL>
static GasketInterface::Ptr createGasket(
        tlm::tlm_initiator_socket<BUSWIDTH, TYPES, N, POL> *initiator_socket,
        const simics::ConfObjectRef &simics_obj) {
    FATAL_ERROR_IF(initiator_socket->name() == NULL,
                   "initiator socket not fully initialized, name missing");
    auto gasket = internal::createGasket<BUSWIDTH, TYPES>(
            initiator_socket->name(), simics_obj);
    gasket->bind(*initiator_socket);
    return GasketInterface::Ptr(gasket);
}

/** Factory for creating multiple Gaskets and bind it to one initiator
    socket. */
template<unsigned int BUSWIDTH, typename TYPES,
         int N, sc_core::sc_port_policy POL>
static GasketInterface::Ptr createMultiGasket(
        tlm::tlm_initiator_socket<BUSWIDTH, TYPES, N, POL> *initiator_socket,
        const simics::ConfObjectRef &simics_obj) {
    FATAL_ERROR_IF(initiator_socket->name() == NULL,
                   "initiator socket not fully initialized, name missing");
    std::string name = "gasket_";
    name += initiator_socket->name();
    std::replace(name.begin(), name.end(), '.', '_');

    const char* char_name = name.c_str();
    if (sc_core::sc_find_object(char_name))
        char_name = sc_core::sc_gen_unique_name(char_name);

    return GasketDispatcher<BUSWIDTH, TYPES, N, POL>::bind(
            initiator_socket, char_name, simics_obj);
}

/** Factory for creating a Gasket and bind it to an initiator socket of type
    tlm_utils::multi_passthrough_initiator_socket. */
template<typename MODULE, unsigned int BUSWIDTH, typename TYPES,
         unsigned int N, sc_core::sc_port_policy POL>
static GasketInterface::Ptr createGasket(
        tlm_utils::multi_passthrough_initiator_socket<
            MODULE, BUSWIDTH, TYPES, N, POL> *initiator_socket,
        const simics::ConfObjectRef &simics_obj) {
    FATAL_ERROR_IF(initiator_socket->name() == NULL,
                   "initiator socket not fully initialized, name missing");
    auto gasket = internal::createGasket<BUSWIDTH, TYPES>(
            initiator_socket->name(), simics_obj);
    gasket->bind(*initiator_socket);
    return GasketInterface::Ptr(gasket);
}

GasketInterface::Ptr createGasketByName(std::string socket_name,
        const simics::ConfObjectRef &simics_obj);

class GasketFactoryInterface {
  public:
    virtual ~GasketFactoryInterface() {}
    virtual GasketInterface::Ptr create(sc_core::sc_object *object,
                                        const ConfObjectRef &simics_obj) = 0;
};

template <unsigned int BUSWIDTH, typename TYPES, int N = 1,
          sc_core::sc_port_policy POL = sc_core::SC_ONE_OR_MORE_BOUND>
class GasketFactory : public Registrant<GasketFactoryInterface> {
  public:
    typedef tlm::tlm_base_initiator_socket<BUSWIDTH,
        tlm::tlm_fw_transport_if<TYPES>,
        tlm::tlm_bw_transport_if<TYPES>,
        N, POL> Socket;

    virtual GasketInterface::Ptr create(sc_core::sc_object *object,
                                        const ConfObjectRef &simics_obj) {
        Socket *socket = dynamic_cast<Socket *>(object);
        if (!socket)
            return NULL;

        auto gasket = internal::createGasket<BUSWIDTH, TYPES>(
                socket->name(), simics_obj);
        gasket->bind(*socket);
        return GasketInterface::Ptr(gasket);
    }
};

/// @}

}  // namespace tlm2simics
}  // namespace systemc
}  // namespace simics

#endif
