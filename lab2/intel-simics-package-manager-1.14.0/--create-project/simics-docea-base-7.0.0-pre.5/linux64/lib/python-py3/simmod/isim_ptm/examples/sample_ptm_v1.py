from simmod.isim_ptm.isim_ptm_v1 import (PTMDebugs, PTMError, PTMInputs,
                                         PTMOutputs, PTMValues,
                                         PTMVariableData, PTMVariableNature)


class SamplePtmV1:
    LOW_FREQ_MHZ = 2000
    HIGH_FREQ_MHZ = 3000

    def initialize(self, knobs: PTMValues) -> PTMError:
        error = PTMError()
        self.__temp_key = "temperature_1"
        self.__freq_key = "frequency_1"
        self.__temp_threshold_key = "temp_threshold_celsius"

        if self.__temp_threshold_key in knobs.floats.keys():
            self.__temp_threshold = knobs.floats[self.__temp_threshold_key]
        else:
            error.code = 1
            error.description = f"Missing key '{self.__temp_threshold_key}'."

        return error

    def run(self, time_ps: int, inputs: PTMInputs) -> PTMOutputs:
        outputs = PTMOutputs()

        if self.__temp_key not in inputs.floats.keys():
            outputs.error = PTMError()
            outputs.error.code = 2
            outputs.error.description = (
                f"No temperature '{self.__temp_key}' in inputs")
            return outputs

        if inputs.floats[self.__temp_key] > self.__temp_threshold:
            outputs.values.ints[self.__freq_key] = SamplePtmV1.LOW_FREQ_MHZ
        else:
            outputs.values.ints[self.__freq_key] = SamplePtmV1.HIGH_FREQ_MHZ

        return outputs

    def get_debug_values(self, time_ps: int, inputs: PTMInputs) -> PTMOutputs:
        debugs = PTMDebugs()

        if self.__temp_key not in inputs.floats.keys():
            debugs.error = PTMError()
            debugs.error.code = 2
            debugs.error.description = (
                f"No temperature '{self.__temp_key}' in inputs")
            return debugs

        # Add any other values that may help debugging/understanding the
        # PTM computations done in the run function
        debugs.values.floats[self.__temp_threshold_key] = self.__temp_threshold

        return debugs

    def get_input_variables_list(self) -> tuple[PTMVariableData, ...]:
        temperature_1 = PTMVariableData(
            name=self.__temp_key,
            var_type=float,
            nature=PTMVariableNature.TEMPERATURE,
            unit="degree_celsius",
        )
        return (temperature_1, )

    def get_output_variables_list(self) -> tuple[PTMVariableData, ...]:
        frequency_1 = PTMVariableData(
            name=self.__freq_key,
            var_type=int,
            nature=PTMVariableNature.FREQUENCY,
            unit="megahertz",
        )
        return (frequency_1, )

    def get_debug_variables_list(self) -> tuple[PTMVariableData, ...]:
        temp_threshold = PTMVariableData(
            name=self.__temp_threshold_key,
            var_type=float,
            nature=PTMVariableNature.TEMPERATURE,
            unit="degree_celsius",
        )
        return (temp_threshold, )
