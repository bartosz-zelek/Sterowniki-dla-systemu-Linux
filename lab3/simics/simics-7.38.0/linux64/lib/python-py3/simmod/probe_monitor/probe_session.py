# © 2024 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


# Takes a dict (from json) with probe information which has been generated
# and wrap this as a class where the probe-data can be accesses as if they
# are probe-objects. Also creates 'class-probes' for probes which share the
# same simics-class.

import collections
import re

from dataclasses import dataclass
from typing import (Optional, Any)

# Note, this is just to get hold of probes.probe_type_classes
# We need this functionality to be available outside of Simics too
import probes
import table

exec_mode_re = re.compile(
    "cls[.]cpu[.]exec_mode[.](hypersim|jit|vmp|interpreter)_percent")

def drop_prefix(k):
    if ":" in k:
        return k.split(':')[1]
    return k

def format_probe_value(probe, cell_value):
    float_decimals = None
    # TODO: pass this instead of createing a new for each point
    prop_obj = table.cell_props.CellProps(probe.table_properties())
    cell = table.cell.data_to_cell(cell_value, float_decimals, prop_obj)
    val_str = "\n".join([cell.line(i) for i in range(cell.num_lines())])
    return val_str

def dict_to_probe(probe_dict):
    # Ignoring some probe properties not used:
    # - desc, metric, definition, binary, time_fmt, width
    return Probe(kind=probe_dict["kind"],
                 mode=probe_dict["mode"],
                 owner=probe_dict["owner"],
                 clsname=probe_dict["classname"],
                 ptype=probe_dict["type"],
                 display_name=probe_dict["display_name"].replace('\n', ' '),
                 float_decimals=probe_dict["float_decimals"],
                 percent=probe_dict["percent"],
                 raw_sample_history=probe_dict["raw_sample_history"],
                 final_value=probe_dict["final_value"],
                 final_value_cell=probe_dict["final_value_cell"],
                 final_value_fmt=probe_dict["final_value_fmt"],
                 metric=probe_dict["metric"],
                 binary=probe_dict["binary"],
                 time_fmt=probe_dict["time_fmt"])


# Representation of a probe, converted from the json format or generated
# as class-probes from other object-probes of the same Simics class.
@dataclass
class Probe:
    # Required, used by normal and class-probes
    kind : str
    mode : str
    ptype : str
    owner : str
    display_name : str
    raw_sample_history : [Any]
    final_value : [Any]

    # Optional
    clsname : Optional[str] = ""
    float_decimals : Optional[int] = None
    percent : Optional[bool] = False
    final_value_cell : Optional[Any] = None
    final_value_fmt : Optional[str] = None
    metric: Optional[str] = None
    binary: Optional[str] = None
    time_fmt: Optional[bool] = False
    cls_probe: Optional[bool] = False

    @property
    def sample_history(self):
        if not hasattr(self, "plot_formatter"):
            self.plot_formatter = probes.CellFormatter(
                max_lines=10,
                ignore_column_widths=True)

        cls = probes.probe_type_classes.get_value_class(self.ptype)
        return [cls.table_cell_value(v, self.plot_formatter)
                for v in self.raw_sample_history]

    @property
    def global_probe(self):
        return self.owner in ["sim", "host"]

    # Create the corresponding table-properties to format the
    # value according to the probe's properties.
    def table_properties(self):
        l = [(table.Column_Key_Int_Radix, 10)]
        if self.percent:
            l.append((table.Column_Key_Float_Percent, True))
        if self.float_decimals:
            l.append((table.Column_Key_Float_Decimals, self.float_decimals))
        if self.metric != None:
            l.append((table.Column_Key_Metric_Prefix, self.metric))
        if self.binary != None:
            l.append((table.Column_Key_Binary_Prefix, self.binary))
        if self.time_fmt:
            l.append((table.Column_Key_Time_Format, self.time_fmt))
        return l

# FakeProbe used when there is no probe, returning "n/a" for any
# member accessed
class FakeProbe:
    def __getattr__(self, a):
        return "n/a"

