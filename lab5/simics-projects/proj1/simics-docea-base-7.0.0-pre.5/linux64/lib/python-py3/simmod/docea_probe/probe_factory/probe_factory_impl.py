# -*- coding: utf-8 -*-

from typing import Callable, Dict, List

import cli
import conf
import pyobj
import simics_6_api as simics
from simmod.power_thermal.path_sanitizer_impl import PathSanitizer

from .metadata_parser_impl import MetadataParser


class ConnectorError(Exception):
    pass


class ConnectorValidator():
    @staticmethod
    def validate_connector(connector: pyobj.ConfObject):
        if not ConnectorValidator._is_valid_connector(connector):
            err = "Cannot create probe. Owner object has a deprecated"
            err += " physical connector. Use at least physical_connector_v2"
            raise ConnectorError(err)

    @staticmethod
    def _is_valid_connector(connector: pyobj.ConfObject) -> bool:
        valid_classes = ['physical_connector_v2', 'physical_connector_v3']
        connector_class = simics.SIM_object_class(connector).classname
        return connector_class in valid_classes


class NamespaceCreator():
    @staticmethod
    def create_parent_namespace(hierarchical_object_name: str):
        """Create a nested namespace given a hierarchical object name, e.g.,
        if the given name is 'a.b.c.obj', it first creates a, then a.b,
        then a.b.c, if a or a.b or a.b.c do not exist"""
        namespace = hierarchical_object_name.rsplit('.', 1)[0]
        nested_attributes = namespace.split('.')
        attr_chain = ""
        for attr in nested_attributes:
            attr_chain = attr if len(
                attr_chain) == 0 else attr_chain + "." + attr
            if not NamespaceCreator.has_deep_attr(conf, attr_chain):
                obj = simics.pre_conf_object(attr_chain, 'namespace')
                simics.SIM_add_configuration([obj], None)

    @staticmethod
    def get_deep_attr(obj: pyobj.ConfObject, attrs: str) -> pyobj.ConfObject:
        for attr in attrs.split("."):
            obj = getattr(obj, attr)
        return obj

    @staticmethod
    def has_deep_attr(obj: pyobj.ConfObject, attrs: str) -> bool:
        try:
            NamespaceCreator.get_deep_attr(obj, attrs)
            return True
        except AttributeError:
            return False

    @staticmethod
    def is_namespace_object(hierarchical_object_name: str) -> bool:
        try:
            obj = simics.SIM_get_object(hierarchical_object_name)
        except Exception:
            return False
        return simics.SIM_object_class(obj).classname == 'namespace'

    @staticmethod
    def _rename_with_temporary_name(name: str) -> str:
        new_name = cli.get_available_object_name('temp')
        NamespaceCreator._rename(name, new_name)
        return new_name

    @staticmethod
    def _rename(old_name: str, new_name: str) -> None:
        cli.run_command("move-object " + old_name + " " + new_name)

    @staticmethod
    def _move_children(old_parent: str, new_parent: str) -> None:
        direct_children = NamespaceCreator._get_direct_children(old_parent)
        for name in direct_children:
            old_name = old_parent + "." + name
            new_name = new_parent + "." + name
            NamespaceCreator._rename(old_name, new_name)

    @staticmethod
    def _get_direct_children(obj_name: str) -> None:
        obj = simics.SIM_get_object(obj_name)
        it = simics.SIM_object_iterator(obj)
        direct_children = set()
        for child in it:
            name = NamespaceCreator._get_direct_child_name(child.name)
            direct_children.add(name)
        return list(direct_children)

    @staticmethod
    def _get_direct_child_name(hierarchical_name: str):
        return hierarchical_name.split(".")[1]


