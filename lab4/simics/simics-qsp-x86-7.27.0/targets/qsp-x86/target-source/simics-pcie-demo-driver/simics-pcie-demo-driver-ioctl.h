/* This Software is part of Simics. The rights to copy, distribute,
   modify, or otherwise make use of this Software may be licensed only
   pursuant to the terms of an applicable license agreement.

   Copyright (C) 2010-2021 Intel Corporation */

#ifndef SIMICS_PCIE_DRIVER_IOCTL_H
#define SIMICS_PCIE_DRIVER_IOCTL_H

#include <linux/ioctl.h>

#define device_type 's'

// Define commands to enable and disable interrupts
#define INTR_DISABLE _IO(device_type,1)
#define INTR_ENABLE  _IO(device_type,2)

#endif
