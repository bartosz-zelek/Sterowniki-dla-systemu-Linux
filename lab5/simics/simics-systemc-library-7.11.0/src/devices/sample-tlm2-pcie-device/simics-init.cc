/*
  © 2024 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include "pcie-endpoint.h"

#include <simics/systemc/systemc_library.h>

#include <list>
#include <array>

namespace scl = simics::systemc;

class Adapter : public scl::Adapter,
                public scl::composite::PcieGasket<> {
  public:
    explicit Adapter(simics::ConfObjectRef o)
        : scl::Adapter(o),
          scl::composite::PcieGasket<>(this) {}

    static void init_class(simics::ConfClass *cls);

    void bindGaskets() override {
        connect(pcie_endpoint_.pointer());
    }
    void elaborate() override;

    std::array<uint16, 2> getVendorId() const;
    void setVendorId(const std::array<uint16, 2> &val);
    std::array<uint16, 2> getDeviceId() const;
    void setDeviceId(const std::array<uint16, 2> &val);
    std::array<uint16, 2> getCommand() const;
    void setCommand(const std::array<uint16, 2> &val);
    std::array<uint16, 2> getStatus() const;
    void setStatus(const std::array<uint16, 2> &val);
    std::array<uint32, 2> getVersion() const;
    void setVersion(const std::array<uint32, 2> &val);
    std::array<uint32_t, 14> getBaseAddress() const;
    void setBaseAddress(const std::array<uint32_t, 14> &val);
    void dmaTrigger(const std::list<physical_address_t> &val);

  private:
    scl::Device<PcieEndpoint> pcie_endpoint_;

    // Attribute value cache
    std::array<uint16, 2> vendor_id_ {0, 0};
    std::array<uint16, 2> device_id_ {0, 0};
    std::array<uint16, 2> command_ {0, 0};
    std::array<uint16, 2> status_ {0, 0};
    std::array<uint32, 2> version_ {0, 0};
    std::array<uint32, 14> base_address_ {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
};

// Attribute get/set methods
std::array<uint16, 2> Adapter::getVendorId() const {
    return {pcie_endpoint_->vendor_id(0), pcie_endpoint_->vendor_id(1)};
}

void Adapter::setVendorId(const std::array<uint16, 2> &val) {
    vendor_id_ = val;
    if (SIM_object_is_configured(obj())) {
        pcie_endpoint_->set_vendor_id(0, val[0]);
        pcie_endpoint_->set_vendor_id(1, val[1]);
    }
}

std::array<uint16, 2> Adapter::getDeviceId() const {
    return {pcie_endpoint_->device_id(0), pcie_endpoint_->device_id(1)};
}

void Adapter::setDeviceId(const std::array<uint16, 2> &val) {
    device_id_ = val;
    if (SIM_object_is_configured(obj())) {
        pcie_endpoint_->set_device_id(0, val[0]);
        pcie_endpoint_->set_device_id(1, val[1]);
    }
}

std::array<uint16, 2> Adapter::getCommand() const {
    return {pcie_endpoint_->command(0), pcie_endpoint_->command(1)};
}

void Adapter::setCommand(const std::array<uint16, 2> &val) {
    command_ = val;
    if (SIM_object_is_configured(obj())) {
        pcie_endpoint_->set_command(0, val[0]);
        pcie_endpoint_->set_command(1, val[1]);
    }
}

std::array<uint16, 2> Adapter::getStatus() const {
    return {pcie_endpoint_->status(0), pcie_endpoint_->status(1)};
}

void Adapter::setStatus(const std::array<uint16, 2> &val) {
    status_ = val;
    if (SIM_object_is_configured(obj())) {
        pcie_endpoint_->set_status(0, val[0]);
        pcie_endpoint_->set_status(1, val[1]);
    }
}

std::array<uint32, 2> Adapter::getVersion() const {
    return {pcie_endpoint_->version(0), pcie_endpoint_->version(1)};
}

void Adapter::setVersion(const std::array<uint32, 2> &val) {
    version_ = val;
    if (SIM_object_is_configured(obj())) {
        pcie_endpoint_->set_version(0, val[0]);
        pcie_endpoint_->set_version(1, val[1]);
    }
}

std::array<uint32_t, 14> Adapter::getBaseAddress() const {
    return {pcie_endpoint_->base_address(0, 0),
            pcie_endpoint_->base_address(0, 1),
            pcie_endpoint_->base_address(0, 2),
            pcie_endpoint_->base_address(0, 3),
            pcie_endpoint_->base_address(0, 4),
            pcie_endpoint_->base_address(0, 5),
            pcie_endpoint_->base_address(0, 6),
            pcie_endpoint_->base_address(1, 0),
            pcie_endpoint_->base_address(1, 1),
            pcie_endpoint_->base_address(1, 2),
            pcie_endpoint_->base_address(1, 3),
            pcie_endpoint_->base_address(1, 4),
            pcie_endpoint_->base_address(1, 5),
            pcie_endpoint_->base_address(1, 6)};
}
void Adapter::setBaseAddress(const std::array<uint32_t, 14> &val) {
    base_address_ = val;
    if (SIM_object_is_configured(obj())) {
        for (int i = 0; i < 14; ++i) {
            pcie_endpoint_->set_base_address(i / 7, i % 7, val[i]);
        }
    }
}

void Adapter::dmaTrigger(const std::list<physical_address_t> &val) {
    pcie_endpoint_->dmaTrigger(val);
}

void Adapter::elaborate() {
    // Because we create the Device in elaborate, any attribute setter that
    // use it must be guarded against early access (i.e. during checkpoint
    // restore)
    scl::Device<PcieEndpoint> d(this, "pcie_endpoint");
    pcie_endpoint_ = d;

    // Handle attribute side-effects here, default to model defaults
    for (int i = 0; i < 2; ++i) {
        if (vendor_id_[i]) {
            pcie_endpoint_->set_vendor_id(i, vendor_id_[i]);
        }
        if (device_id_[i]) {
            pcie_endpoint_->set_device_id(i, device_id_[i]);
        }
        if (command_[i]) {
            pcie_endpoint_->set_command(i, command_[i]);
        }
        if (status_[i]) {
            pcie_endpoint_->set_status(i, status_[i]);
        }
        if (version_[i]) {
            pcie_endpoint_->set_version(i, version_[i]);
        }
    }
    for (int i = 0; i < 14; ++i) {
        if (base_address_[i]) {
            pcie_endpoint_->set_base_address(i / 7, i % 7, base_address_[i]);
        }
    }
}

void Adapter::init_class(simics::ConfClass *cls) {
    cls->add(simics::Attribute("vendor_id", "[ii]",
                               "The value of vendor id register.",
                               ATTR_GETTER(Adapter, getVendorId),
                               ATTR_SETTER(Adapter, setVendorId)));
    cls->add(simics::Attribute("device_id", "[ii]",
                               "The value of device id register.",
                               ATTR_GETTER(Adapter, getDeviceId),
                               ATTR_SETTER(Adapter, setDeviceId)));
    cls->add(simics::Attribute("command", "[ii]",
                               "The value of command register.",
                               ATTR_GETTER(Adapter, getCommand),
                               ATTR_SETTER(Adapter, setCommand)));
    cls->add(simics::Attribute("status", "[ii]",
                               "The value of status register.",
                               ATTR_GETTER(Adapter, getStatus),
                               ATTR_SETTER(Adapter, setStatus)));
    cls->add(simics::Attribute("version", "[ii]",
                               "The value of (MMIO) version register.",
                               ATTR_GETTER(Adapter, getVersion),
                               ATTR_SETTER(Adapter, setVersion)));
    cls->add(simics::Attribute("base_address", "[i{14}]",
                               "The value of (MMIO) base_address register.",
                               ATTR_GETTER(Adapter, getBaseAddress),
                               ATTR_SETTER(Adapter, setBaseAddress)));
    cls->add(simics::Attribute("dma_trigger", "[iii]",
                               "read size bytes from src address in pci-bus"
                               " memory space and write it back to dst address."
                               " Attribute format: [src, dst, size]",
                               nullptr,
                               ATTR_SETTER(Adapter, dmaTrigger),
                               Sim_Attr_Pseudo));
}

extern "C" void init_local(void) {
    simics::make_class<Adapter>(
        "sample_tlm2_pcie_device",
        "sample SystemC TLM2 PCIe device",
        "The <class>sample_tlm2_pcie_device</class> is a Simics module"
        " encapsulating a SystemC TLM2-based PCI device to demonstrate"
        " the use of Simics SystemC Library.");
}
