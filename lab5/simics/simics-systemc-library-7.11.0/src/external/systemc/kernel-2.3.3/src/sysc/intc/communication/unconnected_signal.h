/***                                                                       ***/
/***   INTEL CORPORATION PROPRIETARY INFORMATION                           ***/
/***                                                                       ***/
/***   This software is supplied under the terms of a license              ***/
/***   agreement or nondisclosure agreement with Intel Corporation         ***/
/***   and may not be copied or disclosed except in accordance with        ***/
/***   the terms of that agreement.                                        ***/
/***   Copyright 2015-2021 Intel Corporation                               ***/
/***                                                                       ***/

#ifndef COMMUNICATION_UNCONNECTED_SIGNAL_H
#define COMMUNICATION_UNCONNECTED_SIGNAL_H

#include "sysc/kernel/sc_event.h"
#include "sysc/intc/communication/unconnected_base.h"
#include "sysc/communication/sc_signal_ifs.h"

namespace {
// This is a copy of sc_signal_channel::deprecated_get_data_ref()
void deprecated_get_data_ref() {
    static bool warn_get_data_ref_deprecated=true;
    if ( warn_get_data_ref_deprecated ) {
        warn_get_data_ref_deprecated=false;
	SC_REPORT_INFO(sc_core::SC_ID_IEEE_1666_DEPRECATION_,
	    "sc_signal<T>::get_data_ref() is deprecated, use read() instead" );
    }
}
}

namespace intc {

template< class T >
class UnconnectedSignal : public UnconnectedBase,
                          public sc_core::sc_signal_inout_if<T> {
  public:
    UnconnectedSignal(sc_core::sc_object *bound_by)
        : UnconnectedBase(bound_by),
          never_notified_(sc_core::sc_event::kernel_event) {
    }
    
    const sc_core::sc_event& default_event() const {
        report_unbound();
        return never_notified_;
    }
    
    // sc_signal_in_if
    virtual const sc_core::sc_event& value_changed_event() const {
        return default_event();
    }
    virtual const T& read() const {
        static const T val = T();  // Needed to take reference
        report_unbound();
        return val;
    }
    virtual const T& get_data_ref() const {
        ::deprecated_get_data_ref();
        return read();
    }
    virtual bool event() const {
        report_unbound();
        return false;
    }

    // sc_signal_in_if<bool>
    virtual const sc_core::sc_event& posedge_event() const {
        return default_event();
    }
    virtual const sc_core::sc_event& negedge_event() const {
        return default_event();
    }
    virtual bool posedge() const { return event(); }
    virtual bool negedge() const { return event(); }
    
    // sc_signal_write_if
    virtual void write(const T&) {
        report_unbound();
    }

  private:
    // NOTE: do not confuse this with the static sc_event::m_never_notified
    // event, as we cannot use a static event due to how we handle multiple
    // contexts in Simics
    sc_core::sc_event never_notified_;
};

}  // namespace intc

#endif
