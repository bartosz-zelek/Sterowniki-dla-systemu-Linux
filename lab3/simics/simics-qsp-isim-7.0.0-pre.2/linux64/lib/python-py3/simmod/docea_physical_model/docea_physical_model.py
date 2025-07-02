import csv
import math
import os
from collections import deque

import pyobj
import simics_6_api as simics

simics.SIM_load_module("docea-ptm-interface")
from simmod.docea_ptm_interface import docea_ptm2_interface  # noqa: E402

simics.SIM_load_module("docea-perf-interface")
from simmod.docea_perf_interface import docea_perf2_interface  # noqa: E402


class DynamicPower():
    def __init__(self, cdyn_idle):
        self.V = 1.0
        self.F = 1e9
        self.cdyn = 1e-09
        self.value = 0
        self.cdyn_idle = cdyn_idle

    def _solve(self):
        self.value = (self.cdyn + self.cdyn_idle) * self.V * self.V * self.F

    def solve_and_get(self):
        self._solve()
        return self.value


class StaticPower():
    def __init__(self, ambient, reference, leakage_parameter):
        self.V = 1.0
        self.T = ambient
        self.i = 0.002
        self.reference = reference
        self.value = 0
        self.leakage_parameter = leakage_parameter

    def _solve(self):
        self.value = (
            self.i * self.V * math.exp(self.leakage_parameter *  # noqa: W504
                                       (self.T - self.reference)))

    def solve_and_get(self):
        self._solve()
        return self.value


class Temperature():
    def __init__(self, R, C, ambient):
        self.R = R
        self.C = C
        self.amb = ambient
        self.value = ambient
        self.power = 10

    def _solve(self, dt):
        tau = self.R * self.C
        if dt != 0:
            self.value = (((self.power * self.R) + self.amb +  # noqa: W504
                           (tau / dt) * self.value) / (1 + (tau / dt)))

    def solve_and_get(self, dt):
        self._solve(dt)
        return self.value


class Core():
    def __init__(self,
                 path_mini_perf,
                 R=40.0,
                 C=0.1,
                 ambient=40.0,
                 reference=40.0,
                 leakage_parameter=0.069,
                 initial_time=0.0,
                 cdyn_idle=1.5e-10):
        # Signals
        self.V = 1.0
        self.F = 1e9
        self.cdyn = 1e-09
        self.IPC = 1.0
        # Models
        self.dyn_model = DynamicPower(cdyn_idle)
        self.sta_model = StaticPower(ambient, reference, leakage_parameter)
        self.temp_model = Temperature(R, C, ambient)
        # PT signals
        self.Temperature = ambient
        self.Power_Dyn = 0.0
        self.instructions = 0
        self.Power_Stat = 0.0
        self.Power = 0.0
        self.time = initial_time
        # Path to perf provider
        self.path_mini_perf = path_mini_perf
        self._perf_paths_setter()
        # Simulation metrics
        self.Energy_Per_Instruction = 0.0
        # Average metrics
        self.avg_Energy_Per_Instruction = {
            'value': 0.0,
            'nb_of_data_points': 0
        }
        self.avg_freq = {'value': 0.0, 'nb_of_data_points': 0}

    def _perf_paths_setter(self):
        self.derived_class_id = self.path_mini_perf.get_telemetry_class_id(
            "SAMPLE_CORE_TIMING_PERIOD_DERIVED")
        self.active_cdyn_id = self.path_mini_perf.get_telemetry_id(
            self.derived_class_id, "ACTIVITY_PER_CYCLE")
        self.cumulative_class_id = self.path_mini_perf.get_telemetry_class_id(
            "SAMPLE_CORE_TIMING_CUMULATIVE")
        self.instructions_id = self.path_mini_perf.get_telemetry_id(
            self.cumulative_class_id, "INSTRUCTIONS")

    def _metrics_compute(self, dt):
        nrj = self.Power * dt
        new_instructions = self.path_mini_perf.get_value(
            self.cumulative_class_id, self.instructions_id)
        Dinstructions = new_instructions - self.instructions
        self.instructions = new_instructions
        if Dinstructions > 0.0:
            self.Energy_Per_Instruction = nrj / Dinstructions
        else:
            self.Energy_Per_Instruction = nrj / 1
        self.avg_Energy_Per_Instruction['value'] = (
            self.avg_Energy_Per_Instruction['value']
            * self.avg_Energy_Per_Instruction['nb_of_data_points']
            + self.Energy_Per_Instruction) / (
                self.avg_Energy_Per_Instruction['nb_of_data_points'] + 1)
        self.avg_Energy_Per_Instruction['nb_of_data_points'] += 1

    def update_cdyn_idle(self, val):
        self.dyn_model.cdyn_idle = val

    def update_leakage_parameter(self, val):
        self.sta_model.leakage_parameter = val

    def update_thermal_R(self, val):
        self.temp_model.R = val

    def update_thermal_C(self, val):
        self.temp_model.C = val

    def _apply_inputs(self):
        # Apply voltage
        self.dyn_model.V = self.V
        self.sta_model.V = self.V
        # Apply frequency and Cdyn
        self.dyn_model.F = self.F
        self.dyn_model.cdyn = self.cdyn

    def set_Cdyn(self, cdyn, time_s):
        self.cdyn = cdyn * 1e-12

    def set_V(self, V, time_s=None):
        self.V = V

    def set_IPC(self, ipc, time_s):
        self.IPC = ipc
        self.cdyn = self.path_mini_perf.get_value(self.derived_class_id,
                                                  self.active_cdyn_id) * 1e-12
        self._solve_PT(time_s)

    def set_F(self, F, time_s=None):
        self.F = F
        self.avg_freq['value'] = (
            self.avg_freq['value'] * self.avg_freq['nb_of_data_points']
            + self.F) / (self.avg_freq['nb_of_data_points'] + 1)
        self.avg_freq['nb_of_data_points'] += 1

    def get_Cdyn(self):
        return (self.cdyn)

    def get_IPC(self):
        return (self.IPC)

    def get_avg_F(self):
        return (self.avg_freq['value'])

    def get_avg_Energy_Per_Instruction(self):
        return (self.avg_Energy_Per_Instruction['value'])

    def get_V(self):
        return (self.V)

    def get_F(self):
        return (self.F * 1e-9)

    def get_Temperature(self):
        return (self.Temperature)

    def get_Power(self):
        return (self.Power)

    def get_Current(self):
        return (self.Power / self.V)

    def get_Energy_Per_Instruction(self):
        return (self.Energy_Per_Instruction)

    def _solve_PT(self, time_s):
        dt = time_s - self.time
        self._apply_inputs()
        self.sta_model.T = self.Temperature
        self.Power_Dyn = self.dyn_model.solve_and_get()
        self.Power_Stat = self.sta_model.solve_and_get()
        self.Power = self.Power_Dyn + self.Power_Stat
        self.temp_model.power = self.Power
        self.Temperature = self.temp_model.solve_and_get(dt)
        self._metrics_compute(dt)
        self.time = time_s


