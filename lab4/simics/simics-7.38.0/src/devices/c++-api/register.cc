// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2025 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include "simics/register.h"

#include <fmt/fmt/format.h>
#include <simics/base/conf-object.h>  // SIM_object_class
#include <simics/base/notifier.h>  // SIM_notify
#include <simics/simulator/conf-object.h>  // SIM_class_has_attribute

#include <algorithm>
#include <bitset>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <limits>
#include <set>
#include <string>
#include <string_view>
#include <utility>  // move
#include <vector>

#include "simics/attribute-traits.h"
#include "simics/bank-interface.h"
#include "simics/field.h"
#include "simics/field-interface.h"
#include "simics/log.h"
#include "simics/mappable-conf-object.h"
#include "simics/utility.h"  // overlap_range

namespace simics {

// @return the first bit of one found from bit 0 of bits
static unsigned find_first(const std::bitset<64> &bits) {
    unsigned idx = 0;
    while (idx < 64 && !bits[idx]) {
      ++idx;
    }
    return idx;
}

Register::Register(MappableConfObject *dev_obj, const std::string &name)
    : HierarchicalObject(dev_obj, name, this) {}

Register::Register(BankInterface *parent, std::string_view reg_name)
    : HierarchicalObject(
            parent->dev_obj(),
            std::string(parent->name()) + SEPARATOR + reg_name.data(),
            this),
      parent_(parent) {}

Register::Register(Register &&rhs)
    : HierarchicalObject(std::move(rhs)),
    init_val_(rhs.init_val_),
    byte_mask_(rhs.byte_mask_),
    byte_pointers_(std::move(rhs.byte_pointers_)),
    fields_(std::move(rhs.fields_)),
    allocated_fields_(std::move(rhs.allocated_fields_)) {
    HierarchicalObject::init_iface(this);
}

Register& Register::operator=(Register&& rhs) {
    // check for self-assignment
    if (this == &rhs)
        return *this;

    HierarchicalObject::operator=(std::move(rhs));
    HierarchicalObject::init_iface(this);
    init_val_ = rhs.init_val_;
    byte_mask_ = rhs.byte_mask_;
    byte_pointers_ = std::move(rhs.byte_pointers_);
    fields_ = std::move(rhs.fields_);
    allocated_fields_ = std::move(rhs.allocated_fields_);
    return *this;
}

std::ostream& operator<< (std::ostream& stream, const Register& reg) {
    // Save the original formatting state
    std::ios old_state(nullptr);
    old_state.copyfmt(stream);

    stream << "0x" << std::setfill('0')
           << std::setw(static_cast<int>(reg.number_of_bytes() * 2))
           << std::hex << reg.get();

    // Restore the original formatting state
    stream.copyfmt(old_state);

    return stream;
}

std::size_t Register::offset(const RegisterInterface *reg) {
    for (const auto &[offset_, iface] : reg->parent()->mapped_registers()) {
        if (reg == iface) {
            return offset_;
        }
    }
    // Unable to find the register
    return std::numeric_limits<std::size_t>::max();
}

std::string_view Register::name() const {
    return HierarchicalObject::name();
}

const std::string &Register::hierarchical_name() const {
    return HierarchicalObject::hierarchical_name();
}

const std::string &Register::description() const {
    return HierarchicalObject::description();
}

MappableConfObject *Register::dev_obj() const {
    return HierarchicalObject::dev_obj();
}

ConfObjectRef Register::bank_obj_ref() const {
    return HierarchicalObject::bank_obj_ref();
}

unsigned Register::number_of_bytes() const {
    auto size = byte_pointers_.size();
    if (size > (std::numeric_limits<unsigned int>::max)()) {
        SIM_LOG_INFO(2, bank_obj_ref(), 0,
                     "The number of bytes exceeds the maximum"
                     " supported value (0x%x)",
                     (std::numeric_limits<unsigned int>::max)());
        return (std::numeric_limits<unsigned int>::max)();
    }
    return static_cast<unsigned>(size);
}

void Register::init(std::string_view desc, unsigned number_of_bytes,
                    uint64_t init_val) {
    set_description(desc);
    check_number_of_bytes(number_of_bytes);
    set_init_value(init_val);
    set(init_val_);
    if (parent_ == nullptr) {
        parent_ = dev_obj()->get_iface<BankInterface>(
                std::string(parent_name()));
    }
    // Add attribute for both mapped and unmapped registers
    add_register_as_simics_attribute(this);
}

void Register::reset() {
    set(init_val_);
}

bool Register::is_read_only() const {
    return false;
}

bool Register::is_mapped() const {
    return true;
}

void Register::set_byte_pointers(const register_memory_t &byte_pointers) {
    // Since no duplication check for register names and this function
    // is the first one called when adding a register, the duplication
    // will be detected here
    if (!byte_pointers_.empty()) {
        SIM_LOG_ERROR_STR(
                bank_obj_ref(), 0,
                fmt::format(
                        "Multiple calls to Register::set_byte_pointers()"
                        " detected. Make sure register name ({}) is not"
                        " duplicated within the same bank",
                        hierarchical_name()));
        return;
    }
    if (byte_pointers.size() > 8 || byte_pointers.empty()) {
        SIM_LOG_ERROR(
                bank_obj_ref(), 0,
                "The supported register size is [1-8] bytes");
        return;
    }

    std::set<register_memory_t::value_type> unique_pointers(
            byte_pointers.begin(), byte_pointers.end());
    if (byte_pointers.size() != unique_pointers.size()) {
        SIM_LOG_ERROR(
                bank_obj_ref(), 0,
                "The byte_pointers contains duplicate items");
        return;
    }

    // Shallow copy is OK since the pointers are intended to be shared
    byte_pointers_ = byte_pointers;

    byte_mask_ = (std::numeric_limits<uint64_t>::max)();
    byte_mask_ >>= (8 - number_of_bytes()) * 8;
}

uint64_t Register::get() const {
    uint64_t value = read_from_byte_pointers();
    if (fields_.empty()) {
        return value;
    }

    auto num_bytes = number_of_bytes();
    // Pass to field.get
    for (const auto &[offset, f] : fields_) {
        size_t bits_mask = byte_mask_;
        auto number_of_bits = f->number_of_bits();
        if (number_of_bits != num_bytes * 8) {
            bits_mask = (1ULL << number_of_bits) - 1;
        }
        value &= ~(bits_mask << offset);
        value |= (f->get() & bits_mask) << offset;
    }
    return value;
}

void Register::set(uint64_t value) {
    auto v = value & byte_mask_;
    bool changed = false;
    for (const auto byte_pointer : byte_pointers_) {
        if (*byte_pointer != (v & 0xff)) {
            changed = true;
            *byte_pointer = v & 0xff;
        }
        v = v >> 8;
    }

    auto num_bytes = number_of_bytes();
    for (const auto &[offset, f] : fields_) {
        size_t bits_mask = byte_mask_;
        auto number_of_bits = f->number_of_bits();
        if (number_of_bits != num_bytes * 8) {
            bits_mask = (1ULL << number_of_bits) - 1;
        }
        f->set((value >> offset) & bits_mask);
    }
    if (changed) {
        SIM_notify(bank_obj_ref(), Sim_Notify_Bank_Register_Value_Change);
    }
}

uint64_t Register::read(uint64_t enabled_bits) {
    enabled_bits &= byte_mask_;
    if (enabled_bits == 0) {
        return 0;
    }

    std::bitset<64> bits = enabled_bits;
    size_t start_bit_offset = find_first(bits);
    size_t size = bits.count();
    size_t end_bit_offset = start_bit_offset + size;
    if (end_bit_offset != 64 && enabled_bits >> end_bit_offset != 0) {
        SIM_LOG_ERROR_STR(bank_obj_ref(), Register_Read,
                          fmt::format("enabled_bits({:#x}) is malformed:"
                                      " does not contain consecutive ones",
                                      enabled_bits));
        return 0;
    }

    bool partial = enabled_bits != byte_mask_;
    uint64_t ret = 0;
    if (fields_.empty()) {
        ret = get() & enabled_bits;
    } else {
        bits &= read_from_byte_pointers();

        // The first_field points to the first item in the map with a key
        // less than or equal to offset
        auto first_field = fields_.upper_bound(start_bit_offset);
        if (first_field != fields_.begin()) {
            --first_field;
        }

        for (auto it = first_field;
             it != fields_.end() && it->first < end_bit_offset; ++it) {
            auto[field_offset, field_iface] = *it;
            size_t field_end_range = field_offset \
                                     + field_iface->number_of_bits();
            auto[overlap_start, overlap_end] = overlap_range(
                    start_bit_offset, end_bit_offset,
                    field_offset, field_end_range);
            size_t bits_to_read = overlap_end - overlap_start;

            // Field has no overlap with the access
            if (bits_to_read == 0) {
                continue;
            }

            uint64_t bits_mask = (std::numeric_limits<uint64_t>::max)();
            size_t bits_shift = overlap_start - field_offset;
            if (bits_to_read < 64)
                bits_mask = (1ULL << bits_to_read) - 1;
            bits_mask <<= bits_shift;

            uint64_t field_val = field_iface->read(bits_mask) & bits_mask;
            field_val >>= bits_shift;

            for (size_t index = 0; index < bits_to_read; ++index) {
                bits[index + overlap_start] = (field_val >> index) & 1;
            }
        }

        ret = bits.to_ullong();
    }

    if (partial) {
        if (start_bit_offset % 8 == 0 && end_bit_offset % 8 == 0) {
            SIM_LOG_INFO_STR(4, bank_obj_ref(), Register_Read,
                             fmt::format("Partial read from register {}:"
                                         " bytes {}-{} -> {:#x}", name(),
                                         start_bit_offset / 8,
                                         end_bit_offset / 8 - 1, ret));
        } else {
            SIM_LOG_INFO_STR(4, bank_obj_ref(), Register_Read,
                             fmt::format("Partial read from register {}:"
                                         " bits {}-{} -> {:#x}", name(),
                                         start_bit_offset,
                                         end_bit_offset - 1, ret));
        }
    } else {
        SIM_LOG_INFO_STR(4, bank_obj_ref(), Register_Read,
                         fmt::format("Read from register {} -> {:#x}",
                                     name(), ret));
    }

    return ret;
}

void Register::write(uint64_t value, uint64_t enabled_bits) {
    enabled_bits &= byte_mask_;
    if (enabled_bits == 0) {
        return;
    }

    std::bitset<64> bits = enabled_bits;
    size_t start_bit_offset = find_first(bits);
    size_t number_of_bits = bits.count();
    size_t end_bit_offset = start_bit_offset + number_of_bits;
    if (end_bit_offset != 64 && enabled_bits >> end_bit_offset != 0) {
        SIM_LOG_ERROR_STR(bank_obj_ref(), Register_Write,
                          fmt::format("enabled_bits({:#x}) is malformed:"
                                      " does not contain consecutive ones",
                                      enabled_bits));
        return;
    }
    if (enabled_bits != byte_mask_) {
        if (start_bit_offset % 8 == 0 && end_bit_offset % 8 == 0) {
            SIM_LOG_INFO_STR(4, bank_obj_ref(), Register_Write,
                             fmt::format("Partial write to register {}:"
                                         " bytes {}-{} <- {:#x}", name(),
                                         start_bit_offset / 8,
                                         end_bit_offset / 8 - 1,
                                         value & enabled_bits));
        } else {
            SIM_LOG_INFO_STR(4, bank_obj_ref(), Register_Write,
                             fmt::format("Partial write to register {}:"
                                         " bits {}-{} <- {:#x}", name(),
                                         start_bit_offset,
                                         end_bit_offset - 1,
                                         value & enabled_bits));
        }
    } else {
        SIM_LOG_INFO_STR(4, bank_obj_ref(), Register_Write,
                         fmt::format("Write to register {} <- {:#x}",
                                     name(), value & enabled_bits));
    }

    if (fields_.empty()) {
        set((get() & ~enabled_bits) | (value & enabled_bits));
        return;
    }

    // The first_field points to the first item in the map with a key
    // less than or equal to offset
    auto first_field = fields_.upper_bound(start_bit_offset);
    if (first_field != fields_.begin()) {
        --first_field;
    }

    for (auto it = first_field;
         it != fields_.end() && it->first < end_bit_offset; ++it) {
        auto[field_offset, field_iface] = *it;
        size_t field_end_range = field_offset \
                                 + field_iface->number_of_bits();
        auto[overlap_start, overlap_end] = overlap_range(
                start_bit_offset, end_bit_offset,
                field_offset, field_end_range);
        size_t bits_to_write = overlap_end - overlap_start;

        // Field has no overlap with the access
        if (bits_to_write == 0) {
            continue;
        }

        uint64_t bits_mask = (std::numeric_limits<uint64_t>::max)();
        if (bits_to_write < 64)
            bits_mask = (1ULL << bits_to_write) - 1;

        auto write_value = (value >> overlap_start) & bits_mask;
        auto bits_shift = overlap_start - field_offset;
        write_value <<= bits_shift;
        bits_mask <<= bits_shift;

        field_iface->write(write_value, bits_mask);
    }
}

void Register::parse_field(const field_t &f) {
    if (dev_obj()->finalized()) {
        SIM_LOG_ERROR_STR(bank_obj_ref(), 0,
                          fmt::format("Cannot add fields for register ({})"
                                      " when device has finalized",
                                      hierarchical_name()));
        return;
    }

    const auto &[name, desc, offset, width] = f;

    if (width == 0) {
        SIM_LOG_ERROR(bank_obj_ref(), 0,
                      "Ignored invalid field as the width is 0");
        return;
    }

    if (name.array_str().empty()) {
        add_field(name, desc, offset, width);
    } else {
        for (const auto &[_name, _offset]
                 : name.arrayNamesToOffsets(width)) {
            add_field(_name, desc, offset + _offset, width);
        }
    }
}

std::vector<field_t> Register::fields_info() const {
    std::vector<field_t> info;
    for (auto const &field : fields_) {
        info.emplace_back(field.second->name().data(),
                          field.second->description(),
                          field.first,
                          field.second->number_of_bits());
    }
    return info;
}

BankInterface *Register::parent() const {
    return parent_;
}

void Register::set_init_value(uint64_t init_val) {
    init_val_ = init_val;
}

void Register::add_register_as_simics_attribute(
        const RegisterInterface *iface) {
    auto *bank_class = SIM_object_class(iface->bank_obj_ref());

    auto reg_name = iface->name();
    // Extract the name before the '[' character from the input
    // string_view. Since later code relies on null-terminated
    // strings (using data()), we use std::string instead of
    // std::string_view to ensure the extracted name is
    // null-terminated.
    std::string reg_name_wo_brackets {
        reg_name.substr(0, reg_name.find('['))
    };

    if (SIM_class_has_attribute(bank_class,
                                reg_name_wo_brackets.c_str())) {
        return;
    }

    std::string type {"i"};
    auto dims = std::count(reg_name.begin(), reg_name.end(), '[');
    for (int index = 0; index < dims; ++index) {
        type = '[' + type + "+]";
    }
    auto hashed = hash_str(iface->hierarchical_name());
    SIM_register_attribute_with_user_data(
            bank_class, reg_name_wo_brackets.c_str(),
            get_reg, reinterpret_cast<void *>(hashed),
            set_reg, reinterpret_cast<void *>(hashed),
            Sim_Attr_Optional, type.c_str(),
            iface->description().c_str());
}

attr_value_t Register::get_reg_array(size_t indices, size_t dim_index,
                                     MappableConfObject *obj,
                                     const std::string &base_name) {
    size_t array_index = 0;
    if (dim_index == indices) {
        // The most inner dimension
        std::vector<size_t> values;
        while (true) {
            auto *reg_iface = obj->template get_iface<
                RegisterInterface>(
                        base_name + '[' + std::to_string(array_index)   \
                        + ']');
            if (reg_iface == nullptr) {
                break;
            }
            values.push_back(reg_iface->get());
            ++array_index;
        }
        if (values.empty()) {
            return SIM_make_attr_nil();
        } else {
            return std_to_attr(values);
        }
    } else {
        std::vector<attr_value_t> values;
        while (true) {
            auto attr = get_reg_array(indices, dim_index + 1, obj,
                                      fmt::format("{}[{}]", base_name,
                                                  array_index++));
            if (SIM_attr_is_nil(attr)) {
                break;
            } else {
                values.push_back(attr);
            }
        }
        return std_to_attr(values);
    }
}

attr_value_t Register::get_reg(conf_object_t *obj, void *data) {
    auto *mappable_obj = from_obj<MappableConfObject>(
            SIM_port_object_parent(obj));
    auto name_hash = reinterpret_cast<size_t>(data);
    auto *reg_iface = mappable_obj->get_iface(name_hash);
    if (reg_iface == nullptr) {
        SIM_c_attribute_error("register not found");
        return SIM_make_attr_nil();
    }

    auto reg_name = reg_iface->name();
    size_t indices = static_cast<size_t>(
        std::count(reg_name.cbegin(), reg_name.cend(), '['));
    if (indices == 0) {
        return std_to_attr(reg_iface->get());
    }

    // This register contains an array but size unknown
    auto base_name = std::string(reg_iface->parent()->name()) + SEPARATOR \
        + std::string(reg_name.substr(0, reg_name.find('[')));
    return get_reg_array(indices, 1, mappable_obj, base_name);
}

set_error_t Register::set_reg_array(size_t indices, size_t dim_index,
                                    MappableConfObject *obj,
                                    const std::string &base_name,
                                    attr_value_t *val) {
    size_t array_index = 0;
    if (dim_index == indices) {
        // The most inner dimension
        auto values = attr_to_std<std::vector<size_t>>(*val);
        while (true) {
            auto *reg_iface = obj->template get_iface<
                RegisterInterface>(
                        base_name + '[' + std::to_string(array_index)   \
                        + ']');
            if (reg_iface == nullptr) {
                break;
            } else if (array_index == values.size()) {
                return Sim_Set_Illegal_Index;
            }
            reg_iface->set(values.at(array_index));
            ++array_index;
        }
        if (values.size() == array_index) {
            return Sim_Set_Ok;
        } else {
            return Sim_Set_Illegal_Index;
        }
    } else {
        auto values = attr_to_std<std::vector<attr_value_t>>(*val);
        while (array_index < values.size()) {
            auto status = set_reg_array(indices, dim_index + 1, obj,
                                        fmt::format("{}[{}]", base_name,
                                                    array_index),
                                        &values.at(array_index));
            if (status == Sim_Set_Illegal_Index) {
                if (array_index + 1 == values.size()) {
                    return Sim_Set_Ok;
                } else {
                    return Sim_Set_Illegal_Index;
                }
            }
            ++array_index;
        }
        return Sim_Set_Ok;
    }
}

set_error_t Register::set_reg(conf_object_t *obj, attr_value_t *val,
                              void *data) {
    auto *mappable_obj = from_obj<MappableConfObject>(
            SIM_port_object_parent(obj));
    auto name_hash = reinterpret_cast<size_t>(data);
    auto reg_iface = mappable_obj->get_iface(name_hash);
    if (!reg_iface) {
        return Sim_Set_Interface_Not_Found;
    }

    auto reg_name = reg_iface->name();
    size_t indices = static_cast<size_t>(
        std::count(reg_name.cbegin(), reg_name.cend(), '['));
    if (indices == 0) {
        reg_iface->set(attr_to_std<size_t>(*val));
        return Sim_Set_Ok;
    }

    // This register contains an array but size unknown
    auto base_name = std::string(reg_iface->parent()->name()) + SEPARATOR \
        + std::string(reg_name.substr(0, reg_name.find('[')));
    return set_reg_array(indices, 1, mappable_obj, base_name, val);
}

void Register::check_number_of_bytes(unsigned number_of_bytes) {
    if (number_of_bytes > 8 || number_of_bytes == 0) {
        SIM_LOG_ERROR(bank_obj_ref(), 0,
                      "The supported register size is [1-8] bytes");
        return;
    }
    if (byte_pointers_.size() != number_of_bytes) {
        SIM_LOG_ERROR_STR(bank_obj_ref(), 0,
                          fmt::format("The memory size({}) does not fit "
                                      "the number of bytes({})",
                                      byte_pointers_.size(),
                                      number_of_bytes));
        return;
    }
}

bool Register::has_range_overlap(size_t lsb, size_t msb) const {
    return std::any_of(fields_.cbegin(), fields_.cend(),
               [lsb, msb](auto p){
                   return lsb < (p.first + p.second->number_of_bits())
                       && p.first < msb;
               });
}

void Register::add_field(std::string_view field_name, std::string_view desc,
                         Offset offset, BitWidth width) {
    if (offset >= number_of_bytes() * 8
        || offset + width > number_of_bytes() * 8) {
        SIM_LOG_ERROR(bank_obj_ref(), 0,
                      "Ignored invalid field as the offset exceeds"
                      " the number of bits");
        return;
    }

    if (has_range_overlap(offset, offset + width - 1)) {
        SIM_LOG_ERROR(bank_obj_ref(), 0,
                      "Ignored invalid field as it overlaps with"
                      " other field");
        return;
    }

    const std::string full_name {
        hierarchical_name() + SEPARATOR + field_name.data()};
    fields_[offset] = dev_obj()->get_iface<FieldInterface>(full_name);
    if (fields_[offset] == nullptr) {
        allocated_fields_.push_back(std::make_unique<Field>(dev_obj(),
                                                            full_name));
        fields_[offset] = allocated_fields_.back().get();
        SIM_LOG_INFO_STR(3, bank_obj_ref(), 0,
                         fmt::format("Created default field for {}",
                                     full_name));
    } else if (fields_[offset]->number_of_bits() == 0) {
        SIM_LOG_INFO_STR(3, bank_obj_ref(), 0,
                         fmt::format("Used user defined field for {}",
                                     full_name));
    } else {
        fields_.erase(offset);
        SIM_LOG_ERROR_STR(bank_obj_ref(), 0,
                          fmt::format("Duplicated field name({}) on same"
                                      " register", field_name));
        return;
    }

    if (byte_pointers_.empty()) {
        SIM_LOG_ERROR_STR(bank_obj_ref(), 0,
                          fmt::format("No storage allocated on register %s",
                                      name()));
        return;
    }

    // Pass the related bits to the field in format of byte
    // pointer with a mask
    bits_type bits;
    for (auto i = offset / 8; i <= (offset + width - 1) / 8; ++i) {
        uint8_t bits_mask = 0xff;
        if (i == offset / 8) {
            // The first byte
            bits_mask <<= (offset % 8);
        }
        if (i == (offset + width - 1) / 8) {
            // The last byte
            uint8_t num_masked_bits = (offset + width) % 8;
            if (num_masked_bits != 0) {
                bits_mask &= static_cast<uint8_t>(
                        (1 << num_masked_bits) - 1);
            }
        }
        bits.push_back(std::make_pair(byte_pointers_[i], bits_mask));
    }
    fields_[offset]->init(desc, bits, static_cast<int8_t>(offset));
}

uint64_t Register::read_from_byte_pointers() const {
    uint64_t result = 0;
    auto num_bytes = number_of_bytes();
    for (size_t i = 0; i < num_bytes; ++i) {
        result |= uint64_t{*byte_pointers_[i]} << (8 * i);
    }
    return result;
}

}  // namespace simics
