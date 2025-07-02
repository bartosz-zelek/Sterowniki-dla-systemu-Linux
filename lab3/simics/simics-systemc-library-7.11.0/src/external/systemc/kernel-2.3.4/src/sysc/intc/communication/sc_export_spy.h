/***                                                                       ***/
/***   INTEL CORPORATION PROPRIETARY INFORMATION                           ***/
/***                                                                       ***/
/***   This software is supplied under the terms of a license              ***/
/***   agreement or nondisclosure agreement with Intel Corporation         ***/
/***   and may not be copied or disclosed except in accordance with        ***/
/***   the terms of that agreement.                                        ***/
/***   Copyright 2015-2021 Intel Corporation                               ***/
/***                                                                       ***/

#ifndef COMMUNICATION_SC_EXPORT_SPY_H
#define COMMUNICATION_SC_EXPORT_SPY_H

namespace sc_core {
template<class IF>
class sc_export;
}

namespace intc {

template<class IF>
class sc_export_spy {
  public:
    explicit sc_export_spy(sc_core::sc_export<IF>* export_p)
        : m_if(0), m_spy(0), m_export_p(export_p) {}
    void set_spy_interface(IF* if_) {
        if (!m_if)
            m_if = m_export_p->m_interface_p;

        m_export_p->m_interface_p = if_;
        m_spy = if_;
    }
    IF* get_spy_interface() {
        return m_spy;
    }
    IF* set_interface(IF* if_) {
        return m_if = if_;
    }
    IF* get_interface() {
        return m_if;
    }
    IF* get_current_interface() {
        return m_export_p->m_interface_p;
    }

  private:
    IF* m_if;
    IF* m_spy;
    sc_core::sc_export<IF>* m_export_p;
};

}  // namespace intc

#endif
