# INTEL CONFIDENTIAL

# © 2010 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


import enum
import cli, vmp_common
import pyobj
from comp import (CompException, StandardConnector, StandardConnectorComponent,
                  SimpleConfigAttribute)
from component_utils import class_has_iface
from math import log2, ceil
from simics import (Sim_Connector_Direction_Up, SIM_class_has_attribute,
                    SIM_hap_add_callback_obj, SIM_log_spec_violation)
from simics import Sim_Attr_Pseudo, SIM_log_error

from simmod.x86_comp.x86_motherboard import X86ApicProcessorDownConnector

try:
    from simmod.x86_intel_only import cpu_extensions
    has_intel_only_extensions = True
except ImportError:
    has_intel_only_extensions = False

processed_mappings = set()

class McheckCmaLayout(enum.IntEnum):
    """MCHECK CMA layout, must be kept in sync with src/extensions/mcheck."""
    legacy = 0
    post_sapphire_rapids = 1


class X86ApicProcessorUpConnector(StandardConnector):
    def __init__(self, required=False):
        self.hotpluggable = False
        self.required = required
        self.multi = False
        self.direction = Sim_Connector_Direction_Up
        self.type = 'x86-apic-processor'

    def connect(self, cmp, cnt, attr):
        if isinstance(attr[0], int):
            # old collaborators' package connects to us,
            # use old format: [id /*not used*/, mem_space, port_space,
            # apic_bus, ... /* probably other items not interesting for us */]
            mem_space = attr[1]
            port_space = attr[2]
            apic_bus = attr[3]
        else:
            args_dict = attr[0]
            mem_space = args_dict['phys_mem']
            port_space = args_dict['port_mem']
            apic_bus = args_dict['apic_bus']
            # clock is optional (collaborators' packages can omit it)
            clock = args_dict.get('clock', None)

        for t in cmp.get_all_threads():
            t.port_space = port_space
            t.physical_memory.default_target = [mem_space, 0, 0, None]
            t.shared_physical_memory = mem_space
            t.apic.apic_id = 0     # temporary value
            t.apic.apic_bus = apic_bus
            if clock is None or class_has_iface(t.classname, 'cycle'):
                # If no separate clock is defined, or if the CPU
                # implements cycle, then it needs to be its own clock.
                t.queue = t
            else:
                t.queue = clock

    def get_connect_data(self, cmp, cnt):
        ls = [[[cpu, cpu.apic, True] for cpu in cmp.get_all_threads()]]
        ls[0][0][2] = False # First CPU in component is not logical
        return ls

    def disconnect(self, cmp, cnt):
        raise CompException(f"disconnecting {self.type} connection not allowed")


class X86exApicProcessorUpConnector(X86ApicProcessorUpConnector):
    def attr_to_dict(self, attr):
        if isinstance(attr[0], int):
            # Old Simics motherboard case, new components are not supported
            return dict()
        return attr[0]

    def distribute_tpm_link(self, all_threads, args_dict):
        tpm_link = args_dict.get("tpm_link")
        if tpm_link:
            for t in all_threads:
                t.txt_chipset = tpm_link

    def distribute_x86_monitor(self, all_threads, args_dict):
        x86_monitor = args_dict.get("x86_monitor")
        if x86_monitor:
            listener_info = []
            for t in all_threads:
                t.x86_monitor = x86_monitor
                listener_info.append([t, -1])
            x86_monitor.listener_info += listener_info

    def distribute_accelerator_space(self, all_threads, args_dict):
        accelerator_space = args_dict.get("accelerator_space")
        if accelerator_space:
            for t in all_threads:
                t.accelerator_space = accelerator_space

    def distribute_cstate_controller(self, all_threads, args_dict):
        for t in all_threads:
            t.doma_cstate = None

    def distribute_creg_space(self, all_threads, args_dict):
        creg_space = args_dict.get("creg_space")
        if creg_space:
            for t in all_threads:
                t.uncore = creg_space

    def distribute_mktme_agent(self, all_threads, args_dict):
        mktme_agent = args_dict.get("mktme_agent")
        if mktme_agent:
            for t in all_threads:
                t.mktme_handler = mktme_agent

    def distribute_tdx_mem_image(self, all_threads, args_dict):
        tdx_mem_image = args_dict.get("tdx_mem_image")
        if tdx_mem_image:
            for t in all_threads:
                t.tdx_mem_image = tdx_mem_image

    def override_shared_memory(self, all_threads, args_dict):
        shared_mem_space = args_dict.get("shared_mem_space")
        if shared_mem_space:
            for t in all_threads:
                t.shared_physical_memory = shared_mem_space

    def connect(self, cmp, cnt, attr):
        args_dict = self.attr_to_dict(attr)
        all_threads = cmp.get_all_threads()
        self.distribute_tpm_link(all_threads, args_dict)
        self.distribute_x86_monitor(all_threads, args_dict)
        self.distribute_accelerator_space(all_threads, args_dict)
        self.distribute_cstate_controller(all_threads, args_dict)
        self.distribute_creg_space(all_threads, args_dict)
        self.distribute_mktme_agent(all_threads, args_dict)
        self.distribute_tdx_mem_image(all_threads, args_dict)
        super().connect(cmp, cnt, attr)
        self.override_shared_memory(all_threads, args_dict)

