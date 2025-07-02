# © 2023 Intel Corporation

simics_add_module(empty-components
  COMPONENTS empty_components
  SOURCES empty_components.py module_load.py
  SIMICS_API latest
)
