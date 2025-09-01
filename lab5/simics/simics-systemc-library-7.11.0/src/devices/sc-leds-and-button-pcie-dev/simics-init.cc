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

#include <simics/systemc/systemc_library.h>

#include "pci-device.h"

#include <list>

namespace scl = simics::systemc;

class Adapter : public scl::Adapter,
                public scl::composite::PciGasket<>,
                public scl::simics2systemc::SignalGasketAdapter {
  public:
    explicit Adapter(simics::ConfObjectRef o)
        : scl::Adapter(o),
        scl::composite::PciGasket<>(this),
        scl::simics2systemc::SignalGasketAdapter(&systemc_dma_, this),
        pci_device_(this, "pci_device") {
        connect(pci_device_.pointer());
        simics_pin_out_0_->set_pin(pci_device_->pin_out[0]);
        simics_pin_out_1_->set_pin(pci_device_->pin_out[1]);
        simics_pin_out_2_->set_pin(pci_device_->pin_out[2]);
        simics_pin_out_3_->set_pin(pci_device_->pin_out[3]);
        simics_system_onoff_led_->set_pin(&pci_device_->system_onoff_led_out);
        systemc_dma_.set_pin(&pci_device_->dma_button, false, simics_object());
    }

    static void init_class(simics::ConfClass *cls);

    // Attribute get/set methods
    uint16 getVendorId() const {
        return pci_device_->vendor_id(0);
    }
    void setVendorId(const uint16 &val) {
        pci_device_->set_vendor_id(0, val);
    }
    uint16 getDeviceId() const {
        return pci_device_->device_id(0);
    }
    void setDeviceId(const uint16 &val) {
        pci_device_->set_device_id(0, val);
    }
    uint16 getCommand() const {
        return pci_device_->command(0);
    }
    void setCommand(const uint16 &val) {
        pci_device_->set_command(0, val);
    }
    uint16 getStatus() const {
        return pci_device_->status(0);
    }
    void setStatus(const uint16 &val) {
        pci_device_->set_status(0, val);
    }
    uint32 getBaseAddress0() const {
        return pci_device_->base_address(0, 0);
    }
    void setBaseAddress0(const uint32 &val) {
        pci_device_->set_base_address(0, 0, val);
    }
    uint32 getBaseAddress1() const {
        return pci_device_->base_address(0, 1);
    }
    void setBaseAddress1(const uint32 &val) {
        pci_device_->set_base_address(0, 1, val);
    }
    uint32 getBaseAddress2() const {
        return pci_device_->base_address(0, 2);
    }
    void setBaseAddress2(const uint32 &val) {
        pci_device_->set_base_address(0, 2, val);
    }
    uint32 getVersion() const {
        return pci_device_->version(0);
    }
    void setVersion(const uint32 &val) {
        pci_device_->set_version(0, val);
    }
    bool getSystemOnOffLedVal() const {
        return pci_device_->system_onoff_led();
    }
    void setSystemOnOffLedVal(const bool &val) {
        pci_device_->set_system_onoff_led(val);
    }
    uint32 getSystemOnOffLedCountVal() const {
        return pci_device_->system_onoff_led_count();
    }
    void setSystemOnOffLedCountVal(const uint32 &val) {
        pci_device_->set_system_onoff_led_count(val);
    }
    std::list<bool> getPinOutVals() const {
        return pci_device_->pin_out_vals();
    }
    void setPinOutVals(const std::list<bool> &state) {
        pci_device_->set_pin_out_vals(state);
    }

    scl::Connector<scl::systemc2simics::Signal> simics_pin_out_0_;
    scl::Connector<scl::systemc2simics::Signal> simics_pin_out_1_;
    scl::Connector<scl::systemc2simics::Signal> simics_pin_out_2_;
    scl::Connector<scl::systemc2simics::Signal> simics_pin_out_3_;
    scl::Connector<scl::systemc2simics::Signal> simics_system_onoff_led_;

    class Port : public simics::Port<Adapter>,
                 public scl::simics2systemc::SignalGasketAdapter {
      public:
        explicit Port(simics::ConfObjectRef o)
            : simics::Port<Adapter>(o),
              SignalGasketAdapter(&parent()->systemc_dma_, parent()) {}
    };

  private:
    scl::simics2systemc::Signal systemc_dma_;

    // Must use the Device helper class to wrap the sc_module
    scl::Device<PciDevice> pci_device_;
};