class X86exProcessorDownConnector(X86ApicProcessorDownConnector):
    def __init__(self, id, phys_mem, port_mem, apic_bus, callback=None,
                 required=False, tpm_link=None, shared_mem_space=None,
                 clock=None, x86_monitor=None, accelerator_space=None,
                 creg_space=None, mktme_agent=None, tdx_mem_image=None):
        super().__init__(id, phys_mem, port_mem, apic_bus, callback=callback,
                         required=required, clock=clock)
        self.tpm_link = tpm_link
        self.shared_mem_space = shared_mem_space
        self.x86_monitor = x86_monitor
        self.accelerator_space = accelerator_space
        self.creg_space = creg_space
        self.mktme_agent = mktme_agent
        self.tdx_mem_image = tdx_mem_image

    @classmethod
    def get_obj(cls, cmp, arg):
        if isinstance(arg, str):
            return cmp.get_slot(arg)
        if callable(arg):
            return arg()
        raise TypeError(f"Unsupported input {type(arg)}")

    @classmethod
    def get_optional_obj(cls, cmp, arg):
        if arg is None:
            return None
        return cls.get_obj(cmp, arg)

    def get_slots_dict(self, cmp):
        required_names = "phys_mem", "port_mem", "apic_bus"
        optional_names = ("tpm_link", "shared_mem_space",
            "clock", # used by x86-x56, not by later platforms
            "x86_monitor", "accelerator_space", "creg_space",
            "mktme_agent", "tdx_mem_image")
        slots = {"id":self.id}
        for name in required_names:
            slots[name] = self.get_obj(cmp, self.__getattribute__(name))
        for name in optional_names:
            slots[name] = self.get_optional_obj(cmp, self.__getattribute__(name))
        return slots

    def get_compat_tail(self, cmp):
        # TODO The remaining are compatibility items in the tuple,
        # to work with viper? Are they really needed these days?
        return [self.get_obj(cmp, self.phys_mem),
                self.get_obj(cmp, self.port_mem),
                self.get_obj(cmp, self.apic_bus)]

    def get_connect_data(self, cmp, cnt):
        named_slots = self.get_slots_dict(cmp)
        compat_tail = self.get_compat_tail(cmp)
        return [named_slots] + compat_tail


