from __future__ import annotations

from collections.abc import Callable
from dataclasses import dataclass, field
from enum import Enum
from typing import Any, Optional, Protocol, runtime_checkable


@dataclass
class PTMError:
    """Data class designed to encapsulate error information.

    Errors mayo arise during the execution of Power Thermal Management (PTM)
    functions and are return as instances of this class.

    Args:
        code: an integer that uniquely identifies the error. This
            could be used to categorize errors and facilitate error handling
            strategies. The default value of 0 indicated no error.
        description: an optional string providing a human-readable explanation
            of the error, which can be useful for debugging and logging
            purposes.
    """

    code: int = 0
    description: Optional[str] = None


@dataclass
class PTMValues:
    """Data class that stores key/value pairs representing various
    PTM-related values.

    These values are categorized into separate dictionaries based on their
    data types, which include integers, floats, booleans, and strings.

    Args:
        ints: a dictionary to store integer values with their respective
            string keys. These could represent discrete data such as counts or
            indices relevant to PTM.
        floats: a dictionary to store floating-point numbers with their
            respective string keys. These are useful for representing
             continuous PTM parameters like temperatures or voltages.
        bools: a dictionary to store boolean values with their respective
            string keys. These often represent binary states or conditions
            such as switches or status flags in a PTM context.
        strings: a dictionary to store string values with their respective
            string keys. These could include states, descriptive data or labels
            relevant to PTM operations or configurations.
    """

    ints: dict[str, int] = field(default_factory=dict)
    floats: dict[str, float] = field(default_factory=dict)
    bools: dict[str, bool] = field(default_factory=dict)
    strings: dict[str, str] = field(default_factory=dict)


PTMInputs = PTMValues


@dataclass
class PTMOutputs:
    """Data class that aggregates output values from PTM functions.

    It includes the actual output values and any errors that occurred during
    the function's execution. It serves to communicate the results of PTM
    computations.

    Args:
        values: a PTMValues instance containing the main output values.
        error: an optional PTMError instance providing details about any
            errors encountered during the PTM function's execution.
    """

    values: PTMValues = field(default_factory=PTMValues)
    error: Optional[PTMError] = None


@dataclass
class PTMDebugs:
    """Data class that aggregates debug values from PTM functions.

    It includes the actual debug values and any errors that occurred during the
    function's execution. It serves to communicate the results of PTM
    computations.

    Args:
        valuesu: a PTMValues instance containing additional debug information
            that can help in troubleshooting and fine-tuning the PTM
            algorithms.
        error: an optional PTMError instance providing details about any
            errors encountered during the PTM function's execution.
    """

    values: PTMValues = field(default_factory=PTMValues)
    error: Optional[PTMError] = None


class PTMVariableNature(Enum):
    """Enumeration that defines the possible natures of PTM variables.

    Each member of this enum represents a specific type of data
    that a PTM variable can hold, which helps in categorizing the variables.

    Members:
        ACTIVITY, POWER, CURRENT, STATE, FREQUENCY, VOLTAGE, TEMPERATURE,
        TIME_PS, BANDWIDTH, UNKNOWN.
    """

    ACTIVITY = "activity"
    POWER = "power"
    CURRENT = "current"
    STATE = "state"
    FREQUENCY = "frequency"
    VOLTAGE = "voltage"
    TEMPERATURE = "temperature"
    TIME_PS = "time_ps"
    BANDWIDTH = "bandwidth"
    UNKNOWN = "unknown"


@dataclass
class PTMVariableData:
    """Data class that stores metadata about PTM variables.

    These variables are used either as inputs or outputs in PTM functions.
    Their metadata helps in utilizing the variables for Simics telemetry
    interconnection and probing functionalities.

    Args:
        name: the name of the variable, which uniquely identifies it within
            the PTM system.
        var_type: the Python type of the variable, which indicates the kind
            of data the variable holds.
        nature: a member of the PTMVariableNature enum.
        unit: string representing the variable's unit.
    """

    name: str
    var_type: type
    nature: PTMVariableNature
    unit: str


PTMInitFunction = Callable[[PTMValues], PTMError]
"""Initialize the PTM (Power Thermal Management) algorithms.

Sample definition:
  def initialize(knobs: PTMValues) -> PTMError

Args:
    PTMInputs object encapsulating the configuration values.

Returns:
    PTMError object indicating any issues encountered during the
    initialization.
"""

