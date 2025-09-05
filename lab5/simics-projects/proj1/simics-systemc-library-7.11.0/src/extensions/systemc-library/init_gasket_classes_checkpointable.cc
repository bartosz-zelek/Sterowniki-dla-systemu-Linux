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

#if INTC_EXT && USE_SIMICS_CHECKPOINTING

#include <simics/systemc/simics2systemc/signal_class.h>
#include <simics/systemc/systemc2simics/signal_class.h>

#include <string>
#include <algorithm>

namespace simics {
namespace systemc {

namespace composite {
    extern void initGasketClasses(const char *module_name);
}  // namespace composite

namespace simics2systemc {
static void initGasketClasses(const char *module_name) {
    std::string gasket_name = module_name;
    gasket_name += "_gasket_simics2systemc_Signal";
    std::replace(gasket_name.begin(), gasket_name.end(), '-', '_');
    SignalClassSerializable::registerGasketClass(gasket_name.c_str());
}
}  // namespace simics2systemc

namespace systemc2simics {
static void initGasketClasses(const char *module_name) {
    std::string gasket_name = module_name;
    gasket_name += "_gasket_systemc2simics_Signal";
    std::replace(gasket_name.begin(), gasket_name.end(), '-', '_');
    SignalClassSerializable::registerGasketClass(gasket_name.c_str());
}
}  // namespace systemc2simics

namespace simics2tlm {
    extern void initGasketClasses(const char *module_name);
}  // namespace simics2tlm

namespace tlm2simics {
    extern void initGasketClasses(const char *module_name);
}  // namespace tlm2simics

void initGasketClassesCheckpointable(const char *module_name) {
    composite::initGasketClasses(module_name);
    simics2systemc::initGasketClasses(module_name);
    systemc2simics::initGasketClasses(module_name);
    simics2tlm::initGasketClasses(module_name);
    tlm2simics::initGasketClasses(module_name);
}

}  // namespace systemc
}  // namespace simics
#endif