#define CLASS_NAME "sc_leds_and_button_pcie_device"

void
Adapter::init_class(simics::ConfClass *cls) {
    cls->add(simics::Attribute("vendor_id", "i",
                               "The value of vendor id register.",
                               ATTR_GETTER(Adapter, getVendorId),
                               ATTR_SETTER(Adapter, setVendorId)));
    cls->add(simics::Attribute("device_id", "i",
                               "The value of device id register.",
                               ATTR_GETTER(Adapter, getDeviceId),
                               ATTR_SETTER(Adapter, setDeviceId)));
    cls->add(simics::Attribute("command", "i",
                               "The value of command register.",
                               ATTR_GETTER(Adapter, getCommand),
                               ATTR_SETTER(Adapter, setCommand)));
    cls->add(simics::Attribute("status", "i",
                               "The value of status register.",
                               ATTR_GETTER(Adapter, getStatus),
                               ATTR_SETTER(Adapter, setStatus)));
    // TODO(ah): Expose _all_ BARs for _all_ functions
    cls->add(simics::Attribute("base_address_0", "i",
                               "The value of BAR0 base address.",
                               ATTR_GETTER(Adapter, getBaseAddress0),
                               ATTR_SETTER(Adapter, setBaseAddress0)));
    cls->add(simics::Attribute("base_address_1", "i",
                               "The value of BAR1 base address.",
                               ATTR_GETTER(Adapter, getBaseAddress1),
                               ATTR_SETTER(Adapter, setBaseAddress1)));
    cls->add(simics::Attribute("base_address_2", "i",
                               "The value of BAR2 base address.",
                               ATTR_GETTER(Adapter, getBaseAddress2),
                               ATTR_SETTER(Adapter, setBaseAddress2)));
    cls->add(simics::Attribute("version", "i",
                               "The value of (MMIO) version register.",
                               ATTR_GETTER(Adapter, getVersion),
                               ATTR_SETTER(Adapter, setVersion)));
    cls->add(simics::Attribute("system_onoff_led_value", "b",
                               "boolean value of device's sc_out"
                               " system_onoff_led pin",
                               ATTR_GETTER(Adapter, getSystemOnOffLedVal),
                               ATTR_SETTER(Adapter, setSystemOnOffLedVal)));
    cls->add(simics::Attribute("system_onoff_led_count_value", "i",
                               "value of device's sc_out"
                               " system_onoff_led_count",
                               ATTR_GETTER(Adapter, getSystemOnOffLedCountVal),
                               ATTR_SETTER(Adapter,
                                           setSystemOnOffLedCountVal)));
    cls->add(simics::Attribute("pin_out_values", "[bbbb]",
                               "Values of all sc_out pin_out pins",
                               ATTR_GETTER(Adapter, getPinOutVals),
                               ATTR_SETTER(Adapter, setPinOutVals)));
    cls->add(simics::Attribute("pin_out_0", "o|n", "Pin 0 LED.",
                               ATTR_CLS_VAR(Adapter, simics_pin_out_0_)));
    cls->add(simics::Attribute("pin_out_1", "o|n", "Pin 1 LED.",
                               ATTR_CLS_VAR(Adapter, simics_pin_out_1_)));
    cls->add(simics::Attribute("pin_out_2", "o|n", "Pin 2 LED.",
                               ATTR_CLS_VAR(Adapter, simics_pin_out_2_)));
    cls->add(simics::Attribute("pin_out_3", "o|n", "Pin 3 LED.",
                               ATTR_CLS_VAR(Adapter, simics_pin_out_3_)));
    cls->add(simics::Attribute("system_onoff_led", "o|n",
                               "System On/Off LED.",
                               ATTR_CLS_VAR(Adapter,
                                            simics_system_onoff_led_)));

    auto port = simics::make_class<Adapter::Port>(
            CLASS_NAME ".port", CLASS_NAME " port", CLASS_NAME " port");
    port->add(scl::iface::createAdapter<
              scl::iface::SignalSimicsAdapter<Adapter::Port> >());
    cls->add(port, "port.dma");
}

extern "C" void init_local(void) {
    simics::make_class<Adapter>(
        CLASS_NAME,
        "simple SystemC LEDs and button PCIE unit",
        "The <class>" CLASS_NAME "</class> is a Simics module"
        " encapsulating a SystemC TLM2-based PCI device to demonstrate"
        " the use of Simics SystemC Library.");
}