class processor_x86_common(StandardConnectorComponent):
    """Base class for IA processors."""
    _do_not_init = object()

    def _initialize(self, cpu_threads=1, cpu_cores=1):
        super()._initialize()
        self.apic_type = "X2"
        self.cpu_threads = cpu_threads
        self.cpu_cores = cpu_cores
        self.cores_per_module = 1
        self.cpuclass = 'x86-p4'
        self.has_aes_engine = False
        self.has_crypto_engine = False
        self.x2apic_mode_only = False
        self.ignore_1gb = False
        self.tlbclass = "x86ex-tlb"
        self.mapper_dev_prefix = None

    def _get_cpuid(self, processor_obj, leaf, subleaf):
        return processor_obj.iface.x86_cpuid_query.cpuid_query(leaf, subleaf)

    class basename(StandardConnectorComponent.basename):
        val = "processor"

    class cpi(SimpleConfigAttribute(1, 'i')):
        """Cycles per instruction."""

    class freq_mhz(SimpleConfigAttribute(20, 'i')):
        """Processor frequency in MHz, default is 10 MHz."""
        def setter(self, val):
            if val <= 0:
                raise CompException('Illegal processor frequency %d' % val)
            self.val = val

    class cores(pyobj.Attribute):
        """The number of processor cores."""
        attrattr = Sim_Attr_Pseudo
        attrtype = "i"
        def getter(self):
            return self._up.cpu_cores
        def setter(self, val):
            pass

    class threads(pyobj.Attribute):
        """The number of threads per core."""
        attrattr = Sim_Attr_Pseudo
        attrtype = "i"
        def getter(self):
            return self._up.cpu_threads
        def setter(self, val):
            pass

    class n_cores(SimpleConfigAttribute(-1, 'i|n')):
        """Number of cores"""
        def setter(self, val):
            if val != -1 and (val < 1 or val > 128):
                raise CompException('Illegal number of cores %d (must be'
                                    ' between 1 and 128)' % val)
            self.val = val

    class n_threads(SimpleConfigAttribute(-1, 'i|n')):
        """Number of threads per core"""
        def setter(self, val):
            if val != -1 and (val < 1 or val > 8):
                raise CompException('Illegal number of threads %d (must be'
                                    ' between 1 and 8)' % val)
            self.val = val

    class package_number(SimpleConfigAttribute(0, 'i')):
        """Package number"""

    class apic_freq_mhz(SimpleConfigAttribute(10.0, 'f')):
        """APIC bus frequency in MHz, default is 10 MHz."""
        def setter(self, val):
            if val <= 0:
                raise CompException('Illegal apic frequency %f' % val)
            self.val = val

    class tsc_invariant_freq_mhz(SimpleConfigAttribute(0.0, 'f')):
        """Deprecated, do not use"""

    class powered_up(SimpleConfigAttribute(True, 'b')):
        """Power the processor up at instantiation time or leave it unpowered"""

    class use_vmp(SimpleConfigAttribute(True, 'b')):
        """Enable VMP at setup by setting attribute to True, disable
        VMP at setup by setting attribute to False. The attribute can
        be changed at run-time but the setting will only affect the
        threads in the component at instantiation. This option affects
        simulated time. See the performance chapter in the Simics
        User's Guide for more information about VMP."""

    def get_all_threads(self):
        return [x for y in self.get_slot('core') for x in y]

    def vmp_enable(self, vmp_timing, startup, enable):
        for t in self.get_all_threads():
            if not vmp_common.setup_vmp(t, vmp_timing, startup, enable):
                # failed to activate - but we still need to set timing model
                enable = False

    def set_apic_mode(self, apic):
        if self.apic_type == "X2":
            apic.physical_broadcast_address = 0xff
            apic.bank.apic_regs.Version = 0x14
            apic.apic_type = "X2"
        elif self.apic_type == "P4":
            apic.physical_broadcast_address = 0xff
            apic.bank.apic_regs.Version = 0x14
            apic.apic_type = "P4"
        elif self.apic_type == "P6":
            apic.physical_broadcast_address = 0xf
            apic.bank.apic_regs.Version = 0x18
            apic.apic_type = "P6"
        else:
            raise CompException('Unknown APIC type %s' % self.apic_type)

    def setup_topology(self):
        if self.n_cores.val != -1:
            self.cpu_cores = self.n_cores.val
        if self.n_threads.val != -1:
            self.cpu_threads = self.n_threads.val

    def setup(self):
        super().setup()
        self.setup_topology()
        if not self.instantiated.val:
            self.add_objects()
        self.add_connectors()

    def add_objects(self):
        self.add_objects_core()
        self.add_objects_mem()

        self.add_objects_apic()
        self.add_objects_car()
        self.add_objects_engine_aes()
        self.add_objects_engine_sha256()
        self.add_objects_engine_sha384()
        self.add_objects_msr()
        self.add_objects_ports_proxy()
        self.add_objects_smx()
        self.add_objects_tlb()
        self.add_objects_tsx()

        self.add_objects_intel_only_extensions()

    def add_objects_apic(self):
        idx = '[%d][%d]' % (self.cpu_cores, self.cpu_threads)
        apics = self.add_pre_obj('apic' + idx, 'x2apic_v2')
        for c in range(self.cpu_cores):
            for t in range(self.cpu_threads):
                apic = apics[c][t]
                core = self.get_slot('core')[c][t]
                apic.cpu = core
                apic.queue = core
                apic.bank.apic_regs.queue = core
                apic.frequency = int(self.apic_freq_mhz.val * 1000000)
                apic.post_smi_delay = 200
                apic.p4_x2apic = self.p4_x2apic.val
                apic.interrupt_subscriber = core
                self.set_apic_mode(apic)
                apic.x2apic_mode_only = self.x2apic_mode_only
                core.apic = apic
                self.get_slot('mem')[c][t].map += [
                    [0xfee00000, apic.bank.apic_regs, 0, 0, 0x4000]]

    def add_objects_car(self):
        idx = '[%d][%d]' % (self.cpu_cores, self.cpu_threads)
        car_images = self.add_pre_obj('car_img' + idx, 'image')
        cars = self.add_pre_obj('car' + idx, 'ram')
        for c in range(self.cpu_cores):
            for t in range(self.cpu_threads):
                p = self.get_slot('core')[c][t]
                car_image = car_images[c][t]
                car_image.page_alignment = 0x1000
                car_image.size = 4 * 1024 * 1024 * 1024  # 4GiB
                car = cars[c][t]
                car.image = car_image
                p.car = car

    def add_objects_core(self):
        idx = "[%d][%d]" % (self.cpu_cores, self.cpu_threads)
        cores = self.add_pre_obj('core' + idx, self.cpuclass)
        cpu_idx = 0
        for c in range(self.cpu_cores):
            for t in range(self.cpu_threads):
                p = cores[c][t]
                p.cpu_idx = cpu_idx
                cpu_idx += 1
                p.freq_mhz = self.freq_mhz.val
                p.package_id = self.package_number.val
                if SIM_class_has_attribute(self.cpuclass, "bsp"):
                    p.bsp = 0
                if SIM_class_has_attribute(self.cpuclass, "fabric_bsp"):
                    p.fabric_bsp = 0
                if SIM_class_has_attribute(self.cpuclass, "step_rate"):
                    p.step_rate = [1, self.cpi.val, 0]
                if self.cpu_threads > 1:
                    p.threads = cores[c]
                if not self.powered_up.val:
                    p.power_disabled = True
                if self.ignore_1gb:
                    p.ignore_1gb = True

    def add_objects_engine_aes(self):
        if not self.has_aes_engine:
            return
        engine_aes = self.add_pre_obj('aes_engine', 'crypto_engine_aes')
        for c in range(self.cpu_cores):
            for t in range(self.cpu_threads):
                p = self.get_slot('core')[c][t]
                p.aes_engine = engine_aes

    def add_objects_engine_sha256(self):
        if not self.has_crypto_engine:
            return
        engine_sha256 = self.add_pre_obj('crypto_sha256',
                                         'crypto_engine_sha256')
        for c in range(self.cpu_cores):
            for t in range(self.cpu_threads):
                p = self.get_slot('core')[c][t]
                p.crypto_sha256 = engine_sha256

    def add_objects_engine_sha384(self):
        if not self.has_crypto_engine:
            return
        engine_sha384 = self.add_pre_obj('crypto_sha384',
                                         'crypto_engine_sha384')
        for c in range(self.cpu_cores):
            for t in range(self.cpu_threads):
                p = self.get_slot('core')[c][t]
                p.crypto_sha384 = engine_sha384

    def add_objects_mem(self):
        idx = "[%d][%d]" % (self.cpu_cores, self.cpu_threads)
        mems = self.add_pre_obj('mem' + idx, 'memory-space')
        for c in range(self.cpu_cores):
            for t in range(self.cpu_threads):
                p = self.get_slot('core')[c][t]
                mem = mems[c][t]
                mem.map = []
                p.physical_memory = mem

    def add_objects_msr(self):
        idx = '[%d][%d]' % (self.cpu_cores, self.cpu_threads)
        # msr[c][t] is where MSR handlers implemented in devices are mapped
        msrs = self.add_pre_obj('msr' + idx, 'memory-space')
        for c in range(self.cpu_cores):
            for t in range(self.cpu_threads):
                p = self.get_slot('core')[c][t]
                p.msr_space = msrs[c][t]

    def add_objects_ports_proxy(self):
        idx = '[%d][%d]' % (self.cpu_cores, self.cpu_threads)
        self.add_pre_obj('ports_proxy' + idx, 'memory-space')

    def add_objects_smx(self):
        idx = '[%d][%d]' % (self.cpu_cores, self.cpu_threads)
        acrams = self.add_pre_obj('acram' + idx, 'ram')
        acram_images = self.add_pre_obj('acram_image' + idx, 'image')
        for c in range(self.cpu_cores):
            for t in range(self.cpu_threads):
                acram = acrams[c][t]
                acram_image = acram_images[c][t]
                # Set to large number to be confident it will be enough
                acram_image.size = 0x800000000
                acram.image = acram_image
                self.get_slot('core')[c][t].acram = acram

    def add_objects_tlb(self):
        if not SIM_class_has_attribute(self.cpuclass, "tlb"):
            return
        idx = "[%d][%d]" % (self.cpu_cores, self.cpu_threads)
        tlbs = self.add_pre_obj('tlb' + idx, self.tlbclass)
        for c in range(self.cpu_cores):
            for t in range(self.cpu_threads):
                p = self.get_slot('core')[c][t]
                tlb = tlbs[c][t]
                tlb.cpu = p
                p.tlb = tlb

    def add_objects_tsx(self):
        if not class_has_iface(self.cpuclass, 'x86_tsx'):
            return
        idx = '[%d][%d]' % (self.cpu_cores, self.cpu_threads)
        snares = self.add_pre_obj('snare' + idx, 'tm-snare')
        tx_stores_images = self.add_pre_obj('tx_store_img' + idx, 'image')
        tx_stores = self.add_pre_obj('tx_store' + idx, 'ram')
        for c in range(self.cpu_cores):
            for t in range(self.cpu_threads):
                p = self.get_slot('core')[c][t]
                snare = snares[c][t]
                tx_store_image = tx_stores_images[c][t]
                tx_store = tx_stores[c][t]
                p.snare = snare
                tx_store_image.page_alignment = 0x1000
                # 4GB, big enough to hold a small Tx cache
                tx_store_image.size = 0x100000000
                tx_store.image = tx_store_image
                snare.thread = p
                snare.tx_store = tx_store
                # snare.snoopers and snare.phys_mem are set at post-instantiate

    def add_objects_intel_only_extensions(self):
        if not has_intel_only_extensions:
            return
        aes_engine = None
        if self.has_aes_engine:
            aes_engine = self.get_slot('aes_engine')
        mcheck_cma_layout = McheckCmaLayout.legacy
        if hasattr(self, "mcheck_cma_layout"):
            mcheck_cma_layout = self.mcheck_cma_layout
        crypto_engine = None
        if (
            mcheck_cma_layout == McheckCmaLayout.post_sapphire_rapids
            and self.has_crypto_engine
        ):
            crypto_engine = self.get_slot('crypto_sha256')
        cpu_extensions.add_objects(
            self, aes_engine, crypto_engine, mcheck_cma_layout
        )

    def add_connectors(self):
        self.add_connector('socket', X86exApicProcessorUpConnector())

    class model_stepping_correction(SimpleConfigAttribute(None, 'i|n')):
        """Override value for CPUID.1.EAX"""

    class p4_x2apic(SimpleConfigAttribute(False, 'b')):
        """When set to TRUE it allows write to ia32_apic_base[10] bit without GP with apic_type != \"X2\",
            but value of this bit won't be changed by write operation of the WRMSR instruction."""

    class component(StandardConnectorComponent.component):
        def post_instantiate(self):
            super().post_instantiate()
            if self._up.use_vmp.val:
                self._up.vmp_enable(True, True, True)
            self._up.at_post_instantiate()

    def post_config_snare(self, core):
        if core.snare is None:
            return

        # Get snare to know which memspace it overlays
        snare = core.snare
        snare.physical_memory = core.physical_memory.default_target[0]
        # Find all logical processors at the same broadcast object.
        # Use it to build the snoopers list.
        if core.system is not None:
            # Add all snares but self
            # this list spans across multiple packages
            coherent_list = core.system.cpus
            snare.snoopers = [i.snare for i in coherent_list if (i != core)]
        else:
            snare.snoopers = []

    def get_leaf_0x1f_shifts(self, logical_processor):
        "Return list of shift between APIC ID topology levels of leaf 0x1f"
        def validate_shifts(shifts_list):
            if len(shifts_list) <= 1: # only implicit shift found
                raise CompException("Leaf 0x1f is empty")
            for shift in shifts_list:
                if shift >= 32:
                    raise CompException("Leaf 0x1f shift %d is too big" % shift)
            for i in range(len(shifts_list) - 1):
                sh_i = shifts_list[i]
                sh_next = shifts_list[i+1]
                if sh_next <= sh_i:
                    raise CompException("Leaf 0x1f shifts are non-monotonic")

        res = [0] # SMT level has implicit shift of zero
        level_limit = 32 # one bit per level can be fit into x2APIC ID
        for lvl in range(level_limit):
            lvl_type = (self._get_cpuid(logical_processor,
                                        0x1f, lvl).c >> 8) & 0xff
            if lvl_type == 0: # End of enumeration
                break
            shift = self._get_cpuid(logical_processor, 0x1f, lvl).a & 0x1f
            res.append(shift)
        validate_shifts(res)
        return res

    def get_leaf_0xb_shifts(self, logical_processor):
        smt_shift = 0
        core_shift = self._get_cpuid(logical_processor, 0xb, 0).a & 0x1f
        package_shift = self._get_cpuid(logical_processor, 0xb, 1).a & 0x1f
        return [smt_shift, core_shift, package_shift]

    def apic_id_multi_level(self, coordinates, level_shifts):
        "Return APIC ID for models that use leaf 0x1f for topology"
        if len(coordinates) == 3 and len(level_shifts) > 3:
            # Component code provides classic (t_num, c_num, p_num) coordinates,
            # but CPUID enumerates more than three levels. Use the latest level
            # as fake "core"-level, essentially cramming everything between
            # thread and package into the core-level
            smt_shift = level_shifts[0]
            core_shift = level_shifts[1]
            package_shift = level_shifts[-1]
            surrogate_shifts = [smt_shift, core_shift, package_shift]
            return self.apic_id_two_levels(coordinates, surrogate_shifts)
        if len(level_shifts) != len(coordinates):
            raise CompException("Coordinates do not match topology levels")
        res = 0
        for (shift, val) in zip(level_shifts, coordinates):
            assert (res >> shift) == 0, ("Overflow of coordinate value"
                " res = %d; env = %s" %
                (res, str(list(zip(level_shifts, coordinates)))))
            res |= val << shift
        return res

    def apic_id_two_levels(self, coordinates, level_shifts):
        "Return APIC ID for models that use leaf 0xb to describe topology"
        (t_num, c_num, p_num) = coordinates
        (smt_shift, core_shift, package_shift) = level_shifts
        return ((t_num << smt_shift)
                 + (c_num << core_shift)
                 + (p_num << package_shift))

    def apic_id_legacy(self, coordinates, level_capacities):
        "Return APIC ID for models that do not have leaf 0xb"
        (t_num, c_num, p_num) = coordinates
        (threads_in_core, threads_in_package) = level_capacities
        v = t_num + c_num * threads_in_core + p_num * threads_in_package
        if (v < 8):
            return v
        else: # Skip 8 and 9
            return (v + 2)

    def calculate_apic_id(self, core_num, thread_num):
        "Return APIC ID for logical processor with specified coordinates"

        # Currently component code does not operate with more than two levels
        # of topology, SIMICSTS-445
        logical_processor = self.get_slot('core')[core_num][thread_num]
        max_leaf = self._get_cpuid(logical_processor, 0, 0).a
        coordinates = (thread_num, core_num, self.package_number.val)
        if max_leaf >= 0x1f:
            lvl_shifts = self.get_leaf_0x1f_shifts(logical_processor)
            return self.apic_id_multi_level(coordinates, lvl_shifts)
        elif max_leaf >= 0xb:
            lvl_shifts = self.get_leaf_0xb_shifts(logical_processor)
            return self.apic_id_two_levels(coordinates, lvl_shifts)
        else:
            lvl_cap = (self.cpu_threads, self.cpu_threads * self.cpu_cores)
            return self.apic_id_legacy(coordinates, lvl_cap)

    def override_model_stepping_correction(self, core):
        cpuid_1_eax = self.model_stepping_correction.val
        if cpuid_1_eax is None:
            return
        core.cpuid_stepping_id_override  = cpuid_1_eax        & 0xf
        core.cpuid_model_override        =(cpuid_1_eax >>  4) & 0xf
        core.cpuid_family_id_override    =(cpuid_1_eax >>  8) & 0xf
        core.cpuid_ext_model_id_override =(cpuid_1_eax >> 16) & 0xf
        core.cpuid_ext_family_id_override=(cpuid_1_eax >> 20) & 0xff
        core.edx = cpuid_1_eax

    def post_config_bios_guard(self, core):
        if not hasattr(core, "bios_guard_threads"):
            return
        # Init BIOS Guard with all logical processors. There is one BIOS Guard
        # instance per PCH device (== motherboard)
        if core.system is not None:
            core.bios_guard_threads = core.system.cpus
        else:
            core.bios_guard_threads = [core]

    def post_config_bsp_ap(self, core):
        if core.bsp == 1:
            apic_id = core.cpuid_physical_apic_id
            core.apic.iface.apic_cpu.power_on(1, apic_id)
            core.activity_state = 0
        else:
            core.activity_state = 3

    def at_post_instantiate(self):
        for c in range(self.cpu_cores):
            for t in range(self.cpu_threads):
                core = self.get_slot('core')[c][t]
                apic = self.get_slot('apic')[c][t]
                ports_proxy = self.get_slot('ports_proxy')[c][t]

                self.post_config_snare(core)

                # Place memory space in front of port_space to enable io-breakpoints.
                ports_proxy.default_target = [core.port_space, 0, 0, None]
                core.port_space = ports_proxy

                apic_id = self.calculate_apic_id(c, t)
                core.cpuid_physical_apic_id = apic_id
                apic.apic_id = apic_id

                self.post_config_bsp_ap(core)
                self.override_model_stepping_correction(core)
                self.post_config_bios_guard(core)

                if hasattr(core, "vcr_list"):
                    self.vcr_dict = dict()
                    for (vcr_id, name, _, _) in core.vcr_list:
                        self.vcr_dict[name] = vcr_id
                if hasattr(core, "msr_space") and hasattr(core, "msr_table"):
                    SIM_hap_add_callback_obj("Core_Memory_Space_Map_Changed",
                        core.msr_space, 0, self.on_msr_space_map_change, core)

        if has_intel_only_extensions:
            cpu_extensions.at_post_instantiate(self)

    @staticmethod
    def on_msr_space_map_change(cpu, msr_mem_space):
        non_over_msrs_in_core = {m[0] for m in cpu.msr_table if m[4]}
        for mapping in msr_mem_space.map:
            if str(mapping) in processed_mappings:
                continue
            if mapping[0]//8 in non_over_msrs_in_core:
                base = mapping[0]
                msg = (f"MSR {hex(base//8)} in non-overridable,"
                       f" mapping with base {hex(base)} will be ignored")
                SIM_log_error(msr_mem_space, 0, msg)
            processed_mappings.add(str(mapping))

    class cstate(pyobj.Attribute):
        """C-state"""
        attrattr = Sim_Attr_Pseudo
        attrtype = '[ii]'

        def getter(self):
            core = self._up.get_slot('core')[0][0]
            return core.pkg_cstate

        def setter(self, val):
            core = self._up.get_slot('core')[0][0]
            core.pkg_cstate = val

    class cstate_listeners(pyobj.Attribute):
        """List of all devices to be notified on C-state change"""
        attrattr = Sim_Attr_Pseudo
        attrtype = '[o*]'

        def getter(self):
            core = self._up.get_slot('core')[0][0]
            return core.pkg_cstate_listeners

        def setter(self, val):
            core = self._up.get_slot('core')[0][0]
            core.pkg_cstate_listeners = val


