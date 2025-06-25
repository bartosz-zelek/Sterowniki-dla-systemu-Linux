/***                                                                       ***/
/***   INTEL CORPORATION PROPRIETARY INFORMATION                           ***/
/***                                                                       ***/
/***   This software is supplied under the terms of a license              ***/
/***   agreement or nondisclosure agreement with Intel Corporation         ***/
/***   and may not be copied or disclosed except in accordance with        ***/
/***   the terms of that agreement.                                        ***/
/***   Copyright 2014-2021 Intel Corporation                               ***/
/***                                                                       ***/

#ifndef COMMUNICATION_SC_SIGNAL_CALLBACK_H
#define COMMUNICATION_SC_SIGNAL_CALLBACK_H

#include "sysc/kernel/sc_attribute.h"

namespace intc {

class sc_signal_callback_interface {
  public:
    virtual void callback(const sc_core::sc_object& signal_object) = 0;
    virtual ~sc_signal_callback_interface() {}
};

class sc_signal_callback
    : public sc_core::sc_attribute<sc_signal_callback_interface*> {
  public:
    explicit sc_signal_callback(sc_signal_callback_interface* callback)
      : sc_core::sc_attribute<sc_signal_callback_interface*> (name(),
                                                              callback) {}
    static const char* name() {
        return "sc_signal_callback";
    };
};

}  // namespace intc

#endif
