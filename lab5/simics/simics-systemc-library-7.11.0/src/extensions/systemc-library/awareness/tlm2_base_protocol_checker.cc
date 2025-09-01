// -*- mode: C++; c-file-style: "virtutech-c++" -*-

//----------------------------------------------------------------------
//  Copyright (c) 2008-2013 by Doulos Ltd.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//----------------------------------------------------------------------

// Author: John Aynsley, Doulos

// Version  1, 11 July 2008
// Version  2, 16 July 2008  Only generate ref_count > 1 warning from 1st checker of path
// Version  3, 17 July 2008  Support compilation under SystemC 2.1.v1
// Version  4, 12 Aug  2008  Add header #include <map>
// Version  5, 08 Sep  2008  Fix bugs in message text
// Version  6, 01 Aug  2010  Update messages to refer to OSCI TLM-2.0 LRM of July 2009
// Version  7, 25 Oct  2011  Minor bug fix for certain compilers: replace u_char with uchar_t
// Version  8, 02 Nov  2011  Support the endianness conversion functions by excluding the
//                           tlm_endian_context extension from the protocol checks
// Version  9, 17 Aug  2012  Fix LRM reference on line 805 (should be 8.2.11 a) [NOT YET RELEASED]
// Version 10,  3 Jan  2013  Updated messages to refer to IEEE Std 1666-2011, the combined SystemC + TLM-2.0 LRM
//                           Added checks related to the generic payload option attribute
// Version 11, 14 Mar  2016  Fix minor bug - start_phase should be a copy, not a reference

// TLM-2.0 Base Protocol Compliance Checker


#include <simics/systemc/awareness/tlm2_base_protocol_checker.h>
#include <simics/systemc/iface/transaction_extension.h>
#include <simics/systemc/internals.h>

using std::cout;
using std::endl;
using std::dec;
using std::hex;

#include <sstream>
#include <map>

