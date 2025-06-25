from dataclasses import dataclass
from importlib import import_module
from typing import Any, Dict, List, Optional

import pyobj
import simics_6_api as simics

from .isim_ptm_v1 import PTMError, PTMInputs, PTMOutputs, PTMValues


@dataclass
class _Telemetry:
    source: Any
    class_name: str
    class_id: int
    tele_name: str
    tele_id: int
    ptm_name: str
    value: Any


class _TelemetryIndex:
    def __init__(self) -> None:
        self.clear()

    def __unknown(self) -> _Telemetry:
        return _Telemetry(None, None, -1, None, -1, None, 0.0)

    def add_telemetry(self, source: Any, class_name: str, tele_name: str,
                      ptm_name: str) -> _Telemetry:
        telemetries = self.__telemetries_by_class_name.get(class_name)
        class_id = self.__class_ids_by_name.get(class_name,
                                                len(self.__class_ids_by_name))
        if telemetries is None:
            telemetries: Dict[str, _Telemetry] = {}
            self.__telemetries_by_class_name[class_name] = telemetries
            self.__class_ids_by_name[class_name] = class_id
            self.__telemetries_by_class_id.append([])
        telemetry = telemetries.get(tele_name)
        if telemetry is None:
            tele_id = len(telemetries)
            # FIXME: default initial value
            telemetry = _Telemetry(source, class_name, class_id, tele_name,
                                   tele_id, ptm_name, 0.0)
            telemetries[tele_name] = telemetry
            self.__telemetries_by_class_id[class_id].append(telemetry)
        telemetry.ptm_name = ptm_name
        self.__telemetries_by_ptm_name[ptm_name] = telemetry
        return telemetry

    def get_class_id_by_name(self, class_name) -> int:
        return self.__class_ids_by_name.get(class_name, -1)

    def get_class_name_by_id(self, class_id) -> Optional[str]:
        # FIXME: improve with an index (but not a time-critical operation)
        for name, id in self.__class_ids_by_name.items():
            if id == class_id:
                return name
        return None

    def get_telemetry_by_ptm_name(self, ptm_name: str) -> _Telemetry:
        return self.__telemetries_by_ptm_name.get(ptm_name, self.__unknown())

    def get_telemetry_by_names(self, class_name: str,
                               tele_name: str) -> _Telemetry:
        telemetries = self.__telemetries_by_class_name.get(class_name, {})
        return telemetries.get(tele_name, self.__unknown())

    def get_telemetry_by_ids(self, class_id: int, tele_id: int) -> _Telemetry:
        if class_id < 0 or class_id >= len(self.__telemetries_by_class_id):
            return self.__unknown()
        telemetries = self.__telemetries_by_class_id[class_id]
        if tele_id < 0 or tele_id >= len(telemetries):
            return self.__unknown()
        return telemetries[tele_id]

    @property
    def telemetries(self) -> List[_Telemetry]:
        result = []
        for telemetries in self.__telemetries_by_class_name.values():
            for telemetry in telemetries.values():
                result.append(telemetry)

        def key(t: _Telemetry) -> str:
            return f"{t.class_id}{t.tele_id}"

        result.sort(key=key)
        return result

    def clear(self) -> None:
        self.__class_ids_by_name: Dict[str, int] = {}
        self.__telemetries_by_class_id: List[List[_Telemetry]] = []
        self.__telemetries_by_class_name: Dict[str, Dict[str, _Telemetry]] = {}
        self.__telemetries_by_ptm_name: Dict[str, _Telemetry] = {}


