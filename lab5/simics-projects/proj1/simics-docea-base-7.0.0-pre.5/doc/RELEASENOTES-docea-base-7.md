Docea Base 7 Release Notes
==========================

## Changes
  * 7.0.0-pre5
    * Introduce the ability to create Docea writable and readable objects using DoceaPy's SimulableModel API, removing the need for metadata files for object creation.
    * Add a `power` nature to the readable objects, enabling differentiation between power and currents in simulation logs.
    * Enable the configuration of heat transfer coefficient parameters during simulations through Docea writable objects.
    * Add support for managing power consumer temperatures through writables objects for input temperatures, readable objects for output temperatures, and new class of readable/writable objects for both input and output.
    * Remove deprecated classes for PPMF models: `physical_connector` and `physical_connector_v2`.
    * Remove deprecated classes supporting results collection: `physical_collector`, `physical_observer`, `physical_tracer`, and `isim_hub`. Use the probe framework instead.
    * Remove deprecated targets: `docea/docea_model_objects.py`, `docea/docea-initial-time.simics`, and `docea/docea-physical.simics`.

  * 7.0.0-pre4
    * Integrate support for newest physical model from DoceaPy.
    * Deprecate physical-connector-v2, physical-tracer-v2, docea-streamer, docea-probe (V1).
    * Handle NULL/None values when probing power string variables.
  
  * 7.0.0-pre3
    * Fix integration of the docea-probe-array module which could not actually be loaded in version 7.0.0-pre2.

  * 7.0.0-pre2
    * Integrate the docea-probe-array module which offers enhanced performance and simplified usage compared to the Docea probes tech preview.
    * Incorporate the isim-ptm module, which includes the ISIM PTM V1 contract and the ptm_wrapper class. The ISIM PTM V1 contract specifies the required interface for any Python-based power/thermal management algorithm for integration into ISIM, while the ptm_wrapper class encapsulates such algorithms into a Simics class for use within ISIM.
    * Implement the periodic-ticker module that features the periodic_ticker Simics class, designed to generate periodic notifications at user-configurable intervals.

  * 7.0.0-pre1
    * Second preview of Docea Base package for Simics 7

  * 7.0.0-pre0
    * First preview of Docea Base package for Simics 7
