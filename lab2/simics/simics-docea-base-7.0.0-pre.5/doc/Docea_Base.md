Docea Base
==========

The "Docea Base" package provides power/thermal simulation capabilities.

It should be used with Simics Base versions 6.0.155+ 
(with support to Python 3.9).

## At a glance

This package provides an API to enable power/thermal simulation:
  - a collection of modules that can be instantiated and configured to setup the
    simulation,
  - a small Python API to ease the instantiation of some of the modules.

This package does not provide advanced capabilities for platform integration.
Instead, it provides configurable basic blocks that can be used during platform
integration.

## Drive the simulation using power/thermal model objects

The power/thermal model objects are "writable" and "readable" objects that
represent the inputs and outputs of a power/thermal model.

A specific collection of power/thermal model objects represents the interface of
a specific power/thermal model. For example, a writable object named
`power_thermal.model.Core0.Voltage` represents the `Voltage` input of `Core0` in
the power/thermal model. Likewise, a readable object named
`power_thermal.model.Core0.Hotspot` represents the `Hotspot` output (most
certainly a temperature) of `Core0` in the power/thermal model.

Each power/thermal model object represents a single parameter of the model. It
is always written or read as a single *timestamped* value: while writing an
input, the simulation time when the value applies must be provided alongside the
value; while reading an output, the simulation time when the value is requested
must be provided.

### A python API to instantiate and configure power/thermal model objects

Refer to [Simulator_v3.md](Simulator_v3.md)

### Connect the power/thermal facet to the rest of the platform

The power/thermal model objects are "writable" or "readable" because they
implement an interface that allows to call a `write` method or a `read` method.
This gives the "direction" of each model parameter represented by each object:
model inputs are writable, model outputs are readable.

Even if these objects are directly writable or readable, the preferred way to
connect them to the rest of the platform is through the Simics notifier API (to
trigger when to write or read) plus the Simics telemetry interface (to obtain
the data).

#### Connecting writable model objects

Writable model objects support the Simics notifier API and the Simics telemetry
interface. Instead of explicitly writing timestamped values into these objects,
you can configure them with a notifier and with a telemetry provider: they will
automatically listen to notifications and fetch values during the simulation.

This configuration needs to be done only once, before the simulation starts. You
need to configure:
  - The *notifier* of newly available telemetry values
  - The *telemetry provider*, where to fetch values
  - The *telemetry names*, used to know which telemetry to fetch

Assume that `power_thermal.model.Core0.Cdyn` is the writable object that you
want to configure to receive telemetry.
Assume that `vp.Core0` is the notifier of activity telemetry changes, and that
it notifies with the notifier type name `"isim-telemetry-update"`.
You can configure the notifier like this:

```
simics> @conf.power_thermal.model.Core0.Cdyn.notifier = [conf.vp.Core0, "isim-telemetry-update"]
```

If `vp.Core0` implements the Simics telemetry interface, then you do not need to
configure the telemetry provider explicitly, because the notifier will
automatically be used as the telemetry provider too. Otherwise, you can
configure the telemetry provider like this:

```
simics> @conf.power_thermal.model.Core0.Cdyn.telemetry_provider = THE_TELEMETRY_PROVIDER_OBJECT
```

Finally, you need to configure the telemetry names: which telemetry is the
writable object going to receive?
Assume that `"POWER"` is the name of the telemetry class.
Assume that `"TIME"` is the name of the *time* telemetry (remember: a model
object expects timestamped values) and that `"ACTIVE_CDYN"` is the name of the
telemetry *value* to fetch.
You can configure the telemetry names like this:

```
simics> @conf.power_thermal.model.Core0.Cdyn.upstream_time_telemetry = ["POWER", "TIME"]
simics> @conf.power_thermal.model.Core0.Cdyn.upstream_value_telemetry = ["POWER", "ACTIVE_CDYN"]
```

The names given above are the default values for writable objects of type
"cdyn". Model objects of this type represent activity parameters.
So, in the above example, the explicit configuration of *time* and *value*
telemetries is equal to the default configuration. It can be omitted.

Now you can run the simulation and you do not need to explicitly write values
into `power_thermal.model.Core0.Cdyn`. The latter will listen to notifications
from `vp.Core0` and it will automatically get the values.

#### Connecting readable model objects

Readable model objects implement the Simics telemetry interface.

**Before being able to get timestamped values from these objects using the
telemetry interface, you must configure a *time provider* to them**. Indeed, as
explained earlier, time is always an input for the power/thermal model: it must
be provided when reading or writing parameters.

Each readable object has a `time_provider` attribute which expects a valid
picosecond clock. For example, for a readable current object
`power_thermal.model.Core0.I`:

```
simics> @ps_clock = conf.board.mb.cpu0.core[0][0]
simics> @conf.power_thermal.model.Core0.I.time_provider = ps_clock
```

From now on, the current object `I` is ready to be accessed using the telemetry
interface: each time you get a value from it, the configured *time provider* is
used to obtain the timestamp for the read operation.

