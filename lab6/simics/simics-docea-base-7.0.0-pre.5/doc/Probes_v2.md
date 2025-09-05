# Docea Base Probes v2
The Docea Base Probes v2 feature allows users to monitor Docea's power and thermal variables at specific times.
It enhances scalability, compatibility with Simics, and ease of use compared to its predecessor.

## Instantiation
To set up the probes, instantiate an object of class `docea_probe_array`. Consider the following example:

```python
traced_parameters = [
    ['traffic', 'cdyn'],
    ['f_xtal', 'frequency'],
    ['v_bat', 'voltage'],
    ['active_state', 'state'],
    ['v_bat.load_current', 'current'], 
    ['Simple system/Package/East', 'probe_temperature'],
    ['Simple system/Package/East', 'source_temperature'],
    ['Simple system/Package/East', 'thermal_injected_power']
]
 
dpa0 = simics.SIM_create_object('docea_probe_array', 'dpa0',
                                [['physical_connector', physical_connector],
                                ['traced_parameters', traced_parameters]])
```

The `docea_probe_array` class requires two attributes:
1. **`physical_connector`**: An object containing the power and thermal model and simulator.
Refer to the [simulator v3 documentation](Simulator_v3.md) for more details.
2. **`traced_parameters`**: A list of power and thermal variables to monitor.
Each variable is defined by a two-element list: 
the variable name and its nature (one of `'frequency'`, `'voltage'`, `'cdyn'`, 
`'current'`, `'activity'`,`'power_float_variable'`, `'power_bool_variable'`,
`'state'`, `'power_string_variable'`, `'thermal_injected_power'`,
`'probe_temperature'` and `'source_temperature'`).

Users have the flexibility to create the list of traced parameters in various ways. 
They can manually specify the list, extract and filter variables from a metadata file, or query their models for a list of power and thermal variables and then apply a filter to these.

In the example, the `dpa0` object is a single Simics object that implements several probes.
For the above example, such probes are:
- `dpa0:docea.cdyn.traffic`
- `dpa0:docea.frequency.f_xtal`
- `dpa0:docea.voltage.v_bat`
- `dpa0:docea.state.active_state`
- `dpa0:docea.current.v_bat.load_current`
- `dpa0:docea.probe_temperature.Simple_system.Package.East`
- `dpa0:docea.source_temperature.Simple_system.Package.Die.Docea_map`
- `dpa0:docea.thermal_injected_power.Simple_system.Package.Die.Docea_map`

In general, for any traced parameter, the corresponding probe follows the syntax `<object_name>:docea.<nature>.<sanitized_power_thermal_name>`, where the power thermal name is sanitized as follows: 
1. Blank spaces (`' '`) become underscores `'_'`.
1. At signs (`'@'`) become underscores`'_'`.
1. Hyphens (`'-'`) become underscores `'_'`.
1. Slashes (`'/'`) become points `'.'`.

Sanitization occurs only for models that require it. 
In particular, models whose DoceaPy version is greater or equal to 2.10.0 have power/thermal names that already comply with Simics object naming rules, so no sanitization is applied. 

We will eventually drop the support to models with power/thermal names requiring sanitization.

In addition, the `dpa0` object contains the `dpa0:docea_time_ps` probe, which returns the time used for probing the power and thermal values. 

## Listing

You can list the created probes by running the following command in the Simics console:

```
enable-probes
probes.list-kinds -objects categories=docea max=0
```

The `max=0` switch allows to list all probes.

For our example, the output is the following:

```
┌─────┬───────────────────────────────────────────────────────┬──────────────────────────┬───┬───────┬───────────────────────────────────────────────┐
│Row #│                      Probe Kind                       │       Display Name       │Num│Objects│                  Description                  │
├─────┼───────────────────────────────────────────────────────┼──────────────────────────┼───┼───────┼───────────────────────────────────────────────┤
│    1│docea.cdyn.traffic                                     │traffic                   │  1│dpa0   │Probe on a Docea input/output value            │
│    2│docea.current.v_bat.load_current                       │v_bat.load_current        │  1│dpa0   │Probe on a Docea input/output value            │
│    3│docea.frequency.f_xtal                                 │f_xtal                    │  1│dpa0   │Probe on a Docea input/output value            │
│    4│docea.probe_temperature.Simple_system.Package.East     │Simple system/Package/East│  1│dpa0   │Probe on a Docea input/output value            │
│    5│docea.source_temperature.Simple_system.Package.East    │Simple system/Package/East│  1│dpa0   │Probe on a Docea input/output value            │
│    6│docea.state.active_state                               │active_state              │  1│dpa0   │Probe on a Docea input/output value            │
│    7│docea.thermal_injected_power.Simple_system.Package.East│Simple system/Package/East│  1│dpa0   │Probe on a Docea input/output value            │
│    8│docea.voltage.v_bat                                    │v_bat                     │  1│dpa0   │Probe on a Docea input/output value            │
│    9│docea_time_ps                                          │docea_time_ps             │  1│dpa0   │Probe on the time of a Docea input/output value│
├─────┼───────────────────────────────────────────────────────┼──────────────────────────┼───┼───────┼───────────────────────────────────────────────┤
│Sum  │                                                       │                          │  9│       │                                               │
└─────┴───────────────────────────────────────────────────────┴──────────────────────────┴───┴───────┴───────────────────────────────────────────────┘

```


## Reading
To retrieve the value of a probe in Simics, use the `probes.read` command as follows:

```
$v = (probes.read dpa0:docea.cdyn.Cdyn)
```

This command assigns the probe value to the `$v` variable as a string.
If you need the value in its original data type (like float or int), use the `-values` switch:

```
$v = (probes.read -values dpa0:docea.cdyn.Cdyn)
```

## Tracing

To read and record probe values, utilize the `probe-streamer` module.
This module can read probe data and output it when new power and thermal results are available. 
Set up the `probe-streamer` by using the `new-probe-streamer` command, as the following example shows:

```
$ps0 = (new-probe-streamer
            name=ps0
            sampling-mode=notifier
            notifier-type=ISIM-POWER-THERMAL-RESULTS
            notifier-obj=docea.physical_connector0
            csv-output-file="streamer_output.csv"
            timestamp-probe=dpa0:docea_time_ps)
$ps0.add-probe probe=dpa0:docea. mode=current
```


For detailed explanations of each parameter, refer to the [Simics documentation](https://simics-download.pdx.intel.com/simics-6/docs/html/rm-base/rm-cmd-new-probe-streamer.html#description).

When configuring the probe-streamer for use with Docea's probes, you should set:
- The sampling mode (`sampling-mode`) to `notifier`.
- The notifier type (`notifier-type`) to `ISIM-POWER-THERMAL-RESULTS`, which is the one used by all physical connectors.
- The notifier object (`notifier-obj`) to your physical connector's name (e.g., `docea.physical_connector0`).
- The timestamp probe (`timestamp-probe`) to your `docea_probe_array` object's time probe (e.g., `dpa0:docea_time_ps`).

## Optionally use the object's queue time

If instead of using the last physical connector's results time, you need the time
of another object to read and record the probe values, 
you can optionnally set the queue attribute to your `docea_probe_array` object to
a processor or clock and set its `use_queue_time` to `True`. 
Probe instantiation will then occur as in the following example:

```python
...
clock0 = simics.SIM_create_object('clock', 'clock0', [['freq_mhz', 1000]])
dpa0 = simics.SIM_create_object('docea_probe_array', 'dpa0',
                                [['physical_connector', physical_connector],
                                ['traced_parameters', traced_parameters],
                                ['queue', clock0],
                                ['use_queue_time', True]])
```

And probe streamer instantiation would then take the form of the following example:

```python
...
$ps0 = (new-probe-streamer
            name=ps0
            sampling-mode=virtual
            interval=0.001
            csv-output-file="streamer_output.csv"
            timestamp-probe=dpa0:docea_time_ps)
```

The probe streamer object will then read and record the probe values every millisecond of virtual time.

To see the full list of arguments to create a probe streamer, please refer to the official documentation at https://simics-download.pdx.intel.com/simics-6/docs/html/rm-base/rm-cmd-new-probe-streamer.html.



## Additional references:

For more details on the Simics probes command set, please refer to https://simics-download.pdx.intel.com/simics-6/docs/html/simics-user-guide/probes-command-set.html#Probes-Command-Set