class processor_x86ex(processor_x86_common):
    """Base class for IA processor package."""
    _do_not_init = object()

    def log2_round_up(self, x):
        return int(ceil(log2(x)))

    def set_package_attributes_bsp(self):
        for c in range(self.cpu_cores):
            for t in range(self.cpu_threads):
                p = self.get_slot('core')[c][t]
                if (c, t) == (0, 0):
                    bsp = 1
                else:
                    bsp = 0
                if SIM_class_has_attribute(self.cpuclass, "bsp"):
                    p.bsp = bsp
                if SIM_class_has_attribute(self.cpuclass, "fabric_bsp"):
                    p.fabric_bsp = bsp

    def set_package_attributes(self):
        self.set_package_attributes_bsp()

        cores = self.get_slot('core')
        all_threads = self.get_all_threads()
        self.core_num = max(self.log2_round_up(self.cpu_cores) - 3, 0)
        for c in range(self.cpu_cores):
            for t in range(self.cpu_threads):
                p = cores[c][t]
                p.threads_in_package = all_threads
                p.threads_in_diegrp = all_threads
                p.threads_in_die = all_threads
                p.threads_in_tile = all_threads
                p.package_group = cores[0][0]
                p.module_group = cores[0][0]
                p.core_num = self.core_num

    def add_objects(self):
        super().add_objects()
        self.set_package_attributes()


