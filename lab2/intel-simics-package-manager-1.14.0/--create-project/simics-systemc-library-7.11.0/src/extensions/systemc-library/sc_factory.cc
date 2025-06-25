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

// The following two class_decorator inclusions must be before cc-api.h
#include <simics/systemc/class_decorator_include.h>
#include <simics/systemc/class_decorator.h>

#include <simics/cc-api.h>
#include <simics/systemc/adapter.h>
#include <simics/systemc/report_handler.h>

#include <simics/systemc/sc_factory.h>
#include <simics/device-api.h>  // Needed for init_local

using simics::systemc::RegisterModel;

namespace {
class SystemcModel : public simics::systemc::Adapter {
    typedef RegisterModel::FactoryInterface Factory;

  public:
    SystemcModel(simics::ConfObjectRef o, void *arg)
        : simics::systemc::Adapter(o) {
        factory_ = static_cast<Factory *>(arg);
    }

    void elaborate() {
        factory_->setup(sc_core::sc_argc(),
                        const_cast<char **>(sc_core::sc_argv()));
    }

    ~SystemcModel() {
        simics::systemc::ReportHandler rh(obj());
        factory_->teardown();
    }

  private:
    Factory *factory_;
};
}  // namespace

extern "C" void
init_local(void) {
    RegisterModel::registerWithFramework<SystemcModel,
        RegisterModel::ModelData<RegisterModel> >();
}
