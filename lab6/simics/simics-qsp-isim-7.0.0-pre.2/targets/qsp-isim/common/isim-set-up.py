import json
import os

from simmod.power_thermal import model_objects

import simics


class Isim_Setter():
    def __init__(self, machine_name, logs_fol):
        self.machine_name = machine_name
        self.machine = simics.SIM_get_object(self.machine_name)
        self.nb_cores = len(self.machine.cpu_list)
        self.docea_name = self.machine_name + '.docea'
        pre_docea = simics.pre_conf_object(self.docea_name, 'namespace')
        simics.SIM_add_configuration([pre_docea], None)
        self.docea = simics.SIM_get_object(self.docea_name)
        self.model_name = self.docea_name + '.model'
        self.results_folder = logs_fol + "/sim_result"
        self.metadata_folder = logs_fol + "/metadatas"

    def set_behavioral_module(self, out, signals_to_observe, thermal_C,
                              thermal_R, leakage_parameter, ambient,
                              reference_temp, cdyn_idle):
        if not os.path.exists(self.results_folder):
            os.makedirs(self.results_folder)
        self.docea_physical_name = self.docea_name + '.docea_physical_model'
        pre_docea_physical_model = simics.pre_conf_object(
            self.docea_physical_name, "docea_physical_model")
        pre_docea_physical_model.nb_cores = self.nb_cores
        pre_docea_physical_model.output_file = self.results_folder + "/" + out
        pre_docea_physical_model.signals_to_observe = signals_to_observe
        pre_docea_physical_model.thermal_C = thermal_C
        pre_docea_physical_model.thermal_R = thermal_R
        pre_docea_physical_model.leakage_parameter = leakage_parameter
        pre_docea_physical_model.ambient = ambient
        pre_docea_physical_model.reference_temp = reference_temp
        pre_docea_physical_model.cdyn_idle = cdyn_idle
        pre_docea_physical_model.path_mini_perfs = [
            self.machine.sample_core_timing.connections[i]
            for i in range(self.nb_cores)
        ]
        time_s = [
            simics.SIM_time(
                simics.SIM_get_object(self.machine_name
                                      + f'.mb.cpu0.core[{i}][0]'))
            for i in range(self.nb_cores)
        ]
        pre_docea_physical_model.initial_time_s = time_s
        simics.SIM_add_configuration([pre_docea_physical_model], None)
        self.docea_physical_model = simics.SIM_get_object(
            self.docea_physical_name)

    def set_interface_modules(self, path_to_metadata, time_provider=None):
        self.T_paths = [f'T_core{i}' for i in range(self.nb_cores)]
        self.V_paths = [f'V_core{i}' for i in range(self.nb_cores)]
        self.F_paths = [f'F_core{i}' for i in range(self.nb_cores)]
        self.Cdyn_paths = [f'Cdyn_core{i}' for i in range(self.nb_cores)]
        self.IPC_paths = [f'IPC_core{i}' for i in range(self.nb_cores)]
        self.I_paths = [f'I_core{i}' for i in range(self.nb_cores)]
        self.IPC_paths = [f'IPC_core{i}' for i in range(self.nb_cores)]
        metadata = {
            "version": 2,
            "interface": {
                "inputs": {
                    "cdyns": [{
                        "name": self.Cdyn_paths[i]
                    } for i in range(self.nb_cores)],
                    "frequencies": [{
                        "name": self.F_paths[i]
                    } for i in range(self.nb_cores)],
                    "voltages": [{
                        "name": self.V_paths[i]
                    } for i in range(self.nb_cores)]
                },
                "outputs": {
                    "currents": [{
                        "name": self.I_paths[i]
                    } for i in range(self.nb_cores)],
                    "temperatures": [{
                        "name": self.T_paths[i]
                    } for i in range(self.nb_cores)]
                }
            }
        }
        for i in range(self.nb_cores):
            metadata["interface"]["inputs"]["cdyns"].append(
                {"name": self.IPC_paths[i]})
        if not os.path.exists(self.metadata_folder):
            os.makedirs(self.metadata_folder)
        with open(self.metadata_folder + '/' + path_to_metadata,
                  "w") as outfile:
            json.dump(metadata, outfile)
        model_objects.create_from_metadata_file(
            self.model_name, self.metadata_folder + '/' + path_to_metadata,
            self.docea_physical_model)
        if time_provider is not None:
            model_objects.set_time_provider_to_hierarchy(
                self.model_name, time_provider)

    def set_power_managers(self, thermal_throt_limit, thermal_hysteresis,
                           kdict, governor, workpoints, cdyn_virus):
        self.power_managers_name = self.machine_name + '.power_managers'
        pre_power_managers = simics.pre_conf_object(self.power_managers_name,
                                                    'index-map')
        for i in range(self.nb_cores):
            pre_power_managers[i] = simics.pre_conf_object('power_manager')
            pre_power_managers[i].Tdocea = simics.SIM_get_object(
                self.docea.model.name + "." + self.T_paths[i])
            pre_power_managers[i].Vdocea = simics.SIM_get_object(
                self.docea.model.name + "." + self.V_paths[i])
            pre_power_managers[i].Fdocea = simics.SIM_get_object(
                self.docea.model.name + "." + self.F_paths[i])
            pre_power_managers[
                i].path_mini_perf = self.machine.sample_core_timing.connections[  # noqa: E501
                    i]
            pre_power_managers[i].thermal_throt_limit = thermal_throt_limit
            pre_power_managers[i].thermal_hysteresis = thermal_hysteresis
            pre_power_managers[i].queue = simics.SIM_get_object(
                self.machine_name + f'.mb.cpu0.core[{i}][0]')
            pre_power_managers[i].kp = kdict['Kp']
            pre_power_managers[i].kd = kdict['Kd']
            pre_power_managers[i].ki = kdict['Ki']
            pre_power_managers[i].governor = governor
            pre_power_managers[i].workpoints = workpoints
            pre_power_managers[i].cdyn_virus = cdyn_virus
        simics.SIM_add_configuration([pre_power_managers], None)
        self.power_managers = simics.SIM_get_object(self.power_managers_name)
        for i in range(self.nb_cores):
            core = simics.SIM_get_object(self.machine_name
                                         + f'.mb.cpu0.core[{i}][0]')
            core.frequency = self.power_managers[i]

    def set_perf_to_docea(self):
        for i in range(self.nb_cores):
            core = simics.SIM_get_object(self.machine_name
                                         + f'.mb.cpu0.core[{i}][0]')
            cdyn_path = simics.SIM_get_object(self.model_name + "."
                                              + self.Cdyn_paths[i])
            cdyn_path.notifier = [core, "isim-telemetry-update"]
            cdyn_path.upstream_time_telemetry = [
                "SAMPLE_CORE_TIMING_PERIOD", "TIME"
            ]
            cdyn_path.upstream_value_telemetry = [
                "SAMPLE_CORE_TIMING_PERIOD_DERIVED", "ACTIVITY_PER_CYCLE"
            ]
            ipc_path = simics.SIM_get_object(self.model_name + "."
                                             + self.IPC_paths[i])
            ipc_path.notifier = [core, "isim-telemetry-update"]
            ipc_path.upstream_time_telemetry = [
                "SAMPLE_CORE_TIMING_PERIOD", "TIME"
            ]
            ipc_path.upstream_value_telemetry = [
                "SAMPLE_CORE_TIMING_PERIOD_DERIVED", "INSTRUCTION_PER_CYCLE"
            ]


