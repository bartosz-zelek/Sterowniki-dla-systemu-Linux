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

#ifndef SIMICS_SYSTEMC_DEVICE_H
#define SIMICS_SYSTEMC_DEVICE_H


#include <systemc>

#include <simics/systemc/context.h>
#include <simics/systemc/device_access.h>
#include <simics/systemc/iface/simulation_interface.h>

#include <string>
#include <utility>

namespace simics {
namespace systemc {

/**
 * Utility class for handling direct access to the SystemC top module (i.e. the
 * device).  Using this class ensures that the correct simulation context is
 * set in the SC kernel.
 */
template <typename T>
class Device {
  public:
    enum ownership_t {
        DONT_TAKE_OWNERSHIP,
        TAKE_OWNERSHIP
    };

    /**
     * Default constructor. Use when elaborating the SystemC device during the
     * finalize phase.
     */
    Device() : simulation_(NULL), device_(NULL), is_device_owner_(false) {}

    /**
     * Assume ownership of the underlying simulation and device model.
     *
     * Creating one Device from another will transfer the ownership of the
     * underlying device_model and simulation reference to the new Device
     * object.
     *
     * @param other The old device will have its device_model and simulation
     * set to NULL. Thus it is an error to dereference the device_model through
     * the original Device object. However, the original Device object can
     * safely be assigned to again.
     */
    Device(Device &other)
        : simulation_(NULL), device_(NULL), is_device_owner_(false) {
        *this = other;
    }

    /**
     * This constructor automatically allocates an object of type T with
     * default arguments.
     *
     * @param simulation The simulation interface (typically the Adapter
     * object).
     */
    explicit Device(iface::SimulationInterface *simulation)
        : simulation_(simulation), device_(new T), is_device_owner_(true) {
    }

    /**
     * This constructor automatically allocates an object of type T with
     * device_name passed as the only argument to T's constructor.
     *
     * @param simulation The simulation interface (typically the Adapter
     * object).
     * @param device_name Name of the SystemC model, passed as the argument to
     * T's constructor.
     */
    Device(iface::SimulationInterface *simulation,
           const std::string &device_name)
        : simulation_(simulation), device_(new T(device_name.c_str())),
          is_device_owner_(true) {
    }

    /**
     * Wrap an existing model in this Device.
     *
     * @param simulation The simulation interface (typically the Adapter
     * object).
     * @param device_model The SystemC model wrapped by this Device
     * @param ownership If ownership is set to DONT_TAKE_OWNERSHIP the
     * device_model will *not* be freed by the Device object, it is the callers
     * responsibility to free the pointer passed to the constructor. If
     * ownership is set to TAKE_OWNERSHIP the Device object will claim
     * ownership of the device_model and will delete it when the Device is
     * deleted.
     */
    Device(iface::SimulationInterface *simulation, T *device_model,
           ownership_t ownership = TAKE_OWNERSHIP)
        : simulation_(simulation), device_(device_model),
          is_device_owner_(ownership == TAKE_OWNERSHIP) {
    }


    /**
     * Assume ownership of the underlying simulation and device model.
     *
     * Assigning one Device to another will transfer the ownership of the
     * underlying device_model and simulation reference to the left hand side
     * of the expression. The operand on the right hand side will have its
     * device_model and simulation set to NULL. Thus it is an error to
     * dereference the device_model through the original Device object (rhs)
     * after the assignment. However, the original Device object can safely be
     * assigned to again.
     *
     * It is an error to assign to a Device that already has a non-NULL
     * device_model or simulation.
     */
    Device& operator=(Device &other) {  // NOLINT(runtime/references)
        FATAL_ERROR_IF(device_, "Changing the device is not allowed");
        FATAL_ERROR_IF(simulation_, "Changing the simulation is not allowed");

        std::swap(device_, other.device_);
        std::swap(simulation_, other.simulation_);
        std::swap(is_device_owner_, other.is_device_owner_);
        return *this;
    }

    /// Access the underlying device object with the proper simulation context.
    DeviceAccess<T> operator->() {
        return DeviceAccess<T>(simulation_, device_);
    }

    /// Access the underlying device object with the proper simulation context.
    const DeviceAccess<T> operator->() const {
        return DeviceAccess<T>(simulation_, device_);
    }

    /// pointer to underlying device object
    T *pointer() {
        return device_;
    }

    /// pointer to underlying device object
    const T *pointer() const {
        return device_;
    }

    virtual ~Device() {
        if (is_device_owner_) {
            simics::systemc::Context c(simulation_);
            delete device_;
        }
    }

  private:
    iface::SimulationInterface *simulation_;
    T *device_;
    bool is_device_owner_;
};

}  // namespace systemc
}  // namespace simics

#endif
