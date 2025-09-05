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


class sample_notifier_py(pyobj.ConfObject):
    """A sample notifier object"""

    _class_desc = """A sample notifier object"""

    # Prevents registration in case the file is imported from another module
    _do_not_init = object()

    def _initialize(self):
        pyobj.ConfObject._initialize(self)
        self.notifier_type_name = "SAMPLE-NOTIFIED-EVENT"
        self._register_notifier()

    def _register_notifier(self):
        simics.SIM_register_notifier(
            "sample_notifier_py",
            simics.SIM_notifier_type(self.notifier_type_name),
            self.notifier_type_name)

    class do_notification(pyobj.Attribute):
        """Pseudo-attribute: set it to TRUE to trigger a notification."""
        attrattr = simics.Sim_Attr_Pseudo
        attrtype = 'b'

        def setter(self, val):
            if val:
                self._up._notify()

        def getter(self):
            return False

    def _notify(self):
        simics.SIM_log_info(1, self.obj, 0, "what=notify")
        simics.SIM_notify(self.obj,
                          simics.SIM_notifier_type(self.notifier_type_name))
