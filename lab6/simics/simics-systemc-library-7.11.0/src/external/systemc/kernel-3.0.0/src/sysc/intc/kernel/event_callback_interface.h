/***                                                                       ***/
/***   INTEL CORPORATION PROPRIETARY INFORMATION                           ***/
/***                                                                       ***/
/***   This software is supplied under the terms of a license              ***/
/***   agreement or nondisclosure agreement with Intel Corporation         ***/
/***   and may not be copied or disclosed except in accordance with        ***/
/***   the terms of that agreement.                                        ***/
/***   Copyright 2016-2021 Intel Corporation                               ***/
/***                                                                       ***/

#ifndef KERNEL_EVENT_CALLBACK_INTERFACE_H
#define KERNEL_EVENT_CALLBACK_INTERFACE_H

namespace intc {

enum EventType {
    INTC_EVENT_PROCESS_ACTIVATION,
    INTC_EVENT_EVENT_TRIGGERED,
    INTC_EVENT_ASYNC_REQUEST_UPDATE,
    INTC_EVENT_DELTA_CYCLE_COMPLETED
};


class event_callback_interface {
  public:
    /** @brief Callback function routing events from the kernel to a 
     *         registered router.
     * @param event_type String describing the event.
     * @param class_type String describing the class of the object.
     * @param object Pointer to the object triggering the event.
     * @param ts Time stamp when the event is triggered.
     */
    virtual void event_callback(EventType event_type,
                                const char *class_type,
                                void *object,
                                const sc_core::sc_time &ts) = 0;
    
    virtual ~event_callback_interface() {}
};

}  // namespace intc

#endif
