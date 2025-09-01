/*
  x86ex_interface.h - a collection of IA-32 CPU internal interfaces.

  © 2010 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef X86EX_INTERFACE_H
#define X86EX_INTERFACE_H

#include <simics/base/types.h>
#include <simics/device-api.h>
#include <simics/pywrap.h>
#include <simics/devs/apic.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Enumeration of messages sent from CPU to TXT chipset */
typedef enum {
        Txt_Msg_Senter = 0,
        Txt_Msg_Senter_Ack = 1,
        Txt_Msg_Senter_Cont = 2,
        Txt_Msg_Sexit = 3,
        Txt_Msg_Sexit_Ack = 4,
        Txt_Msg_Wakeup = 5,
        Txt_Msg_Processor_Hold = 6,
        Txt_Msg_Open_Private = 7,
        Txt_Msg_Open_Locality3 = 8,
        Txt_Msg_Close_Locality3 = 9,
        Txt_Msg_Lock_Smram = 10,
        Txt_Msg_Processor_Release = 11,
        Txt_Msg_Unlock_Smram = 12,
        Txt_Msg_Sexit_Continue = 13,
        Txt_Msg_Close_Private = 14
} txt_msg_t;

/* <add id="x86_txt_chipset_interface">
    This interface is meant to be implemented by Intel&reg; Trusted Execution
    Technology (Intel&reg; TXT) chipset. Refer
    to the "Intel&reg; Trusted Execution Technology Software Development Guide
    Measured Launched Environment Developer's Guide" document for explanation
    of interactions between CPU and Intel&reg; TXT chipset.

    This interface is internal and may change without notice.

    The <b>signal_txt_msg</b> method is to send TXT-specific message to
    chipset.  The <b>get_platform_feature</b> method is to query the chipset
    for supported Intel&reg; TXT features.
    The <b>get_platform_cpus</b> method returns a list of all logical processors
    connected to the chipset.

    <insert-until text="// ADD INTERFACE x86_txt_chipset_interface"/>
    </add>
    <add id="x86_txt_chipset_interface_exec_context">
    Cell Context for all methods.
    </add> */
SIM_INTERFACE(x86_txt_chipset) {
        void (*signal_txt_msg)(conf_object_t *obj, txt_msg_t txt_msg,
                uint8 package_id);

        //supporting method to report platform capabilities to processor
        bool (*get_platform_feature)(conf_object_t *obj, int feature);

        //get list of cpus (threads) available in the current system
        attr_value_t (*get_platform_cpus)(conf_object_t *obj);
};

#define X86_TXT_CHIPSET_INTERFACE "x86_txt_chipset"
// ADD INTERFACE x86_txt_chipset_interface

/* <add id="x86_unplug_interface_t">
    The methods of the interface are intended to be used
    by a platform to unplug (and optionally re-plug) CPU cores/threads,
    effectively hiding them from the #RESET/INIT signals.

    This interface is internal and may change without notice.

    The <b>unplug_core</b> method is to disable an operating core with all
    threads on it.
    The <b>replug_core</b> method is to enable a core previously unplugged
    with all threads on it.
    The <b>disable_ht_package</b> method is to disable every secondary thread.
    The <b>enable_ht_package</b> method is to enable every secondary thread.

    Values returned from these methods indicate success (1) or failure (0).

    <insert-until text="// ADD INTERFACE x86_unplug_interface"/>
    </add>
    <add id="x86_unplug_interface_exec_context">
    Cell Context for all methods.
    </add> */
SIM_INTERFACE(x86_unplug) {
        int (*unplug_core)(conf_object_t *obj);
        int (*replug_core)(conf_object_t *obj);
        int (*disable_ht_package)(conf_object_t *obj);
        int (*enable_ht_package)(conf_object_t *obj);
};
#define X86_UNPLUG_INTERFACE "x86_unplug"
// ADD INTERFACE x86_unplug_interface

