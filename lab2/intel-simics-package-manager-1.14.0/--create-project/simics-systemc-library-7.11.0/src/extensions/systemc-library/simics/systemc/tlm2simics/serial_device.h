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

#ifndef SIMICS_SYSTEMC_TLM2SIMICS_SERIAL_DEVICE_H
#define SIMICS_SYSTEMC_TLM2SIMICS_SERIAL_DEVICE_H

#include <simics/systemc/iface/receiver_interface.h>
#include <simics/systemc/iface/serial_device_extension.h>
#include <simics/systemc/iface/serial_device_interface.h>
#include <simics/systemc/interface_provider.h>
#include <simics/systemc/tlm2simics/transaction_handler.h>

namespace simics {
namespace systemc {
namespace tlm2simics {

/**
 * Class that translates a TLM transaction with protocol specific
 * SerialDeviceExtension to the Simics C++ SerialDeviceInterface.
 */
//:: pre tlm2simics_SerialDevice {{
class SerialDevice : public InterfaceProvider,
                     public TransactionHandler,
                     public iface::SerialDeviceInterface {
  public:
    SerialDevice() : InterfaceProvider("serial_device"),
                     TransactionHandler(this,
                         iface::SerialDeviceExtension::createIgnoreReceiver()),
                     receiver_(
                         iface::SerialDeviceExtension::createReceiver(this)) {}

    // SerialDeviceInterface
    int write(int value) override;
    void receive_ready() override;
    // TransactionHandler
    iface::ReceiverInterface *receiver() override;

    virtual ~SerialDevice();

  private:
    tlm::tlm_response_status simics_transaction(
            ConfObjectRef &simics_obj,
            tlm::tlm_generic_payload *trans) override;
    iface::ReceiverInterface *receiver_;
};
// }}

}  // namespace tlm2simics
}  // namespace systemc
}  // namespace simics

#endif
