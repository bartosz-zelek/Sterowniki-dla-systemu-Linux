/*
  © 2022 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SIMULATOR_IFACE_PROBE_INTERFACE_H
#define SIMICS_SIMULATOR_IFACE_PROBE_INTERFACE_H

#include <simics/pywrap.h>
#include <simics/base/attr-value.h>
#include <simics/base/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* <add-type id="probe_key_t def"></add-type> */
typedef enum {
        Probe_Key_Kind = 0,                  /* Identifier of the probe */
        Probe_Key_Name = 0,                  /* Old name of Probe_Key_Kind,
                                                kept for compatibility */

        Probe_Key_Type = 1,                  /* string, any of: {
                                                "int", "float, "fraction",
                                                "histogram", "int128", "string"
                                                } */
        Probe_Key_Categories = 2,            /* list of strings */
        Probe_Key_Cause_Slowdown = 3,        /* bool */

        Probe_Key_Owner_Object = 4,          /* object: owner object */

        Probe_Key_Display_Name = 5,          /* string: short narrow name */
        Probe_Key_Description = 6,           /* string */

        Probe_Key_Float_Percent = 7,
        Probe_Key_Float_Decimals = 8,
        Probe_Key_Metric_Prefix = 9,
        Probe_Key_Unit = 10,
        Probe_Key_Binary_Prefix = 11,
        Probe_Key_Time_Format = 12,
        Probe_Key_Width = 13,

        Probe_Key_Value_Notifier = 14,

        Probe_Key_Aggregates = 17,   /* Defines new probes which aggregate
                                        over this probe:
                                        [list of [list properties]]
                                        Invalid for Type: "string"
                                     */
        Probe_Key_Aggregate_Scope = 18, /* Aggregate over all probes in the
                                           cells or all probes in the system.
                                           String and of ["global", "cell"] */
        Probe_Key_Aggregate_Function = 19, /* How the aggregation should be
                                              done:
                                              "sum",
                                              "weighted-arith-mean",
                                              - only fractions
                                           */
        Probe_Key_Definition = 20,         /* string: how the probe is
                                              calculated */
} probe_key_t;


