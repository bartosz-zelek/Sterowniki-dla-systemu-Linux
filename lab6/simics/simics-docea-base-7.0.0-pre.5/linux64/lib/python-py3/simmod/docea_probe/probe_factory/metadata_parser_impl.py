# -*- coding: utf-8 -*-

import functools
import json
from typing import Callable, Dict, List


class MetadataParser():
    def __init__(self, namespace: str, metadata: str):
        self.namespace = namespace
        with open(metadata) as metadata_file:
            self.metadata_json = json.loads(metadata_file.read())
        self._natures_dict = self._get_natures_dict()
        self._input_names = self._get_io_names("inputs")
        self._output_names = self._get_io_names("outputs")

    def get_power_thermal_data(
            self, filter_func: Callable[[str, str, str],
                                        bool]) -> List[Dict[str, str]]:
        data = self._get_power_thermal_data()
        filtered = list(
            filter(
                lambda entry: self._apply_filter_to_entry(filter_func, entry),
                data))
        return filtered

    def _get_power_thermal_data(self) -> List[Dict[str, str]]:
        inputs = self._get_inputs_data()
        outputs = self._get_outputs_data()
        internals = self._get_internal_data()
        return inputs + outputs + internals

    def _get_inputs_data(self) -> List[Dict[str, str]]:
        return (self._get_input_output_data("inputs", "cdyns")
                + self._get_input_output_data("inputs", "frequencies")
                + self._get_input_output_data("inputs", "states")
                + self._get_input_output_data("inputs", "voltages"))

    def _get_input_output_data(self, input_or_output: str,
                               field: str) -> List[Dict[str, str]]:
        names = self._get_input_output_names(input_or_output, field)

        def get_nature(name: str, field: str) -> str:
            if field != "temperatures":
                return self._field_to_nature(field)
            return self._natures_dict[name]

        probe_type = "input" if input_or_output == "inputs" else "output"
        data = map(
            lambda el: self._build_element_power_thermal_data(
                el, probe_type, get_nature(el, field)), names)
        return list(data)

    def _get_input_output_names(self, input_or_output: str,
                                field: str) -> List[str]:
        if field not in self.metadata_json["interface"][input_or_output]:
            return []
        entries = self.metadata_json["interface"][input_or_output][field]
        return list(map(lambda x: x["name"], entries))

    def _field_to_nature(self, field: str) -> str:
        field_nature_map = {
            "cdyns": "cdyn",
            "frequencies": "frequency",
            "states": "state",
            "voltages": "voltage",
            "currents": "current"
        }
        return field_nature_map[field]

    def _build_element_power_thermal_data(self, name: str, type: str,
                                          nature: str) -> Dict[str, str]:
        data = {}
        data["power_thermal_name"] = name
        data["type"] = type
        data["nature"] = nature
        return data

    def _get_outputs_data(self) -> List[Dict[str, str]]:
        return (self._get_input_output_data("outputs", "currents")
                + self._get_input_output_data("outputs", "temperatures"))

    def _get_internal_data(self) -> List[Dict[str, str]]:
        names = self._get_probable_power_thermal_names()
        internal_names = list(
            set(names) - set(self._input_names) - set(self._output_names))
        data = map(
            lambda n: self._build_element_power_thermal_data(
                n, "internal", self._natures_dict[n]), internal_names)
        return list(data)

    def _apply_filter_to_entry(self, filter_func: Callable[[str, str, str],
                                                           bool],
                               entry: Dict[str, str]):
        return filter_func(entry["power_thermal_name"], entry["type"],
                           entry["nature"])

    def _get_probable_power_thermal_names(self) -> List[str]:
        names = self._get_probable_power_names()
        names += self._get_probable_thermal_names()
        return names

    def _get_probable_power_names(self) -> List[str]:
        return list(map(lambda x: x["path"], self._get_power_variables()))

    def _get_power_variables(self) -> List[Dict[str, str]]:
        if "power_variables" in self.metadata_json:
            return self.metadata_json["power_variables"]
        return []

    def _get_probable_thermal_names(self) -> List[str]:
        names = self._get_thermal_probe_names()
        names += self._get_thermal_source_names()
        return names

    def _get_thermal_probe_names(self) -> List[str]:
        outputs = self._get_thermal_outputs()
        filtered = filter(lambda output: output["type"] == "probe", outputs)
        names = map(lambda x: x["path"], filtered)
        return list(names)

    def _get_thermal_outputs(self) -> List[Dict[str, str]]:
        try:
            return self.metadata_json["thermal_outputs"]
        except Exception:
            return []

    def _get_thermal_source_names(self) -> List[str]:
        thermal_vars = self._get_thermal_outputs() + self._get_thermal_inputs()
        filtered = filter(lambda x: "source" in x["type"], thermal_vars)
        names = list(map(lambda x: x["path"], filtered))
        names = self._remove_duplicates(names)
        return names

    def _get_thermal_inputs(self) -> List[Dict[str, str]]:
        try:
            return self.metadata_json["thermal_inputs"]
        except Exception:
            return []

    def _remove_duplicates(self, names: List[str]) -> List[str]:
        return list(set(names))

    def _get_io_names(self, input_or_output) -> List[str]:
        natures = self.metadata_json["interface"][input_or_output]
        # Each nature defines a list, this line flattens them
        entries = functools.reduce(lambda x, y: x + y, natures.values(),
                                   list())
        names = map(lambda x: x["name"], entries)
        return list(names)

    def _get_natures_dict(self):
        natures_dict = {}
        for power_thermal_name in self._get_thermal_probe_names():
            natures_dict[power_thermal_name] = "probe_temperature"
        for power_thermal_name in self._get_thermal_source_names():
            natures_dict[power_thermal_name] = "source_temperature"
        for var in self._get_power_variables():
            natures_dict[var["path"]] = "unknown_" + var["type"]
        return natures_dict
