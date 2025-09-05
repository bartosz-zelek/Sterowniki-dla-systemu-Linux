###############################################################################
# Disclaimer: this is sample code of a telemetry provider intended to be
# used as part of a Docea Base tutorial.
# By no means the implementation is general or exhaustive:
# we have dropped some validations and hard-coded some variables
# to reduce complexity and favor understanding of the underlying concepts.
# Usage in production is advised against.
###############################################################################

import pyobj
import simics_6_api as simics


class sample_telemetry_consumer_py(pyobj.ConfObject):
    """A sample telemetry consumer object"""

    _class_desc = """A sample telemetry consumer object"""

    # Prevents registration in case the file is imported from another Simics
    # module
    _do_not_init = object()

    def _initialize(self):
        pyobj.ConfObject._initialize(self)
        self.notifier = []
        self.provider = None
        self.upstream_value_telemetry = []
        self.upstream_time_telemetry = []

    class notifier(pyobj.Attribute):
        attrattr = simics.Sim_Attr_Optional
        attrtype = '[os]|[]|n'

        def setter(self, val):
            self._up.notifier = val
            self._up.add_notifier()

        def getter(self):
            return self._up.notifier

    class provider(pyobj.Attribute):
        attrattr = simics.Sim_Attr_Optional
        attrtype = 'o|n'

        def setter(self, val):
            self._up.provider = val

        def getter(self):
            return self._up.provider

    class upstream_value_telemetry(pyobj.Attribute):
        attrattr = simics.Sim_Attr_Optional
        attrtype = '[ss]|[]|n'

        def setter(self, val):
            self._up.upstream_value_telemetry = val

        def getter(self):
            return self._up.upstream_value_telemetry

    class upstream_time_telemetry(pyobj.Attribute):
        attrattr = simics.Sim_Attr_Optional
        attrtype = '[ss]|[]|n'

        def setter(self, val):
            self._up.upstream_time_telemetry = val

        def getter(self):
            return self._up.upstream_time_telemetry

    def add_notifier(self):
        if len(self.notifier) > 0:
            notifier_obj = self.notifier[0]
            notifier_type = simics.SIM_notifier_type(self.notifier[1])
            subscriber = self.obj
            context = None
            simics.SIM_add_notifier(notifier_obj, notifier_type, subscriber,
                                    self.notification_callback, context)

    def notification_callback(self, subscriber, notifier, context):
        value = self.get_value_telemetry()
        time = self.get_time_telemetry()
        msg = "what=notification_callback"
        msg += " value=" + str(value) + " time=" + str(time)
        simics.SIM_log_info(1, self.obj, 0, msg)

    def get_value_telemetry(self):
        telemetry_class_name = self.upstream_value_telemetry[0]
        telemetry_name = self.upstream_value_telemetry[1]
        return self.get_telemetry(telemetry_class_name, telemetry_name)

    def get_time_telemetry(self):
        telemetry_class_name = self.upstream_time_telemetry[0]
        telemetry_name = self.upstream_time_telemetry[1]
        return self.get_telemetry(telemetry_class_name, telemetry_name)

    def get_telemetry(self, telemetry_class_name, telemetry_name):
        telemetry_class_id = self.provider.iface.telemetry.get_telemetry_class_id(  # noqa: E501
            telemetry_class_name)
        telemetry_id = self.provider.iface.telemetry.get_telemetry_id(
            telemetry_class_id, telemetry_name)
        return self.provider.iface.telemetry.get_value(telemetry_class_id,
                                                       telemetry_id)
