// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2015 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_SYSTEMC_LIBRARY_H
#define SIMICS_SYSTEMC_SYSTEMC_LIBRARY_H

// It must be placed before cc-api.h
#include <simics/systemc/class_decorator.h>

#include <simics/systemc/adapter.h>
#include <simics/systemc/awareness/init.h>
#if defined SIMICS_5_API || defined SIMICS_6_API
#include <simics/systemc/composite/pci_gasket.h>
#include <simics/systemc/composite/pci_gasket_class.h>
#include <simics/systemc/composite/pcie_gasket.h>
#include <simics/systemc/composite/pcie_gasket_class.h>
#endif
#include <simics/systemc/connector.h>
#include <simics/systemc/device.h>
#include <simics/systemc/iface/iface.h>
#include <simics/systemc/module_loaded.h>
#include <simics/systemc/sc_factory.h>
#include <simics/systemc/sc_signal_access_template.h>
#include <simics/systemc/simics2systemc/signal.h>
#include <simics/systemc/simics2systemc/signal_gasket_adapter.h>
#include <simics/systemc/systemc2simics/signal.h>
#include <simics/systemc/simics2tlm/simics2tlm.h>
#include <simics/systemc/tlm2simics/tlm2simics.h>

#endif
