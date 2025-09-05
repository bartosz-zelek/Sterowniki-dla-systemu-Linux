# © 2021 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


from typing import Literal, TypeAlias
from blueprints import (BlueprintFun, ConfObject, State, MemMap, Namespace,
                        Port, SignalPort, Config)
from ..x86_cpu.state import CPUFabric
from simmod.std_bp.state import (
    EthConnectionState, GFXConnectionState, PCIEBus, Queue, SATAConnectionState,
    UARTConnectionState, USBConnectionState)

class RTCDefaultConfig(State):
    rtc_time: str|None = None
    "Default value for RTC config."

QSP_PATH = "%simics%/targets/qsp-x86/images"

class X58BackplaneConfig(Config):
    "X58 settings."
    memory_size_mb = 4096
    rtc_time = '2010-12-03 14:00:00 UTC'
    spi_flash_image = f"{QSP_PATH}/spi-flash.bin"
    mac_address = "00:19:A0:E1:1C:9F"
    bios = f"{QSP_PATH}/SIMICSX58X64_2_0_0_r.fd"
    show_con = True
    show_gfx_con = True
    disk_bp = ""
    disk = BlueprintFun()
    disk_image_path = QSP_PATH

class IDEDisks(State):
    master = ConfObject()
    slave: ConfObject|None = ConfObject()

class ScriptConsole(State):
    con = ConfObject()
    "Console object, which can be used for auto-login scripting."

class ICH10Connectors(State):
    com = [UARTConnectionState() for x in range(4)]
    sata = [SATAConnectionState() for x in range(6)]
    ide = [IDEDisks() for x in range(4)]
    usb0 = USBConnectionState()
    usb1 = USBConnectionState()
    eth0 = EthConnectionState()
    gfx_console = GFXConnectionState()

class X58Connectors(ICH10Connectors):
    pass

class X58Backplane(State):
    connectors = X58Connectors()
    "Device connectors (Ethernet, Serial, ...)"
    fabric = CPUFabric()
    "CPU Fabric."
    queue = Queue()
    "The object to use as queue (the first CPU)"

    # RESET
    CORERST_tgts: list[SignalPort] = []
    "CORERST reset targets"
    PLTRST_tgts: list[SignalPort] = []
    "PLTRST reset targets"
    cpu_INIT = SignalPort()
    "Signal target which sends INIT to all CPUs."
    i8042_reset = SignalPort()
    "Reset target triggered by the i8042 reset functionality."

    # APIC
    io_apic_list: list[ConfObject] = []
    "List with all IOAPICs in the system"

    # Objects
    lpc_obj = ConfObject()
    "The LPC object."
    motherboard = ConfObject()
    "The motherboard object."
    pic_8259 = ConfObject()
    "The PIC 8259 object."
    rtc = ConfObject()
    "The RTC object."

    # Connectors
    pcie_port = [PCIEBus() if not i in (0, 6) else None for i in range(8)]
    "PCIE root ports (1,2,3,4,5 and 7)"
    pci_bus = PCIEBus()
    "PCI bus 0"

    # Fabric
    sb_pci_io = ConfObject()
    "The IO space in the SB."
    shadow_mem_map: list[MemMap] = []
    "List with shadow memory mappings."
    system_io_port_map: list[MemMap] = []
    "IO port mappings for 'system_io'"
    system_mem_map: list[MemMap] = []
    "Mappings for 'system_mem'"
    dram_space = ConfObject()
    "Memory space containing the DRAM"

    # Namespace pointing to the "board"
    system_ns = Namespace("_not_set_")

    disk = BlueprintFun()

class LPCUart(State):
    uart = ConfObject()
    irq_signal = SignalPort()

# The 'x58-backplane' could have been used for ICH10 components, but here
# we provide a stand-alone version of the ICH10 which uses its own backplane.
class ICH10Backplane(State):
    "Type for the 'ich10-backplane' context key."

    connectors = ICH10Connectors()
    pci_bus = PCIEBus()

    # Fields identified with corresponding fields in the X58 backplane
    i8042_reset = SignalPort()
    cpu_INIT = SignalPort()
    motherboard = ConfObject()
    PLTRST_tgts: list[SignalPort] = []

    io_apic_list: list[ConfObject] = []
    pic_8259 = ConfObject()
    rtc = ConfObject()
    lpc_obj = ConfObject()
    pci_io = ConfObject()

    # Fields only used by the ICH10
    cf9_io_map: list[MemMap] = []
    io_port_map: list[MemMap] = []
    lpc_chipset_config_bank = Port()
    spi_flash_bank = Port()
    lan_flash_bank = Port()
    lan_flash_func: int|None = None
    lpc_uart = [LPCUart() for x in range(4)]
    isa_irq_dev = ConfObject()
    isa_dma = ConfObject()