Readable model objects provide one telemetry class and two telemetries: the
*value* telemetry (e.g., value of a current or a temperature) and the *time*
telemetry (picosecond timestamp of the value). Their names have default values,
but they are configurable in case you need to use specific names.

```
simics> help power_thermal.model.Core0.I->downstream_telemetry_class
simics> help power_thermal.model.Core0.I->downstream_value_telemetry
simics> help power_thermal.model.Core0.I->downstream_time_telemetry
```

In this package, the attributes related to telemetry use the wording "upstream"
and "downstream" to distinguish the position in a data flow. This is a reference
to a common software data flow concept:

```
 data ---------> processing_block ------------> data
       uptream                     downstream
```

You can use the configurable names of the downstream telemetry attributes to
obtain the corresponding telemetry IDs. Then, you can use telemetry IDs to get
telemetry values. Please refer to the documentation of the Simics telemetry
interface for details about how telemetry works.

As an example, here is how you can obtain the telemetry class ID from
`power_thermal.model.Core0.I`:

```
simics> @name = conf.power_thermal.model.Core0.I.downstream_telemetry_class
simics> @class_id = conf.power_thermal.model.Core0.I.iface.telemetry.get_telemetry_class_id(name)
```

Typically, you would extract and store IDs only once, during the setup phase of
your platform. Then, during simulation, you can read telemetries like this:

```
simics> @current = conf.power_thermal.model.Core0.I.iface.telemetry.get_value(class_id, value_id)
simics> @timestamp = conf.power_thermal.model.Core0.I.iface.telemetry.get_value(class_id, time_id)
```

Note that the timestamp, in picoseconds, is provided by the object's time
provider. There are helper functions to easily set the time provider to multiple
readable objects at once.

To set the time provider to a specific collection of readable objects:

```
simics> @time_provider = ...
simics> @readable_objects = [conf.power_thermal.model.Core0.I, conf.power_thermal.model.Core0.T]
simics> @DoceaModel.set_time_provider(readable_objects, time_provider)
```

To set the time provide to all the readable objects under a given namespace,
deeply:

```
simics> @DoceaModel.set_time_provider_to_hierarchy('my.namespace', time_provider)
```

#### Fallback connection without telemetry

The preferred way to connect power/thermal model objects to the rest of the
platform is through the Simics notifier API plus the Simics telemetry interface,
as described earlier.

If you cannot rely on the above, for example because your platform does not
support telemetry yet, you can fall back to explicitly writing and reading the
model objects. To do that, you will need to keep a reference to the model
objects that you want to access in your code. You will also need to augment
your code with calls to `write` and `read` methods of objects. See the next
sections for some examples.

### Drive the power/thermal simulation

If the solution based on the Simics telemetry interface is used
  - Writable objects receive notifications and automatically "write themselves"
    by fetching telemetries.
  - Readable objects provide telemetry, so to read values you need to setup a
    similar method: decide when to pull values (it could be using a timer, or
    reacting upon an event notified by another device...) and get the values
    using the telemetry interface methods.

Otherwise, you can also explicitly write and read model objects.

A writable object allows to write timestamped values to the physical simulator
(for example, a voltage). It can be used like this:

```
simics> @picoseconds_in_1ms = 1_000_000_000
simics> @time_ps = 102 * picoseconds_in_1ms  # value in picoseconds, so 102 ms here
simics> @volts = 1.2
simics> @result = conf.power_thermal.model.Core0.Voltage.iface.writable_float.write(time_ps, volts)
simics> @print(result.status)
```

A readable object allows to read timestamped values from the physical simulator
(for example, a temperature). It can be used like this:

```
simics> @picoseconds_in_1ms = 1_000_000_000
simics> @time_ps = 102 * picoseconds_in_1ms  # value in picoseconds, so 102 ms here
simics> @temperature = conf.power_thermal.model.Core0.hotspot0.iface.readable_float.read(time_ps)
simics> @print(temperature.status)
simics> @print(temperature.value)
```

The "readable" and "writable" interfaces and their implementations are available
in C and compatible with Python and DML.

## Deal with timing and delays

The simulation has a virtual time but in Simics each clock has its own timeline,
so a clock can be temporally decoupled from other clocks. Please refer to the
Simics documentation for more details on this matter.

The power/thermal model must be resilient with respect to temporal decoupling:
it should accept read and write operations that can be "a little bit" unordered.
In Simics, there are settings to control the virtual time max difference among
clocks, so "how much unordered" can be better quantified by reading those
settings.

However, there may be other sources of delay that can lead to unordered
operations, for example the buffering of some inputs to be averaged before being
sent.

All these sources of delay, including the temporal decoupling, must be taken
into account when configuring the power/thermal model.

## CSV streaming of power/thermal parameters

Refer to [Probes_v2.md](Probes_v2.md)

## Checkpointing support

The power/thermal model objects are checkpointable.

However, when saving a checkpoint in Simics, keep in mind that the whole
platform must be checkpointable.

## Known limitations

The physical tracer is not checkpointable. When a checkpoint is restored, the
recommended solution is to instantiate also a new physical tracer.
