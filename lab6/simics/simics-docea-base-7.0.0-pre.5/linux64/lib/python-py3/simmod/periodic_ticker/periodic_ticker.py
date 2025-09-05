from typing import Any

import pyobj
import simics_6_api as simics

_DEFAULT_OUTPUT_EVENT_NAME = "PERIODIC_TICKER_TICK"


class periodic_ticker(pyobj.ConfObject):
    """Periodic ticker.
    Simics object emits notifications periodically."""

    # Once-line doc for the class:
    _class_desc = "Periodic ticker"
    _do_not_init = object()

    event_class_name = "PERIODIC_TICKER_EVENT"
    event_class = None

    def _initialize(self):
        super()._initialize()

        self.set_output_notification(_DEFAULT_OUTPUT_EVENT_NAME)
        self._period_offset_ps = 0
        self._is_warmup_over = False

        # will move to @cls.class_constructor when confclass is used
        if periodic_ticker.event_class is None:
            periodic_ticker.event_class = simics.SIM_register_event(
                periodic_ticker.event_class_name,
                simics.SIM_object_class(self.obj),
                simics.Sim_EC_Notsaved,
                periodic_ticker.do_notification,
                None,
                None,
                None,
                None,
            )

    def _finalize(self):
        if not simics.SIM_object_clock(self.obj):
            raise simics.SimExc_General(
                f"periodic ticker {self.obj.name} has no clock")

        if not simics.SIM_has_notifier(self.obj, self._output_notifier_type):
            simics.SIM_register_notifier(self.obj.classname,
                                         self._output_notifier_type,
                                         self._output_event_name)

        self.init_timing_ps = self.timing_ps()
        # relative to the init timing
        self.latest_notification_relative_timing_ps = -self._period_offset_ps
        self.post_notification()

    def timing_ps(self):
        return simics.SIM_object_clock(self.obj).iface.cycle.get_time_in_ps().t

    def relative_timing_ps(self):  # relative to the init timing
        return self.timing_ps() - self.init_timing_ps

    def set_output_notification(self, value: str) -> None:
        self._output_event_name = value
        self._output_notifier_type = simics.SIM_notifier_type(
            self._output_event_name)

    def _notify(self):
        simics.SIM_notify(self.obj, self._output_notifier_type)

    @staticmethod
    def do_notification(obj: Any, data: Any) -> None:
        self = obj.object_data
        self._notify()
        self.latest_notification_relative_timing_ps = self.relative_timing_ps()
        ms = round(self.latest_notification_relative_timing_ps / 1_000_000_000)
        simics.SIM_log_info(2, self.obj, 0, f"{ms} ms")
        self.post_notification()

    def post_notification(self):
        if self.relative_timing_ps(
        ) + self._warmup_period_ps <= self._warmup_duration_ps:
            time_till_next_notification_ps = self._warmup_period_ps
        else:
            if not self._is_warmup_over:
                self._is_warmup_over = True
                simics.SIM_log_info(2, self.obj, 0, "warmup over")
            time_till_next_notification_ps = self._period_ps

        time_till_next_notification_ps -= self._period_offset_ps
        time_till_next_notification_s = time_till_next_notification_ps * 1e-12
        simics.SIM_event_post_time(self.obj, periodic_ticker.event_class,
                                   self.obj, time_till_next_notification_s,
                                   None)
        self._period_offset_ps = 0

    class warmup_duration_ps(pyobj.Attribute):
        """Duration in ps of the warmup.
        Must be positive. If 0 then no warmup period (warmup is over)."""

        attrattr = simics.Sim_Attr_Required
        attrtype = "i"

        def setter(self, value):
            if simics.SIM_object_is_configured(self._up.obj):
                simics.SIM_attribute_error(
                    """warmup_duration_ps cannot be reset once object is
                     configured""")
                return simics.Sim_Set_Not_Writable

            _warmup_duration_ps = value
            if _warmup_duration_ps < 0:
                simics.SIM_attribute_error("warmup_duration_ps must be >= 0")
                return simics.Sim_Set_Illegal_Value

            self._up._warmup_duration_ps = _warmup_duration_ps
            return simics.Sim_Set_Ok

        def getter(self):
            return max(
                0,
                self._up._warmup_duration_ps - self._up.relative_timing_ps())

    class warmup_period_ps(pyobj.Attribute):
        """Notification period in ps during warmup.
        Must be strictly positive."""

        attrattr = simics.Sim_Attr_Required
        attrtype = "i"

        def setter(self, value):
            if simics.SIM_object_is_configured(self._up.obj):
                simics.SIM_attribute_error(
                    """warmup_period_ps cannot be reset once object is
                     configured""")
                return simics.Sim_Set_Not_Writable

            _warmup_period_ps = value
            if _warmup_period_ps <= 0:
                simics.SIM_attribute_error("warmup_period_ps must be > 0")
                return simics.Sim_Set_Illegal_Value

            self._up._warmup_period_ps = _warmup_period_ps
            return simics.Sim_Set_Ok

        def getter(self):
            return self._up._warmup_period_ps

    class period_ps(pyobj.Attribute):
        """Notification period in ps.
        Must be strictly positive."""

        attrattr = simics.Sim_Attr_Required
        attrtype = "i"

        def setter(self, value):
            if simics.SIM_object_is_configured(self._up.obj):
                simics.SIM_attribute_error(
                    "period_ps cannot be reset once object is configured")
                return simics.Sim_Set_Not_Writable

            _period_ps = value
            if _period_ps <= 0:
                simics.SIM_attribute_error("period_ps must be > 0")
                return simics.Sim_Set_Illegal_Value

            self._up._period_ps = _period_ps
            return simics.Sim_Set_Ok

        def getter(self):
            return self._up._period_ps

    class period_offset_ps(pyobj.Attribute):
        """Period offset in ps.
        Must be positive. Default to 0."""

        attrattr = simics.Sim_Attr_Optional
        attrtype = "i"

        def setter(self, value):
            if simics.SIM_object_is_configured(self._up.obj):
                simics.SIM_attribute_error(
                    """period_offset_ps cannot be reset once object is
                     configured""")
                return simics.Sim_Set_Not_Writable

            _period_offset_ps = value
            if _period_offset_ps < 0:
                simics.SIM_attribute_error("period_offset_ps must be >= 0")
                return simics.Sim_Set_Illegal_Value

            self._up._period_offset_ps = _period_offset_ps
            return simics.Sim_Set_Ok

        def getter(self):
            return self._up.relative_timing_ps(
            ) - self._up.latest_notification_relative_timing_ps

    class output_notification(pyobj.Attribute):
        """Name of the output notification."""

        attrattr = simics.Sim_Attr_Optional
        attrtype = "s"

        def setter(self, value):
            self._up.set_output_notification(value)
            return simics.Sim_Set_Ok

        def getter(self):
            return self._up._output_event_name

    class notify(pyobj.Attribute):
        """Pseudo-attribute: set it to TRUE to trigger a notification.

        Triggering a notification does not schedule any other notification in
        the future"""
        attrattr = simics.Sim_Attr_Pseudo
        attrtype = 'b'

        def setter(self, val):
            if val:
                self._up._notify()

        def getter(self):
            return False
