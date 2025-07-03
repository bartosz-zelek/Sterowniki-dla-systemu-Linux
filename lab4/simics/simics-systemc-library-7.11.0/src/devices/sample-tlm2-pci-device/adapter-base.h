/*                                                              -*- C++ -*-

  © 2017 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SAMPLE_TLM2_PCI_DEVICE_ADAPTER_BASE_H
#define SAMPLE_TLM2_PCI_DEVICE_ADAPTER_BASE_H

#include <simics/systemc/systemc_library.h>
#include <simics/c++/devs/pci.h>

#include <list>
#include <string>

#include "pci-device.h"

namespace scl = simics::systemc;

class AdapterBase : public scl::Adapter,
                    public simics::iface::PciMultiFunctionDeviceInterface {
  public:
    explicit AdapterBase(simics::ConfObjectRef o)
        : scl::Adapter(o)
        , vendor_id_(0)
        , device_id_(0)
        , command_(0)
        , status_(0)
        , base_address_0_(0)
        , base_address_2_(0)
        , base_address_5_(0)
        , version_(0) {
    }

    void elaborate() {
        // Because we create the Device in elaborate, any attribute setter that
        // use it must be guarded against early access (i.e. during checkpoint
        // restore)
        scl::Device<PciDevice> d(this, "pci_device");
        pci_device_ = d;

        // Handle attribute side-effects here, default to model defaults
        if (vendor_id_)
            pci_device_->set_vendor_id(0, vendor_id_);
        if (device_id_)
            pci_device_->set_device_id(0, device_id_);
        if (command_)
            pci_device_->set_command(0, command_);
        if (status_)
            pci_device_->set_status(0, status_);
        if (base_address_0_)
            pci_device_->set_base_address(0, 0, base_address_0_);
        if (base_address_2_)
            pci_device_->set_base_address(0, 2, base_address_2_);
        if (base_address_5_)
            pci_device_->set_base_address(0, 5, base_address_5_);
        if (version_)
            pci_device_->set_version(0, version_);
    }

    // Attribute get/set methods
    uint16 getVendorId() const {
        return pci_device_->vendor_id(0);
    }
    void setVendorId(const uint16 &val) {
        vendor_id_ = val;
        if (SIM_object_is_configured(obj())) {
            pci_device_->set_vendor_id(0, val);
        }
    }
    uint16 getDeviceId() const {
        return pci_device_->device_id(0);
    }
    void setDeviceId(const uint16 &val) {
        device_id_ = val;
        if (SIM_object_is_configured(obj())) {
            pci_device_->set_device_id(0, val);
        }
    }
    uint16 getCommand() const {
        return pci_device_->command(0);
    }
    void setCommand(const uint16 &val) {
        command_ = val;
        if (SIM_object_is_configured(obj())) {
            pci_device_->set_command(0, val);
        }
    }
    uint16 getStatus() const {
        return pci_device_->status(0);
    }
    void setStatus(const uint16 &val) {
        status_ = val;
        if (SIM_object_is_configured(obj())) {
            pci_device_->set_status(0, val);
        }
    }
    uint32 getBaseAddress0() const {
        return pci_device_->base_address(0, 0);
    }
    void setBaseAddress0(const uint32 &val) {
        base_address_0_ = val;
        if (SIM_object_is_configured(obj())) {
            pci_device_->set_base_address(0, 0, val);
        }
    }
    uint32 getBaseAddress2() const {
        return pci_device_->base_address(0, 2);
    }
    void setBaseAddress2(const uint32 &val) {
        base_address_2_ = val;
        if (SIM_object_is_configured(obj())) {
            pci_device_->set_base_address(0, 2, val);
        }
    }
    uint32 getBaseAddress5() const {
        return pci_device_->base_address(0, 5);
    }
    void setBaseAddress5(const uint32 &val) {
        base_address_5_ = val;
        if (SIM_object_is_configured(obj())) {
            pci_device_->set_base_address(0, 5, val);
        }
    }
    uint32 getVersion() const {
        return pci_device_->version(0);
    }
    void setVersion(const uint32 &val) {
        version_ = val;
        if (SIM_object_is_configured(obj())) {
            pci_device_->set_version(0, val);
        }
    }
    void dmaTrigger(const std::list<physical_address_t> &val) {
        pci_device_->dmaTrigger(val);
    }

    static const char *getF0Name() {
        return "port.F0";
    }

    static const char *getF5Name() {
        return "port.F5";
    }

    // simics::iface::PciMultiFunctionDeviceInterface
    attr_value_t supported_functions() override {
        attr_value_t list = SIM_alloc_attr_list(2);
        conf_object_t *f0 = SIM_object_descendant(obj(), getF0Name());
        assert(f0);
        conf_object_t *f5 = SIM_object_descendant(obj(), getF5Name());
        assert(f5);
        SIM_attr_list_set_item(&list, 0,
                               SIM_make_attr_list(2,
                                                  SIM_make_attr_uint64(0),
                                                  SIM_make_attr_object(f0)));
        SIM_attr_list_set_item(&list, 1,
                               SIM_make_attr_list(2,
                                                  SIM_make_attr_uint64(5),
                                                  SIM_make_attr_object(f5)));
        return list;
    }

    static void initClass(simics::ConfClass *cls) {
        cls->add(simics::Attribute("vendor_id", "i",
                                   "The value of vendor id register.",
                                   ATTR_GETTER(AdapterBase, getVendorId),
                                   ATTR_SETTER(AdapterBase, setVendorId)));
        cls->add(simics::Attribute("device_id", "i",
                                   "The value of device id register.",
                                   ATTR_GETTER(AdapterBase, getDeviceId),
                                   ATTR_SETTER(AdapterBase, setDeviceId)));
        cls->add(simics::Attribute("command", "i",
                                   "The value of command register.",
                                   ATTR_GETTER(AdapterBase, getCommand),
                                   ATTR_SETTER(AdapterBase, setCommand)));
        cls->add(simics::Attribute("status", "i",
                                   "The value of status register.",
                                   ATTR_GETTER(AdapterBase, getStatus),
                                   ATTR_SETTER(AdapterBase, setStatus)));
        // Limitation: Only expose BARs for F0
        cls->add(simics::Attribute("base_address_0", "i",
                                   "The value of BAR0 base register.",
                                   ATTR_GETTER(AdapterBase, getBaseAddress0),
                                   ATTR_SETTER(AdapterBase, setBaseAddress0)));
        cls->add(simics::Attribute("base_address_2", "i",
                                   "The value of BAR2 base register.",
                                   ATTR_GETTER(AdapterBase, getBaseAddress2),
                                   ATTR_SETTER(AdapterBase, setBaseAddress2)));
        cls->add(simics::Attribute("base_address_5", "i",
                                   "The value of BAR5 base register.",
                                   ATTR_GETTER(AdapterBase, getBaseAddress5),
                                   ATTR_SETTER(AdapterBase, setBaseAddress5)));
        cls->add(simics::Attribute("version", "i",
                                   "The value of (MMIO) version register.",
                                   ATTR_GETTER(AdapterBase, getVersion),
                                   ATTR_SETTER(AdapterBase, setVersion)));
        cls->add(simics::iface::PciMultiFunctionDeviceInterface::Info());
    }

  protected:
    // Must use the Device helper class to wrap the sc_module
    scl::Device<PciDevice> pci_device_;

    // Attribute value cache
    uint16 vendor_id_;
    uint16 device_id_;
    uint16 command_;
    uint16 status_;
    uint32 base_address_0_;
    uint32 base_address_2_;
    uint32 base_address_5_;
    uint32 version_;
};

#endif