/* <add id="probe_interface_t">

   This interface defines a probe in the system. A probe is a mechanism that
   lets a model expose values of interest to a user. By using this interface,
   generic tools can extract these values and present them in different forms,
   like tables and graphs. A probe should always have a meaningful value that
   can be read at any time during the simulation.

   Probes should be used for user observable values and are not intended for
   model to model communication.

   The <fun>properties()</fun> method is typically called once for each tool
   or use-case to get information about the probe. It should return a key/value
   <type>attr_value_t</type> list, i.e., list of lists of two values, with
   probe specific properties. The first value of the inner list represents the
   property key as an Sim_Val_Integer using the enum <type>probe_key_t</type>
   type, and the second value is a key specific data item described below. The
   <type>probe_key_t</type> is defined as:

   <insert id="probe_key_t def"/>

   The <fun>value()</fun> method should return the data for the probe as an
   <type>attr_value_t</type> in accordance of how the property type has been
   defined. This method is called each time a tool/user wants to read the latest
   probe value.

   The property keys are describes here as follows:

   <dl>
   <dt><tt>Probe_Key_Kind</tt></dt>

   <dd> Sets the kind of the probe, as a string (Sim_Val_String).
   The probe-kinds are what uniquely define the probes in the system.

   There can be many probes associated with the same kind,
   but they should all represent the same type of measured data.

   The kind should be named so it is humanly possible to understand what
   you get back when you read it.

   To avoid conflicts, the kind should be an hierarchical name where each level
   is separated with a dot. Probes falling into the same domain should have
   logical names according to this. Currently, there are some defined names on
   the first level that can be used to build the hierarchy. Among them are:
   <i>cpu.</i> that collects probes related to a CPU, <i>dev.</i> that collects
   probes for devices, <i>cell.</i> that collects probes related to cells,
   <i>host.</i> collects probes related to the host environment, and
   <i>sim.</i> that collects probes having to do with the overall simulation
   environment.

   Some examples, include: <i>cpu.cycles</i>,
   <i>cpu.exec_mode.jit_steps</i>, <i>dev.io_access_count</i>,
   <i>cell.steps</i>, <i>host.swap.used</i>, and <i>sim.slowdown</i>.

   Several objects implementing this interface can use the same probe-kind,
   for similar probe values. The probe's unique identifier is a string
   containing the object name then a colon, followed by the probe-kind, e.g.,
   "board.mb.cpu0.cpu[0][0]:cpu.cycles".

   </dd>

   <dt><tt>Probe_Key_Display_Name</tt></dt>

   <dd> Sets a display name for the probe as a string (Sim_Val_String). This is
   a more human friendly name than the probe name, and a tool can use this name
   in table headings or graph legends. </dd>

   <dt><tt>Probe_Key_Type</tt></dt>

   <dd> Sets the type of the probe as a string (Sim_Val_String). The following
   types are defined: "int", "float", "string", "fraction", "int128" and
   "histogram". The <type>attr_value_t</type> value that should be returned by
   the value method for these types are as follows:

   For the int type, an <type>attr_value_t</type> of type Sim_Val_Integer
   should be returned.

   For the float type, return a Sim_Val_Floating value.

   The string type is returned by a Sim_Val_String value. The string typically
   represents some momentarily state. Due to nature of a string, a tool cannot do
   any calculation on the probe value, only show the current string content.

   The fraction type should use a <type>attr_value_t</type> list of two
   Sim_Val_Integer or Sim_Val_Floating values. This represent a mathematical
   fraction value, where the first one is the numerator and and the second
   value is the denominator value. Note however that a float value can be used
   for both of them. To get the proper value for this probe the tool/user
   should divide the numerator with the denominator. Using fraction makes it
   possible for the tool to calculate a mean value for this probe between any
   point in time where the probe was read, e.g., say you have a probe that
   reads the frequency of a simulated processor that varies over time. Then
   this probe can be defined as a fraction between the cycle count and the
   elapsed virtual time (cycles/time = frequency). If the tool saves the cycle
   difference and the time difference between to points in time, it can
   calculate the mean frequency between those point by dividing the
   differences.

   For very large numbers the int128 type can be used, the type is represented
   as a <attr>attr_value_t</attr> list of two value; the high 64-bit number
   followed by the low 64-bit number.

   The histogram type is represented by a <type>attr_value_t</type> list of
   lists of two values. Where the inner list is used as a tuple of a named
   group as a Sim_Val_String, and its corresponding value as a Sim_Val_Integer
   or Sim_Val_Floating. A tool can then display an histogram of these groups
   and/or calculate the difference between two histograms over time and display
   that.</dd>.

   <dt><tt>Probe_Key_Categories</tt></dt>

   <dd> The key is a list of string that defines some categories for the
   probe. This information can be used to search for probes of a specific use
   case.</dd>

   <dt><tt>Probe_Key_Cause_Slowdown</tt></dt>

   <dd> The value for this key is a Sim_Val_Boolean. True means that using the
   probe can cause a significant slowdown of the simulation. False means it does
   not cause any particular slowdown.</dd>

   <dt><tt>Probe_Key_Owner_Object</tt></dt>

   <dd> The value for this key is a Sim_Val_Object that should be regarded as
   the owner of the probe instead of the object that implements this interface
   (which is default). This can be useful if some other objects implements
   functionality for the main object.</dd>

   <dt><tt>Probe_Key_Description</tt></dt>

   <dd> The value for this key is a Sim_Val_String that documents this
   probe in details.</dd>

   <dt><tt>Probe_Key_Definition</tt></dt>

   <dd> "Function" definition string, explaining how the probe is
   being calculated out from other probes. This is automatically created for
   probe aggregates and derivatives. Default is an empty string.</dd>

   <dt><tt>Probe_Key_Float_Decimals</tt></dt>

   <dd> The value for this key is a Sim_Val_Integer that sets the preferred
   number of decimals of this is float probe when a value is displayed.</dd>

   <dt><tt>Probe_Key_Float_Percent</tt></dt>

   <dd> The value for this key is a Sim_Val_Boolean that sets the preferred
   representation of a float probe as a percent representation, e.g., a value
   of 0.176 will be displayed as 17.6% if the number of decimals are one.</dd>

   <dt><tt>Probe_Key_Metric_Prefix</tt></dt>

   <dd> The value for this key is a Sim_Val_String describing a unit name. This
   sets the preferred representation of a float probe to be displayed with a
   metric prefix for the unit given. Supported metric prefixes are: k, M, G, T,
   P, E, Z, Y, for values > 1 and m, &micro;, n, p, f, a, z, y for values &lt;
   1. For example if the unit is set to "s" (seconds) and the float value is
   0.0000347, then the displayed representation should be "34.7 &micro;s". To
   omit the unit in the output use the empty string, "", as the unit. If the
   value is outside the prefix ranges, an scientific E notation will be used.
   </dd>

   <dt><tt>Probe_Key_Binary_Prefix</tt></dt>

   <dd> The value for this key is a Sim_Val_String describing a unit name. This
   sets the preferred representation of an integer probe to be displayed with a
   binary prefix for the unit given. Supported prefixes are: ki, Mi, Gi, Ti,
   Pi, Ei, Zi, and Yi. For example if the unit is set to "B" (representing
   bytes), and the integer value is 10485760 (10*1024*1024), then the displayed
   representation should be "10 MiB".
   .</dd>

   <dt><tt>Probe_Key_Unit</tt></dt>

   <dd> The value for this key is a Sim_Val_String that sets the unit for the
   value. A tool can use this to display the unit for the value.</dd>

   <dt><tt>Probe_Key_Time_Format</tt></dt>

   <dd> The value for this key is a Sim_Val_Boolean that sets the preferred
   representation for a float value to a time format, hh:mm:ss.d, where h is
   hours, m minutes, s seconds, and d is fractions of a seconds. The raw value
   returned by the value method is the number of seconds. The
   Probe_Key_Float_Decimals key/value pair controls the number of digits in the
   fraction. If the number of seconds represents more than 99 hours, it should
   continue adding digits for the hours, e.g., 100:44:10.123</dd>

   <dt><tt>Probe_Key_Width</tt></dt>

   <dd>The value for this key is a Sim_Val_Integer that sets the preferred
   width of a probe value in characters. This can be used to determine the best
   width of columns in a table tool, for instance.</dd>

   <dt><tt>Probe_Key_Value_Notifier</tt></dt>

   <dd>The value for this key is a Sim_Val_String that describes a notifier
   that is triggered when the value changes. This can be used by a tool to
   track all changes of the value. For performance reasons the value should be
   changes infrequently.</dd>

   <dt><tt>Probe_Key_Global_Sum</tt></dt>

   <dd>Deprecated, use <tt>Probe_Key_Aggregates</tt> instead.</dd>

   <dt><tt>Probe_Key_Cell_Sum</tt></dt>

   <dd>Deprecated, use <tt>Probe_Key_Aggregates</tt> instead.</dd>

   <dt><tt>Probe_Key_Aggregates</tt></dt>

   <dd>Defines lists of new probes which are aggregates of the probe defined.
   Each list-element defines a new probe, specified as a list of the key-value
   properties for the aggregate probe. All properties are inherited from
   the probe being aggregated, except the new name of the aggregate probe which
   must be defined. Probe keys can be redefined if they appear in the
   aggregate scope. For example, it is advisable to change the description
   making it clear what value that is being returned by the aggregate.

   Only in the aggregate scope, the <tt>Probe_Key_Aggregate_Scope</tt>
   and <tt>Probe_Key_Aggregate_Function</tt> key-value pairs are used to further
   define how the aggregation is done.
   Note that aggregates depend on the the type of the underlying probe.
   Some probe types can only be aggregated with certain functions.
   </dd>

   <dt><tt>Probe_Key_Aggregate_Scope</tt></dt>

   <dd>Defines the scope for the aggregate probe. That is, which objects that
   implements the probe, should be part of the aggregation.
   Valid values are "cell" or "global". Default is "global",
   which means that all objects which implements the probe will be part of
   the aggregate and put in the singleton owner object, such as the 'sim' or
   'host' objects.

   Cell-aggregates are put under the available cell objects
   and will aggregate the probe which belongs to the respective cell object.
   </dd>

   <dt><tt>Probe_Key_Aggregate_Function</tt></dt>

   <dd>Defines how the aggregate should be generated.
   Valid values are "sum", "min", "max", "arith-mean", "weighted-arith-mean"
   and "median". Default is "sum".

   For probes of the fraction type, if any denominator contains a zero value
   (division by zero), the calculated value is undefined and [0, 0] is
   returned. This applies to all fraction aggregates except weighted-arith-mean,
   where the zero denominator can be handled.

   <dt><tt>sum</tt></dt>
   <dd>All probe values are summed together. Not supported on "string" probes.</dd>

   <dt><tt>min</tt></dt>
   <dd>The lowest value among the probe values is returned. Not supported on "string"
   and "histogram" probes.
   For fraction-probes, if any denominator contains a zero value, the calculated sum
   returned is [0, 0].
   </dd>

   <dt><tt>max</tt></dt>
   <dd>The highest value among probe values is returned. Not supported on "string"
   and "histogram" probes.</dd>

   <dt><tt>arith-mean</tt></dt>
   <dd>The arithmetic mean is calculated for all the probe values.
   Not supported on "string" and "histogram" probes.</dd>

   <dt><tt>weighted-arith-mean</tt></dt>
   <dd>Only supported in fraction type probes.
   The denominators are used as weights.

   Using these weights implies that the weighted arithmetic mean can be
   calculated by adding all numerators and denominators, producing a new
   fraction of these sums.

   For example, when calculating the mean instruction per cycles (IPC)
   on all processors (where the IPC per processor is represented as a
   fraction: instructions / cycles).
   With two processors having [20/30] and [40/50], the total IPC
   becomes [(20+40)/(30+50)] or [60/80] and the IPC value of 0.75.
   </dd>

   <dt><tt>median</tt></dt>
   <dd>The median among the probe values is returned.
   Not supported on "fraction", "string" and "histogram" probes.</dd>

   <dt><tt>object-histogram</tt></dt>
   <dd>A histogram probe is created using the probe-owner-objects as key
   and their probe-values value. The type of this aggregate probe
   must be set to "histogram".
   Only supported on "int" and "float" probes types.</dd>

   <dt><tt>class-histogram</tt></dt>
   <dd>Similar to the object-histogram, but here the histogram uses
   the classname of the owner-object as key, and the value is the
   sum of the probe-values with the same class. The type of this aggregate
   probe must be set to "histogram".
   Only supported on "int" and "float" probes types.</dd>

   </dd>
   </dl>

   All key/value pairs except the Probe_Key_Kind and Probe_Key_Type are
   optional.

   <insert-until text="// ADD INTERFACE probe_interface"/>
   </add>
   <add id="probe_interface_exec_context">
   Global Context for all methods.
   </add> */
