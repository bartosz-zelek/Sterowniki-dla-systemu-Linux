Timing and results delay
========================

As mentioned in the package introductory documentation, there may be several
sources of delay that can lead to unordered read and write operations in the
power/thermal simulation.

The physical connector has a configuration attribute
`results_delay_picoseconds` to make it more "resilient" with respect to delays
and unordered operations.

The results delay ensures that unordered events are ignored within a limited
time window (of the size of the results delay). The time window is sliding: it
advances as the time knowledge in the power/thermal simulator advances.

```
                              Greatest timestamp received
                              by the power/thermal simulator
                               |
                               |
                               v
 |--------|--------------------|--------> Virtual time
          |                    |
          [results_delay window]
                    .
                    .
                    .
           Events within this
           window are ignored
           by the simulator
           when computing
           current or 
           temperature results
```

In practice, it means that when a temperature or current is requested for time
`t` to the physical simulator, the latter systematically replies for time
`t - results_delay`.

## Dealing with temporal decoupling

The most common source of unordered operations is temporal decoupling. Please
refer to the Simics documentation for details on this matter.

Let's assume that two CPUs, `cpu1` and `cpu2`, are temporally decoupled in a
platform. `cpu2` may send a timed event to the power/thermal simulator for
virtual time `t2` before `cpu1` sends another timed event for `t1`, with
`t1 < t2`:

```
 |
 |----------------------cpu2_event_at_t2-----> Virtual time
 |
 |-----cpu1_event_at_t1----------------------> Virtual time
 |
 v
 Execution order
```

Simics provides settings to control the time between processor switches, and to
control the virtual time max differences among CPUs simulated in parallel.

Assume that the maximum temporal decoupling among all the CPUs is
`max_temporal_decoupling`. Assume that `physical_connector0` is an instance of
the physical connector class. It should be configured with at least the following:

```
simics> @conf.physical_connector0.results_delay_picoseconds += max_temporal_decoupling
```

Note the `+=` in the code snippet above: the `max_temporal_decoupling` is
**added** to the existing results delay.

## Dealing with other sources of delay

There are other sources of delay that may stack on the temporal decoupling.

Assume that there is performance model responsible for sending activity events,
e.g., timestamped cdyn values, to the power/thermal simulator. Assume that
`max_heartbeat` is the max time span between two activity events. Finally,
assume that the performance model sends events in a "backward" manner
(simulation timestamp at the end of the cdyn interval).

Since the power/thermal simulator expects only events in a "forward" manner
(simulation timestamp at the beginning of the value interval), a conversion
"backward intervals -> forward intervals" must be done.

Performing such a conversion during simulation adds a delay equal to
`max_heartbeat` because of the buffering:

```
simics> @conf.physical_connector0.results_delay_picoseconds += max_heartbeat
```

Note that there is a module `float_forwarder` to help you perform the "backward
intervals -> forward intervals" conversion for float values.

Now assume that you want to average cdyn values on fixed intervals before
sending them to the power/thermal simulator. There is a module
`float_aggregator` that performs such an averaging. The aggregator is
event-driven, so it can close an average interval only when it receives a first
event past the interval.

The above averaging adds a delay equal to the "aggregation interval" plus the
`max_heartbeat` described earlier (max time span to close an interval):

```
simics> @aggregator = SIM_create_object('float_aggregator', ...)
simics> ...
simics> @aggr_delay = aggregator.aggregation_interval_picoseconds + max_heartbeat
simics> @conf.physical_connector0.results_delay_picoseconds += aggr_delay
```

(Again, note the `+=` in the code snippet above).

## Experimenting by removing temporal constraints

By default, the physical connector has a results delay equal to 1 ps. This means that
  - you cannot change the past,
  - if `t_latest` is the greatest timestamp received by the simulator, then all
    events at `t_latest` are ignored when computing a temperature or current.

If you want to remove the two constraints above, then you can set a
`results_delay_picoseconds` equal to 0.

**Pay attention**: this is an advanced mode that could be considered as a
"debugging" mode. You must be in control of the simulation, and aware that you
may mess with simulation results. This mode should not be used for standard
simulations.
