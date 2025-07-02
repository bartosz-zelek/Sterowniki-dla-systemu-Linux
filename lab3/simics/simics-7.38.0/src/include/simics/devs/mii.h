/*
  © 2010 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_DEVS_MII_H
#define SIMICS_DEVS_MII_H

#include <simics/base/types.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add id="mii_management_interface_t">
   <insert-until text="// ADD INTERFACE mii_management_interface_t"/>

   Interface that should be implemented by classes that represents one
   or multiple PHY's that have MII management interfaces.

   The <fun>read_register</fun> function should return the 16-bit value
   of the specified register. There are 32 registers numbered 0-31. The
   phy argument indicates the PHY number (0-31). Classes that represents
   one PHY can ignore this argument.
   The <fun>write_register</fun> function is called when a register is
   written.

   To support low-level bitwise accesses via MDIO and MDC pins, the
   function <fun>serial_access</fun> can be used. It is recommended to
   leave this function unimplemented and let an instance of
   <class>mii-management-bus</class> convert the low-level bit operations
   to higher level read/write register calls.
   The <fun>serial_access</fun> function takes as argument the MDIO and MDC
   pin levels on the master, and return the MDIO pin from the slave. Note
   that <class>mii-management-bus</class> also have <iface>signal</iface>
   interfaces for these pins.

   </add>
   <add id="mii_management_interface_exec_context">
   Cell Context for all methods.
   </add>
*/

SIM_INTERFACE(mii_management) {
        int    (*serial_access)(conf_object_t *obj, int data_in, int clock);
        uint16 (*read_register)(conf_object_t *obj, int phy, int reg);
        void   (*write_register)(conf_object_t *obj, int phy, int reg,
                                 uint16 value);
};
#define MII_MANAGEMENT_INTERFACE "mii_management"

// ADD INTERFACE mii_management_interface_t

/* <add id="mii_interface_t">
   <insert-until text="// ADD INTERFACE mii_interface_t"/>

   Obsolete interface that is implemented by some PHY's that support the MII
   management interface.

   It has the same methods as the <iface>mii_management</iface> interface,
   but does not pass along the PHY number.

   To support low-level bitwise accesses via MDIO and MDC pins, the
   function <fun>serial_access</fun> can be used. It is recommended to
   leave this function unimplemented and let an instance of
   <class>mii-management-bus</class> convert the low-level bit operations
   to higher level read/write register calls.

   </add>
   <add id="mii_interface_exec_context">
   Cell Context for all methods.
   </add>
*/

SIM_INTERFACE(mii) {
        int    (*serial_access)(conf_object_t *obj, int data_in, int clock);
        uint16 (*read_register)(conf_object_t *obj, int index);
        void   (*write_register)(conf_object_t *obj, int index, uint16 value);
};
#define MII_INTERFACE "mii"

// ADD INTERFACE mii_interface_t

/* <add id="mdio45_bus_interface_t">
   <insert-until text="// ADD INTERFACE mdio45_bus_interface_t"/>

   Interface that should be implemented by classes that either represent
   an MDIO bus, or multi PHY devices implementing the IEEE 802.3 clause 45
   MDIO management interface.

   The <fun>read_register</fun> function should return the 16-bit value of
   the specified register. <param>phy</param> specifies the PHY address (0-31),
   <param>mmd</param> specifies the MMD (MDIO manageable device) within the PHY
   (0-31) and <param>reg</param> specifies the register number within the MMD
   (0-65535).

   Devices modeling a single PHY should implement the <iface>mdio45_phy</iface>
   interface instead.

   </add>
   <add id="mdio45_bus_interface_exec_context">
   Cell Context for all methods.
   </add>
*/

SIM_INTERFACE(mdio45_bus) {
        uint16 (*read_register)(conf_object_t *obj, int phy, int mmd, int reg);
        void   (*write_register)(conf_object_t *obj, int phy, int mdd, int reg,
                                 uint16 value);

};
#define MDIO45_BUS_INTERFACE "mdio45_bus"

// ADD INTERFACE mdio45_bus_interface_t

/* <add id="mdio45_phy_interface_t">
   <insert-until text="// ADD INTERFACE mdio45_phy_interface_t"/>

   Interface that should be implemented by classes representing a single PHY
   implementing the IEEE 802.3 clause 45 MDIO management interface.

   The <fun>read_register</fun> function should return the 16-bit value of
   the specified register. <param>mmd</param> specifies the MMD
   (MDIO manageable device) within the PHY (0-31) and <param>reg</param>
   specifies the register number within the MMD (0-65535).

   Devices modeling either an MDIO bus, or multiple PHYs should implement
   the <iface>mdio45_bus</iface> interface instead.

   </add>
   <add id="mdio45_phy_interface_exec_context">
   Cell Context for all methods.
   </add>
*/

SIM_INTERFACE(mdio45_phy) {
        uint16 (*read_register)(conf_object_t *obj, int mmd, int reg);
        void   (*write_register)(conf_object_t *obj, int mmd, int reg,
                                 uint16 value);

};
#define MDIO45_PHY_INTERFACE "mdio45_phy"

// ADD INTERFACE mdio45_phy_interface_t


#if defined(__cplusplus)
}
#endif

#endif