SIM_INTERFACE(probe) {
        attr_value_t (*value)(conf_object_t *obj);
        attr_value_t (*properties)(conf_object_t *obj);
};
#define PROBE_INTERFACE "probe"
// ADD INTERFACE probe_interface

/*
  <add id="probe_index_interface_t">

  This interface is similar to the <iface>probe</iface> interface, except that
  the indexed version allows multiple probes to be defined with a single
  interface. The same index corresponds to the same probe for <fun>value</fun>
  and <fun>properties</fun> methods. The amount of probes that the interfaces
  supports is returned through the <fun>num_indices()</fun> method, which should
  be static.

  <insert-until text="// ADD INTERFACE probe_index_interface"/>
  </add>
  <add id="probe_index_interface_exec_context">
  Global Context for all methods.
  </add>
*/
SIM_INTERFACE(probe_index) {
        int (*num_indices)(conf_object_t *obj);
        attr_value_t (*value)(conf_object_t *obj, int idx);
        attr_value_t (*properties)(conf_object_t *obj, int idx);
};
#define PROBE_INDEX_INTERFACE "probe_index"
// ADD INTERFACE probe_index_interface_t


/*
  <add id="probe_array_interface_t">

  This interface is similar to the <iface>probe_index</iface> interface, except
  that an additional <fun>all_values()</fun> method returns an
  <type>attr_value_t</type> list of values as individually returned when calling
  <fun>value(idx)</fun> on each indexed probe. Using <fun>all_values()</fun>
  instead of looping over <fun>value(idx)</fun> can provide a significant
  improvement in performance, depending on the number of probes whose values
  need to be read, starting from a couple of probes.
  The <i>probe_sampler</i> objects, provided in the <i>probe-monitor</i>
  extension, make use of the <fun>all_values()</fun> method to boost their
  performance when sampling probes implemented with the <iface>probe_array</iface>
  interface. 

  <insert-until text="// ADD INTERFACE probe_array_interface"/>
  </add>
  <add id="probe_array_interface_exec_context">
  Global Context for all methods.
  </add>
*/
SIM_INTERFACE(probe_array) {
        int (*num_indices)(conf_object_t *obj);
        attr_value_t (*value)(conf_object_t *obj, int idx);
        attr_value_t (*all_values)(conf_object_t *obj);
        attr_value_t (*properties)(conf_object_t *obj, int idx);
};
#define PROBE_ARRAY_INTERFACE "probe_array"
// ADD INTERFACE probe_array_interface_t