/* <add id="x86_unplug_v2_interface_t">
    The methods of the interface are intended to be used
    by a platform to unplug (and optionally re-plug) CPU cores/threads,
    effectively hiding them from the #RESET/INIT signals.

    This interface is internal and may change without notice.

    The <b>unplug_core</b> method is to disable an operating core with all
    threads on it.
    The <b>replug_core</b> method is to enable a core previously unplugged
    with all threads on it.
    The <b>disable_core_ht</b> method is to disable HT in the core (not the package).
    The <b>enable_core_ht</b> method is to enable HT in the core (not the package).
    The <b>core_is_plugged</b> method returns true if core is plugged.
    The <b>core_ht_is_enabled</b> method returns true if HT is enabled in the core (not the package).
    The <b>set_alive_logical_processors</b> method updates number of alive processors reported through CPUID.
    The <b>get_alive_logical_processors</b> method returns number of alive processors reported through CPUID.

    Values returned from enable and disable methods indicate success (1) or failure (0).

    <insert-until text="// ADD INTERFACE x86_unplug_v2_interface"/>
    </add>
    <add id="x86_unplug_v2_interface_exec_context">
    Cell Context for all methods.
    </add> */
SIM_INTERFACE(x86_unplug_v2) {
        int (*unplug_core)(conf_object_t *obj);
        int (*replug_core)(conf_object_t *obj);
        int (*disable_core_ht)(conf_object_t *obj);
        int (*enable_core_ht)(conf_object_t *obj);
        bool (*core_is_plugged)(conf_object_t *obj);
        bool (*core_ht_is_enabled)(conf_object_t *obj);
        void (*set_alive_logical_processors)(conf_object_t *obj, int count);
        int (*get_alive_logical_processors)(conf_object_t *obj);
};
#define X86_UNPLUG_V2_INTERFACE "x86_unplug_v2"
// ADD INTERFACE x86_unplug_v2_interface

/* <add id="x86_external_reset">
   The interface is used to notify platform devices about reset events that
   happened inside a CPU.

   The <b>triple_fault_event</b> method is used to inform <b>obj</b>
   that a reset happened due to a triple fault.

   <insert-until text="// ADD INTERFACE x86_external_reset"/>
   </add>
   <add id="x86_external_reset_interface_exec_context">
   Cell Context for all methods.
   </add> */
SIM_INTERFACE(x86_external_reset) {
        void (*triple_fault_event)(conf_object_t *obj);
};
#define X86_EXTERNAL_RESET_INTERFACE "x86_external_reset"
// ADD INTERFACE x86_external_reset

/* <add-type id="x86_smm_event_type_t"> </add-type> */
typedef enum x86_smm_event_type {
    X86_Smm_Enter,
    X86_Smm_Leave
} x86_smm_event_type_t;

/* <add-type id="x86_smm_phase_type_t"> </add-type> */
typedef enum x86_smm_phase_type {
    X86_Smm_Phase0,
    X86_Smm_Phase1
} x86_smm_phase_type_t;

/* <add id="x86_smm_notification_interface_t">

   Objects registered in processor's <attr>smm_listeners</attr> attribute will
   be called via the <fun>notification</fun> method whenever the CPU's enters
   and leaves SMI handler. SMI handler entry occurs on SMI processing. SMI
   handler exit occurs by RSM instruction execution. Please note that for both
   SMI handler entry and SMI handler exit <fun>notification</fun> method will be
   invoked twice: at the beginning of the entry/exit and at the end when CPU
   state was already modified. <arg>event</arg> argument says if entry/exit is
   performed, <arg>phase</arg> argument equals to <b>X86_Smm_Phase0</b> for
   the beginning of the event and to <b>X86_Smm_Phase1</b> for the end.

   <ndx>x86_smm_event_type_t</ndx>
   <insert id="x86_smm_event_type_t"/>

   <ndx>x86_smm_phase_type_t</ndx>
   <insert id="x86_smm_phase_type_t"/>

   <insert-until text="// ADD INTERFACE x86_smm_notification_interface"/>

   </add>
   <add id="x86_smm_notification_interface_exec_context">
   Cell Context.
   </add> */
SIM_INTERFACE(x86_smm_notification) {
        void (*notification)(conf_object_t *listener, conf_object_t *cpu,
                             x86_smm_event_type_t event,
                             x86_smm_phase_type_t phase);
};
#define X86_SMM_NOTIFICATION_INTERFACE "x86_smm_notification"
// ADD INTERFACE x86_smm_notification_interface

