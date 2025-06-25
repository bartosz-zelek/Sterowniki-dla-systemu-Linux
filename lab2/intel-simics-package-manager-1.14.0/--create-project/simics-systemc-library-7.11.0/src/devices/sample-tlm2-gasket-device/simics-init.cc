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

// Adapter base class (i.e. the Simics object), etc...
#include <simics/systemc/systemc_library.h>

// The SystemC TLM device being wrapped
#include "gasket-device.h"

// <add id="sample-tlm2-gasket-device/Adapter">
// <insert-until text="// EOF_GASKET_ADAPTER"/></add>
template<class TModel>
class Adapter : public simics::systemc::Adapter {
 public:
    explicit Adapter(simics::ConfObjectRef o)
        : simics::systemc::Adapter(o)
        , top_("top") {}

 private:
    TModel top_;
};
// EOF_GASKET_ADAPTER

#define CLASS_NAME "sample_tlm2_gasket_device"

static void initializeDevice(void) {
    simics::make_class<Adapter<GasketDevice>>(
        CLASS_NAME,
        "sample SystemC TLM2 device",
        "The <class>" CLASS_NAME "</class> is a Simics module"
        " encapsulating a SystemC TLM2-based simple device to demonstrate"
        " the use of the Simics SystemC Library.");
}

#define CLASS_NAME_DOC "sample_tlm2_gasket_device_doc_example"

static void initializeDeviceDoc(void) {
    simics::make_class<Adapter<Top>>(
        CLASS_NAME_DOC,
        "sample SystemC TLM2 device",
        "The <class>" CLASS_NAME_DOC "</class> is a Simics module"
        " encapsulating a SystemC TLM2-based simple device to demonstrate"
        " the use of the Simics SystemC Library.");
}

#define CLASS_NAME_DOC_VECTOR "sample_tlm2_gasket_device_doc_vector_example"

static void initializeDeviceDocVector(void) {
    simics::make_class<Adapter<TopVector>>(
        CLASS_NAME_DOC_VECTOR,
        "sample SystemC TLM2 gasket device",
        "The <class>" CLASS_NAME_DOC_VECTOR "</class> is a Simics module"
        " encapsulating a SystemC TLM2-based simple device to demonstrate"
        " the use of the Simics SystemC Library.");
}

extern "C" void init_local(void) {
    initializeDevice();
    initializeDeviceDoc();
    initializeDeviceDocVector();
}
