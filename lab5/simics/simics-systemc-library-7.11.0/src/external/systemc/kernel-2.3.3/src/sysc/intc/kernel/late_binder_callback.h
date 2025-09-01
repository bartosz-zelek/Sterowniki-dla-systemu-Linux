/***                                                                       ***/
/***   INTEL CORPORATION PROPRIETARY INFORMATION                           ***/
/***                                                                       ***/
/***   This software is supplied under the terms of a license              ***/
/***   agreement or nondisclosure agreement with Intel Corporation         ***/
/***   and may not be copied or disclosed except in accordance with        ***/
/***   the terms of that agreement.                                        ***/
/***   Copyright 2015-2021 Intel Corporation                               ***/
/***                                                                       ***/

#ifndef KERNEL_LATE_BINDER_CALLBACK_H
#define KERNEL_LATE_BINDER_CALLBACK_H

namespace sc_core {
class sc_port_base;
}

namespace intc {

/* The late_binder_callback_interface is invoked by sc_port from
   complete_binding() when the port needs to be bound. It is the responsibility
   of the late binder to bind the port to an object that implements the correct
   interface. The interace will not be invoked if the port has already been
   bound or will be bound during complete_binding(). */         
class late_binder_callback_interface {
  public:
    virtual void bind(sc_core::sc_port_base *port) = 0;
    virtual ~late_binder_callback_interface() {}
};

}  // namespace intc

#endif