/*
  <add id="probe_subscribe_interface_t">

  This is an optional additional probe interface.
  This interface should  be implemented to prevent any slowdown or unwanted
  side-effects if there are no listener on the probe.
  The <fun>subscribe()</fun> method should increment a reference count on
  how many subscribers there are, and do any kind of preparation to
  activate the probe when it is being used.

  Similarly, the <fun>unsubscribe()</fun> method should decrement the
  reference count and disable the feature when there is no subscribers
  left.

  The <fun>num_subscribers()</fun> method should return the current
  reference count.

  <insert-until text="// ADD INTERFACE probe_subscribe_interface"/>
  </add>
  <add id="probe_subscribe_interface_exec_context">
  Global Context for all methods.
  </add>
*/
SIM_INTERFACE(probe_subscribe) {
        void (*subscribe)(conf_object_t *obj);
        void (*unsubscribe)(conf_object_t *obj);
        int  (*num_subscribers)(conf_object_t *obj);
};
#define PROBE_SUBSCRIBE_INTERFACE "probe_subscribe"
// ADD INTERFACE probe_subscribe_interface_t


/*
  <add id="probe_sampler_cache_interface_t">
  This interface is implemented by the singleton <obj>probes</obj>
  object. The interface is expected to be used by either probe-samplers
  (<fun>enable</fun>, <fun>disable</fun> or by a probe
  which supports caching (<fun>get_generation_id</fun>).

  When a probe-sampler calls the <fun>enable</fun> method,
  caching can start. Caching depends on a generation id, this is
  automatically increased by <fun>enable</fun>.

  With caching enabled, probes can return the previous value back,
  avoiding expensive calculation, if they are read multiple times
  (either directly, or indirectly from other probes). It can also be
  used to avoid probe values to return a slightly different value the
  next time in the same sample, such as wallclock time.

  A probe which wants to use caching needs to call the
  <fun>get_generation_id</fun> method. As long as the generation id is the same
  as the last time the probe-value was returned, the same probe-value can be
  returned. Otherwise a new value needs to be returned.
  Generation id zero, is special, it means that caching is not currently enabled.

  When sampling is finished the probe_sampler calls the <fun>disable</fun>
  which will cause the generation id zero to be returned until the
  next sample begins again.

  <insert-until text="// ADD INTERFACE probe_sampler_cache_interface"/>
  </add>

  <add id="probe_sampler_cache_interface_exec_context">
  Global Context for all methods.
  </add>
*/
SIM_INTERFACE(probe_sampler_cache) {
        void (*enable)(conf_object_t *obj);
        void (*disable)(conf_object_t *obj);

        uint64 (*get_generation)(conf_object_t *obj);
};
#define PROBE_SAMPLER_CACHE_INTERFACE "probe_sampler_cache"
// ADD INTERFACE probe_sampler_cache_interface


#ifdef __cplusplus
}
#endif

#endif /* ! SIMICS_SIMULATOR_IFACE_PROBE_INTERFACE_H */
