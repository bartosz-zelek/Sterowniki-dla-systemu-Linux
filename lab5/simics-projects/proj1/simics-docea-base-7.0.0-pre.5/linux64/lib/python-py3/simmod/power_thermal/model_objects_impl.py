import json
from abc import ABCMeta, abstractmethod
from functools import cache
from typing import Any, List, Tuple

import conf
import pyobj
import simics_6_api as simics

from .path_sanitizer_impl import PathSanitizer


class DoceaBuilder(metaclass=ABCMeta):
    @abstractmethod
    def build(self):
        raise NotImplementedError


class DoceaParameterBuilder(DoceaBuilder):
    def __init__(self):
        self._name = ""
        self._physical_connector = None
        self._nature = ""
        self._id = 0
        self._power_thermal_name = None

    def set_qualified_name(self, name: str):
        self._name = name

    def set_physical_connector(self, connector: pyobj.ConfObject):
        self._physical_connector = connector

    def set_nature(self, nature: str):
        self._nature = nature

    def set_id(self, id: int):
        self._id = id

    def set_power_thermal_name(self, name: str):
        self._power_thermal_name = name


class DoceaReadFloatBuilder(DoceaParameterBuilder):
    def build(self):
        return simics.SIM_create_object(
            "read_float", self._name,
            [["physical_connector", self._physical_connector],
             ["nature", self._nature], ["id", self._id],
             ["power_thermal_name", self._power_thermal_name]])


class DoceaWrittenFloatBuilder(DoceaParameterBuilder):
    def build(self):
        return simics.SIM_create_object(
            "written_float", self._name,
            [["physical_connector", self._physical_connector],
             ["nature", self._nature], ["id", self._id],
             ["power_thermal_name", self._power_thermal_name]])


class DoceaWrittenStringBuilder(DoceaParameterBuilder):
    def build(self):
        return simics.SIM_create_object(
            "written_string", self._name,
            [["physical_connector", self._physical_connector],
             ["nature", self._nature], ["id", self._id],
             ["power_thermal_name", self._power_thermal_name]])


class DoceaReadWrittenFloatBuilder(DoceaParameterBuilder):
    def build(self):
        return simics.SIM_create_object(
            "read_written_float", self._name,
            [["physical_connector", self._physical_connector],
             ["nature", self._nature],
             ["power_thermal_name", self._power_thermal_name]])


class DoceaSectionBuilder(DoceaBuilder):
    def __init__(self, target_namespace: str, model_interface: dict,
                 section_name: str, physical_connector: pyobj.ConfObject):
        self._target_namespace = target_namespace
        self._model_interface = model_interface
        self._section_name = section_name
        self._physical_connector = physical_connector

    def build(self):
        if self._section_name not in self._model_interface:
            return []
        section = self._model_interface[self._section_name]
        return self._build_section(section)

    @abstractmethod
    def _build_section(self, section):
        raise NotImplementedError

    def _get_configured_builder(self, path: str, id: str,
                                builder: DoceaParameterBuilder):
        path_slice = PathSanitizer.get_sanitized_path_slice(path)
        parent_path_slice = path_slice[:len(path_slice) - 1]
        PathSanitizer.ensure_namespace(
            self.__get_qualified_name(parent_path_slice))
        builder.set_qualified_name(self.__get_qualified_name(path_slice))
        builder.set_physical_connector(self._physical_connector)
        builder.set_nature(self.__get_nature())
        builder.set_id(int(id, 0))
        return builder

    def _get_configured_v2_builder(self, path: str,
                                   builder: DoceaParameterBuilder):
        self._get_configured_builder(path, "0", builder)
        builder.set_power_thermal_name(path)
        return builder

    def __get_qualified_name(self, path_slice):
        if len(self._target_namespace) == 0:
            return ".".join(path_slice)
        return self._target_namespace + "." + ".".join(path_slice)

    def __get_nature(self):
        if self._section_name == "frequencies":
            return "frequency"
        if self._section_name == "activities":
            return "activity"
        return self._section_name[:len(self._section_name) - 1]


