/*                                                              -*- C++ -*-

  © 2021 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

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

/*****************************************************************************

 * simple example of async events
 *
 * Copyright (C) 2017, GreenSocs Ltd.
 *
 * Developed by  Mark Burton mark@greensocs.com
 *
 *****************************************************************************/

#ifndef SIMPLE_ASYNC_TOP_H
#define SIMPLE_ASYNC_TOP_H

#include "systemc.h"
#include "async_event.h"

#if SC_CPLUSPLUS >= 201103L
// this version properly uses separate host threads.
// Without host threads, the example simply notifys the async event
// directly from the constructor
#include <thread>
#include <chrono>
#endif

SC_MODULE(watchDog)
{
  async_event when;
  bool barked;
public:
  SC_CTOR(watchDog) : barked(false)
  {
    SC_METHOD(call_stop);
    sensitive << when;
    dont_initialize();
  }

#if SC_CPLUSPLUS >= 201103L // C++11 threading support
  ~watchDog()
  {
    m_thread.join();
  }

private:
  std::thread m_thread;

  void start_of_simulation()
  {
    m_thread=std::thread( [this] { this->process(); } );
  }

  void process()
  {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    // asynchronous notification from a separate thread
    when.notify(sc_time(10,SC_NS));
  }
#else
  void start_of_simulation()
  {
    // no threading support, notifiy directly
    when.notify(sc_time(10,SC_NS));
  }
#endif // C++11 threading support

private:
  void call_stop()
  {
    std::ostringstream os;
    os << "Asked to stop at time " << sc_time_stamp();
    SC_REPORT_INFO("simple_async", os.str().c_str());
    barked=true;
    sc_stop();
  }

  void end_of_simulation()
  {
    sc_assert(barked==true);
    SC_REPORT_INFO("simple_async",
                   "The dog barks before the end of simulation");
  }
};

SC_MODULE(activity)
{
  SC_CTOR(activity)
  {
    SC_THREAD(busy);
  }

  void busy()
  {
    SC_REPORT_INFO("simple_async", "I'm busy!");
  }
};

#endif