class ProbeFactory():
    def __init__(self,
                 namespace: str,
                 metadata: str,
                 connector: pyobj.ConfObject = None,
                 time_provider: pyobj.ConfObject = None):
        self.metadata_parser = MetadataParser(namespace, metadata)
        self.probe_class = None
        self.connector = connector
        self.time_provider = time_provider
        self.sim_probes_namespace = namespace + ".probes.simulation"
        self.event_probes_namespace = namespace + ".probes.event_based"

    def create_simulation_probes_from_metadata(
        self, filter_func: Callable[[pyobj.ConfObject],
                                    bool]) -> List[pyobj.ConfObject]:
        try:
            return self._create_simulation_probes_or_raise_error(filter_func)
        except ConnectorError as err:
            simics.SIM_log_info(1, conf.sim, 0, f"{err}")
            probes = []
            return probes
        except Exception as err:
            simics.SIM_log_error(conf.sim, 0, f"{err}")
            raise ValueError(err)

    def create_event_based_probes_from_metadata(
        self, filter_func: Callable[[pyobj.ConfObject],
                                    bool]) -> List[pyobj.ConfObject]:
        try:
            return self._create_event_based_probes_or_raise_error(filter_func)
        except ConnectorError as err:
            simics.SIM_log_info(1, conf.sim, 0, f"{err}")
            probes = []
            return probes
        except Exception as err:
            simics.SIM_log_error(conf.sim, 0, f"{err}")
            raise ValueError(err)

    def _create_simulation_probes_or_raise_error(
        self, filter_func: Callable[[pyobj.ConfObject],
                                    bool]) -> List[pyobj.ConfObject]:
        self.probe_class = "docea_simulation_probe"
        probes = self._create_probes_or_raise_error(filter_func)
        self._set_time_provider(probes)
        return probes

    def _create_event_based_probes_or_raise_error(
        self, filter_func: Callable[[pyobj.ConfObject],
                                    bool]) -> List[pyobj.ConfObject]:
        self.probe_class = "docea_event_based_probe"
        return self._create_probes_or_raise_error(filter_func)

    def _create_probes_or_raise_error(
        self, filter_func: Callable[[pyobj.ConfObject],
                                    bool]) -> List[pyobj.ConfObject]:
        ConnectorValidator.validate_connector(self.connector)
        data = self.metadata_parser.get_power_thermal_data(filter_func)
        probes = []
        for entry in data:
            probe = self._create_probe_or_log_duplicated_probe_info(entry)
            if probe is not None:
                probes.append(probe)
        return probes

    def _create_probe_or_log_duplicated_probe_info(
            self, probe_data: Dict[str, str]) -> pyobj.ConfObject:
        try:
            return self._create_probe(probe_data)
        except self.DuplicateProbeError as err:
            simics.SIM_log_info(1, conf.sim, 0, f"{err}")
            return None

    def _create_probe(self, probe_data: Dict[str, str]) -> pyobj.ConfObject:
        name = PathSanitizer.sanitize(probe_data["power_thermal_name"])
        probe_name = self._get_probe_hierarchical_name(name)
        self._raise_error_if_probe_exists(probe_name)
        if NamespaceCreator.is_namespace_object(probe_name):
            return self._replace_namespace_with_new_probe(
                probe_name, probe_data)
        else:
            return self._new_probe(probe_name, probe_data)

    def _replace_namespace_with_new_probe(
            self, probe_name: str, probe_data: Dict[str,
                                                    str]) -> pyobj.ConfObject:
        temp_name = NamespaceCreator._rename_with_temporary_name(probe_name)
        p = self._create_probe_object(probe_name, probe_data)
        NamespaceCreator._move_children(temp_name, probe_name)
        simics.SIM_delete_object(simics.SIM_get_object(temp_name))
        return p

    def _get_temporary_probe_name(self, base_name: str) -> str:
        return cli.get_available_object_name(base_name)

    def _create_probe_object(self, name: str,
                             probe_data: Dict[str, str]) -> pyobj.ConfObject:
        return simics.SIM_create_object(
            self.probe_class, name,
            [['physical_connector', self.connector],
             ['power_thermal_name', probe_data["power_thermal_name"]],
             ['nature', probe_data["nature"]]])

    def _new_probe(self, probe_name: str,
                   probe_data: Dict[str, str]) -> pyobj.ConfObject:
        NamespaceCreator.create_parent_namespace(probe_name)
        return self._create_probe_object(probe_name, probe_data)

    def _raise_error_if_probe_exists(self, probe_name: str):
        if self._probe_object_exists(probe_name):
            msg = "Object '" + probe_name + "' already exists."
            msg += " Skipping probe creation"
            raise self.DuplicateProbeError(msg)

    def _probe_object_exists(self, probe_name: str) -> bool:
        try:
            obj = simics.SIM_get_object(probe_name)
            obj_class = simics.SIM_object_class(obj).classname
            return obj_class == self.probe_class
        except simics.SimExc_General:
            return False

    def _get_probe_hierarchical_name(self, name: str) -> str:
        return self._get_probe_namespace() + "." + name

    def _get_probe_namespace(self) -> str:
        if self.probe_class == "docea_simulation_probe":
            return self.sim_probes_namespace
        return self.event_probes_namespace

    def _set_time_provider(self, probes: List[pyobj.ConfObject]):
        for probe in probes:
            probe.time_provider = self.time_provider

    class DuplicateProbeError(Exception):
        pass


def interface_probe_filter(ptn: str, type: str, nature: str) -> bool:
    interface_types = ["input", "output"]
    return type in interface_types


def internal_probe_filter(ptn: str, type: str, nature: str) -> bool:
    interface_types = ["input", "output"]
    return type not in interface_types


def all_probes_filter(ptn: str, type: str, nature: str) -> bool:
    return True


def create_simulation_probes_from_metadata(
        namespace: str,
        metadata: str,
        connector: pyobj.ConfObject,
        time_provider: pyobj.ConfObject,
        filter_func: Callable[[str, str, str], bool] = interface_probe_filter):
    """Create simulation probe objects in the given namespace for the physical
        model variables by parsing the metadata file. Apply a filtering
        function that takes in the power_thermal_name, type, and nature, and
        returns a boolean indicating whether the probe for the variable
        must be created or not"""
    simics.SIM_log_warning(
        conf.sim, 0, "The create_simulation_probes_from_metadata function is"
        " DEPRECATED. Consider using the docea-probe-array Simics class"
        " instead.")
    factory = ProbeFactory(namespace, metadata, connector, time_provider)
    return factory.create_simulation_probes_from_metadata(filter_func)


def create_event_based_probes_from_metadata(
        namespace: str,
        metadata: str,
        connector: str,
        filter_func: Callable[[str, str, str], bool] = interface_probe_filter):
    """Create event-based probe objects in the given namespace for the physical
        model variables by parsing the metadata file. Apply a filtering
        function that takes in the power_thermal_name, type, and nature, and
        returns a boolean indicating whether the probe for the variable
        must be created or not"""
    simics.SIM_log_warning(
        conf.sim, 0, "The create_event_based_probes_from_metadata function is"
        " DEPRECATED. Consider using the docea-probe-array Simics class"
        " instead.")
    factory = ProbeFactory(namespace, metadata, connector, time_provider=None)
    return factory.create_event_based_probes_from_metadata(filter_func)


def does_support_probes() -> bool:
    """Return True if the given Simics installation supports probes and false
     otherwise"""
    simics.SIM_log_warning(
        conf.sim, 0, "The does_support_probes function is"
        " DEPRECATED. Consider using a recent version of Simics to ensure"
        " probe support.")
    does_support = True
    try:
        import probes  # noqa: F401
    except ImportError:
        does_support = False
    return does_support
