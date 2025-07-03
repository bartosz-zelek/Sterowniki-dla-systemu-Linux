/*
  © 2018 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <simics/systemc/systemc_library.h>

#include "i2c-master.h"
#include <stdint.h>
#include <simics/types/i2c_ack.h>

#define DEVICE_CLASS "sample_tlm2_i2c_master"

namespace scl = simics::systemc;

class MasterAdapter : public scl::Adapter,
                      public scl::simics2tlm::I2cMasterV2GasketAdapter {
  public:
    explicit MasterAdapter(simics::ConfObjectRef o)
        : scl::Adapter(o),
          I2cMasterV2GasketAdapter(&systemc_i2c_master_, this),
          i2c_master_dev_(this, "i2c_master_device") {
        systemc_i2c_master_.set_gasket(scl::simics2tlm::createGasket(
            &i2c_master_dev_->i2c_target_socket, o));
        simics_i2c_slave->set_gasket(scl::tlm2simics::createGasket(
            &i2c_master_dev_->i2c_initiator_socket, o));
    }

    static void init_class(simics::ConfClass *cls);

    scl::Connector<scl::tlm2simics::I2cSlaveV2> simics_i2c_slave;

    uint8_t get_address() const;
    void set_address(const uint8_t &val);
    uint8_t get_data_rd() const;
    void set_data_rd(const uint8_t &val);
    uint8_t get_data_wr() const;
    void set_data_wr(const uint8_t &val);
    int get_ack() const;
    void set_ack(const int &val);

  private:
    // Incoming from Simics to I2C device
    scl::simics2tlm::I2cMasterV2 systemc_i2c_master_;

    // I2C master device implementation by SystemC/TLM
    scl::Device<I2cMaster> i2c_master_dev_;
};

uint8_t MasterAdapter::get_address() const {
    return i2c_master_dev_->get_address();
}

void MasterAdapter::set_address(const uint8_t &val) {
    i2c_master_dev_->set_address(val);
}

uint8_t MasterAdapter::get_data_rd() const {
    return i2c_master_dev_->get_data_rd();
}

void MasterAdapter::set_data_rd(const uint8_t &val) {
    i2c_master_dev_->set_data_rd(val);
}

uint8_t MasterAdapter::get_data_wr() const {
    return i2c_master_dev_->get_data_wr();
}

void MasterAdapter::set_data_wr(const uint8_t &val) {
    i2c_master_dev_->set_data_wr(val);
}

int MasterAdapter::get_ack() const {
    return (int)i2c_master_dev_->get_ack();
}

void MasterAdapter::set_ack(const int &val) {
    if (val != 0 && val != 1)
        throw("Invalid value for i2c_ack_t.");
    i2c_master_dev_->set_ack(
            val == 0 ? simics::types::I2C_ack : simics::types::I2C_noack);
}

void MasterAdapter::init_class(simics::ConfClass *cls) {
    cls->add(simics::Attribute("i2c_link_v2", "[os]|o|n",
                               "I2C link to receive I2C transactions.",
                               ATTR_CLS_VAR(MasterAdapter, simics_i2c_slave)));
    cls->add(scl::iface::createAdapter<
                scl::iface::I2cMasterV2SimicsAdapter<MasterAdapter> >());
    cls->add(simics::Attribute("address", "i",
                               "The value of I2C address.",
                               ATTR_GETTER(MasterAdapter, get_address),
                               ATTR_SETTER(MasterAdapter, set_address)));
    cls->add(simics::Attribute("data_rd", "i",
                               "The data read from I2C.",
                               ATTR_GETTER(MasterAdapter, get_data_rd),
                               ATTR_SETTER(MasterAdapter, set_data_rd)));
    cls->add(simics::Attribute("data_wr", "i",
                               "The data write to I2C.",
                               ATTR_GETTER(MasterAdapter, get_data_wr),
                               ATTR_SETTER(MasterAdapter, set_data_wr)));
    cls->add(simics::Attribute("ack", "i",
                               "The acknowledge value received from I2C.",
                               ATTR_GETTER(MasterAdapter, get_ack),
                               ATTR_SETTER(MasterAdapter, set_ack)));
}

extern "C" void init_local_master(void) {
    simics::make_class<MasterAdapter>(
        DEVICE_CLASS,
        "sample OSCI TLM2 I2C master",
        "The <class>" DEVICE_CLASS "</class> is a Simics module"
        " encapsulating a SystemC TLM2-based I2C master to demonstrate"
        " the use of Simics SystemC Library.");
}