class SimicsSession:
    def __init__(self, json):
        self.json = json

        # Create a dict of the probes in json format represented as
        # Probe objects
        self.probes = {
            key : dict_to_probe(value)
            for (key, value) in self.json["target"]["probes"].items()
        }
        self._generate_class_probes()

        # All probe-name used
        kinds = {p.kind for p in self.probes.values()}
        self.all_probe_kinds = kinds

        # All probes used with object prefix and mode suffix
        names = set(self.probes.keys())
        self.distinct_probe_names = names

        # All probes, but with removed object prefix
        names = {drop_prefix(n) for n in names}
        self.no_prefix_probe_names = names

    @staticmethod
    def _get_aggregate_function_for_probe(probe_name, ptype):
        type_class = probes.probe_type_classes
        if ptype != "fraction":
            if not type_class.supports_aggregate_function(ptype, "sum"):
                return None
            return type_class.get_aggregate_function(ptype, "sum")

        # Check fractions. How class probe's data should be
        # constructed, depends on what the fractions represents. There
        # is currently no hint in properties for this.

        # Uses common denominator, do a simple sum of the fractions
        if probe_name in ["cls.cpu.schedule_percent",
                          "cls.cpu.load_sim_percent"]:
            return type_class.get_aggregate_function(ptype, "sum")

        mo = exec_mode_re.match(probe_name)
        if mo:
            return type_class.get_aggregate_function(ptype,
                                                     "weighted-arith-mean")

        if probe_name in ["cls.cpu.mips"]:
            return  type_class.get_aggregate_function(ptype,
                                                      "weighted-arith-mean")

        # Unspecified fraction, do not sum it
        return None

    def _generate_class_probes(self):
        cls_history = {}
        for k, v in self.probes.items():
            if v.global_probe:
                continue # Nothing to aggregate on global probes

            (obj, probe_name) = k.split(":")
            cls = v.clsname
            cls_probe = f"<{cls}>:cls.{probe_name}"
            cls_history.setdefault(cls_probe, [])
            cls_history[cls_probe].append(v)

        for k, v in cls_history.items():
            type_set = {co.ptype for co in v}
            if len(type_set) != 1:
                # different types
                continue
            ptype = type_set.pop()
            (cls, cls_probe_kind) = k.split(':')
            aggregate_fun = self._get_aggregate_function_for_probe(
                cls_probe_kind, ptype)
            if not aggregate_fun:
                continue        # Not supported to aggregate

            sample_history = []
            for l in zip(*[co.raw_sample_history for co in v]):
                row = aggregate_fun(l)
                sample_history.append(row)

            # Get hold of class for this probe-type
            probe_cls = probes.probe_type_classes.get_value_class(ptype)

            final_value = aggregate_fun([co.final_value for co in v])
            final_value_cell = probe_cls.table_cell_value(final_value)
            final_value_fmt = format_probe_value(v[0], final_value_cell)

            # Add the new class probe
            self.probes[k] = Probe(
                kind=f"cls.{v[0].kind}",
                mode=v[0].mode,
                ptype=ptype,
                owner=cls,
                display_name=v[0].display_name,
                raw_sample_history=sample_history,
                final_value=final_value,
                final_value_cell=final_value_cell,
                final_value_fmt=final_value_fmt,
                cls_probe=True
            )


    def get_object_probes(self, probe_name):
        y_probes = []
        for k, v in self.probes.items():
            if v.global_probe:  # Skip global classes
                continue
            (obj, pn) = k.split(":")
            if pn == probe_name:
                y_probes.append(k)
        return y_probes

    def get_object_probes_from_wildcard(self, obj_name, probe_kind):
        y_probes = []
        for k, v in self.probes.items():
            try:
                (obj, pk) = k.split(":")
            except Exception as msg:
                print("probe-name:", k)
                raise Exception(msg)
            if pk == probe_kind:
                if obj_name == "*" or obj_name == obj:
                    y_probes.append(k)

        return y_probes

    def get_wildcard_objects(self, probe_kind):
        objs = set()
        for k, v in self.probes.items():
            (obj, pk) = k.split(":")
            if pk == probe_kind:
                objs.add(obj)
        return objs

    def summary_data(self):
        target = self.json["target"]
        host = self.json["host"]
        no_probe = FakeProbe()
        return collections.OrderedDict({
            "Workload": target.get("workload", "n/a"),
            "Date": self.json.get("date", "n/a"),
            "Target": target.get("CPU summary", "n/a"),
            "Wallclock Time": self.probes.get("sim:sim.time.wallclock-session",
                                              no_probe),
            "Virtual Time": self.probes.get("sim:sim.time.virtual-session",
                                            no_probe),
            "Slowdown": self.probes.get("sim:sim.slowdown", no_probe),
            "MIPS": self.probes.get("sim:sim.mips", no_probe),
            "Target load%": self.probes.get("sim:sim.load_percent", no_probe),
            "Host HW": host["CPU brand"] + (
                " [ "
                + str(host["CPU cores"]) + " cores "
                + "@ " + ",".join(str(int(n)) for n in host["CPU max freqs"])
                + " MHz"
                + " ]"
                ),
            "Host SW": (host["OS"]
                        + ("Hypervisor:" + host["hypervisor"]
                           if host["hypervisor"] != "no" else "")
                        ),
            "Simics CPU load%": self.probes.get("sim:sim.process.cpu_percent",
                                                no_probe),
            "Threading mode": self.json["simics"]["Threading mode"]
        })

    def host_data(self):
        return self.json["host"]

    def simics_data(self):
        return self.json["simics"]

    def target_data(self):
        d = self.json["target"]
        d.pop("probes")
        return d

    def probes_data(self):
        return self.probes

    @staticmethod
    def _long_probe_name(p):
        if p.mode == "delta":
            return p.kind
        return f"{p.kind}-{p.mode}" # Append "current" or "session"

    # Returns the full name of the probe, including the obj:kind-[mode]
    def global_probe_names_no_histograms(self):
        return sorted(
            {k for (k,v) in self.probes.items()
             if v.global_probe and (v.ptype != "histogram")
             })

    def class_probe_names_no_histograms(self):
        return sorted(
            {self._long_probe_name(v) for (k,v) in self.probes.items()
             if (not v.global_probe) and (v.cls_probe) and (v.ptype != "histogram")
             })

    def object_probe_names_no_histograms(self):
        return sorted(
            {self._long_probe_name(v) for (k,v) in self.probes.items()
             if (not v.global_probe) and (not v.cls_probe) and (v.ptype != "histogram")
             })


    def flamegraph_data(self):
        if not "flamegraph" in self.json:
            return None
        return self.json["flamegraph"]

    # Returns True if any of the probes are not present
    def probes_missing(self, probe_names):
        return not set(probe_names).issubset(self.no_prefix_probe_names)