class processor_x86ex_module(processor_x86_common):
    """Base class for IA processor module."""
    _do_not_init = object()

    class die_number(SimpleConfigAttribute(0, 'i')):
        """Die number"""

    class module_number(SimpleConfigAttribute(0, 'i')):
        """Module number"""

    class die_num(SimpleConfigAttribute(0, 'i')):
        """DIE_NUM field of WHOAMI CR indicating the max number of dies
           supported within a package"""

    class core_num(SimpleConfigAttribute(0, 'i')):
        """CORE_NUM field of WHOAMI CR indicating the max number of cores
           (IDI attach points) supported within a package or a die,
           if there are dies in the package"""

    class lpid_num(SimpleConfigAttribute(0, 'i')):
        """LPID_NUM field of WHOAMI CR indicating the max number of logical
           processors"""

    class smt_in_apic_id(SimpleConfigAttribute(1, 'i')):
        """SMT knob for x2APIC ID calculation"""

    def set_topology_ids(self):
        for c in range(self.cpu_cores):
            for t in range(self.cpu_threads):
                p = self.get_slot('core')[c][t]
                p.die_id = self.die_number.val
                p.module_id = self.module_number.val
                p.die_num = self.die_num.val
                p.core_num = self.core_num.val
                p.lpid_num = self.lpid_num.val
                p.smt_in_apic_id = self.smt_in_apic_id.val

    def set_module_attributes(self):
        all_threads = self.get_all_threads()
        for c in range(self.cpu_cores):
            for t in range(self.cpu_threads):
                p = self.get_slot('core')[c][t]

                # By default includes all logical processors of the module. It
                # must be overwritten in platform if its package/die consists
                # of more than one module.
                p.threads_in_package = all_threads
                p.threads_in_die = all_threads

    def add_objects(self):
        super().add_objects()
        self.set_topology_ids()
        self.set_module_attributes()


