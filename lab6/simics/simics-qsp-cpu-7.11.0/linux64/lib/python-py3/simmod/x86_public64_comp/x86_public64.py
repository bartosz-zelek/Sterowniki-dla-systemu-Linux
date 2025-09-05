# INTEL CONFIDENTIAL

# © 2016 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


from simmod.x86ex_comp.x86ex_processor import *

class processor_x86_public64_base(processor_x86ex):
    """Base class for public Intel® 64 processors."""
    _do_not_init = object()

    def _initialize(self, cpu_threads = 1, cpu_cores = 1):
        super()._initialize(cpu_threads, cpu_cores)
        self.cpuclass = 'x86-public64'

class processor_x86_haswell(processor_x86_public64_base):
    '''Intel® Core™ (formerly Haswell)'''
    _class_desc = 'model of Intel® Core™ (formerly Haswell)'
    _no_new_command = object()

    def _initialize(self, cpu_threads = 1, cpu_cores = 1):
        super()._initialize(cpu_threads, cpu_cores)
        self.cpuclass = 'x86-haswell'

class processor_x86_haswell_xeon(processor_x86_public64_base):
    '''Intel® Xeon® Haswell (formerly Haswell)'''
    _class_desc = 'model of Intel® Xeon® (formerly Haswell)'
    _no_new_command = object()

    def _initialize(self, cpu_threads = 1, cpu_cores = 1):
        super()._initialize(cpu_threads, cpu_cores)
        self.cpuclass = 'x86-haswell-xeon'

    def setup(self):
        super().setup()
        if not self.instantiated.val:
            for c in range(self.cpu_cores):
                for t in range(self.cpu_threads):
                    core = self.get_slot('core')[c][t]
                    core.is_cpuid_topology_fabric_hardcoded = True

    def at_post_instantiate(self):
        def get_leaf_0xb_values(core):
            core_shift = self._get_cpuid(core, 0xb, 0).a & 0x1f
            package_shift = self._get_cpuid(core, 0xb, 1).a & 0x1f

            return (core_shift, package_shift)

        # find out the cluster and apic_id in the > 16-core system
        def cluster1(seq):
            if seq < 5:
                return seq
            if seq < 9:
                return seq - 5 + 0x08
            if seq < 14:
                return seq - 9 + 0x10
            if seq < 18:
                return seq - 14 + 0x18
            return seq

        def cluster2(seq):
            nc2 = self.cpu_cores // 2
            if seq < nc2:
                return seq
            if seq < 2 * nc2:
                return seq - nc2 + 0x08
            return seq


        def calc_apic_id_v1(c, t, core_shift, package_shift, pckg):
            if self.cpu_cores > 16:
                c = cluster1(c)
            elif self.cpu_cores > 8:
                c = cluster2(c)
            return t | (c << core_shift) | (pckg << package_shift)


        super().at_post_instantiate()

        for c in range(self.cpu_cores):
            for t in range(self.cpu_threads):
                core = self.get_slot('core')[c][t]
                apic = self.get_slot('apic')[c][t]
                (core_shift, package_shift) = get_leaf_0xb_values(core)
                core.cpuid_physical_apic_id = calc_apic_id_v1(c, t, core_shift, package_shift, self.package_number.val)
                apic.apic_id = core.cpuid_physical_apic_id

class processor_x86_silvermont(processor_x86_public64_base):
    '''Intel Atom® (formerly Silvermont)'''
    _class_desc = 'model of Intel Atom® (formerly Silvermont)'
    _no_new_command = object()

    def _initialize(self, cpu_threads = 1, cpu_cores = 1):
        super()._initialize(cpu_threads, cpu_cores)
        self.cpuclass = 'x86-silvermont'
        self.ignore_1gb = True

class processor_x86_airmont(processor_x86_public64_base):
    '''Intel Atom® (formerly Airmont)'''
    _class_desc = 'model of Intel Atom® (formerly Airmont)'
    _no_new_command = object()

    def _initialize(self, cpu_threads = 1, cpu_cores = 1):
        super()._initialize(cpu_threads, cpu_cores)
        self.cpuclass = 'x86-airmont'
        self.ignore_1gb = True

