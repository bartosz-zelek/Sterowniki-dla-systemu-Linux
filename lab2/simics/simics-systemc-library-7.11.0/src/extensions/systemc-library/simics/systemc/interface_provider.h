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

#ifndef SIMICS_SYSTEMC_INTERFACE_PROVIDER_H
#define SIMICS_SYSTEMC_INTERFACE_PROVIDER_H

#include <simics/conf-object.h>
#include <simics/systemc/simics_target_lock.h>

#include <set>

namespace simics {
namespace systemc {

/**
 * Returns the interface provided by the associated Simics object. This class
 * is typically used with tlm2simics gaskets and connector attributes, making
 * sure that the Simics object implements the requested interface.
 */
class InterfaceProvider {
  public:
    class TargetUpdateListener {
      public:
        virtual ~TargetUpdateListener() {}

        virtual void update_target(ConfObjectRef old_target,
                                   ConfObjectRef new_target) = 0;
    };

    explicit InterfaceProvider(const char *interface_name)
        : interface_(NULL), interface_name_(interface_name), optional_(false),
          target_update_listeners_(new std::set<TargetUpdateListener*>()) {
    }
    InterfaceProvider(const InterfaceProvider&) = delete;
    InterfaceProvider& operator=(const InterfaceProvider&) = delete;
    virtual ~InterfaceProvider() {
        // NOTE: The set cannot be owned by the class, it must be created and
        // destroyed by the CTOR/DTOR; or else the DmiTransactionHandler could
        // end up accessing an invalid set when invoking
        // remove_target_update_listener() from its DTOR
        delete target_update_listeners_;
        target_update_listeners_ = NULL;
    }

    // Interface for derived class to define how to set/use simics interface
    virtual void set_target(const ConfObjectRef &obj) {
        for (std::set<TargetUpdateListener*>::iterator it
                 = target_update_listeners_->begin();
             it != target_update_listeners_->end(); ++it) {
            (*it)->update_target(target_obj_, obj);
        }
        target_obj_ = simics::ConfObjectRef(obj);
        interface_ = NULL;
    }

    virtual void add_target_update_listener(TargetUpdateListener *l) {
        if (l) {
            target_update_listeners_->insert(l);
        }
    }

    virtual void remove_target_update_listener(TargetUpdateListener *l) {
        // NOTE: could be called from a DTOR, must check for valid set
        if (target_update_listeners_) {
            target_update_listeners_->erase(l);
        }
    }

    virtual const simics::ConfObjectRef &target() const {
        return target_obj_;
    }

    virtual bool has_interface() {
        return get_interface() != NULL;
    }

    virtual const char *get_interface_name() const {
        return interface_name_;
    }

    template<typename TInterface>
    SimicsTargetLock<TInterface> get_interface() {
        interface_t *iface = const_cast<interface_t *>(get_interface());
        return SimicsTargetLock<TInterface>(target_obj_.object(),
                                            static_cast<TInterface*>(iface));
    }

    virtual void set_optional(bool optional) {
        optional_ = optional;
    }
    virtual bool optional() const {
        return optional_;
    }

  protected:
    virtual const interface_t *get_interface() {
        if (interface_ == NULL) {
            interface_ = target_obj_ ?
                target_obj_.get_interface(interface_name_) : NULL;
        }
        return interface_;
    }

    simics::ConfObjectRef target_obj_;
    const interface_t *interface_;
    const char *interface_name_;
    bool optional_;
    std::set<TargetUpdateListener*> *target_update_listeners_;
};

class InterfaceProviderAddOn : public InterfaceProvider,
                               public InterfaceProvider::TargetUpdateListener {
  public:
    InterfaceProviderAddOn(InterfaceProvider *parent,
                           const char *interface_name)
        : InterfaceProvider(interface_name), parent_(parent) {
        target_obj_ = parent_->target();
        parent_->add_target_update_listener(this);
    }
    ~InterfaceProviderAddOn() {
        parent_->remove_target_update_listener(this);
    }

    virtual void set_target(const ConfObjectRef &obj) {
        parent_->set_target(obj);
    }

    virtual void set_optional(bool optional) {
        parent_->set_optional(optional);
    }
    virtual bool optional() const {
        return parent_->optional();
    }

  protected:
    virtual void update_target(ConfObjectRef old_target,
                               ConfObjectRef new_target) {
        InterfaceProvider::set_target(new_target);
    }

    InterfaceProvider *parent_;
};

}  // namespace systemc
}  // namespace simics

#endif
