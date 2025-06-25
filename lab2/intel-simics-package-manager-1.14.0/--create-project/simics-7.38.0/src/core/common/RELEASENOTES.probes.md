<!---
© 2024 Intel Corporation

This software and the related documents are Intel copyrighted materials, and
your use of them is governed by the express license under which they were
provided to you ("License"). Unless the License provides otherwise, you may
not use, modify, copy, publish, distribute, disclose or transmit this software
or the related documents without Intel's prior written permission.

This software and the related documents are provided as is, with no express or
implied warranties, other than those that are expressly stated in the License.
-->

# Probes Framework

- `major 6`
- `note 6` Prevent run-time division-by-zero errors for aggregates of
  the fraction type, (fixes HSD-18039438849).
- `release 6 6324`
- `release 7 7034`
- `note 6` The commands `probes.new-attribute-probe`, `probes.new-percent-probe`,
           `probes.new-fraction-probe`, `probes.new-sum-probe`,
	   and `probes.new-aggregate-probe` now print the probe-kind
	   created, and return this name for scripting,
	   (fixes SIMICS-22706).
- `release 6 6335`
- `release 7 7044`
- `note 6` Improved the functionality and documentation of the
  `probes.new-aggregate-probe`, `probes.new-attribute-probe`, and
  `probes.new-percent-probe` commands.
- `release 6 6337`
- `release 7 7045`
