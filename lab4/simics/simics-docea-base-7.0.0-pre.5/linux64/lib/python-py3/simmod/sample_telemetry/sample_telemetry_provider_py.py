###############################################################################
# Disclaimer: this is sample code of a telemetry provider intended to be
# used as part of a Docea Base tutorial.
# By no means the implementation is general or exhaustive:
# we have dropped some validations and hard-coded some variables
# to reduce complexity and favor understanding of the underlying concepts.
# Usage in production is advised against.
###############################################################################

import random

import pyobj
import simics_6_api as simics


class sample_telemetry_provider_py(pyobj.ConfObject):
    """A sample telemetry provider object"""

    _class_desc = """A sample telemetry provider object"""

    # Prevents registration in case the file is imported from another Simics
    # module
    _do_not_init = object()

    def _initialize(self):
        pyobj.ConfObject._initialize(self)
        self.telemetry_classes = {"GRAPHICS-CARD-INPUT-TELEMETRIES": 1}
        self.telemetries = {
            "GRAPHICS-CARD-INPUT-TELEMETRIES": {
                "STATE": 1,
                "VOLTAGE": 2,
                "FREQUENCY": 3,
                "TIME_PS": 4
            }
        }
        self.clock = None

    def get_time_ps(self):
        if self.clock is None:
            return 0
        return simics.SIM_cycle_count(simics.SIM_picosecond_clock(self.clock))

    def get_random_state(self):
        states = ["IDLE", "DECODE", "BLITTING"]
        return states[random.randint(0, 2)]

    def get_random_voltage(self):
        return random.uniform(0.8, 1.5)

    def get_random_frequency(self):
        return random.uniform(0.1, 1) * 1e09

    class clock(pyobj.Attribute):
        """The clock used to return the time via telemetry"""
        attrattr = simics.Sim_Attr_Optional
        attrtype = 'o|n'

        def setter(self, val):
            self._up.clock = val

        def getter(self):
            return self._up.clock

    class telemetry(pyobj.Interface):
        def get_telemetry_class_id(self, telemetry_class_name):
            if telemetry_class_name not in self._up.telemetry_classes:
                return 0
            return self._up.telemetry_classes[telemetry_class_name]

        def get_telemetry_class_name(self, telemetry_class_id):
            for name, id in self._up.telemetry_classes.items():
                if id == telemetry_class_id:
                    return name
            return ""

        def get_telemetry_class_description(self, telemetry_class_id):
            class_name = self.get_telemetry_class_name(telemetry_class_id)
            if not class_name:
                return ""
            return "Sample description for telemetry class %s" % class_name

        def get_telemetry_id(self, telemetry_class_id, telemetry_name):
            class_name = self.get_telemetry_class_name(telemetry_class_id)
            if class_name not in self._up.telemetries:
                return 0
            ids_by_name = self._up.telemetries[class_name]
            if telemetry_name not in ids_by_name:
                return 0
            return ids_by_name[telemetry_name]

        def get_telemetry_name(self, telemetry_class_id, telemetry_id):
            class_name = self.get_telemetry_class_name(telemetry_class_id)
            if class_name not in self._up.telemetries:
                return ""
            ids_by_name = self._up.telemetries[class_name]
            for name, id in ids_by_name.items():
                if id == telemetry_id:
                    return name
            return ""

        def get_telemetry_description(self, telemetry_class_id, telemetry_id):
            telemetry = self.get_telemetry_name(telemetry_class_id,
                                                telemetry_id)
            if not telemetry:
                return ""
            return "Sample description for telemetry " + telemetry

        def get_value(self, telemetry_class_id, telemetry_id):
            telemetry = self.get_telemetry_name(telemetry_class_id,
                                                telemetry_id)
            if telemetry == "STATE":
                return self._up.get_random_state()
            elif telemetry == "VOLTAGE":
                return self._up.get_random_voltage()
            elif telemetry == "FREQUENCY":
                return self._up.get_random_frequency()
            elif telemetry == "TIME_PS":
                return self._up.get_time_ps()
            return None
