// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2022 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_HIERARCHICAL_OBJECT_H
#define SIMICS_HIERARCHICAL_OBJECT_H

#include <cstdint>
#include <string>
#include <string_view>

#include "simics/conf-object.h"  // ConfObjectRef
#include "simics/hierarchical-object-interface.h"
#include "simics/mappable-conf-object.h"

namespace simics {

class FieldInterface;
class RegisterInterface;
class BankInterface;

/// The hierarchy separator between bank, register and field names
constexpr static const char &SEPARATOR = '.';

/**
 * @brief Base class for Bank, Register, and Field classes.
 *
 * The HierarchicalObject class serves as a base class for objects used in
 * bank register modeling. A register bank contains registers, and each
 * register can contain fields. This class provides common functionality
 * and interfaces for these hierarchical objects.
 */
// coverity[rule_of_five_violation:FALSE]
class HierarchicalObject : public HierarchicalObjectInterface {
  public:
    /**
     * @brief Constructor for HierarchicalObject.
     * @param dev_obj Pointer to the device object.
     * @param name Unique hierarchical name of a bank/register/field.
     */
    HierarchicalObject(MappableConfObject *dev_obj, const std::string &name);

    /**
     * @brief Constructor for HierarchicalObject with FieldInterface.
     * @param dev_obj Pointer to the device object.
     * @param name Unique hierarchical name of a bank/register/field.
     * @param iface Pointer to the FieldInterface.
     */
    HierarchicalObject(MappableConfObject *dev_obj,
                       const std::string &name,
                       FieldInterface *iface);

    /**
     * @brief Constructor for HierarchicalObject with RegisterInterface.
     * @param dev_obj Pointer to the device object.
     * @param name Unique hierarchical name of a bank/register/field.
     * @param iface Pointer to the RegisterInterface.
     */
    HierarchicalObject(MappableConfObject *dev_obj,
                       const std::string &name,
                       RegisterInterface *iface);

    /**
     * @brief Constructor for HierarchicalObject with BankInterface.
     * @param dev_obj Pointer to the device object.
     * @param name Unique hierarchical name of a bank/register/field.
     * @param iface Pointer to the BankInterface.
     */
    HierarchicalObject(MappableConfObject *dev_obj,
                       const std::string &name,
                       BankInterface *iface);

    virtual ~HierarchicalObject();

    // No duplication
    HierarchicalObject(const HierarchicalObject&) = delete;
    HierarchicalObject& operator=(const HierarchicalObject&) = delete;

    HierarchicalObject(HierarchicalObject &&rhs) noexcept;
    HierarchicalObject& operator=(HierarchicalObject&& rhs) noexcept;

    /// @return If the hierarchical name is valid
    static bool is_valid_hierarchical_name(std::string_view name);

    /// @return The level of the hierarchical name
    /// throws an exception if level is greater than 2
    static std::string_view::size_type level_of_hierarchical_name(
            std::string_view name);

    /// Log group ID for Register_Read and Register_Write is fixed
    static const uint64_t Register_Read {1};
    static const uint64_t Register_Write {2};

    /// @return Pointer to the device object
    template<typename T> T *dev_ptr() {
        static_assert(std::is_base_of<MappableConfObject, T>::value,
                      "T needs be a MappableConfObject");
        return dynamic_cast<T *>(dev_obj());
    }

    /**
     * @brief Initialize the interface with BankInterface.
     * @param iface Pointer to the BankInterface.
     */
    void init_iface(BankInterface *iface);

    /**
     * @brief Initialize the interface with RegisterInterface.
     * @param iface Pointer to the RegisterInterface.
     */
    void init_iface(RegisterInterface *iface);

    /**
     * @brief Initialize the interface with FieldInterface.
     * @param iface Pointer to the FieldInterface.
     */
    void init_iface(FieldInterface *iface);

    // see HierarchicalObjectInterface
    const std::string &hierarchical_name() const override {
        return hierarchical_name_;
    }

    std::string_view name() const override;

    const std::string &description() const override {
        return desc_;
    }

    void set_description(std::string_view desc) override {
        desc_ = desc;
    }

    Level hierarchy_level() const override {
        return level_;
    }

    std::string_view bank_name() const override;

    MappableConfObject *dev_obj() const override {
        return dev_obj_;
    }

    ConfObjectRef bank_obj_ref() const override {
        return bank_obj_ref_;
    }

    std::string_view parent_name() const override;

    FieldInterface *lookup_field(const std::string &name) const override;

    RegisterInterface *lookup_register(const std::string &name) const override;

    BankInterface *lookup_bank(const std::string &name) const override;

  private:
    void init();
    // Ensure that a bank port is created before the initialization of
    // HierarchicalObject.
    void ensureBankPortExists(const std::string &bank_name);

    MappableConfObject *dev_obj_ {nullptr};
    std::string hierarchical_name_;
    std::string desc_;
    ConfObjectRef bank_obj_ref_;
    Level level_;
};

}  // namespace simics

#endif