def build_view(result_file):
    import pandas as pd  # type: ignore
    import plotly.graph_objects as go  # type: ignore
    import plotly.subplots as sub  # type: ignore

    df = pd.read_csv(result_file)
    time_s = df["time"] * 1e-12

    all_charts = []

    power_charts = []
    for y_label in ["P_core0", "P_core1", "P_core2"]:
        power_charts += [go.Scattergl(x=time_s, y=df[y_label], name=y_label)]

    temp_charts = []
    for y_label in ["T_core0", "T_core1", "T_core2"]:
        temp_charts += [go.Scattergl(x=time_s, y=df[y_label], name=y_label)]

    freq_charts = []
    for y_label in ["F_core0", "F_core1", "F_core2"]:
        freq_charts += [go.Scattergl(x=time_s, y=df[y_label], name=y_label)]

    ipc_charts = []
    for y_label in ["IPC_core0", "IPC_core1", "IPC_core2"]:
        ipc_charts += [go.Scattergl(x=time_s, y=df[y_label], name=y_label)]

    row_titles = [
        "Power per core",
        "Temperature per core",
        "Clock frequency per core",
        "Average instruction per cycle for cores",
    ]
    y_titles = [
        "Power [W]",
        "Temperature [°C]",
        "Frequency [GHz]",
        "IPC [1/cycle]",
    ]
    all_charts = [power_charts, temp_charts, freq_charts, ipc_charts]

    rows = len(all_charts)
    fig = sub.make_subplots(
        rows=rows,
        cols=1,
        shared_xaxes=True,
        shared_yaxes=False,
        subplot_titles=row_titles,
    )

    for i in range(rows):
        fig.layout[f"yaxis{i+1}"].title.text = y_titles[i]
        charts = all_charts[i]
        for trace in charts:
            trace.line.shape = "hv"
            # trace.legendgroup = i
            fig.add_trace(trace, row=i + 1, col=1)

    height_per_chart = 400
    fig.layout.height = height_per_chart * rows
    fig.layout.xaxis.title = "timing [s]"

    return fig


def launch_viewer(trace_file):
    if not os.path.exists(trace_file):
        print('Error:', os.path.abspath(trace_file),
              'folder not found - cannot launch the viewer')
        return

    try:
        fig = build_view(trace_file)
        fig.show()
    except:  # noqa: E722
        # Fallback if execution is done from simics exe and not from python exe
        import sys
        if sys.platform == "linux":
            sys.path += [".venv/lib/python3.10/site-packages"]
        else:
            sys.path += [".venv/Lib/site-packages"]
        try:
            fig = build_view(trace_file)
            fig.show()
        except Exception as e:  # noqa: E722
            print("Error launching viewer: ", e)
            print("Result file is here: " + trace_file)