class ptm_wrapper(pyobj.ConfObject):
    """Power Thermal Manager Wrapper.

    Simics objects that can wrap a Python algorithm for power and thermal
    management."""

    # Once-line doc for the class:
    _class_desc = "Power Thermal Manager Wrapper"
    _do_not_init = object()

    def _initialize(self):
        super()._initialize()

        self.__input_telemetries: List[_Telemetry] = []
        self._output_telemetries = _TelemetryIndex()
        # FIXME: remove this (temporarily exposed to ease debug)
        self.latest_outputs = PTMOutputs()

        self.input_notifier_source_obj = None
        self.input_notifier_event_name = None

        self.f_knobs = []
        self.i_knobs = []
        self.s_knobs = []
        self.b_knobs = []

        self._ptm_module = ""
        self._ptm_constructor = ""
        self._ptm_obj = None

    def _finalize(self):
        if not simics.SIM_has_notifier(self.obj, self.__output_notifier_type):
            simics.SIM_register_notifier(self.obj.classname,
                                         self.__output_notifier_type,
                                         self._output_event_name)

        notifier_type = simics.SIM_notifier_type(
            self.input_notifier_event_name)
        subscriber = self.obj
        callback = self._input_notification_callback
        simics.SIM_require_object(self.input_notifier_source_obj)
        simics.SIM_add_notifier(self.input_notifier_source_obj, notifier_type,
                                subscriber, callback, None)
        self._setup_ptm()

    def _setup_ptm(self):
        self._ptm_obj = self._create_ptm_object()

        knobs = PTMValues()
        for k, v in self.f_knobs:
            knobs.floats[k] = v
        for k, v in self.i_knobs:
            knobs.ints[k] = v
        for k, v in self.s_knobs:
            knobs.strings[k] = v
        for k, v in self.b_knobs:
            knobs.bools[k] = v
        error = self._ptm_obj.initialize(knobs)
        if error.code != 0:
            simics.SIM_log_error(self.obj, 0,
                                 ("error in PTM initialization: "
                                  f"{error.description} (code={error.code})"))

    def _create_ptm_object(self):
        m = import_module(self._ptm_module)
        c = getattr(m, self._ptm_constructor)
        return c()

    def set_input_telemetries(self, values: List[List[Any]]) -> None:
        self.__input_telemetries = []
        for v in values:
            source = v[0]
            class_name = v[1]
            class_id = source.iface.telemetry.get_telemetry_class_id(
                class_name)
            tele_name = v[2]
            tele_id = source.iface.telemetry.get_telemetry_id(
                class_id, tele_name)
            ptm_name = v[3]
            self.__input_telemetries.append(
                # FIXME: default initial value (not very important for inputs)
                _Telemetry(source, class_name, class_id, tele_name, tele_id,
                           ptm_name, 0.0))

    def get_input_telemetries(self) -> List[List[Any]]:
        return [[t.source, t.class_name, t.tele_name, t.ptm_name]
                for t in self.__input_telemetries]

    def set_output_notification(self, value: str) -> None:
        self._output_event_name = value
        self.__output_notifier_type = simics.SIM_notifier_type(
            self._output_event_name)

    def set_output_telemetries(self, values: List[List[str]]) -> None:
        self._output_telemetries.clear()
        for v in values:
            class_name = v[0]
            tele_name = v[1]
            ptm_name = v[2]
            self._output_telemetries.add_telemetry(self.obj, class_name,
                                                   tele_name, ptm_name)

    def get_output_telemetries(self) -> List[List[Any]]:
        telemetries = self._output_telemetries.telemetries
        return [[t.class_name, t.tele_name, t.ptm_name] for t in telemetries]

    def _read_telemetries(self) -> PTMInputs:
        result = PTMInputs()
        for t in self.__input_telemetries:
            v = t.source.iface.telemetry.get_value(t.class_id, t.tele_id)
            if isinstance(v, float):
                result.floats[t.ptm_name] = v
            elif isinstance(v, int):
                result.ints[t.ptm_name] = v
            elif isinstance(v, bool):
                result.bools[t.ptm_name] = v
            elif isinstance(v, str):
                result.strings[t.ptm_name] = v
            else:
                simics.SIM_log_error(
                    self.obj, 0,
                    f"Unsupported type for {t.tele_name}, telemetry ignored")
        return result

    def __write_telemetries(self, values: PTMValues) -> None:
        for name, value in values.ints.items():
            t = self._output_telemetries.get_telemetry_by_ptm_name(name)
            t.value = value
        for name, value in values.floats.items():
            t = self._output_telemetries.get_telemetry_by_ptm_name(name)
            t.value = value
        for name, value in values.bools.items():
            t = self._output_telemetries.get_telemetry_by_ptm_name(name)
            t.value = value
        for name, value in values.strings.items():
            t = self._output_telemetries.get_telemetry_by_ptm_name(name)
            t.value = value

    def _input_notification_callback(self, subscriber: Any, notifier: Any,
                                     data: Optional[Any]) -> None:
        inputs = self._read_telemetries()
        simics.SIM_log_info(4, self.obj, 0, f"inputs: {inputs}")
        time_ps = inputs.ints.get("time_ps")
        if time_ps is None:
            outputs: PTMOutputs = PTMOutputs()
            outputs.error = PTMError()
            outputs.error.code = 1
            outputs.error.description = "'time_ps' not found"
        else:
            outputs: PTMOutputs = self._ptm_obj.run(time_ps, inputs)
            outputs.values.ints["time_ps"] = time_ps
        simics.SIM_log_info(4, self.obj, 0, f"outputs: {outputs}")
        if outputs.error is not None:
            simics.SIM_log_error(
                self.obj, 0,
                (f"error in PTM execution: "
                 f"{outputs.error.description} (code={outputs.error.code})"))
        # FIXME: should we handle the error differently? avoid writing?
        self.__write_telemetries(outputs.values)
        simics.SIM_notify(self.obj, self.__output_notifier_type)
        # FIXME: remove this (temporarily exposed to ease debug)
        self.latest_outputs = outputs

    class model(pyobj.Attribute):
        """PTM model"""

        attrattr = simics.Sim_Attr_Required
        attrtype = "[ss]"

        def setter(self, values):
            self._up._ptm_module = values[0]
            self._up._ptm_constructor = values[1]
            return simics.Sim_Set_Ok

        def getter(self):
            return [self._up._ptm_module, self._up._ptm_constructor]

    class float_knobs(pyobj.Attribute):
        """Knobs of type float to initialize the PTM"""
        attrattr = simics.Sim_Attr_Optional
        attrtype = "[[sf]*]"

        def setter(self, values):
            if simics.SIM_object_is_configured(self._up.obj):
                simics.SIM_log_error(
                    self._up.obj, 0,
                    "attribute float_knobs cannot be set after the object is"
                    " configured")
                return simics.Sim_Set_Not_Writable
            self._up.f_knobs = values
            return simics.Sim_Set_Ok

        def getter(self):
            return self._up.f_knobs

    class int_knobs(pyobj.Attribute):
        """Knobs of type int to initialize the PTM"""
        attrattr = simics.Sim_Attr_Optional
        attrtype = "[[si]*]"

        def setter(self, values):
            if simics.SIM_object_is_configured(self._up.obj):
                simics.SIM_log_error(
                    self._up.obj, 0,
                    "attribute int_knobs cannot be set after the object is"
                    " configured")
                return simics.Sim_Set_Not_Writable
            self._up.i_knobs = values
            return simics.Sim_Set_Ok

        def getter(self):
            return self._up.i_knobs

    class string_knobs(pyobj.Attribute):
        """Knobs of type string to initialize the PTM"""
        attrattr = simics.Sim_Attr_Optional
        attrtype = "[[ss]*]"

        def setter(self, values):
            if simics.SIM_object_is_configured(self._up.obj):
                simics.SIM_log_error(
                    self._up.obj, 0,
                    "attribute string_knobs cannot be set after the object is"
                    " configured")
                return simics.Sim_Set_Not_Writable
            self._up.s_knobs = values
            return simics.Sim_Set_Ok

        def getter(self):
            return self._up.s_knobs

    class bool_knobs(pyobj.Attribute):
        """Knobs of type bool to initialize the PTM"""
        attrattr = simics.Sim_Attr_Optional
        attrtype = "[[sb]*]"

        def setter(self, values):
            if simics.SIM_object_is_configured(self._up.obj):
                simics.SIM_log_error(
                    self._up.obj, 0,
                    "attribute bool_knobs cannot be set after the object is"
                    " configured")
                return simics.Sim_Set_Not_Writable
            self._up.b_knobs = values
            return simics.Sim_Set_Ok

        def getter(self):
            return self._up.b_knobs

    class input_telemetries(pyobj.Attribute):
        """Input telemetries."""

        attrattr = simics.Sim_Attr_Optional
        attrtype = "[[osss]*]"

        def setter(self, values):
            self._up.set_input_telemetries(values)
            return simics.Sim_Set_Ok

        def getter(self):
            return self._up.get_input_telemetries()

    class input_notifier(pyobj.Attribute):
        """Input telemetries notifier."""

        attrattr = simics.Sim_Attr_Required
        attrtype = "[os]"

        def setter(self, values):
            if simics.SIM_object_is_configured(self._up.obj):
                simics.SIM_attribute_error(
                    "input_notifier cannot be reset once object is configured")
                return simics.Sim_Set_Not_Writable

            self._up.input_notifier_source_obj = values[0]
            self._up.input_notifier_event_name = values[1]
            return simics.Sim_Set_Ok

        def getter(self):
            return [
                self._up.input_notifier_source_obj,
                self._up.input_notifier_event_name
            ]

    class output_notification(pyobj.Attribute):
        """Output notification event name."""

        attrattr = simics.Sim_Attr_Required
        attrtype = "s"

        def setter(self, value):
            if simics.SIM_object_is_configured(self._up.obj):
                simics.SIM_attribute_error(
                    ("cannot reset output_notification attribute once the"
                     " object is configured"))
                return simics.Sim_Set_Not_Writable

            self._up.set_output_notification(value)
            return simics.Sim_Set_Ok

        def getter(self):
            return self._up._output_event_name

    class output_telemetries(pyobj.Attribute):
        """Output telemetries."""

        attrattr = simics.Sim_Attr_Optional
        attrtype = "[[sss]*]"

        def setter(self, values):
            self._up.set_output_telemetries(values)
            return simics.Sim_Set_Ok

        def getter(self):
            return self._up.get_output_telemetries()

    class telemetry(pyobj.Interface):
        def get_telemetry_class_id(self, class_name):
            me: ptm_wrapper = self._up
            return me._output_telemetries.get_class_id_by_name(class_name)

        def get_telemetry_class_name(self, class_id):
            me: ptm_wrapper = self._up
            return me._output_telemetries.get_class_name_by_id(class_id)

        def get_telemetry_class_description(self, class_id):
            # FIXME: this is just a lazy shortcut
            return self.get_telemetry_class_name(class_id)

        def get_telemetry_id(self, class_id, tele_name):
            me: ptm_wrapper = self._up
            # FIXME: not optimal (but not a time-critical operation)
            class_name = me._output_telemetries.get_class_name_by_id(class_id)
            return me._output_telemetries.get_telemetry_by_names(
                class_name, tele_name).tele_id

        def get_telemetry_name(self, class_id, tele_id):
            me: ptm_wrapper = self._up
            return me._output_telemetries.get_telemetry_by_ids(
                class_id, tele_id).tele_name

        def get_telemetry_description(self, class_id, tele_id):
            # FIXME: this is just a lazy shortcut
            return self.get_telemetry_name(class_id, tele_id)

        def get_value(self, class_id, tele_id):
            me: ptm_wrapper = self._up
            return me._output_telemetries.get_telemetry_by_ids(
                class_id, tele_id).value