/* <add id="apic_bus_v2_interface_t">
    The interface is used to handle extended 32-bit addressing mode supported
    by x2APIC. See <b>apic_bus</b> documentation.
<insert-until text="// ADD INTERFACE apic_bus_v2"/>
   </add>

   <add id="apic_bus_v2_interface_exec_context">
   Cell Context.
   </add> */
SIM_INTERFACE(apic_bus_v2) {
        apic_bus_status_t (*interrupt)(conf_object_t *obj,
                apic_destination_mode_t dest_mode,
                apic_delivery_mode_t delivery_mode,
                int level_assert,
                apic_trigger_mode_t trigger_mode,
                uint8 vector,
                uint32 destination);
};
#define APIC_BUS_V2_INTERFACE "apic_bus_v2"
// ADD INTERFACE apic_bus_v2

/* <add id="apic_timer">
    The interface provide remote control for the APIC timer, which is usually
    available only via registers.
<insert-until text="// ADD INTERFACE apic_timer"/>
    </add>
*/
typedef enum {
    Apic_Timer_Mode_One_Shot        = 0,
    Apic_Timer_Mode_Periodic        = 1,
    Apic_Timer_Mode_Tsc_Deadline    = 2
} apic_timer_mode_t;

SIM_INTERFACE(apic_timer) {
        void (*enable_timer)(conf_object_t *obj);
        void (*disable_timer)(conf_object_t *obj);
        int (*is_timer_enabled)(conf_object_t *obj);
        apic_timer_mode_t (*get_timer_mode)(conf_object_t *obj);
        void (*set_timer_mode)(conf_object_t *obj, apic_timer_mode_t mode);
        uint32 (*get_timer_initial_count)(conf_object_t *obj);
        void (*set_timer_initial_count)(conf_object_t *obj, uint32 initial_count);
        uint32 (*get_timer_current_count)(conf_object_t *obj);
        void (*set_timer_current_count)(conf_object_t *obj, uint32 current_count);
        uint32 (*get_timer_divisor)(conf_object_t *obj);
        void (*set_timer_divisor)(conf_object_t *obj, uint32 divisor);
        uint64 (*get_timer_tsc)(conf_object_t *obj);
        void (*set_timer_tsc)(conf_object_t *obj, uint64 tsc);
        uint64 (*get_timer_tsc_deadline)(conf_object_t *obj);
        void (*set_timer_tsc_deadline)(conf_object_t *obj, uint64 tsc_deadline);
};

#define APIC_TIMER_INTERFACE "apic_timer"
// ADD INTERFACE apic_timer

/* <add id="x86_sai">
    The <b>get_current_sai</b> method is used to obtain
    current value of SAI for processor-initiated memory
    transactions.
   <insert-until text="// ADD INTERFACE x86_sai"/>
   </add> */
SIM_INTERFACE(x86_sai) {
    uint64  (*get_current_sai)(conf_object_t *obj);
};
#define X86_SAI_INTERFACE "x86_sai"
// ADD INTERFACE x86_sai

typedef enum {
    Mktme_KeyCtrl_Clear_Key = 0,
    Mktme_KeyCtrl_Set_Key   = 1,
    Mktme_KeyCtrl_No_Key    = 3,
} mktme_keyctrl_t;
SIM_PY_ALLOCATABLE(mktme_keyctrl_t);

SIM_INTERFACE(mktme) {
    void (*command)(conf_object_t *obj, uint16 key_id, mktme_keyctrl_t cmd,
                    uint8 keyid_key[64], uint8 tweak_key[64]);
    void (*enable_mktme)(conf_object_t *obj, uint8 key_msb, uint8 key_lsb);
    void (*enable_tme)(conf_object_t *obj);
    void (*is_internal_transaction) (conf_object_t *obj, bool val);
};
#define MKTME_INTERFACE "mktme"

#ifdef __cplusplus
}
#endif

#endif /* ! X86EX_INTERFACE_H */
