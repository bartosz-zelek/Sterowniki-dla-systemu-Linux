Docea Base 6 Release Notes
==========================

## Changes
  * 6.0.43
    * Introduce the ability to create Docea writable and readable objects using DoceaPy's SimulableModel API, removing the need for metadata files for object creation.
    * Add a `power` nature to the readable objects, enabling differentiation between power and currents in simulation logs.
    * Enable the configuration of heat transfer coefficient parameters during simulations through Docea writable objects.
    * Add support for managing power consumer temperatures through writables objects for input temperatures, readable objects for output temperatures, and new class of readable/writable objects for both input and output.
    * Remove deprecated classes for PPMF models: `physical_connector` and `physical_connector_v2`.
    * Remove deprecated classes supporting results collection: `physical_collector`, `physical_observer`, `physical_tracer`, and `isim_hub`. Use the probe framework instead.
    * Remove deprecated targets: `docea/docea_model_objects.py`, `docea/docea-initial-time.simics`, and `docea/docea-physical.simics`.

  * 6.0.42
    * Integrate support for newest physical model from DoceaPy.
    * Deprecate physical-connector-v2, physical-tracer-v2, docea-streamer, docea-probe (V1).
    * Handle NULL/None values when probing power string variables.

  * 6.0.41
    * Fix integration of the docea-probe-array module which could not actually be loaded in version 6.0.40.

  * 6.0.40
    * Integrate the docea-probe-array module which offers enhanced performance and simplified usage compared to the Docea probes tech preview.
    * Incorporate the isim-ptm module, which includes the ISIM PTM V1 contract and the ptm_wrapper class. The ISIM PTM V1 contract specifies the required interface for any Python-based power/thermal management algorithm for integration into ISIM, while the ptm_wrapper class encapsulates such algorithms into a Simics class for use within ISIM.
    * Implement the periodic-ticker module that features the periodic_ticker Simics class, designed to generate periodic notifications at user-configurable intervals.

  * 6.0.39
    * Integrate support to SPRIMA thermal model simulation.
    * Integrate fixes to source temperature probing.

  * 6.0.38
    * Make Docea Base compatible with Simics 6.0.178+ and incompatible with 
      prior versions.

  * 6.0.37
    * Allow to configure retention of buffered events on restart.

  * 6.0.36
    * Integrate security and legal fixes.
    * Integrate Docea Base tutorial.

  * 6.0.35
    * Optimize Docea probes creation by reducing the metadata parsing time.

    * **Tech preview**: allow changing the initial time when restarting the
      physical connector (restore of a power/thermal simulation from a state).

    * **Tech preview**: introduce a `PowerThermalSimulator` object in pure
      Python to ease the setup of the power/thermal simulation.

  * 6.0.34
    * **Tech preview**: support checkpoint with simulator "v3".
      * The `physical_connector_v3` is now checkpointable, so it can be saved
        and restored when the Simics commands for manipulating checkpoints are
        used.
      * The power/thermal state attribute can be read any time from the
        `physical_connector_v3`.
      * The power/thermal state attribute can be written into the
        `physical_connector_v3` and it will be used when the latter is started.
        The `physical_connector_v3` also implements a new `docea_session`
        interface that allows to `restart` the connector any time, hence
        restoring a power/thermal simulation if a state was previously provided.
    * **Tech preview**: better memory control in the power/thermal simulation,
      thanks to an attribute to configure the size of the "retained events"
      window in the `physical_connector_v3`.

  * 6.0.33
    * Integrate security fixes.

  * 6.0.32
    * Integrate security fixes.

  * 6.0.31
    * Integrate fixes reducing memory footprint for long simulations and 
    optimizations when a physical model does not contain a thermal model.

  * 6.0.30
    * Integrate a fix on incorrect simulator behavior provoking null 
    CurrentMode and NaN values during power model simulation.

  * 6.0.29
    * Integrate the `docea-streamer` module that allows users to collect 
      and dump Docea's probe values in a CSV file.

  * 6.0.28
    * Integrate the support of thermal models with Active cooling enabled.
      More generally, all physical models with thermal components coming from an
      iDTP 7.2 export are now supported (thermal metadata version 1.1.0).
      Models may include dense matrices.

  * 6.0.27
    * Integrate fixes on the support of thermal models with Reference
      Temperatures that was not properly working in the 6.0.25 and 6.0.26
      releases. More generally, physical models with thermal components coming
      from an iDTP 7.2 export are now supported, as long as these models do not
      have Active Cooling enabled.

  * 6.0.26
    
    * Add tech. preview of Docea probes for input/output and internal physical
      model variables.

  * 6.0.25

    * Integrate support for thermal models that include Reference Temperatures
      in Physical models used for ISIM. Reference temperatures are a property of
      thermal Exchange Regions, indicating that the environment they exchange
      heat with has a fixed temperature other than the ambient.
      Note that reference temperatures are not supported in the legacy,
      PPMF-based, flow.

    * Physical models with thermal model set to 'None' are now supported for
      simulation in ISIM.

  * 6.0.24
  
    * Make the internal release of Docea Base (features contained in version 
      6.0.22) compatible with Simics 6.0.155 and Python 3.9.

  * 6.0.23

    * Recreate package Docea Base 6.0.16 to make it compatible with Simics 6.0.155 
      and Python 3.9 for public ISIM release.

  * 6.0.22

    * Enable event-based probes: users can inspect the value and time in the 
      Docea `read-float`, `written-float`, and `written-string` objects by 
      reading from probes at the event of the physical connector having results 
      ready. 

    * Introduce a new simulator "v3" available as a **tech preview**:
      - A new module `physical_connector_v3` allows to instantiate and configure 
      the new simulator. It can be used seamlessly with the power/thermal model 
      objects (`read-float`, `written-float`, and `written-string` objects).
      - The main difference of the connector v3 w.r.t. the v2 is the simulated 
      model. The v3 simulates DoceaPy models. Please refer to the documentation
      for details.
      - The power/thermal model objects also support the new simulator v3 
      without requiring any specific configuration.

  * 6.0.21

    * Enable simulation probes: users can inspect the value and time in the 
      Docea `read-float`, `written-float`, and `written-string` objects by using 
      probes. 

    * Fix the Windows packaging to include only `py` files instead of compiled 
      `pyc` files to enable Python 3.9 support by Simics.

  * 6.0.20

    * Update packaging to enable Python 3.9 support by Simics. Now the `py`
      files of the modules replace the compiled `pyc` files in the package.

  * 6.0.19

    * Restore the simulation speed of the `physical_tracer_v2` to match the one
      of the legacy tracer in most cases, thanks to an internal cache for the
      computed results.

    * Relax the constraint of some objects that required their `time_provider`
      attribute to be connected to an object implemeting the `cycle` interface.
      Now the `time_provider` can be any object that "handles time in any way"
      (i.e., its `queue` attribute is properly set).

  * 6.0.18

    * The `physical_tracer_v2` module exposes the new attribute `max_total_size_gb` that lets users configure the maximum allowed size of the CSV trace file.
    
    * The `float-aggregator` and `float-forwarder` modules are now checkpointable.
    Users can save and restore simulation checkpoints of platforms containing these modules.

    * The `float-aggregator` module handles initial simulation times that are different from zero and possibly large. 
    To this end, the module extends its first aggregation interval to go from zero to the initial time.
    This way, users can set large initial times without this module aggregating over too many invalid intervals.  

    * The `model_objects` Python API now exposes only three public functions: `create_from_metadata_file`, `set_time_provider`, and `set_time_provider_to_hierarchy`. 
    Users can import this API from the new `power_thermal` module.
    
    * The simulator logs a warning message when users instantiate or invoke deprecated modules and features.

  * 6.0.17

    * Bring in a new simulator "v2" with a coherent and clearer behavior:

      * A new module `physical_connector_v2` allows to instantiate and configure
        the new simulator. It can be used seamlessly with the power/thermal
        model objects ("writable" and "readable" objects). It also replaces the
        capabilities of the observer and it can be connected directly to a new
        tracer (see item below).

      * A new module `physical_tracer_v2` allows to trace power/thermal
        parameters using the new simulator. It does not need an additional
        observer module and it can be connected directly to the
        `physical_connector_v2`.

      * The power/thermal model objects ("writable" and "readable" objects)
        support both the legacy simulator and the new simulator "v2". No
        specific configuration is required to these objects.

      * The behavior of the new simulator is *coherent*: it does not mix anymore
        power/thermal value changes as forward intervals (simulation timestamp
        at the beginning of the interval) with changes as backward intervals
        (simulation timestamp at the end of the interval). All the changes are
        expected as forward intervals. Among the benefits, it is now easy to
        initialize power/thermal parameters.

      * The behavior of the new simulator is *clearer*: because all the
        power/thermal value changes are expressed as forward intervals, it is
        always possible to read a temperature or a current. The read error
        "missing events" does not exist anymore. In case of results_delay
        (configured to cope with temporal decoupling and other delay sources),
        the temperatures and currents are systematically read at the "requested
        time minus the delay".

      * The legacy modules `physical_connector`, `physical_observer` and
        `physical_tracer` are deprecated but they are still available. This
        allows to smoothly transition to the new simulator.

    * Make averaging of values explicit and controllable:

      * The new simulator does not perform ad-hoc averaging of activity inputs
        (Cdyn) "under the hood" anymore.

      * A new object `float_aggregator` can be instantiated and plugged wherever
        averaging over fixed intervals is required. The `float_aggregator`
        supports the telemetry interface and the notifier mechanism: it can be
        connected to a telemetry provider (to get raw float values and
        timestamps), it can listen to a notifier (to decide when to read
        values), it exposes the average as a telemetry and it notifies whenever
        a new average is available. It is also a "writable" object, meaning that
        raw values can be explicitly written into it.

      * A new object `float_forwarder` can be instantiated and plugged wherever
        a conversion is required from backward intervals (value changes where
        the simulation timestamp is at the end of the interval) to forward
        intervals (timestamp at the beginning). Like the aggregator, it supports
        the telemetry interface and the notifier mechanism.

      * **Known limitation**: the `float_aggregator` and the `float_forwarder`
        are not checkpointable yet.

    * Fully support telemetry in the power/thermal model objects:

      * The "readable" objects are telemetry providers (they implement
        telemetry). However, since they provide timestamped values, in order to
        enable telemetry they must be configured with a time provider (a valid
        picosecond clock). Two new functions in the Python API allow to set a
        time provider to a collection of objects or directly to a whole
        namespace.

      * All the "writable" objects, including those of type "string", can be
        connected to a telemetry provider. They can listen to notifications to
        decide when to fetch values to send to the physical simulator.

      * The name of the attributes to configure telemetry has been clarified and
        made coherent across all the modules (model objects, aggregator,
        forwarder...).

    * Quality enhancements and bug fixes:

      * The tracer now checks whether the connector has been configured with a
        `ppmf_metadata`.

      * Warn instead of raising errors in case of unknown telemetry names or
        IDs (aligned with the telemetry interface specification).

      * Fix attributes documentation about default values in the
        `physical_connector`.

      * Add a short description for the package.

      * Correctly implement `info` and `status` commands on the base new
        modules.

      * Avoid some spurious error messages during telemetry configuration.

      * Workflow enhancement: simplify the package release process.

      * Workflow enhancement: the simulation performance is tracked on Linux, by
        measuring the real time spent to complete given simulations. The
        collection of cases is still to be incremented, but it already includes
        a first VP trace plus some hand-written stimuli patterns.

    * **NEW DEPRECATIONS:**

      * The `physical_connector` is deprecated and replaced by
        `physical_connector_v2`.

      * The `physical_observer` is deprecated and replaced by
        `physical_connector_v2`.

      * The `physical_tracer` is deprecated and replaced by
        `physical_tracer_v2`.


  * 6.0.16

    * Restore the `THREAD_SAFE` flag for the `physical_connector` and for the
      `physical_observer`. These modules behave like "passive" devices and they
      are to be considered thread-safe for Simics simulations. Other "initiator"
      modules that call the interface methods of the connector/observer are
      responsible for properly acquiring the thread domains.

    * Support Microsoft Windows by providing a package for Windows.

    * Support the new Simics Package Manager by providing packages in ISPM
      format.

    * Provide a new set of APIs to drive and observe the simulation based on
      power/thermal names and picoseconds.

      * The new APIs are defined by the new interfaces `docea_ptm2`,
        `docea_perf2` and `docea_obs2`.

      * These APIs replace the deprecated numerical ID parameter by a character
        string ID parameter that matches the path (a hierarchical logical name)
        in the physical model.

      * These APIs replace the deprecated representation of time as a
        floating-point number of seconds by an integer number of picoseconds.

      * The `physical_tracer` now dumps the time column as an integer number of
        picoseconds in the CSV file.

    * Enhance the configuration of the the `physical_connector`.

      * A new set of attributes, matching the time representation in
        picoseconds, is available.

      * The new attributes can be set in any order, before the simulation is
        started.

      * Each attribute represent a single parameter of the configuration,
        instead of grouping multiple parameters.

      * The configuration flow is clarified and made more robust.

    * Improve the security and the code quality with various fixes and by
      unifying recommended compilation flags.

    * Remove support of the deprecated `docea_sim` interface.

    * Remove the deprecated and flawed `physical_introspector` module.

    * DEPRECATIONS

      * Deprecate the old interfaces `docea_ptm`, `docea_perf` and `docea_obs`.
        These interfaces were relying on hardware IDs and they have been
        superseded by new versions using logical names. They will be removed in
        the upcoming releases.

      * Deprecate the old configuration attributes of the `physical_connector`.
        They will be removed in the upcoming releases.

      * Deprecate the old row-based CSV streaming based on the `isim_hub` and
        the `physical_collector`. These modules will be removed in the upcoming
        releases.

      * Deprecate docea target scripts. They will be removed in the upcoming
        releases. The user is encouraged to instantiate and configure the
        objects explicitly.

  * 6.0.15

    * First stable version promoting pre14 version.

  * 6.0.pre14

    * Flag `physical_connector` and `physical_observer` modules as **not**
      thread-safe.

  * 6.0.pre13

    * Switch thermal simulation to mono-thread. In most cases we have observed a
      degradation of performance when multi-threadable calculations are enabled
      in the thermal simulator. This fix forces the number of threads to 1 for
      thermal multi-threadable calculations.

  * 6.0.pre12

    * Output physical data (currents, temperatures, voltages...) to a
      column-based CSV.

      * A `physical_tracer` object can be instantiated and used to stream
        physical data to a file in CSV format. It can be configured with the
        list of parameters to trace.

    * Support state change.

      * The `docea_ptm` interface declares a new method `update_state` to send
        state changes (C-state, D-state...) to the physical simulator. The
        `physical_connector` implements this new method.

      * The physical model writable objects support states. "Writable string"
        objects are now instantiated and can be used to send state changes.

    * Make types returned by interface methods allocatable in Python.

  * 6.0.pre11

    * Support notifier API and telemetry interface.

      * The physical model writable objects support the notifier API and can be
        configured with a notifier. For example, a writable "Cdyn" object can be
        configured to be automatically notified each time a new cdyn timed value
        is available.

      * The physical model writable objects support the telemetry interface,
        that is, they can fetch new values from a object implementing the Simics
        telemetry interface. For example, a writable "Cdyn" object can be
        configured to automatically fetch the cdyn and time values from a
        source each time it is notified by a notifier.

  * 6.0.pre10

    * Remove deprecated support of the `simple_timing_v2` interface in the
      `physical_connector`. Using the physical model objects is the preferred
      way for sending cdyn values to the physical simulator.

    * Support the `docea_perf` interface in DML.

    * Support the physical model objects in DML (the interfaces implemented by
      the objects are available in DML).

    * Use picoseconds in the physical model objects. The time value used when
      reading or writing the objects is expected to be an integer number of
      picoseconds, representing the absolute simulation time.

  * 6.0.pre9 (build intel.docea:14)

    * Add `aggregated_cdyn` to the `docea_obs` interface, to read cdyn values
      averaged over time windows (when aggregation is enabled) and used by the
      solver for computation.

    * Fix the documentation about the default behavior for the CSV file streamer.

    * Add a new binding capability through physical model "proxy" objects.

      * As an alternative to using the ID-based API of the `physical_connector`,
        the physical simulation can be driven by directly writing to / reading
        from objects that match the paths defined at the interface inside the
        PPMF metadata file.

      * A Python function allows to load a PPMF metadata file and automatically
        instantiate all the corresponding objects.

  * 6.0.pre8 (build intel.docea:13)

    * Enable runtime flush of the CSV trace file.

      * The CSV trace file is now constantly filled during simulation, instead
        of being flushed at the end of the simulation. The number of CSV lines
        accumulated in a buffer before being dumped can be controlled with the
        `csv_buffer_size` attribute of the `isim_hub`.

  * 6.0.pre7 (build intel.docea:12)

    * Add missing class documentation line to the sources of `isim_hub` and
      `physical_collector`.

  * 6.0.pre6 (build intel.docea:11)

    * Fix the return type of the methods of the `docea_obs` interface
      implemented by the physical observer.

  * 6.0.pre5 (build intel.docea:10)

    * Fix the cdyn aggregation method.

      * Only definitely closed aggregation windows are taken into account to
        compute the outputs of the physical simulator.

    * Add a physical observer module.

      * A `physical_observer` object can be instantiated to read values from the
        physical simulator without sending any new event to it, that is, without
        having any direct side-effect on the physical simulation.

    * Enable export of physical data in CSV trace file.

      * An `isim_hub` object and a `physical_collector` object can be
        instantiated to gather physical data and write them in a CSV trace file.

  * 6.0.pre4 (build 8)

    * Update packaging.

      * The package is built using a more classic process and it embeds an
        installer.

  * 6.0.pre3 (build 7)

    * Fix temporal decoupling support.

      * The physical simulator supports temporal decoupling via a
        `results_delay` parameter. In case of temporal decoupling, it is
        possible to set the `results_delay` according to the value of the
        temporal decoupling max time interval (sometime named "time quantum").
        This way, the physical simulator will take into account the specified
        delay and avoid returning outputs computed on a time interval where
        the inputs could still change.

    * Add a platform-independent "performance" interface.

      * A new interface `docea_perf` allows to send cdyn values (estimated by
        a performance model) to the physical simulator using core IDs instead
        of core objects. The interface is public in the "Docea Base" package.
      * For the moment, the `physical_connector` implements both the new
        `docea_perf` interface and the one named `simple_timing_v2` (provided
        by CAPP). The latter yields a platform-dependency because the module
        that implements it must know how to extract a core ID. Its support
        will be dropped in future releases.

  * 6.0.pre2 (build 6)

    * Support checkpointing.

      * The package modules support checkpoint and restore (when the Simics
        commands `write-configuration` and `read-configuration` are used).
      * The `physical_configuration` attribute of the `physical_connector`
        object returns the serialized "physical state", in addition to the
        PPMF path and initial time.

  * 6.0.pre1 (build 5)

    * Fix and improve the "Power/Thermal Management" interface.

      * The new version of the `docea_sim` interface is named `docea_ptm` and
        it is made public in the "Docea Base" package.
      * The spurious `socketID` parameter is removed from the function for
        reading temperature in the `docea_ptm` interface.
      * The `docea_ptm` interface is written in C, but it is now Python and
        DML friendly.

  * 6.0.pre0 (build 2)

    * Initial version.