class DoceaInputSectionBuilder(DoceaSectionBuilder):
    def _build_section(self, section):
        result = []
        for id in section:
            paths = section[id]
            for path in paths:
                b = self._get_builder(path, id)
                result.append(b.build())
        return result

    @abstractmethod
    def _get_builder(self, path: str, id: str):
        raise NotImplementedError


class DoceaInputSectionV2Builder(DoceaSectionBuilder):
    def _build_section(self, section):
        result = []
        for item in section:
            b = self._get_builder(item['name'])
            result.append(b.build())
        return result

    @abstractmethod
    def _get_builder(self, path: str):
        raise NotImplementedError


class DoceaFloatInputSectionBuilder(DoceaInputSectionBuilder):
    def _get_builder(self, path: str, id: str):
        i = DoceaWrittenFloatBuilder()
        return self._get_configured_builder(path, id, i)


class DoceaFloatInputSectionV2Builder(DoceaInputSectionV2Builder):
    def _get_builder(self, path: str):
        i = DoceaWrittenFloatBuilder()
        return self._get_configured_v2_builder(path, i)


class DoceaStringInputSectionBuilder(DoceaInputSectionBuilder):
    def _get_builder(self, path: str, id: str):
        i = DoceaWrittenStringBuilder()
        return self._get_configured_builder(path, id, i)


class DoceaStringInputSectionV2Builder(DoceaInputSectionV2Builder):
    def _get_builder(self, path: str):
        i = DoceaWrittenStringBuilder()
        return self._get_configured_v2_builder(path, i)


class DoceaOutputSectionBuilder(DoceaSectionBuilder):
    def _build_section(self, section):
        result = []
        for path in section:
            ids = section[path]
            for id in ids:
                b = self.__get_read_float_builder(path, id)
                result.append(b.build())
        return result

    def __get_read_float_builder(self, path: str, id: str):
        i = DoceaReadFloatBuilder()
        return self._get_configured_builder(path, id, i)


class DoceaOutputSectionV2Builder(DoceaSectionBuilder):
    def _build_section(self, section):
        result = []
        for item in section:
            b = self.__get_read_float_builder(item['name'])
            result.append(b.build())
        return result

    def __get_read_float_builder(self, path: str):
        b = DoceaReadFloatBuilder()
        return self._get_configured_v2_builder(path, b)


class DoceaInputOutputSectionBuilder(DoceaSectionBuilder):
    def _build_section(self, section):
        result = []
        for item in section:
            b = DoceaReadWrittenFloatBuilder()
            b = self._get_configured_v2_builder(item['name'], b)
            result.append(b.build())
        return result