class processor_x86_sandybridge(processor_x86_public64_base):
    '''Intel® Core™ (formerly SandyBridge)'''
    _class_desc = 'model of Intel® Core™ (formerly SandyBridge)'
    _no_new_command = object()

    def _initialize(self, cpu_threads = 1, cpu_cores = 1):
        super()._initialize(cpu_threads, cpu_cores)
        self.cpuclass = 'x86-sandybridge'

class processor_x86_broadwell_xeon(processor_x86_public64_base):
    '''Intel® Xeon® (formerly Broadwell)'''
    _class_desc = 'model of Intel® Xeon® (formerly Broadwell)'
    _no_new_command = object()

    def _initialize(self, cpu_threads = 1, cpu_cores = 1):
        super()._initialize(cpu_threads, cpu_cores)
        self.cpuclass = 'x86-broadwell-xeon'

    def at_post_instantiate(self):
        def get_leaf_0xb_values(core):
            core_shift = self._get_cpuid(core, 0xb, 0).a & 0x1f
            package_shift = self._get_cpuid(core, 0xb, 1).a & 0x1f

            return (core_shift, package_shift)

        # find out the cluster and apic_id in the > 16-core system
        def cluster1(seq):
            if seq < 5:
                return seq
            if seq < 9:
                return seq - 5 + 0x08
            if seq < 14:
                return seq - 9 + 0x10
            if seq < 18:
                return seq - 14 + 0x18
            if seq < 24:
                return seq - 20 + 0x20
            print ("Warning: a non-SKU configuration is specified, APIC IDs may"
                   " be assigned with incorrect values.")
            return seq

        def cluster2(seq):
            nc2 = self.cpu_cores // 2
            if seq < nc2:
                return seq
            if seq < 2 * nc2:
                return seq - nc2 + 0x08
            return seq

        def cluster3(seq):
            return (seq//6)*8 + (c%6)

        def calc_apic_id_v1(c, t, core_shift, package_shift, pckg):
            if self.cpu_cores >= 24:
                c = cluster3(c)
            elif self.cpu_cores > 16:
                c = cluster1(c)
            elif self.cpu_cores > 8:
                c = cluster2(c)
            return t | (c << core_shift) | (pckg << package_shift)

        super().at_post_instantiate()

        for c in range(self.cpu_cores):
            for t in range(self.cpu_threads):
                core = self.get_slot('core')[c][t]
                apic = self.get_slot('apic')[c][t]
                (core_shift, package_shift) = get_leaf_0xb_values(core)
                core.cpuid_physical_apic_id = calc_apic_id_v1(c, t, core_shift,
                                        package_shift, self.package_number.val)
                apic.apic_id = core.cpuid_physical_apic_id

class processor_x86_knights_landing(processor_x86_public64_base):
    '''Intel® Xeon Phi™ (formerly Knights Landing)'''
    _class_desc = 'model of Intel® Xeon Phi™ (former Knights Landing)'
    _no_new_command = object()

    def _initialize(self, cpu_threads = 1, cpu_cores = 1):
        super()._initialize(cpu_threads, cpu_cores)
        self.cpuclass = 'x86-knights-landing'

class processor_x86_knights_mill(processor_x86_public64_base):
    '''Intel® Xeon Phi™ (formerly Knights Mill)'''
    _class_desc = 'model of Intel® Xeon Phi™ (formerly Knights Mill)'
    _no_new_command = object()

    def _initialize(self, cpu_threads = 1, cpu_cores = 1):
        super()._initialize(cpu_threads, cpu_cores)
        self.cpuclass = 'x86-knights-mill'

class processor_x86_skylake(processor_x86_public64_base):
    '''Intel® Core™ (formerly Skylake)'''
    _class_desc = 'model of Intel® Core™ (formerly Skylake)'
    _no_new_command = object()

    def _initialize(self, cpu_threads = 1, cpu_cores = 1):
        super()._initialize(cpu_threads, cpu_cores)
        self.cpuclass = 'x86-skylake'
        self.has_crypto_engine = True

class processor_x86QSP2(processor_x86_public64_base):
    '''x86-64 QSP processor, version 2'''
    _class_desc = 'x86-64 QSP, version 2'
    _no_new_command = object()

    def _initialize(self, cpu_threads = 1, cpu_cores = 1):
        super()._initialize(cpu_threads, cpu_cores)
        self.cpuclass = 'x86QSP2'

class processor_x86_goldmont(processor_x86_public64_base):
    '''Intel Atom® (formerly Goldmont)'''
    _class_desc = 'model of Intel Atom® (formerly Goldmont)'
    _no_new_command = object()

    def _initialize(self, cpu_threads = 1, cpu_cores = 1):
        super()._initialize(cpu_threads, cpu_cores)
        self.cpuclass = 'x86-goldmont'
        self.has_crypto_engine = True

class processor_x86_goldmont_plus(processor_x86_public64_base):
    '''Intel Atom® (formerly Goldmont Plus)'''
    _class_desc = 'model of Intel Atom® (formerly Goldmont Plus)'
    _no_new_command = object()

    def _initialize(self, cpu_threads = 1, cpu_cores = 1):
        super()._initialize(cpu_threads, cpu_cores)
        self.cpuclass = 'x86-goldmont-plus'

class processor_x86_cooper_lake(processor_x86_public64_base):
    '''Intel® Xeon® (formerly Cooper Lake)'''
    _class_desc = 'model of Intel® Xeon® (formerly Cooper Lake)'
    _no_new_command = object()

    def _initialize(self, cpu_threads = 1, cpu_cores = 1):
        super()._initialize(cpu_threads, cpu_cores)
        self.cpuclass = 'x86-cooper-lake'

    def setup(self):
        super().setup()
        if not self.instantiated.val:
            for c in range(self.cpu_cores):
                for t in range(self.cpu_threads):
                    core = self.get_slot('core')[c][t]
                    #TODO does it make sense any longer?
                    core.is_cpuid_topology_fabric_hardcoded = True

class processor_x86_coffee_lake(processor_x86_public64_base):
    '''Intel® Core™ (formerly Coffee lake)'''
    _class_desc = 'model of Intel® Core™ (formerly Coffee lake)'
    _no_new_command = object()

    def _initialize(self, cpu_threads = 1, cpu_cores = 1):
        super()._initialize(cpu_threads, cpu_cores)
        self.cpuclass = 'x86-coffee-lake'

def print_topology_warning(n_cores):
    limit = 28
    if n_cores > limit:
        print(("Warning: number of requested cores %d exceeds"
              " supported limit of %d. Make sure that APIC IDs"
              " are unique") % (n_cores, limit))

class processor_x86_coffee_lake_server(processor_x86ex):
    '''Intel® Xeon® (formerly Coffeelake)'''
    _class_desc = 'model of Intel® Xeon® (formerly Coffeelake)'
    _no_new_command = object()

    def _initialize(self, cpu_threads = 1, cpu_cores = 1):
        super()._initialize(cpu_threads, cpu_cores)
        self.cpuclass = 'x86-coffee-lake-server'

    def setup(self):
        super().setup()
        if not self.instantiated.val:
            for c in range(self.cpu_cores):
                for t in range(self.cpu_threads):
                    core = self.get_slot('core')[c][t]
                    core.is_cpuid_topology_fabric_hardcoded = True

    def at_post_instantiate(self):
        def get_leaf_0xb_values(core):
            core_shift = self._get_cpuid(core, 0xb, 0).a & 0x1f
            package_shift = self._get_cpuid(core, 0xb, 1).a & 0x1f

            return (core_shift, package_shift)

        # find out the cluster and apic_id in the > 16-core system
        #EX  28C: 0x0 – 0xD, 0x10 – 0x1D, 0x20 – 0x2D, 0x30 - 0x3D
        #HCC 22C: 0x0 – 0xB, 0x10 – 0x19, 0x20 – 0x2B, 0x30 - 0x39
        def cluster1(seq, ex):
            #ex = 1 means 28 cores
            s = (6, 11, 17, 22)
            if ex:
                s = (7, 14, 21, 28)
            if seq < s[0]:
                return seq
            if seq < s[1]:
                return seq - s[0] + 0x08
            if seq < s[2]:
                return seq - s[1] + 0x10
            if seq < s[3]:
                return seq - s[2] + 0x18
            return seq

        def cluster2(seq):
            nc2 = self.cpu_cores // 2
            if seq < nc2:
                return seq
            if seq < 2 * nc2:
                return seq - nc2 + 0x08
            return seq

        def calc_apic_id_custom(c, t, core_shift, package_shift, pckg):
            print_topology_warning(self.cpu_cores)
            if self.cpu_cores > 22:
                c = cluster1(c, 1)
            elif self.cpu_cores > 16:
                c = cluster1(c, 0)
            elif self.cpu_cores > 8:
                c = cluster2(c)
            return t | (c << core_shift) | (pckg << package_shift)

        super().at_post_instantiate()

        for c in range(self.cpu_cores):
            for t in range(self.cpu_threads):
                core = self.get_slot('core')[c][t]
                apic = self.get_slot('apic')[c][t]
                (core_shift, package_shift) = get_leaf_0xb_values(core)
                core.cpuid_physical_apic_id = calc_apic_id_custom(c, t,
                        core_shift, package_shift, self.package_number.val)
                apic.apic_id = core.cpuid_physical_apic_id

class processor_x86_skylake_server(processor_x86ex):
    '''Intel® Xeon® (formerly Skylake)'''
    _class_desc = 'model of Intel® Xeon® (formerly Skylake)'
    _no_new_command = object()

    def _initialize(self, cpu_threads = 1, cpu_cores = 1):
        super()._initialize(cpu_threads, cpu_cores)
        self.cpuclass = 'x86-skylake-server'

    def setup(self):
        super().setup()
        if not self.instantiated.val:
            for c in range(self.cpu_cores):
                for t in range(self.cpu_threads):
                    core = self.get_slot('core')[c][t]
                    core.is_cpuid_topology_fabric_hardcoded = True

    def at_post_instantiate(self):
        def get_leaf_0xb_values(core):
            core_shift = self._get_cpuid(core, 0xb, 0).a & 0x1f
            package_shift = self._get_cpuid(core, 0xb, 1).a & 0x1f

            return (core_shift, package_shift)

        # find out the cluster and apic_id in the > 16-core system
        #EX  28C: 0x0 – 0xD, 0x10 – 0x1D, 0x20 – 0x2D, 0x30 - 0x3D
        #HCC 22C: 0x0 – 0xB, 0x10 – 0x19, 0x20 – 0x2B, 0x30 - 0x39
        def cluster1(seq, ex):
            #ex = 1 means 28 cores
            s = (6, 11, 17, 22)
            if ex:
                s = (7, 14, 21, 28)
            if seq < s[0]:
                return seq
            if seq < s[1]:
                return seq - s[0] + 0x08
            if seq < s[2]:
                return seq - s[1] + 0x10
            if seq < s[3]:
                return seq - s[2] + 0x18
            return seq

        def cluster2(seq):
            nc2 = self.cpu_cores // 2
            if seq < nc2:
                return seq
            if seq < 2 * nc2:
                return seq - nc2 + 0x08
            return seq

        def calc_apic_id_custom(c, t, core_shift, package_shift, pckg):
            print_topology_warning(self.cpu_cores)
            if self.cpu_cores > 22:
                c = cluster1(c, 1)
            elif self.cpu_cores > 16:
                c = cluster1(c, 0)
            elif self.cpu_cores > 8:
                c = cluster2(c)
            return t | (c << core_shift) | (pckg << package_shift)

        super().at_post_instantiate()

        for c in range(self.cpu_cores):
            for t in range(self.cpu_threads):
                core = self.get_slot('core')[c][t]
                apic = self.get_slot('apic')[c][t]
                (core_shift, package_shift) = get_leaf_0xb_values(core)
                core.cpuid_physical_apic_id = calc_apic_id_custom(c, t,
                        core_shift, package_shift, self.package_number.val)
                apic.apic_id = core.cpuid_physical_apic_id

class processor_x86_experimental_fred(processor_x86_public64_base):
    '''x86-64 core with experimental FRED support'''
    _class_desc = 'x86-64 core with experimental FRED support'
    _no_new_command = object()

    def _initialize(self, cpu_threads = 1, cpu_cores = 1):
        super()._initialize(cpu_threads, cpu_cores)
        self.cpuclass = 'x86-experimental-fred'

class processor_x86_goldencove_base(processor_x86ex_disaggregated):
    _do_not_init = object()

    def _initialize(self, cpu_threads = 1, cpu_cores = 1):
        processor_x86ex._initialize(self, cpu_threads, cpu_cores)
        self.has_aes_engine = True
        self.has_crypto_engine = True

class processor_x86_goldencove_server(processor_x86_goldencove_base):
    '''Intel® Core™ (codenamed Golden Cove)'''
    _class_desc = 'model of Intel® Core™ (codenamed Golden Cove)'
    _no_new_command = object()

    def _initialize(self, cpu_threads = 1, cpu_cores = 1):
        super()._initialize(cpu_threads, cpu_cores)
        self.cpuclass = 'x86-goldencove-server'

    def setup(self):
        super().setup()
        if not self.instantiated.val:
            for c in range(self.cpu_cores):
                for t in range(self.cpu_threads):
                    core = self.get_slot('core')[c][t]
                    core.is_cpuid_topology_fabric_hardcoded = True

    def override_apic_ids(self):
        """Adjust APIC numbering for a disaggregated configuration"""
        if self.cpu_dies > 1:
            def get_leaf_0x1f_values(core):
                core_shift = self._get_cpuid(core, 0x1f, 0).a & 0x1f
                die_shift =  self._get_cpuid(core, 0x1f, 1).a & 0x1f
                package_shift = self._get_cpuid(core, 0x1f, 2).a & 0x1f
                return (core_shift, die_shift, package_shift)

            def find_die_for_core(core_number, core_count_per_die):
                # Not super-efficient for the symmetric case, but
                # divmod doesn't cut it for the asymmetric case
                for die, core_count in enumerate(core_count_per_die):
                    if core_number < core_count:
                        return die, core_number
                    core_number -= core_count
                assert False, "cpu_cores > sum(cores_per_die)?"

            if self.cpu_cores_per_die:
                # In this case we use the list the user supplied
                core_count_per_die = self.cpu_cores_per_die
            else:
                # We scaled up self.cpu_cores with the number of dies
                # earlier, so now we'll have to undo that and make a
                # list with one core count per die.
                cpd = self.cpu_cores // self.cpu_dies
                core_count_per_die = [cpd] * self.cpu_dies

            # Recompute the die and core parts of the APIC IDs.
            for c in range(self.cpu_cores):
                die, core_in_die = find_die_for_core(c, core_count_per_die)
                for t in range(self.cpu_threads):
                    core = self.get_slot('core')[c][t]
                    apic = self.get_slot('apic')[c][t]
                    core_shift, die_shift, pkg_shift = (
                        get_leaf_0x1f_values(core))
                    pkg_mask = ~((1 << pkg_shift) - 1)
                    pkg_id = core.cpuid_physical_apic_id & pkg_mask
                    custom_id = (pkg_id |
                                 (die << die_shift) |
                                 (core_in_die << core_shift) |
                                 t)
                    core.cpuid_physical_apic_id = custom_id
                    apic.apic_id = custom_id

    def at_post_instantiate(self):
        super().at_post_instantiate()
        self.override_apic_ids()

class processor_x86_tnt(processor_x86ex):
    '''Intel Atom® (codename Tremont)'''
    _class_desc = 'model of Intel Atom® (codename Tremont)'
    _no_new_command = object()
    def _initialize(self, cpu_threads = 1, cpu_cores = 1):
        super()._initialize(cpu_threads, cpu_cores)
        self.cpuclass = 'x86-tnt'
        self.has_crypto_engine = True

class processor_x86_snc(processor_x86ex):
    '''Intel® Xeon® processor (codenamed Sunny Cove)'''
    _class_desc = 'model of Intel® Xeon® (codenamed Sunny Cove)'
    _no_new_command = object()

    def _initialize(self, cpu_threads = 1, cpu_cores = 1):
        super()._initialize(cpu_threads, cpu_cores)
        self.cpuclass = 'x86-snc'
        self.has_crypto_engine = True

    def setup(self):
        super().setup()
        if not self.instantiated.val:
            for c in range(self.cpu_cores):
                for t in range(self.cpu_threads):
                    core = self.get_slot('core')[c][t]
                    core.is_cpuid_topology_fabric_hardcoded = True

class processor_x86_snc_smt4(processor_x86_snc):
    '''Intel® Xeon® processor (codenamed Sunny Cove)'''
    _class_desc = 'model of Intel® Xeon® (codenamed Sunny Cove)'
    _no_new_command = object()

    def _initialize(self, cpu_threads = 1, cpu_cores = 1):
        if cpu_cores > 16:
            print(("Warning: number of requested cores %d exceeds"
                  " supported limit of %d. Make sure that APIC IDs"
                  " are unique") % (cpu_cores, 16))
        super()._initialize(cpu_threads, cpu_cores)
        self.cpuclass = 'x86-snc-smt4'

class processor_x86_snc_client(processor_x86ex):
    '''Intel® Core™ (codenamed Sunny Cove)'''
    _class_desc = 'model of Intel® Core™ (codenamed Sunny Cove)'
    _no_new_command = object()

    def _initialize(self, cpu_threads = 1, cpu_cores = 1):
        super()._initialize(cpu_threads, cpu_cores)
        self.cpuclass = 'x86-snc-client'

class processor_x86_wlc(processor_x86ex):
    '''Intel® Core™ (codenamed Willow Cove)'''
    _class_desc = 'model of Intel® Core™ (codenamed Willow Cove)'
    _no_new_command = object()

    def _initialize(self, cpu_threads = 1, cpu_cores = 1):
        super()._initialize(cpu_threads, cpu_cores)
        self.cpuclass = 'x86-wlc'
        self.has_aes_engine = True

class processor_x86ex_cnl(processor_x86ex):
    '''Intel® Core™ codenamed CannonLake'''
    _class_desc = 'model of Intel® Core™ codenamed CannonLake'
    _no_new_command = object()

    def _initialize(self, cpu_threads = 1, cpu_cores = 1):
        super()._initialize(cpu_threads, cpu_cores)
        self.cpuclass = 'x86ex-cnl'
        self.has_crypto_engine = True

class processor_x86_glc_base(processor_x86ex_disaggregated):
    _do_not_init = object()

    def _initialize(self, cpu_threads = 1, cpu_cores = 1):
        super()._initialize(cpu_threads, cpu_cores)
        self.has_aes_engine = True
        self.has_crypto_engine = True

class processor_x86_glc(processor_x86_glc_base):
    '''Intel® Core™ (codenamed Golden Cove)'''
    _class_desc = 'model of Intel® Core™ (codenamed Golden Cove)'
    _no_new_command = object()

    def _initialize(self, cpu_threads = 1, cpu_cores = 1):
        super()._initialize(cpu_threads, cpu_cores)
        self.cpuclass = 'x86-glc'

    def setup(self):
        super().setup()
        if not self.instantiated.val:
            for c in range(self.cpu_cores):
                for t in range(self.cpu_threads):
                    core = self.get_slot('core')[c][t]
                    core.is_cpuid_topology_fabric_hardcoded = True

    def get_num_sockets(self):
        unique_package_ids = set()
        first_lp = self.get_slot('core')[0][0]
        for lp in first_lp.system.cpus:
            unique_package_ids.add(lp.package_id)

        return len(unique_package_ids)

    def override_apic_ids(self):
        """Adjust APIC numbering for a disaggregated configuration"""

        if self.cpu_dies > 1:
            def get_leaf_0x1f_values(core):
                core_shift = self._get_cpuid(core, 0x1f, 0).a & 0x1f
                die_shift =  self._get_cpuid(core, 0x1f, 1).a & 0x1f
                package_shift = self._get_cpuid(core, 0x1f, 2).a & 0x1f
                return (core_shift, die_shift, package_shift)

            def find_die_for_core(core_number, core_count_per_die):
                # Not super-efficient for the symmetric case, but
                # divmod doesn't cut it for the asymmetric case
                for die, core_count in enumerate(core_count_per_die):
                    if core_number < core_count:
                        return die, core_number
                    core_number -= core_count
                assert False, "cpu_cores > sum(cores_per_die)?"

            if self.cpu_cores_per_die:
                # In this case we use the list the user supplied
                core_count_per_die = self.cpu_cores_per_die
            else:
                # We scaled up self.cpu_cores with the number of dies
                # earlier, so now we'll have to undo that and make a
                # list with one core count per die.
                cpd = self.cpu_cores // self.cpu_dies
                core_count_per_die = [cpd] * self.cpu_dies

            # Recompute the die and core parts of the APIC IDs.
            for c in range(self.cpu_cores):
                die, core_in_die = find_die_for_core(c, core_count_per_die)
                for t in range(self.cpu_threads):
                    core = self.get_slot('core')[c][t]
                    apic = self.get_slot('apic')[c][t]
                    core_shift, die_shift, pkg_shift = (
                        get_leaf_0x1f_values(core))
                    pkg_mask = ~((1 << pkg_shift) - 1)
                    pkg_id = core.cpuid_physical_apic_id & pkg_mask
                    custom_id = (pkg_id |
                                 (die << die_shift) |
                                 (core_in_die << core_shift) |
                                 t)
                    core.cpuid_physical_apic_id = custom_id
                    apic.apic_id = custom_id

    def at_post_instantiate(self):
        super().at_post_instantiate()
        self.override_apic_ids()
        num_sockets = self.get_num_sockets()
        for c in range(self.cpu_cores):
            for t in range(self.cpu_threads):
                core = self.get_slot('core')[c][t]
                core.cores_per_socket = self.cpu_cores
                core.num_sockets = num_sockets

class processor_x86_alderlake(processor_x86_heterogeneous):
    '''Intel® Core™ (codenamed Alder Lake)'''
    _class_desc = 'model of Intel® Core™ (codenamed Alder Lake)'

    _no_new_command = object()

    def _initialize(self):
        topology = {
            "_big": [8, 2, 'x86-glc-adl'],
            "_small": [8, 1, 'x86-grt-adl']
        }
        super()._initialize(topology)
        self.has_aes_engine = True

    def calculate_apic_id(self, suffix, thread, core):
        # GLC ADL Hybrid HAS 0.5:
        #   GLC Hybrid: APICID = {PACKAGE_ID[26:16], MODULEID[15:3], LPID[2:0]}
        #     [0, 1, 8, 9, 10, 11, 18, 19, 20, 21, 28, 29, 30, 31, 38, 39]
        #   GRT Hybrid: APICID = {MODULEID[15:3], LPID[1:0],0}
        #     [40, 42, 44, 46, 48, 4A, 4C, 4E]
        if suffix == "_big":
            package = self.package_number.val
            module_id = core
            lp_id = thread
            return (package << 16) | (module_id << 3) | lp_id
        if suffix == "_small":
            module_id = 8 + (core >> 2)
            lp_id = core % 4
            return (module_id << 3) | ((lp_id & 0b11) << 1)

    def override_cpuid(self):
        for suffix, (n_cores, n_threads, _) in self.topology.items():
            for c in range(n_cores):
                for t in range(n_threads):
                    p = self.get_slot('core' + suffix)[c][t]
                    if suffix == "_big":
                        # Each big core gets its own HGS Domain ID
                        # (HSD-ES 2208921556)
                        p.hw_feedback_index = c
                    if suffix == "_small":
                        n_cores_big = 0
                        if "_big" in self.topology:
                            n_cores_big = self.topology["_big"][0]
                        # 4 small (Atom) cores share single HGS Domain ID
                        # (HSD-ES 2208921556)
                        p.hw_feedback_index = n_cores_big + c // 4

    def at_post_instantiate(self):
        super().at_post_instantiate()
        self.override_cpuid()

register_extended_processor_commands(['processor_x86_silvermont',
                                      'processor_x86_airmont',
                                      'processor_x86_sandybridge',
                                      'processor_x86_haswell',
                                      'processor_x86_haswell_xeon',
                                      'processor_x86_broadwell_xeon',
                                      'processor_x86_knights_landing',
                                      'processor_x86_knights_mill',
                                      'processor_x86_skylake',
                                      'processor_x86QSP2',
                                      'processor_x86_goldmont',
                                      'processor_x86_goldmont_plus',
                                      'processor_x86_cooper_lake',
                                      'processor_x86_coffee_lake',
                                      'processor_x86_skylake_server',
                                      'processor_x86_coffee_lake_server',
                                      'processor_x86_experimental_fred',
                                      'processor_x86_goldencove_server',
                                      'processor_x86_tnt',
                                      'processor_x86_snc',
                                      'processor_x86_snc_smt4',
                                      'processor_x86_snc_client',
                                      'processor_x86_wlc',
                                      'processor_x86ex_cnl',
                                      'processor_x86_glc',
                                      'processor_x86_alderlake'])