namespace tlm_utils {

std::map<tlm::tlm_generic_payload*, tlm2_base_protocol_checker::path_t>
    tlm2_base_protocol_checker::shared_map;

// ******************** MEMBER FUNCTION DEFINITIONS ********************
#define BOILERPLATE \
        void tlm2_base_protocol_checker::



BOILERPLATE
b_transport_pre_checks(
    tlm::tlm_generic_payload& trans, sc_core::sc_time& delay)
{
  ++ m_map[&trans].b_call;

  if ( trans.has_mm() && trans.get_ref_count() == 0)
  {
    txt << "Transaction passed to b_transport with memory manager and reference count of 0";
    tlm2error(trans, "14.5 t)");
  }
  check_initial_state(trans, "b_transport");

#if !(defined SYSTEMC_VERSION & SYSTEMC_VERSION <= 20050714)
  if (sc_core::sc_get_current_process_handle().proc_kind() == sc_core::SC_METHOD_PROC_)
  {
    txt << "b_transport called from method process";
    tlm2error(trans, "11.1.1.4 b)");
  }
#endif

  if (m_map[&trans].ph > 0 && m_map[&trans].ph < 4)
  {
    txt << "b_transport called during a sequence of nb_transport calls";
    tlm2error(trans, "15.2.10 c)");
  }
}


BOILERPLATE
b_transport_post_checks(
    tlm::tlm_generic_payload& trans, sc_core::sc_time& delay)
{
  check_response_path(trans, "b_transport");
  check_trans_not_modified(trans, "b_transport");
  -- m_map[&trans].b_call;
}


BOILERPLATE
nb_transport_fw_pre_checks(
    tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& delay)
{
  if ( !trans.has_mm() )
  {
    txt << "Transaction passed to nb_transport_fw with no memory manager set";
    tlm2error(trans, "14.5 i)");
  }
  if ( trans.get_ref_count() == 0)
  {
    txt << "Transaction passed to nb_transport_fw with reference count of 0";
    tlm2error(trans, "14.5 t)");
  }

  switch (phase)
  {
    case tlm::BEGIN_REQ:
      check_initial_state(trans, "nb_transport_fw");

      if (m_map[&trans].ph > 0 &&  m_map[&trans].ph < 4) // END_RESP -> BEGIN_REQ is legal
      {
        txt << "Phase " << phase << " sent out-of-sequence on forward path, detected in nb_transport_fw";
        tlm2error(trans, "15.2.4");
      }

      if (m_request_in_progress)
      {
        txt << "Transaction violates BEGIN_REQ exclusion rule, detected in nb_transport_fw";
        tlm2error(trans, "15.2.6 e)");
      }
      m_request_in_progress = &trans;

      if (m_map[&trans].b_call)
      {
        txt << "nb_transport_fw called during a b_transport call";
        tlm2error(trans, "15.2.10 c)");
      }
      break;

    case tlm::END_REQ:
    case tlm::BEGIN_RESP:
    case tlm::UNINITIALIZED_PHASE:
      txt << "Phase " << phase << " sent on forward path, detected in nb_transport_fw";
      tlm2error(trans, " 15.2.3 c)");
      break;

    case tlm::END_RESP:
      if (m_map[&trans].ph != tlm::BEGIN_RESP)
      {
        txt << "Phase " << phase << " sent out-of-sequence on forward path, detected in nb_transport_fw";
        tlm2error(trans, "15.2.4");
      }
      m_response_in_progress = 0;
      break;
  }

  if (phase < 5)  // Ignore extended phases
    m_map[&trans].ph = phase;

  if (sc_core::sc_time_stamp() + delay < m_map[&trans].time)
  {
    txt << "nb_transport_fw called with decreasing timing annotation:"
        << " delay = " << delay
        << ", sc_time_stamp() + delay from previous call = " << m_map[&trans].time;
    tlm2error(trans, "15.2.7 c)");
  }
  m_map[&trans].time = sc_core::sc_time_stamp() + delay;
}


BOILERPLATE
nb_transport_fw_post_checks(
    tlm::tlm_generic_payload& trans, tlm::tlm_phase& start_phase, tlm::tlm_phase& phase,
    sc_core::sc_time& delay, tlm::tlm_sync_enum status)
{
  if (status == tlm::TLM_UPDATED)
  {
    nb_transport_response_checks(
        trans, phase, delay, "(forward) return", "Return from nb_transport_fw", "nb_transport_fw");
  }
  else if (status == tlm::TLM_COMPLETED)
  {
    if (start_phase == tlm::BEGIN_REQ)
      check_response_path(trans, "nb_transport_fw");
    m_request_in_progress = 0;
    m_map[&trans].ph = tlm::UNINITIALIZED_PHASE;
  }

  // Transaction object should not be re-allocated, even during the END_RESP phase
  //if (phase != tlm::END_RESP)
  {
    std::ostringstream txt;
    txt << "nb_transport_fw, phase = " << phase;
    check_trans_not_modified(trans, txt.str().c_str());
  }
}


BOILERPLATE
nb_transport_bw_pre_checks(
    tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& delay)
{
  if ( !trans.has_mm() )
  {
    txt << "Transaction passed to nb_transport_bw with no memory manager set";
    tlm2error(trans, "14.5 i)");
  }
  if ( trans.get_ref_count() == 0)
  {
    txt << "Transaction passed to nb_transport_bw with reference count of 0";
    tlm2error(trans, "14.5 t)");
  }
  nb_transport_response_checks(
      trans, phase, delay, "backward", "nb_transport_bw called", "nb_transport_bw");
}


BOILERPLATE
nb_transport_bw_post_checks(
    tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& delay,
    tlm::tlm_sync_enum status)
{
  if (status == tlm::TLM_UPDATED)
  {
    switch (phase)
    {
      case tlm::BEGIN_REQ:
        txt << "Phase " << phase << " sent out-of-sequence on (backward) return path, detected in nb_transport_bw";
        tlm2error(trans, "15.2.4");
        break;

      case tlm::END_REQ:
      case tlm::BEGIN_RESP:
      case tlm::UNINITIALIZED_PHASE:
        txt << "Phase " << phase << " sent on (backward) return path, detected in nb_transport_bw";
        tlm2error(trans, "15.2.3 c)");
        break;

      case tlm::END_RESP:
        if (m_map[&trans].ph != tlm::BEGIN_RESP)
        {
          txt << "Phase " << phase << " sent out-of-sequence on (backward) return path, detected in nb_transport_bw";
          tlm2error(trans, "15.2.4");
        }

        m_response_in_progress = 0;
        break;
    }

    if (phase < 5)  // Ignore extended phases
      m_map[&trans].ph = phase;

    if (sc_core::sc_time_stamp() + delay < m_map[&trans].time)
    {
      txt << "Return from nb_transport_bw with decreasing timing annotation:"
          << " delay = " << delay
          << ", sc_time_stamp() + delay from previous call = " << m_map[&trans].time;
      tlm2error(trans, "15.2.7 c)");
    }
    m_map[&trans].time = sc_core::sc_time_stamp() + delay;
  }
  else if (status == tlm::TLM_COMPLETED)
  {
    m_response_in_progress = 0;
    m_map[&trans].ph = tlm::UNINITIALIZED_PHASE;
  }

  // Transaction object should not be re-allocated, even during the END_RESP phase
  //if (phase != tlm::END_RESP)
  {
    std::ostringstream txt;
    txt << "nb_transport_bw, phase = " << phase;
    check_trans_not_modified(trans, txt.str().c_str());
  }
}


BOILERPLATE
nb_transport_response_checks(
    tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& delay,
    const char* txt2, const char* txt3, const char* txt4)
{
  if (trans.is_response_ok())
    if (shared_map[&trans].response_in_progress && !shared_map[&trans].ok_response)
    {
      txt << "Interconnect component sets response status attribute to TLM_OK_RESPONSE"
          << ", detected in " << txt4;
      tlm2error(trans, "14.7");

    }

  switch (phase)
  {
    case tlm::BEGIN_REQ:
    case tlm::END_RESP:
    case tlm::UNINITIALIZED_PHASE:
      txt << "Phase " << phase << " sent on " << txt2 << " path"
          << ", detected in " << txt4;
      tlm2error(trans, "15.2.3 c)");
      break;

    case tlm::END_REQ:
      if (m_map[&trans].ph != tlm::BEGIN_REQ)
      {
        txt << "Phase " << phase << " sent out-of-sequence on " << txt2 << " path"
            << ", detected in " << txt4;
        tlm2error(trans, "15.2.4");
      }

      m_request_in_progress = 0;
      break;

    case tlm::BEGIN_RESP:
      if (m_map[&trans].ph != tlm::BEGIN_REQ && m_map[&trans].ph != tlm::END_REQ)
      {
        txt << "Phase " << phase << " sent out-of-sequence on " << txt2 << " path"
            << ", detected in " << txt4;
        tlm2error(trans, "15.2.4");
      }

      if (&trans == m_request_in_progress)
        m_request_in_progress = 0;

      if (m_response_in_progress)
      {
        txt << "Transaction violates BEGIN_RESP exclusion rule"
            << ", detected in " << txt4;
        tlm2error(trans, "15.2.6 f)");
      }
      m_response_in_progress = &trans;

      check_response_path(trans, txt4);
      break;
  }

  if (phase < 5)  // Ignore extended phases
    m_map[&trans].ph = phase;

  if (sc_core::sc_time_stamp() + delay < m_map[&trans].time)
  {
    txt << txt3 << " with decreasing timing annotation:"
        << " delay = " << delay
        << ", sc_time_stamp() + delay from previous call = " << m_map[&trans].time;
    tlm2error(trans, "15.2.7 c)");
  }
  m_map[&trans].time = sc_core::sc_time_stamp() + delay;
}


BOILERPLATE
check_initial_state(
    tlm::tlm_generic_payload& trans, const char* txt2 )
{
  if ( trans.has_mm() && trans.get_ref_count() > 1 && shared_map[&trans].path.empty() )
  {
    // In Simics we first check for the existence of the TransactionExtension
    // to determine if a payload originates from a simics2tlm gasket. Such
    // payloads are expected to have a ref count > 1 because the SystemC thread
    // increases the ref count to avoid that the payload is returned to the
    // pool if the gasket call returns for the async case but the transaction
    // is completed later.
    if (!trans.get_extension<simics::systemc::iface::TransactionExtension>()) {
    txt << "New transaction passed to " << txt2 << " with reference count = "
        << trans.get_ref_count();
    tlm2error(trans, "14.5 t)", true);
    }
  }
  if (trans.get_data_ptr() == 0 && trans.get_command() != tlm::TLM_IGNORE_COMMAND)
  {
    txt << "Transaction not properly initialized: data_ptr == 0, detected in " << txt2;
    tlm2error(trans, "14.11 e)");
  }
  if (trans.get_data_length() == 0 && trans.get_command() != tlm::TLM_IGNORE_COMMAND)
  {
    txt << "Transaction not properly initialized: data_length == 0, detected in " << txt2;
    tlm2error(trans, "14.12 d)");
  }
  if (trans.get_byte_enable_ptr() != 0 && trans.get_byte_enable_length() == 0)
  {
    txt << "Transaction not properly initialized: "
        << "byte_enable_ptr != 0 and byte_enable_length == 0, detected in " << txt2;
    tlm2error(trans, "14.14 f)");
  }
  if (trans.get_streaming_width() == 0)
  {
    txt << "Transaction not properly initialized: streaming_width == 0, detected in " << txt2;
    tlm2error(trans, "14.15 f)");
  }
  if (trans.is_dmi_allowed())
  {
    txt << "Transaction not properly initialized: dmi_allowed == true, detected in " << txt2;
    tlm2error(trans, "14.16");
  }
  if (trans.get_response_status() != tlm::TLM_INCOMPLETE_RESPONSE)
  {
    txt << "Transaction not properly initialized: response_status != TLM_INCOMPLETE_RESPONSE, detected in " << txt2;
    tlm2error(trans, "14.17 e)");
  }
  if (trans.get_gp_option() != tlm::TLM_MIN_PAYLOAD)
  {
    txt << "Transaction not properly initialized: gp_option != TLM_MIN_PAYLOAD, detected in " << txt2;
    tlm2error(trans, "14.8 g)");
  }

  // Setup clones of transaction and buffers in map
  tlm::tlm_generic_payload* gp = m_map[&trans].gp;
  if (gp == 0)
    gp = new tlm::tlm_generic_payload;  // Memory leak: transactions are never cleared from map
  else
  {
    delete [] gp->get_data_ptr();
    gp->free_all_extensions();
  }
  gp->set_data_ptr( new uchar_t[trans.get_data_length()] );
  m_map[&trans].data_ptr = trans.get_data_ptr();

  if (gp->get_byte_enable_ptr())
    delete [] gp->get_byte_enable_ptr();
  if (trans.get_byte_enable_ptr())
    gp->set_byte_enable_ptr( new uchar_t[trans.get_byte_enable_length()] );
  else
    gp->set_byte_enable_ptr(0);
  m_map[&trans].byte_enable_ptr = trans.get_byte_enable_ptr();

  gp->deep_copy_from(trans);
  m_map[&trans].gp = gp;
  m_map[&trans].time = sc_core::SC_ZERO_TIME;
  m_map[&trans].has_mm = trans.has_mm();

  // Store request path checker sequence
  if (shared_map[&trans].resp_data_ptr)
  {
    delete [] shared_map[&trans].resp_data_ptr;
    shared_map[&trans].resp_data_ptr = 0;
  }
  if (shared_map[&trans].response_in_progress)
  {
    txt << "Transaction object sent with BEGIN_REQ while still being used on a previous response path, detected in " << txt2;
    tlm2error(trans, "14.5 x)");
  }
  shared_map[&trans].ok_response = false;
  shared_map[&trans].path.push_back(checker_instance_->systemc_obj());
}


BOILERPLATE
remember_gp_option(
    tlm::tlm_generic_payload& trans)
{
  // Setup clone of transaction in map in order to check gp_option only
  tlm::tlm_generic_payload* gp = m_map[&trans].gp;
  if (gp == 0)
    gp = new tlm::tlm_generic_payload;  // Memory leak: transactions are never cleared from map
  gp->set_gp_option( trans.get_gp_option() );
  m_map[&trans].gp = gp;
}


BOILERPLATE
check_trans_not_modified(
    tlm::tlm_generic_payload& trans, const char* txt2 )
{
  tlm::tlm_generic_payload* init = m_map[&trans].gp;
  if (!init)
      return;

  if (trans.get_command() != init->get_command())
  {
    txt << "Command attribute modified during transaction lifetime, detected in " << txt2;
    tlm2error(trans, "14.7");
  }
  if (trans.get_data_ptr() != m_map[&trans].data_ptr)
  {
    txt << "Data pointer attribute modified during transaction lifetime, detected in " << txt2;
    tlm2error(trans, "14.7");
  }
  if (trans.get_data_length() != init->get_data_length())
  {
    txt << "Data length attribute modified during transaction lifetime, detected in " << txt2;
    tlm2error(trans, "14.7");
  }
  if (trans.get_command() == tlm::TLM_WRITE_COMMAND)
    for (unsigned int i = 0; i < init->get_data_length(); i++)
      if (trans.get_data_ptr()[i] != init->get_data_ptr()[i])
      {
        txt << "Data array modified during transaction lifetime, detected in " << txt2;
        tlm2error(trans, "14.7");
      }
  if (trans.get_byte_enable_ptr() != m_map[&trans].byte_enable_ptr)
  {
    txt << "Byte enable pointer attribute modified during transaction lifetime, detected in " << txt2;
    tlm2error(trans, "14.7");
  }
  if (trans.get_byte_enable_length() != init->get_byte_enable_length())
  {
    txt << "Byte enable length attribute modified during transaction lifetime, detected in " << txt2;
    tlm2error(trans, "14.7");
  }
  if (trans.get_byte_enable_ptr())
    for (unsigned int i = 0; i < init->get_byte_enable_length(); i++)
      if (trans.get_byte_enable_ptr()[i] != init->get_byte_enable_ptr()[i])
      {
        txt << "Byte enable array modified during transaction lifetime, detected in " << txt2;
        tlm2error(trans, "14.7");
      }
  if (trans.get_streaming_width() != init->get_streaming_width())
  {
    txt << "Streaming width attribute modified during transaction lifetime, detected in " << txt2;
    tlm2error(trans, "14.7");
  }
  if (init->get_gp_option() == tlm::TLM_MIN_PAYLOAD && trans.get_gp_option() != tlm::TLM_MIN_PAYLOAD)
  {
    txt << "Generic payload option attribute modified during transaction lifetime, detected in " << txt2;
    tlm2error(trans, "14.8 g)");
  }
  if ( !m_map[&trans].has_mm )
  {
    if (trans.has_mm())
    {
      txt << "Interconnect component sets a memory manager, but does not clear it on return, detected in " << txt2;
      tlm2error(trans, "14.5 aa)");
    }

    for (unsigned int i = 0; i < tlm::max_num_extensions(); i++)
      // Exclude tlm_endian_context extension from the check because it is not cloned in m_map
      if (i != tlm::tlm_endian_context::ID)
        if (trans.get_extension(i))
          if ( !m_map[&trans].gp->get_extension(i) )
          {
            txt << "Extension set (index = " << i << ") without also being deleted in the absence of a memory manager, detected in " << txt2;
            tlm2error(trans, "14.5 aa)");
          }
  }

  uchar_t* resp_data_ptr = shared_map[&trans].resp_data_ptr;
  if (resp_data_ptr)
    for (unsigned int i = 0; i < trans.get_data_length(); i++)
      if (trans.get_data_ptr()[i] != resp_data_ptr[i])
      {
        txt << "Transaction data array modified in interconnect component on response path, detected in " << txt2;
        tlm2error(trans, "14.7");
      }
}


BOILERPLATE
check_response_path(
    tlm::tlm_generic_payload& trans, const char* txt2 )
{
  if ( !shared_map[&trans].path.empty() )
  {
    if ( checker_instance_->systemc_obj() != shared_map[&trans].path.back() )
    {
      txt << "BEGIN_RESP path is not the reverse of the BEGIN_REQ path.";
      txt << "\nBEGIN_REQ path includes these sockets: -> ";
      deque_t path = shared_map[&trans].path;
      for (deque_t::iterator i = path.begin(); i < path.end(); i++) {
        if (i != path.begin())
          txt << " -> ";
        txt << (*i)->name();
      }
      txt << "\nDetected in " << txt2;
      tlm2error(trans, "15.2.11 a)");
    }
    shared_map[&trans].path.pop_back();
    shared_map[&trans].response_in_progress = !shared_map[&trans].path.empty();
    shared_map[&trans].ok_response = trans.is_response_ok();

    // Create a copy of the data array for comparison on the response path
    if ( !shared_map[&trans].resp_data_ptr  && trans.get_data_ptr())
    {
      shared_map[&trans].resp_data_ptr = new uchar_t[trans.get_data_length()];
      memcpy(shared_map[&trans].resp_data_ptr, trans.get_data_ptr(), trans.get_data_length());
    }
  }
}


BOILERPLATE
get_direct_mem_ptr_pre_checks(
    tlm::tlm_generic_payload& trans, tlm::tlm_dmi& dmi_data )
{
  remember_gp_option(trans);

  if (dmi_data.get_dmi_ptr() != 0)
  {
    txt << "DMI descriptor not properly initialized: dmi_ptr != 0";
    tlm2error(trans, "11.2.5 f)");
  }
  if (!dmi_data.is_none_allowed())
  {
    txt << "DMI descriptor not properly initialized: granted_access != DMI_ACCESS_NONE";
    tlm2error(trans, "11.2.5 a)");
  }
  if (dmi_data.get_start_address() != 0)
  {
    txt << "DMI descriptor not properly initialized: start_address != 0";
    tlm2error(trans, "11.2.5 u)");
  }
  if (dmi_data.get_end_address() != (sc_dt::uint64)(-1))
  {
    txt << "DMI descriptor not properly initialized: end_address != 0";
    tlm2error(trans, "11.2.5 u)");
  }
  if (dmi_data.get_read_latency() != sc_core::SC_ZERO_TIME)
  {
    txt << "DMI descriptor not properly initialized: read_latency != SC_ZERO_TIME";
    tlm2error(trans, "11.2.5 ac)");
  }
  if (dmi_data.get_write_latency() != sc_core::SC_ZERO_TIME)
  {
    txt << "DMI descriptor not properly initialized: write_latency != SC_ZERO_TIME";
    tlm2error(trans, "11.2.5 ac)");
  }

  if (trans.get_gp_option() == tlm::TLM_FULL_PAYLOAD)
  {
    /*
    if (trans.is_dmi_allowed())  // Would be rather brutal to flag dmi_allowed as an error for a DMI transaction!
    {
      txt << "DMI transaction not properly initialized: dmi_allowed == true";
      tlm2error(trans, "14.8 e) & 14.16");
    }
  */
    if (trans.get_response_status() != tlm::TLM_INCOMPLETE_RESPONSE)
    {
      txt << "DMI transaction not properly initialized: response_status != TLM_INCOMPLETE_RESPONSE";
      tlm2error(trans, "14.8 e) & 14.17 e)");
    }
  }
  else if (trans.get_gp_option() == tlm::TLM_FULL_PAYLOAD_ACCEPTED)
  {
    txt << "DMI transaction not properly initialized: gp_option == TLM_FULL_PAYLOAD_ACCEPTED";
    tlm2error(trans, "14.8 c) & e) & j)");
  }
}


BOILERPLATE
get_direct_mem_ptr_post_checks( tlm::tlm_generic_payload& trans, tlm::tlm_dmi& dmi_data )
{
  tlm::tlm_generic_payload* init = m_map[&trans].gp;

  if (init->get_gp_option() == tlm::TLM_MIN_PAYLOAD && trans.get_gp_option() != tlm::TLM_MIN_PAYLOAD)
  {
    txt << "DMI transaction gp_option attribute value TLM_MIN_PAYLOAD modified during transaction lifetime";
    tlm2error(trans, "14.8 h)");
  }
  else if (init->get_gp_option() == tlm::TLM_FULL_PAYLOAD && trans.get_gp_option() == tlm::TLM_MIN_PAYLOAD)
  {
    txt << "DMI transaction gp_option attribute value changed from TLM_FULL_PAYLOAD to TLM_MIN_PAYLOAD";
    tlm2error(trans, "14.8 j)");
  }
}


BOILERPLATE
transport_dbg_pre_checks( tlm::tlm_generic_payload& trans )
{
  remember_gp_option(trans);

  if (trans.get_data_length() > 0 && trans.get_data_ptr() == 0)
  {
    txt << "Debug transaction has data_ptr == 0";
    tlm2error(trans, "11.3.4 l)");
  }

  if (trans.get_gp_option() == tlm::TLM_FULL_PAYLOAD)
  {
    if (trans.get_byte_enable_ptr() != 0 && trans.get_byte_enable_length() == 0)
    {
      txt << "Debug transaction not properly initialized: "
          << "byte_enable_ptr != 0 and byte_enable_length == 0";
      tlm2error(trans, "14.8 f) & 14.14 f)");
    }
    if (trans.get_streaming_width() == 0)
    {
      txt << "Debug transaction not properly initialized: streaming_width == 0";
      tlm2error(trans, "14.8 f) & 14.15 f)");
    }
    if (trans.is_dmi_allowed())
    {
      txt << "Debug transaction not properly initialized: dmi_allowed == true";
      tlm2error(trans, "14.8 f) & 14.16");
    }
    if (trans.get_response_status() != tlm::TLM_INCOMPLETE_RESPONSE)
    {
      txt << "Debug transaction not properly initialized: response_status != TLM_INCOMPLETE_RESPONSE";
      tlm2error(trans, "14.8 f) & 14.17 e)");
    }
  }
  else if (trans.get_gp_option() == tlm::TLM_FULL_PAYLOAD_ACCEPTED)
  {
    txt << "Debug transaction not properly initialized: gp_option == TLM_FULL_PAYLOAD_ACCEPTED";
    tlm2error(trans, "14.8 c) & f) & l)");
  }}


BOILERPLATE
transport_dbg_post_checks( tlm::tlm_generic_payload& trans, unsigned int count )
{
  tlm::tlm_generic_payload* init = m_map[&trans].gp;

  if (trans.get_data_length() > 0 && trans.get_data_ptr() == 0)
  {
    txt << "Debug transaction has data_ptr == 0";
    tlm2error(trans, "11.3.4 l)");
  }
  if (count > trans.get_data_length())
  {
    txt << "Count returned from transport_dbg is greater than data_length";
    tlm2error(trans, "11.3.4 s)");
  }

  if (init->get_gp_option() == tlm::TLM_MIN_PAYLOAD && trans.get_gp_option() != tlm::TLM_MIN_PAYLOAD)
  {
    txt << "Debug transaction gp_option attribute value TLM_MIN_PAYLOAD modified during transaction lifetime";
    tlm2error(trans, "14.8 h)");
  }
  else if (init->get_gp_option() == tlm::TLM_FULL_PAYLOAD && trans.get_gp_option() == tlm::TLM_MIN_PAYLOAD)
  {
    txt << "Debug transaction gp_option attribute value changed from TLM_FULL_PAYLOAD to TLM_MIN_PAYLOAD";
    tlm2error(trans, "14.8 l)");
  }}


BOILERPLATE
tlm2error( tlm::tlm_generic_payload& trans, const char* ref, bool warning )
{
  txt << "\n\nRefer to IEEE Std 1666-2011, clause " << ref;
  txt << "\n\nChecker instance: " << checker_instance_->systemc_obj()->name();
  txt << "\n\nTransaction details:";
  txt << "\n  has_mm             = " << dec << trans.has_mm() << " (bool)";
  txt << "\n  ref_count          = " << dec << trans.get_ref_count() << " (int)";
  txt << "\n\n  gp_option          = " <<
      (trans.get_gp_option() == tlm::TLM_MIN_PAYLOAD  ? "TLM_MIN_PAYLOAD"
    :trans.get_gp_option() == tlm::TLM_FULL_PAYLOAD ? "TLM_FULL_PAYLOAD"
                                                    : "TLM_FULL_PAYLOAD_ACCEPTED");
  txt << "\n  command            = " <<
     (trans.get_command() == tlm::TLM_READ_COMMAND  ? "TLM_READ_COMMAND"
     :trans.get_command() == tlm::TLM_WRITE_COMMAND ? "TLM_WRITE_COMMAND"
                                                    : "TLM_IGNORE_COMMAND");
  txt << "\n  address            = " << hex << trans.get_address() << " (hex)";
  txt << "\n  data_ptr           = " << hex
      << reinterpret_cast<int*>(trans.get_data_ptr()) << " (hex)";
  txt << "\n  data_length        = " << hex << trans.get_data_length() << " (hex)";
  txt << "\n  streaming_width    = " << hex << trans.get_streaming_width() << " (hex)";
  txt << "\n  byte_enable_ptr    = " << hex
      << reinterpret_cast<int*>(trans.get_byte_enable_ptr()) << " (hex)";
  txt << "\n  byte_enable_length = " << hex << trans.get_byte_enable_length() << " (hex)";
  txt << "\n  dmi_allowed        = " << dec << trans.is_dmi_allowed() << " (bool)";
  txt << "\n  response_status    = " << trans.get_response_string();

  bool extensions_present = false;
  for (unsigned int i = 0; i < tlm::max_num_extensions(); i++)
  {
    tlm::tlm_extension_base* ext = trans.get_extension(i);
    if (ext)
    {
      if (!extensions_present)
        txt << "\n\n  extensions:";
      txt << "\n    index = " << i << "   type = " << typeid(*ext).name();
      extensions_present = true;
    }
  }

  txt << "\n\n";
  reportError(&txt, warning);
}

BOILERPLATE
reportError(std::ostringstream *txt, bool asWarning) {
    if (asWarning)
        SIM_LOG_INFO(1, checker_instance_->simics_obj(), 0,
                     "\n  %s", txt->str().c_str());
    else
        SIM_LOG_SPEC_VIOLATION(1, checker_instance_->simics_obj(), 0,
                               "\n  %s", txt->str().c_str());
    txt->str("");
    txt->clear();
}

}  // namespace tlm_utils
