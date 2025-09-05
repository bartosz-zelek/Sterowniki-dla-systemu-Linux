/***                                                                       ***/
/***   INTEL CORPORATION PROPRIETARY INFORMATION                           ***/
/***                                                                       ***/
/***   This software is supplied under the terms of a license              ***/
/***   agreement or nondisclosure agreement with Intel Corporation         ***/
/***   and may not be copied or disclosed except in accordance with        ***/
/***   the terms of that agreement.                                        ***/
/***   Copyright 2015-2021 Intel Corporation                               ***/
/***                                                                       ***/

#ifndef COMMUNICATION_SC_PORT_SPY_H
#define COMMUNICATION_SC_PORT_SPY_H

namespace sc_core {
template<class IF>
class sc_port_b;
}

namespace intc {

template<class IF>
class sc_port_spy {
  public:
    sc_port_spy(sc_core::sc_port_b<IF>* port_p, int interface_idx = 0)
        : m_idx(interface_idx), m_if(0), m_spy(0), m_port_p(port_p) {}
    void set_spy_interface(IF* if_) {
        if (!m_if)
            m_if = m_port_p->m_interface_vec.at(m_idx);

        m_port_p->m_interface_vec.at(m_idx) = if_;
        m_spy = if_;

        if (m_idx == 0)
            m_port_p->m_interface = if_;
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
        return m_port_p->m_interface_vec.at(m_idx);
    }

  private:
    int m_idx;
    IF* m_if;
    IF* m_spy;
    sc_core::sc_port_b<IF>* m_port_p;
};

}  // namespace intc

#endif
