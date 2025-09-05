/***                                                                       ***/
/***   INTEL CORPORATION PROPRIETARY INFORMATION                           ***/
/***                                                                       ***/
/***   This software is supplied under the terms of a license              ***/
/***   agreement or nondisclosure agreement with Intel Corporation         ***/
/***   and may not be copied or disclosed except in accordance with        ***/
/***   the terms of that agreement.                                        ***/
/***   Copyright 2015-2021 Intel Corporation                               ***/
/***                                                                       ***/

#ifndef COMMUNICATION_UNCONNECTED_BASE_H
#define COMMUNICATION_UNCONNECTED_BASE_H

#include <systemc>
#include <string>

namespace intc {

class UnconnectedBase {
  public:
    UnconnectedBase(sc_core::sc_object *bound_by)
        : m_msg(std::string("Access to unbound port: ") + bound_by->name()) {
    }
    virtual ~UnconnectedBase() {}
    
  protected:
    void report_unbound() const {
        SC_REPORT_INFO("intc/unimplemented", m_msg.c_str());
    }

  private:
    std::string m_msg;
};

}  // namespace intc

#endif
