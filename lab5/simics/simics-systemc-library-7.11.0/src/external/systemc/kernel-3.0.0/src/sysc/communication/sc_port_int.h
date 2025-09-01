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

/* Intel extensions need these structs to be available outside of sc_port.cpp */

namespace sc_core {

class sc_port_base;
        
// ----------------------------------------------------------------------------
//  STRUCT : sc_bind_elem
// ----------------------------------------------------------------------------

struct sc_bind_elem
{
    // constructors
    sc_bind_elem();
    explicit sc_bind_elem( sc_interface* interface_ );
    explicit sc_bind_elem( sc_port_base* parent_ );

    sc_interface* iface;
    sc_port_base* parent;
};

// ----------------------------------------------------------------------------
//  STRUCT : sc_bind_ef
// ----------------------------------------------------------------------------

struct sc_bind_ef
{
    // constructor
    sc_bind_ef( sc_process_b* , sc_event_finder* );

    // destructor
    ~sc_bind_ef();

    sc_process_b* handle;
    sc_event_finder* event_finder;
};

// ----------------------------------------------------------------------------
//  STRUCT : sc_bind_info
// ----------------------------------------------------------------------------

struct sc_bind_info
{
    typedef std::vector<sc_bind_ef*> ef_vector;

    // constructor
    explicit sc_bind_info( int max_size_, 
	sc_port_policy policy_=SC_ONE_OR_MORE_BOUND );

    // destructor
    ~sc_bind_info();

    int            max_size() const;
    sc_port_policy policy() const; 
    int            size() const;

    int                        m_max_size;
    sc_port_policy             m_policy;
    std::vector<sc_bind_elem*> vec;
    bool                       has_parent;
    int                        last_add;
    bool                       is_leaf;
    bool                       complete;

    ef_vector                  thread_vec;
    ef_vector                  method_vec;
};

} // namespace sc_core
