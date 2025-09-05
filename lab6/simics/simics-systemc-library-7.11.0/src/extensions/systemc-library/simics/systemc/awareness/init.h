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

#ifndef SIMICS_SYSTEMC_AWARENESS_INIT_H
#define SIMICS_SYSTEMC_AWARENESS_INIT_H

#include <simics/systemc/adapter.h>
#include <simics/systemc/awareness/tlm_spy_factory.h>
#include <simics/systemc/awareness/tlm_spy_factory_registry.h>
#include <simics/systemc/awareness/proxy_factory_socket.h>
#include <simics/systemc/injection/inject_gp.h>
#include <simics/systemc/injection/extension/inject_ethernet_common.h>
#include <simics/systemc/injection/extension/inject_i2c_master_v2.h>
#include <simics/systemc/injection/extension/inject_i2c_slave_v2.h>
#include <simics/systemc/injection/extension/inject_map_info.h>
#include <simics/systemc/injection/extension/inject_pci_bus.h>
#include <simics/systemc/injection/extension/inject_pci_device.h>
#include <simics/systemc/injection/extension/inject_pci_express.h>
#include <simics/systemc/injection/extension/inject_pci_upstream_operation.h>
#include <simics/systemc/injection/extension/inject_serial_device.h>

#include <simics/systemc/simics2tlm/gasket_factory.h>
#include <simics/systemc/tlm2simics/gasket_factory.h>

#include <tlm>

namespace simics {
namespace systemc {
namespace awareness {

template <typename TPAYLOAD>
void registerInjectorType() {
    static injection::InjectGp<TPAYLOAD> injectGp;
    static injection::extension::InjectEthernetCommon<TPAYLOAD>
        injectEthernetCommon;
    static injection::extension::InjectI2cMasterV2<TPAYLOAD> injectI2cMasterV2;
    static injection::extension::InjectI2cSlaveV2<TPAYLOAD> injectI2cSlaveV2;
    static injection::extension::InjectMapInfo<TPAYLOAD> injectMapInfo;
    static injection::extension::InjectPciBus<TPAYLOAD> injectPciBus;
    static injection::extension::InjectPciDevice<TPAYLOAD> injectPciDevice;
#if defined SIMICS_5_API || defined SIMICS_6_API
    static injection::extension::InjectPciExpress<TPAYLOAD> injectPciExpress;
#endif
    static injection::extension::InjectPciUpstreamOperation<TPAYLOAD>
        injectPciUpstreamOperation;
    static injection::extension::InjectSerialDevice<TPAYLOAD>
        injectSerialDevice;
}

template <typename TYPES>
void registerFactoryType() {
#if INTC_EXT
    Adapter::spy_factory_registry()->createSpyFactory<TYPES>();

    ProxyFactoryRegistry *registry
        = Adapter::proxy_builder()->proxy_factory_registry();
    registry->addFactory(new ProxyFactoryTargetSocket<TYPES>);
    registry->addFactory(new ProxyFactoryInitiatorSocket<TYPES>);
#endif
}

template <unsigned int BUSWIDTH, typename TYPES>
static void registerSocketType() {
    registerInjectorType<typename TYPES::tlm_payload_type>();
    registerFactoryType<TYPES>();
    static simics2tlm::GasketFactory<BUSWIDTH, TYPES> simics2tlm_factory;
    static tlm2simics::GasketFactory<BUSWIDTH, TYPES> tlm2simics_factory;
    static simics2tlm::GasketFactory<BUSWIDTH, TYPES, 0>
        multi_simics2tlm_factory;
    static tlm2simics::GasketFactory<BUSWIDTH, TYPES, 0>
        multi_tlm2simics_factory;
}

}  // namespace awareness
}  // namespace systemc
}  // namespace simics

#endif
