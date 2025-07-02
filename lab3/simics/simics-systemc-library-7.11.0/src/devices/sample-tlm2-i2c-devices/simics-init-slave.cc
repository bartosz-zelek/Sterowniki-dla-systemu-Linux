/*
  © 2012 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <simics/systemc/systemc_library.h>
#include <array>

#include "i2c-slave.h"

#define DEVICE_CLASS "sample_tlm2_i2c_slave"

// <add id="sample-tlm2-i2c-devices/Adapter">
// This adapter wraps two SystemC I2C slave devices using Simics port object.
// <insert-until text="// EOF_I2C_SLAVE_ADAPTER"/></add>
namespace scl = simics::systemc;

class SlaveAdapter : public scl::Adapter {
  public:
    explicit SlaveAdapter(simics::ConfObjectRef o)
        : scl::Adapter(o) {
        for (unsigned i = 0; i < 2; ++i) {
            systemc_io[i].set_gasket(scl::simics2tlm::createGasket(
                &i2c_slave_devs[i]->io_target_socket, o));
            systemc_i2c[i].set_gasket(scl::simics2tlm::createGasket(
                &i2c_slave_devs[i]->i2c_target_socket, o));
            simics_i2c[i].set_gasket(scl::tlm2simics::createGasket(
                &i2c_slave_devs[i]->i2c_master_initiator_socket, o));
        }
    }

    static void init_class(simics::ConfClass *cls);

    template <int id> int get_register() const;
    template <int id> void set_register(const int &val);

    template <int id> int get_i2c_address() const;
    template <int id> void set_i2c_address(const int &val);

    template <int id> simics::ConfObjectRef get_i2c_link() const;
    template <int id> void set_i2c_link(const simics::ConfObjectRef &obj_ref);

    class Port : public simics::Port<SlaveAdapter>,
                 public scl::simics2tlm::TransactionGasketAdapter,
                 public scl::simics2tlm::I2cSlaveV2GasketAdapter {
      public:
        explicit Port(simics::ConfObjectRef o)
            : simics::Port<SlaveAdapter>(o),
              TransactionGasketAdapter(&parent()->systemc_io[index()],
                                       parent()),
              I2cSlaveV2GasketAdapter(&parent()->systemc_i2c[index()], parent()) {
        }
    };

  private:
    scl::simics2tlm::Transaction     systemc_io[2];
    scl::simics2tlm::I2cSlaveV2   systemc_i2c[2];
    scl::tlm2simics::I2cMasterV2  simics_i2c[2];
    std::array<scl::Device<I2cSlave>, 2> i2c_slave_devs {{
                    {this, "i2c_dev0"},
                    {this, "i2c_dev1"}}};
};  // EOF_I2C_SLAVE_ADAPTER

template <int id>
int SlaveAdapter::get_register() const {
    return i2c_slave_devs[id]->get_register();
}

template <int id>
void SlaveAdapter::set_register(const int &val) {
    i2c_slave_devs[id]->set_register(val);
}

template <int id>
int SlaveAdapter::get_i2c_address() const {
    return i2c_slave_devs[id]->get_address();
}

template <int id>
void SlaveAdapter::set_i2c_address(const int &val) {
    i2c_slave_devs[id]->set_address(val);
}

template <int id>
simics::ConfObjectRef SlaveAdapter::get_i2c_link() const {
    return simics_i2c[id].target();
}

template <int id>
void SlaveAdapter::set_i2c_link(const simics::ConfObjectRef &obj_ref) {
    simics_i2c[id].set_target(obj_ref);
}

void SlaveAdapter::init_class(simics::ConfClass *cls) {
    cls->add(simics::Attribute(
                     "I2C0_register", "i",
                     "The value of I2C register.",
                     ATTR_GETTER(SlaveAdapter, get_register<0>),
                     ATTR_SETTER(SlaveAdapter, set_register<0>)));
    cls->add(simics::Attribute(
                     "I2C0_i2c_link_v2", "[os]|o|n",
                     "The I2C link which the slave connects to.",
                     ATTR_GETTER(SlaveAdapter, get_i2c_link<0>),
                     ATTR_SETTER(SlaveAdapter, set_i2c_link<0>)));
    cls->add(simics::Attribute(
                     "I2C0_i2c_address", "i",
                     "The 7-bit I2C address.",
                     ATTR_GETTER(SlaveAdapter, get_i2c_address<0>),
                     ATTR_SETTER(SlaveAdapter, set_i2c_address<0>)));

    cls->add(simics::Attribute(
                     "I2C1_register", "i",
                     "The value of I2C register.",
                     ATTR_GETTER(SlaveAdapter, get_register<1>),
                     ATTR_SETTER(SlaveAdapter, set_register<1>)));
    cls->add(simics::Attribute(
                     "I2C1_i2c_link_v2", "[os]|o|n",
                     "The I2C link which the slave connects to.",
                     ATTR_GETTER(SlaveAdapter, get_i2c_link<1>),
                     ATTR_SETTER(SlaveAdapter, set_i2c_link<1>)));
    cls->add(simics::Attribute(
                     "I2C1_i2c_address", "i",
                     "The 7-bit I2C address.",
                     ATTR_GETTER(SlaveAdapter, get_i2c_address<1>),
                     ATTR_SETTER(SlaveAdapter, set_i2c_address<1>)));

    // <add id="sample-tlm2-i2c-devices/port_registration">
    // <insert-until text="// EOF_I2C_PORT_REGISTRATION"/></add>
    auto port = simics::make_class<SlaveAdapter::Port>(
            "sample_tlm2_i2c_slave.port", "i2C port", "I2C port");
    port->add(scl::iface::createAdapter<
              scl::iface::TransactionSimicsAdapter<SlaveAdapter::Port>>());
    port->add(scl::iface::createAdapter<
              scl::iface::I2cSlaveV2SimicsAdapter<SlaveAdapter::Port>>());
    cls->add(port, "port.I2C[2]");
    // EOF_I2C_PORT_REGISTRATION
}

// <add id="sample-tlm2-i2c-devices/init_local">
// <insert-until text="// EOF_I2C_INIT_LOCAL"/></add>
extern "C" void init_local_slave(void) {
    simics::make_class<SlaveAdapter>(
        DEVICE_CLASS,
        "sample OSCI TLM2 I2C slave",
        "The <class>" DEVICE_CLASS "</class> is a Simics module"
        " encapsulating a SystemC TLM2-based I2C slave to demonstrate"
        " the use of Simics SystemC Library.");
}  // EOF_I2C_INIT_LOCAL
