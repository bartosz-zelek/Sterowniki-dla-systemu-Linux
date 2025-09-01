# © 2023 Intel Corporation

simics_add_module(first_device
  CLASSES first_device
  SOURCES first_device.dml module_load.py
  SIMICS_API 7
)
add_subdirectory(test)