class DoceaModel:
    """DoceaModel provides an API for creating and manipulating physical model
    objects.

    This class is not meant to be instantiate explicitly. Instead, it is used
    as a namespace that gathers public static methods."""
    def __init__(self, target_namespace: str,
                 physical_connector: pyobj.ConfObject):
        self.__target_namespace = target_namespace
        self.__physical_connector = physical_connector

    @staticmethod
    def __except_invalid_time_provider(o: pyobj.ConfObject):
        queue = simics.SIM_get_attribute(o, 'queue')
        if queue is None:
            raise Exception('object {} has no queue attribute'.format(o))
        simics.SIM_get_interface(queue, 'cycle')

    @staticmethod
    def __is_valid_readable(o: pyobj.ConfObject):
        try:
            simics.SIM_get_interface(o, 'readable_float')
            return True
        except Exception:
            return False

    @staticmethod
    def __is_time_providable(o: pyobj.ConfObject):
        return (DoceaModel.__is_valid_readable(o))

    @staticmethod
    def _get_time_providable_objects(namespace: str):
        namespace_object = None
        result = []
        if len(namespace) > 0:
            try:
                namespace_object = simics.SIM_get_object(namespace)
            except simics.SimExc_General:
                return result
        it = simics.SIM_object_iterator(namespace_object)
        for o in it:
            if DoceaModel.__is_time_providable(o):
                result.append(o)
        return result

    @staticmethod
    def set_time_provider(objects: list, time_provider: pyobj.ConfObject):
        DoceaModel.__except_invalid_time_provider(time_provider)
        for o in objects:
            if DoceaModel.__is_time_providable(o):
                o.time_provider = time_provider
            else:
                simics.SIM_log_info(
                    1,
                    conf.sim,
                    0,
                    'Ignoring time_provider configuration on {} because it is not a valid readable object'  # noqa: E501
                    .format(o))

    @staticmethod
    def set_time_provider_to_hierarchy(namespace: str,
                                       time_provider: pyobj.ConfObject):
        if not isinstance(namespace, str):
            simics.SIM_log_error(
                conf.sim, 0,
                '{} is not a valid namespace name'.format(namespace))
            return
        DoceaModel.set_time_provider(
            DoceaModel._get_time_providable_objects(namespace), time_provider)

    def create_from_v1(self, model_interface_json: str):
        """Create the model objects using the metadata format v1.

        Instead of calling this method explicitly, please prefer the static and
        version-detecting method `create_from_metadata_file`."""
        model_interface = json.loads(model_interface_json)
        result = []
        float_input_sections = ["frequencies", "voltages", "cdyns"]
        for section in float_input_sections:
            i = DoceaFloatInputSectionBuilder(self.__target_namespace,
                                              model_interface, section,
                                              self.__physical_connector)
            result.extend(i.build())
        string_input_sections = ["states"]
        for section in string_input_sections:
            i = DoceaStringInputSectionBuilder(self.__target_namespace,
                                               model_interface, section,
                                               self.__physical_connector)
            result.extend(i.build())
        output_sections = ["currents", "temperatures"]
        for section in output_sections:
            i = DoceaOutputSectionBuilder(self.__target_namespace,
                                          model_interface, section,
                                          self.__physical_connector)
            result.extend(i.build())
        return result

    def create_from_v2(self, model_interface_json: str):
        """Create the model objects using the metadata format v2.

        Instead of calling this method explicitly, please prefer the static and
        version-detecting method `create_from_metadata_file`."""
        model_interface = json.loads(model_interface_json)
        result = []
        float_input_sections = ["frequencies", "voltages", "cdyns"]
        for section_name in float_input_sections:
            i = DoceaFloatInputSectionV2Builder(self.__target_namespace,
                                                model_interface['inputs'],
                                                section_name,
                                                self.__physical_connector)
            result.extend(i.build())
        string_input_sections = ["states"]
        for section_name in string_input_sections:
            i = DoceaStringInputSectionV2Builder(self.__target_namespace,
                                                 model_interface['inputs'],
                                                 section_name,
                                                 self.__physical_connector)
            result.extend(i.build())
        output_sections = ["currents", "temperatures"]
        for section_name in output_sections:
            i = DoceaOutputSectionV2Builder(self.__target_namespace,
                                            model_interface['outputs'],
                                            section_name,
                                            self.__physical_connector)
            result.extend(i.build())
        return result

    @staticmethod
    def create_from_metadata_file(target_namespace: str, metadata_file: str,
                                  physical_connector: pyobj.ConfObject):
        file = open(metadata_file, mode='r')
        metadata_json = file.read()
        file.close()
        metadata = json.loads(metadata_json)

        if 'version' not in metadata or metadata['version'] == 1:
            if 'hardware-bindings' not in metadata:
                return []
            model_interface_json = json.dumps(metadata['hardware-bindings'])
            i = DoceaModel(target_namespace, physical_connector)
            return i.create_from_v1(model_interface_json)

        if metadata['version'] == 2:
            if 'interface' not in metadata:
                return []
            model_interface_json = json.dumps(metadata['interface'])
            i = DoceaModel(target_namespace, physical_connector)
            objs = i.create_from_v2(model_interface_json)
            return objs

        simics.SIM_log_error(
            physical_connector, 0,
            'Unsupported physical metadata version "{}" in "{}"'.format(
                metadata['version'], metadata_file))
        return []

    # FIXME: should be replaced by a proper method in the simulable model,
    #  e.g., simulable_model.heat_transfer_coefficients
    def __get_heat_transfer_coefficient_names(
            self, simulable_model: Any) -> set[str]:
        if simulable_model.thermal_model_with_ambient is None:
            return set()
        thermal_with_ambient = simulable_model.thermal_model_with_ambient[0]
        if not hasattr(thermal_with_ambient, "htc_ports"):
            return set()
        return set(port.htc_parameter
                   for port in thermal_with_ambient.htc_ports)

    @cache
    def __get_power_consumer_temperature_names(
            self,
            simulable_model: Any) -> Tuple[List[str], List[str], List[str]]:
        if (not hasattr(simulable_model, "input_power_temperatures")
                or simulable_model.input_power_temperatures is None):
            input_names = set()
        else:
            input_names = {
                x.variable.name
                for x in simulable_model.input_power_temperatures
            }
        if (not hasattr(simulable_model, "output_power_temperatures")
                or simulable_model.output_power_temperatures is None):
            output_names = set()
        else:
            output_names = {
                x.variable.name
                for x in simulable_model.output_power_temperatures
            }
        only_inputs = list(input_names - output_names)
        only_outputs = list(output_names - input_names)
        inputs_and_outputs = list(input_names & output_names)
        return (only_inputs, only_outputs, inputs_and_outputs)

    def __create_input_floats_from_simulable_model(self, simulable_model: Any):
        objects = []
        input_floats = {}
        if hasattr(simulable_model, "input_clocks"):
            input_floats["frequencies"] = [{
                "name": x.variable.name
            } for x in simulable_model.input_clocks]
        if hasattr(simulable_model, "input_voltages"):
            input_floats["voltages"] = [{
                "name": input.variable.name
            } for input in simulable_model.input_voltages]
        if hasattr(simulable_model, "input_float_parameters"):
            input_floats["activities"] = [{
                "name": input.variable.name
            } for input in simulable_model.input_float_parameters]
        if hasattr(simulable_model, "thermal_model_with_ambient"):
            input_floats["heat_transfer_coefficients"] = [{
                "name": name
            } for name in self.__get_heat_transfer_coefficient_names(
                simulable_model)]

        (input_power_temperatures, _,
         _) = self.__get_power_consumer_temperature_names(simulable_model)
        input_floats["power_consumer_temperatures"] = [{
            "name": input
        } for input in input_power_temperatures]

        for section_name in input_floats.keys():
            builder = DoceaFloatInputSectionV2Builder(
                self.__target_namespace, input_floats, section_name,
                self.__physical_connector)
            objects.extend(builder.build())

        return objects

    def __create_input_strings_from_simulable_model(self,
                                                    simulable_model: Any):
        objects = []
        input_strings = {"states": []}
        if hasattr(simulable_model, "input_states"):
            input_strings["states"] = [{
                "name": input.variable.name
            } for input in simulable_model.input_states]

        if hasattr(simulable_model, "input_string_parameters"):
            input_strings["states"].extend([{
                "name": input.variable.name
            } for input in simulable_model.input_string_parameters])

        for section_name in input_strings.keys():
            builder = DoceaStringInputSectionV2Builder(
                self.__target_namespace, input_strings, section_name,
                self.__physical_connector)
            objects.extend(builder.build())
        return objects

    def __create_output_floats_from_simulable_model(self,
                                                    simulable_model: Any):
        objects = []
        output_floats = {}
        if hasattr(simulable_model, "output_currents"):
            output_floats["currents"] = [{
                "name": output.variable.name
            } for output in simulable_model.output_currents]

        if hasattr(simulable_model, "output_powers"):
            output_floats["powers"] = [{
                "name": output.variable.name
            } for output in simulable_model.output_powers]

        if hasattr(simulable_model, "output_temperatures"):
            output_floats["temperatures"] = [{
                "name": output.variable.name
            } for output in simulable_model.output_temperatures]

        (_, output_power_temperatures,
         _) = self.__get_power_consumer_temperature_names(simulable_model)
        output_floats["power_consumer_temperatures"] = [{
            "name": output
        } for output in output_power_temperatures]

        for section_name in output_floats.keys():
            builder = DoceaOutputSectionV2Builder(self.__target_namespace,
                                                  output_floats, section_name,
                                                  self.__physical_connector)
            objects.extend(builder.build())
        return objects

    def __create_input_output_floats_from_simulable_model(
            self, simulable_model: Any) -> None:
        objects = []
        (_, _, input_output_power_temperatures
         ) = self.__get_power_consumer_temperature_names(simulable_model)
        input_output_floats = {"power_consumer_temperatures": []}
        for name in input_output_power_temperatures:
            input_output_floats["power_consumer_temperatures"].append(
                {"name": name})

        for section_name in input_output_floats.keys():
            builder = DoceaInputOutputSectionBuilder(self.__target_namespace,
                                                     input_output_floats,
                                                     section_name,
                                                     self.__physical_connector)
            objects.extend(builder.build())
        return objects

    def __create_from_simulable_model(self, simulable_model: Any):
        objects = self.__create_input_floats_from_simulable_model(
            simulable_model)
        objects.extend(
            self.__create_input_strings_from_simulable_model(simulable_model))
        objects.extend(
            self.__create_output_floats_from_simulable_model(simulable_model))
        objects.extend(
            self.__create_input_output_floats_from_simulable_model(
                simulable_model))
        return objects

    @staticmethod
    def create_from_simulable_model(target_namespace: str,
                                    simulable_model: Any,
                                    physical_connector: pyobj.ConfObject):
        docea_model = DoceaModel(target_namespace, physical_connector)
        return docea_model.__create_from_simulable_model(simulable_model)