class Streamer():
    def __init__(self, signals_to_observe, output_file, time):
        self.time = time
        self.delta_time_averaging = 0.002
        self.buffer_size = 70
        self.signals = signals_to_observe
        self.output_file = output_file
        self.signals_values = {signal: deque([]) for signal in self.signals}
        self.averages = [None] * len(self.signals)
        self.simulation_data = deque([])
        self._init_csv()

    def push(self, time, signal, value):
        if signal in self.signals_values:
            self.signals_values[signal].append(value)
            if time >= self.time + self.delta_time_averaging:
                self.time = time
                self._add_to_simulation_datas()
            if len(self.simulation_data) >= self.buffer_size:
                self._output_data()

    def _init_csv(self):
        with open(self.output_file, 'w', newline='') as f:
            w = csv.writer(f)
            w.writerow(['time'] + self.signals)

    def _add_to_simulation_datas(self):
        self._average_and_clear()
        self.simulation_data.append([int(self.time * 1e12)] + self.averages)

    def _output_data(self):
        with open(self.output_file, 'a', newline='') as f:
            w = csv.writer(f)
            while self.simulation_data:
                w.writerow(self.simulation_data.popleft())

    def _average_and_clear(self):
        for i in range(len(self.signals)):
            if self.signals_values[self.signals[i]]:
                self.averages[i] = sum(
                    self.signals_values[self.signals[i]]) / len(
                        self.signals_values[self.signals[i]])
                self.signals_values[self.signals[i]].clear()


