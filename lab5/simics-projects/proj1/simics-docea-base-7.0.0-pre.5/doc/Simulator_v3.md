Instantiate and configure the physical simulator "v3"
=====================================================

The `physical_connector_v3` is a facade object to enable the power/thermal
simulator "v3", that is, a simulator that support DoceaPy models.

There is a higher-level Python object named `PowerThermalSimulator`, provided by
the `simmod.power_thermal.simulator` module. It wraps the physical connector and
it is nicer to manipulate in Python. It is recommended to instantiate the
power/thermal simulator using the `PowerThermalSimulator` object.


## At a glance

Let's assume that the Python function `get_doceapy_model()` returns an instance
of the desired DoceaPy `SimulableModel`. In a Simics Python script, a
power/thermal simulator can be instantiated like this:

```python
from simmod.power_thermal.simulator import Configuration, PowerThermalSimulator

...

model = get_doceapy_model()  # Returns a DoceaPy SimulableModel instance
cfg = Configuration()
simulator = PowerThermalSimulator(cfg, model)

print(simulator.connector.name)  # Prints the name of the physical_connector_v3
```


## Access the lower-level physical_connector_v3 object

An instance of `PowerThermalSimulator` has a field `connector` that holds the
`physical_connector_v3`. The latter is a plain Simics object. In the example
above, it can be accessed as `simulator.connector`. For example, it is possible
to change the log level of the connector: `simulator.connector.log_level = 4`.


## Configure the power/thermal simulator

The physical connector has several attributes to setup the power/thermal
simulation. The Python object named `Configuration`, provided by the
`simmod.power_thermal.simulator` module, allows to configure those attributes on
the `PowerThermalSimulator`. Moreover, it adds some additional configuration
fields to shorten the setup task.

For example, to instantiate a power/thermal simulator with a custom "results
delay" (see also ["Timing and results delay"](Timing_and_results_delay.md)), and
with a physical connector named "phy0", you can proceed as follows:

```python
from simmod.power_thermal.simulator import Configuration, PowerThermalSimulator

model = ...
cfg = Configuration(results_delay_picoseconds=123, connector_name="phy0")
simulator = PowerThermalSimulator(cfg, model)
```

You can look at the `Configuration` object for the complete list of attributes.
For example:

```python
c = Configuration()
print(vars(c))
```


## Caveats

### The PowerThermalSimulator creates interface objects by default

By default, a `Configuration` for the `PowerThermalSimulator` instructs to
instantiate also all the objects corresponding to the DoceaPy model
interface (see also the section about "power/thermal model objects" in the
[Docea Base documentation](Docea_Base.md)).

This behavior can be changed using the `must_create_interface_objects` knob in
the `Configuration`:

```python
cfg = Configuration()
cfg.must_create_interface_objects = False
simulator = PowerThermalSimulator(cfg, model)
```

If the creation of the interface objects is enabled, then the objects are
accessible from the `interface` property of the `PowerThermalSimulator`. For
example, for a `PowerThermalSimulator` instance named `simulator`, if the model
interface contains a voltage input named `v_in`, then the latter is reachable as
`simulator.interface.v_in`.


### I have a DoceaPy model package: how to create a model for the PowerThermalSimulator?

This is pretty straightforward, assuming that the package already provides a
constructor for the model!

Let's assume that your DoceaPy python package is `my_doceapy_model.physical`.
Let's assume that it contains a `SimulableModel` constructor function named
`get_doceapy_model`. Then, it is sufficient to proceed as follows:

```python
from my_doceapy_model.physical import get_doceapy_model
from simmod.power_thermal.simulator import Configuration, PowerThermalSimulator

model = get_doceapy_model()
cfg = Configuration()
simulator = PowerThermalSimulator(cfg, model)
```

Documenting DoceaPy models creation is out of scope here.


## Legacy section: manipulate the lower-level physical connector object alone


## Instantiate the power/thermal simulator

A power/thermal simulator can be instantiated like this:

```
simics> @SIM_create_object('physical_connector_v3', 'physical_connector3', [])
```

The physical connector has several configuration attributes that allow to setup
the simulation.

Note that you can instantiate several `physical_connector_v3`. Each one is
independent from the others and it can be used to simulate an independent model.
For example, in case of multiple independent systems across concurrent Simics
cells, you should instantiate one connector per system.


## Configure the power/thermal simulation

The main configuration attribute is the power/thermal `model` to simulate. It
consists of two pieces of information:
  - The name of the Python module that provides the model
  - The name of the function to call to instantiate the model (the constructor)

If your model does not come from a Simics package, then you must tell Simics
where to look for the Python module(s) that provide the power/thermal model.

Let's assume that
  - your model is provided by a Python module named `graphics_card_physical`
  - the module, along with all its dependencies, is placed in a folder
    `dpm/graphics_card-0.2.1` under your current Simics project
    ("0.2.1" is the version of the model, it is always a good idea to properly
    handle versioning)
  - the model's constructor function is named `build_model`

Then, following the example in the previous section, you can

  1. Tell Simics where to look for the Python module(s):

```
simics> @import sys
simics> @sys.path.append("dpm/graphics_card-0.2.1")
```

  2. Set the `model` attribute of the connector with "module name" and
     "constructor name":

```
simics> @conf.physical_connector3.model = ["graphics_card_physical", "build_model"]
```


## Enable model's parameters observation and CSV tracing

If you want your model to be observable, for example to trace power/thermal
parameters in a CSV, you must also specify the metadata of the power/thermal
model.

If the metadata file does not exist, then it will be generated automatically
once the power/thermal simulation is started (see the sections below).
Otherwise, if you want to use a specific metadata, you can provide the path to
an existing file.

To set the metadata:

```
simics> @conf.physical_connector3.model_metadata = 'path/to/metadata.json'
```


## More configuration

For the complete list of configuration attributes, see
`list-attributes physical_connector_v3` and use `help` on each attribute.


## Starting the simulation

All the configuration attributes can be changed in any order, until the
power/thermal simulation is started. Once the simulation has started, the
configuration becomes read-only (`model`, `model_metadata`, ...).

Sending any first event to the power/thermal simulator will automatically start
it. Alternatively, if you want to explicitly manage the configuration lifecycle
of the connector, you can start the simulation explicitly by setting the
`started` attribute to `True`. This may be useful in case you want to be sure to
seal the setup at a given point.

For example, you may need to seal the configuration and generate the metadata
file. In that case, you can explicitly set `started` to `True`.