PTMRunFunction = Callable[[int, PTMInputs], PTMOutputs]
"""Run PTM (Power Thermal Management) algorithms.

Sample definition:
  def run(time_ps: int, inputs: PTMInputs) -> PTMOutputs

Args:
    int time parameter specified in picoseconds.
    PTMInputs object encapsulating the input values required for the PTM
    computation.

Returns:
    PTMOutputs object containing the output values from the PTM computation and
    including any potential error encapsulated within a PTMError instance.

Behavioral expectations:
    Immutability of Input: the input parameter, specifically the input.values
    dictionary, must not be modified during the function's execution.
    Its size and the values it contains should remain consistent pre and post
    invocation.
    Output Handling: all newly computed key/value pairs must be populated in
    the outputs dictionary of the PTMOutputs instance.
    Time Value Handling: any output values that represent time must be added
    to the PTMOutputs dictionary as integer values denominated in picoseconds.
    Stateless: the execution of the PTM algorithms is assumed to be stateless:
    they do not store any value or state from computation to computation.
    The result of invoking PTMRunFunction does not depend on previous calls to
    this or any other function appart PTMInitFunction.
"""

PTMGetDebugValuesFunction = Callable[[int, PTMInputs], PTMDebugs]
"""Get debug values corresponding to a run of the PTM algorithms.

Sample definition:
  def get_debug_values(time_ps: int, inputs: PTMInputs) -> PTMDebugs

Args:
    int time parameter specified in picoseconds.
    PTMInputs object encapsulating the input values required for the PTM
    computation.

Returns:
    PTMDebugs object containing the debug values from the PTM computation and
    including any potential errors encapsulated within a PTMError.

Behavioral expectations:
    The behavioral expectations on the PTMRunFunction also apply to
    PTMRunFunction.
"""

PTMGetInputVariablesList = Callable[[], tuple[PTMVariableData, ...]]
"""Retrieve a list of PTM input variable metadata.

The list is useful for configuring Simics telemetries and probing
functionalities.
Sample definition:
  def get_input_variables_list() -> tuple[PTMVariableData, ...]

Args:
    None

Returns:
    A tuple containing instances of PTMVariableData, each detailing the name,
    type, and nature of an input variable associated with the PTM.
"""

PTMGetOutputVariablesList = Callable[[], tuple[PTMVariableData, ...]]
"""Retrieve a list of PTM debug variable metadata.

The list is useful for configuring Simics telemetries and probing
functionalities.
Sample definition:
  def get_output_variables_list() -> tuple[PTMVariableData, ...]

Args:
    None

Returns:
    A tuple containing instances of PTMVariableData, each detailing the name,
    type, and nature of an output variable associated with the PTM.
"""

PTMGetDebugVariablesList = Callable[[], tuple[PTMVariableData, ...]]
"""Retrieve a list of PTM debug variable metadata.

The list is useful for configuring Simics telemetries and probing
functionalities.
Sample definition:
  def get_debug_variables_list() -> tuple[PTMVariableData, ...]

Args:
    None

Returns:
    A tuple containing instances of PTMVariableData, each detailing the name,
    type, and nature of an input variable associated with the PTM.
"""


@runtime_checkable
class _IsimPtmV1(Protocol):
    """Private protocol used to check ISIM PTM V1 contract compliance.

    Not intended for public usage. Users should import the
    implements_isim_ptm_v1 function to check contract compliance instead.
    """
    def initialize(self, knobs: PTMValues) -> PTMError:
        ...

    def run(self, time_ps: int, inputs: PTMInputs) -> PTMOutputs:
        ...

    def get_debug_values(self, time_ps: int, inputs: PTMInputs) -> PTMOutputs:
        ...

    def get_input_variables_list(self) -> tuple[PTMVariableData, ...]:
        ...

    def get_output_variables_list(self) -> tuple[PTMVariableData, ...]:
        ...

    def get_debug_variables_list(self) -> tuple[PTMVariableData, ...]:
        ...


def implements_isim_ptm_v1(myclass: Any) -> bool:
    """Function that checks ISIM PTM V1 contract compliance for a given class.

    Args:
        None

    Returns:
        A boolean true value if the given class complies with the ISIM PTM V1
        contract and false otherwise.
    """
    return isinstance(myclass, _IsimPtmV1)