class docea_physical_model(pyobj.ConfObject):

    _class_desc = "one-line doc for the class"
    _do_not_init = object()

    def _initialize(self):
        pyobj.ConfObject._initialize(self)
        self.Cores = []

    def _finalize(self):
        for i in range(self.nb_cores.val):
            self.Cores.append(
                Core(self.path_mini_perfs.val[i], self.thermal_R.val,
                     self.thermal_C.val, self.ambient.val,
                     self.reference_temp.val, self.leakage_parameter.val,
                     self.initial_time_s.val[i], self.cdyn_idle.val))
        self.T_paths = [f'T_core{i}' for i in range(self.nb_cores.val)]
        self.V_paths = [f'V_core{i}' for i in range(self.nb_cores.val)]
        self.F_paths = [f'F_core{i}' for i in range(self.nb_cores.val)]
        self.Cdyn_paths = [f'Cdyn_core{i}' for i in range(self.nb_cores.val)]
        self.IPC_paths = [f'IPC_core{i}' for i in range(self.nb_cores.val)]
        self.I_paths = [f'I_core{i}' for i in range(self.nb_cores.val)]
        self.P_paths = [f'P_core{i}' for i in range(self.nb_cores.val)]
        self.Energy_Per_Instruction_paths = [
            f'Energy_Per_Instruction_core{i}' for i in range(self.nb_cores.val)
        ]
        self.map_signal_setter = {}
        self.map_signal_getter = {}
        self.PT_computation_signal = {
            self.IPC_paths[i]: [
                self.I_paths[i], self.P_paths[i], self.T_paths[i],
                self.Energy_Per_Instruction_paths[i]
            ]
            for i in range(self.nb_cores.val)
        }
        for i in range(self.nb_cores.val):
            # Getters
            self.map_signal_getter[
                self.T_paths[i]] = self.Cores[i].get_Temperature
            self.map_signal_getter[self.P_paths[i]] = self.Cores[i].get_Power
            self.map_signal_getter[self.I_paths[i]] = self.Cores[i].get_Current
            self.map_signal_getter[self.V_paths[i]] = self.Cores[i].get_V
            self.map_signal_getter[self.F_paths[i]] = self.Cores[i].get_F
            self.map_signal_getter[self.Cdyn_paths[i]] = self.Cores[i].get_Cdyn
            self.map_signal_getter[self.IPC_paths[i]] = self.Cores[i].get_IPC
            self.map_signal_getter[self.Energy_Per_Instruction_paths[
                i]] = self.Cores[i].get_Energy_Per_Instruction
            # Setters
            self.map_signal_setter[self.V_paths[i]] = self.Cores[i].set_V
            self.map_signal_setter[self.F_paths[i]] = self.Cores[i].set_F
            self.map_signal_setter[self.Cdyn_paths[i]] = self.Cores[i].set_Cdyn
            self.map_signal_setter[self.IPC_paths[i]] = self.Cores[i].set_IPC
        self.streamer = Streamer(self.signals_to_observe.val,
                                 self.output_file.val,
                                 max(self.initial_time_s.val))

    class nb_cores(pyobj.SimpleAttribute(1, type='i')):
        """Number of Cores in the simulation"""

    class ambient(pyobj.SimpleAttribute(40.0, type='f')):
        """Ambient temperature"""

    class reference_temp(pyobj.SimpleAttribute(40.0, type='f')):
        """Leakage reference temperature"""

    class output_file(pyobj.Attribute):
        """CSV file to write simulation results"""
        attrattr = simics.Sim_Attr_Pseudo
        attrtype = 's'

        def _initialize(self):
            result_dir = "logs/sim_result"
            if not os.path.exists(result_dir):
                os.makedirs(result_dir)
            self.val = result_dir + '/tracer0.csv'

        def setter(self, val):
            self.val = val

        def getter(self):
            return self.val

    class path_mini_perfs(pyobj.Attribute):
        """Path to the mini perf module"""
        attrattr = simics.Sim_Attr_Pseudo
        attrtype = 'a'

        def setter(self, list_mini_perfs):
            self.val = [
                simics.SIM_get_interface(mini_perf, "telemetry")
                for mini_perf in list_mini_perfs
            ]

        def getter(self):
            return self.val

    class leakage_parameter(pyobj.Attribute):
        """Parameter used in the temperature exponential (static power)"""
        attrattr = simics.Sim_Attr_Pseudo
        attrtype = 'f'

        def _initialize(self):
            self.val = 0.069

        def setter(self, val):
            self.val = val
            self._apply_update()

        def getter(self):
            return self.val

        def _apply_update(self):
            if self._up.Cores:
                for i in range(self._up.nb_cores.val):
                    self._up.Cores[i].update_leakage_parameter(self.val)

    class thermal_R(pyobj.Attribute):
        """Thermal resistance of the cores"""
        attrattr = simics.Sim_Attr_Pseudo
        attrtype = 'f'

        def _initialize(self):
            self.val = 40.0

        def setter(self, val):
            self.val = val
            self._apply_update()

        def getter(self):
            return self.val

        def _apply_update(self):
            if self._up.Cores:
                for i in range(self._up.nb_cores.val):
                    self._up.Cores[i].update_thermal_R(self.val)

    class thermal_C(pyobj.Attribute):
        """Thermal capacitance of the cores"""
        attrattr = simics.Sim_Attr_Pseudo
        attrtype = 'f'

        def _initialize(self):
            self.val = 0.1

        def setter(self, val):
            self.val = val
            self._apply_update()

        def getter(self):
            return self.val

        def _apply_update(self):
            if self._up.Cores:
                for i in range(self._up.nb_cores.val):
                    self._up.Cores[i].update_thermal_C(self.val)

    class cdyn_idle(pyobj.Attribute):
        """Minimum value of Cdyn"""
        attrattr = simics.Sim_Attr_Pseudo
        attrtype = 'f'

        def _initialize(self):
            self.val = 1.5e-10

        def setter(self, val):
            self.val = val
            self._apply_update()

        def getter(self):
            return self.val

        def _apply_update(self):
            if self._up.Cores:
                for i in range(self._up.nb_cores.val):
                    self._up.Cores[i].update_cdyn_idle(self.val)

    class initial_time_s(pyobj.Attribute):
        """Time when Docea get enabled
        in the simulation (relative to each core)"""
        attrattr = simics.Sim_Attr_Pseudo
        attrtype = '[f+]'

        def _initialize(self):
            self.val = [0.0]

        def setter(self, val):
            self.val = val

        def getter(self):
            return self.val

    class signals_to_observe(pyobj.Attribute):
        """Signals the user wants to ouptut in the output_file.
        Ordered by each core and its signals"""
        attrattr = simics.Sim_Attr_Pseudo
        attrtype = '[s+]'

        def _initialize(self):
            self.val = [
                "T_core0", "IPC_core0", "F_core0", "P_core0",
                "Energy_Per_Instruction_core0", "Cdyn_core0"
            ]

        def setter(self, val):
            self.val = val

        def getter(self):
            return self.val

    class avg_metrics(pyobj.Attribute):
        """Access to the average metrics computed in the cores"""
        attrattr = simics.Sim_Attr_Pseudo
        attrtype = 'D'

        def getter(self):
            list_of_avg_F = [
                self._up.Cores[i].get_avg_F()
                for i in range(self._up.nb_cores.val)
            ]
            list_of_avg_Energy_Per_Instruction = [
                self._up.Cores[i].get_avg_Energy_Per_Instruction()
                for i in range(self._up.nb_cores.val)
            ]
            avg_freq = sum(list_of_avg_F) / len(list_of_avg_F)
            avg_Energy_Per_Instruction = sum(
                list_of_avg_Energy_Per_Instruction) / len(
                    list_of_avg_Energy_Per_Instruction)
            return ({
                "Frequency": avg_freq,
                "Energy_Per_Instruction": avg_Energy_Per_Instruction
            })

    class all_signals_values(pyobj.Attribute):
        def setter(self):
            pass

        def getter(self):
            self.val = dict()
            for signal in self._up.map_signal_getter:
                self.val[signal] = self._up.map_signal_getter[signal]()
            return self.val

    def set_signal(self, time_s, name, value):
        self.map_signal_setter[name](value, time_s)
        if name in self.signals_to_observe.val:
            self.streamer.push(time_s, name, self.map_signal_getter[name]())
        if name in self.PT_computation_signal:
            for name_computed_signals in self.PT_computation_signal[name]:
                if name_computed_signals in self.signals_to_observe.val:
                    self.streamer.push(
                        time_s, name_computed_signals,
                        self.map_signal_getter[name_computed_signals]())
        simics.SIM_log_info(3, self.obj, 0,
                            f"{name} time: {time_s}, new_value: {value}")

    class docea_ptm2(pyobj.Interface):
        def set_frequency(self, time_ps, name, value):
            time_s = 1e-12 * time_ps
            self._up.set_signal(time_s, name, value)
            r = docea_ptm2_interface.docea_ptm2_status_result_t()
            r.status = 0
            return r

        def set_voltage(self, time_ps, name, value):
            time_s = 1e-12 * time_ps
            self._up.set_signal(time_s, name, value)
            r = docea_ptm2_interface.docea_ptm2_status_result_t()
            r.status = 0
            return r

        def get_current(self, time_ps, name):
            result = docea_ptm2_interface.docea_ptm2_double_timed_result_t()
            result.status, result.value, result.time_ps = 1, 0.0, 0

            if time_ps < 0:
                return result

            current = self._up.map_signal_getter[name]()

            result.status, result.value, result.time_ps = 0, current, time_ps
            return result

        def get_temperature(self, time_ps, name):
            result = docea_ptm2_interface.docea_ptm2_double_timed_result_t()
            result.status, result.value, result.time_ps = 1, 0.0, 0

            if time_ps < 0:
                return result

            T = self._up.map_signal_getter[name]()
            t = round(T, 2)

            result.status, result.value, result.time_ps = 0, t, time_ps
            return result

    class docea_perf2(pyobj.Interface):
        def set_past_cdyn(self, time_ps, name, value):
            time_s = time_ps * 1e-12
            self._up.set_signal(time_s, name, value)
            r = docea_perf2_interface.docea_perf2_status_result_t()
            r.status = 0
            return r

    class docea_perf(pyobj.Interface):
        pass

    class docea_ptm(pyobj.Interface):
        pass
