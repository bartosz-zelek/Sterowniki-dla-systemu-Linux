// -*- mode: C++; c-file-style: "virtutech-c++" -*-

// Filename: tlm2_base_protocol_checker.h

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

/*
Instantiate this checker module in-line between initiator and target, initiator and interconnect,
or interconnect and target by binding the target_socket and initiator_socket
Binding two checkers either side of an interconnect component, or interleaving a series of
checkers with interconnect components, will enable some deeper checks as against having just
a single checker

For example

  Initiator *initiator;
  Bus       *bus;
  Memory    *memory;
  ...
  initiator->socket.bind(bus->target_socket);
  bus->initiator_socket.bind(memory->socket);

might become

  tlm_utils::tlm2_base_protocol_checker<32> *checker1;
  tlm_utils::tlm2_base_protocol_checker<32> *checker2;
  ...
  initiator->socket.bind(checker1->target_socket);
  checker1->initiator_socket.bind(bus->target_socket);
  bus->initiator_socket.bind(checker2->target_socket);
  checker2->initiator_socket.bind(memory->socket);


GENERAL FEATURES OF THE BASE PROTOCOL CHECKER

The checks are relatively expensive, hence by default the number of checks is limited.
A maximum number can be set explicitly by calling set_num_checks(max)
Checking can be deactivated at any time by calling set_num_checks(0)
All checkers decrement a single global count, because having some checkers running and
others not can cause bogus violation reports
It is not permitted to turn checks on by calling set_num_checks() once checking has been
deactivated, because this could cause bogus violation reports

The DMI and debug checks are unaffected by the num_checks count (because they are cheap)

The error messages contain User Manual references

The checker is designed to be used with a transaction pool: otherwise it could consume
a lot of memory. The checker keeps a local copy of each transaction object
Failures are reported with a severity of SC_ERROR. The actions may be overridden by calling:
   sc_report_handler::set_actions("tlm2_protocol_checker", ...);

SPECIFIC CHECKS

nb_transport: phase sequence BEGIN_REQ -> END_REQ -> BEGIN_RESP -> END_RESP
Must not have two outstanding requests or responses (exclusion rules)
Must not have decreasing timing annotations on calls to or returns from nb_transport_fw/bw
Phase extensions permitted and ignored
Must not call b_transport during nb_transport phase sequence and vice-versa

nb_transport: memory manager must be set
nb_transport: reference count must be non-zero
First checker in BEGIN_REQ path should see ref_count == 1 (warning only)
An interconnect component that sets a memory manager should also clear it
An interconnect component that sets extensions with no memory manager should also clear them
(Does not bother with these memory manager checks for DMI and debug)

Transaction object must be properly initialized
Many generic payload attributes must not be modified during the transaction lifetime
Transaction object must not be re-allocated for a new transaction while still in use
DMI descriptor must be properly initialized
Debug transaction must be properly initialized
Debug byte count must be less than data_length

Checks that require multiple checkers to be instantiated along a transaction path:
The BEGIN_RESP path must be the reverse of the BEGIN_REQ path
Transaction object must not be sent with BEGIN_REQ while participating in a previous response
Interconnect component must not set response status attribute to TLM_OK_RESPONSE
Interconnect component must not modify data array on the response path

Generic payload option attribute (IEEE Std 1666-2011, SystemC 2.3.x)
gp_option must be properly initialized and only used for DMI and debug transport
When gp_option is used, other gp attributes must be initialized and used as per the transport interfaces
*/


// ******************** PREAMBLE ********************
#ifndef SIMICS_SYSTEMC_AWARENESS_TLM2_BASE_PROTOCOL_CHECKER_H
#define SIMICS_SYSTEMC_AWARENESS_TLM2_BASE_PROTOCOL_CHECKER_H

#include <simics/systemc/awareness/proxy_interface.h>

#include <systemc>
#include <tlm.h>

#include <deque>
#include <sstream>
#include <map>

namespace tlm_utils {


// ******************** CLASS DEFINITION ********************
class tlm2_base_protocol_checker {
  public:
    // Types used when building a trace of the transaction path
    typedef unsigned char uchar_t;
    typedef std::deque<sc_core::sc_object*> deque_t;

    explicit tlm2_base_protocol_checker(
            simics::systemc::awareness::ProxyInterface *proxy)
        : m_request_in_progress(0), m_response_in_progress(0),
          checker_instance_(proxy) {
    }

    virtual ~tlm2_base_protocol_checker() {}

    void b_transport_pre_checks( tlm::tlm_generic_payload& trans, sc_core::sc_time& delay);

    void b_transport_post_checks( tlm::tlm_generic_payload& trans, sc_core::sc_time& delay);

    void nb_transport_fw_pre_checks(
        tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& delay);

    void nb_transport_fw_post_checks(
        tlm::tlm_generic_payload& trans, tlm::tlm_phase& start_phase, tlm::tlm_phase& phase,
        sc_core::sc_time& delay, tlm::tlm_sync_enum status);

    void nb_transport_bw_pre_checks(
        tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& delay);

    void nb_transport_bw_post_checks(
        tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& delay,
        tlm::tlm_sync_enum status);

    void nb_transport_response_checks(
        tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& delay,
        const char* txt2, const char* txt3, const char* txt4);

    void check_initial_state(       tlm::tlm_generic_payload& trans, const char* txt2 );
    void check_trans_not_modified(  tlm::tlm_generic_payload& trans, const char* txt2 );
    void check_response_path(       tlm::tlm_generic_payload& trans, const char* txt2 );
    void remember_gp_option(        tlm::tlm_generic_payload& trans );

    void get_direct_mem_ptr_pre_checks( tlm::tlm_generic_payload& trans, tlm::tlm_dmi& dmi_data );

    void get_direct_mem_ptr_post_checks( tlm::tlm_generic_payload& trans, tlm::tlm_dmi& dmi_data );

    void transport_dbg_pre_checks( tlm::tlm_generic_payload& trans );

    void transport_dbg_post_checks( tlm::tlm_generic_payload& trans, unsigned int count );

    void tlm2error( tlm::tlm_generic_payload& trans, const char* ref, bool warning = false  );

  private:

    virtual void reportError(std::ostringstream *stream, bool = false);

    struct state_t {
      state_t() { has_mm = false; b_call = 0; ph = tlm::UNINITIALIZED_PHASE;
                  gp = 0; data_ptr = 0; byte_enable_ptr = 0; }

      bool                      has_mm;
      unsigned int              b_call;    // Number of b_transport calls in progress
      tlm::tlm_phase            ph;
      sc_core::sc_time          time;      // Current time + annotated delay
      tlm::tlm_generic_payload* gp;        // Points to new data and byte enable buffers
      uchar_t*                  data_ptr;  // Stores original pointers
      uchar_t*                  byte_enable_ptr;
    };

    // Transaction state for the specific hop where this checker is inlined
    std::map<tlm::tlm_generic_payload*, state_t> m_map;

    // Flags for exclusion rules
    tlm::tlm_generic_payload* m_request_in_progress;
    tlm::tlm_generic_payload* m_response_in_progress;

    std::ostringstream txt;

    struct path_t {
      path_t () { response_in_progress = false; ok_response = false; resp_data_ptr = 0; }

      bool      response_in_progress;
      bool      ok_response;
      deque_t   path;
      uchar_t*  resp_data_ptr;  // Copy of data on response path
    };


    // Global variable used for checks involving multiple checkers along a transaction path
    static std::map<tlm::tlm_generic_payload*, path_t> shared_map;

    simics::systemc::awareness::ProxyInterface *checker_instance_;
};

}  // namespace tlm_utils

#endif
