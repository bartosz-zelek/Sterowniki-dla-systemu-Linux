/***                                                                       ***/
/***   INTEL CORPORATION PROPRIETARY INFORMATION                           ***/
/***                                                                       ***/
/***   This software is supplied under the terms of a license              ***/
/***   agreement or nondisclosure agreement with Intel Corporation         ***/
/***   and may not be copied or disclosed except in accordance with        ***/
/***   the terms of that agreement.                                        ***/
/***   Copyright 2015-2021 Intel Corporation                               ***/
/***                                                                       ***/

/*****************************************************************************

  Licensed to Accellera Systems Initiative Inc. (Accellera) under one or
  more contributor license agreements.  See the NOTICE file distributed
  with this work for additional information regarding copyright ownership.
  Accellera licenses this file to you under the Apache License, Version 2.0
  (the "License"); you may not use this file except in compliance with the
  License.  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
  implied.  See the License for the specific language governing
  permissions and limitations under the License.

 *****************************************************************************/
//==============================================================================
///  @file lt_td_initiator.cpp
///  @Details
///  
///
//==============================================================================
//  Original Authors:
//    Jack Donovan, ESLX
//    Charles Wilson, ESLX
//==============================================================================

#include "reporting.h"                             ///< Reporting convenience macros
#include "lt_td_initiator.h"                          ///< Our header

using namespace sc_core;
static const char *filename = "lt_td_initiator.cpp"; ///< filename for reporting

//==============================================================================
///  @fn lt_td_initiator::lt_td_initiator
///
///  @brief class constructor
///
///  @details
///    This is the class constructor.
///
//==============================================================================
SC_HAS_PROCESS(lt_td_initiator);
lt_td_initiator::lt_td_initiator                        // constructor
( sc_module_name name                             // module name
, const unsigned int  ID                          // initiator ID
)
: sc_module           (name)                      // initialize module name
, initiator_socket    ("initiator_socket")        // initiator socket
, m_delay             (0,sc_core::SC_NS)
, m_ID                (ID)                        // initialize initiator ID

{                
  tlm_utils::tlm_quantumkeeper::set_global_quantum(sc_core::sc_time(500,sc_core::SC_NS));
  // register thread process
  SC_THREAD(initiator_thread);                  
#if INTC_CHKPT
  transaction_ptr = 0;
  m_would_block = true;
  m_trans_sent = false;
  m_data_buffer[0] = 0;
  m_data_buffer[1] = 0;
  m_data_buffer[2] = 0;
  m_data_buffer[3] = 0;
#endif
}

/*==============================================================================
///  @fn lt_td_initiator::initiator_thread
///
///  @brief initiates non-blocking transport
///
///  @details
/// 
==============================================================================*/
void lt_td_initiator::initiator_thread(void)   ///< initiator thread
{  
#if !INTC_CHKPT
  tlm::tlm_generic_payload *transaction_ptr;    ///< transaction pointer
#else
  // transaction_ptr is stored as part of the checkpoint
#endif
  std::ostringstream       msg;                 ///< log message
    

  while (true) 
  {
//=============================================================================
// Read FIFO to Get new transaction GP from the traffic generator 
//=============================================================================
#if INTC_CHKPT
    if (transaction_ptr == 0) {
      if (m_would_block)
        sc_core::wait(request_in_port->data_written_event());
#endif
    transaction_ptr = request_in_port->read();  // get request from input fifo
    
    msg.str("");

    m_delay = m_quantum_keeper.get_local_time();
    
    msg << "Initiator: " << m_ID               
        << " b_transport(GP, " 
        << m_delay << ")";
    REPORT_INFO(filename,  __FUNCTION__, msg.str());

#if INTC_CHKPT
    }
    if (!m_trans_sent) {
      initiator_socket->b_transport(*transaction_ptr, m_delay);
      m_trans_sent = true;
      assert (transaction_ptr->get_data_length() == 4);
      memcpy(m_data_buffer, transaction_ptr->get_data_ptr(), 4);
    }
#else
    initiator_socket->b_transport(*transaction_ptr, m_delay);
#endif
    
    gp_status = transaction_ptr->get_response_status();
    
    if(gp_status == tlm::TLM_OK_RESPONSE)
    {
       msg.str("");
       msg << "Initiator: " << m_ID               
          << " b_transport returned delay = " 
          << m_delay << " and quantum keeper to be set"
          << endl << "      ";
       REPORT_INFO(filename, __FUNCTION__, msg.str());
 
       m_quantum_keeper.set(m_delay);
       if(m_quantum_keeper.need_sync())
         {
           msg.str("");
           msg << "Initiator: " << m_ID               
               << " the quantum keeper needs synching";
           REPORT_INFO(filename, __FUNCTION__, msg.str());  
           
           m_quantum_keeper.sync();
           
           msg.str("");
           msg << "Initiator: " << m_ID               
               << " return from quantum keeper synch";
           REPORT_INFO(filename, __FUNCTION__, msg.str());
         }
    }
    else
    {
      msg << "Initiator: " << m_ID               
          << " Bad GP status returned = " << gp_status;
       REPORT_WARNING(filename,  __FUNCTION__, msg.str());
    }
    
    response_out_port->write(transaction_ptr);  // return txn to traffic gen
#if INTC_CHKPT
    m_trans_sent = false;
    transaction_ptr = 0;
    m_would_block = request_in_port->num_available() == 0;
#endif
  } // end while true
} // end initiator_thread 