def create_from_metadata_file(target_namespace: str, metadata_file: str,
                              physical_connector: pyobj.ConfObject):
    """Create the model objects under a namespace by parsing the metadata file.

    All the objects are put under the given namespace. If the namespace does
    not exist yet, it will be created, including the necessary parent
    hierarchy.

    The metadata file defines the interface of the model. Readable objects are
    created for accessing model outputs, writable objects are created for
    driving model inputs.

    The physical connector provides the API of the physical simulator to all
    the objects."""
    return DoceaModel.create_from_metadata_file(target_namespace,
                                                metadata_file,
                                                physical_connector)


def create_from_simulable_model(target_namespace: str, simulable_model: Any,
                                physical_connector: pyobj.ConfObject):
    """Create the model objects under a namespace by examining the simulable model.

    All the objects are put under the given namespace. If the namespace does
    not exist yet, it will be created, including the necessary parent
    hierarchy.

    Readable objects are created for accessing model outputs,
    writable objects are created for driving model inputs.

    The physical connector provides the API of the physical simulator to all
    the objects."""
    return DoceaModel.create_from_simulable_model(target_namespace,
                                                  simulable_model,
                                                  physical_connector)


def set_time_provider(objects: list, time_provider: pyobj.ConfObject):
    """Set the time provider to all the readable objects in the list.

    `None` can be used as a time provider to remove any previously set provider
    (without a time provider, readable objects cannot be accessed using the
    telemetry interface)."""
    return DoceaModel.set_time_provider(objects, time_provider)


def set_time_provider_to_hierarchy(namespace: str,
                                   time_provider: pyobj.ConfObject):
    """Set the time provider to all the readable objects under the namespace.

    It searches for all the readable objects deeply. `None` can be used as a
    time provider to remove any previously set provider (without a time
    provider, readable objects cannot be accessed using the telemetry
    interface)."""
    return DoceaModel.set_time_provider_to_hierarchy(namespace, time_provider)
