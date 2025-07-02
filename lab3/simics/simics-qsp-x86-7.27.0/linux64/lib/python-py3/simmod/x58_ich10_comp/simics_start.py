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


import update_checkpoint
import copy
import simics

def address_remapping_update(mb):
    nb = mb.static_slots["nb"]
    sb = mb.static_slots["sb"]
    dram = mb.static_slots["dram_space"]
    mspace = nb.static_slots["pci_mem"]
    nb_pci_bus = nb.static_slots["pci_bus"]
    remap_dispatcher = nb.static_slots["remap_dispatcher"]

    update_checkpoint.remove_attr(remap_dispatcher, "interrupt_space")
    update_checkpoint.remove_attr(remap_dispatcher, "default_int_remapping_unit")
    update_checkpoint.rename_attr(remap_dispatcher,
                                  "default_remapping_unit",
                                  "default_dma_remapping_unit")

    mspace.default_target = [dram, 0, 0, dram]
    for i in range(len(mspace.map)):
        if mspace.map[i][0] == 0xfee00000:
            del mspace.map[i]
            break

    nb_pci_bus.upstream_target = remap_dispatcher
    remap_dispatcher.pci_bus = nb_pci_bus

    sb.static_slots["ext_pci_bus"].upstream_target = sb.static_slots["bridge"]
    sb.static_slots["bridge"].upstream_target = nb_pci_bus

update_checkpoint.install_class_configuration_update(
    3518, "motherboard_x58_ich10", address_remapping_update)

# Remove ich10_lan class in favor of v2 - SIMICS-14097
def remove_old_lan(obj):
    update_checkpoint.remove_attr(obj, "old_lan")

def update_smbus_pci_banks_to_pcie_banks(conf):
    smbus_pci_banks = update_checkpoint.all_objects(conf,
                        'ich10_smbus_i2c_v2.pci_config')
    smbus_pci_banks += update_checkpoint.all_objects(conf,
                        'ich10_smbus.pci_config')
    new_smbus_pci_banks = []
    changed_objs = []
    for bank in smbus_pci_banks:
        conf.pop(bank.name)
        new_name = '.'.join(bank.name.split('.')[:-1]+['pcie_config'])
        dev_name = '.'.join(bank.name.split('.')[:-2])
        new_obj = copy.deepcopy(bank)
        new_obj.__class_name__ = '.'.join(
                            bank.__class_name__.split('.')[:-1]+['pcie_config'])
        new_obj.name = new_name
        unused_regs = ['expansion_rom_base', 'interrupts', 'max_lat', 'min_gnt',
                       'base_address_2', 'base_address_3', 'base_address_5',
                       'cardbus_cis_ptr']
        for reg in unused_regs:
            update_checkpoint.remove_attr(new_obj, reg)
        new_smbus_pci_banks.append(new_obj)
        conf[new_name] = new_obj
        update_checkpoint.remove_attr(conf[dev_name], 'expansion_rom')
        update_checkpoint.remove_attr(conf[dev_name], 'expansion_rom_size')
        update_checkpoint.remove_attr(conf[dev_name], 'pci_bus')
        changed_objs.append(conf[dev_name])
    mbs = update_checkpoint.all_objects(conf, 'motherboard_x58_ich10')
    for mb in mbs:
        if mb.nb.pci_bus.__class_name__ == 'pcie-bus':
            found = [e for e in mb.nb.pci_bus.pci_devices if e[0:2]==[31,3]]
            if len(found) == 1:
                adapter = simics.pre_conf_object(found[0][2].name + '_adapter',
                                                 "legacy-upstream-pcie-adapter")
                conf[found[0][2].name + '_adapter'] = adapter
                new_smbus_pci_banks.append(adapter)
    return (smbus_pci_banks, changed_objs, new_smbus_pci_banks)

def switch_smbus_ctrl_from_pci_to_pcie(mb):
    found = [e for e in mb.nb.pci_bus.pci_devices if e[0:2]==[31,3]]
    if len(found) == 1:
        mb.nb.pci_bus.pci_devices.remove(found[0])
        if mb.nb.pci_bus.__class_name__ == 'pcie-bus':
            a_name = found[0][2].name.split('.')[-1] + '_adapter'
            adapter = getattr(mb.sb, a_name)
            adapter.pci_bus = mb.nb.pci_bus
            adapter.devices = [[0, 0, found[0][2]]]
            adapter.qsp_pcie_bus_mode = [31, 3]
            mb.nb.pci_bus.pci_devices.append([31, 3, adapter])
            mb.nb.pci_conf.map.remove(
                                [0xfb000,found[0][2],255,0,0x1000,None,0,8,0])
            for t_map in [mb.nb.pci_io.map, mb.nb.pci_mem.map]:
                for m in t_map:
                    if m[1] == found[0][2]:
                        t_map.pop(t_map.index(m))
        else:
            mb.nb.pci_bus.devices.append(found[0][:-1])
            mb.nb.pci_bus.cfg_space.map.remove(
                                [0xfb0000,found[0][2],255,0,0x10000,None,0,8,0])
            for t_map in [mb.nb.pci_bus.io_space.map,
                            mb.nb.pci_bus.mem_space.map]:
                for m in t_map:
                    if m[1] == found[0][2].bank.smbus_func:
                        t_map.pop(t_map.index(m))

def handle_simics_5_smbus(smbus):
    if smbus.build_id < 6000:
        attr_list = ['pci_config_expansion_rom_base',
                     'pci_config_interrupts',
                     'pci_config_max_lat',
                     'pci_config_min_gnt',
                     'pci_config_base_address_2',
                     'pci_config_base_address_3',
                     'pci_config_base_address_5',
                     'pci_config_cardbus_cis_ptr',
                     'pci_bus',
                     'expansion_rom_size',
                     'expansion_rom']
        for attr in attr_list:
            update_checkpoint.remove_attr(smbus, attr)
        attr_list = ['pci_config_smbus_sram_bar',
                     'pci_config_bist',
                     'pci_config_revision_id',
                     'pci_config_subsystem_id',
                     'pci_config_subsystem_vendor_id',
                     'pci_config_interrupt_pin',
                     'pci_config_host_configuration',
                     'pci_config_cache_line_size',
                     'pci_config_vendor_id',
                     'pci_config_io_bar',
                     'pci_config_class_code',
                     'pci_config_device_id',
                     'pci_config_command',
                     'pci_config_header_type',
                     'pci_config_latency_timer',
                     'pci_config_interrupt_line',
                     'pci_config_status']
        for attr in attr_list:
            update_checkpoint.rename_attr(smbus, attr.replace('pci_', 'pcie_'),
                                          attr)


update_checkpoint.install_class_configuration_update(
    6033, "motherboard_x58_ich10", remove_old_lan)
update_checkpoint.install_class_configuration_update(
    6033, "southbridge_ich10", remove_old_lan)

# NOTE: The update from legacy PCI to new-style PCIe is only possible
#       because the SMBus device is a trivial PCI device with only
#       simple mem and IO BARs and legacy IRQs.
#       This would likely not be possible for devices using MSI, MSI-X,
#       expansion ROM, DMA, extended capabilities etc.
update_checkpoint.SIM_register_generic_update(
    7059, update_smbus_pci_banks_to_pcie_banks)
update_checkpoint.SIM_register_class_update(
    7059, "motherboard_x58_ich10", switch_smbus_ctrl_from_pci_to_pcie)
update_checkpoint.SIM_register_class_update(
    7059, "ich10_smbus", handle_simics_5_smbus)