class processor_x86ex_disaggregated(processor_x86ex):
    """Extend the base class to allow n_dies"""
    _do_not_init = object()

    class n_dies(SimpleConfigAttribute(-1, 'i|n')):
        """Number of dies per CPU"""
        def setter(self, val):
            if val != -1 and (val < 1 or val > 8):
                raise CompException('Illegal number of dies %d (must'
                                    ' be between 1 and 8)' % val)
            self.val = val

    class n_cores_per_die(SimpleConfigAttribute(None, '[iii]|n')):
        """List of number of cores per die (for asymmetric configurations)"""
        def setter(self, val):
            if val:
                for die, c in enumerate(val):
                    if c < 0:
                        raise CompException(
                            'Core count for die %d: %d must'
                            ' be non-negative' % (die, c))
                self.val = val

    def check_for_n_core_conflicts(self):
        if self.n_cores.val != -1 and self.n_cores_per_die.val:
            raise CompException('Specify either n_core or'
                                ' n_cores_per_die, not both')
        if self.n_cores_per_die.val and self.n_dies.val not in [1, 2, 3]:
            raise CompException('if n_cores_per_die is specified'
                                ' n_dies must be 1, 2, or 3')

    def ignore_unused_dies(self):
        """Clear the core count for unused dies"""
        if self.n_cores_per_die.val:
            for i in range(self.n_dies.val, 3):
                self.n_cores_per_die.val[i] = 0

    def setup_topology(self):
        self.check_for_n_core_conflicts()
        self.ignore_unused_dies()

        super().setup_topology()
        self.cpu_dies = 1
        self.cpu_cores_per_die = None
        if self.n_dies.val != -1:
            self.cpu_dies = self.n_dies.val
        if self.n_cores_per_die.val != None:
            self.cpu_cores_per_die = self.n_cores_per_die.val

        # We have to squeeze in the dies somewhere in current cpu
        # numbering:  ....mb.cpu<x>.core[<y>][<z>] so we scale up
        # position <y> with the number of dies.
        if self.cpu_cores_per_die:
            self.cpu_cores = sum(self.cpu_cores_per_die)
        else:
            self.cpu_cores *= self.cpu_dies

    def setup(self):
        if self.instantiated.val: return

        super().setup()
        cores_per_die = self.cpu_cores / self.cpu_dies
        modules_per_die = max(cores_per_die // self.cores_per_module, 1)
        self.core_num = max(self.log2_round_up(modules_per_die) - 3, 0)
        self.die_num = self.log2_round_up(self.cpu_dies)
        for c in range(self.cpu_cores):
            for t in range(self.cpu_threads):
                core = self.get_slot('core')[c][t]
                core.num_dies = self.cpu_dies
                core.core_num = self.core_num
                core.die_num = self.die_num


class processor_x86ex_disaggregated_modules(processor_x86ex_disaggregated):
    _do_not_init = object()

    def _initialize(self, cpu_threads = 1, cpu_cores = 1):
        super()._initialize(cpu_threads, cpu_cores)
        self.cores_per_module = 4

    def setup(self):
        if self.instantiated.val: return

        super().setup()
    def calculate_apic_id(self, core_num, thread_num):
        "Return APIC ID for logical processor with specified coordinates"

        def find_ids_from_core_number(core_number):
            if self.cpu_cores_per_die:
                # In this case we use the list the user supplied
                core_count_per_die = self.cpu_cores_per_die
            else:
                # We scaled up self.cpu_cores with the number of dies
                # earlier, so now we'll have to undo that and make a
                # list with one core count per die.
                cpd = self.cpu_cores // self.cpu_dies
                core_count_per_die = [cpd] * self.cpu_dies

            # Not super-efficient for the symmetric case, but
            # divmod doesn't cut it for the asymmetric case
            for die, core_count in enumerate(core_count_per_die):
                if core_number < core_count:
                    # Our core number consists of both module_id and lp_id
                    return die, core_number >> 2, core_number & 0x3
                core_number -= core_count
            assert False, "cpu_cores > sum(cores_per_die)?"

        # For a target like GLC we could simply pull the values from
        # CPUID.1F, subleaf 0 (core_shift), 1 (die_shift), and 2
        # (package_shift), but BHS does not have a subleaf for
        # dielets. We got this from Mondal, Sanjoy K:
        #
        # For CMT (e.g. for MTL, GRR/SRF SoCs), WHOAMI format is changed as
        # follows:
        #
        # WHOAMI format
        # [63:61]     CORENUM (should have been names in MODULENUM)
        # [60:58]     DIENUM
        # [57:32]     RSVD
        # [31:31]     BSP
        # [30:20]     PACKAGEID
        # [19:16]     RSVD
        # [15:11]     DIEID
        # [10:3]      MODULEID (aka IDI_AGENT_ID)
        # [2:0]       LPID
        #
        # CoreNum field: (This encoding is used to get the number of
        #                 bits from the ModuleID as follows)
        #   000: upto 8 modules
        #   001: upto 16 modules
        #   010: upto 32 modules
        #   011: upto 64 modules
        #   100: upto 128 modules
        #   101: upto 256 modules
        #   110: upto 512 modules
        #   111: upto 1024 modules
        #
        # so Dynamic ModuleID_MSB = LPID_MSB + (CORENUM + 3)
        #
        # DieNum field: (This encoding is used to get the number of bits from
        #                the ModuleID as follows)
        #   000: up to one compute die
        #   001: 2 dies
        #   010: 4 dies
        #   011: 8 dies
        #   100: 16 dies
        #   101: 32 dies
        #   11x: future expansion
        # So Dynamic DieID_MSB = ModuleID_MSB + DIENUM
        #
        # So APICID = (WHOAMI.PACKAGE_ID << DieID_MSB)
        #           | (WHOAMI.DIE_ID << ModuleID_MSB)
        #           | (WHOAMI.MODULE_ID << 3)
        #           | (WHOAMI.LP_ID << 1)
        #

        die_id, module_id, lp_id = find_ids_from_core_number(core_num)
        package_id = self.package_number.val

        module_id_msb = 3 + (self.core_num + 3)
        die_id_msb = module_id_msb + self.die_num

        return ((package_id << die_id_msb)
                | (die_id << module_id_msb)
                | (module_id << 3)
                | (lp_id << 1))


class processor_x86_heterogeneous(processor_x86ex):
    """Base class for heterogeneous IA processor."""
    _do_not_init = object()

    def _initialize(self, topology):
        super()._initialize(cpu_threads=0, cpu_cores=0)
        self.topology = topology

    class n_cores(SimpleConfigAttribute(-1, "i")):
        def setter(self, val):
            if val != -1:
                raise CompException("Use n_cores_big and n_cores_small"
                                    " attributes instead of n_cores")

    class n_threads(SimpleConfigAttribute(-1, "i")):
        def setter(self, val):
            if val != -1:
                raise CompException("Use n_threads_big and n_threads_small"
                                    " attributes instead of n_threads")

    class n_cores_big(SimpleConfigAttribute(-1, "i")):
        """Number of big cores"""

        def setter(self, val):
            if val != -1 and (val < 0 or val > 128):
                raise CompException("Illegal number of big cores %d"
                                    " (must be between 0 and 128)" % val)
            self.val = val

    class n_cores_small(SimpleConfigAttribute(-1, "i|n")):
        """Number of small cores"""

        def setter(self, val):
            if val != -1 and (val < 0 or val > 128):
                raise CompException("Illegal number of small cores %d"
                                    " (must be between 0 and 128)" % val)
            self.val = val

    class n_threads_big(SimpleConfigAttribute(-1, "i|n")):
        """Number of threads per 1 big core"""

        def setter(self, val):
            if val != -1 and (val < 1 or val > 4):
                raise CompException("Illegal number of threads %d per big"
                                    " core (must be between 1 and 4)" % val)
            self.val = val

    class n_threads_small(SimpleConfigAttribute(-1, "i|n")):
        """Number of threads per 1 small core"""

        def setter(self, val):
            if val != -1 and (val < 1 or val > 2):
                raise CompException("Illegal number of threads %d per small"
                                    " core (must be 1 or 2)" % val)
            self.val = val

    class die_number(SimpleConfigAttribute(0, 'i')):
        """Die identification number"""

    def add_objects(self):
        cg_number = 0
        for suffix, (cpu_cores, cpu_threads, classname) in self.topology.items():
            arrayspec = "[%d][%d]" % (cpu_cores, cpu_threads)
            cores = self.add_pre_obj('core' + suffix  + arrayspec, classname)
            mems = self.add_pre_obj('mem' + suffix + arrayspec, 'memory-space')
            tlbs = self.add_pre_obj('tlb' + suffix + arrayspec, self.tlbclass)
            apics = self.add_pre_obj('apic' + suffix + arrayspec, 'x2apic_v2')
            acrams = self.add_pre_obj('acram' + suffix + arrayspec, 'ram')
            acram_images = self.add_pre_obj('acram_image' + suffix + arrayspec,
                                            'image')

            cars_img    = self.add_pre_obj('car_img' + suffix  + arrayspec, 'image')
            cars        = self.add_pre_obj('car' + suffix  + arrayspec, 'ram')
            self.add_pre_obj('ports_proxy' + suffix  + arrayspec, 'memory-space')
            msr_mem     = self.add_pre_obj('msr' + suffix  + arrayspec, 'memory-space')

            snares        = self.add_pre_obj('snare' + suffix  + arrayspec, 'tm-snare')
            tx_stores_img = self.add_pre_obj('tx_store_img' + suffix  + arrayspec, 'image')
            tx_stores     = self.add_pre_obj('tx_store' + suffix  + arrayspec, 'ram')

            if self.has_aes_engine:
                aes_engine = self.add_pre_obj('aes_engine', 'crypto_engine_aes')
            if self.has_crypto_engine:
                crypto_engine = self.add_pre_obj('crypto_sha256', 'crypto_engine_sha256')

            cpu_idx = 0

            for c in range(cpu_cores):
                # Bind thread-local objects to each other
                for t in range(cpu_threads):
                    core = cores[c][t]
                    mem = mems[c][t]
                    apic = apics[c][t]
                    car_img = cars_img[c][t]
                    car = cars[c][t]
                    if not self.powered_up.val:
                        core.power_disabled = True

                    core.freq_mhz = self.freq_mhz.val # TODO: may be not correct in heterogeneous system
                    core.step_rate = [1, self.cpi.val, 0] # TODO the same as above
                    core.physical_memory = mem

                    tlb = tlbs[c][t]
                    core.tlb = tlb
                    tlb.cpu = core

                    if (c,t) == (0, 0) and cg_number == 0: # BSP resides at the first group
                        bsp = 1
                    else: bsp = 0

                    core.bsp = bsp
                    if SIM_class_has_attribute(classname, "fabric_bsp"):
                        core.fabric_bsp = bsp
                    mem.map = []

                    # Let sibling threads know about each other (MSR sharing)
                    if cpu_threads > 1:
                        core.threads = cores[c]

                    apic.cpu = core
                    apic.queue = core
                    apic.bank.apic_regs.queue = core
                    apic.frequency = int(self.apic_freq_mhz.val * 1000000)
                    apic.post_smi_delay = 200
                    self.set_apic_mode(apic)
                    core.apic = apic
                    mem.map += [[0xfee00000, apic.bank.apic_regs, 0, 0, 0x4000]]

                    acram = acrams[c][t]
                    acram_image = acram_images[c][t]
                    acram_image.size = 0x800000000 #set to large number to be confident it will be enough
                    acram.image = acram_image
                    core.acram = acram

                    car_img.page_alignment = 0x1000
                    car_img.size = 0x100000000 # 4GB
                    car.image = car_img
                    core.car = car

                    core.cpu_idx = cpu_idx
                    core.package_id = self.package_number.val
                    cpu_idx += 1

                    snare                       = snares[c][t]
                    tx_store_img                = tx_stores_img[c][t]
                    tx_store                    = tx_stores[c][t]
                    core.snare                  = snare
                    tx_store_img.page_alignment = 0x1000
                    tx_store_img.size           = 0x100000000 # 4GB, big enough to hold a small Tx cache
                    tx_store.image              = tx_store_img
                    snare.thread                = core
                    snare.tx_store              = tx_store
                    # snare.snoopers will be set up at post-instantiate phase
                    # snare.phys_mem will be set up at post-instantiate phase

                    if self.has_aes_engine:
                        core.aes_engine = aes_engine
                    if self.has_crypto_engine:
                        core.crypto_sha256 = crypto_engine

                    core.msr_space = msr_mem[c][t]

            cg_number += 1

        threads = self.get_all_threads()
        for t in threads:
            t.threads_in_package = threads
            t.threads_in_diegrp = threads
            t.threads_in_die = threads
            t.threads_in_tile = threads
            t.package_group = threads[0]
            t.module_group = threads[0]

    def get_all_threads(self):
        result = []
        for suffix in self.topology:
            result += [x for y in self.get_slot('core' + suffix) for x in y]
        return result


    def setup_topology(self):
        if self.n_cores_big.val != -1:
            self.topology["_big"][0] = self.n_cores_big.val
        if self.n_threads_big.val != -1:
            self.topology["_big"][1] = self.n_threads_big.val
        if self.n_cores_small.val != -1:
            self.topology["_small"][0] = self.n_cores_small.val
        if self.n_threads_small.val != -1:
            self.topology["_small"][1] = self.n_threads_small.val
        for s in list(self.topology):
            if self.topology[s][0] == 0 or self.topology[s][1] == 0:
                del self.topology[s]
        if not self.topology:
            raise CompException("n_cores_big and n_cores_small are both 0")

    def setup(self):
        self.setup_topology()
        StandardConnectorComponent.setup(self)
        # If restoring from a checkpoint, no need to create objects here
        if self.instantiated.val: return
        self.add_objects()
        self.add_connectors()

    # Return APIC_ID value for models that have leaf 0xb
    def calculate_apic_id(self, suffix, thread, core):
        core_obj = self.get_slot('core' + suffix)[core][thread]
        core_shift = self._get_cpuid(core_obj, 0xb, 0).a
        package_shift = self._get_cpuid(core_obj, 0xb, 1).a
        return ((self.package_number.val << package_shift)
            | (core << core_shift) | thread)

    def select_bsp_obj(self):
        for s in ["_big", "_small"]:
            if s in self.topology:
                return self.get_slot('core' + s)[0][0]

    def at_post_instantiate(self):
        def set_bsp(obj):
            bsp_obj = self.select_bsp_obj()
            is_bsp = (obj == bsp_obj)
            obj.bsp = is_bsp
            if SIM_class_has_attribute(obj.classname, "fabric_bsp"):
                obj.fabric_bsp = is_bsp

        for suffix, (cpu_cores, cpu_threads, _) in self.topology.items():
            for c in range(cpu_cores):
                for t in range(cpu_threads):
                    core = self.get_slot('core' + suffix)[c][t]
                    apic = self.get_slot('apic' + suffix)[c][t]
                    ports_proxy = self.get_slot('ports_proxy' + suffix)[c][t]
                    set_bsp(core)
                    core.cpuid_physical_apic_id = self.calculate_apic_id(
                        suffix, t, c)
                    apic.apic_id = core.cpuid_physical_apic_id

                    self.post_config_snare(core)

                    # Place memory space in front of port_space to enable io-breakpoints.
                    ports_proxy.default_target = [core.port_space, 0, 0, None]
                    core.port_space = ports_proxy

                    self.post_config_bsp_ap(core)
                    self.override_model_stepping_correction(core)

    class heterogeneous_suffixes(pyobj.Attribute):
        """List of string suffixes used for subsystems inside the package"""
        attrattr = Sim_Attr_Pseudo
        attrtype = '[s*]'
        def getter(self):
            return list(self._up.topology)
#
# CLI command registration part.
#
def add_msrs_cmd(cpu_pkg, to_add_msrs_list):
    # Iterate through all logical processors and assign add_msr attributes
    if hasattr(cpu_pkg, "heterogeneous_suffixes"):
        suffixes = cpu_pkg.heterogeneous_suffixes
    else:
        suffixes = [''] # regular model
    # prevent multiple MSRs with same number
    thr = cpu_pkg.iface.component.get_slot_value("core" + suffixes[0])[0][0]
    cur_msrs = set([x[0] for x in thr.add_msr])
    new_msrs = set([x[0] for x in to_add_msrs_list])
    if cur_msrs & new_msrs:
        print("Error: remove duplicated MSRs from add_msrs list")
        return
    for suffix in suffixes:
        package = cpu_pkg.iface.component.get_slot_value("core" + suffix)
        for core in package:
            for thread in core:
                # Some people like adding MSRs one by one instead of
                # all at once. To preserve compatibility, current
                # add_msr list is always extended instead of being
                # overwritten. Drawbacks: 1) the list cannot be shrunk;
                # 2) performance problems if many MSRs are being added
                # because of long list manipulations.
                thread.add_msr = thread.add_msr + to_add_msrs_list

def register_extended_processor_commands(classes):
    for class_name in classes:
        cli.new_command("add_msrs", add_msrs_cmd,
            args = [cli.arg(cli.list_t, "add_msr_list")],
            cls = class_name,
            short = "add new or override existing MSRs",
            doc = """
Adds new or overrides existing MSRs by new ones described by tuples of
<arg>add_msr_list</arg>.
Tuple format: [msr_number, scope, reset_value, ro_mask, rsvd_mask,
              ignore_reset, msr_name,flags].
Accepted scope values: 0 - thread, 1 - core, 2 - package.
flags are unused.
Usage example:<br/>
cpu0.add_msrs [[0x17, 0, 0x12345678, 0xfffffffffff, 0, FALSE, "PLATFORM_ID", 0]]

Note that once added, MSRs cannot be deleted.
Handlers registered with x86_msr.register_handlers must be re-registered
to overridden MSRs.
""")
