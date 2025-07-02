import math

import pyobj
import simics_6_api as simics


class power_manager(pyobj.ConfObject):

    _class_desc = ""
    _do_not_init = object()

    def _initialize(self):
        pyobj.ConfObject._initialize(self)
        self.Tdocea = None
        self.Vdocea = None
        self.Fdocea = None
        self.Cdyn = 0
        # Themal throttling parameters:
        self.wp_id = 0
        self.Tcore = 40
        self.prev_error = 0
        self.active_cycles = 0
        self.idle_cycles = 0
        self.thermal_throt_prev_time = None
        self.Tcore_prev = None
        self.i = 0
        # on demand governor setting:
        # 15ms, i.e. 15 x thermal throttling period
        self.ondemand_sampling_rate = 0.015
        # cpu utilisation threshold for ondemand governor in %
        self.ondemand_up_threshold = 80.0
        self.prev_active_cycles = None
        self.ondemand_prev_idle_cycles = None
        self.ondemand_accu_time = 0
        self.ondemand_prev_time = 0
        self.ospm_period_cnt = 0

    def _finalize(self):
        self.thermal_throt_freq = self.workpoints.val[0][1]
        self.scaling_min_freq = self.workpoints.val[-1][1]
        self.scaling_max_freq = self.workpoints.val[0][1]
        if self.governor.val in ["ondemand", "powersave"]:
            self.cpu_freq_req = self.scaling_min_freq
        else:
            self.cpu_freq_req = self.scaling_max_freq
        simics.SIM_register_notifier(power_manager.__name__,
                                     simics.Sim_Notify_Frequency_Change,
                                     "Sim_Notify_Frequency_Change")
        self.avg_Cdyn = self.cdyn_virus.val
        self.Fcore = self.workpoints.val[self.wp_id][1]
        self.Vcore = self.workpoints.val[self.wp_id][0]
        self.sumerror = self.thermal_throt_limit.val - self.Tcore
        self.event.post_next(None)
        self.derived_class_id = self.path_mini_perf.val.get_telemetry_class_id(
            "SAMPLE_CORE_TIMING_PERIOD_DERIVED")
        self.cumulative_class_id = self.path_mini_perf.val.get_telemetry_class_id(  # noqa: E501
            "SAMPLE_CORE_TIMING_CUMULATIVE")
        self.cdyn_id = self.path_mini_perf.val.get_telemetry_id(
            self.derived_class_id, "ACTIVITY_PER_CYCLE")
        self.active_cycles_id = self.path_mini_perf.val.get_telemetry_id(
            self.cumulative_class_id, "ACTIVE_CYCLES")
        self.idle_cycles_id = self.path_mini_perf.val.get_telemetry_id(
            self.cumulative_class_id, "IDLE_CYCLES")

    def compute_V(self, F):
        if F > self.workpoints.val[0][1]:
            simics.SIM_log_info(1, self.obj, 0,
                                f"Requested frequency out of range: {F}")
            return None
        else:
            for id in range(len(self.workpoints.val) - 1):
                if F <= self.workpoints.val[id][1]:
                    V_id = id
            return self.workpoints.val[V_id][0]

    def ospm(self, time):
        """
        Computes the frequency request from cpu load estimation.
        C0 residency is used as cpu load proxy.
        C0 residency is computed from active and idle cycles.
        Telemetry is provided by mini-performance.
        ospm is executed every self.ondemand_sampling_rate.
        """
        if self.governor.val == "ondemand":
            if self.ondemand_prev_time is not None:
                delta_time = time - self.ondemand_prev_time
                self.ondemand_accu_time += delta_time
            if self.prev_active_cycles is None:
                self.prev_active_cycles = self.active_cycles
                self.prev_idle_cycles = self.idle_cycles
            if self.ondemand_accu_time >= self.ondemand_sampling_rate:
                delta_active = float(self.active_cycles) - float(
                    self.prev_active_cycles)
                delta_idle = float(self.idle_cycles) - float(
                    self.prev_idle_cycles)
                if (delta_active + delta_idle) == 0:
                    cpu_load = 0
                else:
                    cpu_load = delta_active / (delta_active + delta_idle)
                self.prev_idle_cycles = self.idle_cycles
                self.prev_active_cycles = self.active_cycles
                # self.accu_cpu_load = 0
                self.ondemand_accu_time = 0
                if cpu_load > self.ondemand_up_threshold / 100.0:
                    self.cpu_freq_req = self.scaling_max_freq
                else:
                    # Frequency set proportional to the estimated load:
                    self.cpu_freq_req = self.scaling_min_freq + cpu_load * (
                        self.scaling_max_freq - self.scaling_min_freq)
                    # rounded to upper bin (multiple of 100 MHz):
                    self.cpu_freq_req = math.ceil(
                        self.cpu_freq_req / 1e8) * 1e8
            self.ondemand_prev_time = time
        elif self.governor.val == "performance":
            self.cpu_freq_req = self.scaling_max_freq
        elif self.governor.val == "powersave":
            self.cpu_freq_req = self.scaling_min_freq
        else:
            simics.SIM_log_info(
                1, self.obj, 0,
                f"Non implemented governor: {self.governor.val}")
        return self.cpu_freq_req

    def thermal_throttling(self, time):
        """Computes the new working point for the core.
        Controls Tcore - thermal_throt_limit.val"""
        if self.thermal_throt_prev_time is not None:
            dt = time - self.thermal_throt_prev_time
        else:
            dt = 0.001
        if self.Tcore_prev is not None:
            d_T = self.Tcore - self.Tcore_prev
        else:
            d_T = 0
        error = self.thermal_throt_limit.val - self.Tcore

        Ki = self.ki.val
        Kp = self.kp.val
        Kd = self.kd.val
        self.i += Ki * error * dt
        self.p = Kp * error
        self.d = -Kd * d_T / dt

        out = self.i + self.p + self.d
        if out < self.thermal_hysteresis.val[
                0] and self.thermal_throt_freq > self.workpoints.val[-1][1]:
            self.thermal_throt_freq -= 1e8  # one bin lower (100MHz)
        elif out > self.thermal_hysteresis.val[
                1] and self.thermal_throt_freq < self.workpoints.val[0][1]:
            self.thermal_throt_freq += 1e8

        self.prev_error = error
        self.thermal_throt_prev_time = time
        self.Tcore_prev = self.Tcore
        return self.thermal_throt_freq

    def compute_VF(self, time):
        """Chooses between thermal throttling and ospm"""
        ospm_freq_req = self.ospm(time)
        thermal_throt_freq = self.thermal_throttling(time)

        # min between OS (ospm) and thermal throttling frequency is selected:
        self.Fcore = min(thermal_throt_freq, ospm_freq_req)
        self.Vcore = self.compute_V(self.Fcore)

    def callback_action(self, time):
        time_ps = int(time * 1e12)
        T_core = self.Tdocea.iface.readable_float.read(time_ps)
        self.Cdyn = self.path_mini_perf.val.get_value(self.derived_class_id,
                                                      self.cdyn_id)
        self.active_cycles = self.path_mini_perf.val.get_value(
            self.cumulative_class_id, self.active_cycles_id)
        self.idle_cycles = self.path_mini_perf.val.get_value(
            self.cumulative_class_id, self.idle_cycles_id)
        self.Tcore = T_core.value
        self.compute_VF(time)
        self.Fdocea.iface.writable_float.write(time_ps, self.Fcore)
        self.Vdocea.iface.writable_float.write(time_ps, self.Vcore)
        self._notify()

    def _notify(self):
        simics.SIM_log_info(
            3, self.obj, 0,
            "time=%.12f time_unit=s what=notify" % simics.SIM_time(self.obj))
        simics.SIM_notify(self.obj, simics.Sim_Notify_Frequency_Change)

    class event(pyobj.Event):
        def post_next(self, data):
            self.post(simics.SIM_object_clock(self._up.obj.queue),
                      data,
                      seconds=self._up.invocation_interval.val)

        def callback(self, data):
            time = simics.SIM_time(self._up.obj.queue)
            self._up.callback_action(time)
            self.post_next(data)

    class invocation_interval(pyobj.SimpleAttribute(0.001, type='f')):
        """Time between invocations of thermal throttling (s)"""

    class cdyn_virus(pyobj.SimpleAttribute(1.05e-9, type='f')):
        """Minimum value for Cdyn"""

    class governor(pyobj.SimpleAttribute("ondemand", type='s')):
        """Governor select"""

    class workpoints(
            pyobj.SimpleAttribute([[1.2, 4.0e9], [1.16, 3.8e9], [1.1, 3.6e9],
                                   [1.06, 3.2e9], [1.0, 3e9], [0.97, 2.7e9],
                                   [0.90, 2e9], [0.88, 1.7e9], [0.85, 1e9]],
                                  type='[[ff]+]')):
        """The workpoint table, containing pairs of voltage
           and clock frequencies (V,Hz)."""

    class thermal_hysteresis(pyobj.SimpleAttribute([-5.0, 5.0], type='[ff]')):
        """Power Manager stays on the same Workpoint if:
        (if thermal_hysteresis[0] < error < thermal_hysteresis[1])"""

    class kd(pyobj.SimpleAttribute(1.0, type='f')):
        """Kd value for the PD controller (see role of Kd in Compute_VF)"""

    class kp(pyobj.SimpleAttribute(1.0, type='f')):
        """Kp value for the PD controller (see role of Kp in Compute_VF)"""

    class ki(pyobj.SimpleAttribute(1.0, type='f')):
        """Ki value for the PD controller (see role of Ki in Compute_VF)"""

    class thermal_throt_limit(pyobj.SimpleAttribute(100.0, type='f')):
        """Limit temperature of the core (for thermal throttling)"""

    class Tdocea(pyobj.Attribute):
        """Docea path to the Temperature of the core"""
        attrattr = simics.Sim_Attr_Pseudo
        attrtype = 'o'

        def setter(self, val):
            self._up.Tdocea = val

    class path_mini_perf(pyobj.Attribute):
        """Path to the mini perf module"""
        attrattr = simics.Sim_Attr_Pseudo
        attrtype = 'a'

        def setter(self, mini_perf):
            self.val = simics.SIM_get_interface(mini_perf, "telemetry")

        def getter(self):
            return self.val

    class Vdocea(pyobj.Attribute):
        """Docea path to the Voltage of the core"""
        attrattr = simics.Sim_Attr_Pseudo
        attrtype = 'o'

        def setter(self, val):
            self._up.Vdocea = val

    class Fdocea(pyobj.Attribute):
        """Docea path to the Frequency of the core"""
        attrattr = simics.Sim_Attr_Pseudo
        attrtype = 'o'

        def setter(self, val):
            self._up.Fdocea = val

    class freq(pyobj.Attribute):
        """Frequency of the core"""
        attrtype = "f"

        def getter(self):
            return self._up.Fcore

        def setter(self, val):
            self._up.Fcore = val
            simics.SIM_notify(self._up.obj, simics.Sim_Notify_Frequency_Change)

    class frequency(pyobj.Interface):
        """Frequency interface"""
        def get(self):
            return self._up.Fcore
