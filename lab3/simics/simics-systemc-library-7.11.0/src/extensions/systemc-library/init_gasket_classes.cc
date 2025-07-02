/*
  © 2017 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <simics/systemc/iface/iface.h>
#if !defined(SIMICS_6_API)
#define SIMICS_6_API
#include <simics/systemc/composite/pci_gasket_class.h>
#undef SIMICS_6_API
#endif
#include <simics/systemc/composite/pcie_gasket_class.h>
#include <simics/systemc/simics2systemc/signal_class.h>
#include <simics/systemc/simics2tlm/gasket_class.h>
#include <simics/systemc/simics2tlm/simics2tlm.h>
#include <simics/systemc/systemc2simics/signal_class.h>
#include <simics/systemc/tlm2simics/gasket_class.h>
#include <simics/systemc/tlm2simics/tlm2simics.h>

#include <string>
#include <algorithm>

#define REGISTER_SIMICS2TLM_GASKET(gasket)                                   \
{                                                                            \
    std::string gasket_name = module_name;                                   \
    gasket_name += "_gasket_simics2tlm_"#gasket;                             \
    std::replace(gasket_name.begin(), gasket_name.end(), '-', '_');          \
    GasketClass<gasket, gasket##GasketAdapter, iface::gasket##SimicsAdapter, \
                iface::gasket##Interface>::registerGasketClass(              \
        gasket_name.c_str());                                                \
}

#define REGISTER_TLM2SIMICS_GASKET(gasket)                         \
{                                                                  \
    std::string gasket_name = module_name;                         \
    gasket_name += "_gasket_tlm2simics_"#gasket;                   \
    std::replace(gasket_name.begin(), gasket_name.end(), '-', '_');\
    GasketClass<gasket>::registerGasketClass(gasket_name.c_str()); \
}

namespace simics {
namespace systemc {

namespace composite {
void initGasketClasses(const char *module_name) {
    std::string pci_gasket_name = module_name;
    pci_gasket_name += "_gasket_composite_Pci";
    std::replace(pci_gasket_name.begin(), pci_gasket_name.end(), '-', '_');
    PciGasketClass<>::registerGasketClass(pci_gasket_name.c_str());

    std::string pcie_gasket_name = module_name;
    pcie_gasket_name += "_gasket_composite_Pcie";
    std::replace(pcie_gasket_name.begin(), pcie_gasket_name.end(), '-', '_');
    PcieGasketClass<>::registerGasketClass(pcie_gasket_name.c_str());
}
}  // namespace composite

namespace simics2systemc {
static void initGasketClasses(const char *module_name) {
    std::string gasket_name = module_name;
    gasket_name += "_gasket_simics2systemc_Signal";
    std::replace(gasket_name.begin(), gasket_name.end(), '-', '_');
    SignalClassNonSerializable::registerGasketClass(gasket_name.c_str());
}
}  // namespace simics2systemc

namespace systemc2simics {
static void initGasketClasses(const char *module_name) {
    std::string gasket_name = module_name;
    gasket_name += "_gasket_systemc2simics_Signal";
    std::replace(gasket_name.begin(), gasket_name.end(), '-', '_');
    SignalClassNonSerializable::registerGasketClass(gasket_name.c_str());
}
}  // namespace systemc2simics

namespace simics2tlm {
void initGasketClasses(const char *module_name) {
    REGISTER_SIMICS2TLM_GASKET(EthernetCommon);
    REGISTER_SIMICS2TLM_GASKET(Packet);
    REGISTER_SIMICS2TLM_GASKET(I2cSlaveV2);
    REGISTER_SIMICS2TLM_GASKET(IoMemory);
    REGISTER_SIMICS2TLM_GASKET(PciDevice);
    REGISTER_SIMICS2TLM_GASKET(PciExpress);
    REGISTER_SIMICS2TLM_GASKET(SerialDevice);
}
}  // namespace simics2tlm

namespace tlm2simics {
void initGasketClasses(const char *module_name) {
    REGISTER_TLM2SIMICS_GASKET(EthernetCommon);
    REGISTER_TLM2SIMICS_GASKET(Packet);
    REGISTER_TLM2SIMICS_GASKET(I2cMasterV2);
    REGISTER_TLM2SIMICS_GASKET(MemorySpace);
    REGISTER_TLM2SIMICS_GASKET(PciBus);
    REGISTER_TLM2SIMICS_GASKET(SerialDevice);
}
}  // namespace tlm2simics

void initGasketClasses(const char *module_name) {
    composite::initGasketClasses(module_name);
    simics2systemc::initGasketClasses(module_name);
    systemc2simics::initGasketClasses(module_name);
    simics2tlm::initGasketClasses(module_name);
    tlm2simics::initGasketClasses(module_name);
}

}  // namespace systemc
}  // namespace simics
