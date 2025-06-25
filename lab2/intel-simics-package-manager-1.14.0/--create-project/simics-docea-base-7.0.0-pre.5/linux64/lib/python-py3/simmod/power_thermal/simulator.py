import os
import sys
from dataclasses import dataclass
from importlib.metadata import version
from typing import Any, Optional

import simics_6_api as simics
from simmod.power_thermal import model_objects

# FIXME: eventually, this definition should come from outside this module
SimulableModel = Any


@dataclass
class Configuration:
    initial_time_picoseconds: Optional[int] = None
    metadata_file: Optional[str] = None
    interface_objects_namespace: str = ""
    must_create_interface_objects: bool = True
    must_overwrite_metadata: bool = False
    connector_name: Optional[str] = None
    results_delay_picoseconds: Optional[int] = None
    retained_events_window_size_picoseconds: Optional[int] = None
    start_on_creation: bool = False


class PowerThermalSimulator:
    def __init__(
        self,
        cfg: Configuration,
        model: SimulableModel,
    ) -> None:
        self.__connector = simics.SIM_create_object("physical_connector_v3",
                                                    cfg.connector_name)
        self.__set_optional_fields(cfg)
        self.__set_model(model)
        self.__set_metadata_file(cfg)
        if self.__must_start(cfg):
            self.__connector.started = True
        self.__set_interface(cfg)

    @property
    def connector(self) -> Any:
        return self.__connector

    @property
    def interface(self) -> Any:
        return self.__interface

    def __set_optional_fields(self, cfg: Configuration) -> None:
        if cfg.initial_time_picoseconds is not None:
            self.__connector.initial_time_picoseconds = \
                cfg.initial_time_picoseconds
        if cfg.results_delay_picoseconds is not None:
            self.__connector.results_delay_picoseconds = \
                cfg.results_delay_picoseconds
        if cfg.retained_events_window_size_picoseconds is not None:
            self.__connector.retained_events_window_size_picoseconds = \
                cfg.retained_events_window_size_picoseconds

    def __set_model(self, model: SimulableModel) -> None:
        self.__model = model

        def get_model_for_simulation() -> SimulableModel:
            return model

        module_name = __name__
        constructor_name = f"{self.__connector.name}_get_model_for_simulation"
        me = sys.modules[module_name]
        setattr(me, constructor_name, get_model_for_simulation)
        self.__connector.model = [module_name, constructor_name]

    def __set_metadata_file(self, cfg: Configuration) -> None:
        name = cfg.metadata_file if cfg.metadata_file \
            else f"{self.__connector.name}.metadata.json"
        if cfg.must_overwrite_metadata and os.path.exists(name):
            os.remove(name)
        self.__connector.model_metadata = name

    def __must_start(self, cfg: Configuration) -> bool:
        # TODO: warn if incoherent
        # TODO: should we start if must_create_interface_objects?
        if cfg.must_overwrite_metadata:
            return True
        if not os.path.exists(self.__connector.model_metadata):
            return True
        return cfg.start_on_creation

    def __is_simulable_api_supported(self) -> bool:
        DOCEAPY_VERSION_SUPPORT_SIMULABLE_API = "2.0.0"
        try:
            is_supported = version(
                "doceapy") >= DOCEAPY_VERSION_SUPPORT_SIMULABLE_API
        except Exception:
            return False
        return is_supported

    def __set_interface(self, cfg: Configuration) -> None:
        class Interface:
            pass

        self.__interface = Interface()
        if not cfg.must_create_interface_objects:
            return

        if self.__is_simulable_api_supported():
            objs = model_objects.create_from_simulable_model(
                cfg.interface_objects_namespace, self.__model,
                self.__connector)
        else:
            objs = model_objects.create_from_metadata_file(
                cfg.interface_objects_namespace,
                self.__connector.model_metadata, self.__connector)

        if cfg.interface_objects_namespace == "":
            added = set()
            for o in objs:
                p = self.__get_oldest_ancestor(o)
                if p.name not in added:
                    setattr(self.__interface, p.name, p)
                    added.add(p.name)
        else:
            # FIXME: this implementation is a bit clumsy, because the Simics
            # object corresponding to interface_objects_namespace could contain
            # more that just the interface objects
            self.__interface = simics.SIM_get_object(
                cfg.interface_objects_namespace)

    def __get_oldest_ancestor(self, obj: Any) -> Any:
        # Note that I cannot define this function as a private function in the
        # current module. Simics will not find it
        p = obj
        pp = simics.SIM_object_parent(p)
        while pp is not None:
            p = pp
            pp = simics.SIM_object_parent(p)
        return p
