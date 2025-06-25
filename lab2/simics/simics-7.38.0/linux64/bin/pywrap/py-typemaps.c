/*
  © 2022 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/
enum __anonymous0;
struct __anonymous1;
struct __anonymous2;
enum __anonymous3;
enum __anonymous4;
struct __anonymous5;
struct __anonymous6;
enum __anonymous7;
struct __anonymous8;
enum __anonymous9;
enum __anonymous10;
enum __anonymous11;
struct thread_domain_group;
struct thread_domain;
struct thread_domain_holder;
enum __anonymous12;
struct __anonymous13;
typedef struct __anonymous13 host_hypervisor_info_t;
struct __anonymous14;
struct __anonymous15;
typedef struct __anonymous15 deferred_transaction_info_t;
struct __anonymous16;
typedef struct __anonymous16 terminus_cb_data_t;
struct __anonymous17;
typedef struct __anonymous17 probe_result_t;
enum __anonymous18;
struct _object;
typedef struct _object PyObject;
typedef PyObject SIM_PYOBJECT;
typedef SIM_PYOBJECT lang_void;
typedef unsigned long int size_t;
struct profile_area;
struct execute_environ;
enum __anonymous19;
enum __anonymous20;
enum __anonymous21;
enum breakpoint_flag;
enum __anonymous22;
struct wait_handler;
struct conf_object;
typedef struct conf_object conf_object_t;
enum sim_exception;
struct domain_lock;
enum __anonymous23;
typedef enum __anonymous23 notifier_type_t;
struct notifier_handle;
enum __anonymous24;
struct __anonymous25;
struct transaction_trace_atom_access;
enum __anonymous26;
typedef enum __anonymous26 exception_type_t;
struct transaction;
typedef struct transaction transaction_t;
struct __anonymous27;
enum __anonymous28;
struct __anonymous29;
typedef struct __anonymous29 breakpoint_set_t;
typedef unsigned long long uint64;
struct map_target;
typedef struct map_target map_target_t;
enum __anonymous30;
typedef enum __anonymous30 access_t;
enum __anonymous31;
typedef enum __anonymous31 translation_flags_t;
enum __anonymous32;
enum __anonymous33;
struct global_notifier_callback;
enum __anonymous34;
enum __anonymous35;
struct attr_value;
typedef struct attr_value attr_value_t;
enum __anonymous36;
struct __anonymous37;
enum __anonymous38;
typedef enum __anonymous38 set_error_t;
struct conf_class;
typedef struct conf_class conf_class_t;
enum __anonymous39;
struct class_info;
enum __anonymous40;
struct class_data;
struct completion;
struct xtensa_wwdt_faultinfo_interface;
struct xtensa_wwdt_config_interface;
struct xtensa_output_queue_interface;
struct xtensa_mpu_lookup;
typedef struct xtensa_mpu_lookup xtensa_mpu_lookup_t;
struct xtensa_mpu_lookup_interface;
struct xtensa_lookup_interface;
struct xtensa_internal_memories_interface;
struct xtensa_input_queue_interface;
struct xtensa_import_wire_interface;
struct xtensa_export_state_interface;
enum __anonymous41;
struct xed_access_interface;
struct x86_vmp_control_interface;
struct __anonymous42;
struct __anonymous43;
typedef struct __anonymous43 x86_tlb_attrs_v3_t;
struct x86_tlb_v3_interface;
struct __anonymous44;
struct __anonymous45;
typedef struct __anonymous45 x86_tlb_attrs_t;
struct x86_tlb_v2_interface;
struct __anonymous46;
typedef struct __anonymous46 tagged_physical_address_t;
struct x86_tlb_interface;
typedef unsigned short uint16;
typedef unsigned int uint32;
struct __anonymous47;
struct x86_smm_state_interface;
struct x86_smm_interface;
struct __anonymous48;
typedef struct __anonymous48 ymm_reg_t;
struct __anonymous49;
typedef struct __anonymous49 x86_xmode_info_t;
struct __anonymous50;
typedef struct __anonymous50 xmm_reg_t;
enum __anonymous51;
struct __anonymous52;
typedef struct __anonymous52 x86_system_seg_reg_t;
enum __anonymous53;
struct __anonymous54;
typedef struct __anonymous54 x86_seg_reg_t;
struct __anonymous55;
typedef struct __anonymous55 x86_pending_debug_exc_t;
struct __anonymous56;
typedef struct __anonymous56 x86_mwait_info_t;
struct __anonymous57;
typedef struct __anonymous57 x86_monitor_info_t;
enum __anonymous58;
struct __anonymous59;
typedef struct __anonymous59 x86_fpu_reg_t;
struct __anonymous60;
typedef struct __anonymous60 x86_fpu_env_t;
enum __anonymous61;
enum __anonymous62;
enum __anonymous63;
enum __anonymous64;
struct x86_reg_access_interface;
struct x86_pkg_cstate_interface;
enum __anonymous65;
typedef enum __anonymous65 x86_msr_ret_t;
typedef long long int64;
enum __anonymous66;
typedef enum __anonymous66 x86_msr_access_type_t;
struct __anonymous67;
typedef struct __anonymous67 x86_msr_getter_ret_t;
struct x86_msr_interface;
enum __anonymous68;
struct x86_memory_query_interface;
struct x86_memory_access_interface;
struct x86_instrumentation_subscribe_v2_interface;
enum __anonymous69;
typedef enum __anonymous69 x86_detailed_exec_mode_t;
struct x86_instrumentation_subscribe_interface;
struct x86_instruction_query_interface;
enum __anonymous70;
struct x86_exception_query_interface;
struct x86_exception_interface;
enum __anonymous71;
struct x86_ept_interface;
struct x86_cstate_notification_interface;
struct __anonymous72;
typedef struct __anonymous72 x86_cstate_t;
struct x86_cstate_interface;
struct __anonymous73;
typedef struct __anonymous73 cpuid_value_t;
struct x86_cpuid_query_interface;
struct __anonymous74;
typedef struct __anonymous74 cpuid_ret_t;
struct x86_cpuid_interface;
struct x86_cache_flush_interface;
enum __anonymous75;
struct x86_address_query_interface;
enum x86_access_type;
struct x86_access_type_interface;
enum __anonymous76;
struct __anonymous77;
typedef struct __anonymous77 tagged_linear_address_t;
enum __anonymous78;
struct x86_interface;
struct winsome_console_interface;
struct vnc_server_v2_interface;
enum __anonymous79;
typedef enum __anonymous79 vmx_mode_t;
typedef uint64 physical_address_t;
struct vmx_instrumentation_subscribe_interface;
enum __anonymous80;
enum __anonymous81;
struct vmp_interface;
struct cbdata;
typedef struct cbdata cbdata_t;
typedef cbdata_t cbdata_call_t;
struct __anonymous82;
typedef struct __anonymous82 bytes_t;
struct virtual_instruction_breakpoint_interface;
typedef uint64 generic_address_t;
struct virtual_data_breakpoint_interface;
struct vga_update_interface;
struct vga_text_update_interface;
struct vga_text_interface;
struct vectored_interrupt_source_interface;
struct vectored_interrupt_interface;
enum __anonymous83;
struct usb_device_interface;
enum __anonymous84;
enum __anonymous85;
enum __anonymous86;
enum __anonymous87;
struct __anonymous88;
struct usb_interface;
struct uint64_state_interface;
struct translator_interface;
struct translation_flush_interface;
struct translate_interface;
struct translation;
typedef struct translation translation_t;
struct transaction_translator_interface;
struct __anonymous89;
typedef struct __anonymous89 next_map_t;
struct map_info;
typedef struct map_info map_info_t;
typedef next_map_t (*do_map_target_cb_t)(const map_target_t *, transaction_t *, uint64, map_info_t *, lang_void *);
struct transaction_callback_entry;
struct transaction_subscribe_interface;
struct transaction_interface;
struct timing_model_interface;
struct text_console_frontend_interface;
enum __anonymous90;
enum __anonymous91;
struct text_console_backend_interface;
struct terminal_server_interface;
struct terminal_client_interface;
struct temporal_state_interface;
struct telnet_connection_v2_interface;
struct telemetry_manager_interface;
struct telemetry_interface;
struct tcp_connection_info_interface;
struct tcf_channel_interface;
struct table_interface;
struct synchronous_mode_interface;
struct sync_initial_time_interface;
struct __anonymous92;
struct __anonymous93;
typedef struct __anonymous93 symbol_t;
enum __anonymous94;
struct symbols_interface;
struct step_info_interface;
struct step_instrumentation;
typedef int64 simtime_t;
typedef simtime_t pc_step_t;
struct step_event_instrumentation_interface;
struct __anonymous95;
typedef struct __anonymous95 step_cycle_ratio_t;
struct step_cycle_ratio_interface;
struct step_interface;
struct page;
struct stc_interface;
struct stall_interface;
enum __anonymous96;
typedef enum __anonymous96 gen_spr_ret_t;
enum __anonymous97;
typedef enum __anonymous97 gen_spr_access_type_t;
struct spr_interface;
struct sparc_v8_ecc_fault_injection_interface;
struct sparc_v8_interface;
struct snoop_memory_interface;
struct smm_instrumentation_subscribe_interface;
struct slaver_message_interface;
struct slave_agent_interface;
struct simulator_cache_interface;
struct simple_interrupt_interface;
struct simple_dispatcher_interface;
struct signal_interface;
struct sh_interrupt_interface;
enum serial_peripheral_interface_flags;
struct serial_peripheral_interface_slave_interface;
struct serial_peripheral_interface_master_interface;
struct serial_device_interface;
struct serial_console_frontend_interface;
struct screenshot_interface;
struct scale_factor_listener_interface;
struct scalar_time_interface;
struct sata_interface;
struct rs232_device_interface;
enum __anonymous98;
enum __anonymous99;
struct rs232_console_interface;
struct riscv_signal_sgeip_interface;
struct riscv_instruction_action_interface;
enum __anonymous100;
struct riscv_imsic_interface;
enum __anonymous101;
typedef enum __anonymous101 riscv_csr_access_type_t;
struct riscv_custom_csr_interface;
struct riscv_coprocessor_interface;
enum __anonymous102;
enum __anonymous103;
struct riscv_clic_interrupt_interface;
struct riscv_clic_interface;
struct register_view_read_only_interface;
struct register_view_catalog_interface;
struct register_view_interface;
struct register_breakpoint_interface;
struct recorder_v2_interface;
struct recorder;
struct dbuffer;
typedef struct dbuffer dbuffer_t;
struct recorder_interface;
struct recorded_interface;
struct ram_callback_entry;
struct ram_access_subscribe_interface;
enum __anonymous104;
struct ram_interface;
struct pulse_interface;
struct processor_info_v2_interface;
struct processor_info_interface;
struct processor_gui_interface;
enum __anonymous105;
struct processor_endian_interface;
struct __anonymous106;
typedef struct __anonymous106 physical_block_t;
struct processor_cli_interface;
struct probe_subscribe_interface;
struct probe_sampler_cache_interface;
struct probe_index_interface;
struct probe_array_interface;
struct probe_interface;
struct preference_interface;
enum __anonymous107;
enum __anonymous108;
struct ppc_interface;
struct port_space_interface;
struct __anonymous109;
typedef struct __anonymous109 ip_port_t;
struct port_forward_interface;
struct pool_protect_interface;
struct pmr_interface;
struct pcie_port_control_interface;
enum __anonymous110;
struct pcie_map_interface;
struct pcie_link_training_interface;
enum __anonymous111;
enum __anonymous112;
struct pcie_hotplug_events_interface;
struct pcie_device_interface;
struct pcie_adapter_compat_interface;
struct pci_upstream_operation_interface;
struct pci_upstream_interface;
struct pci_multi_function_device_interface;
struct pci_interrupt_interface;
struct pci_express_hotplug_interface;
enum __anonymous113;
struct pci_express_interface;
struct pci_downstream_interface;
struct pci_device_interface;
struct pci_bus_interface;
struct pci_bridge_interface;
struct page_bank_client_interface;
struct pb_page;
typedef struct pb_page pb_page_t;
struct __anonymous114;
typedef struct __anonymous114 page_bank_grant_t;
enum __anonymous115;
struct page_bank_interface;
struct packet_interface;
struct osa_tracker_state_query_interface;
struct osa_tracker_state_notification_interface;
struct osa_tracker_state_admin_interface;
struct osa_tracker_control_interface;
struct osa_tracker_component_interface;
struct osa_parameters_interface;
struct osa_node_tree_query_interface;
typedef uint64 node_id_t;
struct osa_node_tree_notification_interface;
struct osa_node_tree_admin_interface;
struct osa_node_path_interface;
struct osa_micro_checkpoint_interface;
struct osa_mapper_query_interface;
struct osa_mapper_control_interface;
struct osa_mapper_admin_interface;
enum __anonymous116;
struct osa_machine_query_interface;
enum __anonymous117;
typedef enum __anonymous117 processor_mode_t;
struct osa_machine_notification_interface;
struct osa_control_v2_interface;
struct osa_control_interface;
struct maybe_node_id;
typedef struct maybe_node_id maybe_node_id_t;
struct osa_component_interface;
struct __anonymous118;
typedef struct __anonymous118 opcode_length_info_t;
struct opcode_info_interface;
struct nios_eic_interface;
struct nios_custom_interface;
struct nios_cache_interface;
struct nios_interface;
struct network_breakpoint_interface;
struct nand_flash_interface;
struct multi_level_signal_interface;
struct mouse_interface;
struct mmc_interface;
struct mips_mt_ase_policy_interface;
struct mips_mt_ase_interface;
struct mips_ite_interface;
struct mips_exception_query_interface;
struct mips_eic_interface;
struct mips_coprocessor_interface;
struct mips_cache_instruction_interface;
struct mips_interface;
struct mii_management_interface;
struct mii_interface;
struct microwire_interface;
enum __anonymous119;
struct map_list;
struct memory_space_interface;
struct memory_profiler_interface;
struct mdio45_phy_interface;
struct mdio45_bus_interface;
struct map_demap_interface;
struct log_object_interface;
struct link_endpoint_v2_interface;
struct __anonymous120;
typedef struct __anonymous120 slave_time_t;
struct link_endpoint_interface;
enum __anonymous121;
struct linear_image_interface;
struct leader_message_interface;
struct keyboard_console_interface;
struct keyboard_interface;
struct jit_control_interface;
enum __anonymous122;
struct io_memory_interface;
enum __anonymous123;
struct interrupt_subscriber_interface;
struct interrupt_cpu_interface;
struct interrupt_ack_interface;
struct internal_class_testing_interface;
enum __anonymous124;
struct int_register_interface;
struct instrumentation_tool_interface;
struct instrumentation_order_interface;
struct instrumentation_filter_status_interface;
struct instrumentation_filter_slave_interface;
struct instrumentation_filter_master_interface;
struct instrumentation_connection_interface;
struct instrum_registration_interface;
enum __anonymous125;
struct instruction_fetch_interface;
struct image_interface;
struct ieee_802_3_phy_v3_interface;
struct ieee_802_3_phy_v2_interface;
struct ieee_802_3_phy_interface;
struct ieee_802_3_mac_v3_interface;
enum __anonymous126;
struct ieee_802_3_mac_interface;
struct icode_interface;
struct i8051_timer_interface;
struct i8051_interrupt_interface;
struct i3c_slave_interface;
struct i3c_master_interface;
struct i3c_hdr_slave_interface;
enum __anonymous127;
struct i3c_hdr_master_interface;
struct i3c_daa_snoop_interface;
struct i2c_slave_v2_interface;
struct i2c_slave_interface;
enum __anonymous128;
struct i2c_master_v2_interface;
struct i2c_master_interface;
enum __anonymous129;
struct i2c_link_interface;
enum i2c_device_state;
struct i2c_device_interface;
enum __anonymous130;
struct i2c_bus_interface;
struct i2c_bridge_interface;
struct host_serial_interface;
struct gui_console_backend_interface;
struct global_time_interface;
enum __anonymous131;
struct gfx_console_frontend_interface;
enum __anonymous132;
struct gfx_console_backend_interface;
enum __anonymous133;
struct gfx_con_interface;
struct gfx_break_interface;
struct gbic_transceiver_interface;
struct frequency_listener_interface;
struct frequency_interface;
struct freerun_interface;
typedef unsigned char uint8;
struct forward_rewrite_endpoint;
typedef struct forward_rewrite_endpoint forward_rewrite_endpoint_t;
struct forward_destination_rewriter_interface;
struct follower_agent_interface;
struct fmn_station_control_v2_interface;
struct fmn_station_control_interface;
struct firewire_device_interface;
enum __anonymous134;
struct firewire_bus_interface;
struct external_connection_events_interface;
enum __anonymous135;
enum __anonymous136;
struct external_connection_ctl_interface;
struct extended_serial_interface;
struct execute_control_interface;
struct execute_interface;
typedef uint64 linear_address_t;
typedef uint64 logical_address_t;
struct __anonymous137;
typedef struct __anonymous137 byte_string_t;
struct exec_trace_interface;
struct exception_interface;
struct event_handler_interface;
struct event_delta_interface;
struct ethernet_vlan_snoop_interface;
struct frags;
typedef struct frags frags_t;
enum __anonymous138;
typedef enum __anonymous138 eth_frame_crc_status_t;
struct ethernet_snoop_interface;
enum __anonymous139;
typedef enum __anonymous139 eth_probe_side_t;
struct ethernet_probe_interface;
struct ethernet_common_interface;
struct ethernet_cable_interface;
struct dummy_memory_page_interface;
struct dist_control_node_interface;
enum __anonymous140;
struct dist_control_interface;
struct disk_component_interface;
struct direct_memory_update_interface;
struct __anonymous141;
typedef struct __anonymous141 direct_memory_tags_t;
struct direct_memory_tags_interface;
struct direct_memory_lookup_v2_interface;
struct __anonymous142;
typedef struct __anonymous142 direct_memory_lookup_t;
struct direct_memory_lookup_interface;
struct direct_memory_flush_interface;
struct __anonymous143;
typedef struct __anonymous143 direct_memory_t;
struct granted_mem;
struct direct_memory_interface;
struct device_identification_interface;
struct debug_symbol_file_interface;
struct debug_symbol_interface;
struct debug_step_interface;
struct debug_setup_interface;
struct debug_query_interface;
struct debug_notification_interface;
struct datagram_link_interface;
struct data_profiler_interface;
struct clock_instrumentation;
typedef simtime_t cycles_t;
struct cycle_event_instrumentation_interface;
struct cycle_event_interface;
struct cycle_control_interface;
struct __anonymous144;
typedef struct __anonymous144 duration_t;
struct __anonymous145;
typedef struct __anonymous145 local_time_t;
struct event_class;
struct cycle_interface;
struct cxl_non_device_decoder_handling_interface;
struct cxl_mem_downstream_port_managing_interface;
enum __anonymous146;
struct cxl_map_interface;
enum __anonymous147;
enum __anonymous148;
struct cpu_memory_query_interface;
enum __anonymous149;
struct __anonymous150;
typedef struct __anonymous150 tuple_int_string_t;
struct cpu_bytes;
typedef struct cpu_bytes cpu_bytes_t;
struct cpu_replace_info;
typedef struct cpu_replace_info decoder_handle_t;
struct cpu_instrumentation_info;
typedef struct cpu_instrumentation_info instruction_handle_t;
struct cpu_exception_return_info;
typedef struct cpu_exception_return_info exception_return_handle_t;
struct cpu_exception_info;
typedef struct cpu_exception_info exception_handle_t;
struct cached_instruction_data;
typedef struct cached_instruction_data cached_instruction_handle_t;
struct cpu_address_info;
typedef struct cpu_address_info address_handle_t;
struct instrumentation_entry;
struct cpu_instrumentation_subscribe_interface;
struct cpu_instruction_query_interface;
enum __anonymous151;
typedef enum __anonymous151 cpu_emulation_t;
struct cpu_instruction_decoder_interface;
struct cpu_exception_query_interface;
struct cpu_cached_instruction_once_interface;
struct cpu_memory_info;
typedef struct cpu_memory_info memory_handle_t;
struct cpu_cached_instruction_interface;
struct __anonymous152;
typedef struct __anonymous152 coreint_reply_t;
struct coreint_interface;
struct context_handler_interface;
enum __anonymous153;
struct connector_interface;
enum __anonymous154;
struct concurrency_mode_interface;
struct concurrency_group_interface;
enum __anonymous155;
struct con_input_code_interface;
struct con_input_interface;
struct component_connector_interface;
struct component_interface;
struct co_execute_interface;
struct cmd_line_selection_interface;
struct cmd_line_frontend_interface;
struct __anonymous156;
struct __anonymous157;
typedef struct __anonymous157 disassembly_mode_info_t;
struct __anonymous158;
struct __anonymous159;
typedef struct __anonymous159 disassembly_instruction_t;
struct class_disassembly_interface;
enum __anonymous160;
struct checkpoint_interface;
struct cell_inspection_interface;
struct cdrom_media_interface;
struct callback_info_interface;
enum __anonymous161;
struct cache_control_interface;
enum swap_mode;
struct bridge_interface;
struct breakpoint_type_provider_interface;
struct breakpoint_type_interface;
struct breakpoint_registration_interface;
struct breakpoint_query_v2_interface;
struct breakpoint_query_interface;
struct breakpoint_manager_interface;
struct breakpoint_change_interface;
struct __anonymous162;
typedef struct __anonymous162 breakpoint_info_t;
struct breakpoint_interface;
struct break_strings_v2_interface;
struct break_strings_interface;
struct branch_recorder_handler_interface;
enum __anonymous163;
struct __anonymous164;
struct branch_arc_iter;
enum __anonymous165;
struct branch_arc_interface;
struct bp_manager_interface;
struct bank_before_write_interface;
typedef struct bank_before_write_interface bank_before_write_interface_t;
struct bank_access;
typedef struct bank_access bank_access_t;
struct bank_before_read_interface;
typedef struct bank_before_read_interface bank_before_read_interface_t;
struct bank_after_write_interface;
typedef struct bank_after_write_interface bank_after_write_interface_t;
struct bank_after_read_interface;
typedef struct bank_after_read_interface bank_after_read_interface_t;
struct bank_instrumentation_subscribe_interface;
enum __anonymous166;
struct arm_trustzone_interface;
struct arm_gic_cpu_state_interface;
struct gic_reg_info;
typedef struct gic_reg_info gic_reg_info_t;
struct arm_cpu_state;
typedef struct arm_cpu_state arm_cpu_state_t;
struct arm_gic_interface;
struct arm_external_debug_interface;
enum __anonymous167;
struct arm_cpu_group_tlb_interface;
struct arm_cpu_group_exclusive_interface;
struct arm_cpu_group_event_interface;
struct arm_coprocessor_interface;
struct arm_avic;
typedef struct arm_avic arm_avic_t;
struct arm_avic_interface;
struct arm_interface;
struct arinc429_receiver_interface;
struct arinc429_bus_interface;
enum __anonymous168;
struct apic_cpu_interface;
enum __anonymous169;
enum __anonymous170;
enum __anonymous171;
enum __anonymous172;
struct apic_bus_interface;
enum __anonymous173;
struct alg_connection;
struct alg_interface;
struct addr_prof_iter;
struct address_profiler_interface;
struct abs_pointer_activate_interface;
enum __anonymous174;
struct __anonymous175;
typedef struct __anonymous175 abs_pointer_state_t;
struct abs_pointer_interface;
struct a20_interface;
struct virtual_data_bp_handle;
typedef struct virtual_data_bp_handle virtual_data_bp_handle_t;
struct virtual_instr_bp_handle;
typedef struct virtual_instr_bp_handle virtual_instr_bp_handle_t;
struct __anonymous176;
typedef struct __anonymous176 buffer_t;
typedef void (*obj_hap_func_t)();
struct arm_memory_transaction;
typedef struct arm_memory_transaction arm_memory_transaction_t;
struct pci_memory_transaction;
typedef struct pci_memory_transaction pci_memory_transaction_t;
struct ppc_memory_transaction;
typedef struct ppc_memory_transaction ppc_memory_transaction_t;
struct mips_memory_transaction;
typedef struct mips_memory_transaction mips_memory_transaction_t;
struct x86_memory_transaction;
typedef struct x86_memory_transaction x86_memory_transaction_t;
struct generic_transaction;
typedef struct generic_transaction generic_transaction_t;
typedef enum sim_exception sim_exception_t;
typedef cbdata_t cbdata_register_t;
struct __anonymous177;
typedef struct __anonymous177 gbp_header_t;
struct __anonymous178;
typedef struct __anonymous178 gfx_console_screen_text_t;
struct __anonymous179;
typedef struct __anonymous179 source_lines_t;
struct __anonymous180;
typedef struct __anonymous180 follower_time_t;
struct __anonymous181;
typedef struct __anonymous181 bigtime_t;
struct __anonymous182;
typedef struct __anonymous182 global_time_t;
struct __anonymous183;
typedef struct __anonymous183 int128;
typedef struct cycle_interface cycle_interface_t;
typedef void interface_t;
typedef void class_interface_t;
typedef struct branch_arc_iter branch_arc_iter_t;
typedef struct addr_prof_iter addr_prof_iter_t;
struct pre_conf_object_set_t;
typedef struct pre_conf_object_set_t pre_conf_object_set_t;
struct __anonymous184;
typedef struct __anonymous184 uint32_array_t;
typedef int socket_t;
typedef signed char int8;
typedef short int16;
typedef int int32;
typedef enum __anonymous39 attr_attr_t;
typedef enum __anonymous40 class_kind_t;
typedef struct class_data class_data_t;
typedef struct class_info class_info_t;
typedef attr_value_t (*get_attr_t)(lang_void *, conf_object_t *, attr_value_t *);
typedef attr_value_t (*get_class_attr_t)(lang_void *, conf_class_t *, attr_value_t *);
typedef set_error_t (*set_attr_t)(lang_void *, conf_object_t *, attr_value_t *, attr_value_t *);
typedef set_error_t (*set_class_attr_t)(lang_void *, conf_class_t *, attr_value_t *, attr_value_t *);
typedef struct __anonymous37 interface_array_t;
typedef int64 nano_secs_t;
typedef enum __anonymous36 event_class_flag_t;
typedef struct event_class event_class_t;
typedef int64 pico_secs_t;
typedef void (*hap_func_t)(lang_void *);
typedef enum __anonymous35 log_type_t;
typedef enum __anonymous34 global_notifier_type_t;
typedef struct global_notifier_callback global_notifier_callback_t;
typedef int hap_type_t;
typedef enum __anonymous78 data_or_instr_t;
typedef enum __anonymous105 cpu_endian_t;
typedef enum __anonymous22 breakpoint_kind_t;
typedef int breakpoint_id_t;
typedef unsigned long int uintptr_t;
typedef uintptr_t breakpoint_handle_t;
typedef enum __anonymous122 addr_space_t;
typedef enum __anonymous121 read_or_write_t;
typedef enum swap_mode swap_mode_t;
typedef enum __anonymous119 map_type_t;
typedef struct page page_t;
typedef struct map_list map_list_t;
typedef struct granted_mem granted_mem_t;
typedef granted_mem_t *direct_memory_handle_t;
typedef uint64 direct_memory_ack_id_t;
typedef enum __anonymous33 mem_op_type_t;
typedef enum __anonymous148 ini_type_t;
typedef enum __anonymous24 transaction_target_type_t;
typedef struct __anonymous25 transaction_target_type_ret_t;
typedef enum __anonymous28 atom_id_t;
typedef struct __anonymous27 atom_t;
typedef struct transaction transaction_t;
typedef exception_type_t (*transaction_completion_t)(conf_object_t *, transaction_t *, exception_type_t);
typedef enum __anonymous32 transaction_flags_t;
typedef struct transaction_trace_atom_access transaction_trace_atom_access_t;
typedef struct notifier_handle notifier_handle_t;
typedef struct domain_lock domain_lock_t;
typedef struct breakpoint_interface breakpoint_interface_t;
typedef struct breakpoint_change_interface breakpoint_change_interface_t;
typedef struct breakpoint_query_v2_interface breakpoint_query_v2_interface_t;
typedef struct virtual_data_breakpoint_interface virtual_data_breakpoint_interface_t;
typedef struct virtual_instruction_breakpoint_interface virtual_instruction_breakpoint_interface_t;
typedef gen_spr_ret_t (*gen_spr_user_getter_func_t)(conf_object_t *, int64, gen_spr_access_type_t, lang_void *);
typedef gen_spr_ret_t (*gen_spr_user_setter_func_t)(conf_object_t *, int64, uint64, gen_spr_access_type_t, lang_void *);
typedef struct breakpoint_query_interface breakpoint_query_interface_t;
typedef struct pool_protect_interface pool_protect_interface_t;
typedef struct link_endpoint_interface link_endpoint_interface_t;
typedef struct recorder recorder_t;
typedef void (*recorder_input_handler_t)(conf_object_t *, dbuffer_t *, uint32);
typedef struct recorder_interface recorder_interface_t;
typedef struct slave_agent_interface slave_agent_interface_t;
typedef struct slaver_message_interface slaver_message_interface_t;
typedef int hap_flags_t;
typedef enum i2c_device_state i2c_device_state_t;
typedef enum __anonymous130 i2c_device_flag_t;
typedef struct i2c_bus_interface i2c_bus_interface_t;
typedef struct i2c_device_interface i2c_device_interface_t;
typedef enum __anonymous129 i2c_status_t;
typedef struct i2c_link_interface i2c_link_interface_t;
typedef struct i2c_slave_interface i2c_slave_interface_t;
typedef struct i2c_master_interface i2c_master_interface_t;
typedef struct i2c_bridge_interface i2c_bridge_interface_t;
typedef struct context_handler_interface context_handler_interface_t;
typedef enum __anonymous163 branch_arc_type_t;
typedef struct __anonymous164 branch_arc_t;
typedef enum __anonymous165 branch_recorder_direction_t;
typedef struct branch_recorder_handler_interface branch_recorder_handler_interface_t;
typedef struct completion completion_t;
typedef struct wait_handler wait_handler_t;
typedef enum breakpoint_flag breakpoint_flag_t;
typedef enum __anonymous136 notify_mode_t;
typedef struct telemetry_manager_interface telemetry_manager_interface_t;
typedef int cmd_line_id_t;
typedef enum __anonymous20 cmd_line_key_t;
typedef enum __anonymous21 cmd_line_mod_t;
typedef struct cmd_line_frontend_interface cmd_line_frontend_interface_t;
typedef struct cmd_line_selection_interface cmd_line_selection_interface_t;
typedef struct terminal_server_interface terminal_server_interface_t;
typedef struct terminal_client_interface terminal_client_interface_t;
typedef char *(*prompt_customizer_t)(const char *);
typedef enum __anonymous160 save_flags_t;
typedef struct tcf_channel_interface tcf_channel_interface_t;
typedef enum __anonymous19 stop_type_t;
typedef struct execute_environ execute_environ_t;
typedef int hap_handle_t;
typedef struct profile_area profile_area_t;
typedef void (*output_handler_t)(lang_void *, const char *, size_t);
typedef enum __anonymous18 snapshot_error_t;
typedef struct direct_memory_interface direct_memory_interface_t;
typedef struct direct_memory_lookup_interface direct_memory_lookup_interface_t;
typedef struct direct_memory_lookup_v2_interface direct_memory_lookup_v2_interface_t;
typedef struct direct_memory_update_interface direct_memory_update_interface_t;
typedef struct direct_memory_flush_interface direct_memory_flush_interface_t;
typedef struct direct_memory_tags_interface direct_memory_tags_interface_t;
typedef enum __anonymous115 page_bank_bits_t;
typedef struct page_bank_interface page_bank_interface_t;
typedef struct page_bank_client_interface page_bank_client_interface_t;
typedef struct stc_interface stc_interface_t;
typedef struct log_object_interface log_object_interface_t;
typedef struct __anonymous14 deferred_transaction_info_vect_t;
typedef struct icode_interface icode_interface_t;
typedef enum __anonymous12 license_product_t;
typedef struct thread_domain thread_domain_t;
typedef struct thread_domain_holder thread_domain_holder_t;
typedef struct thread_domain_group thread_domain_group_t;
typedef enum __anonymous174 abs_pointer_buttons_t;
typedef struct abs_pointer_interface abs_pointer_interface_t;
typedef struct abs_pointer_activate_interface abs_pointer_activate_interface_t;
typedef struct instrumentation_order_interface instrumentation_order_interface_t;
typedef struct callback_info_interface callback_info_interface_t;
typedef uintptr_t bank_callback_handle_t;
typedef void (*before_read_callback_t)(conf_object_t *, bank_before_read_interface_t *, bank_access_t *, lang_void *);
typedef void (*after_read_callback_t)(conf_object_t *, bank_after_read_interface_t *, bank_access_t *, lang_void *);
typedef void (*before_write_callback_t)(conf_object_t *, bank_before_write_interface_t *, bank_access_t *, lang_void *);
typedef void (*after_write_callback_t)(conf_object_t *, bank_after_write_interface_t *, bank_access_t *, lang_void *);
typedef struct bank_instrumentation_subscribe_interface bank_instrumentation_subscribe_interface_t;
typedef enum __anonymous161 cache_control_operation_t;
typedef struct cache_control_interface cache_control_interface_t;
typedef struct cdrom_media_interface cdrom_media_interface_t;
typedef struct component_interface component_interface_t;
typedef enum __anonymous153 connector_direction_t;
typedef struct connector_interface connector_interface_t;
typedef struct component_connector_interface component_connector_interface_t;
typedef struct disk_component_interface disk_component_interface_t;
typedef struct co_execute_interface co_execute_interface_t;
typedef struct synchronous_mode_interface synchronous_mode_interface_t;
typedef enum __anonymous154 concurrency_mode_t;
typedef struct concurrency_mode_interface concurrency_mode_interface_t;
typedef struct concurrency_group_interface concurrency_group_interface_t;
typedef struct instrumentation_entry _cpu_instrumentation_entry_t;
typedef _cpu_instrumentation_entry_t cpu_cb_handle_t;
typedef _cpu_instrumentation_entry_t cpu_stream_handle_t;
typedef struct cpu_instruction_query_interface cpu_instruction_query_interface_t;
typedef enum __anonymous147 page_crossing_info_t;
typedef struct cpu_memory_query_interface cpu_memory_query_interface_t;
typedef struct cpu_exception_query_interface cpu_exception_query_interface_t;
typedef void (*cpu_instruction_cb_t)(conf_object_t *, conf_object_t *, instruction_handle_t *, lang_void *);
typedef void (*cpu_callback_free_user_data_cb_t)(conf_object_t *, conf_object_t *, lang_void *);
typedef void (*cpu_memory_cb_t)(conf_object_t *, conf_object_t *, memory_handle_t *, lang_void *);
typedef struct cpu_cached_instruction_interface cpu_cached_instruction_interface_t;
typedef struct cpu_cached_instruction_once_interface cpu_cached_instruction_once_interface_t;
typedef void (*cpu_cached_instruction_cb_t)(conf_object_t *, conf_object_t *, cached_instruction_handle_t *, instruction_handle_t *, lang_void *);
typedef cpu_emulation_t (*cpu_emulation_cb_t)(conf_object_t *, conf_object_t *, lang_void *);
typedef struct cpu_instruction_decoder_interface cpu_instruction_decoder_interface_t;
typedef int (*cpu_instruction_decoder_cb_t)(conf_object_t *, conf_object_t *, decoder_handle_t *, instruction_handle_t *, lang_void *);
typedef tuple_int_string_t (*cpu_instruction_disassemble_cb_t)(conf_object_t *, conf_object_t *, generic_address_t, cpu_bytes_t);
typedef logical_address_t (*cpu_address_cb_t)(conf_object_t *, conf_object_t *, logical_address_t, address_handle_t *, lang_void *);
typedef void (*cpu_exception_cb_t)(conf_object_t *, conf_object_t *, exception_handle_t *, lang_void *);
typedef void (*cpu_exception_return_cb_t)(conf_object_t *, conf_object_t *, exception_return_handle_t *, lang_void *);
typedef void (*cpu_mode_change_cb_t)(conf_object_t *, conf_object_t *, processor_mode_t, processor_mode_t, lang_void *);
typedef void (*cpu_control_register_read_cb_t)(conf_object_t *, conf_object_t *, int, lang_void *);
typedef void (*cpu_control_register_write_cb_t)(conf_object_t *, conf_object_t *, int, uint64, lang_void *);
typedef enum __anonymous149 cpu_access_scope_t;
typedef struct cpu_instrumentation_subscribe_interface cpu_instrumentation_subscribe_interface_t;
typedef uint64 cpu_stream_bits_t;
typedef struct cycle_event_interface cycle_event_interface_t;
typedef struct cycle_control_interface cycle_control_interface_t;
typedef struct data_profiler_interface data_profiler_interface_t;
typedef struct device_identification_interface device_identification_interface_t;
typedef struct event_delta_interface event_delta_interface_t;
typedef struct event_handler_interface event_handler_interface_t;
typedef struct step_instrumentation step_handle_t;
typedef struct clock_instrumentation cycle_handle_t;
typedef void (*step_event_cb_t)(conf_object_t *, conf_object_t *, conf_object_t *, pc_step_t, const char *, const char *, attr_value_t, lang_void *);
typedef struct step_event_instrumentation_interface step_event_instrumentation_interface_t;
typedef void (*cycle_event_cb_t)(conf_object_t *, conf_object_t *, conf_object_t *, cycles_t, const char *, const char *, attr_value_t, lang_void *);
typedef struct cycle_event_instrumentation_interface cycle_event_instrumentation_interface_t;
typedef struct exception_interface exception_interface_t;
typedef void (*instruction_trace_callback_t)(lang_void *, conf_object_t *, linear_address_t, logical_address_t, physical_address_t, byte_string_t);
typedef struct exec_trace_interface exec_trace_interface_t;
typedef struct execute_interface execute_interface_t;
typedef struct cell_inspection_interface cell_inspection_interface_t;
typedef struct execute_control_interface execute_control_interface_t;
typedef struct freerun_interface freerun_interface_t;
typedef struct vga_update_interface vga_update_interface_t;
typedef enum __anonymous133 gfx_con_pixel_fmt_t;
typedef struct gfx_con_interface gfx_con_interface_t;
typedef struct image_interface image_interface_t;
typedef struct linear_image_interface linear_image_interface_t;
typedef enum __anonymous125 instruction_fetch_mode_t;
typedef struct instruction_fetch_interface instruction_fetch_interface_t;
typedef enum __anonymous124 ireg_info_t;
typedef struct int_register_interface int_register_interface_t;
typedef struct jit_control_interface jit_control_interface_t;
typedef struct keyboard_interface keyboard_interface_t;
typedef struct keyboard_console_interface keyboard_console_interface_t;
typedef struct mouse_interface mouse_interface_t;
typedef struct dummy_memory_page_interface dummy_memory_page_interface_t;
typedef struct transaction_callback_entry transaction_cb_handle_t;
typedef next_map_t (*transaction_issue_cb_t)(conf_object_t *, const map_target_t *, const char *, map_info_t *, transaction_t *, uint64, do_map_target_cb_t, lang_void *, lang_void *);
typedef struct transaction_subscribe_interface transaction_subscribe_interface_t;
typedef struct memory_profiler_interface memory_profiler_interface_t;
typedef struct mmc_interface mmc_interface_t;
typedef struct packet_interface packet_interface_t;
typedef struct processor_info_interface processor_info_interface_t;
typedef struct processor_info_v2_interface processor_info_v2_interface_t;
typedef struct processor_endian_interface processor_endian_interface_t;
typedef struct processor_cli_interface processor_cli_interface_t;
typedef struct opcode_info_interface opcode_info_interface_t;
typedef struct processor_gui_interface processor_gui_interface_t;
typedef enum __anonymous104 ram_operation_flags_t;
typedef struct ram_interface ram_interface_t;
typedef ram_interface_t rom_interface_t;
typedef struct ram_callback_entry ram_cb_handle_t;
typedef access_t (*access_filter_cb_t)(conf_object_t *, conf_object_t *, conf_object_t *, uint64, unsigned, access_t, lang_void *);
typedef void (*ram_access_cb_t)(conf_object_t *, conf_object_t *, uint64, transaction_t *, lang_void *);
typedef struct ram_access_subscribe_interface ram_access_subscribe_interface_t;
typedef struct register_view_interface register_view_interface_t;
typedef struct register_view_catalog_interface register_view_catalog_interface_t;
typedef struct register_view_read_only_interface register_view_read_only_interface_t;
typedef enum __anonymous155 sim_key_t;
typedef struct simulator_cache_interface simulator_cache_interface_t;
typedef struct uint64_state_interface uint64_state_interface_t;
typedef struct step_interface step_interface_t;
typedef struct step_cycle_ratio_interface step_cycle_ratio_interface_t;
typedef struct stall_interface stall_interface_t;
typedef struct step_info_interface step_info_interface_t;
typedef struct temporal_state_interface temporal_state_interface_t;
typedef struct timing_model_interface timing_model_interface_t;
typedef struct snoop_memory_interface snoop_memory_interface_t;
typedef struct transaction_interface transaction_interface_t;
typedef struct vga_text_interface vga_text_interface_t;
typedef struct vga_text_update_interface vga_text_update_interface_t;
typedef struct address_profiler_interface address_profiler_interface_t;
typedef struct branch_arc_interface branch_arc_interface_t;
typedef struct checkpoint_interface checkpoint_interface_t;
typedef struct serial_console_frontend_interface serial_console_frontend_interface_t;
typedef struct con_input_interface con_input_interface_t;
typedef struct con_input_code_interface con_input_code_interface_t;
typedef int (*break_string_cb_t)(conf_object_t *, const char *, int64, lang_void *);
typedef struct break_strings_interface break_strings_interface_t;
typedef struct break_strings_v2_interface break_strings_v2_interface_t;
typedef int (*gfx_break_cb_t)(conf_object_t *, int64, lang_void *);
typedef struct gfx_break_interface gfx_break_interface_t;
typedef struct telnet_connection_v2_interface telnet_connection_v2_interface_t;
typedef struct vnc_server_v2_interface vnc_server_v2_interface_t;
typedef struct host_serial_interface host_serial_interface_t;
typedef struct screenshot_interface screenshot_interface_t;
typedef struct text_console_frontend_interface text_console_frontend_interface_t;
typedef enum __anonymous91 text_console_key_t;
typedef enum __anonymous90 text_console_modifier_t;
typedef struct text_console_backend_interface text_console_backend_interface_t;
typedef enum __anonymous131 gfx_console_led_t;
typedef enum __anonymous132 gfx_console_mouse_button_t;
typedef struct gfx_console_frontend_interface gfx_console_frontend_interface_t;
typedef struct gfx_console_backend_interface gfx_console_backend_interface_t;
typedef struct winsome_console_interface winsome_console_interface_t;
typedef struct gui_console_backend_interface gui_console_backend_interface_t;
typedef uint64 debug_cancel_id_t;
typedef struct debug_notification_interface debug_notification_interface_t;
typedef int64 debug_setup_id_t;
typedef struct debug_setup_interface debug_setup_interface_t;
typedef struct debug_query_interface debug_query_interface_t;
typedef struct debug_step_interface debug_step_interface_t;
typedef struct debug_symbol_interface debug_symbol_interface_t;
typedef struct debug_symbol_file_interface debug_symbol_file_interface_t;
typedef struct instrumentation_filter_slave_interface instrumentation_filter_slave_interface_t;
typedef struct instrumentation_filter_status_interface instrumentation_filter_status_interface_t;
typedef struct instrumentation_filter_master_interface instrumentation_filter_master_interface_t;
typedef struct instrumentation_tool_interface instrumentation_tool_interface_t;
typedef struct instrumentation_connection_interface instrumentation_connection_interface_t;
typedef uint64 cancel_id_t;
typedef uint64 entity_id_t;
typedef uint64 transaction_id_t;
typedef uint64 request_id_t;
typedef struct osa_component_interface osa_component_interface_t;
typedef struct osa_control_interface osa_control_interface_t;
typedef struct osa_control_v2_interface osa_control_v2_interface_t;
typedef struct osa_node_tree_query_interface osa_node_tree_query_interface_t;
typedef struct osa_node_tree_notification_interface osa_node_tree_notification_interface_t;
typedef struct osa_node_path_interface osa_node_path_interface_t;
typedef enum __anonymous116 osa_read_len_t;
typedef struct osa_machine_notification_interface osa_machine_notification_interface_t;
typedef struct osa_machine_query_interface osa_machine_query_interface_t;
typedef struct osa_mapper_admin_interface osa_mapper_admin_interface_t;
typedef struct osa_mapper_control_interface osa_mapper_control_interface_t;
typedef struct osa_mapper_query_interface osa_mapper_query_interface_t;
typedef struct osa_node_tree_admin_interface osa_node_tree_admin_interface_t;
typedef struct osa_parameters_interface osa_parameters_interface_t;
typedef struct osa_tracker_component_interface osa_tracker_component_interface_t;
typedef struct osa_tracker_state_admin_interface osa_tracker_state_admin_interface_t;
typedef struct osa_tracker_state_notification_interface osa_tracker_state_notification_interface_t;
typedef struct osa_tracker_state_query_interface osa_tracker_state_query_interface_t;
typedef struct osa_tracker_control_interface osa_tracker_control_interface_t;
typedef struct osa_micro_checkpoint_interface osa_micro_checkpoint_interface_t;
typedef struct register_breakpoint_interface register_breakpoint_interface_t;
typedef struct recorder_v2_interface recorder_v2_interface_t;
typedef struct recorded_interface recorded_interface_t;
typedef struct scalar_time_interface scalar_time_interface_t;
typedef struct probe_interface probe_interface_t;
typedef struct probe_index_interface probe_index_interface_t;
typedef struct probe_array_interface probe_array_interface_t;
typedef struct probe_subscribe_interface probe_subscribe_interface_t;
typedef struct probe_sampler_cache_interface probe_sampler_cache_interface_t;
typedef struct table_interface table_interface_t;
typedef struct breakpoint_registration_interface breakpoint_registration_interface_t;
typedef struct breakpoint_type_interface breakpoint_type_interface_t;
typedef struct breakpoint_type_provider_interface breakpoint_type_provider_interface_t;
typedef struct breakpoint_manager_interface breakpoint_manager_interface_t;
typedef struct preference_interface preference_interface_t;
typedef enum __anonymous172 apic_destination_mode_t;
typedef enum __anonymous171 apic_delivery_mode_t;
typedef enum __anonymous170 apic_trigger_mode_t;
typedef enum __anonymous169 apic_bus_status_t;
typedef struct apic_bus_interface apic_bus_interface_t;
typedef struct arinc429_bus_interface arinc429_bus_interface_t;
typedef struct arinc429_receiver_interface arinc429_receiver_interface_t;
typedef struct datagram_link_interface datagram_link_interface_t;
typedef struct ethernet_common_interface ethernet_common_interface_t;
typedef struct ethernet_cable_interface ethernet_cable_interface_t;
typedef void (*ethernet_link_snoop_t)(lang_void *, conf_object_t *, const frags_t *, eth_frame_crc_status_t);
typedef struct ethernet_snoop_interface ethernet_snoop_interface_t;
typedef struct ethernet_vlan_snoop_interface ethernet_vlan_snoop_interface_t;
typedef int (*break_net_cb_t)(conf_object_t *, bytes_t, int, int64);
typedef struct network_breakpoint_interface network_breakpoint_interface_t;
typedef void (*ethernet_probe_snoop_t)(lang_void *, conf_object_t *, eth_probe_side_t, const frags_t *, eth_frame_crc_status_t);
typedef struct ethernet_probe_interface ethernet_probe_interface_t;
typedef enum __anonymous134 firewire_ack_code_t;
typedef struct firewire_bus_interface firewire_bus_interface_t;
typedef struct firewire_device_interface firewire_device_interface_t;
typedef struct frequency_interface frequency_interface_t;
typedef struct frequency_listener_interface frequency_listener_interface_t;
typedef struct scale_factor_listener_interface scale_factor_listener_interface_t;
typedef struct simple_dispatcher_interface simple_dispatcher_interface_t;
typedef int mod_def_t;
typedef struct gbic_transceiver_interface gbic_transceiver_interface_t;
typedef struct i2c_slave_v2_interface i2c_slave_v2_interface_t;
typedef enum __anonymous128 i2c_ack_t;
typedef struct i2c_master_v2_interface i2c_master_v2_interface_t;
typedef enum __anonymous127 i3c_ack_t;
typedef struct i3c_master_interface i3c_master_interface_t;
typedef struct i3c_slave_interface i3c_slave_interface_t;
typedef struct i3c_daa_snoop_interface i3c_daa_snoop_interface_t;
typedef struct i3c_hdr_slave_interface i3c_hdr_slave_interface_t;
typedef struct i3c_hdr_master_interface i3c_hdr_master_interface_t;
typedef enum __anonymous126 ieee_802_3_link_status_t;
typedef struct ieee_802_3_phy_interface ieee_802_3_phy_interface_t;
typedef struct ieee_802_3_phy_v2_interface ieee_802_3_phy_v2_interface_t;
typedef struct ieee_802_3_phy_v3_interface ieee_802_3_phy_v3_interface_t;
typedef struct ieee_802_3_mac_interface ieee_802_3_mac_interface_t;
typedef struct ieee_802_3_mac_v3_interface ieee_802_3_mac_v3_interface_t;
typedef int (*interrupt_ack_fn_t)(conf_object_t *NOTNULL);
typedef struct interrupt_ack_interface interrupt_ack_interface_t;
typedef struct interrupt_cpu_interface interrupt_cpu_interface_t;
typedef enum __anonymous123 interrupt_source_t;
typedef struct interrupt_subscriber_interface interrupt_subscriber_interface_t;
typedef struct simple_interrupt_interface simple_interrupt_interface_t;
typedef struct io_memory_interface io_memory_interface_t;
typedef struct map_demap_interface map_demap_interface_t;
typedef struct memory_space_interface memory_space_interface_t;
typedef struct port_space_interface port_space_interface_t;
typedef struct translate_interface translate_interface_t;
typedef struct bridge_interface bridge_interface_t;
typedef struct microwire_interface microwire_interface_t;
typedef struct mii_management_interface mii_management_interface_t;
typedef struct mii_interface mii_interface_t;
typedef struct mdio45_bus_interface mdio45_bus_interface_t;
typedef struct mdio45_phy_interface mdio45_phy_interface_t;
typedef struct nand_flash_interface nand_flash_interface_t;
typedef struct pci_device_interface pci_device_interface_t;
typedef struct pci_bus_interface pci_bus_interface_t;
typedef struct pci_upstream_interface pci_upstream_interface_t;
typedef struct pci_upstream_operation_interface pci_upstream_operation_interface_t;
typedef struct pci_downstream_interface pci_downstream_interface_t;
typedef struct pci_bridge_interface pci_bridge_interface_t;
typedef struct pci_interrupt_interface pci_interrupt_interface_t;
typedef enum __anonymous113 pcie_message_type_t;
typedef enum __anonymous11 pcie_ecs_t;
typedef struct pci_multi_function_device_interface pci_multi_function_device_interface_t;
typedef struct pcie_device_interface pcie_device_interface_t;
typedef enum __anonymous110 pcie_type_t;
typedef struct pcie_map_interface pcie_map_interface_t;
typedef struct pcie_port_control_interface pcie_port_control_interface_t;
typedef enum __anonymous111 pcie_hotplug_pd_t;
typedef enum __anonymous112 pcie_hotplug_mrl_t;
typedef struct pcie_hotplug_events_interface pcie_hotplug_events_interface_t;
typedef struct pcie_link_training_interface pcie_link_training_interface_t;
typedef enum __anonymous10 pcie_msg_route_t;
typedef enum __anonymous9 pcie_at_t;
typedef enum __anonymous7 pcie_error_t;
typedef struct __anonymous8 pcie_error_ret_t;
typedef struct __anonymous6 pcie_byte_count_ret_t;
typedef struct pcie_adapter_compat_interface pcie_adapter_compat_interface_t;
typedef struct pci_express_interface pci_express_interface_t;
typedef struct pci_express_hotplug_interface pci_express_hotplug_interface_t;
typedef enum __anonymous4 pcie_link_speed_t;
typedef enum __anonymous3 pcie_link_width_t;
typedef struct __anonymous5 pcie_link_negotiation_t;
typedef struct __anonymous2 pcie_ide_secured_t;
typedef enum __anonymous146 cxl_type_t;
typedef struct cxl_map_interface cxl_map_interface_t;
typedef struct cxl_non_device_decoder_handling_interface cxl_non_device_decoder_handling_interface_t;
typedef struct cxl_mem_downstream_port_managing_interface cxl_mem_downstream_port_managing_interface_t;
typedef struct sata_interface sata_interface_t;
typedef struct serial_device_interface serial_device_interface_t;
typedef struct extended_serial_interface extended_serial_interface_t;
typedef struct rs232_device_interface rs232_device_interface_t;
typedef enum __anonymous98 stop_bits_t;
typedef enum __anonymous99 parity_mode_t;
typedef struct rs232_console_interface rs232_console_interface_t;
typedef enum serial_peripheral_interface_flags serial_peripheral_interface_flags_t;
typedef struct serial_peripheral_interface_master_interface serial_peripheral_interface_master_interface_t;
typedef struct serial_peripheral_interface_slave_interface serial_peripheral_interface_slave_interface_t;
typedef struct signal_interface signal_interface_t;
typedef struct multi_level_signal_interface multi_level_signal_interface_t;
typedef struct pulse_interface pulse_interface_t;
typedef int telemetry_class_id_t;
typedef int telemetry_id_t;
typedef struct telemetry_interface telemetry_interface_t;
typedef struct translator_interface translator_interface_t;
typedef struct transaction_translator_interface transaction_translator_interface_t;
typedef struct translation_flush_interface translation_flush_interface_t;
typedef enum __anonymous83 usb_transfer_completion_t;
typedef enum __anonymous87 usb_direction_t;
typedef enum __anonymous86 usb_status_t;
typedef enum __anonymous85 usb_type_t;
typedef enum __anonymous84 usb_speed_t;
typedef struct __anonymous88 usb_transfer_t;
typedef struct usb_device_interface usb_device_interface_t;
typedef struct usb_interface usb_interface_t;
typedef struct vectored_interrupt_interface vectored_interrupt_interface_t;
typedef struct vectored_interrupt_source_interface vectored_interrupt_source_interface_t;
typedef struct x86_cstate_interface x86_cstate_interface_t;
typedef struct x86_pkg_cstate_interface x86_pkg_cstate_interface_t;
typedef struct x86_cstate_notification_interface x86_cstate_notification_interface_t;
typedef struct i8051_interrupt_interface i8051_interrupt_interface_t;
typedef struct i8051_timer_interface i8051_timer_interface_t;
typedef struct arm_interface arm_interface_t;
typedef struct arm_coprocessor_interface arm_coprocessor_interface_t;
typedef struct arm_avic_interface arm_avic_interface_t;
typedef enum __anonymous166 arm_trustzone_mode_t;
typedef struct arm_trustzone_interface arm_trustzone_interface_t;
typedef struct arm_external_debug_interface arm_external_debug_interface_t;
typedef struct arm_gic_interface arm_gic_interface_t;
typedef struct arm_gic_cpu_state_interface arm_gic_cpu_state_interface_t;
typedef struct arm_cpu_group_exclusive_interface arm_cpu_group_exclusive_interface_t;
typedef struct arm_cpu_group_event_interface arm_cpu_group_event_interface_t;
typedef enum __anonymous167 arm_translation_regime_t;
typedef struct arm_cpu_group_tlb_interface arm_cpu_group_tlb_interface_t;
typedef struct mips_interface mips_interface_t;
typedef struct mips_coprocessor_interface mips_coprocessor_interface_t;
typedef struct fmn_station_control_interface fmn_station_control_interface_t;
typedef struct fmn_station_control_v2_interface fmn_station_control_v2_interface_t;
typedef struct mips_cache_instruction_interface mips_cache_instruction_interface_t;
typedef struct mips_ite_interface mips_ite_interface_t;
typedef struct mips_eic_interface mips_eic_interface_t;
typedef struct mips_exception_query_interface mips_exception_query_interface_t;
typedef enum __anonymous107 ppc_mc_exc_t;
typedef enum __anonymous108 ppc_sleep_state_t;
typedef struct ppc_interface ppc_interface_t;
typedef struct spr_interface spr_interface_t;
typedef struct pmr_interface pmr_interface_t;
typedef enum x86_access_type x86_access_type_t;
typedef enum __anonymous68 x86_memory_type_t;
typedef struct x86_access_type_interface x86_access_type_interface_t;
typedef struct x86_tlb_interface x86_tlb_interface_t;
typedef struct __anonymous44 x86_tlb_entry_t;
typedef struct x86_tlb_v2_interface x86_tlb_v2_interface_t;
typedef struct __anonymous42 x86_tlb_entry_v3_t;
typedef struct x86_tlb_v3_interface x86_tlb_v3_interface_t;
typedef enum __anonymous76 x86_pin_t;
typedef struct x86_interface x86_interface_t;
typedef struct x86_cpuid_interface x86_cpuid_interface_t;
typedef struct x86_cpuid_query_interface x86_cpuid_query_interface_t;
typedef enum __anonymous168 local_apic_interrupt_t;
typedef struct apic_cpu_interface apic_cpu_interface_t;
typedef struct a20_interface a20_interface_t;
typedef x86_msr_getter_ret_t (*x86_msr_getter_func_t)(conf_object_t *, int64, x86_msr_access_type_t, lang_void *);
typedef x86_msr_ret_t (*x86_msr_setter_func_t)(conf_object_t *, int64, uint64, x86_msr_access_type_t, lang_void *);
typedef struct x86_msr_interface x86_msr_interface_t;
typedef struct x86_cache_flush_interface x86_cache_flush_interface_t;
typedef struct __anonymous47 smm_reg_state_t;
typedef struct x86_smm_state_interface x86_smm_state_interface_t;
typedef struct x86_smm_interface x86_smm_interface_t;
typedef enum __anonymous53 x86_system_seg_t;
typedef enum __anonymous63 x86_cr_t;
typedef enum __anonymous62 x86_dr_t;
typedef enum __anonymous51 x86_xcr_t;
typedef enum __anonymous64 x86_activity_t;
typedef enum __anonymous61 x86_exec_mode_t;
typedef enum __anonymous75 x86_seg_t;
typedef enum __anonymous58 x86_intstate_t;
typedef struct x86_reg_access_interface x86_reg_access_interface_t;
typedef struct x86_exception_interface x86_exception_interface_t;
typedef enum __anonymous70 x86_exception_source_t;
typedef enum __anonymous71 x86_processor_mode_t;
typedef struct x86_memory_access_interface x86_memory_access_interface_t;
typedef struct x86_vmp_control_interface x86_vmp_control_interface_t;
typedef enum __anonymous81 vmp_feature_t;
typedef enum __anonymous80 vmp_info_t;
typedef struct vmp_interface vmp_interface_t;
typedef enum __anonymous41 xed_data_type_t;
typedef struct xed_access_interface xed_access_interface_t;
typedef struct x86_ept_interface x86_ept_interface_t;
typedef struct x86_instruction_query_interface x86_instruction_query_interface_t;
typedef struct x86_memory_query_interface x86_memory_query_interface_t;
typedef struct x86_exception_query_interface x86_exception_query_interface_t;
typedef struct x86_address_query_interface x86_address_query_interface_t;
typedef void (*x86_mode_switch_cb_t)(conf_object_t *, conf_object_t *, x86_detailed_exec_mode_t, lang_void *);
typedef struct x86_instrumentation_subscribe_interface x86_instrumentation_subscribe_interface_t;
typedef struct x86_instrumentation_subscribe_v2_interface x86_instrumentation_subscribe_v2_interface_t;
typedef void (*vmx_mode_switch_cb_t)(conf_object_t *, conf_object_t *, vmx_mode_t, physical_address_t, lang_void *);
typedef struct vmx_instrumentation_subscribe_interface vmx_instrumentation_subscribe_interface_t;
typedef void (*smm_switch_cb_t)(conf_object_t *obj, conf_object_t *cpu, lang_void *user_data);
typedef struct smm_instrumentation_subscribe_interface smm_instrumentation_subscribe_interface_t;
typedef struct nios_interface nios_interface_t;
typedef struct nios_eic_interface nios_eic_interface_t;
typedef struct nios_cache_interface nios_cache_interface_t;
typedef struct nios_custom_interface nios_custom_interface_t;
typedef struct xtensa_wwdt_config_interface xtensa_wwdt_config_interface_t;
typedef struct xtensa_wwdt_faultinfo_interface xtensa_wwdt_faultinfo_interface_t;
typedef struct xtensa_internal_memories_interface xtensa_internal_memories_interface_t;
typedef struct xtensa_mpu_lookup_interface xtensa_mpu_lookup_interface_t;
typedef struct xtensa_lookup_interface xtensa_lookup_interface_t;
typedef struct xtensa_export_state_interface xtensa_export_state_interface_t;
typedef struct xtensa_output_queue_interface xtensa_output_queue_interface_t;
typedef struct xtensa_input_queue_interface xtensa_input_queue_interface_t;
typedef struct xtensa_import_wire_interface xtensa_import_wire_interface_t;
typedef enum __anonymous102 riscv_cpu_mode_t;
typedef struct riscv_coprocessor_interface riscv_coprocessor_interface_t;
typedef enum __anonymous100 riscv_imsic_file_id_t;
typedef struct riscv_imsic_interface riscv_imsic_interface_t;
typedef struct riscv_signal_sgeip_interface riscv_signal_sgeip_interface_t;
typedef enum __anonymous103 riscv_vectoring_mode_t;
typedef struct riscv_clic_interrupt_interface riscv_clic_interrupt_interface_t;
typedef struct riscv_clic_interface riscv_clic_interface_t;
typedef enum __anonymous0 riscv_io_error_t;
typedef struct __anonymous1 riscv_io_error_ret_t;
typedef struct riscv_instruction_action_interface riscv_instruction_action_interface_t;
typedef uint64 (*riscv_csr_access_cb_t)(conf_object_t *, conf_object_t *, uint32, uint64, uint64, riscv_csr_access_type_t);
typedef uint64 (*riscv_reset_cb_t)(conf_object_t *, conf_object_t *);
typedef struct riscv_custom_csr_interface riscv_custom_csr_interface_t;
typedef struct sh_interrupt_interface sh_interrupt_interface_t;
typedef struct sparc_v8_interface sparc_v8_interface_t;
typedef struct sparc_v8_ecc_fault_injection_interface sparc_v8_ecc_fault_injection_interface_t;
typedef struct coreint_interface coreint_interface_t;
typedef struct bp_manager_interface bp_manager_interface_t;
typedef enum __anonymous140 async_flush_status_t;
typedef struct dist_control_interface dist_control_interface_t;
typedef struct dist_control_node_interface dist_control_node_interface_t;
typedef struct global_time_interface global_time_interface_t;
typedef struct instrum_registration_interface instrum_registration_interface_t;
typedef int symfile_id_t;
typedef struct __anonymous92 symbol_list_t;
typedef enum __anonymous94 load_symbols_flags_t;
typedef struct symbols_interface symbols_interface_t;
typedef struct sync_initial_time_interface sync_initial_time_interface_t;
typedef enum __anonymous135 exec_mode_t;
typedef struct external_connection_ctl_interface external_connection_ctl_interface_t;
typedef struct external_connection_events_interface external_connection_events_interface_t;
typedef struct tcp_connection_info_interface tcp_connection_info_interface_t;
typedef struct mips_mt_ase_interface mips_mt_ase_interface_t;
typedef struct mips_mt_ase_policy_interface mips_mt_ase_policy_interface_t;
typedef struct alg_connection *connection_handle_t;
typedef struct alg_connection alg_connection_t;
typedef enum __anonymous173 alg_ret_t;
typedef struct alg_interface alg_interface_t;
typedef struct port_forward_interface port_forward_interface_t;
typedef struct forward_destination_rewriter_interface forward_destination_rewriter_interface_t;
typedef struct link_endpoint_v2_interface link_endpoint_v2_interface_t;
typedef struct follower_agent_interface follower_agent_interface_t;
typedef struct leader_message_interface leader_message_interface_t;
typedef struct __anonymous158 disassembly_vect_t;
typedef struct __anonymous156 disassembly_mode_info_vect_t;
typedef struct class_disassembly_interface class_disassembly_interface_t;
typedef struct internal_class_testing_interface internal_class_testing_interface_t;
typedef long int intptr_t;

#ifdef PYWRAPGEN_TYPEMAPS
%body (python) {
static UNUSED opaque_type_t *struct_a20_interface_t_type_p;
};

%init (python) {
#ifdef struct_a20_interface_t_type_p
	struct_a20_interface_t_type_p = VT_lookup_python_opaque_type("api.a20_interface_t");
#endif /* struct_a20_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_abs_pointer_interface_t_type_p;
};

%init (python) {
#ifdef struct_abs_pointer_interface_t_type_p
	struct_abs_pointer_interface_t_type_p = VT_lookup_python_opaque_type("api.abs_pointer_interface_t");
#endif /* struct_abs_pointer_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_abs_pointer_state_t_type_p;
};

%init (python) {
#ifdef struct_abs_pointer_state_t_type_p
	struct_abs_pointer_state_t_type_p = VT_lookup_python_opaque_type("api.abs_pointer_state_t");
#endif /* struct_abs_pointer_state_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_abs_pointer_activate_interface_t_type_p;
};

%init (python) {
#ifdef struct_abs_pointer_activate_interface_t_type_p
	struct_abs_pointer_activate_interface_t_type_p = VT_lookup_python_opaque_type("api.abs_pointer_activate_interface_t");
#endif /* struct_abs_pointer_activate_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_address_profiler_interface_t_type_p;
};

%init (python) {
#ifdef struct_address_profiler_interface_t_type_p
	struct_address_profiler_interface_t_type_p = VT_lookup_python_opaque_type("api.address_profiler_interface_t");
#endif /* struct_address_profiler_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_addr_prof_iter_t_type_p;
};

%init (python) {
#ifdef struct_addr_prof_iter_t_type_p
	struct_addr_prof_iter_t_type_p = VT_lookup_python_opaque_type("api.addr_prof_iter_t");
#endif /* struct_addr_prof_iter_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_alg_interface_t_type_p;
};

%init (python) {
#ifdef struct_alg_interface_t_type_p
	struct_alg_interface_t_type_p = VT_lookup_python_opaque_type("api.alg_interface_t");
#endif /* struct_alg_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_alg_connection_t_type_p;
};

%init (python) {
#ifdef struct_alg_connection_t_type_p
	struct_alg_connection_t_type_p = VT_lookup_python_opaque_type("api.alg_connection_t");
#endif /* struct_alg_connection_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_apic_bus_interface_t_type_p;
};

%init (python) {
#ifdef struct_apic_bus_interface_t_type_p
	struct_apic_bus_interface_t_type_p = VT_lookup_python_opaque_type("api.apic_bus_interface_t");
#endif /* struct_apic_bus_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_apic_cpu_interface_t_type_p;
};

%init (python) {
#ifdef struct_apic_cpu_interface_t_type_p
	struct_apic_cpu_interface_t_type_p = VT_lookup_python_opaque_type("api.apic_cpu_interface_t");
#endif /* struct_apic_cpu_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_arinc429_bus_interface_t_type_p;
};

%init (python) {
#ifdef struct_arinc429_bus_interface_t_type_p
	struct_arinc429_bus_interface_t_type_p = VT_lookup_python_opaque_type("api.arinc429_bus_interface_t");
#endif /* struct_arinc429_bus_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_arinc429_receiver_interface_t_type_p;
};

%init (python) {
#ifdef struct_arinc429_receiver_interface_t_type_p
	struct_arinc429_receiver_interface_t_type_p = VT_lookup_python_opaque_type("api.arinc429_receiver_interface_t");
#endif /* struct_arinc429_receiver_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_arm_interface_t_type_p;
};

%init (python) {
#ifdef struct_arm_interface_t_type_p
	struct_arm_interface_t_type_p = VT_lookup_python_opaque_type("api.arm_interface_t");
#endif /* struct_arm_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_arm_avic_interface_t_type_p;
};

%init (python) {
#ifdef struct_arm_avic_interface_t_type_p
	struct_arm_avic_interface_t_type_p = VT_lookup_python_opaque_type("api.arm_avic_interface_t");
#endif /* struct_arm_avic_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_arm_avic_t_type_p;
};

%init (python) {
#ifdef struct_arm_avic_t_type_p
	struct_arm_avic_t_type_p = VT_lookup_python_opaque_type("api.arm_avic_t");
#endif /* struct_arm_avic_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_arm_coprocessor_interface_t_type_p;
};

%init (python) {
#ifdef struct_arm_coprocessor_interface_t_type_p
	struct_arm_coprocessor_interface_t_type_p = VT_lookup_python_opaque_type("api.arm_coprocessor_interface_t");
#endif /* struct_arm_coprocessor_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_arm_cpu_group_event_interface_t_type_p;
};

%init (python) {
#ifdef struct_arm_cpu_group_event_interface_t_type_p
	struct_arm_cpu_group_event_interface_t_type_p = VT_lookup_python_opaque_type("api.arm_cpu_group_event_interface_t");
#endif /* struct_arm_cpu_group_event_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_arm_cpu_group_exclusive_interface_t_type_p;
};

%init (python) {
#ifdef struct_arm_cpu_group_exclusive_interface_t_type_p
	struct_arm_cpu_group_exclusive_interface_t_type_p = VT_lookup_python_opaque_type("api.arm_cpu_group_exclusive_interface_t");
#endif /* struct_arm_cpu_group_exclusive_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_arm_cpu_group_tlb_interface_t_type_p;
};

%init (python) {
#ifdef struct_arm_cpu_group_tlb_interface_t_type_p
	struct_arm_cpu_group_tlb_interface_t_type_p = VT_lookup_python_opaque_type("api.arm_cpu_group_tlb_interface_t");
#endif /* struct_arm_cpu_group_tlb_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_arm_external_debug_interface_t_type_p;
};

%init (python) {
#ifdef struct_arm_external_debug_interface_t_type_p
	struct_arm_external_debug_interface_t_type_p = VT_lookup_python_opaque_type("api.arm_external_debug_interface_t");
#endif /* struct_arm_external_debug_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_arm_gic_interface_t_type_p;
};

%init (python) {
#ifdef struct_arm_gic_interface_t_type_p
	struct_arm_gic_interface_t_type_p = VT_lookup_python_opaque_type("api.arm_gic_interface_t");
#endif /* struct_arm_gic_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_arm_cpu_state_t_type_p;
};

%init (python) {
#ifdef struct_arm_cpu_state_t_type_p
	struct_arm_cpu_state_t_type_p = VT_lookup_python_opaque_type("api.arm_cpu_state_t");
#endif /* struct_arm_cpu_state_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_gic_reg_info_t_type_p;
};

%init (python) {
#ifdef struct_gic_reg_info_t_type_p
	struct_gic_reg_info_t_type_p = VT_lookup_python_opaque_type("api.gic_reg_info_t");
#endif /* struct_gic_reg_info_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_arm_gic_cpu_state_interface_t_type_p;
};

%init (python) {
#ifdef struct_arm_gic_cpu_state_interface_t_type_p
	struct_arm_gic_cpu_state_interface_t_type_p = VT_lookup_python_opaque_type("api.arm_gic_cpu_state_interface_t");
#endif /* struct_arm_gic_cpu_state_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_arm_trustzone_interface_t_type_p;
};

%init (python) {
#ifdef struct_arm_trustzone_interface_t_type_p
	struct_arm_trustzone_interface_t_type_p = VT_lookup_python_opaque_type("api.arm_trustzone_interface_t");
#endif /* struct_arm_trustzone_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_bank_after_read_interface_t_type_p;
};

%init (python) {
#ifdef struct_bank_after_read_interface_t_type_p
	struct_bank_after_read_interface_t_type_p = VT_lookup_python_opaque_type("api.bank_after_read_interface_t");
#endif /* struct_bank_after_read_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_bank_access_t_type_p;
};

%init (python) {
#ifdef struct_bank_access_t_type_p
	struct_bank_access_t_type_p = VT_lookup_python_opaque_type("api.bank_access_t");
#endif /* struct_bank_access_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_bank_after_write_interface_t_type_p;
};

%init (python) {
#ifdef struct_bank_after_write_interface_t_type_p
	struct_bank_after_write_interface_t_type_p = VT_lookup_python_opaque_type("api.bank_after_write_interface_t");
#endif /* struct_bank_after_write_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_bank_before_read_interface_t_type_p;
};

%init (python) {
#ifdef struct_bank_before_read_interface_t_type_p
	struct_bank_before_read_interface_t_type_p = VT_lookup_python_opaque_type("api.bank_before_read_interface_t");
#endif /* struct_bank_before_read_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_bank_before_write_interface_t_type_p;
};

%init (python) {
#ifdef struct_bank_before_write_interface_t_type_p
	struct_bank_before_write_interface_t_type_p = VT_lookup_python_opaque_type("api.bank_before_write_interface_t");
#endif /* struct_bank_before_write_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_bank_instrumentation_subscribe_interface_t_type_p;
};

%init (python) {
#ifdef struct_bank_instrumentation_subscribe_interface_t_type_p
	struct_bank_instrumentation_subscribe_interface_t_type_p = VT_lookup_python_opaque_type("api.bank_instrumentation_subscribe_interface_t");
#endif /* struct_bank_instrumentation_subscribe_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_bp_manager_interface_t_type_p;
};

%init (python) {
#ifdef struct_bp_manager_interface_t_type_p
	struct_bp_manager_interface_t_type_p = VT_lookup_python_opaque_type("api.bp_manager_interface_t");
#endif /* struct_bp_manager_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_branch_arc_interface_t_type_p;
};

%init (python) {
#ifdef struct_branch_arc_interface_t_type_p
	struct_branch_arc_interface_t_type_p = VT_lookup_python_opaque_type("api.branch_arc_interface_t");
#endif /* struct_branch_arc_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_branch_arc_iter_t_type_p;
};

%init (python) {
#ifdef struct_branch_arc_iter_t_type_p
	struct_branch_arc_iter_t_type_p = VT_lookup_python_opaque_type("api.branch_arc_iter_t");
#endif /* struct_branch_arc_iter_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_branch_arc_t_type_p;
};

%init (python) {
#ifdef struct_branch_arc_t_type_p
	struct_branch_arc_t_type_p = VT_lookup_python_opaque_type("api.branch_arc_t");
#endif /* struct_branch_arc_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_branch_recorder_handler_interface_t_type_p;
};

%init (python) {
#ifdef struct_branch_recorder_handler_interface_t_type_p
	struct_branch_recorder_handler_interface_t_type_p = VT_lookup_python_opaque_type("api.branch_recorder_handler_interface_t");
#endif /* struct_branch_recorder_handler_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_break_strings_interface_t_type_p;
};

%init (python) {
#ifdef struct_break_strings_interface_t_type_p
	struct_break_strings_interface_t_type_p = VT_lookup_python_opaque_type("api.break_strings_interface_t");
#endif /* struct_break_strings_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_break_strings_v2_interface_t_type_p;
};

%init (python) {
#ifdef struct_break_strings_v2_interface_t_type_p
	struct_break_strings_v2_interface_t_type_p = VT_lookup_python_opaque_type("api.break_strings_v2_interface_t");
#endif /* struct_break_strings_v2_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_breakpoint_interface_t_type_p;
};

%init (python) {
#ifdef struct_breakpoint_interface_t_type_p
	struct_breakpoint_interface_t_type_p = VT_lookup_python_opaque_type("api.breakpoint_interface_t");
#endif /* struct_breakpoint_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_breakpoint_info_t_type_p;
};

%init (python) {
#ifdef struct_breakpoint_info_t_type_p
	struct_breakpoint_info_t_type_p = VT_lookup_python_opaque_type("api.breakpoint_info_t");
#endif /* struct_breakpoint_info_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_breakpoint_change_interface_t_type_p;
};

%init (python) {
#ifdef struct_breakpoint_change_interface_t_type_p
	struct_breakpoint_change_interface_t_type_p = VT_lookup_python_opaque_type("api.breakpoint_change_interface_t");
#endif /* struct_breakpoint_change_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_breakpoint_manager_interface_t_type_p;
};

%init (python) {
#ifdef struct_breakpoint_manager_interface_t_type_p
	struct_breakpoint_manager_interface_t_type_p = VT_lookup_python_opaque_type("api.breakpoint_manager_interface_t");
#endif /* struct_breakpoint_manager_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_breakpoint_query_interface_t_type_p;
};

%init (python) {
#ifdef struct_breakpoint_query_interface_t_type_p
	struct_breakpoint_query_interface_t_type_p = VT_lookup_python_opaque_type("api.breakpoint_query_interface_t");
#endif /* struct_breakpoint_query_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_breakpoint_query_v2_interface_t_type_p;
};

%init (python) {
#ifdef struct_breakpoint_query_v2_interface_t_type_p
	struct_breakpoint_query_v2_interface_t_type_p = VT_lookup_python_opaque_type("api.breakpoint_query_v2_interface_t");
#endif /* struct_breakpoint_query_v2_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_breakpoint_registration_interface_t_type_p;
};

%init (python) {
#ifdef struct_breakpoint_registration_interface_t_type_p
	struct_breakpoint_registration_interface_t_type_p = VT_lookup_python_opaque_type("api.breakpoint_registration_interface_t");
#endif /* struct_breakpoint_registration_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_breakpoint_type_interface_t_type_p;
};

%init (python) {
#ifdef struct_breakpoint_type_interface_t_type_p
	struct_breakpoint_type_interface_t_type_p = VT_lookup_python_opaque_type("api.breakpoint_type_interface_t");
#endif /* struct_breakpoint_type_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_breakpoint_type_provider_interface_t_type_p;
};

%init (python) {
#ifdef struct_breakpoint_type_provider_interface_t_type_p
	struct_breakpoint_type_provider_interface_t_type_p = VT_lookup_python_opaque_type("api.breakpoint_type_provider_interface_t");
#endif /* struct_breakpoint_type_provider_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_bridge_interface_t_type_p;
};

%init (python) {
#ifdef struct_bridge_interface_t_type_p
	struct_bridge_interface_t_type_p = VT_lookup_python_opaque_type("api.bridge_interface_t");
#endif /* struct_bridge_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_map_info_t_type_p;
};

%init (python) {
#ifdef struct_map_info_t_type_p
	struct_map_info_t_type_p = VT_lookup_python_opaque_type("api.map_info_t");
#endif /* struct_map_info_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_cache_control_interface_t_type_p;
};

%init (python) {
#ifdef struct_cache_control_interface_t_type_p
	struct_cache_control_interface_t_type_p = VT_lookup_python_opaque_type("api.cache_control_interface_t");
#endif /* struct_cache_control_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_callback_info_interface_t_type_p;
};

%init (python) {
#ifdef struct_callback_info_interface_t_type_p
	struct_callback_info_interface_t_type_p = VT_lookup_python_opaque_type("api.callback_info_interface_t");
#endif /* struct_callback_info_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_cdrom_media_interface_t_type_p;
};

%init (python) {
#ifdef struct_cdrom_media_interface_t_type_p
	struct_cdrom_media_interface_t_type_p = VT_lookup_python_opaque_type("api.cdrom_media_interface_t");
#endif /* struct_cdrom_media_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_cell_inspection_interface_t_type_p;
};

%init (python) {
#ifdef struct_cell_inspection_interface_t_type_p
	struct_cell_inspection_interface_t_type_p = VT_lookup_python_opaque_type("api.cell_inspection_interface_t");
#endif /* struct_cell_inspection_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_checkpoint_interface_t_type_p;
};

%init (python) {
#ifdef struct_checkpoint_interface_t_type_p
	struct_checkpoint_interface_t_type_p = VT_lookup_python_opaque_type("api.checkpoint_interface_t");
#endif /* struct_checkpoint_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_class_disassembly_interface_t_type_p;
};

%init (python) {
#ifdef struct_class_disassembly_interface_t_type_p
	struct_class_disassembly_interface_t_type_p = VT_lookup_python_opaque_type("api.class_disassembly_interface_t");
#endif /* struct_class_disassembly_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_disassembly_instruction_t_type_p;
};

%init (python) {
#ifdef struct_disassembly_instruction_t_type_p
	struct_disassembly_instruction_t_type_p = VT_lookup_python_opaque_type("api.disassembly_instruction_t");
#endif /* struct_disassembly_instruction_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_disassembly_mode_info_t_type_p;
};

%init (python) {
#ifdef struct_disassembly_mode_info_t_type_p
	struct_disassembly_mode_info_t_type_p = VT_lookup_python_opaque_type("api.disassembly_mode_info_t");
#endif /* struct_disassembly_mode_info_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_cmd_line_frontend_interface_t_type_p;
};

%init (python) {
#ifdef struct_cmd_line_frontend_interface_t_type_p
	struct_cmd_line_frontend_interface_t_type_p = VT_lookup_python_opaque_type("api.cmd_line_frontend_interface_t");
#endif /* struct_cmd_line_frontend_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_cmd_line_selection_interface_t_type_p;
};

%init (python) {
#ifdef struct_cmd_line_selection_interface_t_type_p
	struct_cmd_line_selection_interface_t_type_p = VT_lookup_python_opaque_type("api.cmd_line_selection_interface_t");
#endif /* struct_cmd_line_selection_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_co_execute_interface_t_type_p;
};

%init (python) {
#ifdef struct_co_execute_interface_t_type_p
	struct_co_execute_interface_t_type_p = VT_lookup_python_opaque_type("api.co_execute_interface_t");
#endif /* struct_co_execute_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_component_interface_t_type_p;
};

%init (python) {
#ifdef struct_component_interface_t_type_p
	struct_component_interface_t_type_p = VT_lookup_python_opaque_type("api.component_interface_t");
#endif /* struct_component_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_component_connector_interface_t_type_p;
};

%init (python) {
#ifdef struct_component_connector_interface_t_type_p
	struct_component_connector_interface_t_type_p = VT_lookup_python_opaque_type("api.component_connector_interface_t");
#endif /* struct_component_connector_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_con_input_interface_t_type_p;
};

%init (python) {
#ifdef struct_con_input_interface_t_type_p
	struct_con_input_interface_t_type_p = VT_lookup_python_opaque_type("api.con_input_interface_t");
#endif /* struct_con_input_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_con_input_code_interface_t_type_p;
};

%init (python) {
#ifdef struct_con_input_code_interface_t_type_p
	struct_con_input_code_interface_t_type_p = VT_lookup_python_opaque_type("api.con_input_code_interface_t");
#endif /* struct_con_input_code_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_concurrency_group_interface_t_type_p;
};

%init (python) {
#ifdef struct_concurrency_group_interface_t_type_p
	struct_concurrency_group_interface_t_type_p = VT_lookup_python_opaque_type("api.concurrency_group_interface_t");
#endif /* struct_concurrency_group_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_concurrency_mode_interface_t_type_p;
};

%init (python) {
#ifdef struct_concurrency_mode_interface_t_type_p
	struct_concurrency_mode_interface_t_type_p = VT_lookup_python_opaque_type("api.concurrency_mode_interface_t");
#endif /* struct_concurrency_mode_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_connector_interface_t_type_p;
};

%init (python) {
#ifdef struct_connector_interface_t_type_p
	struct_connector_interface_t_type_p = VT_lookup_python_opaque_type("api.connector_interface_t");
#endif /* struct_connector_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_context_handler_interface_t_type_p;
};

%init (python) {
#ifdef struct_context_handler_interface_t_type_p
	struct_context_handler_interface_t_type_p = VT_lookup_python_opaque_type("api.context_handler_interface_t");
#endif /* struct_context_handler_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_coreint_interface_t_type_p;
};

%init (python) {
#ifdef struct_coreint_interface_t_type_p
	struct_coreint_interface_t_type_p = VT_lookup_python_opaque_type("api.coreint_interface_t");
#endif /* struct_coreint_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_coreint_reply_t_type_p;
};

%init (python) {
#ifdef struct_coreint_reply_t_type_p
	struct_coreint_reply_t_type_p = VT_lookup_python_opaque_type("api.coreint_reply_t");
#endif /* struct_coreint_reply_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_cpu_cached_instruction_interface_t_type_p;
};

%init (python) {
#ifdef struct_cpu_cached_instruction_interface_t_type_p
	struct_cpu_cached_instruction_interface_t_type_p = VT_lookup_python_opaque_type("api.cpu_cached_instruction_interface_t");
#endif /* struct_cpu_cached_instruction_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_cached_instruction_handle_t_type_p;
};

%init (python) {
#ifdef struct_cached_instruction_handle_t_type_p
	struct_cached_instruction_handle_t_type_p = VT_lookup_python_opaque_type("api.cached_instruction_handle_t");
#endif /* struct_cached_instruction_handle_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_instruction_handle_t_type_p;
};

%init (python) {
#ifdef struct_instruction_handle_t_type_p
	struct_instruction_handle_t_type_p = VT_lookup_python_opaque_type("api.instruction_handle_t");
#endif /* struct_instruction_handle_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_memory_handle_t_type_p;
};

%init (python) {
#ifdef struct_memory_handle_t_type_p
	struct_memory_handle_t_type_p = VT_lookup_python_opaque_type("api.memory_handle_t");
#endif /* struct_memory_handle_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_cpu_cached_instruction_once_interface_t_type_p;
};

%init (python) {
#ifdef struct_cpu_cached_instruction_once_interface_t_type_p
	struct_cpu_cached_instruction_once_interface_t_type_p = VT_lookup_python_opaque_type("api.cpu_cached_instruction_once_interface_t");
#endif /* struct_cpu_cached_instruction_once_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_cpu_exception_query_interface_t_type_p;
};

%init (python) {
#ifdef struct_cpu_exception_query_interface_t_type_p
	struct_cpu_exception_query_interface_t_type_p = VT_lookup_python_opaque_type("api.cpu_exception_query_interface_t");
#endif /* struct_cpu_exception_query_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_exception_handle_t_type_p;
};

%init (python) {
#ifdef struct_exception_handle_t_type_p
	struct_exception_handle_t_type_p = VT_lookup_python_opaque_type("api.exception_handle_t");
#endif /* struct_exception_handle_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_cpu_instruction_decoder_interface_t_type_p;
};

%init (python) {
#ifdef struct_cpu_instruction_decoder_interface_t_type_p
	struct_cpu_instruction_decoder_interface_t_type_p = VT_lookup_python_opaque_type("api.cpu_instruction_decoder_interface_t");
#endif /* struct_cpu_instruction_decoder_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_decoder_handle_t_type_p;
};

%init (python) {
#ifdef struct_decoder_handle_t_type_p
	struct_decoder_handle_t_type_p = VT_lookup_python_opaque_type("api.decoder_handle_t");
#endif /* struct_decoder_handle_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_cpu_instruction_query_interface_t_type_p;
};

%init (python) {
#ifdef struct_cpu_instruction_query_interface_t_type_p
	struct_cpu_instruction_query_interface_t_type_p = VT_lookup_python_opaque_type("api.cpu_instruction_query_interface_t");
#endif /* struct_cpu_instruction_query_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_cpu_instrumentation_subscribe_interface_t_type_p;
};

%init (python) {
#ifdef struct_cpu_instrumentation_subscribe_interface_t_type_p
	struct_cpu_instrumentation_subscribe_interface_t_type_p = VT_lookup_python_opaque_type("api.cpu_instrumentation_subscribe_interface_t");
#endif /* struct_cpu_instrumentation_subscribe_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct__cpu_instrumentation_entry_t_type_p;
};

%init (python) {
#ifdef struct__cpu_instrumentation_entry_t_type_p
	struct__cpu_instrumentation_entry_t_type_p = VT_lookup_python_opaque_type("api._cpu_instrumentation_entry_t");
#endif /* struct__cpu_instrumentation_entry_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_address_handle_t_type_p;
};

%init (python) {
#ifdef struct_address_handle_t_type_p
	struct_address_handle_t_type_p = VT_lookup_python_opaque_type("api.address_handle_t");
#endif /* struct_address_handle_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_exception_return_handle_t_type_p;
};

%init (python) {
#ifdef struct_exception_return_handle_t_type_p
	struct_exception_return_handle_t_type_p = VT_lookup_python_opaque_type("api.exception_return_handle_t");
#endif /* struct_exception_return_handle_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_cpu_memory_query_interface_t_type_p;
};

%init (python) {
#ifdef struct_cpu_memory_query_interface_t_type_p
	struct_cpu_memory_query_interface_t_type_p = VT_lookup_python_opaque_type("api.cpu_memory_query_interface_t");
#endif /* struct_cpu_memory_query_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_cxl_map_interface_t_type_p;
};

%init (python) {
#ifdef struct_cxl_map_interface_t_type_p
	struct_cxl_map_interface_t_type_p = VT_lookup_python_opaque_type("api.cxl_map_interface_t");
#endif /* struct_cxl_map_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_cxl_mem_downstream_port_managing_interface_t_type_p;
};

%init (python) {
#ifdef struct_cxl_mem_downstream_port_managing_interface_t_type_p
	struct_cxl_mem_downstream_port_managing_interface_t_type_p = VT_lookup_python_opaque_type("api.cxl_mem_downstream_port_managing_interface_t");
#endif /* struct_cxl_mem_downstream_port_managing_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_cxl_non_device_decoder_handling_interface_t_type_p;
};

%init (python) {
#ifdef struct_cxl_non_device_decoder_handling_interface_t_type_p
	struct_cxl_non_device_decoder_handling_interface_t_type_p = VT_lookup_python_opaque_type("api.cxl_non_device_decoder_handling_interface_t");
#endif /* struct_cxl_non_device_decoder_handling_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_cycle_interface_t_type_p;
};

%init (python) {
#ifdef struct_cycle_interface_t_type_p
	struct_cycle_interface_t_type_p = VT_lookup_python_opaque_type("api.cycle_interface_t");
#endif /* struct_cycle_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_event_class_t_type_p;
};

%init (python) {
#ifdef struct_event_class_t_type_p
	struct_event_class_t_type_p = VT_lookup_python_opaque_type("api.event_class_t");
#endif /* struct_event_class_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_local_time_t_type_p;
};

%init (python) {
#ifdef struct_local_time_t_type_p
	struct_local_time_t_type_p = VT_lookup_python_opaque_type("api.local_time_t");
#endif /* struct_local_time_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_duration_t_type_p;
};

%init (python) {
#ifdef struct_duration_t_type_p
	struct_duration_t_type_p = VT_lookup_python_opaque_type("api.duration_t");
#endif /* struct_duration_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_cycle_control_interface_t_type_p;
};

%init (python) {
#ifdef struct_cycle_control_interface_t_type_p
	struct_cycle_control_interface_t_type_p = VT_lookup_python_opaque_type("api.cycle_control_interface_t");
#endif /* struct_cycle_control_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_cycle_event_interface_t_type_p;
};

%init (python) {
#ifdef struct_cycle_event_interface_t_type_p
	struct_cycle_event_interface_t_type_p = VT_lookup_python_opaque_type("api.cycle_event_interface_t");
#endif /* struct_cycle_event_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_cycle_event_instrumentation_interface_t_type_p;
};

%init (python) {
#ifdef struct_cycle_event_instrumentation_interface_t_type_p
	struct_cycle_event_instrumentation_interface_t_type_p = VT_lookup_python_opaque_type("api.cycle_event_instrumentation_interface_t");
#endif /* struct_cycle_event_instrumentation_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_cycle_handle_t_type_p;
};

%init (python) {
#ifdef struct_cycle_handle_t_type_p
	struct_cycle_handle_t_type_p = VT_lookup_python_opaque_type("api.cycle_handle_t");
#endif /* struct_cycle_handle_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_data_profiler_interface_t_type_p;
};

%init (python) {
#ifdef struct_data_profiler_interface_t_type_p
	struct_data_profiler_interface_t_type_p = VT_lookup_python_opaque_type("api.data_profiler_interface_t");
#endif /* struct_data_profiler_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_datagram_link_interface_t_type_p;
};

%init (python) {
#ifdef struct_datagram_link_interface_t_type_p
	struct_datagram_link_interface_t_type_p = VT_lookup_python_opaque_type("api.datagram_link_interface_t");
#endif /* struct_datagram_link_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_debug_notification_interface_t_type_p;
};

%init (python) {
#ifdef struct_debug_notification_interface_t_type_p
	struct_debug_notification_interface_t_type_p = VT_lookup_python_opaque_type("api.debug_notification_interface_t");
#endif /* struct_debug_notification_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_debug_query_interface_t_type_p;
};

%init (python) {
#ifdef struct_debug_query_interface_t_type_p
	struct_debug_query_interface_t_type_p = VT_lookup_python_opaque_type("api.debug_query_interface_t");
#endif /* struct_debug_query_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_debug_setup_interface_t_type_p;
};

%init (python) {
#ifdef struct_debug_setup_interface_t_type_p
	struct_debug_setup_interface_t_type_p = VT_lookup_python_opaque_type("api.debug_setup_interface_t");
#endif /* struct_debug_setup_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_debug_step_interface_t_type_p;
};

%init (python) {
#ifdef struct_debug_step_interface_t_type_p
	struct_debug_step_interface_t_type_p = VT_lookup_python_opaque_type("api.debug_step_interface_t");
#endif /* struct_debug_step_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_debug_symbol_interface_t_type_p;
};

%init (python) {
#ifdef struct_debug_symbol_interface_t_type_p
	struct_debug_symbol_interface_t_type_p = VT_lookup_python_opaque_type("api.debug_symbol_interface_t");
#endif /* struct_debug_symbol_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_debug_symbol_file_interface_t_type_p;
};

%init (python) {
#ifdef struct_debug_symbol_file_interface_t_type_p
	struct_debug_symbol_file_interface_t_type_p = VT_lookup_python_opaque_type("api.debug_symbol_file_interface_t");
#endif /* struct_debug_symbol_file_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_device_identification_interface_t_type_p;
};

%init (python) {
#ifdef struct_device_identification_interface_t_type_p
	struct_device_identification_interface_t_type_p = VT_lookup_python_opaque_type("api.device_identification_interface_t");
#endif /* struct_device_identification_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_direct_memory_interface_t_type_p;
};

%init (python) {
#ifdef struct_direct_memory_interface_t_type_p
	struct_direct_memory_interface_t_type_p = VT_lookup_python_opaque_type("api.direct_memory_interface_t");
#endif /* struct_direct_memory_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_granted_mem_t_type_p;
};

%init (python) {
#ifdef struct_granted_mem_t_type_p
	struct_granted_mem_t_type_p = VT_lookup_python_opaque_type("api.granted_mem_t");
#endif /* struct_granted_mem_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_direct_memory_t_type_p;
};

%init (python) {
#ifdef struct_direct_memory_t_type_p
	struct_direct_memory_t_type_p = VT_lookup_python_opaque_type("api.direct_memory_t");
#endif /* struct_direct_memory_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_direct_memory_flush_interface_t_type_p;
};

%init (python) {
#ifdef struct_direct_memory_flush_interface_t_type_p
	struct_direct_memory_flush_interface_t_type_p = VT_lookup_python_opaque_type("api.direct_memory_flush_interface_t");
#endif /* struct_direct_memory_flush_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_direct_memory_lookup_interface_t_type_p;
};

%init (python) {
#ifdef struct_direct_memory_lookup_interface_t_type_p
	struct_direct_memory_lookup_interface_t_type_p = VT_lookup_python_opaque_type("api.direct_memory_lookup_interface_t");
#endif /* struct_direct_memory_lookup_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_direct_memory_lookup_t_type_p;
};

%init (python) {
#ifdef struct_direct_memory_lookup_t_type_p
	struct_direct_memory_lookup_t_type_p = VT_lookup_python_opaque_type("api.direct_memory_lookup_t");
#endif /* struct_direct_memory_lookup_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_direct_memory_lookup_v2_interface_t_type_p;
};

%init (python) {
#ifdef struct_direct_memory_lookup_v2_interface_t_type_p
	struct_direct_memory_lookup_v2_interface_t_type_p = VT_lookup_python_opaque_type("api.direct_memory_lookup_v2_interface_t");
#endif /* struct_direct_memory_lookup_v2_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_direct_memory_tags_interface_t_type_p;
};

%init (python) {
#ifdef struct_direct_memory_tags_interface_t_type_p
	struct_direct_memory_tags_interface_t_type_p = VT_lookup_python_opaque_type("api.direct_memory_tags_interface_t");
#endif /* struct_direct_memory_tags_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_direct_memory_tags_t_type_p;
};

%init (python) {
#ifdef struct_direct_memory_tags_t_type_p
	struct_direct_memory_tags_t_type_p = VT_lookup_python_opaque_type("api.direct_memory_tags_t");
#endif /* struct_direct_memory_tags_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_direct_memory_update_interface_t_type_p;
};

%init (python) {
#ifdef struct_direct_memory_update_interface_t_type_p
	struct_direct_memory_update_interface_t_type_p = VT_lookup_python_opaque_type("api.direct_memory_update_interface_t");
#endif /* struct_direct_memory_update_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_disk_component_interface_t_type_p;
};

%init (python) {
#ifdef struct_disk_component_interface_t_type_p
	struct_disk_component_interface_t_type_p = VT_lookup_python_opaque_type("api.disk_component_interface_t");
#endif /* struct_disk_component_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_dist_control_interface_t_type_p;
};

%init (python) {
#ifdef struct_dist_control_interface_t_type_p
	struct_dist_control_interface_t_type_p = VT_lookup_python_opaque_type("api.dist_control_interface_t");
#endif /* struct_dist_control_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_dist_control_node_interface_t_type_p;
};

%init (python) {
#ifdef struct_dist_control_node_interface_t_type_p
	struct_dist_control_node_interface_t_type_p = VT_lookup_python_opaque_type("api.dist_control_node_interface_t");
#endif /* struct_dist_control_node_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_dummy_memory_page_interface_t_type_p;
};

%init (python) {
#ifdef struct_dummy_memory_page_interface_t_type_p
	struct_dummy_memory_page_interface_t_type_p = VT_lookup_python_opaque_type("api.dummy_memory_page_interface_t");
#endif /* struct_dummy_memory_page_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_ethernet_cable_interface_t_type_p;
};

%init (python) {
#ifdef struct_ethernet_cable_interface_t_type_p
	struct_ethernet_cable_interface_t_type_p = VT_lookup_python_opaque_type("api.ethernet_cable_interface_t");
#endif /* struct_ethernet_cable_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_ethernet_common_interface_t_type_p;
};

%init (python) {
#ifdef struct_ethernet_common_interface_t_type_p
	struct_ethernet_common_interface_t_type_p = VT_lookup_python_opaque_type("api.ethernet_common_interface_t");
#endif /* struct_ethernet_common_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_ethernet_probe_interface_t_type_p;
};

%init (python) {
#ifdef struct_ethernet_probe_interface_t_type_p
	struct_ethernet_probe_interface_t_type_p = VT_lookup_python_opaque_type("api.ethernet_probe_interface_t");
#endif /* struct_ethernet_probe_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_ethernet_snoop_interface_t_type_p;
};

%init (python) {
#ifdef struct_ethernet_snoop_interface_t_type_p
	struct_ethernet_snoop_interface_t_type_p = VT_lookup_python_opaque_type("api.ethernet_snoop_interface_t");
#endif /* struct_ethernet_snoop_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_ethernet_vlan_snoop_interface_t_type_p;
};

%init (python) {
#ifdef struct_ethernet_vlan_snoop_interface_t_type_p
	struct_ethernet_vlan_snoop_interface_t_type_p = VT_lookup_python_opaque_type("api.ethernet_vlan_snoop_interface_t");
#endif /* struct_ethernet_vlan_snoop_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_event_delta_interface_t_type_p;
};

%init (python) {
#ifdef struct_event_delta_interface_t_type_p
	struct_event_delta_interface_t_type_p = VT_lookup_python_opaque_type("api.event_delta_interface_t");
#endif /* struct_event_delta_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_event_handler_interface_t_type_p;
};

%init (python) {
#ifdef struct_event_handler_interface_t_type_p
	struct_event_handler_interface_t_type_p = VT_lookup_python_opaque_type("api.event_handler_interface_t");
#endif /* struct_event_handler_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_exception_interface_t_type_p;
};

%init (python) {
#ifdef struct_exception_interface_t_type_p
	struct_exception_interface_t_type_p = VT_lookup_python_opaque_type("api.exception_interface_t");
#endif /* struct_exception_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_exec_trace_interface_t_type_p;
};

%init (python) {
#ifdef struct_exec_trace_interface_t_type_p
	struct_exec_trace_interface_t_type_p = VT_lookup_python_opaque_type("api.exec_trace_interface_t");
#endif /* struct_exec_trace_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_execute_interface_t_type_p;
};

%init (python) {
#ifdef struct_execute_interface_t_type_p
	struct_execute_interface_t_type_p = VT_lookup_python_opaque_type("api.execute_interface_t");
#endif /* struct_execute_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_execute_control_interface_t_type_p;
};

%init (python) {
#ifdef struct_execute_control_interface_t_type_p
	struct_execute_control_interface_t_type_p = VT_lookup_python_opaque_type("api.execute_control_interface_t");
#endif /* struct_execute_control_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_extended_serial_interface_t_type_p;
};

%init (python) {
#ifdef struct_extended_serial_interface_t_type_p
	struct_extended_serial_interface_t_type_p = VT_lookup_python_opaque_type("api.extended_serial_interface_t");
#endif /* struct_extended_serial_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_external_connection_ctl_interface_t_type_p;
};

%init (python) {
#ifdef struct_external_connection_ctl_interface_t_type_p
	struct_external_connection_ctl_interface_t_type_p = VT_lookup_python_opaque_type("api.external_connection_ctl_interface_t");
#endif /* struct_external_connection_ctl_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_external_connection_events_interface_t_type_p;
};

%init (python) {
#ifdef struct_external_connection_events_interface_t_type_p
	struct_external_connection_events_interface_t_type_p = VT_lookup_python_opaque_type("api.external_connection_events_interface_t");
#endif /* struct_external_connection_events_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_firewire_bus_interface_t_type_p;
};

%init (python) {
#ifdef struct_firewire_bus_interface_t_type_p
	struct_firewire_bus_interface_t_type_p = VT_lookup_python_opaque_type("api.firewire_bus_interface_t");
#endif /* struct_firewire_bus_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_firewire_device_interface_t_type_p;
};

%init (python) {
#ifdef struct_firewire_device_interface_t_type_p
	struct_firewire_device_interface_t_type_p = VT_lookup_python_opaque_type("api.firewire_device_interface_t");
#endif /* struct_firewire_device_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_fmn_station_control_interface_t_type_p;
};

%init (python) {
#ifdef struct_fmn_station_control_interface_t_type_p
	struct_fmn_station_control_interface_t_type_p = VT_lookup_python_opaque_type("api.fmn_station_control_interface_t");
#endif /* struct_fmn_station_control_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_fmn_station_control_v2_interface_t_type_p;
};

%init (python) {
#ifdef struct_fmn_station_control_v2_interface_t_type_p
	struct_fmn_station_control_v2_interface_t_type_p = VT_lookup_python_opaque_type("api.fmn_station_control_v2_interface_t");
#endif /* struct_fmn_station_control_v2_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_follower_agent_interface_t_type_p;
};

%init (python) {
#ifdef struct_follower_agent_interface_t_type_p
	struct_follower_agent_interface_t_type_p = VT_lookup_python_opaque_type("api.follower_agent_interface_t");
#endif /* struct_follower_agent_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_forward_destination_rewriter_interface_t_type_p;
};

%init (python) {
#ifdef struct_forward_destination_rewriter_interface_t_type_p
	struct_forward_destination_rewriter_interface_t_type_p = VT_lookup_python_opaque_type("api.forward_destination_rewriter_interface_t");
#endif /* struct_forward_destination_rewriter_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_forward_rewrite_endpoint_t_type_p;
};

%init (python) {
#ifdef struct_forward_rewrite_endpoint_t_type_p
	struct_forward_rewrite_endpoint_t_type_p = VT_lookup_python_opaque_type("api.forward_rewrite_endpoint_t");
#endif /* struct_forward_rewrite_endpoint_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_freerun_interface_t_type_p;
};

%init (python) {
#ifdef struct_freerun_interface_t_type_p
	struct_freerun_interface_t_type_p = VT_lookup_python_opaque_type("api.freerun_interface_t");
#endif /* struct_freerun_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_frequency_interface_t_type_p;
};

%init (python) {
#ifdef struct_frequency_interface_t_type_p
	struct_frequency_interface_t_type_p = VT_lookup_python_opaque_type("api.frequency_interface_t");
#endif /* struct_frequency_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_frequency_listener_interface_t_type_p;
};

%init (python) {
#ifdef struct_frequency_listener_interface_t_type_p
	struct_frequency_listener_interface_t_type_p = VT_lookup_python_opaque_type("api.frequency_listener_interface_t");
#endif /* struct_frequency_listener_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_gbic_transceiver_interface_t_type_p;
};

%init (python) {
#ifdef struct_gbic_transceiver_interface_t_type_p
	struct_gbic_transceiver_interface_t_type_p = VT_lookup_python_opaque_type("api.gbic_transceiver_interface_t");
#endif /* struct_gbic_transceiver_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_gfx_break_interface_t_type_p;
};

%init (python) {
#ifdef struct_gfx_break_interface_t_type_p
	struct_gfx_break_interface_t_type_p = VT_lookup_python_opaque_type("api.gfx_break_interface_t");
#endif /* struct_gfx_break_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_gfx_con_interface_t_type_p;
};

%init (python) {
#ifdef struct_gfx_con_interface_t_type_p
	struct_gfx_con_interface_t_type_p = VT_lookup_python_opaque_type("api.gfx_con_interface_t");
#endif /* struct_gfx_con_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_gfx_console_backend_interface_t_type_p;
};

%init (python) {
#ifdef struct_gfx_console_backend_interface_t_type_p
	struct_gfx_console_backend_interface_t_type_p = VT_lookup_python_opaque_type("api.gfx_console_backend_interface_t");
#endif /* struct_gfx_console_backend_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_gfx_console_frontend_interface_t_type_p;
};

%init (python) {
#ifdef struct_gfx_console_frontend_interface_t_type_p
	struct_gfx_console_frontend_interface_t_type_p = VT_lookup_python_opaque_type("api.gfx_console_frontend_interface_t");
#endif /* struct_gfx_console_frontend_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_global_time_interface_t_type_p;
};

%init (python) {
#ifdef struct_global_time_interface_t_type_p
	struct_global_time_interface_t_type_p = VT_lookup_python_opaque_type("api.global_time_interface_t");
#endif /* struct_global_time_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_gui_console_backend_interface_t_type_p;
};

%init (python) {
#ifdef struct_gui_console_backend_interface_t_type_p
	struct_gui_console_backend_interface_t_type_p = VT_lookup_python_opaque_type("api.gui_console_backend_interface_t");
#endif /* struct_gui_console_backend_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_host_serial_interface_t_type_p;
};

%init (python) {
#ifdef struct_host_serial_interface_t_type_p
	struct_host_serial_interface_t_type_p = VT_lookup_python_opaque_type("api.host_serial_interface_t");
#endif /* struct_host_serial_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_i2c_bridge_interface_t_type_p;
};

%init (python) {
#ifdef struct_i2c_bridge_interface_t_type_p
	struct_i2c_bridge_interface_t_type_p = VT_lookup_python_opaque_type("api.i2c_bridge_interface_t");
#endif /* struct_i2c_bridge_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_i2c_bus_interface_t_type_p;
};

%init (python) {
#ifdef struct_i2c_bus_interface_t_type_p
	struct_i2c_bus_interface_t_type_p = VT_lookup_python_opaque_type("api.i2c_bus_interface_t");
#endif /* struct_i2c_bus_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_i2c_device_interface_t_type_p;
};

%init (python) {
#ifdef struct_i2c_device_interface_t_type_p
	struct_i2c_device_interface_t_type_p = VT_lookup_python_opaque_type("api.i2c_device_interface_t");
#endif /* struct_i2c_device_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_i2c_link_interface_t_type_p;
};

%init (python) {
#ifdef struct_i2c_link_interface_t_type_p
	struct_i2c_link_interface_t_type_p = VT_lookup_python_opaque_type("api.i2c_link_interface_t");
#endif /* struct_i2c_link_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_i2c_master_interface_t_type_p;
};

%init (python) {
#ifdef struct_i2c_master_interface_t_type_p
	struct_i2c_master_interface_t_type_p = VT_lookup_python_opaque_type("api.i2c_master_interface_t");
#endif /* struct_i2c_master_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_i2c_master_v2_interface_t_type_p;
};

%init (python) {
#ifdef struct_i2c_master_v2_interface_t_type_p
	struct_i2c_master_v2_interface_t_type_p = VT_lookup_python_opaque_type("api.i2c_master_v2_interface_t");
#endif /* struct_i2c_master_v2_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_i2c_slave_interface_t_type_p;
};

%init (python) {
#ifdef struct_i2c_slave_interface_t_type_p
	struct_i2c_slave_interface_t_type_p = VT_lookup_python_opaque_type("api.i2c_slave_interface_t");
#endif /* struct_i2c_slave_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_i2c_slave_v2_interface_t_type_p;
};

%init (python) {
#ifdef struct_i2c_slave_v2_interface_t_type_p
	struct_i2c_slave_v2_interface_t_type_p = VT_lookup_python_opaque_type("api.i2c_slave_v2_interface_t");
#endif /* struct_i2c_slave_v2_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_i3c_daa_snoop_interface_t_type_p;
};

%init (python) {
#ifdef struct_i3c_daa_snoop_interface_t_type_p
	struct_i3c_daa_snoop_interface_t_type_p = VT_lookup_python_opaque_type("api.i3c_daa_snoop_interface_t");
#endif /* struct_i3c_daa_snoop_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_i3c_hdr_master_interface_t_type_p;
};

%init (python) {
#ifdef struct_i3c_hdr_master_interface_t_type_p
	struct_i3c_hdr_master_interface_t_type_p = VT_lookup_python_opaque_type("api.i3c_hdr_master_interface_t");
#endif /* struct_i3c_hdr_master_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_i3c_hdr_slave_interface_t_type_p;
};

%init (python) {
#ifdef struct_i3c_hdr_slave_interface_t_type_p
	struct_i3c_hdr_slave_interface_t_type_p = VT_lookup_python_opaque_type("api.i3c_hdr_slave_interface_t");
#endif /* struct_i3c_hdr_slave_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_i3c_master_interface_t_type_p;
};

%init (python) {
#ifdef struct_i3c_master_interface_t_type_p
	struct_i3c_master_interface_t_type_p = VT_lookup_python_opaque_type("api.i3c_master_interface_t");
#endif /* struct_i3c_master_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_i3c_slave_interface_t_type_p;
};

%init (python) {
#ifdef struct_i3c_slave_interface_t_type_p
	struct_i3c_slave_interface_t_type_p = VT_lookup_python_opaque_type("api.i3c_slave_interface_t");
#endif /* struct_i3c_slave_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_i8051_interrupt_interface_t_type_p;
};

%init (python) {
#ifdef struct_i8051_interrupt_interface_t_type_p
	struct_i8051_interrupt_interface_t_type_p = VT_lookup_python_opaque_type("api.i8051_interrupt_interface_t");
#endif /* struct_i8051_interrupt_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_i8051_timer_interface_t_type_p;
};

%init (python) {
#ifdef struct_i8051_timer_interface_t_type_p
	struct_i8051_timer_interface_t_type_p = VT_lookup_python_opaque_type("api.i8051_timer_interface_t");
#endif /* struct_i8051_timer_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_icode_interface_t_type_p;
};

%init (python) {
#ifdef struct_icode_interface_t_type_p
	struct_icode_interface_t_type_p = VT_lookup_python_opaque_type("api.icode_interface_t");
#endif /* struct_icode_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_ieee_802_3_mac_interface_t_type_p;
};

%init (python) {
#ifdef struct_ieee_802_3_mac_interface_t_type_p
	struct_ieee_802_3_mac_interface_t_type_p = VT_lookup_python_opaque_type("api.ieee_802_3_mac_interface_t");
#endif /* struct_ieee_802_3_mac_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_ieee_802_3_mac_v3_interface_t_type_p;
};

%init (python) {
#ifdef struct_ieee_802_3_mac_v3_interface_t_type_p
	struct_ieee_802_3_mac_v3_interface_t_type_p = VT_lookup_python_opaque_type("api.ieee_802_3_mac_v3_interface_t");
#endif /* struct_ieee_802_3_mac_v3_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_ieee_802_3_phy_interface_t_type_p;
};

%init (python) {
#ifdef struct_ieee_802_3_phy_interface_t_type_p
	struct_ieee_802_3_phy_interface_t_type_p = VT_lookup_python_opaque_type("api.ieee_802_3_phy_interface_t");
#endif /* struct_ieee_802_3_phy_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_ieee_802_3_phy_v2_interface_t_type_p;
};

%init (python) {
#ifdef struct_ieee_802_3_phy_v2_interface_t_type_p
	struct_ieee_802_3_phy_v2_interface_t_type_p = VT_lookup_python_opaque_type("api.ieee_802_3_phy_v2_interface_t");
#endif /* struct_ieee_802_3_phy_v2_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_ieee_802_3_phy_v3_interface_t_type_p;
};

%init (python) {
#ifdef struct_ieee_802_3_phy_v3_interface_t_type_p
	struct_ieee_802_3_phy_v3_interface_t_type_p = VT_lookup_python_opaque_type("api.ieee_802_3_phy_v3_interface_t");
#endif /* struct_ieee_802_3_phy_v3_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_image_interface_t_type_p;
};

%init (python) {
#ifdef struct_image_interface_t_type_p
	struct_image_interface_t_type_p = VT_lookup_python_opaque_type("api.image_interface_t");
#endif /* struct_image_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_instruction_fetch_interface_t_type_p;
};

%init (python) {
#ifdef struct_instruction_fetch_interface_t_type_p
	struct_instruction_fetch_interface_t_type_p = VT_lookup_python_opaque_type("api.instruction_fetch_interface_t");
#endif /* struct_instruction_fetch_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_instrum_registration_interface_t_type_p;
};

%init (python) {
#ifdef struct_instrum_registration_interface_t_type_p
	struct_instrum_registration_interface_t_type_p = VT_lookup_python_opaque_type("api.instrum_registration_interface_t");
#endif /* struct_instrum_registration_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_instrumentation_connection_interface_t_type_p;
};

%init (python) {
#ifdef struct_instrumentation_connection_interface_t_type_p
	struct_instrumentation_connection_interface_t_type_p = VT_lookup_python_opaque_type("api.instrumentation_connection_interface_t");
#endif /* struct_instrumentation_connection_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_instrumentation_filter_master_interface_t_type_p;
};

%init (python) {
#ifdef struct_instrumentation_filter_master_interface_t_type_p
	struct_instrumentation_filter_master_interface_t_type_p = VT_lookup_python_opaque_type("api.instrumentation_filter_master_interface_t");
#endif /* struct_instrumentation_filter_master_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_instrumentation_filter_slave_interface_t_type_p;
};

%init (python) {
#ifdef struct_instrumentation_filter_slave_interface_t_type_p
	struct_instrumentation_filter_slave_interface_t_type_p = VT_lookup_python_opaque_type("api.instrumentation_filter_slave_interface_t");
#endif /* struct_instrumentation_filter_slave_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_instrumentation_filter_status_interface_t_type_p;
};

%init (python) {
#ifdef struct_instrumentation_filter_status_interface_t_type_p
	struct_instrumentation_filter_status_interface_t_type_p = VT_lookup_python_opaque_type("api.instrumentation_filter_status_interface_t");
#endif /* struct_instrumentation_filter_status_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_instrumentation_order_interface_t_type_p;
};

%init (python) {
#ifdef struct_instrumentation_order_interface_t_type_p
	struct_instrumentation_order_interface_t_type_p = VT_lookup_python_opaque_type("api.instrumentation_order_interface_t");
#endif /* struct_instrumentation_order_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_instrumentation_tool_interface_t_type_p;
};

%init (python) {
#ifdef struct_instrumentation_tool_interface_t_type_p
	struct_instrumentation_tool_interface_t_type_p = VT_lookup_python_opaque_type("api.instrumentation_tool_interface_t");
#endif /* struct_instrumentation_tool_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_int_register_interface_t_type_p;
};

%init (python) {
#ifdef struct_int_register_interface_t_type_p
	struct_int_register_interface_t_type_p = VT_lookup_python_opaque_type("api.int_register_interface_t");
#endif /* struct_int_register_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_internal_class_testing_interface_t_type_p;
};

%init (python) {
#ifdef struct_internal_class_testing_interface_t_type_p
	struct_internal_class_testing_interface_t_type_p = VT_lookup_python_opaque_type("api.internal_class_testing_interface_t");
#endif /* struct_internal_class_testing_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_interrupt_ack_interface_t_type_p;
};

%init (python) {
#ifdef struct_interrupt_ack_interface_t_type_p
	struct_interrupt_ack_interface_t_type_p = VT_lookup_python_opaque_type("api.interrupt_ack_interface_t");
#endif /* struct_interrupt_ack_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_interrupt_cpu_interface_t_type_p;
};

%init (python) {
#ifdef struct_interrupt_cpu_interface_t_type_p
	struct_interrupt_cpu_interface_t_type_p = VT_lookup_python_opaque_type("api.interrupt_cpu_interface_t");
#endif /* struct_interrupt_cpu_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_interrupt_subscriber_interface_t_type_p;
};

%init (python) {
#ifdef struct_interrupt_subscriber_interface_t_type_p
	struct_interrupt_subscriber_interface_t_type_p = VT_lookup_python_opaque_type("api.interrupt_subscriber_interface_t");
#endif /* struct_interrupt_subscriber_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_io_memory_interface_t_type_p;
};

%init (python) {
#ifdef struct_io_memory_interface_t_type_p
	struct_io_memory_interface_t_type_p = VT_lookup_python_opaque_type("api.io_memory_interface_t");
#endif /* struct_io_memory_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_jit_control_interface_t_type_p;
};

%init (python) {
#ifdef struct_jit_control_interface_t_type_p
	struct_jit_control_interface_t_type_p = VT_lookup_python_opaque_type("api.jit_control_interface_t");
#endif /* struct_jit_control_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_keyboard_interface_t_type_p;
};

%init (python) {
#ifdef struct_keyboard_interface_t_type_p
	struct_keyboard_interface_t_type_p = VT_lookup_python_opaque_type("api.keyboard_interface_t");
#endif /* struct_keyboard_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_keyboard_console_interface_t_type_p;
};

%init (python) {
#ifdef struct_keyboard_console_interface_t_type_p
	struct_keyboard_console_interface_t_type_p = VT_lookup_python_opaque_type("api.keyboard_console_interface_t");
#endif /* struct_keyboard_console_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_leader_message_interface_t_type_p;
};

%init (python) {
#ifdef struct_leader_message_interface_t_type_p
	struct_leader_message_interface_t_type_p = VT_lookup_python_opaque_type("api.leader_message_interface_t");
#endif /* struct_leader_message_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_linear_image_interface_t_type_p;
};

%init (python) {
#ifdef struct_linear_image_interface_t_type_p
	struct_linear_image_interface_t_type_p = VT_lookup_python_opaque_type("api.linear_image_interface_t");
#endif /* struct_linear_image_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_link_endpoint_interface_t_type_p;
};

%init (python) {
#ifdef struct_link_endpoint_interface_t_type_p
	struct_link_endpoint_interface_t_type_p = VT_lookup_python_opaque_type("api.link_endpoint_interface_t");
#endif /* struct_link_endpoint_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_slave_time_t_type_p;
};

%init (python) {
#ifdef struct_slave_time_t_type_p
	struct_slave_time_t_type_p = VT_lookup_python_opaque_type("api.slave_time_t");
#endif /* struct_slave_time_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_link_endpoint_v2_interface_t_type_p;
};

%init (python) {
#ifdef struct_link_endpoint_v2_interface_t_type_p
	struct_link_endpoint_v2_interface_t_type_p = VT_lookup_python_opaque_type("api.link_endpoint_v2_interface_t");
#endif /* struct_link_endpoint_v2_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_log_object_interface_t_type_p;
};

%init (python) {
#ifdef struct_log_object_interface_t_type_p
	struct_log_object_interface_t_type_p = VT_lookup_python_opaque_type("api.log_object_interface_t");
#endif /* struct_log_object_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_map_demap_interface_t_type_p;
};

%init (python) {
#ifdef struct_map_demap_interface_t_type_p
	struct_map_demap_interface_t_type_p = VT_lookup_python_opaque_type("api.map_demap_interface_t");
#endif /* struct_map_demap_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_mdio45_bus_interface_t_type_p;
};

%init (python) {
#ifdef struct_mdio45_bus_interface_t_type_p
	struct_mdio45_bus_interface_t_type_p = VT_lookup_python_opaque_type("api.mdio45_bus_interface_t");
#endif /* struct_mdio45_bus_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_mdio45_phy_interface_t_type_p;
};

%init (python) {
#ifdef struct_mdio45_phy_interface_t_type_p
	struct_mdio45_phy_interface_t_type_p = VT_lookup_python_opaque_type("api.mdio45_phy_interface_t");
#endif /* struct_mdio45_phy_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_memory_profiler_interface_t_type_p;
};

%init (python) {
#ifdef struct_memory_profiler_interface_t_type_p
	struct_memory_profiler_interface_t_type_p = VT_lookup_python_opaque_type("api.memory_profiler_interface_t");
#endif /* struct_memory_profiler_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_memory_space_interface_t_type_p;
};

%init (python) {
#ifdef struct_memory_space_interface_t_type_p
	struct_memory_space_interface_t_type_p = VT_lookup_python_opaque_type("api.memory_space_interface_t");
#endif /* struct_memory_space_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_map_list_t_type_p;
};

%init (python) {
#ifdef struct_map_list_t_type_p
	struct_map_list_t_type_p = VT_lookup_python_opaque_type("api.map_list_t");
#endif /* struct_map_list_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_microwire_interface_t_type_p;
};

%init (python) {
#ifdef struct_microwire_interface_t_type_p
	struct_microwire_interface_t_type_p = VT_lookup_python_opaque_type("api.microwire_interface_t");
#endif /* struct_microwire_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_mii_interface_t_type_p;
};

%init (python) {
#ifdef struct_mii_interface_t_type_p
	struct_mii_interface_t_type_p = VT_lookup_python_opaque_type("api.mii_interface_t");
#endif /* struct_mii_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_mii_management_interface_t_type_p;
};

%init (python) {
#ifdef struct_mii_management_interface_t_type_p
	struct_mii_management_interface_t_type_p = VT_lookup_python_opaque_type("api.mii_management_interface_t");
#endif /* struct_mii_management_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_mips_interface_t_type_p;
};

%init (python) {
#ifdef struct_mips_interface_t_type_p
	struct_mips_interface_t_type_p = VT_lookup_python_opaque_type("api.mips_interface_t");
#endif /* struct_mips_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_mips_cache_instruction_interface_t_type_p;
};

%init (python) {
#ifdef struct_mips_cache_instruction_interface_t_type_p
	struct_mips_cache_instruction_interface_t_type_p = VT_lookup_python_opaque_type("api.mips_cache_instruction_interface_t");
#endif /* struct_mips_cache_instruction_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_mips_coprocessor_interface_t_type_p;
};

%init (python) {
#ifdef struct_mips_coprocessor_interface_t_type_p
	struct_mips_coprocessor_interface_t_type_p = VT_lookup_python_opaque_type("api.mips_coprocessor_interface_t");
#endif /* struct_mips_coprocessor_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_mips_eic_interface_t_type_p;
};

%init (python) {
#ifdef struct_mips_eic_interface_t_type_p
	struct_mips_eic_interface_t_type_p = VT_lookup_python_opaque_type("api.mips_eic_interface_t");
#endif /* struct_mips_eic_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_mips_exception_query_interface_t_type_p;
};

%init (python) {
#ifdef struct_mips_exception_query_interface_t_type_p
	struct_mips_exception_query_interface_t_type_p = VT_lookup_python_opaque_type("api.mips_exception_query_interface_t");
#endif /* struct_mips_exception_query_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_mips_ite_interface_t_type_p;
};

%init (python) {
#ifdef struct_mips_ite_interface_t_type_p
	struct_mips_ite_interface_t_type_p = VT_lookup_python_opaque_type("api.mips_ite_interface_t");
#endif /* struct_mips_ite_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_mips_mt_ase_interface_t_type_p;
};

%init (python) {
#ifdef struct_mips_mt_ase_interface_t_type_p
	struct_mips_mt_ase_interface_t_type_p = VT_lookup_python_opaque_type("api.mips_mt_ase_interface_t");
#endif /* struct_mips_mt_ase_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_mips_mt_ase_policy_interface_t_type_p;
};

%init (python) {
#ifdef struct_mips_mt_ase_policy_interface_t_type_p
	struct_mips_mt_ase_policy_interface_t_type_p = VT_lookup_python_opaque_type("api.mips_mt_ase_policy_interface_t");
#endif /* struct_mips_mt_ase_policy_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_mmc_interface_t_type_p;
};

%init (python) {
#ifdef struct_mmc_interface_t_type_p
	struct_mmc_interface_t_type_p = VT_lookup_python_opaque_type("api.mmc_interface_t");
#endif /* struct_mmc_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_mouse_interface_t_type_p;
};

%init (python) {
#ifdef struct_mouse_interface_t_type_p
	struct_mouse_interface_t_type_p = VT_lookup_python_opaque_type("api.mouse_interface_t");
#endif /* struct_mouse_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_multi_level_signal_interface_t_type_p;
};

%init (python) {
#ifdef struct_multi_level_signal_interface_t_type_p
	struct_multi_level_signal_interface_t_type_p = VT_lookup_python_opaque_type("api.multi_level_signal_interface_t");
#endif /* struct_multi_level_signal_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_nand_flash_interface_t_type_p;
};

%init (python) {
#ifdef struct_nand_flash_interface_t_type_p
	struct_nand_flash_interface_t_type_p = VT_lookup_python_opaque_type("api.nand_flash_interface_t");
#endif /* struct_nand_flash_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_network_breakpoint_interface_t_type_p;
};

%init (python) {
#ifdef struct_network_breakpoint_interface_t_type_p
	struct_network_breakpoint_interface_t_type_p = VT_lookup_python_opaque_type("api.network_breakpoint_interface_t");
#endif /* struct_network_breakpoint_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_nios_interface_t_type_p;
};

%init (python) {
#ifdef struct_nios_interface_t_type_p
	struct_nios_interface_t_type_p = VT_lookup_python_opaque_type("api.nios_interface_t");
#endif /* struct_nios_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_nios_cache_interface_t_type_p;
};

%init (python) {
#ifdef struct_nios_cache_interface_t_type_p
	struct_nios_cache_interface_t_type_p = VT_lookup_python_opaque_type("api.nios_cache_interface_t");
#endif /* struct_nios_cache_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_nios_custom_interface_t_type_p;
};

%init (python) {
#ifdef struct_nios_custom_interface_t_type_p
	struct_nios_custom_interface_t_type_p = VT_lookup_python_opaque_type("api.nios_custom_interface_t");
#endif /* struct_nios_custom_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_nios_eic_interface_t_type_p;
};

%init (python) {
#ifdef struct_nios_eic_interface_t_type_p
	struct_nios_eic_interface_t_type_p = VT_lookup_python_opaque_type("api.nios_eic_interface_t");
#endif /* struct_nios_eic_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_opcode_info_interface_t_type_p;
};

%init (python) {
#ifdef struct_opcode_info_interface_t_type_p
	struct_opcode_info_interface_t_type_p = VT_lookup_python_opaque_type("api.opcode_info_interface_t");
#endif /* struct_opcode_info_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_opcode_length_info_t_type_p;
};

%init (python) {
#ifdef struct_opcode_length_info_t_type_p
	struct_opcode_length_info_t_type_p = VT_lookup_python_opaque_type("api.opcode_length_info_t");
#endif /* struct_opcode_length_info_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_osa_component_interface_t_type_p;
};

%init (python) {
#ifdef struct_osa_component_interface_t_type_p
	struct_osa_component_interface_t_type_p = VT_lookup_python_opaque_type("api.osa_component_interface_t");
#endif /* struct_osa_component_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_maybe_node_id_t_type_p;
};

%init (python) {
#ifdef struct_maybe_node_id_t_type_p
	struct_maybe_node_id_t_type_p = VT_lookup_python_opaque_type("api.maybe_node_id_t");
#endif /* struct_maybe_node_id_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_osa_control_interface_t_type_p;
};

%init (python) {
#ifdef struct_osa_control_interface_t_type_p
	struct_osa_control_interface_t_type_p = VT_lookup_python_opaque_type("api.osa_control_interface_t");
#endif /* struct_osa_control_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_osa_control_v2_interface_t_type_p;
};

%init (python) {
#ifdef struct_osa_control_v2_interface_t_type_p
	struct_osa_control_v2_interface_t_type_p = VT_lookup_python_opaque_type("api.osa_control_v2_interface_t");
#endif /* struct_osa_control_v2_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_osa_machine_notification_interface_t_type_p;
};

%init (python) {
#ifdef struct_osa_machine_notification_interface_t_type_p
	struct_osa_machine_notification_interface_t_type_p = VT_lookup_python_opaque_type("api.osa_machine_notification_interface_t");
#endif /* struct_osa_machine_notification_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_osa_machine_query_interface_t_type_p;
};

%init (python) {
#ifdef struct_osa_machine_query_interface_t_type_p
	struct_osa_machine_query_interface_t_type_p = VT_lookup_python_opaque_type("api.osa_machine_query_interface_t");
#endif /* struct_osa_machine_query_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_osa_mapper_admin_interface_t_type_p;
};

%init (python) {
#ifdef struct_osa_mapper_admin_interface_t_type_p
	struct_osa_mapper_admin_interface_t_type_p = VT_lookup_python_opaque_type("api.osa_mapper_admin_interface_t");
#endif /* struct_osa_mapper_admin_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_osa_mapper_control_interface_t_type_p;
};

%init (python) {
#ifdef struct_osa_mapper_control_interface_t_type_p
	struct_osa_mapper_control_interface_t_type_p = VT_lookup_python_opaque_type("api.osa_mapper_control_interface_t");
#endif /* struct_osa_mapper_control_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_osa_mapper_query_interface_t_type_p;
};

%init (python) {
#ifdef struct_osa_mapper_query_interface_t_type_p
	struct_osa_mapper_query_interface_t_type_p = VT_lookup_python_opaque_type("api.osa_mapper_query_interface_t");
#endif /* struct_osa_mapper_query_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_osa_micro_checkpoint_interface_t_type_p;
};

%init (python) {
#ifdef struct_osa_micro_checkpoint_interface_t_type_p
	struct_osa_micro_checkpoint_interface_t_type_p = VT_lookup_python_opaque_type("api.osa_micro_checkpoint_interface_t");
#endif /* struct_osa_micro_checkpoint_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_osa_node_path_interface_t_type_p;
};

%init (python) {
#ifdef struct_osa_node_path_interface_t_type_p
	struct_osa_node_path_interface_t_type_p = VT_lookup_python_opaque_type("api.osa_node_path_interface_t");
#endif /* struct_osa_node_path_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_osa_node_tree_admin_interface_t_type_p;
};

%init (python) {
#ifdef struct_osa_node_tree_admin_interface_t_type_p
	struct_osa_node_tree_admin_interface_t_type_p = VT_lookup_python_opaque_type("api.osa_node_tree_admin_interface_t");
#endif /* struct_osa_node_tree_admin_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_osa_node_tree_notification_interface_t_type_p;
};

%init (python) {
#ifdef struct_osa_node_tree_notification_interface_t_type_p
	struct_osa_node_tree_notification_interface_t_type_p = VT_lookup_python_opaque_type("api.osa_node_tree_notification_interface_t");
#endif /* struct_osa_node_tree_notification_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_osa_node_tree_query_interface_t_type_p;
};

%init (python) {
#ifdef struct_osa_node_tree_query_interface_t_type_p
	struct_osa_node_tree_query_interface_t_type_p = VT_lookup_python_opaque_type("api.osa_node_tree_query_interface_t");
#endif /* struct_osa_node_tree_query_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_osa_parameters_interface_t_type_p;
};

%init (python) {
#ifdef struct_osa_parameters_interface_t_type_p
	struct_osa_parameters_interface_t_type_p = VT_lookup_python_opaque_type("api.osa_parameters_interface_t");
#endif /* struct_osa_parameters_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_osa_tracker_component_interface_t_type_p;
};

%init (python) {
#ifdef struct_osa_tracker_component_interface_t_type_p
	struct_osa_tracker_component_interface_t_type_p = VT_lookup_python_opaque_type("api.osa_tracker_component_interface_t");
#endif /* struct_osa_tracker_component_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_osa_tracker_control_interface_t_type_p;
};

%init (python) {
#ifdef struct_osa_tracker_control_interface_t_type_p
	struct_osa_tracker_control_interface_t_type_p = VT_lookup_python_opaque_type("api.osa_tracker_control_interface_t");
#endif /* struct_osa_tracker_control_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_osa_tracker_state_admin_interface_t_type_p;
};

%init (python) {
#ifdef struct_osa_tracker_state_admin_interface_t_type_p
	struct_osa_tracker_state_admin_interface_t_type_p = VT_lookup_python_opaque_type("api.osa_tracker_state_admin_interface_t");
#endif /* struct_osa_tracker_state_admin_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_osa_tracker_state_notification_interface_t_type_p;
};

%init (python) {
#ifdef struct_osa_tracker_state_notification_interface_t_type_p
	struct_osa_tracker_state_notification_interface_t_type_p = VT_lookup_python_opaque_type("api.osa_tracker_state_notification_interface_t");
#endif /* struct_osa_tracker_state_notification_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_osa_tracker_state_query_interface_t_type_p;
};

%init (python) {
#ifdef struct_osa_tracker_state_query_interface_t_type_p
	struct_osa_tracker_state_query_interface_t_type_p = VT_lookup_python_opaque_type("api.osa_tracker_state_query_interface_t");
#endif /* struct_osa_tracker_state_query_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_packet_interface_t_type_p;
};

%init (python) {
#ifdef struct_packet_interface_t_type_p
	struct_packet_interface_t_type_p = VT_lookup_python_opaque_type("api.packet_interface_t");
#endif /* struct_packet_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_page_bank_interface_t_type_p;
};

%init (python) {
#ifdef struct_page_bank_interface_t_type_p
	struct_page_bank_interface_t_type_p = VT_lookup_python_opaque_type("api.page_bank_interface_t");
#endif /* struct_page_bank_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_pb_page_t_type_p;
};

%init (python) {
#ifdef struct_pb_page_t_type_p
	struct_pb_page_t_type_p = VT_lookup_python_opaque_type("api.pb_page_t");
#endif /* struct_pb_page_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_page_bank_grant_t_type_p;
};

%init (python) {
#ifdef struct_page_bank_grant_t_type_p
	struct_page_bank_grant_t_type_p = VT_lookup_python_opaque_type("api.page_bank_grant_t");
#endif /* struct_page_bank_grant_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_page_bank_client_interface_t_type_p;
};

%init (python) {
#ifdef struct_page_bank_client_interface_t_type_p
	struct_page_bank_client_interface_t_type_p = VT_lookup_python_opaque_type("api.page_bank_client_interface_t");
#endif /* struct_page_bank_client_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_pci_bridge_interface_t_type_p;
};

%init (python) {
#ifdef struct_pci_bridge_interface_t_type_p
	struct_pci_bridge_interface_t_type_p = VT_lookup_python_opaque_type("api.pci_bridge_interface_t");
#endif /* struct_pci_bridge_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_pci_bus_interface_t_type_p;
};

%init (python) {
#ifdef struct_pci_bus_interface_t_type_p
	struct_pci_bus_interface_t_type_p = VT_lookup_python_opaque_type("api.pci_bus_interface_t");
#endif /* struct_pci_bus_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_pci_device_interface_t_type_p;
};

%init (python) {
#ifdef struct_pci_device_interface_t_type_p
	struct_pci_device_interface_t_type_p = VT_lookup_python_opaque_type("api.pci_device_interface_t");
#endif /* struct_pci_device_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_pci_downstream_interface_t_type_p;
};

%init (python) {
#ifdef struct_pci_downstream_interface_t_type_p
	struct_pci_downstream_interface_t_type_p = VT_lookup_python_opaque_type("api.pci_downstream_interface_t");
#endif /* struct_pci_downstream_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_pci_express_interface_t_type_p;
};

%init (python) {
#ifdef struct_pci_express_interface_t_type_p
	struct_pci_express_interface_t_type_p = VT_lookup_python_opaque_type("api.pci_express_interface_t");
#endif /* struct_pci_express_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_pci_express_hotplug_interface_t_type_p;
};

%init (python) {
#ifdef struct_pci_express_hotplug_interface_t_type_p
	struct_pci_express_hotplug_interface_t_type_p = VT_lookup_python_opaque_type("api.pci_express_hotplug_interface_t");
#endif /* struct_pci_express_hotplug_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_pci_interrupt_interface_t_type_p;
};

%init (python) {
#ifdef struct_pci_interrupt_interface_t_type_p
	struct_pci_interrupt_interface_t_type_p = VT_lookup_python_opaque_type("api.pci_interrupt_interface_t");
#endif /* struct_pci_interrupt_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_pci_multi_function_device_interface_t_type_p;
};

%init (python) {
#ifdef struct_pci_multi_function_device_interface_t_type_p
	struct_pci_multi_function_device_interface_t_type_p = VT_lookup_python_opaque_type("api.pci_multi_function_device_interface_t");
#endif /* struct_pci_multi_function_device_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_pci_upstream_interface_t_type_p;
};

%init (python) {
#ifdef struct_pci_upstream_interface_t_type_p
	struct_pci_upstream_interface_t_type_p = VT_lookup_python_opaque_type("api.pci_upstream_interface_t");
#endif /* struct_pci_upstream_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_pci_upstream_operation_interface_t_type_p;
};

%init (python) {
#ifdef struct_pci_upstream_operation_interface_t_type_p
	struct_pci_upstream_operation_interface_t_type_p = VT_lookup_python_opaque_type("api.pci_upstream_operation_interface_t");
#endif /* struct_pci_upstream_operation_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_pcie_adapter_compat_interface_t_type_p;
};

%init (python) {
#ifdef struct_pcie_adapter_compat_interface_t_type_p
	struct_pcie_adapter_compat_interface_t_type_p = VT_lookup_python_opaque_type("api.pcie_adapter_compat_interface_t");
#endif /* struct_pcie_adapter_compat_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_pcie_device_interface_t_type_p;
};

%init (python) {
#ifdef struct_pcie_device_interface_t_type_p
	struct_pcie_device_interface_t_type_p = VT_lookup_python_opaque_type("api.pcie_device_interface_t");
#endif /* struct_pcie_device_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_pcie_hotplug_events_interface_t_type_p;
};

%init (python) {
#ifdef struct_pcie_hotplug_events_interface_t_type_p
	struct_pcie_hotplug_events_interface_t_type_p = VT_lookup_python_opaque_type("api.pcie_hotplug_events_interface_t");
#endif /* struct_pcie_hotplug_events_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_pcie_link_training_interface_t_type_p;
};

%init (python) {
#ifdef struct_pcie_link_training_interface_t_type_p
	struct_pcie_link_training_interface_t_type_p = VT_lookup_python_opaque_type("api.pcie_link_training_interface_t");
#endif /* struct_pcie_link_training_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_pcie_map_interface_t_type_p;
};

%init (python) {
#ifdef struct_pcie_map_interface_t_type_p
	struct_pcie_map_interface_t_type_p = VT_lookup_python_opaque_type("api.pcie_map_interface_t");
#endif /* struct_pcie_map_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_pcie_port_control_interface_t_type_p;
};

%init (python) {
#ifdef struct_pcie_port_control_interface_t_type_p
	struct_pcie_port_control_interface_t_type_p = VT_lookup_python_opaque_type("api.pcie_port_control_interface_t");
#endif /* struct_pcie_port_control_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_pmr_interface_t_type_p;
};

%init (python) {
#ifdef struct_pmr_interface_t_type_p
	struct_pmr_interface_t_type_p = VT_lookup_python_opaque_type("api.pmr_interface_t");
#endif /* struct_pmr_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_pool_protect_interface_t_type_p;
};

%init (python) {
#ifdef struct_pool_protect_interface_t_type_p
	struct_pool_protect_interface_t_type_p = VT_lookup_python_opaque_type("api.pool_protect_interface_t");
#endif /* struct_pool_protect_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_port_forward_interface_t_type_p;
};

%init (python) {
#ifdef struct_port_forward_interface_t_type_p
	struct_port_forward_interface_t_type_p = VT_lookup_python_opaque_type("api.port_forward_interface_t");
#endif /* struct_port_forward_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_ip_port_t_type_p;
};

%init (python) {
#ifdef struct_ip_port_t_type_p
	struct_ip_port_t_type_p = VT_lookup_python_opaque_type("api.ip_port_t");
#endif /* struct_ip_port_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_port_space_interface_t_type_p;
};

%init (python) {
#ifdef struct_port_space_interface_t_type_p
	struct_port_space_interface_t_type_p = VT_lookup_python_opaque_type("api.port_space_interface_t");
#endif /* struct_port_space_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_ppc_interface_t_type_p;
};

%init (python) {
#ifdef struct_ppc_interface_t_type_p
	struct_ppc_interface_t_type_p = VT_lookup_python_opaque_type("api.ppc_interface_t");
#endif /* struct_ppc_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_preference_interface_t_type_p;
};

%init (python) {
#ifdef struct_preference_interface_t_type_p
	struct_preference_interface_t_type_p = VT_lookup_python_opaque_type("api.preference_interface_t");
#endif /* struct_preference_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_probe_interface_t_type_p;
};

%init (python) {
#ifdef struct_probe_interface_t_type_p
	struct_probe_interface_t_type_p = VT_lookup_python_opaque_type("api.probe_interface_t");
#endif /* struct_probe_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_probe_array_interface_t_type_p;
};

%init (python) {
#ifdef struct_probe_array_interface_t_type_p
	struct_probe_array_interface_t_type_p = VT_lookup_python_opaque_type("api.probe_array_interface_t");
#endif /* struct_probe_array_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_probe_index_interface_t_type_p;
};

%init (python) {
#ifdef struct_probe_index_interface_t_type_p
	struct_probe_index_interface_t_type_p = VT_lookup_python_opaque_type("api.probe_index_interface_t");
#endif /* struct_probe_index_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_probe_sampler_cache_interface_t_type_p;
};

%init (python) {
#ifdef struct_probe_sampler_cache_interface_t_type_p
	struct_probe_sampler_cache_interface_t_type_p = VT_lookup_python_opaque_type("api.probe_sampler_cache_interface_t");
#endif /* struct_probe_sampler_cache_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_probe_subscribe_interface_t_type_p;
};

%init (python) {
#ifdef struct_probe_subscribe_interface_t_type_p
	struct_probe_subscribe_interface_t_type_p = VT_lookup_python_opaque_type("api.probe_subscribe_interface_t");
#endif /* struct_probe_subscribe_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_processor_cli_interface_t_type_p;
};

%init (python) {
#ifdef struct_processor_cli_interface_t_type_p
	struct_processor_cli_interface_t_type_p = VT_lookup_python_opaque_type("api.processor_cli_interface_t");
#endif /* struct_processor_cli_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_physical_block_t_type_p;
};

%init (python) {
#ifdef struct_physical_block_t_type_p
	struct_physical_block_t_type_p = VT_lookup_python_opaque_type("api.physical_block_t");
#endif /* struct_physical_block_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_processor_endian_interface_t_type_p;
};

%init (python) {
#ifdef struct_processor_endian_interface_t_type_p
	struct_processor_endian_interface_t_type_p = VT_lookup_python_opaque_type("api.processor_endian_interface_t");
#endif /* struct_processor_endian_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_processor_gui_interface_t_type_p;
};

%init (python) {
#ifdef struct_processor_gui_interface_t_type_p
	struct_processor_gui_interface_t_type_p = VT_lookup_python_opaque_type("api.processor_gui_interface_t");
#endif /* struct_processor_gui_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_processor_info_interface_t_type_p;
};

%init (python) {
#ifdef struct_processor_info_interface_t_type_p
	struct_processor_info_interface_t_type_p = VT_lookup_python_opaque_type("api.processor_info_interface_t");
#endif /* struct_processor_info_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_processor_info_v2_interface_t_type_p;
};

%init (python) {
#ifdef struct_processor_info_v2_interface_t_type_p
	struct_processor_info_v2_interface_t_type_p = VT_lookup_python_opaque_type("api.processor_info_v2_interface_t");
#endif /* struct_processor_info_v2_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_pulse_interface_t_type_p;
};

%init (python) {
#ifdef struct_pulse_interface_t_type_p
	struct_pulse_interface_t_type_p = VT_lookup_python_opaque_type("api.pulse_interface_t");
#endif /* struct_pulse_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_ram_interface_t_type_p;
};

%init (python) {
#ifdef struct_ram_interface_t_type_p
	struct_ram_interface_t_type_p = VT_lookup_python_opaque_type("api.ram_interface_t");
#endif /* struct_ram_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_ram_access_subscribe_interface_t_type_p;
};

%init (python) {
#ifdef struct_ram_access_subscribe_interface_t_type_p
	struct_ram_access_subscribe_interface_t_type_p = VT_lookup_python_opaque_type("api.ram_access_subscribe_interface_t");
#endif /* struct_ram_access_subscribe_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_ram_cb_handle_t_type_p;
};

%init (python) {
#ifdef struct_ram_cb_handle_t_type_p
	struct_ram_cb_handle_t_type_p = VT_lookup_python_opaque_type("api.ram_cb_handle_t");
#endif /* struct_ram_cb_handle_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_recorded_interface_t_type_p;
};

%init (python) {
#ifdef struct_recorded_interface_t_type_p
	struct_recorded_interface_t_type_p = VT_lookup_python_opaque_type("api.recorded_interface_t");
#endif /* struct_recorded_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_recorder_interface_t_type_p;
};

%init (python) {
#ifdef struct_recorder_interface_t_type_p
	struct_recorder_interface_t_type_p = VT_lookup_python_opaque_type("api.recorder_interface_t");
#endif /* struct_recorder_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_recorder_t_type_p;
};

%init (python) {
#ifdef struct_recorder_t_type_p
	struct_recorder_t_type_p = VT_lookup_python_opaque_type("api.recorder_t");
#endif /* struct_recorder_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_recorder_v2_interface_t_type_p;
};

%init (python) {
#ifdef struct_recorder_v2_interface_t_type_p
	struct_recorder_v2_interface_t_type_p = VT_lookup_python_opaque_type("api.recorder_v2_interface_t");
#endif /* struct_recorder_v2_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_register_breakpoint_interface_t_type_p;
};

%init (python) {
#ifdef struct_register_breakpoint_interface_t_type_p
	struct_register_breakpoint_interface_t_type_p = VT_lookup_python_opaque_type("api.register_breakpoint_interface_t");
#endif /* struct_register_breakpoint_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_register_view_interface_t_type_p;
};

%init (python) {
#ifdef struct_register_view_interface_t_type_p
	struct_register_view_interface_t_type_p = VT_lookup_python_opaque_type("api.register_view_interface_t");
#endif /* struct_register_view_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_register_view_catalog_interface_t_type_p;
};

%init (python) {
#ifdef struct_register_view_catalog_interface_t_type_p
	struct_register_view_catalog_interface_t_type_p = VT_lookup_python_opaque_type("api.register_view_catalog_interface_t");
#endif /* struct_register_view_catalog_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_register_view_read_only_interface_t_type_p;
};

%init (python) {
#ifdef struct_register_view_read_only_interface_t_type_p
	struct_register_view_read_only_interface_t_type_p = VT_lookup_python_opaque_type("api.register_view_read_only_interface_t");
#endif /* struct_register_view_read_only_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_riscv_clic_interface_t_type_p;
};

%init (python) {
#ifdef struct_riscv_clic_interface_t_type_p
	struct_riscv_clic_interface_t_type_p = VT_lookup_python_opaque_type("api.riscv_clic_interface_t");
#endif /* struct_riscv_clic_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_riscv_clic_interrupt_interface_t_type_p;
};

%init (python) {
#ifdef struct_riscv_clic_interrupt_interface_t_type_p
	struct_riscv_clic_interrupt_interface_t_type_p = VT_lookup_python_opaque_type("api.riscv_clic_interrupt_interface_t");
#endif /* struct_riscv_clic_interrupt_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_riscv_coprocessor_interface_t_type_p;
};

%init (python) {
#ifdef struct_riscv_coprocessor_interface_t_type_p
	struct_riscv_coprocessor_interface_t_type_p = VT_lookup_python_opaque_type("api.riscv_coprocessor_interface_t");
#endif /* struct_riscv_coprocessor_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_riscv_custom_csr_interface_t_type_p;
};

%init (python) {
#ifdef struct_riscv_custom_csr_interface_t_type_p
	struct_riscv_custom_csr_interface_t_type_p = VT_lookup_python_opaque_type("api.riscv_custom_csr_interface_t");
#endif /* struct_riscv_custom_csr_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_riscv_imsic_interface_t_type_p;
};

%init (python) {
#ifdef struct_riscv_imsic_interface_t_type_p
	struct_riscv_imsic_interface_t_type_p = VT_lookup_python_opaque_type("api.riscv_imsic_interface_t");
#endif /* struct_riscv_imsic_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_riscv_instruction_action_interface_t_type_p;
};

%init (python) {
#ifdef struct_riscv_instruction_action_interface_t_type_p
	struct_riscv_instruction_action_interface_t_type_p = VT_lookup_python_opaque_type("api.riscv_instruction_action_interface_t");
#endif /* struct_riscv_instruction_action_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_riscv_signal_sgeip_interface_t_type_p;
};

%init (python) {
#ifdef struct_riscv_signal_sgeip_interface_t_type_p
	struct_riscv_signal_sgeip_interface_t_type_p = VT_lookup_python_opaque_type("api.riscv_signal_sgeip_interface_t");
#endif /* struct_riscv_signal_sgeip_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_rs232_console_interface_t_type_p;
};

%init (python) {
#ifdef struct_rs232_console_interface_t_type_p
	struct_rs232_console_interface_t_type_p = VT_lookup_python_opaque_type("api.rs232_console_interface_t");
#endif /* struct_rs232_console_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_rs232_device_interface_t_type_p;
};

%init (python) {
#ifdef struct_rs232_device_interface_t_type_p
	struct_rs232_device_interface_t_type_p = VT_lookup_python_opaque_type("api.rs232_device_interface_t");
#endif /* struct_rs232_device_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_sata_interface_t_type_p;
};

%init (python) {
#ifdef struct_sata_interface_t_type_p
	struct_sata_interface_t_type_p = VT_lookup_python_opaque_type("api.sata_interface_t");
#endif /* struct_sata_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_scalar_time_interface_t_type_p;
};

%init (python) {
#ifdef struct_scalar_time_interface_t_type_p
	struct_scalar_time_interface_t_type_p = VT_lookup_python_opaque_type("api.scalar_time_interface_t");
#endif /* struct_scalar_time_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_scale_factor_listener_interface_t_type_p;
};

%init (python) {
#ifdef struct_scale_factor_listener_interface_t_type_p
	struct_scale_factor_listener_interface_t_type_p = VT_lookup_python_opaque_type("api.scale_factor_listener_interface_t");
#endif /* struct_scale_factor_listener_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_screenshot_interface_t_type_p;
};

%init (python) {
#ifdef struct_screenshot_interface_t_type_p
	struct_screenshot_interface_t_type_p = VT_lookup_python_opaque_type("api.screenshot_interface_t");
#endif /* struct_screenshot_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_serial_console_frontend_interface_t_type_p;
};

%init (python) {
#ifdef struct_serial_console_frontend_interface_t_type_p
	struct_serial_console_frontend_interface_t_type_p = VT_lookup_python_opaque_type("api.serial_console_frontend_interface_t");
#endif /* struct_serial_console_frontend_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_serial_device_interface_t_type_p;
};

%init (python) {
#ifdef struct_serial_device_interface_t_type_p
	struct_serial_device_interface_t_type_p = VT_lookup_python_opaque_type("api.serial_device_interface_t");
#endif /* struct_serial_device_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_serial_peripheral_interface_master_interface_t_type_p;
};

%init (python) {
#ifdef struct_serial_peripheral_interface_master_interface_t_type_p
	struct_serial_peripheral_interface_master_interface_t_type_p = VT_lookup_python_opaque_type("api.serial_peripheral_interface_master_interface_t");
#endif /* struct_serial_peripheral_interface_master_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_serial_peripheral_interface_slave_interface_t_type_p;
};

%init (python) {
#ifdef struct_serial_peripheral_interface_slave_interface_t_type_p
	struct_serial_peripheral_interface_slave_interface_t_type_p = VT_lookup_python_opaque_type("api.serial_peripheral_interface_slave_interface_t");
#endif /* struct_serial_peripheral_interface_slave_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_sh_interrupt_interface_t_type_p;
};

%init (python) {
#ifdef struct_sh_interrupt_interface_t_type_p
	struct_sh_interrupt_interface_t_type_p = VT_lookup_python_opaque_type("api.sh_interrupt_interface_t");
#endif /* struct_sh_interrupt_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_signal_interface_t_type_p;
};

%init (python) {
#ifdef struct_signal_interface_t_type_p
	struct_signal_interface_t_type_p = VT_lookup_python_opaque_type("api.signal_interface_t");
#endif /* struct_signal_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_simple_dispatcher_interface_t_type_p;
};

%init (python) {
#ifdef struct_simple_dispatcher_interface_t_type_p
	struct_simple_dispatcher_interface_t_type_p = VT_lookup_python_opaque_type("api.simple_dispatcher_interface_t");
#endif /* struct_simple_dispatcher_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_simple_interrupt_interface_t_type_p;
};

%init (python) {
#ifdef struct_simple_interrupt_interface_t_type_p
	struct_simple_interrupt_interface_t_type_p = VT_lookup_python_opaque_type("api.simple_interrupt_interface_t");
#endif /* struct_simple_interrupt_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_simulator_cache_interface_t_type_p;
};

%init (python) {
#ifdef struct_simulator_cache_interface_t_type_p
	struct_simulator_cache_interface_t_type_p = VT_lookup_python_opaque_type("api.simulator_cache_interface_t");
#endif /* struct_simulator_cache_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_slave_agent_interface_t_type_p;
};

%init (python) {
#ifdef struct_slave_agent_interface_t_type_p
	struct_slave_agent_interface_t_type_p = VT_lookup_python_opaque_type("api.slave_agent_interface_t");
#endif /* struct_slave_agent_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_slaver_message_interface_t_type_p;
};

%init (python) {
#ifdef struct_slaver_message_interface_t_type_p
	struct_slaver_message_interface_t_type_p = VT_lookup_python_opaque_type("api.slaver_message_interface_t");
#endif /* struct_slaver_message_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_smm_instrumentation_subscribe_interface_t_type_p;
};

%init (python) {
#ifdef struct_smm_instrumentation_subscribe_interface_t_type_p
	struct_smm_instrumentation_subscribe_interface_t_type_p = VT_lookup_python_opaque_type("api.smm_instrumentation_subscribe_interface_t");
#endif /* struct_smm_instrumentation_subscribe_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_snoop_memory_interface_t_type_p;
};

%init (python) {
#ifdef struct_snoop_memory_interface_t_type_p
	struct_snoop_memory_interface_t_type_p = VT_lookup_python_opaque_type("api.snoop_memory_interface_t");
#endif /* struct_snoop_memory_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_sparc_v8_interface_t_type_p;
};

%init (python) {
#ifdef struct_sparc_v8_interface_t_type_p
	struct_sparc_v8_interface_t_type_p = VT_lookup_python_opaque_type("api.sparc_v8_interface_t");
#endif /* struct_sparc_v8_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_sparc_v8_ecc_fault_injection_interface_t_type_p;
};

%init (python) {
#ifdef struct_sparc_v8_ecc_fault_injection_interface_t_type_p
	struct_sparc_v8_ecc_fault_injection_interface_t_type_p = VT_lookup_python_opaque_type("api.sparc_v8_ecc_fault_injection_interface_t");
#endif /* struct_sparc_v8_ecc_fault_injection_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_spr_interface_t_type_p;
};

%init (python) {
#ifdef struct_spr_interface_t_type_p
	struct_spr_interface_t_type_p = VT_lookup_python_opaque_type("api.spr_interface_t");
#endif /* struct_spr_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_stall_interface_t_type_p;
};

%init (python) {
#ifdef struct_stall_interface_t_type_p
	struct_stall_interface_t_type_p = VT_lookup_python_opaque_type("api.stall_interface_t");
#endif /* struct_stall_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_stc_interface_t_type_p;
};

%init (python) {
#ifdef struct_stc_interface_t_type_p
	struct_stc_interface_t_type_p = VT_lookup_python_opaque_type("api.stc_interface_t");
#endif /* struct_stc_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_page_t_type_p;
};

%init (python) {
#ifdef struct_page_t_type_p
	struct_page_t_type_p = VT_lookup_python_opaque_type("api.page_t");
#endif /* struct_page_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_step_interface_t_type_p;
};

%init (python) {
#ifdef struct_step_interface_t_type_p
	struct_step_interface_t_type_p = VT_lookup_python_opaque_type("api.step_interface_t");
#endif /* struct_step_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_step_cycle_ratio_interface_t_type_p;
};

%init (python) {
#ifdef struct_step_cycle_ratio_interface_t_type_p
	struct_step_cycle_ratio_interface_t_type_p = VT_lookup_python_opaque_type("api.step_cycle_ratio_interface_t");
#endif /* struct_step_cycle_ratio_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_step_cycle_ratio_t_type_p;
};

%init (python) {
#ifdef struct_step_cycle_ratio_t_type_p
	struct_step_cycle_ratio_t_type_p = VT_lookup_python_opaque_type("api.step_cycle_ratio_t");
#endif /* struct_step_cycle_ratio_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_step_event_instrumentation_interface_t_type_p;
};

%init (python) {
#ifdef struct_step_event_instrumentation_interface_t_type_p
	struct_step_event_instrumentation_interface_t_type_p = VT_lookup_python_opaque_type("api.step_event_instrumentation_interface_t");
#endif /* struct_step_event_instrumentation_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_step_handle_t_type_p;
};

%init (python) {
#ifdef struct_step_handle_t_type_p
	struct_step_handle_t_type_p = VT_lookup_python_opaque_type("api.step_handle_t");
#endif /* struct_step_handle_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_step_info_interface_t_type_p;
};

%init (python) {
#ifdef struct_step_info_interface_t_type_p
	struct_step_info_interface_t_type_p = VT_lookup_python_opaque_type("api.step_info_interface_t");
#endif /* struct_step_info_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_symbols_interface_t_type_p;
};

%init (python) {
#ifdef struct_symbols_interface_t_type_p
	struct_symbols_interface_t_type_p = VT_lookup_python_opaque_type("api.symbols_interface_t");
#endif /* struct_symbols_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_symbol_t_type_p;
};

%init (python) {
#ifdef struct_symbol_t_type_p
	struct_symbol_t_type_p = VT_lookup_python_opaque_type("api.symbol_t");
#endif /* struct_symbol_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_sync_initial_time_interface_t_type_p;
};

%init (python) {
#ifdef struct_sync_initial_time_interface_t_type_p
	struct_sync_initial_time_interface_t_type_p = VT_lookup_python_opaque_type("api.sync_initial_time_interface_t");
#endif /* struct_sync_initial_time_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_synchronous_mode_interface_t_type_p;
};

%init (python) {
#ifdef struct_synchronous_mode_interface_t_type_p
	struct_synchronous_mode_interface_t_type_p = VT_lookup_python_opaque_type("api.synchronous_mode_interface_t");
#endif /* struct_synchronous_mode_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_table_interface_t_type_p;
};

%init (python) {
#ifdef struct_table_interface_t_type_p
	struct_table_interface_t_type_p = VT_lookup_python_opaque_type("api.table_interface_t");
#endif /* struct_table_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_tcf_channel_interface_t_type_p;
};

%init (python) {
#ifdef struct_tcf_channel_interface_t_type_p
	struct_tcf_channel_interface_t_type_p = VT_lookup_python_opaque_type("api.tcf_channel_interface_t");
#endif /* struct_tcf_channel_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_tcp_connection_info_interface_t_type_p;
};

%init (python) {
#ifdef struct_tcp_connection_info_interface_t_type_p
	struct_tcp_connection_info_interface_t_type_p = VT_lookup_python_opaque_type("api.tcp_connection_info_interface_t");
#endif /* struct_tcp_connection_info_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_telemetry_interface_t_type_p;
};

%init (python) {
#ifdef struct_telemetry_interface_t_type_p
	struct_telemetry_interface_t_type_p = VT_lookup_python_opaque_type("api.telemetry_interface_t");
#endif /* struct_telemetry_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_telemetry_manager_interface_t_type_p;
};

%init (python) {
#ifdef struct_telemetry_manager_interface_t_type_p
	struct_telemetry_manager_interface_t_type_p = VT_lookup_python_opaque_type("api.telemetry_manager_interface_t");
#endif /* struct_telemetry_manager_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_telnet_connection_v2_interface_t_type_p;
};

%init (python) {
#ifdef struct_telnet_connection_v2_interface_t_type_p
	struct_telnet_connection_v2_interface_t_type_p = VT_lookup_python_opaque_type("api.telnet_connection_v2_interface_t");
#endif /* struct_telnet_connection_v2_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_temporal_state_interface_t_type_p;
};

%init (python) {
#ifdef struct_temporal_state_interface_t_type_p
	struct_temporal_state_interface_t_type_p = VT_lookup_python_opaque_type("api.temporal_state_interface_t");
#endif /* struct_temporal_state_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_terminal_client_interface_t_type_p;
};

%init (python) {
#ifdef struct_terminal_client_interface_t_type_p
	struct_terminal_client_interface_t_type_p = VT_lookup_python_opaque_type("api.terminal_client_interface_t");
#endif /* struct_terminal_client_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_terminal_server_interface_t_type_p;
};

%init (python) {
#ifdef struct_terminal_server_interface_t_type_p
	struct_terminal_server_interface_t_type_p = VT_lookup_python_opaque_type("api.terminal_server_interface_t");
#endif /* struct_terminal_server_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_text_console_backend_interface_t_type_p;
};

%init (python) {
#ifdef struct_text_console_backend_interface_t_type_p
	struct_text_console_backend_interface_t_type_p = VT_lookup_python_opaque_type("api.text_console_backend_interface_t");
#endif /* struct_text_console_backend_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_text_console_frontend_interface_t_type_p;
};

%init (python) {
#ifdef struct_text_console_frontend_interface_t_type_p
	struct_text_console_frontend_interface_t_type_p = VT_lookup_python_opaque_type("api.text_console_frontend_interface_t");
#endif /* struct_text_console_frontend_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_timing_model_interface_t_type_p;
};

%init (python) {
#ifdef struct_timing_model_interface_t_type_p
	struct_timing_model_interface_t_type_p = VT_lookup_python_opaque_type("api.timing_model_interface_t");
#endif /* struct_timing_model_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_transaction_interface_t_type_p;
};

%init (python) {
#ifdef struct_transaction_interface_t_type_p
	struct_transaction_interface_t_type_p = VT_lookup_python_opaque_type("api.transaction_interface_t");
#endif /* struct_transaction_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_transaction_subscribe_interface_t_type_p;
};

%init (python) {
#ifdef struct_transaction_subscribe_interface_t_type_p
	struct_transaction_subscribe_interface_t_type_p = VT_lookup_python_opaque_type("api.transaction_subscribe_interface_t");
#endif /* struct_transaction_subscribe_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_transaction_cb_handle_t_type_p;
};

%init (python) {
#ifdef struct_transaction_cb_handle_t_type_p
	struct_transaction_cb_handle_t_type_p = VT_lookup_python_opaque_type("api.transaction_cb_handle_t");
#endif /* struct_transaction_cb_handle_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_map_target_t_type_p;
};

%init (python) {
#ifdef struct_map_target_t_type_p
	struct_map_target_t_type_p = VT_lookup_python_opaque_type("api.map_target_t");
#endif /* struct_map_target_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_next_map_t_type_p;
};

%init (python) {
#ifdef struct_next_map_t_type_p
	struct_next_map_t_type_p = VT_lookup_python_opaque_type("api.next_map_t");
#endif /* struct_next_map_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_transaction_translator_interface_t_type_p;
};

%init (python) {
#ifdef struct_transaction_translator_interface_t_type_p
	struct_transaction_translator_interface_t_type_p = VT_lookup_python_opaque_type("api.transaction_translator_interface_t");
#endif /* struct_transaction_translator_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_translation_t_type_p;
};

%init (python) {
#ifdef struct_translation_t_type_p
	struct_translation_t_type_p = VT_lookup_python_opaque_type("api.translation_t");
#endif /* struct_translation_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_translate_interface_t_type_p;
};

%init (python) {
#ifdef struct_translate_interface_t_type_p
	struct_translate_interface_t_type_p = VT_lookup_python_opaque_type("api.translate_interface_t");
#endif /* struct_translate_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_translation_flush_interface_t_type_p;
};

%init (python) {
#ifdef struct_translation_flush_interface_t_type_p
	struct_translation_flush_interface_t_type_p = VT_lookup_python_opaque_type("api.translation_flush_interface_t");
#endif /* struct_translation_flush_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_translator_interface_t_type_p;
};

%init (python) {
#ifdef struct_translator_interface_t_type_p
	struct_translator_interface_t_type_p = VT_lookup_python_opaque_type("api.translator_interface_t");
#endif /* struct_translator_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_uint64_state_interface_t_type_p;
};

%init (python) {
#ifdef struct_uint64_state_interface_t_type_p
	struct_uint64_state_interface_t_type_p = VT_lookup_python_opaque_type("api.uint64_state_interface_t");
#endif /* struct_uint64_state_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_usb_interface_t_type_p;
};

%init (python) {
#ifdef struct_usb_interface_t_type_p
	struct_usb_interface_t_type_p = VT_lookup_python_opaque_type("api.usb_interface_t");
#endif /* struct_usb_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_usb_transfer_t_type_p;
};

%init (python) {
#ifdef struct_usb_transfer_t_type_p
	struct_usb_transfer_t_type_p = VT_lookup_python_opaque_type("api.usb_transfer_t");
#endif /* struct_usb_transfer_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_usb_device_interface_t_type_p;
};

%init (python) {
#ifdef struct_usb_device_interface_t_type_p
	struct_usb_device_interface_t_type_p = VT_lookup_python_opaque_type("api.usb_device_interface_t");
#endif /* struct_usb_device_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_vectored_interrupt_interface_t_type_p;
};

%init (python) {
#ifdef struct_vectored_interrupt_interface_t_type_p
	struct_vectored_interrupt_interface_t_type_p = VT_lookup_python_opaque_type("api.vectored_interrupt_interface_t");
#endif /* struct_vectored_interrupt_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_vectored_interrupt_source_interface_t_type_p;
};

%init (python) {
#ifdef struct_vectored_interrupt_source_interface_t_type_p
	struct_vectored_interrupt_source_interface_t_type_p = VT_lookup_python_opaque_type("api.vectored_interrupt_source_interface_t");
#endif /* struct_vectored_interrupt_source_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_vga_text_interface_t_type_p;
};

%init (python) {
#ifdef struct_vga_text_interface_t_type_p
	struct_vga_text_interface_t_type_p = VT_lookup_python_opaque_type("api.vga_text_interface_t");
#endif /* struct_vga_text_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_vga_text_update_interface_t_type_p;
};

%init (python) {
#ifdef struct_vga_text_update_interface_t_type_p
	struct_vga_text_update_interface_t_type_p = VT_lookup_python_opaque_type("api.vga_text_update_interface_t");
#endif /* struct_vga_text_update_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_vga_update_interface_t_type_p;
};

%init (python) {
#ifdef struct_vga_update_interface_t_type_p
	struct_vga_update_interface_t_type_p = VT_lookup_python_opaque_type("api.vga_update_interface_t");
#endif /* struct_vga_update_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_virtual_data_breakpoint_interface_t_type_p;
};

%init (python) {
#ifdef struct_virtual_data_breakpoint_interface_t_type_p
	struct_virtual_data_breakpoint_interface_t_type_p = VT_lookup_python_opaque_type("api.virtual_data_breakpoint_interface_t");
#endif /* struct_virtual_data_breakpoint_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_virtual_instruction_breakpoint_interface_t_type_p;
};

%init (python) {
#ifdef struct_virtual_instruction_breakpoint_interface_t_type_p
	struct_virtual_instruction_breakpoint_interface_t_type_p = VT_lookup_python_opaque_type("api.virtual_instruction_breakpoint_interface_t");
#endif /* struct_virtual_instruction_breakpoint_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_vmp_interface_t_type_p;
};

%init (python) {
#ifdef struct_vmp_interface_t_type_p
	struct_vmp_interface_t_type_p = VT_lookup_python_opaque_type("api.vmp_interface_t");
#endif /* struct_vmp_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_vmx_instrumentation_subscribe_interface_t_type_p;
};

%init (python) {
#ifdef struct_vmx_instrumentation_subscribe_interface_t_type_p
	struct_vmx_instrumentation_subscribe_interface_t_type_p = VT_lookup_python_opaque_type("api.vmx_instrumentation_subscribe_interface_t");
#endif /* struct_vmx_instrumentation_subscribe_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_vnc_server_v2_interface_t_type_p;
};

%init (python) {
#ifdef struct_vnc_server_v2_interface_t_type_p
	struct_vnc_server_v2_interface_t_type_p = VT_lookup_python_opaque_type("api.vnc_server_v2_interface_t");
#endif /* struct_vnc_server_v2_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_winsome_console_interface_t_type_p;
};

%init (python) {
#ifdef struct_winsome_console_interface_t_type_p
	struct_winsome_console_interface_t_type_p = VT_lookup_python_opaque_type("api.winsome_console_interface_t");
#endif /* struct_winsome_console_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_x86_interface_t_type_p;
};

%init (python) {
#ifdef struct_x86_interface_t_type_p
	struct_x86_interface_t_type_p = VT_lookup_python_opaque_type("api.x86_interface_t");
#endif /* struct_x86_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_tagged_linear_address_t_type_p;
};

%init (python) {
#ifdef struct_tagged_linear_address_t_type_p
	struct_tagged_linear_address_t_type_p = VT_lookup_python_opaque_type("api.tagged_linear_address_t");
#endif /* struct_tagged_linear_address_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_x86_access_type_interface_t_type_p;
};

%init (python) {
#ifdef struct_x86_access_type_interface_t_type_p
	struct_x86_access_type_interface_t_type_p = VT_lookup_python_opaque_type("api.x86_access_type_interface_t");
#endif /* struct_x86_access_type_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_x86_address_query_interface_t_type_p;
};

%init (python) {
#ifdef struct_x86_address_query_interface_t_type_p
	struct_x86_address_query_interface_t_type_p = VT_lookup_python_opaque_type("api.x86_address_query_interface_t");
#endif /* struct_x86_address_query_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_x86_cache_flush_interface_t_type_p;
};

%init (python) {
#ifdef struct_x86_cache_flush_interface_t_type_p
	struct_x86_cache_flush_interface_t_type_p = VT_lookup_python_opaque_type("api.x86_cache_flush_interface_t");
#endif /* struct_x86_cache_flush_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_x86_cpuid_interface_t_type_p;
};

%init (python) {
#ifdef struct_x86_cpuid_interface_t_type_p
	struct_x86_cpuid_interface_t_type_p = VT_lookup_python_opaque_type("api.x86_cpuid_interface_t");
#endif /* struct_x86_cpuid_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_cpuid_ret_t_type_p;
};

%init (python) {
#ifdef struct_cpuid_ret_t_type_p
	struct_cpuid_ret_t_type_p = VT_lookup_python_opaque_type("api.cpuid_ret_t");
#endif /* struct_cpuid_ret_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_x86_cpuid_query_interface_t_type_p;
};

%init (python) {
#ifdef struct_x86_cpuid_query_interface_t_type_p
	struct_x86_cpuid_query_interface_t_type_p = VT_lookup_python_opaque_type("api.x86_cpuid_query_interface_t");
#endif /* struct_x86_cpuid_query_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_cpuid_value_t_type_p;
};

%init (python) {
#ifdef struct_cpuid_value_t_type_p
	struct_cpuid_value_t_type_p = VT_lookup_python_opaque_type("api.cpuid_value_t");
#endif /* struct_cpuid_value_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_x86_cstate_interface_t_type_p;
};

%init (python) {
#ifdef struct_x86_cstate_interface_t_type_p
	struct_x86_cstate_interface_t_type_p = VT_lookup_python_opaque_type("api.x86_cstate_interface_t");
#endif /* struct_x86_cstate_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_x86_cstate_t_type_p;
};

%init (python) {
#ifdef struct_x86_cstate_t_type_p
	struct_x86_cstate_t_type_p = VT_lookup_python_opaque_type("api.x86_cstate_t");
#endif /* struct_x86_cstate_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_x86_cstate_notification_interface_t_type_p;
};

%init (python) {
#ifdef struct_x86_cstate_notification_interface_t_type_p
	struct_x86_cstate_notification_interface_t_type_p = VT_lookup_python_opaque_type("api.x86_cstate_notification_interface_t");
#endif /* struct_x86_cstate_notification_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_x86_ept_interface_t_type_p;
};

%init (python) {
#ifdef struct_x86_ept_interface_t_type_p
	struct_x86_ept_interface_t_type_p = VT_lookup_python_opaque_type("api.x86_ept_interface_t");
#endif /* struct_x86_ept_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_x86_exception_interface_t_type_p;
};

%init (python) {
#ifdef struct_x86_exception_interface_t_type_p
	struct_x86_exception_interface_t_type_p = VT_lookup_python_opaque_type("api.x86_exception_interface_t");
#endif /* struct_x86_exception_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_x86_exception_query_interface_t_type_p;
};

%init (python) {
#ifdef struct_x86_exception_query_interface_t_type_p
	struct_x86_exception_query_interface_t_type_p = VT_lookup_python_opaque_type("api.x86_exception_query_interface_t");
#endif /* struct_x86_exception_query_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_x86_instruction_query_interface_t_type_p;
};

%init (python) {
#ifdef struct_x86_instruction_query_interface_t_type_p
	struct_x86_instruction_query_interface_t_type_p = VT_lookup_python_opaque_type("api.x86_instruction_query_interface_t");
#endif /* struct_x86_instruction_query_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_x86_instrumentation_subscribe_interface_t_type_p;
};

%init (python) {
#ifdef struct_x86_instrumentation_subscribe_interface_t_type_p
	struct_x86_instrumentation_subscribe_interface_t_type_p = VT_lookup_python_opaque_type("api.x86_instrumentation_subscribe_interface_t");
#endif /* struct_x86_instrumentation_subscribe_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_x86_instrumentation_subscribe_v2_interface_t_type_p;
};

%init (python) {
#ifdef struct_x86_instrumentation_subscribe_v2_interface_t_type_p
	struct_x86_instrumentation_subscribe_v2_interface_t_type_p = VT_lookup_python_opaque_type("api.x86_instrumentation_subscribe_v2_interface_t");
#endif /* struct_x86_instrumentation_subscribe_v2_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_x86_memory_access_interface_t_type_p;
};

%init (python) {
#ifdef struct_x86_memory_access_interface_t_type_p
	struct_x86_memory_access_interface_t_type_p = VT_lookup_python_opaque_type("api.x86_memory_access_interface_t");
#endif /* struct_x86_memory_access_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_x86_memory_query_interface_t_type_p;
};

%init (python) {
#ifdef struct_x86_memory_query_interface_t_type_p
	struct_x86_memory_query_interface_t_type_p = VT_lookup_python_opaque_type("api.x86_memory_query_interface_t");
#endif /* struct_x86_memory_query_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_x86_msr_interface_t_type_p;
};

%init (python) {
#ifdef struct_x86_msr_interface_t_type_p
	struct_x86_msr_interface_t_type_p = VT_lookup_python_opaque_type("api.x86_msr_interface_t");
#endif /* struct_x86_msr_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_x86_msr_getter_ret_t_type_p;
};

%init (python) {
#ifdef struct_x86_msr_getter_ret_t_type_p
	struct_x86_msr_getter_ret_t_type_p = VT_lookup_python_opaque_type("api.x86_msr_getter_ret_t");
#endif /* struct_x86_msr_getter_ret_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_x86_pkg_cstate_interface_t_type_p;
};

%init (python) {
#ifdef struct_x86_pkg_cstate_interface_t_type_p
	struct_x86_pkg_cstate_interface_t_type_p = VT_lookup_python_opaque_type("api.x86_pkg_cstate_interface_t");
#endif /* struct_x86_pkg_cstate_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_x86_reg_access_interface_t_type_p;
};

%init (python) {
#ifdef struct_x86_reg_access_interface_t_type_p
	struct_x86_reg_access_interface_t_type_p = VT_lookup_python_opaque_type("api.x86_reg_access_interface_t");
#endif /* struct_x86_reg_access_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_x86_fpu_env_t_type_p;
};

%init (python) {
#ifdef struct_x86_fpu_env_t_type_p
	struct_x86_fpu_env_t_type_p = VT_lookup_python_opaque_type("api.x86_fpu_env_t");
#endif /* struct_x86_fpu_env_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_x86_fpu_reg_t_type_p;
};

%init (python) {
#ifdef struct_x86_fpu_reg_t_type_p
	struct_x86_fpu_reg_t_type_p = VT_lookup_python_opaque_type("api.x86_fpu_reg_t");
#endif /* struct_x86_fpu_reg_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_x86_monitor_info_t_type_p;
};

%init (python) {
#ifdef struct_x86_monitor_info_t_type_p
	struct_x86_monitor_info_t_type_p = VT_lookup_python_opaque_type("api.x86_monitor_info_t");
#endif /* struct_x86_monitor_info_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_x86_mwait_info_t_type_p;
};

%init (python) {
#ifdef struct_x86_mwait_info_t_type_p
	struct_x86_mwait_info_t_type_p = VT_lookup_python_opaque_type("api.x86_mwait_info_t");
#endif /* struct_x86_mwait_info_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_x86_pending_debug_exc_t_type_p;
};

%init (python) {
#ifdef struct_x86_pending_debug_exc_t_type_p
	struct_x86_pending_debug_exc_t_type_p = VT_lookup_python_opaque_type("api.x86_pending_debug_exc_t");
#endif /* struct_x86_pending_debug_exc_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_x86_seg_reg_t_type_p;
};

%init (python) {
#ifdef struct_x86_seg_reg_t_type_p
	struct_x86_seg_reg_t_type_p = VT_lookup_python_opaque_type("api.x86_seg_reg_t");
#endif /* struct_x86_seg_reg_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_x86_system_seg_reg_t_type_p;
};

%init (python) {
#ifdef struct_x86_system_seg_reg_t_type_p
	struct_x86_system_seg_reg_t_type_p = VT_lookup_python_opaque_type("api.x86_system_seg_reg_t");
#endif /* struct_x86_system_seg_reg_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_xmm_reg_t_type_p;
};

%init (python) {
#ifdef struct_xmm_reg_t_type_p
	struct_xmm_reg_t_type_p = VT_lookup_python_opaque_type("api.xmm_reg_t");
#endif /* struct_xmm_reg_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_x86_xmode_info_t_type_p;
};

%init (python) {
#ifdef struct_x86_xmode_info_t_type_p
	struct_x86_xmode_info_t_type_p = VT_lookup_python_opaque_type("api.x86_xmode_info_t");
#endif /* struct_x86_xmode_info_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_ymm_reg_t_type_p;
};

%init (python) {
#ifdef struct_ymm_reg_t_type_p
	struct_ymm_reg_t_type_p = VT_lookup_python_opaque_type("api.ymm_reg_t");
#endif /* struct_ymm_reg_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_x86_smm_interface_t_type_p;
};

%init (python) {
#ifdef struct_x86_smm_interface_t_type_p
	struct_x86_smm_interface_t_type_p = VT_lookup_python_opaque_type("api.x86_smm_interface_t");
#endif /* struct_x86_smm_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_x86_smm_state_interface_t_type_p;
};

%init (python) {
#ifdef struct_x86_smm_state_interface_t_type_p
	struct_x86_smm_state_interface_t_type_p = VT_lookup_python_opaque_type("api.x86_smm_state_interface_t");
#endif /* struct_x86_smm_state_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_smm_reg_state_t_type_p;
};

%init (python) {
#ifdef struct_smm_reg_state_t_type_p
	struct_smm_reg_state_t_type_p = VT_lookup_python_opaque_type("api.smm_reg_state_t");
#endif /* struct_smm_reg_state_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_x86_tlb_interface_t_type_p;
};

%init (python) {
#ifdef struct_x86_tlb_interface_t_type_p
	struct_x86_tlb_interface_t_type_p = VT_lookup_python_opaque_type("api.x86_tlb_interface_t");
#endif /* struct_x86_tlb_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_tagged_physical_address_t_type_p;
};

%init (python) {
#ifdef struct_tagged_physical_address_t_type_p
	struct_tagged_physical_address_t_type_p = VT_lookup_python_opaque_type("api.tagged_physical_address_t");
#endif /* struct_tagged_physical_address_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_x86_tlb_v2_interface_t_type_p;
};

%init (python) {
#ifdef struct_x86_tlb_v2_interface_t_type_p
	struct_x86_tlb_v2_interface_t_type_p = VT_lookup_python_opaque_type("api.x86_tlb_v2_interface_t");
#endif /* struct_x86_tlb_v2_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_x86_tlb_attrs_t_type_p;
};

%init (python) {
#ifdef struct_x86_tlb_attrs_t_type_p
	struct_x86_tlb_attrs_t_type_p = VT_lookup_python_opaque_type("api.x86_tlb_attrs_t");
#endif /* struct_x86_tlb_attrs_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_x86_tlb_entry_t_type_p;
};

%init (python) {
#ifdef struct_x86_tlb_entry_t_type_p
	struct_x86_tlb_entry_t_type_p = VT_lookup_python_opaque_type("api.x86_tlb_entry_t");
#endif /* struct_x86_tlb_entry_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_x86_tlb_v3_interface_t_type_p;
};

%init (python) {
#ifdef struct_x86_tlb_v3_interface_t_type_p
	struct_x86_tlb_v3_interface_t_type_p = VT_lookup_python_opaque_type("api.x86_tlb_v3_interface_t");
#endif /* struct_x86_tlb_v3_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_x86_tlb_attrs_v3_t_type_p;
};

%init (python) {
#ifdef struct_x86_tlb_attrs_v3_t_type_p
	struct_x86_tlb_attrs_v3_t_type_p = VT_lookup_python_opaque_type("api.x86_tlb_attrs_v3_t");
#endif /* struct_x86_tlb_attrs_v3_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_x86_tlb_entry_v3_t_type_p;
};

%init (python) {
#ifdef struct_x86_tlb_entry_v3_t_type_p
	struct_x86_tlb_entry_v3_t_type_p = VT_lookup_python_opaque_type("api.x86_tlb_entry_v3_t");
#endif /* struct_x86_tlb_entry_v3_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_x86_vmp_control_interface_t_type_p;
};

%init (python) {
#ifdef struct_x86_vmp_control_interface_t_type_p
	struct_x86_vmp_control_interface_t_type_p = VT_lookup_python_opaque_type("api.x86_vmp_control_interface_t");
#endif /* struct_x86_vmp_control_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_xed_access_interface_t_type_p;
};

%init (python) {
#ifdef struct_xed_access_interface_t_type_p
	struct_xed_access_interface_t_type_p = VT_lookup_python_opaque_type("api.xed_access_interface_t");
#endif /* struct_xed_access_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_xtensa_export_state_interface_t_type_p;
};

%init (python) {
#ifdef struct_xtensa_export_state_interface_t_type_p
	struct_xtensa_export_state_interface_t_type_p = VT_lookup_python_opaque_type("api.xtensa_export_state_interface_t");
#endif /* struct_xtensa_export_state_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_xtensa_import_wire_interface_t_type_p;
};

%init (python) {
#ifdef struct_xtensa_import_wire_interface_t_type_p
	struct_xtensa_import_wire_interface_t_type_p = VT_lookup_python_opaque_type("api.xtensa_import_wire_interface_t");
#endif /* struct_xtensa_import_wire_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_xtensa_input_queue_interface_t_type_p;
};

%init (python) {
#ifdef struct_xtensa_input_queue_interface_t_type_p
	struct_xtensa_input_queue_interface_t_type_p = VT_lookup_python_opaque_type("api.xtensa_input_queue_interface_t");
#endif /* struct_xtensa_input_queue_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_xtensa_internal_memories_interface_t_type_p;
};

%init (python) {
#ifdef struct_xtensa_internal_memories_interface_t_type_p
	struct_xtensa_internal_memories_interface_t_type_p = VT_lookup_python_opaque_type("api.xtensa_internal_memories_interface_t");
#endif /* struct_xtensa_internal_memories_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_xtensa_lookup_interface_t_type_p;
};

%init (python) {
#ifdef struct_xtensa_lookup_interface_t_type_p
	struct_xtensa_lookup_interface_t_type_p = VT_lookup_python_opaque_type("api.xtensa_lookup_interface_t");
#endif /* struct_xtensa_lookup_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_xtensa_mpu_lookup_interface_t_type_p;
};

%init (python) {
#ifdef struct_xtensa_mpu_lookup_interface_t_type_p
	struct_xtensa_mpu_lookup_interface_t_type_p = VT_lookup_python_opaque_type("api.xtensa_mpu_lookup_interface_t");
#endif /* struct_xtensa_mpu_lookup_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_xtensa_mpu_lookup_t_type_p;
};

%init (python) {
#ifdef struct_xtensa_mpu_lookup_t_type_p
	struct_xtensa_mpu_lookup_t_type_p = VT_lookup_python_opaque_type("api.xtensa_mpu_lookup_t");
#endif /* struct_xtensa_mpu_lookup_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_xtensa_output_queue_interface_t_type_p;
};

%init (python) {
#ifdef struct_xtensa_output_queue_interface_t_type_p
	struct_xtensa_output_queue_interface_t_type_p = VT_lookup_python_opaque_type("api.xtensa_output_queue_interface_t");
#endif /* struct_xtensa_output_queue_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_xtensa_wwdt_config_interface_t_type_p;
};

%init (python) {
#ifdef struct_xtensa_wwdt_config_interface_t_type_p
	struct_xtensa_wwdt_config_interface_t_type_p = VT_lookup_python_opaque_type("api.xtensa_wwdt_config_interface_t");
#endif /* struct_xtensa_wwdt_config_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_xtensa_wwdt_faultinfo_interface_t_type_p;
};

%init (python) {
#ifdef struct_xtensa_wwdt_faultinfo_interface_t_type_p
	struct_xtensa_wwdt_faultinfo_interface_t_type_p = VT_lookup_python_opaque_type("api.xtensa_wwdt_faultinfo_interface_t");
#endif /* struct_xtensa_wwdt_faultinfo_interface_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_completion_t_type_p;
};

%init (python) {
#ifdef struct_completion_t_type_p
	struct_completion_t_type_p = VT_lookup_python_opaque_type("api.completion_t");
#endif /* struct_completion_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_class_data_t_type_p;
};

%init (python) {
#ifdef struct_class_data_t_type_p
	struct_class_data_t_type_p = VT_lookup_python_opaque_type("api.class_data_t");
#endif /* struct_class_data_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_class_info_t_type_p;
};

%init (python) {
#ifdef struct_class_info_t_type_p
	struct_class_info_t_type_p = VT_lookup_python_opaque_type("api.class_info_t");
#endif /* struct_class_info_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_global_notifier_callback_t_type_p;
};

%init (python) {
#ifdef struct_global_notifier_callback_t_type_p
	struct_global_notifier_callback_t_type_p = VT_lookup_python_opaque_type("api.global_notifier_callback_t");
#endif /* struct_global_notifier_callback_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_atom_t_type_p;
};

%init (python) {
#ifdef struct_atom_t_type_p
	struct_atom_t_type_p = VT_lookup_python_opaque_type("api.atom_t");
#endif /* struct_atom_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_transaction_trace_atom_access_t_type_p;
};

%init (python) {
#ifdef struct_transaction_trace_atom_access_t_type_p
	struct_transaction_trace_atom_access_t_type_p = VT_lookup_python_opaque_type("api.transaction_trace_atom_access_t");
#endif /* struct_transaction_trace_atom_access_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_transaction_target_type_ret_t_type_p;
};

%init (python) {
#ifdef struct_transaction_target_type_ret_t_type_p
	struct_transaction_target_type_ret_t_type_p = VT_lookup_python_opaque_type("api.transaction_target_type_ret_t");
#endif /* struct_transaction_target_type_ret_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_notifier_handle_t_type_p;
};

%init (python) {
#ifdef struct_notifier_handle_t_type_p
	struct_notifier_handle_t_type_p = VT_lookup_python_opaque_type("api.notifier_handle_t");
#endif /* struct_notifier_handle_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_domain_lock_t_type_p;
};

%init (python) {
#ifdef struct_domain_lock_t_type_p
	struct_domain_lock_t_type_p = VT_lookup_python_opaque_type("api.domain_lock_t");
#endif /* struct_domain_lock_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_wait_handler_t_type_p;
};

%init (python) {
#ifdef struct_wait_handler_t_type_p
	struct_wait_handler_t_type_p = VT_lookup_python_opaque_type("api.wait_handler_t");
#endif /* struct_wait_handler_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_execute_environ_t_type_p;
};

%init (python) {
#ifdef struct_execute_environ_t_type_p
	struct_execute_environ_t_type_p = VT_lookup_python_opaque_type("api.execute_environ_t");
#endif /* struct_execute_environ_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_profile_area_t_type_p;
};

%init (python) {
#ifdef struct_profile_area_t_type_p
	struct_profile_area_t_type_p = VT_lookup_python_opaque_type("api.profile_area_t");
#endif /* struct_profile_area_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_probe_result_t_type_p;
};

%init (python) {
#ifdef struct_probe_result_t_type_p
	struct_probe_result_t_type_p = VT_lookup_python_opaque_type("api.probe_result_t");
#endif /* struct_probe_result_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_terminus_cb_data_t_type_p;
};

%init (python) {
#ifdef struct_terminus_cb_data_t_type_p
	struct_terminus_cb_data_t_type_p = VT_lookup_python_opaque_type("api.terminus_cb_data_t");
#endif /* struct_terminus_cb_data_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_deferred_transaction_info_t_type_p;
};

%init (python) {
#ifdef struct_deferred_transaction_info_t_type_p
	struct_deferred_transaction_info_t_type_p = VT_lookup_python_opaque_type("api.deferred_transaction_info_t");
#endif /* struct_deferred_transaction_info_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_host_hypervisor_info_t_type_p;
};

%init (python) {
#ifdef struct_host_hypervisor_info_t_type_p
	struct_host_hypervisor_info_t_type_p = VT_lookup_python_opaque_type("api.host_hypervisor_info_t");
#endif /* struct_host_hypervisor_info_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_thread_domain_holder_t_type_p;
};

%init (python) {
#ifdef struct_thread_domain_holder_t_type_p
	struct_thread_domain_holder_t_type_p = VT_lookup_python_opaque_type("api.thread_domain_holder_t");
#endif /* struct_thread_domain_holder_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_thread_domain_t_type_p;
};

%init (python) {
#ifdef struct_thread_domain_t_type_p
	struct_thread_domain_t_type_p = VT_lookup_python_opaque_type("api.thread_domain_t");
#endif /* struct_thread_domain_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_thread_domain_group_t_type_p;
};

%init (python) {
#ifdef struct_thread_domain_group_t_type_p
	struct_thread_domain_group_t_type_p = VT_lookup_python_opaque_type("api.thread_domain_group_t");
#endif /* struct_thread_domain_group_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_pcie_error_ret_t_type_p;
};

%init (python) {
#ifdef struct_pcie_error_ret_t_type_p
	struct_pcie_error_ret_t_type_p = VT_lookup_python_opaque_type("api.pcie_error_ret_t");
#endif /* struct_pcie_error_ret_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_pcie_byte_count_ret_t_type_p;
};

%init (python) {
#ifdef struct_pcie_byte_count_ret_t_type_p
	struct_pcie_byte_count_ret_t_type_p = VT_lookup_python_opaque_type("api.pcie_byte_count_ret_t");
#endif /* struct_pcie_byte_count_ret_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_pcie_link_negotiation_t_type_p;
};

%init (python) {
#ifdef struct_pcie_link_negotiation_t_type_p
	struct_pcie_link_negotiation_t_type_p = VT_lookup_python_opaque_type("api.pcie_link_negotiation_t");
#endif /* struct_pcie_link_negotiation_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_pcie_ide_secured_t_type_p;
};

%init (python) {
#ifdef struct_pcie_ide_secured_t_type_p
	struct_pcie_ide_secured_t_type_p = VT_lookup_python_opaque_type("api.pcie_ide_secured_t");
#endif /* struct_pcie_ide_secured_t_type_p */
};

%body (python) {
static UNUSED opaque_type_t *struct_riscv_io_error_ret_t_type_p;
};

%init (python) {
#ifdef struct_riscv_io_error_ret_t_type_p
	struct_riscv_io_error_ret_t_type_p = VT_lookup_python_opaque_type("api.riscv_io_error_ret_t");
#endif /* struct_riscv_io_error_ret_t_type_p */
};

%except (python, in) 
	{ VT_handle_python_exception($source, $signature); };
%except (python, out) SIM_new_map_target
	{
        if (pyexception || check_sim_exception()) {
                if (result)
                        SIM_free_map_target(result);
                $error;
        }
};
%except (python, out) DBG_make_pb_page
	{
        if (pyexception || check_sim_exception()) {
                if (result)
                        MM_FREE(result);
                $error;
        }
};
%except (python, out) SIM_register_event
	{
        if (pyexception || check_sim_exception()) {
                if (result) {
                        if (result->conf_class && result->name) {
                                ht_remove_str(
                                        &result->conf_class->event_classes,
                                        result->name);
                        }
                        if (result->name)
                                MM_FREE((char *)result->name);
                        MM_FREE(result);
                }
                $error;
        }
};
%except (python, out) VT_register_python_thread;
%except (python, out) SIM_register_work;
%except (python, out) CORE_python_flush;
%except (python, out) CORE_python_write;
%except (python, out) SIM_thread_safe_callback;
%except (python, out) 
	{ if (pyexception || check_sim_exception()) $error; };
/* user */
%typemap (python, in) buffer_t {

        if (!VT_pyobject_to_buffer_t(&$target, $source))
                $error;
} default_value { (buffer_t){0} };

/* user */
%typemap (python, out) buffer_t {

        $target = VT_buffer_t_to_pyobject($source);
} pycleanup {
        VT_buffer_t_pyobject_invalidate($target);
};

/* user */
%typemap (python, in) dbuffer_t * {
        char *buf = NULL;
        Py_ssize_t len;

        if (PyBytes_AsStringAndSize($source, &buf, &len) >= 0) {
                $target = new_dbuffer();
                size_t ulen = (size_t)len;
                memcpy(dbuffer_append($target, ulen), buf, ulen);
        } else {
                $error;
        }
} cleanup {
        dbuffer_free($target);
};

/* user */
%typemap (python, out) dbuffer_t * {
        if ($source == NULL) {
                $target = Py_NewRef(Py_None);
        } else {
                $target = PyBytes_FromStringAndSize(
                        (char *)dbuffer_read_all($source),
                        (Py_ssize_t)dbuffer_len($source));
        }
} cleanup {
        dbuffer_free($source);
};

/* user */
%typemap (python, out) obj_hap_func_t;

/* user */
%typemap (python, in) obj_hap_func_t {
        if (!VT_get_c_py_wrap((const void **)&$target, $source,
                              &py_wrap_data_FKvoidRKvoid,
                              true))
                $error;
};

/* user */
%typemap (python, in) arm_memory_transaction_t * {
        void *tgt = NULL;
        if (VT_get_py_opaque_arm_transaction_upcast(&tgt, $source)) {
                $target = tgt;
        } else {
                $error;
        }
};

/* user */
%typemap (python, out) arm_memory_transaction_t * { $target = VT_python_wrap_arm_transaction_upcast($source); };

/* user */
%typemap (python, in) pci_memory_transaction_t * {
        void *tgt = NULL;
        if (VT_get_py_opaque_pci_transaction_upcast(&tgt, $source)) {
                $target = tgt;
        } else {
                $error;
        }
};

/* user */
%typemap (python, out) pci_memory_transaction_t * { $target = VT_python_wrap_pci_transaction_upcast($source); };

/* user */
%typemap (python, in) ppc_memory_transaction_t * {
        void *tgt = NULL;
        if (VT_get_py_opaque_ppc_transaction_upcast(&tgt, $source)) {
                $target = tgt;
        } else {
                $error;
        }
};

/* user */
%typemap (python, out) ppc_memory_transaction_t * { $target = VT_python_wrap_ppc_transaction_upcast($source); };

/* user */
%typemap (python, in) mips_memory_transaction_t * {
        void *tgt = NULL;
        if (VT_get_py_opaque_mips_transaction_upcast(&tgt, $source)) {
                $target = tgt;
        } else {
                $error;
        }
};

/* user */
%typemap (python, out) mips_memory_transaction_t * { $target = VT_python_wrap_mips_transaction_upcast($source); };

/* user */
%typemap (python, in) x86_memory_transaction_t * {
        void *tgt = NULL;
        if (VT_get_py_opaque_x86_transaction_upcast(&tgt, $source)) {
                $target = tgt;
        } else {
                $error;
        }
};

/* user */
%typemap (python, out) x86_memory_transaction_t * { $target = VT_python_wrap_x86_transaction_upcast($source); };

/* user */
%typemap (python, in) generic_transaction_t {
        void *tgt = NULL;
        if (VT_get_py_opaque_generic_transaction(&tgt, $source)) {
                $target = *(generic_transaction_t *)tgt;
        } else {
                $error;
        }
} default_value { (generic_transaction_t){0} };

/* user */
%typemap (python, out) generic_transaction_t {
        $target = VT_python_wrap_generic_transaction(&$source);
};

/* user */
%typemap (python, in) generic_transaction_t * {
        void *tgt = NULL;
        if (VT_get_py_opaque_generic_transaction(&tgt, $source)) {
                $target = tgt;
        } else {
                $error;
        }
};

/* user */
%typemap (python, out) generic_transaction_t * { $target = VT_python_wrap_generic_transaction($source); };

/* user */
%typemap (python, in) struct transaction {
        void *tgt = NULL;
        if (VT_get_py_opaque_transaction(&tgt, $source)) {
                $target = *(struct transaction *)tgt;
        } else {
                $error;
        }
} default_value { (struct transaction){0} };

/* user */
%typemap (python, out) struct transaction {
        $target = VT_python_wrap_transaction(&$source);
};

/* user */
%typemap (python, in) struct transaction * {
        void *tgt = NULL;
        if (VT_get_py_opaque_transaction(&tgt, $source)) {
                $target = tgt;
        } else {
                $error;
        }
};

/* user */
%typemap (python, out) struct transaction * { $target = VT_python_wrap_transaction($source); };

/* user */
%typemap (python, in) struct conf_object {
        void *tgt = NULL;
        if (VT_get_py_opaque_conf_object(&tgt, $source)) {
                $target = *(struct conf_object *)tgt;
        } else {
                $error;
        }
} default_value { (struct conf_object){0} };

/* user */
%typemap (python, out) struct conf_object {
        $target = VT_python_wrap_conf_object(&$source);
};

/* user */
%typemap (python, in) struct conf_object * {
        void *tgt = NULL;
        if (VT_get_py_opaque_conf_object(&tgt, $source)) {
                $target = tgt;
        } else {
                $error;
        }
};

/* user */
%typemap (python, out) struct conf_object * { $target = VT_python_wrap_conf_object($source); };

/* user */
%typemap (python, in) struct conf_class {
        void *tgt = NULL;
        if (VT_get_conf_class(&tgt, $source)) {
                $target = *(struct conf_class *)tgt;
        } else {
                $error;
        }
} default_value { (struct conf_class){0} };

/* user */
%typemap (python, out) struct conf_class {
        if ($source == NULL) {
                $target = Py_None;
        } else {
                $target = VT_python_wrap_conf_class(&$source);
        }
};

/* user */
%typemap (python, out) struct conf_class * {
        if ($source == NULL) {
                $target = Py_None;
        } else {
                $target = VT_python_wrap_conf_class($source);
        }
};

/* user */
%typemap (python, in) struct conf_class * { if (!VT_get_conf_class($source, (struct conf_class **)&$target)) $error; };

/* user */
%typemap (python, in) sim_exception_t {
        PyObject *obj;

        if (!PyObject_HasAttrString($source, "number")) {
                PyErr_SetString(PyExc_TypeError,
                                "Cannot convert to sim_exception_t");
                $error;
        } else {
                obj = PyObject_GetAttrString($source, "number");
                if (PyLong_Check(obj)) {
                        $target = (sim_exception_t)PyLong_AsLong(obj);
                } else {
                        PyErr_SetString(PyExc_TypeError,
                                        "Cannot convert to sim_exception_t");
                        $error;
                }
                Py_DECREF(obj);
        }
} default_value { SimExc_No_Exception };

/* user */
%typemap (python, in) frags_t * {
        char *buf = NULL;
        Py_ssize_t len;

        if (PyBytes_AsStringAndSize($source, &buf, &len) >= 0) {
                size_t ulen = (size_t)len;
                uint8 *bufcopy = MM_MALLOC(ulen, uint8);
                memcpy(bufcopy, buf, ulen);
                frags_t *fragcopy = MM_MALLOC(1, frags_t);
                frags_init(fragcopy);
                frags_add(fragcopy, bufcopy, ulen);
                $target = fragcopy;
        } else {
                $error;
        }
} cleanup {
        if ($target) {
                MM_FREE((uint8 *)$target->fraglist[0].start);
                MM_FREE((frags_t *)$target);
        }
};

/* user */
%typemap (python, out) frags_t * {
        if ($source == NULL) {
                $target = Py_NewRef(Py_None);
        } else {


                uint8 p[frags_len($source)];
                frags_extract($source, p);
                $target = PyBytes_FromStringAndSize(
                        (char *)p,
                        (Py_ssize_t)frags_len($source));
        }
} cleanup {



        ERROR: Please do not return a frags_t. It cannot be freed.
};

/* user */
%typemap (python, in) byte_string_t {
        if (!VT_pyobject_to_byte_string($source, &$target))
                $error;
} cleanup {
        MM_FREE($target.str);
} default_value { (const byte_string_t){ .len = 0 } };

/* user */
%typemap (python, out) byte_string_t {
        VT_byte_string_to_pyobject($source, &$target);
} cleanup {
        MM_FREE($source.str);
};

/* user */
%typemap (python, in) cpu_bytes_t {
        char *buf = NULL;
        Py_ssize_t len;

        if (PyBytes_AsStringAndSize($source, &buf, &len) >= 0) {
                size_t ulen = (size_t)len;
                uint8 *bufcopy = MM_MALLOC(ulen, uint8);
                memcpy(bufcopy, buf, ulen);
                $target = (cpu_bytes_t){.size = ulen, .data = bufcopy};
        } else {
                $error;
        }
} cleanup {
        MM_FREE((uint8 *)$target.data);
} default_value { (const cpu_bytes_t){ .size = 0 } };

/* user */
%typemap (python, out) cpu_bytes_t {
        $target = PyBytes_FromStringAndSize((char *)$source.data,
                                            (Py_ssize_t)$source.size);
} cleanup {

};

/* user */
%typemap (python, in) bytes_t {
        char *buf = NULL;
        Py_ssize_t len;

        if (PyBytes_AsStringAndSize($source, &buf, &len) >= 0) {
                size_t ulen = (size_t)len;
                uint8 *bufcopy = MM_MALLOC(ulen, uint8);
                memcpy(bufcopy, buf, ulen);
                $target = (bytes_t){.len = ulen, .data = bufcopy};
        } else {
                $error;
        }
} cleanup {
        MM_FREE((uint8 *)$target.data);
} default_value { (const bytes_t){ .data = NULL } };

/* user */
%typemap (python, out) bytes_t {
        $target = PyBytes_FromStringAndSize((char *)$source.data,
                                            (Py_ssize_t)$source.len);
} cleanup {
        MM_FREE((uint8 *)$source.data);
};

/* user */
%typemap (python, in) exception_type_t { if (!VT_get_exception_type_t($source, &$target)) $error; } default_value { Sim_PE_Stop_Request };

/* user */
%typemap (python, in) set_error_t { if (!get_set_error_t($source, &$target)) $error; } default_value { Sim_Set_Illegal_Value };

/* user */
%typemap (python, in) attr_value_t {
        if (VT_python_obj_to_attr_value_with_error($source, &$target) < 0)
                $error;
} cleanup { SIM_attr_free(&$target); } default_value { SIM_make_attr_invalid() };

/* user */
%typemap (python, in) attr_value_t * {
        $target = &var_$target;
        if (VT_python_obj_to_attr_value_with_error($source, &var_$target) < 0)
                $error;
} decl {

        attr_value_t var_$target;
} cleanup { SIM_attr_free(&var_$target); };

/* user */
%typemap (python, out) attr_value_t {
        const char *err_msg = NULL;
        $target = VT_attr_value_to_python_obj(&$source, &err_msg);
        if ($target == NULL) {
                VT_raise_python_exception(SimExc_Attribute, err_msg);
                $error;
        }
} cleanup { SIM_attr_free(&$source); };

/* user */
%typemap (python, out) cbdata_call_t {
        cbdata_t *src = &$source;
        ASSERT(SIM_cbdata_type(src) == VT_python_cbdata_type());
        $target = Py_NewRef(SIM_cbdata_data(src));
};

/* user */
%typemap (python, in) cbdata_register_t {
        $target = SIM_make_cbdata(VT_python_cbdata_type(), Py_NewRef($source));
};

/* user */
%typemap (python, in) cbdata_call_t {
        if (PyObject_Type($source) != VT_cbdata_python_type()) {
                PyErr_SetString(PyExc_TypeError,
                                "not a Python-wrapped cbdata");
                $error;
        } else {
                python_cbdata_t *pycbd = (python_cbdata_t *)$source;
                $target = pycbd->cbdata;
        }
};

/* user */
%typemap (python, out) cbdata_register_t {
        $target = VT_make_python_cbdata($source);
};

/* user */
%typemap (python, in) cbdata_t *;

/* user */
%typemap (python, out) cbdata_t *;

/* user */
%typemap (python, out) attr_value_t * {
        const char *err_msg = NULL;
        $target = VT_attr_value_to_python_obj($source, &err_msg);
        if ($target == NULL) {
                VT_raise_python_exception(SimExc_Attribute, err_msg);
                $error;
        }
} cleanup {
        SIM_attr_free($source);
        MM_FREE($source);
};

/* generated */
%typemap (python, in) struct __anonymous1 {
#define struct_riscv_io_error_ret_t_type_p struct_riscv_io_error_ret_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_riscv_io_error_ret_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a riscv_io_error_ret_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(riscv_io_error_ret_t *, $source);
} default_value { (const riscv_io_error_ret_t){ .val = Riscv_No_IO_Error } };

/* generated */
%typemap (python, struct-out) struct __anonymous1 {
#define struct_riscv_io_error_ret_t_type_p struct_riscv_io_error_ret_t_type_p
$target = new_opaque_object(&$source, false, false, struct_riscv_io_error_ret_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous1 {
#define struct_riscv_io_error_ret_t_type_p struct_riscv_io_error_ret_t_type_p
	riscv_io_error_ret_t *src = MM_MALLOC(1, riscv_io_error_ret_t);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_riscv_io_error_ret_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous1 * {
#define struct_riscv_io_error_ret_t_type_p struct_riscv_io_error_ret_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_riscv_io_error_ret_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct __anonymous1 * {
#define struct_riscv_io_error_ret_t_type_p struct_riscv_io_error_ret_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_riscv_io_error_ret_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct __anonymous2 {
#define struct_pcie_ide_secured_t_type_p struct_pcie_ide_secured_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_pcie_ide_secured_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a pcie_ide_secured_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(pcie_ide_secured_t *, $source);
} default_value { (const pcie_ide_secured_t){ .t = 0 } };

/* generated */
%typemap (python, struct-out) struct __anonymous2 {
#define struct_pcie_ide_secured_t_type_p struct_pcie_ide_secured_t_type_p
$target = new_opaque_object(&$source, false, false, struct_pcie_ide_secured_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous2 {
#define struct_pcie_ide_secured_t_type_p struct_pcie_ide_secured_t_type_p
	pcie_ide_secured_t *src = MM_MALLOC(1, pcie_ide_secured_t);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_pcie_ide_secured_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous2 * {
#define struct_pcie_ide_secured_t_type_p struct_pcie_ide_secured_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_pcie_ide_secured_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct __anonymous2 * {
#define struct_pcie_ide_secured_t_type_p struct_pcie_ide_secured_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_pcie_ide_secured_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct __anonymous5 {
#define struct_pcie_link_negotiation_t_type_p struct_pcie_link_negotiation_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_pcie_link_negotiation_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a pcie_link_negotiation_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(pcie_link_negotiation_t *, $source);
} default_value { (const pcie_link_negotiation_t){ .maximum_link_speed = PCIE_Link_Speed_Undefined } };

/* generated */
%typemap (python, struct-out) struct __anonymous5 {
#define struct_pcie_link_negotiation_t_type_p struct_pcie_link_negotiation_t_type_p
$target = new_opaque_object(&$source, false, false, struct_pcie_link_negotiation_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous5 {
#define struct_pcie_link_negotiation_t_type_p struct_pcie_link_negotiation_t_type_p
	pcie_link_negotiation_t *src = MM_MALLOC(1, pcie_link_negotiation_t);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_pcie_link_negotiation_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous5 * {
#define struct_pcie_link_negotiation_t_type_p struct_pcie_link_negotiation_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_pcie_link_negotiation_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct __anonymous5 * {
#define struct_pcie_link_negotiation_t_type_p struct_pcie_link_negotiation_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_pcie_link_negotiation_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct __anonymous6 {
#define struct_pcie_byte_count_ret_t_type_p struct_pcie_byte_count_ret_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_pcie_byte_count_ret_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a pcie_byte_count_ret_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(pcie_byte_count_ret_t *, $source);
} default_value { (const pcie_byte_count_ret_t){ .val = 0 } };

/* generated */
%typemap (python, struct-out) struct __anonymous6 {
#define struct_pcie_byte_count_ret_t_type_p struct_pcie_byte_count_ret_t_type_p
$target = new_opaque_object(&$source, false, false, struct_pcie_byte_count_ret_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous6 {
#define struct_pcie_byte_count_ret_t_type_p struct_pcie_byte_count_ret_t_type_p
	pcie_byte_count_ret_t *src = MM_MALLOC(1, pcie_byte_count_ret_t);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_pcie_byte_count_ret_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous6 * {
#define struct_pcie_byte_count_ret_t_type_p struct_pcie_byte_count_ret_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_pcie_byte_count_ret_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct __anonymous6 * {
#define struct_pcie_byte_count_ret_t_type_p struct_pcie_byte_count_ret_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_pcie_byte_count_ret_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct __anonymous8 {
#define struct_pcie_error_ret_t_type_p struct_pcie_error_ret_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_pcie_error_ret_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a pcie_error_ret_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(pcie_error_ret_t *, $source);
} default_value { (const pcie_error_ret_t){ .val = PCIE_Error_Not_Set } };

/* generated */
%typemap (python, struct-out) struct __anonymous8 {
#define struct_pcie_error_ret_t_type_p struct_pcie_error_ret_t_type_p
$target = new_opaque_object(&$source, false, false, struct_pcie_error_ret_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous8 {
#define struct_pcie_error_ret_t_type_p struct_pcie_error_ret_t_type_p
	pcie_error_ret_t *src = MM_MALLOC(1, pcie_error_ret_t);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_pcie_error_ret_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous8 * {
#define struct_pcie_error_ret_t_type_p struct_pcie_error_ret_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_pcie_error_ret_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct __anonymous8 * {
#define struct_pcie_error_ret_t_type_p struct_pcie_error_ret_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_pcie_error_ret_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct thread_domain_group {
#define struct_thread_domain_group_t_type_p struct_thread_domain_group_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_thread_domain_group_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a thread_domain_group_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct thread_domain_group *, $source);
};

/* generated */
%typemap (python, struct-out) struct thread_domain_group {
#define struct_thread_domain_group_t_type_p struct_thread_domain_group_t_type_p
$target = new_opaque_object(&$source, false, false, struct_thread_domain_group_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct thread_domain_group {
#define struct_thread_domain_group_t_type_p struct_thread_domain_group_t_type_p
	struct thread_domain_group *src = MM_MALLOC(1, struct thread_domain_group);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_thread_domain_group_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct thread_domain_group * {
#define struct_thread_domain_group_t_type_p struct_thread_domain_group_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_thread_domain_group_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct thread_domain_group * {
#define struct_thread_domain_group_t_type_p struct_thread_domain_group_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_thread_domain_group_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct thread_domain {
#define struct_thread_domain_t_type_p struct_thread_domain_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_thread_domain_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a thread_domain_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct thread_domain *, $source);
};

/* generated */
%typemap (python, struct-out) struct thread_domain {
#define struct_thread_domain_t_type_p struct_thread_domain_t_type_p
$target = new_opaque_object(&$source, false, false, struct_thread_domain_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct thread_domain {
#define struct_thread_domain_t_type_p struct_thread_domain_t_type_p
	struct thread_domain *src = MM_MALLOC(1, struct thread_domain);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_thread_domain_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct thread_domain * {
#define struct_thread_domain_t_type_p struct_thread_domain_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_thread_domain_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct thread_domain * {
#define struct_thread_domain_t_type_p struct_thread_domain_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_thread_domain_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct thread_domain_holder {
#define struct_thread_domain_holder_t_type_p struct_thread_domain_holder_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_thread_domain_holder_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a thread_domain_holder_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct thread_domain_holder *, $source);
};

/* generated */
%typemap (python, struct-out) struct thread_domain_holder {
#define struct_thread_domain_holder_t_type_p struct_thread_domain_holder_t_type_p
$target = new_opaque_object(&$source, false, false, struct_thread_domain_holder_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct thread_domain_holder {
#define struct_thread_domain_holder_t_type_p struct_thread_domain_holder_t_type_p
	struct thread_domain_holder *src = MM_MALLOC(1, struct thread_domain_holder);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_thread_domain_holder_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct thread_domain_holder * {
#define struct_thread_domain_holder_t_type_p struct_thread_domain_holder_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_thread_domain_holder_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct thread_domain_holder * {
#define struct_thread_domain_holder_t_type_p struct_thread_domain_holder_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_thread_domain_holder_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct __anonymous13 {
#define struct_host_hypervisor_info_t_type_p struct_host_hypervisor_info_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_host_hypervisor_info_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a host_hypervisor_info_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(host_hypervisor_info_t *, $source);
} default_value { (const host_hypervisor_info_t){ .is_hv_detected = 0 } };

/* generated */
%typemap (python, struct-out) struct __anonymous13 {
#define struct_host_hypervisor_info_t_type_p struct_host_hypervisor_info_t_type_p
$target = new_opaque_object(&$source, false, false, struct_host_hypervisor_info_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous13 {
#define struct_host_hypervisor_info_t_type_p struct_host_hypervisor_info_t_type_p
	host_hypervisor_info_t *src = MM_MALLOC(1, host_hypervisor_info_t);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_host_hypervisor_info_t_type_p, NULL);
};

/* generated */
%typemap (python, out) host_hypervisor_info_t * {
#define struct_host_hypervisor_info_t_type_p struct_host_hypervisor_info_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_host_hypervisor_info_t_type_p, NULL);
};

/* generated */
%typemap (python, in) host_hypervisor_info_t * {
#define struct_host_hypervisor_info_t_type_p struct_host_hypervisor_info_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_host_hypervisor_info_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct __anonymous15 {
#define struct_deferred_transaction_info_t_type_p struct_deferred_transaction_info_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_deferred_transaction_info_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a deferred_transaction_info_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(deferred_transaction_info_t *, $source);
} default_value { (const deferred_transaction_info_t){ .t = NULL } };

/* generated */
%typemap (python, struct-out) struct __anonymous15 {
#define struct_deferred_transaction_info_t_type_p struct_deferred_transaction_info_t_type_p
$target = new_opaque_object(&$source, false, false, struct_deferred_transaction_info_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous15 {
#define struct_deferred_transaction_info_t_type_p struct_deferred_transaction_info_t_type_p
	deferred_transaction_info_t *src = MM_MALLOC(1, deferred_transaction_info_t);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_deferred_transaction_info_t_type_p, NULL);
};

/* generated */
%typemap (python, out) deferred_transaction_info_t * {
#define struct_deferred_transaction_info_t_type_p struct_deferred_transaction_info_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_deferred_transaction_info_t_type_p, NULL);
};

/* generated */
%typemap (python, in) deferred_transaction_info_t * {
#define struct_deferred_transaction_info_t_type_p struct_deferred_transaction_info_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_deferred_transaction_info_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct __anonymous16 {
#define struct_terminus_cb_data_t_type_p struct_terminus_cb_data_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_terminus_cb_data_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a terminus_cb_data_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(terminus_cb_data_t *, $source);
} default_value { (const terminus_cb_data_t){ .terminus = NULL } };

/* generated */
%typemap (python, struct-out) struct __anonymous16 {
#define struct_terminus_cb_data_t_type_p struct_terminus_cb_data_t_type_p
$target = new_opaque_object(&$source, false, false, struct_terminus_cb_data_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous16 {
#define struct_terminus_cb_data_t_type_p struct_terminus_cb_data_t_type_p
	terminus_cb_data_t *src = MM_MALLOC(1, terminus_cb_data_t);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_terminus_cb_data_t_type_p, NULL);
};

/* generated */
%typemap (python, out) terminus_cb_data_t * {
#define struct_terminus_cb_data_t_type_p struct_terminus_cb_data_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_terminus_cb_data_t_type_p, NULL);
};

/* generated */
%typemap (python, in) terminus_cb_data_t * {
#define struct_terminus_cb_data_t_type_p struct_terminus_cb_data_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_terminus_cb_data_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct __anonymous17 {
#define struct_probe_result_t_type_p struct_probe_result_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_probe_result_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a probe_result_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(probe_result_t *, $source);
} default_value { (const probe_result_t){ .address = 0 } };

/* generated */
%typemap (python, struct-out) struct __anonymous17 {
#define struct_probe_result_t_type_p struct_probe_result_t_type_p
$target = new_opaque_object(&$source, false, false, struct_probe_result_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous17 {
#define struct_probe_result_t_type_p struct_probe_result_t_type_p
	probe_result_t *src = MM_MALLOC(1, probe_result_t);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_probe_result_t_type_p, NULL);
};

/* generated */
%typemap (python, out) probe_result_t * {
#define struct_probe_result_t_type_p struct_probe_result_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_probe_result_t_type_p, NULL);
};

/* generated */
%typemap (python, in) probe_result_t * {
#define struct_probe_result_t_type_p struct_probe_result_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_probe_result_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct profile_area {
#define struct_profile_area_t_type_p struct_profile_area_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_profile_area_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a profile_area_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct profile_area *, $source);
};

/* generated */
%typemap (python, struct-out) struct profile_area {
#define struct_profile_area_t_type_p struct_profile_area_t_type_p
$target = new_opaque_object(&$source, false, false, struct_profile_area_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct profile_area {
#define struct_profile_area_t_type_p struct_profile_area_t_type_p
	struct profile_area *src = MM_MALLOC(1, struct profile_area);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_profile_area_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct profile_area * {
#define struct_profile_area_t_type_p struct_profile_area_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_profile_area_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct profile_area * {
#define struct_profile_area_t_type_p struct_profile_area_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_profile_area_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct execute_environ {
#define struct_execute_environ_t_type_p struct_execute_environ_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_execute_environ_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an execute_environ_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct execute_environ *, $source);
};

/* generated */
%typemap (python, struct-out) struct execute_environ {
#define struct_execute_environ_t_type_p struct_execute_environ_t_type_p
$target = new_opaque_object(&$source, false, false, struct_execute_environ_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct execute_environ {
#define struct_execute_environ_t_type_p struct_execute_environ_t_type_p
	struct execute_environ *src = MM_MALLOC(1, struct execute_environ);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_execute_environ_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct execute_environ * {
#define struct_execute_environ_t_type_p struct_execute_environ_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_execute_environ_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct execute_environ * {
#define struct_execute_environ_t_type_p struct_execute_environ_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_execute_environ_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct wait_handler {
#define struct_wait_handler_t_type_p struct_wait_handler_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_wait_handler_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a wait_handler_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct wait_handler *, $source);
};

/* generated */
%typemap (python, struct-out) struct wait_handler {
#define struct_wait_handler_t_type_p struct_wait_handler_t_type_p
$target = new_opaque_object(&$source, false, false, struct_wait_handler_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct wait_handler {
#define struct_wait_handler_t_type_p struct_wait_handler_t_type_p
	struct wait_handler *src = MM_MALLOC(1, struct wait_handler);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_wait_handler_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct wait_handler * {
#define struct_wait_handler_t_type_p struct_wait_handler_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_wait_handler_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct wait_handler * {
#define struct_wait_handler_t_type_p struct_wait_handler_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_wait_handler_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct domain_lock {
#define struct_domain_lock_t_type_p struct_domain_lock_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_domain_lock_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a domain_lock_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct domain_lock *, $source);
};

/* generated */
%typemap (python, struct-out) struct domain_lock {
#define struct_domain_lock_t_type_p struct_domain_lock_t_type_p
$target = new_opaque_object(&$source, false, false, struct_domain_lock_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct domain_lock {
#define struct_domain_lock_t_type_p struct_domain_lock_t_type_p
	struct domain_lock *src = MM_MALLOC(1, struct domain_lock);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_domain_lock_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct domain_lock * {
#define struct_domain_lock_t_type_p struct_domain_lock_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_domain_lock_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct domain_lock * {
#define struct_domain_lock_t_type_p struct_domain_lock_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_domain_lock_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct notifier_handle {
#define struct_notifier_handle_t_type_p struct_notifier_handle_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_notifier_handle_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a notifier_handle_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct notifier_handle *, $source);
};

/* generated */
%typemap (python, struct-out) struct notifier_handle {
#define struct_notifier_handle_t_type_p struct_notifier_handle_t_type_p
$target = new_opaque_object(&$source, false, false, struct_notifier_handle_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct notifier_handle {
#define struct_notifier_handle_t_type_p struct_notifier_handle_t_type_p
	struct notifier_handle *src = MM_MALLOC(1, struct notifier_handle);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_notifier_handle_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct notifier_handle * {
#define struct_notifier_handle_t_type_p struct_notifier_handle_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_notifier_handle_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct notifier_handle * {
#define struct_notifier_handle_t_type_p struct_notifier_handle_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_notifier_handle_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct __anonymous25 {
#define struct_transaction_target_type_ret_t_type_p struct_transaction_target_type_ret_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_transaction_target_type_ret_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a transaction_target_type_ret_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(transaction_target_type_ret_t *, $source);
} default_value { (const transaction_target_type_ret_t){ .val = Sim_Transaction_Target_Unset } };

/* generated */
%typemap (python, struct-out) struct __anonymous25 {
#define struct_transaction_target_type_ret_t_type_p struct_transaction_target_type_ret_t_type_p
$target = new_opaque_object(&$source, false, false, struct_transaction_target_type_ret_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous25 {
#define struct_transaction_target_type_ret_t_type_p struct_transaction_target_type_ret_t_type_p
	transaction_target_type_ret_t *src = MM_MALLOC(1, transaction_target_type_ret_t);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_transaction_target_type_ret_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous25 * {
#define struct_transaction_target_type_ret_t_type_p struct_transaction_target_type_ret_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_transaction_target_type_ret_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct __anonymous25 * {
#define struct_transaction_target_type_ret_t_type_p struct_transaction_target_type_ret_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_transaction_target_type_ret_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct transaction_trace_atom_access {
#define struct_transaction_trace_atom_access_t_type_p struct_transaction_trace_atom_access_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_transaction_trace_atom_access_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a transaction_trace_atom_access_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct transaction_trace_atom_access *, $source);
} default_value { (const transaction_trace_atom_access_t){ .callback = 0 } };

/* generated */
%typemap (python, struct-out) struct transaction_trace_atom_access {
#define struct_transaction_trace_atom_access_t_type_p struct_transaction_trace_atom_access_t_type_p
$target = new_opaque_object(&$source, false, false, struct_transaction_trace_atom_access_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct transaction_trace_atom_access {
#define struct_transaction_trace_atom_access_t_type_p struct_transaction_trace_atom_access_t_type_p
	struct transaction_trace_atom_access *src = MM_MALLOC(1, struct transaction_trace_atom_access);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_transaction_trace_atom_access_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct transaction_trace_atom_access * {
#define struct_transaction_trace_atom_access_t_type_p struct_transaction_trace_atom_access_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_transaction_trace_atom_access_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct transaction_trace_atom_access * {
#define struct_transaction_trace_atom_access_t_type_p struct_transaction_trace_atom_access_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_transaction_trace_atom_access_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct __anonymous27 {
#define struct_atom_t_type_p struct_atom_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_atom_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an atom_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(atom_t *, $source);
} default_value { (const atom_t){ .id = Sim_Atom_Id_illegal } };

/* generated */
%typemap (python, struct-out) struct __anonymous27 {
#define struct_atom_t_type_p struct_atom_t_type_p
$target = new_opaque_object(&$source, false, false, struct_atom_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous27 {
#define struct_atom_t_type_p struct_atom_t_type_p
	atom_t *src = MM_MALLOC(1, atom_t);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_atom_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous27 * {
#define struct_atom_t_type_p struct_atom_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_atom_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct __anonymous27 * {
#define struct_atom_t_type_p struct_atom_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_atom_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct global_notifier_callback {
#define struct_global_notifier_callback_t_type_p struct_global_notifier_callback_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_global_notifier_callback_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a global_notifier_callback_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct global_notifier_callback *, $source);
};

/* generated */
%typemap (python, struct-out) struct global_notifier_callback {
#define struct_global_notifier_callback_t_type_p struct_global_notifier_callback_t_type_p
$target = new_opaque_object(&$source, false, false, struct_global_notifier_callback_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct global_notifier_callback {
#define struct_global_notifier_callback_t_type_p struct_global_notifier_callback_t_type_p
	struct global_notifier_callback *src = MM_MALLOC(1, struct global_notifier_callback);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_global_notifier_callback_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct global_notifier_callback * {
#define struct_global_notifier_callback_t_type_p struct_global_notifier_callback_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_global_notifier_callback_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct global_notifier_callback * {
#define struct_global_notifier_callback_t_type_p struct_global_notifier_callback_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_global_notifier_callback_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct class_info {
#define struct_class_info_t_type_p struct_class_info_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_class_info_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a class_info_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct class_info *NOTNULL, $source);
} default_value { (const class_info_t){ .alloc = 0 } };

/* generated */
%typemap (python, struct-out) struct class_info {
#define struct_class_info_t_type_p struct_class_info_t_type_p
$target = new_opaque_object(&$source, false, false, struct_class_info_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct class_info {
#define struct_class_info_t_type_p struct_class_info_t_type_p
	struct class_info *NOTNULL src = MM_MALLOC(1, struct class_info);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_class_info_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct class_info *NOTNULL {
#define struct_class_info_t_type_p struct_class_info_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_class_info_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct class_info *NOTNULL {
#define struct_class_info_t_type_p struct_class_info_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_class_info_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct class_data {
#define struct_class_data_t_type_p struct_class_data_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_class_data_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a class_data_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct class_data *NOTNULL, $source);
} default_value { (const class_data_t){ .alloc_object = 0 } };

/* generated */
%typemap (python, struct-out) struct class_data {
#define struct_class_data_t_type_p struct_class_data_t_type_p
$target = new_opaque_object(&$source, false, false, struct_class_data_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct class_data {
#define struct_class_data_t_type_p struct_class_data_t_type_p
	struct class_data *NOTNULL src = MM_MALLOC(1, struct class_data);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_class_data_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct class_data *NOTNULL {
#define struct_class_data_t_type_p struct_class_data_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_class_data_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct class_data *NOTNULL {
#define struct_class_data_t_type_p struct_class_data_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_class_data_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct completion {
#define struct_completion_t_type_p struct_completion_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_completion_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a completion_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct completion *, $source);
};

/* generated */
%typemap (python, struct-out) struct completion {
#define struct_completion_t_type_p struct_completion_t_type_p
$target = new_opaque_object(&$source, false, false, struct_completion_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct completion {
#define struct_completion_t_type_p struct_completion_t_type_p
	struct completion *src = MM_MALLOC(1, struct completion);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_completion_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct completion * {
#define struct_completion_t_type_p struct_completion_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_completion_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct completion * {
#define struct_completion_t_type_p struct_completion_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_completion_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct xtensa_wwdt_faultinfo_interface {
#define struct_xtensa_wwdt_faultinfo_interface_t_type_p struct_xtensa_wwdt_faultinfo_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_xtensa_wwdt_faultinfo_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a xtensa_wwdt_faultinfo_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct xtensa_wwdt_faultinfo_interface *, $source);
} default_value { (const xtensa_wwdt_faultinfo_interface_t){ .status = 0 } };

/* generated */
%typemap (python, struct-out) struct xtensa_wwdt_faultinfo_interface {
#define struct_xtensa_wwdt_faultinfo_interface_t_type_p struct_xtensa_wwdt_faultinfo_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_xtensa_wwdt_faultinfo_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct xtensa_wwdt_faultinfo_interface {
#define struct_xtensa_wwdt_faultinfo_interface_t_type_p struct_xtensa_wwdt_faultinfo_interface_t_type_p
	struct xtensa_wwdt_faultinfo_interface *src = MM_MALLOC(1, struct xtensa_wwdt_faultinfo_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_xtensa_wwdt_faultinfo_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct xtensa_wwdt_faultinfo_interface * {
#define struct_xtensa_wwdt_faultinfo_interface_t_type_p struct_xtensa_wwdt_faultinfo_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_xtensa_wwdt_faultinfo_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct xtensa_wwdt_faultinfo_interface * {
#define struct_xtensa_wwdt_faultinfo_interface_t_type_p struct_xtensa_wwdt_faultinfo_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_xtensa_wwdt_faultinfo_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct xtensa_wwdt_config_interface {
#define struct_xtensa_wwdt_config_interface_t_type_p struct_xtensa_wwdt_config_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_xtensa_wwdt_config_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a xtensa_wwdt_config_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct xtensa_wwdt_config_interface *, $source);
} default_value { (const xtensa_wwdt_config_interface_t){ .has_wwdt = 0 } };

/* generated */
%typemap (python, struct-out) struct xtensa_wwdt_config_interface {
#define struct_xtensa_wwdt_config_interface_t_type_p struct_xtensa_wwdt_config_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_xtensa_wwdt_config_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct xtensa_wwdt_config_interface {
#define struct_xtensa_wwdt_config_interface_t_type_p struct_xtensa_wwdt_config_interface_t_type_p
	struct xtensa_wwdt_config_interface *src = MM_MALLOC(1, struct xtensa_wwdt_config_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_xtensa_wwdt_config_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct xtensa_wwdt_config_interface * {
#define struct_xtensa_wwdt_config_interface_t_type_p struct_xtensa_wwdt_config_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_xtensa_wwdt_config_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct xtensa_wwdt_config_interface * {
#define struct_xtensa_wwdt_config_interface_t_type_p struct_xtensa_wwdt_config_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_xtensa_wwdt_config_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct xtensa_output_queue_interface {
#define struct_xtensa_output_queue_interface_t_type_p struct_xtensa_output_queue_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_xtensa_output_queue_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a xtensa_output_queue_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct xtensa_output_queue_interface *, $source);
} default_value { (const xtensa_output_queue_interface_t){ .register_output_queue = 0 } };

/* generated */
%typemap (python, struct-out) struct xtensa_output_queue_interface {
#define struct_xtensa_output_queue_interface_t_type_p struct_xtensa_output_queue_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_xtensa_output_queue_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct xtensa_output_queue_interface {
#define struct_xtensa_output_queue_interface_t_type_p struct_xtensa_output_queue_interface_t_type_p
	struct xtensa_output_queue_interface *src = MM_MALLOC(1, struct xtensa_output_queue_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_xtensa_output_queue_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct xtensa_output_queue_interface * {
#define struct_xtensa_output_queue_interface_t_type_p struct_xtensa_output_queue_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_xtensa_output_queue_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct xtensa_output_queue_interface * {
#define struct_xtensa_output_queue_interface_t_type_p struct_xtensa_output_queue_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_xtensa_output_queue_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct xtensa_mpu_lookup {
#define struct_xtensa_mpu_lookup_t_type_p struct_xtensa_mpu_lookup_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_xtensa_mpu_lookup_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a xtensa_mpu_lookup_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(xtensa_mpu_lookup_t *, $source);
} default_value { (const xtensa_mpu_lookup_t){ .background = 0 } };

/* generated */
%typemap (python, struct-out) struct xtensa_mpu_lookup {
#define struct_xtensa_mpu_lookup_t_type_p struct_xtensa_mpu_lookup_t_type_p
$target = new_opaque_object(&$source, false, false, struct_xtensa_mpu_lookup_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct xtensa_mpu_lookup {
#define struct_xtensa_mpu_lookup_t_type_p struct_xtensa_mpu_lookup_t_type_p
	xtensa_mpu_lookup_t *src = MM_MALLOC(1, struct xtensa_mpu_lookup);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_xtensa_mpu_lookup_t_type_p, NULL);
};

/* generated */
%typemap (python, out) xtensa_mpu_lookup_t * {
#define struct_xtensa_mpu_lookup_t_type_p struct_xtensa_mpu_lookup_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_xtensa_mpu_lookup_t_type_p, NULL);
};

/* generated */
%typemap (python, in) xtensa_mpu_lookup_t * {
#define struct_xtensa_mpu_lookup_t_type_p struct_xtensa_mpu_lookup_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_xtensa_mpu_lookup_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct xtensa_mpu_lookup_interface {
#define struct_xtensa_mpu_lookup_interface_t_type_p struct_xtensa_mpu_lookup_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_xtensa_mpu_lookup_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a xtensa_mpu_lookup_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct xtensa_mpu_lookup_interface *, $source);
} default_value { (const xtensa_mpu_lookup_interface_t){ .mpu_region = 0 } };

/* generated */
%typemap (python, struct-out) struct xtensa_mpu_lookup_interface {
#define struct_xtensa_mpu_lookup_interface_t_type_p struct_xtensa_mpu_lookup_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_xtensa_mpu_lookup_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct xtensa_mpu_lookup_interface {
#define struct_xtensa_mpu_lookup_interface_t_type_p struct_xtensa_mpu_lookup_interface_t_type_p
	struct xtensa_mpu_lookup_interface *src = MM_MALLOC(1, struct xtensa_mpu_lookup_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_xtensa_mpu_lookup_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct xtensa_mpu_lookup_interface * {
#define struct_xtensa_mpu_lookup_interface_t_type_p struct_xtensa_mpu_lookup_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_xtensa_mpu_lookup_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct xtensa_mpu_lookup_interface * {
#define struct_xtensa_mpu_lookup_interface_t_type_p struct_xtensa_mpu_lookup_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_xtensa_mpu_lookup_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct xtensa_lookup_interface {
#define struct_xtensa_lookup_interface_t_type_p struct_xtensa_lookup_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_xtensa_lookup_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a xtensa_lookup_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct xtensa_lookup_interface *, $source);
} default_value { (const xtensa_lookup_interface_t){ .register_lookup = 0 } };

/* generated */
%typemap (python, struct-out) struct xtensa_lookup_interface {
#define struct_xtensa_lookup_interface_t_type_p struct_xtensa_lookup_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_xtensa_lookup_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct xtensa_lookup_interface {
#define struct_xtensa_lookup_interface_t_type_p struct_xtensa_lookup_interface_t_type_p
	struct xtensa_lookup_interface *src = MM_MALLOC(1, struct xtensa_lookup_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_xtensa_lookup_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct xtensa_lookup_interface * {
#define struct_xtensa_lookup_interface_t_type_p struct_xtensa_lookup_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_xtensa_lookup_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct xtensa_lookup_interface * {
#define struct_xtensa_lookup_interface_t_type_p struct_xtensa_lookup_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_xtensa_lookup_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct xtensa_internal_memories_interface {
#define struct_xtensa_internal_memories_interface_t_type_p struct_xtensa_internal_memories_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_xtensa_internal_memories_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a xtensa_internal_memories_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct xtensa_internal_memories_interface *, $source);
} default_value { (const xtensa_internal_memories_interface_t){ .iram_mappings = 0 } };

/* generated */
%typemap (python, struct-out) struct xtensa_internal_memories_interface {
#define struct_xtensa_internal_memories_interface_t_type_p struct_xtensa_internal_memories_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_xtensa_internal_memories_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct xtensa_internal_memories_interface {
#define struct_xtensa_internal_memories_interface_t_type_p struct_xtensa_internal_memories_interface_t_type_p
	struct xtensa_internal_memories_interface *src = MM_MALLOC(1, struct xtensa_internal_memories_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_xtensa_internal_memories_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct xtensa_internal_memories_interface * {
#define struct_xtensa_internal_memories_interface_t_type_p struct_xtensa_internal_memories_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_xtensa_internal_memories_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct xtensa_internal_memories_interface * {
#define struct_xtensa_internal_memories_interface_t_type_p struct_xtensa_internal_memories_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_xtensa_internal_memories_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct xtensa_input_queue_interface {
#define struct_xtensa_input_queue_interface_t_type_p struct_xtensa_input_queue_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_xtensa_input_queue_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a xtensa_input_queue_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct xtensa_input_queue_interface *, $source);
} default_value { (const xtensa_input_queue_interface_t){ .register_input_queue = 0 } };

/* generated */
%typemap (python, struct-out) struct xtensa_input_queue_interface {
#define struct_xtensa_input_queue_interface_t_type_p struct_xtensa_input_queue_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_xtensa_input_queue_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct xtensa_input_queue_interface {
#define struct_xtensa_input_queue_interface_t_type_p struct_xtensa_input_queue_interface_t_type_p
	struct xtensa_input_queue_interface *src = MM_MALLOC(1, struct xtensa_input_queue_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_xtensa_input_queue_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct xtensa_input_queue_interface * {
#define struct_xtensa_input_queue_interface_t_type_p struct_xtensa_input_queue_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_xtensa_input_queue_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct xtensa_input_queue_interface * {
#define struct_xtensa_input_queue_interface_t_type_p struct_xtensa_input_queue_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_xtensa_input_queue_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct xtensa_import_wire_interface {
#define struct_xtensa_import_wire_interface_t_type_p struct_xtensa_import_wire_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_xtensa_import_wire_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a xtensa_import_wire_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct xtensa_import_wire_interface *, $source);
} default_value { (const xtensa_import_wire_interface_t){ .register_import_wire = 0 } };

/* generated */
%typemap (python, struct-out) struct xtensa_import_wire_interface {
#define struct_xtensa_import_wire_interface_t_type_p struct_xtensa_import_wire_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_xtensa_import_wire_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct xtensa_import_wire_interface {
#define struct_xtensa_import_wire_interface_t_type_p struct_xtensa_import_wire_interface_t_type_p
	struct xtensa_import_wire_interface *src = MM_MALLOC(1, struct xtensa_import_wire_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_xtensa_import_wire_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct xtensa_import_wire_interface * {
#define struct_xtensa_import_wire_interface_t_type_p struct_xtensa_import_wire_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_xtensa_import_wire_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct xtensa_import_wire_interface * {
#define struct_xtensa_import_wire_interface_t_type_p struct_xtensa_import_wire_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_xtensa_import_wire_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct xtensa_export_state_interface {
#define struct_xtensa_export_state_interface_t_type_p struct_xtensa_export_state_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_xtensa_export_state_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a xtensa_export_state_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct xtensa_export_state_interface *, $source);
} default_value { (const xtensa_export_state_interface_t){ .register_export_state = 0 } };

/* generated */
%typemap (python, struct-out) struct xtensa_export_state_interface {
#define struct_xtensa_export_state_interface_t_type_p struct_xtensa_export_state_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_xtensa_export_state_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct xtensa_export_state_interface {
#define struct_xtensa_export_state_interface_t_type_p struct_xtensa_export_state_interface_t_type_p
	struct xtensa_export_state_interface *src = MM_MALLOC(1, struct xtensa_export_state_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_xtensa_export_state_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct xtensa_export_state_interface * {
#define struct_xtensa_export_state_interface_t_type_p struct_xtensa_export_state_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_xtensa_export_state_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct xtensa_export_state_interface * {
#define struct_xtensa_export_state_interface_t_type_p struct_xtensa_export_state_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_xtensa_export_state_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct xed_access_interface {
#define struct_xed_access_interface_t_type_p struct_xed_access_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_xed_access_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a xed_access_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct xed_access_interface *, $source);
} default_value { (const xed_access_interface_t){ .get_last = 0 } };

/* generated */
%typemap (python, struct-out) struct xed_access_interface {
#define struct_xed_access_interface_t_type_p struct_xed_access_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_xed_access_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct xed_access_interface {
#define struct_xed_access_interface_t_type_p struct_xed_access_interface_t_type_p
	struct xed_access_interface *src = MM_MALLOC(1, struct xed_access_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_xed_access_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct xed_access_interface * {
#define struct_xed_access_interface_t_type_p struct_xed_access_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_xed_access_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct xed_access_interface * {
#define struct_xed_access_interface_t_type_p struct_xed_access_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_xed_access_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct x86_vmp_control_interface {
#define struct_x86_vmp_control_interface_t_type_p struct_x86_vmp_control_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_x86_vmp_control_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a x86_vmp_control_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct x86_vmp_control_interface *, $source);
} default_value { (const x86_vmp_control_interface_t){ .get_block_count = 0 } };

/* generated */
%typemap (python, struct-out) struct x86_vmp_control_interface {
#define struct_x86_vmp_control_interface_t_type_p struct_x86_vmp_control_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_x86_vmp_control_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct x86_vmp_control_interface {
#define struct_x86_vmp_control_interface_t_type_p struct_x86_vmp_control_interface_t_type_p
	struct x86_vmp_control_interface *src = MM_MALLOC(1, struct x86_vmp_control_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_x86_vmp_control_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct x86_vmp_control_interface * {
#define struct_x86_vmp_control_interface_t_type_p struct_x86_vmp_control_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_x86_vmp_control_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct x86_vmp_control_interface * {
#define struct_x86_vmp_control_interface_t_type_p struct_x86_vmp_control_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_x86_vmp_control_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct __anonymous42 {
#define struct_x86_tlb_entry_v3_t_type_p struct_x86_tlb_entry_v3_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_x86_tlb_entry_v3_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a x86_tlb_entry_v3_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(x86_tlb_entry_v3_t *, $source);
} default_value { (const x86_tlb_entry_v3_t){ .linear_page_start = 0 } };

/* generated */
%typemap (python, struct-out) struct __anonymous42 {
#define struct_x86_tlb_entry_v3_t_type_p struct_x86_tlb_entry_v3_t_type_p
$target = new_opaque_object(&$source, false, false, struct_x86_tlb_entry_v3_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous42 {
#define struct_x86_tlb_entry_v3_t_type_p struct_x86_tlb_entry_v3_t_type_p
	x86_tlb_entry_v3_t *src = MM_MALLOC(1, x86_tlb_entry_v3_t);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_x86_tlb_entry_v3_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous42 * {
#define struct_x86_tlb_entry_v3_t_type_p struct_x86_tlb_entry_v3_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_x86_tlb_entry_v3_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct __anonymous42 * {
#define struct_x86_tlb_entry_v3_t_type_p struct_x86_tlb_entry_v3_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_x86_tlb_entry_v3_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct __anonymous43 {
#define struct_x86_tlb_attrs_v3_t_type_p struct_x86_tlb_attrs_v3_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_x86_tlb_attrs_v3_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a x86_tlb_attrs_v3_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(x86_tlb_attrs_v3_t *, $source);
} default_value { (const x86_tlb_attrs_v3_t){ .pte_attrs = 0 } };

/* generated */
%typemap (python, struct-out) struct __anonymous43 {
#define struct_x86_tlb_attrs_v3_t_type_p struct_x86_tlb_attrs_v3_t_type_p
$target = new_opaque_object(&$source, false, false, struct_x86_tlb_attrs_v3_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous43 {
#define struct_x86_tlb_attrs_v3_t_type_p struct_x86_tlb_attrs_v3_t_type_p
	x86_tlb_attrs_v3_t *src = MM_MALLOC(1, x86_tlb_attrs_v3_t);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_x86_tlb_attrs_v3_t_type_p, NULL);
};

/* generated */
%typemap (python, out) x86_tlb_attrs_v3_t * {
#define struct_x86_tlb_attrs_v3_t_type_p struct_x86_tlb_attrs_v3_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_x86_tlb_attrs_v3_t_type_p, NULL);
};

/* generated */
%typemap (python, in) x86_tlb_attrs_v3_t * {
#define struct_x86_tlb_attrs_v3_t_type_p struct_x86_tlb_attrs_v3_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_x86_tlb_attrs_v3_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct x86_tlb_v3_interface {
#define struct_x86_tlb_v3_interface_t_type_p struct_x86_tlb_v3_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_x86_tlb_v3_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a x86_tlb_v3_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct x86_tlb_v3_interface *, $source);
} default_value { (const x86_tlb_v3_interface_t){ .add = 0 } };

/* generated */
%typemap (python, struct-out) struct x86_tlb_v3_interface {
#define struct_x86_tlb_v3_interface_t_type_p struct_x86_tlb_v3_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_x86_tlb_v3_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct x86_tlb_v3_interface {
#define struct_x86_tlb_v3_interface_t_type_p struct_x86_tlb_v3_interface_t_type_p
	struct x86_tlb_v3_interface *src = MM_MALLOC(1, struct x86_tlb_v3_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_x86_tlb_v3_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct x86_tlb_v3_interface * {
#define struct_x86_tlb_v3_interface_t_type_p struct_x86_tlb_v3_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_x86_tlb_v3_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct x86_tlb_v3_interface * {
#define struct_x86_tlb_v3_interface_t_type_p struct_x86_tlb_v3_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_x86_tlb_v3_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct __anonymous44 {
#define struct_x86_tlb_entry_t_type_p struct_x86_tlb_entry_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_x86_tlb_entry_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a x86_tlb_entry_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(x86_tlb_entry_t *, $source);
} default_value { (const x86_tlb_entry_t){ .linear_page_start = 0 } };

/* generated */
%typemap (python, struct-out) struct __anonymous44 {
#define struct_x86_tlb_entry_t_type_p struct_x86_tlb_entry_t_type_p
$target = new_opaque_object(&$source, false, false, struct_x86_tlb_entry_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous44 {
#define struct_x86_tlb_entry_t_type_p struct_x86_tlb_entry_t_type_p
	x86_tlb_entry_t *src = MM_MALLOC(1, x86_tlb_entry_t);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_x86_tlb_entry_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous44 * {
#define struct_x86_tlb_entry_t_type_p struct_x86_tlb_entry_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_x86_tlb_entry_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct __anonymous44 * {
#define struct_x86_tlb_entry_t_type_p struct_x86_tlb_entry_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_x86_tlb_entry_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct __anonymous45 {
#define struct_x86_tlb_attrs_t_type_p struct_x86_tlb_attrs_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_x86_tlb_attrs_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a x86_tlb_attrs_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(x86_tlb_attrs_t *, $source);
} default_value { (const x86_tlb_attrs_t){ .supervisor_access = 0 } };

/* generated */
%typemap (python, struct-out) struct __anonymous45 {
#define struct_x86_tlb_attrs_t_type_p struct_x86_tlb_attrs_t_type_p
$target = new_opaque_object(&$source, false, false, struct_x86_tlb_attrs_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous45 {
#define struct_x86_tlb_attrs_t_type_p struct_x86_tlb_attrs_t_type_p
	x86_tlb_attrs_t *src = MM_MALLOC(1, x86_tlb_attrs_t);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_x86_tlb_attrs_t_type_p, NULL);
};

/* generated */
%typemap (python, out) x86_tlb_attrs_t * {
#define struct_x86_tlb_attrs_t_type_p struct_x86_tlb_attrs_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_x86_tlb_attrs_t_type_p, NULL);
};

/* generated */
%typemap (python, in) x86_tlb_attrs_t * {
#define struct_x86_tlb_attrs_t_type_p struct_x86_tlb_attrs_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_x86_tlb_attrs_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct x86_tlb_v2_interface {
#define struct_x86_tlb_v2_interface_t_type_p struct_x86_tlb_v2_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_x86_tlb_v2_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a x86_tlb_v2_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct x86_tlb_v2_interface *, $source);
} default_value { (const x86_tlb_v2_interface_t){ .flush_all = 0 } };

/* generated */
%typemap (python, struct-out) struct x86_tlb_v2_interface {
#define struct_x86_tlb_v2_interface_t_type_p struct_x86_tlb_v2_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_x86_tlb_v2_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct x86_tlb_v2_interface {
#define struct_x86_tlb_v2_interface_t_type_p struct_x86_tlb_v2_interface_t_type_p
	struct x86_tlb_v2_interface *src = MM_MALLOC(1, struct x86_tlb_v2_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_x86_tlb_v2_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct x86_tlb_v2_interface * {
#define struct_x86_tlb_v2_interface_t_type_p struct_x86_tlb_v2_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_x86_tlb_v2_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct x86_tlb_v2_interface * {
#define struct_x86_tlb_v2_interface_t_type_p struct_x86_tlb_v2_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_x86_tlb_v2_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct __anonymous46 {
#define struct_tagged_physical_address_t_type_p struct_tagged_physical_address_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_tagged_physical_address_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a tagged_physical_address_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(tagged_physical_address_t *, $source);
} default_value { (const tagged_physical_address_t){ .valid = 0 } };

/* generated */
%typemap (python, struct-out) struct __anonymous46 {
#define struct_tagged_physical_address_t_type_p struct_tagged_physical_address_t_type_p
$target = new_opaque_object(&$source, false, false, struct_tagged_physical_address_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous46 {
#define struct_tagged_physical_address_t_type_p struct_tagged_physical_address_t_type_p
	tagged_physical_address_t *src = MM_MALLOC(1, tagged_physical_address_t);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_tagged_physical_address_t_type_p, NULL);
};

/* generated */
%typemap (python, out) tagged_physical_address_t * {
#define struct_tagged_physical_address_t_type_p struct_tagged_physical_address_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_tagged_physical_address_t_type_p, NULL);
};

/* generated */
%typemap (python, in) tagged_physical_address_t * {
#define struct_tagged_physical_address_t_type_p struct_tagged_physical_address_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_tagged_physical_address_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct x86_tlb_interface {
#define struct_x86_tlb_interface_t_type_p struct_x86_tlb_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_x86_tlb_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a x86_tlb_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct x86_tlb_interface *, $source);
} default_value { (const x86_tlb_interface_t){ .flush_all = 0 } };

/* generated */
%typemap (python, struct-out) struct x86_tlb_interface {
#define struct_x86_tlb_interface_t_type_p struct_x86_tlb_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_x86_tlb_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct x86_tlb_interface {
#define struct_x86_tlb_interface_t_type_p struct_x86_tlb_interface_t_type_p
	struct x86_tlb_interface *src = MM_MALLOC(1, struct x86_tlb_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_x86_tlb_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct x86_tlb_interface * {
#define struct_x86_tlb_interface_t_type_p struct_x86_tlb_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_x86_tlb_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct x86_tlb_interface * {
#define struct_x86_tlb_interface_t_type_p struct_x86_tlb_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_x86_tlb_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct __anonymous47 {
#define struct_smm_reg_state_t_type_p struct_smm_reg_state_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_smm_reg_state_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a smm_reg_state_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(smm_reg_state_t *, $source);
} default_value { (const smm_reg_state_t){ .gpr = { 0 } } };

/* generated */
%typemap (python, struct-out) struct __anonymous47 {
#define struct_smm_reg_state_t_type_p struct_smm_reg_state_t_type_p
$target = new_opaque_object(&$source, false, false, struct_smm_reg_state_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous47 {
#define struct_smm_reg_state_t_type_p struct_smm_reg_state_t_type_p
	smm_reg_state_t *src = MM_MALLOC(1, smm_reg_state_t);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_smm_reg_state_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous47 * {
#define struct_smm_reg_state_t_type_p struct_smm_reg_state_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_smm_reg_state_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct __anonymous47 * {
#define struct_smm_reg_state_t_type_p struct_smm_reg_state_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_smm_reg_state_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct x86_smm_state_interface {
#define struct_x86_smm_state_interface_t_type_p struct_x86_smm_state_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_x86_smm_state_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a x86_smm_state_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct x86_smm_state_interface *, $source);
} default_value { (const x86_smm_state_interface_t){ .get_state = 0 } };

/* generated */
%typemap (python, struct-out) struct x86_smm_state_interface {
#define struct_x86_smm_state_interface_t_type_p struct_x86_smm_state_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_x86_smm_state_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct x86_smm_state_interface {
#define struct_x86_smm_state_interface_t_type_p struct_x86_smm_state_interface_t_type_p
	struct x86_smm_state_interface *src = MM_MALLOC(1, struct x86_smm_state_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_x86_smm_state_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct x86_smm_state_interface * {
#define struct_x86_smm_state_interface_t_type_p struct_x86_smm_state_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_x86_smm_state_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct x86_smm_state_interface * {
#define struct_x86_smm_state_interface_t_type_p struct_x86_smm_state_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_x86_smm_state_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct x86_smm_interface {
#define struct_x86_smm_interface_t_type_p struct_x86_smm_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_x86_smm_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a x86_smm_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct x86_smm_interface *, $source);
} default_value { (const x86_smm_interface_t){ .save_state = 0 } };

/* generated */
%typemap (python, struct-out) struct x86_smm_interface {
#define struct_x86_smm_interface_t_type_p struct_x86_smm_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_x86_smm_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct x86_smm_interface {
#define struct_x86_smm_interface_t_type_p struct_x86_smm_interface_t_type_p
	struct x86_smm_interface *src = MM_MALLOC(1, struct x86_smm_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_x86_smm_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct x86_smm_interface * {
#define struct_x86_smm_interface_t_type_p struct_x86_smm_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_x86_smm_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct x86_smm_interface * {
#define struct_x86_smm_interface_t_type_p struct_x86_smm_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_x86_smm_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct __anonymous48 {
#define struct_ymm_reg_t_type_p struct_ymm_reg_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_ymm_reg_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an ymm_reg_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(ymm_reg_t *, $source);
} default_value { (const ymm_reg_t){ .llo64 = 0 } };

/* generated */
%typemap (python, struct-out) struct __anonymous48 {
#define struct_ymm_reg_t_type_p struct_ymm_reg_t_type_p
$target = new_opaque_object(&$source, false, false, struct_ymm_reg_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous48 {
#define struct_ymm_reg_t_type_p struct_ymm_reg_t_type_p
	ymm_reg_t *src = MM_MALLOC(1, ymm_reg_t);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_ymm_reg_t_type_p, NULL);
};

/* generated */
%typemap (python, out) ymm_reg_t * {
#define struct_ymm_reg_t_type_p struct_ymm_reg_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_ymm_reg_t_type_p, NULL);
};

/* generated */
%typemap (python, in) ymm_reg_t * {
#define struct_ymm_reg_t_type_p struct_ymm_reg_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_ymm_reg_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct __anonymous49 {
#define struct_x86_xmode_info_t_type_p struct_x86_xmode_info_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_x86_xmode_info_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a x86_xmode_info_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(x86_xmode_info_t *, $source);
} default_value { (const x86_xmode_info_t){ .efer_lma = 0 } };

/* generated */
%typemap (python, struct-out) struct __anonymous49 {
#define struct_x86_xmode_info_t_type_p struct_x86_xmode_info_t_type_p
$target = new_opaque_object(&$source, false, false, struct_x86_xmode_info_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous49 {
#define struct_x86_xmode_info_t_type_p struct_x86_xmode_info_t_type_p
	x86_xmode_info_t *src = MM_MALLOC(1, x86_xmode_info_t);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_x86_xmode_info_t_type_p, NULL);
};

/* generated */
%typemap (python, out) x86_xmode_info_t * {
#define struct_x86_xmode_info_t_type_p struct_x86_xmode_info_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_x86_xmode_info_t_type_p, NULL);
};

/* generated */
%typemap (python, in) x86_xmode_info_t * {
#define struct_x86_xmode_info_t_type_p struct_x86_xmode_info_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_x86_xmode_info_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct __anonymous50 {
#define struct_xmm_reg_t_type_p struct_xmm_reg_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_xmm_reg_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a xmm_reg_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(xmm_reg_t *, $source);
} default_value { (const xmm_reg_t){ .lo64 = 0 } };

/* generated */
%typemap (python, struct-out) struct __anonymous50 {
#define struct_xmm_reg_t_type_p struct_xmm_reg_t_type_p
$target = new_opaque_object(&$source, false, false, struct_xmm_reg_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous50 {
#define struct_xmm_reg_t_type_p struct_xmm_reg_t_type_p
	xmm_reg_t *src = MM_MALLOC(1, xmm_reg_t);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_xmm_reg_t_type_p, NULL);
};

/* generated */
%typemap (python, out) xmm_reg_t * {
#define struct_xmm_reg_t_type_p struct_xmm_reg_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_xmm_reg_t_type_p, NULL);
};

/* generated */
%typemap (python, in) xmm_reg_t * {
#define struct_xmm_reg_t_type_p struct_xmm_reg_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_xmm_reg_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct __anonymous52 {
#define struct_x86_system_seg_reg_t_type_p struct_x86_system_seg_reg_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_x86_system_seg_reg_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a x86_system_seg_reg_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(x86_system_seg_reg_t *, $source);
} default_value { (const x86_system_seg_reg_t){ .base = 0 } };

/* generated */
%typemap (python, struct-out) struct __anonymous52 {
#define struct_x86_system_seg_reg_t_type_p struct_x86_system_seg_reg_t_type_p
$target = new_opaque_object(&$source, false, false, struct_x86_system_seg_reg_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous52 {
#define struct_x86_system_seg_reg_t_type_p struct_x86_system_seg_reg_t_type_p
	x86_system_seg_reg_t *src = MM_MALLOC(1, x86_system_seg_reg_t);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_x86_system_seg_reg_t_type_p, NULL);
};

/* generated */
%typemap (python, out) x86_system_seg_reg_t * {
#define struct_x86_system_seg_reg_t_type_p struct_x86_system_seg_reg_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_x86_system_seg_reg_t_type_p, NULL);
};

/* generated */
%typemap (python, in) x86_system_seg_reg_t * {
#define struct_x86_system_seg_reg_t_type_p struct_x86_system_seg_reg_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_x86_system_seg_reg_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct __anonymous54 {
#define struct_x86_seg_reg_t_type_p struct_x86_seg_reg_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_x86_seg_reg_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a x86_seg_reg_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(x86_seg_reg_t *, $source);
} default_value { (const x86_seg_reg_t){ .valid = 0 } };

/* generated */
%typemap (python, struct-out) struct __anonymous54 {
#define struct_x86_seg_reg_t_type_p struct_x86_seg_reg_t_type_p
$target = new_opaque_object(&$source, false, false, struct_x86_seg_reg_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous54 {
#define struct_x86_seg_reg_t_type_p struct_x86_seg_reg_t_type_p
	x86_seg_reg_t *src = MM_MALLOC(1, x86_seg_reg_t);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_x86_seg_reg_t_type_p, NULL);
};

/* generated */
%typemap (python, out) x86_seg_reg_t * {
#define struct_x86_seg_reg_t_type_p struct_x86_seg_reg_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_x86_seg_reg_t_type_p, NULL);
};

/* generated */
%typemap (python, in) x86_seg_reg_t * {
#define struct_x86_seg_reg_t_type_p struct_x86_seg_reg_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_x86_seg_reg_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct __anonymous55 {
#define struct_x86_pending_debug_exc_t_type_p struct_x86_pending_debug_exc_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_x86_pending_debug_exc_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a x86_pending_debug_exc_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(x86_pending_debug_exc_t *, $source);
} default_value { (const x86_pending_debug_exc_t){ .pending = 0 } };

/* generated */
%typemap (python, struct-out) struct __anonymous55 {
#define struct_x86_pending_debug_exc_t_type_p struct_x86_pending_debug_exc_t_type_p
$target = new_opaque_object(&$source, false, false, struct_x86_pending_debug_exc_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous55 {
#define struct_x86_pending_debug_exc_t_type_p struct_x86_pending_debug_exc_t_type_p
	x86_pending_debug_exc_t *src = MM_MALLOC(1, x86_pending_debug_exc_t);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_x86_pending_debug_exc_t_type_p, NULL);
};

/* generated */
%typemap (python, out) x86_pending_debug_exc_t * {
#define struct_x86_pending_debug_exc_t_type_p struct_x86_pending_debug_exc_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_x86_pending_debug_exc_t_type_p, NULL);
};

/* generated */
%typemap (python, in) x86_pending_debug_exc_t * {
#define struct_x86_pending_debug_exc_t_type_p struct_x86_pending_debug_exc_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_x86_pending_debug_exc_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct __anonymous56 {
#define struct_x86_mwait_info_t_type_p struct_x86_mwait_info_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_x86_mwait_info_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a x86_mwait_info_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(x86_mwait_info_t *, $source);
} default_value { (const x86_mwait_info_t){ .extensions = 0 } };

/* generated */
%typemap (python, struct-out) struct __anonymous56 {
#define struct_x86_mwait_info_t_type_p struct_x86_mwait_info_t_type_p
$target = new_opaque_object(&$source, false, false, struct_x86_mwait_info_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous56 {
#define struct_x86_mwait_info_t_type_p struct_x86_mwait_info_t_type_p
	x86_mwait_info_t *src = MM_MALLOC(1, x86_mwait_info_t);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_x86_mwait_info_t_type_p, NULL);
};

/* generated */
%typemap (python, out) x86_mwait_info_t * {
#define struct_x86_mwait_info_t_type_p struct_x86_mwait_info_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_x86_mwait_info_t_type_p, NULL);
};

/* generated */
%typemap (python, in) x86_mwait_info_t * {
#define struct_x86_mwait_info_t_type_p struct_x86_mwait_info_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_x86_mwait_info_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct __anonymous57 {
#define struct_x86_monitor_info_t_type_p struct_x86_monitor_info_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_x86_monitor_info_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a x86_monitor_info_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(x86_monitor_info_t *, $source);
} default_value { (const x86_monitor_info_t){ .armed = 0 } };

/* generated */
%typemap (python, struct-out) struct __anonymous57 {
#define struct_x86_monitor_info_t_type_p struct_x86_monitor_info_t_type_p
$target = new_opaque_object(&$source, false, false, struct_x86_monitor_info_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous57 {
#define struct_x86_monitor_info_t_type_p struct_x86_monitor_info_t_type_p
	x86_monitor_info_t *src = MM_MALLOC(1, x86_monitor_info_t);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_x86_monitor_info_t_type_p, NULL);
};

/* generated */
%typemap (python, out) x86_monitor_info_t * {
#define struct_x86_monitor_info_t_type_p struct_x86_monitor_info_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_x86_monitor_info_t_type_p, NULL);
};

/* generated */
%typemap (python, in) x86_monitor_info_t * {
#define struct_x86_monitor_info_t_type_p struct_x86_monitor_info_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_x86_monitor_info_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct __anonymous59 {
#define struct_x86_fpu_reg_t_type_p struct_x86_fpu_reg_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_x86_fpu_reg_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a x86_fpu_reg_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(x86_fpu_reg_t *, $source);
} default_value { (const x86_fpu_reg_t){ .low = 0 } };

/* generated */
%typemap (python, struct-out) struct __anonymous59 {
#define struct_x86_fpu_reg_t_type_p struct_x86_fpu_reg_t_type_p
$target = new_opaque_object(&$source, false, false, struct_x86_fpu_reg_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous59 {
#define struct_x86_fpu_reg_t_type_p struct_x86_fpu_reg_t_type_p
	x86_fpu_reg_t *src = MM_MALLOC(1, x86_fpu_reg_t);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_x86_fpu_reg_t_type_p, NULL);
};

/* generated */
%typemap (python, out) x86_fpu_reg_t * {
#define struct_x86_fpu_reg_t_type_p struct_x86_fpu_reg_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_x86_fpu_reg_t_type_p, NULL);
};

/* generated */
%typemap (python, in) x86_fpu_reg_t * {
#define struct_x86_fpu_reg_t_type_p struct_x86_fpu_reg_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_x86_fpu_reg_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct __anonymous60 {
#define struct_x86_fpu_env_t_type_p struct_x86_fpu_env_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_x86_fpu_env_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a x86_fpu_env_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(x86_fpu_env_t *, $source);
} default_value { (const x86_fpu_env_t){ .cw = 0 } };

/* generated */
%typemap (python, struct-out) struct __anonymous60 {
#define struct_x86_fpu_env_t_type_p struct_x86_fpu_env_t_type_p
$target = new_opaque_object(&$source, false, false, struct_x86_fpu_env_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous60 {
#define struct_x86_fpu_env_t_type_p struct_x86_fpu_env_t_type_p
	x86_fpu_env_t *src = MM_MALLOC(1, x86_fpu_env_t);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_x86_fpu_env_t_type_p, NULL);
};

/* generated */
%typemap (python, out) x86_fpu_env_t * {
#define struct_x86_fpu_env_t_type_p struct_x86_fpu_env_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_x86_fpu_env_t_type_p, NULL);
};

/* generated */
%typemap (python, in) x86_fpu_env_t * {
#define struct_x86_fpu_env_t_type_p struct_x86_fpu_env_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_x86_fpu_env_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct x86_reg_access_interface {
#define struct_x86_reg_access_interface_t_type_p struct_x86_reg_access_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_x86_reg_access_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a x86_reg_access_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct x86_reg_access_interface *, $source);
} default_value { (const x86_reg_access_interface_t){ .get_gpr = 0 } };

/* generated */
%typemap (python, struct-out) struct x86_reg_access_interface {
#define struct_x86_reg_access_interface_t_type_p struct_x86_reg_access_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_x86_reg_access_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct x86_reg_access_interface {
#define struct_x86_reg_access_interface_t_type_p struct_x86_reg_access_interface_t_type_p
	struct x86_reg_access_interface *src = MM_MALLOC(1, struct x86_reg_access_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_x86_reg_access_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct x86_reg_access_interface * {
#define struct_x86_reg_access_interface_t_type_p struct_x86_reg_access_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_x86_reg_access_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct x86_reg_access_interface * {
#define struct_x86_reg_access_interface_t_type_p struct_x86_reg_access_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_x86_reg_access_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct x86_pkg_cstate_interface {
#define struct_x86_pkg_cstate_interface_t_type_p struct_x86_pkg_cstate_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_x86_pkg_cstate_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a x86_pkg_cstate_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct x86_pkg_cstate_interface *, $source);
} default_value { (const x86_pkg_cstate_interface_t){ .get_pkg_cstate = 0 } };

/* generated */
%typemap (python, struct-out) struct x86_pkg_cstate_interface {
#define struct_x86_pkg_cstate_interface_t_type_p struct_x86_pkg_cstate_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_x86_pkg_cstate_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct x86_pkg_cstate_interface {
#define struct_x86_pkg_cstate_interface_t_type_p struct_x86_pkg_cstate_interface_t_type_p
	struct x86_pkg_cstate_interface *src = MM_MALLOC(1, struct x86_pkg_cstate_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_x86_pkg_cstate_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct x86_pkg_cstate_interface * {
#define struct_x86_pkg_cstate_interface_t_type_p struct_x86_pkg_cstate_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_x86_pkg_cstate_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct x86_pkg_cstate_interface * {
#define struct_x86_pkg_cstate_interface_t_type_p struct_x86_pkg_cstate_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_x86_pkg_cstate_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct __anonymous67 {
#define struct_x86_msr_getter_ret_t_type_p struct_x86_msr_getter_ret_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_x86_msr_getter_ret_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a x86_msr_getter_ret_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(x86_msr_getter_ret_t *, $source);
} default_value { (const x86_msr_getter_ret_t){ .status = 0 } };

/* generated */
%typemap (python, struct-out) struct __anonymous67 {
#define struct_x86_msr_getter_ret_t_type_p struct_x86_msr_getter_ret_t_type_p
$target = new_opaque_object(&$source, false, false, struct_x86_msr_getter_ret_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous67 {
#define struct_x86_msr_getter_ret_t_type_p struct_x86_msr_getter_ret_t_type_p
	x86_msr_getter_ret_t *src = MM_MALLOC(1, x86_msr_getter_ret_t);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_x86_msr_getter_ret_t_type_p, NULL);
};

/* generated */
%typemap (python, out) x86_msr_getter_ret_t * {
#define struct_x86_msr_getter_ret_t_type_p struct_x86_msr_getter_ret_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_x86_msr_getter_ret_t_type_p, NULL);
};

/* generated */
%typemap (python, in) x86_msr_getter_ret_t * {
#define struct_x86_msr_getter_ret_t_type_p struct_x86_msr_getter_ret_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_x86_msr_getter_ret_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct x86_msr_interface {
#define struct_x86_msr_interface_t_type_p struct_x86_msr_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_x86_msr_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a x86_msr_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct x86_msr_interface *, $source);
} default_value { (const x86_msr_interface_t){ .register_handlers = 0 } };

/* generated */
%typemap (python, struct-out) struct x86_msr_interface {
#define struct_x86_msr_interface_t_type_p struct_x86_msr_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_x86_msr_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct x86_msr_interface {
#define struct_x86_msr_interface_t_type_p struct_x86_msr_interface_t_type_p
	struct x86_msr_interface *src = MM_MALLOC(1, struct x86_msr_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_x86_msr_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct x86_msr_interface * {
#define struct_x86_msr_interface_t_type_p struct_x86_msr_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_x86_msr_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct x86_msr_interface * {
#define struct_x86_msr_interface_t_type_p struct_x86_msr_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_x86_msr_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct x86_memory_query_interface {
#define struct_x86_memory_query_interface_t_type_p struct_x86_memory_query_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_x86_memory_query_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a x86_memory_query_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct x86_memory_query_interface *, $source);
} default_value { (const x86_memory_query_interface_t){ .linear_address = 0 } };

/* generated */
%typemap (python, struct-out) struct x86_memory_query_interface {
#define struct_x86_memory_query_interface_t_type_p struct_x86_memory_query_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_x86_memory_query_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct x86_memory_query_interface {
#define struct_x86_memory_query_interface_t_type_p struct_x86_memory_query_interface_t_type_p
	struct x86_memory_query_interface *src = MM_MALLOC(1, struct x86_memory_query_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_x86_memory_query_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct x86_memory_query_interface * {
#define struct_x86_memory_query_interface_t_type_p struct_x86_memory_query_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_x86_memory_query_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct x86_memory_query_interface * {
#define struct_x86_memory_query_interface_t_type_p struct_x86_memory_query_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_x86_memory_query_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct x86_memory_access_interface {
#define struct_x86_memory_access_interface_t_type_p struct_x86_memory_access_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_x86_memory_access_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a x86_memory_access_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct x86_memory_access_interface *, $source);
} default_value { (const x86_memory_access_interface_t){ .read_logical = 0 } };

/* generated */
%typemap (python, struct-out) struct x86_memory_access_interface {
#define struct_x86_memory_access_interface_t_type_p struct_x86_memory_access_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_x86_memory_access_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct x86_memory_access_interface {
#define struct_x86_memory_access_interface_t_type_p struct_x86_memory_access_interface_t_type_p
	struct x86_memory_access_interface *src = MM_MALLOC(1, struct x86_memory_access_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_x86_memory_access_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct x86_memory_access_interface * {
#define struct_x86_memory_access_interface_t_type_p struct_x86_memory_access_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_x86_memory_access_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct x86_memory_access_interface * {
#define struct_x86_memory_access_interface_t_type_p struct_x86_memory_access_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_x86_memory_access_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct x86_instrumentation_subscribe_v2_interface {
#define struct_x86_instrumentation_subscribe_v2_interface_t_type_p struct_x86_instrumentation_subscribe_v2_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_x86_instrumentation_subscribe_v2_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a x86_instrumentation_subscribe_v2_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct x86_instrumentation_subscribe_v2_interface *, $source);
} default_value { (const x86_instrumentation_subscribe_v2_interface_t){ .register_mode_enter_cb = 0 } };

/* generated */
%typemap (python, struct-out) struct x86_instrumentation_subscribe_v2_interface {
#define struct_x86_instrumentation_subscribe_v2_interface_t_type_p struct_x86_instrumentation_subscribe_v2_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_x86_instrumentation_subscribe_v2_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct x86_instrumentation_subscribe_v2_interface {
#define struct_x86_instrumentation_subscribe_v2_interface_t_type_p struct_x86_instrumentation_subscribe_v2_interface_t_type_p
	struct x86_instrumentation_subscribe_v2_interface *src = MM_MALLOC(1, struct x86_instrumentation_subscribe_v2_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_x86_instrumentation_subscribe_v2_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct x86_instrumentation_subscribe_v2_interface * {
#define struct_x86_instrumentation_subscribe_v2_interface_t_type_p struct_x86_instrumentation_subscribe_v2_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_x86_instrumentation_subscribe_v2_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct x86_instrumentation_subscribe_v2_interface * {
#define struct_x86_instrumentation_subscribe_v2_interface_t_type_p struct_x86_instrumentation_subscribe_v2_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_x86_instrumentation_subscribe_v2_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct x86_instrumentation_subscribe_interface {
#define struct_x86_instrumentation_subscribe_interface_t_type_p struct_x86_instrumentation_subscribe_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_x86_instrumentation_subscribe_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a x86_instrumentation_subscribe_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct x86_instrumentation_subscribe_interface *, $source);
} default_value { (const x86_instrumentation_subscribe_interface_t){ .register_mode_switch_cb = 0 } };

/* generated */
%typemap (python, struct-out) struct x86_instrumentation_subscribe_interface {
#define struct_x86_instrumentation_subscribe_interface_t_type_p struct_x86_instrumentation_subscribe_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_x86_instrumentation_subscribe_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct x86_instrumentation_subscribe_interface {
#define struct_x86_instrumentation_subscribe_interface_t_type_p struct_x86_instrumentation_subscribe_interface_t_type_p
	struct x86_instrumentation_subscribe_interface *src = MM_MALLOC(1, struct x86_instrumentation_subscribe_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_x86_instrumentation_subscribe_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct x86_instrumentation_subscribe_interface * {
#define struct_x86_instrumentation_subscribe_interface_t_type_p struct_x86_instrumentation_subscribe_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_x86_instrumentation_subscribe_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct x86_instrumentation_subscribe_interface * {
#define struct_x86_instrumentation_subscribe_interface_t_type_p struct_x86_instrumentation_subscribe_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_x86_instrumentation_subscribe_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct x86_instruction_query_interface {
#define struct_x86_instruction_query_interface_t_type_p struct_x86_instruction_query_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_x86_instruction_query_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a x86_instruction_query_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct x86_instruction_query_interface *, $source);
} default_value { (const x86_instruction_query_interface_t){ .linear_address = 0 } };

/* generated */
%typemap (python, struct-out) struct x86_instruction_query_interface {
#define struct_x86_instruction_query_interface_t_type_p struct_x86_instruction_query_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_x86_instruction_query_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct x86_instruction_query_interface {
#define struct_x86_instruction_query_interface_t_type_p struct_x86_instruction_query_interface_t_type_p
	struct x86_instruction_query_interface *src = MM_MALLOC(1, struct x86_instruction_query_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_x86_instruction_query_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct x86_instruction_query_interface * {
#define struct_x86_instruction_query_interface_t_type_p struct_x86_instruction_query_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_x86_instruction_query_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct x86_instruction_query_interface * {
#define struct_x86_instruction_query_interface_t_type_p struct_x86_instruction_query_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_x86_instruction_query_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct x86_exception_query_interface {
#define struct_x86_exception_query_interface_t_type_p struct_x86_exception_query_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_x86_exception_query_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a x86_exception_query_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct x86_exception_query_interface *, $source);
} default_value { (const x86_exception_query_interface_t){ .vector = 0 } };

/* generated */
%typemap (python, struct-out) struct x86_exception_query_interface {
#define struct_x86_exception_query_interface_t_type_p struct_x86_exception_query_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_x86_exception_query_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct x86_exception_query_interface {
#define struct_x86_exception_query_interface_t_type_p struct_x86_exception_query_interface_t_type_p
	struct x86_exception_query_interface *src = MM_MALLOC(1, struct x86_exception_query_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_x86_exception_query_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct x86_exception_query_interface * {
#define struct_x86_exception_query_interface_t_type_p struct_x86_exception_query_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_x86_exception_query_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct x86_exception_query_interface * {
#define struct_x86_exception_query_interface_t_type_p struct_x86_exception_query_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_x86_exception_query_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct x86_exception_interface {
#define struct_x86_exception_interface_t_type_p struct_x86_exception_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_x86_exception_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a x86_exception_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct x86_exception_interface *, $source);
} default_value { (const x86_exception_interface_t){ .DE_fault = 0 } };

/* generated */
%typemap (python, struct-out) struct x86_exception_interface {
#define struct_x86_exception_interface_t_type_p struct_x86_exception_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_x86_exception_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct x86_exception_interface {
#define struct_x86_exception_interface_t_type_p struct_x86_exception_interface_t_type_p
	struct x86_exception_interface *src = MM_MALLOC(1, struct x86_exception_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_x86_exception_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct x86_exception_interface * {
#define struct_x86_exception_interface_t_type_p struct_x86_exception_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_x86_exception_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct x86_exception_interface * {
#define struct_x86_exception_interface_t_type_p struct_x86_exception_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_x86_exception_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct x86_ept_interface {
#define struct_x86_ept_interface_t_type_p struct_x86_ept_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_x86_ept_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a x86_ept_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct x86_ept_interface *, $source);
} default_value { (const x86_ept_interface_t){ .guest_physical_to_physical = 0 } };

/* generated */
%typemap (python, struct-out) struct x86_ept_interface {
#define struct_x86_ept_interface_t_type_p struct_x86_ept_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_x86_ept_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct x86_ept_interface {
#define struct_x86_ept_interface_t_type_p struct_x86_ept_interface_t_type_p
	struct x86_ept_interface *src = MM_MALLOC(1, struct x86_ept_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_x86_ept_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct x86_ept_interface * {
#define struct_x86_ept_interface_t_type_p struct_x86_ept_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_x86_ept_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct x86_ept_interface * {
#define struct_x86_ept_interface_t_type_p struct_x86_ept_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_x86_ept_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct x86_cstate_notification_interface {
#define struct_x86_cstate_notification_interface_t_type_p struct_x86_cstate_notification_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_x86_cstate_notification_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a x86_cstate_notification_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct x86_cstate_notification_interface *, $source);
} default_value { (const x86_cstate_notification_interface_t){ .notification = 0 } };

/* generated */
%typemap (python, struct-out) struct x86_cstate_notification_interface {
#define struct_x86_cstate_notification_interface_t_type_p struct_x86_cstate_notification_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_x86_cstate_notification_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct x86_cstate_notification_interface {
#define struct_x86_cstate_notification_interface_t_type_p struct_x86_cstate_notification_interface_t_type_p
	struct x86_cstate_notification_interface *src = MM_MALLOC(1, struct x86_cstate_notification_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_x86_cstate_notification_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct x86_cstate_notification_interface * {
#define struct_x86_cstate_notification_interface_t_type_p struct_x86_cstate_notification_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_x86_cstate_notification_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct x86_cstate_notification_interface * {
#define struct_x86_cstate_notification_interface_t_type_p struct_x86_cstate_notification_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_x86_cstate_notification_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct __anonymous72 {
#define struct_x86_cstate_t_type_p struct_x86_cstate_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_x86_cstate_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a x86_cstate_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(x86_cstate_t *, $source);
} default_value { (const x86_cstate_t){ .state = 0 } };

/* generated */
%typemap (python, struct-out) struct __anonymous72 {
#define struct_x86_cstate_t_type_p struct_x86_cstate_t_type_p
$target = new_opaque_object(&$source, false, false, struct_x86_cstate_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous72 {
#define struct_x86_cstate_t_type_p struct_x86_cstate_t_type_p
	x86_cstate_t *src = MM_MALLOC(1, x86_cstate_t);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_x86_cstate_t_type_p, NULL);
};

/* generated */
%typemap (python, out) x86_cstate_t * {
#define struct_x86_cstate_t_type_p struct_x86_cstate_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_x86_cstate_t_type_p, NULL);
};

/* generated */
%typemap (python, in) x86_cstate_t * {
#define struct_x86_cstate_t_type_p struct_x86_cstate_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_x86_cstate_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct x86_cstate_interface {
#define struct_x86_cstate_interface_t_type_p struct_x86_cstate_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_x86_cstate_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a x86_cstate_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct x86_cstate_interface *, $source);
} default_value { (const x86_cstate_interface_t){ .get_cstate = 0 } };

/* generated */
%typemap (python, struct-out) struct x86_cstate_interface {
#define struct_x86_cstate_interface_t_type_p struct_x86_cstate_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_x86_cstate_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct x86_cstate_interface {
#define struct_x86_cstate_interface_t_type_p struct_x86_cstate_interface_t_type_p
	struct x86_cstate_interface *src = MM_MALLOC(1, struct x86_cstate_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_x86_cstate_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct x86_cstate_interface * {
#define struct_x86_cstate_interface_t_type_p struct_x86_cstate_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_x86_cstate_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct x86_cstate_interface * {
#define struct_x86_cstate_interface_t_type_p struct_x86_cstate_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_x86_cstate_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct __anonymous73 {
#define struct_cpuid_value_t_type_p struct_cpuid_value_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_cpuid_value_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a cpuid_value_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(cpuid_value_t *, $source);
} default_value { (const cpuid_value_t){ .a = 0 } };

/* generated */
%typemap (python, struct-out) struct __anonymous73 {
#define struct_cpuid_value_t_type_p struct_cpuid_value_t_type_p
$target = new_opaque_object(&$source, false, false, struct_cpuid_value_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous73 {
#define struct_cpuid_value_t_type_p struct_cpuid_value_t_type_p
	cpuid_value_t *src = MM_MALLOC(1, cpuid_value_t);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_cpuid_value_t_type_p, NULL);
};

/* generated */
%typemap (python, out) cpuid_value_t * {
#define struct_cpuid_value_t_type_p struct_cpuid_value_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_cpuid_value_t_type_p, NULL);
};

/* generated */
%typemap (python, in) cpuid_value_t * {
#define struct_cpuid_value_t_type_p struct_cpuid_value_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_cpuid_value_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct x86_cpuid_query_interface {
#define struct_x86_cpuid_query_interface_t_type_p struct_x86_cpuid_query_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_x86_cpuid_query_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a x86_cpuid_query_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct x86_cpuid_query_interface *, $source);
} default_value { (const x86_cpuid_query_interface_t){ .cpuid_query = 0 } };

/* generated */
%typemap (python, struct-out) struct x86_cpuid_query_interface {
#define struct_x86_cpuid_query_interface_t_type_p struct_x86_cpuid_query_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_x86_cpuid_query_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct x86_cpuid_query_interface {
#define struct_x86_cpuid_query_interface_t_type_p struct_x86_cpuid_query_interface_t_type_p
	struct x86_cpuid_query_interface *src = MM_MALLOC(1, struct x86_cpuid_query_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_x86_cpuid_query_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct x86_cpuid_query_interface * {
#define struct_x86_cpuid_query_interface_t_type_p struct_x86_cpuid_query_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_x86_cpuid_query_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct x86_cpuid_query_interface * {
#define struct_x86_cpuid_query_interface_t_type_p struct_x86_cpuid_query_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_x86_cpuid_query_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct __anonymous74 {
#define struct_cpuid_ret_t_type_p struct_cpuid_ret_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_cpuid_ret_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a cpuid_ret_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(cpuid_ret_t *, $source);
} default_value { (const cpuid_ret_t){ .taken = 0 } };

/* generated */
%typemap (python, struct-out) struct __anonymous74 {
#define struct_cpuid_ret_t_type_p struct_cpuid_ret_t_type_p
$target = new_opaque_object(&$source, false, false, struct_cpuid_ret_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous74 {
#define struct_cpuid_ret_t_type_p struct_cpuid_ret_t_type_p
	cpuid_ret_t *src = MM_MALLOC(1, cpuid_ret_t);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_cpuid_ret_t_type_p, NULL);
};

/* generated */
%typemap (python, out) cpuid_ret_t * {
#define struct_cpuid_ret_t_type_p struct_cpuid_ret_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_cpuid_ret_t_type_p, NULL);
};

/* generated */
%typemap (python, in) cpuid_ret_t * {
#define struct_cpuid_ret_t_type_p struct_cpuid_ret_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_cpuid_ret_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct x86_cpuid_interface {
#define struct_x86_cpuid_interface_t_type_p struct_x86_cpuid_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_x86_cpuid_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a x86_cpuid_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct x86_cpuid_interface *, $source);
} default_value { (const x86_cpuid_interface_t){ .cpuid = 0 } };

/* generated */
%typemap (python, struct-out) struct x86_cpuid_interface {
#define struct_x86_cpuid_interface_t_type_p struct_x86_cpuid_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_x86_cpuid_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct x86_cpuid_interface {
#define struct_x86_cpuid_interface_t_type_p struct_x86_cpuid_interface_t_type_p
	struct x86_cpuid_interface *src = MM_MALLOC(1, struct x86_cpuid_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_x86_cpuid_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct x86_cpuid_interface * {
#define struct_x86_cpuid_interface_t_type_p struct_x86_cpuid_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_x86_cpuid_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct x86_cpuid_interface * {
#define struct_x86_cpuid_interface_t_type_p struct_x86_cpuid_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_x86_cpuid_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct x86_cache_flush_interface {
#define struct_x86_cache_flush_interface_t_type_p struct_x86_cache_flush_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_x86_cache_flush_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a x86_cache_flush_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct x86_cache_flush_interface *, $source);
} default_value { (const x86_cache_flush_interface_t){ .flush = 0 } };

/* generated */
%typemap (python, struct-out) struct x86_cache_flush_interface {
#define struct_x86_cache_flush_interface_t_type_p struct_x86_cache_flush_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_x86_cache_flush_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct x86_cache_flush_interface {
#define struct_x86_cache_flush_interface_t_type_p struct_x86_cache_flush_interface_t_type_p
	struct x86_cache_flush_interface *src = MM_MALLOC(1, struct x86_cache_flush_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_x86_cache_flush_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct x86_cache_flush_interface * {
#define struct_x86_cache_flush_interface_t_type_p struct_x86_cache_flush_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_x86_cache_flush_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct x86_cache_flush_interface * {
#define struct_x86_cache_flush_interface_t_type_p struct_x86_cache_flush_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_x86_cache_flush_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct x86_address_query_interface {
#define struct_x86_address_query_interface_t_type_p struct_x86_address_query_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_x86_address_query_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a x86_address_query_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct x86_address_query_interface *, $source);
} default_value { (const x86_address_query_interface_t){ .segment = 0 } };

/* generated */
%typemap (python, struct-out) struct x86_address_query_interface {
#define struct_x86_address_query_interface_t_type_p struct_x86_address_query_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_x86_address_query_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct x86_address_query_interface {
#define struct_x86_address_query_interface_t_type_p struct_x86_address_query_interface_t_type_p
	struct x86_address_query_interface *src = MM_MALLOC(1, struct x86_address_query_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_x86_address_query_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct x86_address_query_interface * {
#define struct_x86_address_query_interface_t_type_p struct_x86_address_query_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_x86_address_query_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct x86_address_query_interface * {
#define struct_x86_address_query_interface_t_type_p struct_x86_address_query_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_x86_address_query_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct x86_access_type_interface {
#define struct_x86_access_type_interface_t_type_p struct_x86_access_type_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_x86_access_type_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a x86_access_type_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct x86_access_type_interface *, $source);
} default_value { (const x86_access_type_interface_t){ .get_enum_name = 0 } };

/* generated */
%typemap (python, struct-out) struct x86_access_type_interface {
#define struct_x86_access_type_interface_t_type_p struct_x86_access_type_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_x86_access_type_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct x86_access_type_interface {
#define struct_x86_access_type_interface_t_type_p struct_x86_access_type_interface_t_type_p
	struct x86_access_type_interface *src = MM_MALLOC(1, struct x86_access_type_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_x86_access_type_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct x86_access_type_interface * {
#define struct_x86_access_type_interface_t_type_p struct_x86_access_type_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_x86_access_type_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct x86_access_type_interface * {
#define struct_x86_access_type_interface_t_type_p struct_x86_access_type_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_x86_access_type_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct __anonymous77 {
#define struct_tagged_linear_address_t_type_p struct_tagged_linear_address_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_tagged_linear_address_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a tagged_linear_address_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(tagged_linear_address_t *, $source);
} default_value { (const tagged_linear_address_t){ .valid = 0 } };

/* generated */
%typemap (python, struct-out) struct __anonymous77 {
#define struct_tagged_linear_address_t_type_p struct_tagged_linear_address_t_type_p
$target = new_opaque_object(&$source, false, false, struct_tagged_linear_address_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous77 {
#define struct_tagged_linear_address_t_type_p struct_tagged_linear_address_t_type_p
	tagged_linear_address_t *src = MM_MALLOC(1, tagged_linear_address_t);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_tagged_linear_address_t_type_p, NULL);
};

/* generated */
%typemap (python, out) tagged_linear_address_t * {
#define struct_tagged_linear_address_t_type_p struct_tagged_linear_address_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_tagged_linear_address_t_type_p, NULL);
};

/* generated */
%typemap (python, in) tagged_linear_address_t * {
#define struct_tagged_linear_address_t_type_p struct_tagged_linear_address_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_tagged_linear_address_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct x86_interface {
#define struct_x86_interface_t_type_p struct_x86_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_x86_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a x86_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct x86_interface *, $source);
} default_value { (const x86_interface_t){ .set_pin_status = 0 } };

/* generated */
%typemap (python, struct-out) struct x86_interface {
#define struct_x86_interface_t_type_p struct_x86_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_x86_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct x86_interface {
#define struct_x86_interface_t_type_p struct_x86_interface_t_type_p
	struct x86_interface *src = MM_MALLOC(1, struct x86_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_x86_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct x86_interface * {
#define struct_x86_interface_t_type_p struct_x86_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_x86_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct x86_interface * {
#define struct_x86_interface_t_type_p struct_x86_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_x86_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct winsome_console_interface {
#define struct_winsome_console_interface_t_type_p struct_winsome_console_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_winsome_console_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a winsome_console_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct winsome_console_interface *, $source);
} default_value { (const winsome_console_interface_t){ .gfx = 0 } };

/* generated */
%typemap (python, struct-out) struct winsome_console_interface {
#define struct_winsome_console_interface_t_type_p struct_winsome_console_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_winsome_console_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct winsome_console_interface {
#define struct_winsome_console_interface_t_type_p struct_winsome_console_interface_t_type_p
	struct winsome_console_interface *src = MM_MALLOC(1, struct winsome_console_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_winsome_console_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct winsome_console_interface * {
#define struct_winsome_console_interface_t_type_p struct_winsome_console_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_winsome_console_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct winsome_console_interface * {
#define struct_winsome_console_interface_t_type_p struct_winsome_console_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_winsome_console_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct vnc_server_v2_interface {
#define struct_vnc_server_v2_interface_t_type_p struct_vnc_server_v2_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_vnc_server_v2_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a vnc_server_v2_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct vnc_server_v2_interface *, $source);
} default_value { (const vnc_server_v2_interface_t){ .listening = 0 } };

/* generated */
%typemap (python, struct-out) struct vnc_server_v2_interface {
#define struct_vnc_server_v2_interface_t_type_p struct_vnc_server_v2_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_vnc_server_v2_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct vnc_server_v2_interface {
#define struct_vnc_server_v2_interface_t_type_p struct_vnc_server_v2_interface_t_type_p
	struct vnc_server_v2_interface *src = MM_MALLOC(1, struct vnc_server_v2_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_vnc_server_v2_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct vnc_server_v2_interface * {
#define struct_vnc_server_v2_interface_t_type_p struct_vnc_server_v2_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_vnc_server_v2_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct vnc_server_v2_interface * {
#define struct_vnc_server_v2_interface_t_type_p struct_vnc_server_v2_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_vnc_server_v2_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct vmx_instrumentation_subscribe_interface {
#define struct_vmx_instrumentation_subscribe_interface_t_type_p struct_vmx_instrumentation_subscribe_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_vmx_instrumentation_subscribe_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a vmx_instrumentation_subscribe_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct vmx_instrumentation_subscribe_interface *, $source);
} default_value { (const vmx_instrumentation_subscribe_interface_t){ .register_vmx_mode_leave_cb = 0 } };

/* generated */
%typemap (python, struct-out) struct vmx_instrumentation_subscribe_interface {
#define struct_vmx_instrumentation_subscribe_interface_t_type_p struct_vmx_instrumentation_subscribe_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_vmx_instrumentation_subscribe_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct vmx_instrumentation_subscribe_interface {
#define struct_vmx_instrumentation_subscribe_interface_t_type_p struct_vmx_instrumentation_subscribe_interface_t_type_p
	struct vmx_instrumentation_subscribe_interface *src = MM_MALLOC(1, struct vmx_instrumentation_subscribe_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_vmx_instrumentation_subscribe_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct vmx_instrumentation_subscribe_interface * {
#define struct_vmx_instrumentation_subscribe_interface_t_type_p struct_vmx_instrumentation_subscribe_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_vmx_instrumentation_subscribe_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct vmx_instrumentation_subscribe_interface * {
#define struct_vmx_instrumentation_subscribe_interface_t_type_p struct_vmx_instrumentation_subscribe_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_vmx_instrumentation_subscribe_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct vmp_interface {
#define struct_vmp_interface_t_type_p struct_vmp_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_vmp_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a vmp_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct vmp_interface *, $source);
} default_value { (const vmp_interface_t){ .class_has_support = 0 } };

/* generated */
%typemap (python, struct-out) struct vmp_interface {
#define struct_vmp_interface_t_type_p struct_vmp_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_vmp_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct vmp_interface {
#define struct_vmp_interface_t_type_p struct_vmp_interface_t_type_p
	struct vmp_interface *src = MM_MALLOC(1, struct vmp_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_vmp_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct vmp_interface * {
#define struct_vmp_interface_t_type_p struct_vmp_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_vmp_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct vmp_interface * {
#define struct_vmp_interface_t_type_p struct_vmp_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_vmp_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct virtual_instruction_breakpoint_interface {
#define struct_virtual_instruction_breakpoint_interface_t_type_p struct_virtual_instruction_breakpoint_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_virtual_instruction_breakpoint_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a virtual_instruction_breakpoint_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct virtual_instruction_breakpoint_interface *, $source);
} default_value { (const virtual_instruction_breakpoint_interface_t){ .add = 0 } };

/* generated */
%typemap (python, struct-out) struct virtual_instruction_breakpoint_interface {
#define struct_virtual_instruction_breakpoint_interface_t_type_p struct_virtual_instruction_breakpoint_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_virtual_instruction_breakpoint_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct virtual_instruction_breakpoint_interface {
#define struct_virtual_instruction_breakpoint_interface_t_type_p struct_virtual_instruction_breakpoint_interface_t_type_p
	struct virtual_instruction_breakpoint_interface *src = MM_MALLOC(1, struct virtual_instruction_breakpoint_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_virtual_instruction_breakpoint_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct virtual_instruction_breakpoint_interface * {
#define struct_virtual_instruction_breakpoint_interface_t_type_p struct_virtual_instruction_breakpoint_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_virtual_instruction_breakpoint_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct virtual_instruction_breakpoint_interface * {
#define struct_virtual_instruction_breakpoint_interface_t_type_p struct_virtual_instruction_breakpoint_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_virtual_instruction_breakpoint_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct virtual_data_breakpoint_interface {
#define struct_virtual_data_breakpoint_interface_t_type_p struct_virtual_data_breakpoint_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_virtual_data_breakpoint_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a virtual_data_breakpoint_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct virtual_data_breakpoint_interface *, $source);
} default_value { (const virtual_data_breakpoint_interface_t){ .add_read = 0 } };

/* generated */
%typemap (python, struct-out) struct virtual_data_breakpoint_interface {
#define struct_virtual_data_breakpoint_interface_t_type_p struct_virtual_data_breakpoint_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_virtual_data_breakpoint_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct virtual_data_breakpoint_interface {
#define struct_virtual_data_breakpoint_interface_t_type_p struct_virtual_data_breakpoint_interface_t_type_p
	struct virtual_data_breakpoint_interface *src = MM_MALLOC(1, struct virtual_data_breakpoint_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_virtual_data_breakpoint_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct virtual_data_breakpoint_interface * {
#define struct_virtual_data_breakpoint_interface_t_type_p struct_virtual_data_breakpoint_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_virtual_data_breakpoint_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct virtual_data_breakpoint_interface * {
#define struct_virtual_data_breakpoint_interface_t_type_p struct_virtual_data_breakpoint_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_virtual_data_breakpoint_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct vga_update_interface {
#define struct_vga_update_interface_t_type_p struct_vga_update_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_vga_update_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a vga_update_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct vga_update_interface *, $source);
} default_value { (const vga_update_interface_t){ .refresh = 0 } };

/* generated */
%typemap (python, struct-out) struct vga_update_interface {
#define struct_vga_update_interface_t_type_p struct_vga_update_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_vga_update_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct vga_update_interface {
#define struct_vga_update_interface_t_type_p struct_vga_update_interface_t_type_p
	struct vga_update_interface *src = MM_MALLOC(1, struct vga_update_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_vga_update_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct vga_update_interface * {
#define struct_vga_update_interface_t_type_p struct_vga_update_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_vga_update_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct vga_update_interface * {
#define struct_vga_update_interface_t_type_p struct_vga_update_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_vga_update_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct vga_text_update_interface {
#define struct_vga_text_update_interface_t_type_p struct_vga_text_update_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_vga_text_update_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a vga_text_update_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct vga_text_update_interface *, $source);
} default_value { (const vga_text_update_interface_t){ .write = 0 } };

/* generated */
%typemap (python, struct-out) struct vga_text_update_interface {
#define struct_vga_text_update_interface_t_type_p struct_vga_text_update_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_vga_text_update_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct vga_text_update_interface {
#define struct_vga_text_update_interface_t_type_p struct_vga_text_update_interface_t_type_p
	struct vga_text_update_interface *src = MM_MALLOC(1, struct vga_text_update_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_vga_text_update_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct vga_text_update_interface * {
#define struct_vga_text_update_interface_t_type_p struct_vga_text_update_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_vga_text_update_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct vga_text_update_interface * {
#define struct_vga_text_update_interface_t_type_p struct_vga_text_update_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_vga_text_update_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct vga_text_interface {
#define struct_vga_text_interface_t_type_p struct_vga_text_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_vga_text_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a vga_text_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct vga_text_interface *, $source);
} default_value { (const vga_text_interface_t){ .add_string_notification = 0 } };

/* generated */
%typemap (python, struct-out) struct vga_text_interface {
#define struct_vga_text_interface_t_type_p struct_vga_text_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_vga_text_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct vga_text_interface {
#define struct_vga_text_interface_t_type_p struct_vga_text_interface_t_type_p
	struct vga_text_interface *src = MM_MALLOC(1, struct vga_text_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_vga_text_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct vga_text_interface * {
#define struct_vga_text_interface_t_type_p struct_vga_text_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_vga_text_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct vga_text_interface * {
#define struct_vga_text_interface_t_type_p struct_vga_text_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_vga_text_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct vectored_interrupt_source_interface {
#define struct_vectored_interrupt_source_interface_t_type_p struct_vectored_interrupt_source_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_vectored_interrupt_source_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a vectored_interrupt_source_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct vectored_interrupt_source_interface *, $source);
} default_value { (const vectored_interrupt_source_interface_t){ .acknowledge = 0 } };

/* generated */
%typemap (python, struct-out) struct vectored_interrupt_source_interface {
#define struct_vectored_interrupt_source_interface_t_type_p struct_vectored_interrupt_source_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_vectored_interrupt_source_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct vectored_interrupt_source_interface {
#define struct_vectored_interrupt_source_interface_t_type_p struct_vectored_interrupt_source_interface_t_type_p
	struct vectored_interrupt_source_interface *src = MM_MALLOC(1, struct vectored_interrupt_source_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_vectored_interrupt_source_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct vectored_interrupt_source_interface * {
#define struct_vectored_interrupt_source_interface_t_type_p struct_vectored_interrupt_source_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_vectored_interrupt_source_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct vectored_interrupt_source_interface * {
#define struct_vectored_interrupt_source_interface_t_type_p struct_vectored_interrupt_source_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_vectored_interrupt_source_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct vectored_interrupt_interface {
#define struct_vectored_interrupt_interface_t_type_p struct_vectored_interrupt_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_vectored_interrupt_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a vectored_interrupt_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct vectored_interrupt_interface *, $source);
} default_value { (const vectored_interrupt_interface_t){ .set_level = 0 } };

/* generated */
%typemap (python, struct-out) struct vectored_interrupt_interface {
#define struct_vectored_interrupt_interface_t_type_p struct_vectored_interrupt_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_vectored_interrupt_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct vectored_interrupt_interface {
#define struct_vectored_interrupt_interface_t_type_p struct_vectored_interrupt_interface_t_type_p
	struct vectored_interrupt_interface *src = MM_MALLOC(1, struct vectored_interrupt_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_vectored_interrupt_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct vectored_interrupt_interface * {
#define struct_vectored_interrupt_interface_t_type_p struct_vectored_interrupt_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_vectored_interrupt_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct vectored_interrupt_interface * {
#define struct_vectored_interrupt_interface_t_type_p struct_vectored_interrupt_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_vectored_interrupt_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct usb_device_interface {
#define struct_usb_device_interface_t_type_p struct_usb_device_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_usb_device_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an usb_device_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct usb_device_interface *, $source);
} default_value { (const usb_device_interface_t){ .submit_transfer = 0 } };

/* generated */
%typemap (python, struct-out) struct usb_device_interface {
#define struct_usb_device_interface_t_type_p struct_usb_device_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_usb_device_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct usb_device_interface {
#define struct_usb_device_interface_t_type_p struct_usb_device_interface_t_type_p
	struct usb_device_interface *src = MM_MALLOC(1, struct usb_device_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_usb_device_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct usb_device_interface * {
#define struct_usb_device_interface_t_type_p struct_usb_device_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_usb_device_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct usb_device_interface * {
#define struct_usb_device_interface_t_type_p struct_usb_device_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_usb_device_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct __anonymous88 {
#define struct_usb_transfer_t_type_p struct_usb_transfer_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_usb_transfer_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an usb_transfer_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(usb_transfer_t *NOTNULL, $source);
} default_value { (const usb_transfer_t){ .function_address = 0 } };

/* generated */
%typemap (python, struct-out) struct __anonymous88 {
#define struct_usb_transfer_t_type_p struct_usb_transfer_t_type_p
$target = new_opaque_object(&$source, false, false, struct_usb_transfer_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous88 {
#define struct_usb_transfer_t_type_p struct_usb_transfer_t_type_p
	usb_transfer_t *NOTNULL src = MM_MALLOC(1, usb_transfer_t);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_usb_transfer_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous88 *NOTNULL {
#define struct_usb_transfer_t_type_p struct_usb_transfer_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_usb_transfer_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct __anonymous88 *NOTNULL {
#define struct_usb_transfer_t_type_p struct_usb_transfer_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_usb_transfer_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct usb_interface {
#define struct_usb_interface_t_type_p struct_usb_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_usb_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an usb_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct usb_interface *, $source);
} default_value { (const usb_interface_t){ .connect_device = 0 } };

/* generated */
%typemap (python, struct-out) struct usb_interface {
#define struct_usb_interface_t_type_p struct_usb_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_usb_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct usb_interface {
#define struct_usb_interface_t_type_p struct_usb_interface_t_type_p
	struct usb_interface *src = MM_MALLOC(1, struct usb_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_usb_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct usb_interface * {
#define struct_usb_interface_t_type_p struct_usb_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_usb_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct usb_interface * {
#define struct_usb_interface_t_type_p struct_usb_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_usb_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct uint64_state_interface {
#define struct_uint64_state_interface_t_type_p struct_uint64_state_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_uint64_state_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an uint64_state_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct uint64_state_interface *, $source);
} default_value { (const uint64_state_interface_t){ .set = 0 } };

/* generated */
%typemap (python, struct-out) struct uint64_state_interface {
#define struct_uint64_state_interface_t_type_p struct_uint64_state_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_uint64_state_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct uint64_state_interface {
#define struct_uint64_state_interface_t_type_p struct_uint64_state_interface_t_type_p
	struct uint64_state_interface *src = MM_MALLOC(1, struct uint64_state_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_uint64_state_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct uint64_state_interface * {
#define struct_uint64_state_interface_t_type_p struct_uint64_state_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_uint64_state_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct uint64_state_interface * {
#define struct_uint64_state_interface_t_type_p struct_uint64_state_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_uint64_state_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct translator_interface {
#define struct_translator_interface_t_type_p struct_translator_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_translator_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a translator_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct translator_interface *, $source);
} default_value { (const translator_interface_t){ .translate = 0 } };

/* generated */
%typemap (python, struct-out) struct translator_interface {
#define struct_translator_interface_t_type_p struct_translator_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_translator_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct translator_interface {
#define struct_translator_interface_t_type_p struct_translator_interface_t_type_p
	struct translator_interface *src = MM_MALLOC(1, struct translator_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_translator_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct translator_interface * {
#define struct_translator_interface_t_type_p struct_translator_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_translator_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct translator_interface * {
#define struct_translator_interface_t_type_p struct_translator_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_translator_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct translation_flush_interface {
#define struct_translation_flush_interface_t_type_p struct_translation_flush_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_translation_flush_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a translation_flush_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct translation_flush_interface *, $source);
} default_value { (const translation_flush_interface_t){ .flush_range = 0 } };

/* generated */
%typemap (python, struct-out) struct translation_flush_interface {
#define struct_translation_flush_interface_t_type_p struct_translation_flush_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_translation_flush_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct translation_flush_interface {
#define struct_translation_flush_interface_t_type_p struct_translation_flush_interface_t_type_p
	struct translation_flush_interface *src = MM_MALLOC(1, struct translation_flush_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_translation_flush_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct translation_flush_interface * {
#define struct_translation_flush_interface_t_type_p struct_translation_flush_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_translation_flush_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct translation_flush_interface * {
#define struct_translation_flush_interface_t_type_p struct_translation_flush_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_translation_flush_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct translate_interface {
#define struct_translate_interface_t_type_p struct_translate_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_translate_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a translate_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct translate_interface *, $source);
} default_value { (const translate_interface_t){ .translate = 0 } };

/* generated */
%typemap (python, struct-out) struct translate_interface {
#define struct_translate_interface_t_type_p struct_translate_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_translate_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct translate_interface {
#define struct_translate_interface_t_type_p struct_translate_interface_t_type_p
	struct translate_interface *src = MM_MALLOC(1, struct translate_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_translate_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct translate_interface * {
#define struct_translate_interface_t_type_p struct_translate_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_translate_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct translate_interface * {
#define struct_translate_interface_t_type_p struct_translate_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_translate_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct translation {
#define struct_translation_t_type_p struct_translation_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_translation_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a translation_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(translation_t *, $source);
} default_value { (const translation_t){ .target = NULL } };

/* generated */
%typemap (python, struct-out) struct translation {
#define struct_translation_t_type_p struct_translation_t_type_p
$target = new_opaque_object(&$source, false, false, struct_translation_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct translation {
#define struct_translation_t_type_p struct_translation_t_type_p
	translation_t *src = MM_MALLOC(1, struct translation);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_translation_t_type_p, NULL);
};

/* generated */
%typemap (python, out) translation_t * {
#define struct_translation_t_type_p struct_translation_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_translation_t_type_p, NULL);
};

/* generated */
%typemap (python, in) translation_t * {
#define struct_translation_t_type_p struct_translation_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_translation_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct transaction_translator_interface {
#define struct_transaction_translator_interface_t_type_p struct_transaction_translator_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_transaction_translator_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a transaction_translator_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct transaction_translator_interface *, $source);
} default_value { (const transaction_translator_interface_t){ .translate = 0 } };

/* generated */
%typemap (python, struct-out) struct transaction_translator_interface {
#define struct_transaction_translator_interface_t_type_p struct_transaction_translator_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_transaction_translator_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct transaction_translator_interface {
#define struct_transaction_translator_interface_t_type_p struct_transaction_translator_interface_t_type_p
	struct transaction_translator_interface *src = MM_MALLOC(1, struct transaction_translator_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_transaction_translator_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct transaction_translator_interface * {
#define struct_transaction_translator_interface_t_type_p struct_transaction_translator_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_transaction_translator_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct transaction_translator_interface * {
#define struct_transaction_translator_interface_t_type_p struct_transaction_translator_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_transaction_translator_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct __anonymous89 {
#define struct_next_map_t_type_p struct_next_map_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_next_map_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a next_map_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(next_map_t *, $source);
} default_value { (const next_map_t){ .mt = NULL } };

/* generated */
%typemap (python, struct-out) struct __anonymous89 {
#define struct_next_map_t_type_p struct_next_map_t_type_p
$target = new_opaque_object(&$source, false, false, struct_next_map_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous89 {
#define struct_next_map_t_type_p struct_next_map_t_type_p
	next_map_t *src = MM_MALLOC(1, next_map_t);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_next_map_t_type_p, NULL);
};

/* generated */
%typemap (python, out) next_map_t * {
#define struct_next_map_t_type_p struct_next_map_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_next_map_t_type_p, NULL);
};

/* generated */
%typemap (python, in) next_map_t * {
#define struct_next_map_t_type_p struct_next_map_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_next_map_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct map_target {
#define struct_map_target_t_type_p struct_map_target_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_map_target_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a map_target_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct map_target *, $source);
};

/* generated */
%typemap (python, struct-out) struct map_target {
#define struct_map_target_t_type_p struct_map_target_t_type_p
$target = new_opaque_object(&$source, false, false, struct_map_target_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct map_target {
#define struct_map_target_t_type_p struct_map_target_t_type_p
	struct map_target *src = MM_MALLOC(1, struct map_target);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_map_target_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct map_target * {
#define struct_map_target_t_type_p struct_map_target_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_map_target_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct map_target * {
#define struct_map_target_t_type_p struct_map_target_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_map_target_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct transaction_callback_entry {
#define struct_transaction_cb_handle_t_type_p struct_transaction_cb_handle_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_transaction_cb_handle_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a transaction_cb_handle_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct transaction_callback_entry *, $source);
};

/* generated */
%typemap (python, struct-out) struct transaction_callback_entry {
#define struct_transaction_cb_handle_t_type_p struct_transaction_cb_handle_t_type_p
$target = new_opaque_object(&$source, false, false, struct_transaction_cb_handle_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct transaction_callback_entry {
#define struct_transaction_cb_handle_t_type_p struct_transaction_cb_handle_t_type_p
	struct transaction_callback_entry *src = MM_MALLOC(1, struct transaction_callback_entry);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_transaction_cb_handle_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct transaction_callback_entry * {
#define struct_transaction_cb_handle_t_type_p struct_transaction_cb_handle_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_transaction_cb_handle_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct transaction_callback_entry * {
#define struct_transaction_cb_handle_t_type_p struct_transaction_cb_handle_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_transaction_cb_handle_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct transaction_subscribe_interface {
#define struct_transaction_subscribe_interface_t_type_p struct_transaction_subscribe_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_transaction_subscribe_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a transaction_subscribe_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct transaction_subscribe_interface *, $source);
} default_value { (const transaction_subscribe_interface_t){ .remove_callback = 0 } };

/* generated */
%typemap (python, struct-out) struct transaction_subscribe_interface {
#define struct_transaction_subscribe_interface_t_type_p struct_transaction_subscribe_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_transaction_subscribe_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct transaction_subscribe_interface {
#define struct_transaction_subscribe_interface_t_type_p struct_transaction_subscribe_interface_t_type_p
	struct transaction_subscribe_interface *src = MM_MALLOC(1, struct transaction_subscribe_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_transaction_subscribe_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct transaction_subscribe_interface * {
#define struct_transaction_subscribe_interface_t_type_p struct_transaction_subscribe_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_transaction_subscribe_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct transaction_subscribe_interface * {
#define struct_transaction_subscribe_interface_t_type_p struct_transaction_subscribe_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_transaction_subscribe_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct transaction_interface {
#define struct_transaction_interface_t_type_p struct_transaction_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_transaction_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a transaction_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct transaction_interface *, $source);
} default_value { (const transaction_interface_t){ .issue = 0 } };

/* generated */
%typemap (python, struct-out) struct transaction_interface {
#define struct_transaction_interface_t_type_p struct_transaction_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_transaction_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct transaction_interface {
#define struct_transaction_interface_t_type_p struct_transaction_interface_t_type_p
	struct transaction_interface *src = MM_MALLOC(1, struct transaction_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_transaction_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct transaction_interface * {
#define struct_transaction_interface_t_type_p struct_transaction_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_transaction_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct transaction_interface * {
#define struct_transaction_interface_t_type_p struct_transaction_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_transaction_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct timing_model_interface {
#define struct_timing_model_interface_t_type_p struct_timing_model_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_timing_model_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a timing_model_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct timing_model_interface *, $source);
} default_value { (const timing_model_interface_t){ .operate = 0 } };

/* generated */
%typemap (python, struct-out) struct timing_model_interface {
#define struct_timing_model_interface_t_type_p struct_timing_model_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_timing_model_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct timing_model_interface {
#define struct_timing_model_interface_t_type_p struct_timing_model_interface_t_type_p
	struct timing_model_interface *src = MM_MALLOC(1, struct timing_model_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_timing_model_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct timing_model_interface * {
#define struct_timing_model_interface_t_type_p struct_timing_model_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_timing_model_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct timing_model_interface * {
#define struct_timing_model_interface_t_type_p struct_timing_model_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_timing_model_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct text_console_frontend_interface {
#define struct_text_console_frontend_interface_t_type_p struct_text_console_frontend_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_text_console_frontend_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a text_console_frontend_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct text_console_frontend_interface *, $source);
} default_value { (const text_console_frontend_interface_t){ .start = 0 } };

/* generated */
%typemap (python, struct-out) struct text_console_frontend_interface {
#define struct_text_console_frontend_interface_t_type_p struct_text_console_frontend_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_text_console_frontend_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct text_console_frontend_interface {
#define struct_text_console_frontend_interface_t_type_p struct_text_console_frontend_interface_t_type_p
	struct text_console_frontend_interface *src = MM_MALLOC(1, struct text_console_frontend_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_text_console_frontend_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct text_console_frontend_interface * {
#define struct_text_console_frontend_interface_t_type_p struct_text_console_frontend_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_text_console_frontend_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct text_console_frontend_interface * {
#define struct_text_console_frontend_interface_t_type_p struct_text_console_frontend_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_text_console_frontend_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct text_console_backend_interface {
#define struct_text_console_backend_interface_t_type_p struct_text_console_backend_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_text_console_backend_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a text_console_backend_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct text_console_backend_interface *, $source);
} default_value { (const text_console_backend_interface_t){ .input = 0 } };

/* generated */
%typemap (python, struct-out) struct text_console_backend_interface {
#define struct_text_console_backend_interface_t_type_p struct_text_console_backend_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_text_console_backend_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct text_console_backend_interface {
#define struct_text_console_backend_interface_t_type_p struct_text_console_backend_interface_t_type_p
	struct text_console_backend_interface *src = MM_MALLOC(1, struct text_console_backend_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_text_console_backend_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct text_console_backend_interface * {
#define struct_text_console_backend_interface_t_type_p struct_text_console_backend_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_text_console_backend_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct text_console_backend_interface * {
#define struct_text_console_backend_interface_t_type_p struct_text_console_backend_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_text_console_backend_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct terminal_server_interface {
#define struct_terminal_server_interface_t_type_p struct_terminal_server_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_terminal_server_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a terminal_server_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct terminal_server_interface *, $source);
} default_value { (const terminal_server_interface_t){ .write = 0 } };

/* generated */
%typemap (python, struct-out) struct terminal_server_interface {
#define struct_terminal_server_interface_t_type_p struct_terminal_server_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_terminal_server_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct terminal_server_interface {
#define struct_terminal_server_interface_t_type_p struct_terminal_server_interface_t_type_p
	struct terminal_server_interface *src = MM_MALLOC(1, struct terminal_server_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_terminal_server_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct terminal_server_interface * {
#define struct_terminal_server_interface_t_type_p struct_terminal_server_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_terminal_server_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct terminal_server_interface * {
#define struct_terminal_server_interface_t_type_p struct_terminal_server_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_terminal_server_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct terminal_client_interface {
#define struct_terminal_client_interface_t_type_p struct_terminal_client_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_terminal_client_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a terminal_client_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct terminal_client_interface *, $source);
} default_value { (const terminal_client_interface_t){ .write = 0 } };

/* generated */
%typemap (python, struct-out) struct terminal_client_interface {
#define struct_terminal_client_interface_t_type_p struct_terminal_client_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_terminal_client_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct terminal_client_interface {
#define struct_terminal_client_interface_t_type_p struct_terminal_client_interface_t_type_p
	struct terminal_client_interface *src = MM_MALLOC(1, struct terminal_client_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_terminal_client_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct terminal_client_interface * {
#define struct_terminal_client_interface_t_type_p struct_terminal_client_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_terminal_client_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct terminal_client_interface * {
#define struct_terminal_client_interface_t_type_p struct_terminal_client_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_terminal_client_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct temporal_state_interface {
#define struct_temporal_state_interface_t_type_p struct_temporal_state_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_temporal_state_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a temporal_state_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct temporal_state_interface *, $source);
} default_value { (const temporal_state_interface_t){ .save = 0 } };

/* generated */
%typemap (python, struct-out) struct temporal_state_interface {
#define struct_temporal_state_interface_t_type_p struct_temporal_state_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_temporal_state_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct temporal_state_interface {
#define struct_temporal_state_interface_t_type_p struct_temporal_state_interface_t_type_p
	struct temporal_state_interface *src = MM_MALLOC(1, struct temporal_state_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_temporal_state_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct temporal_state_interface * {
#define struct_temporal_state_interface_t_type_p struct_temporal_state_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_temporal_state_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct temporal_state_interface * {
#define struct_temporal_state_interface_t_type_p struct_temporal_state_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_temporal_state_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct telnet_connection_v2_interface {
#define struct_telnet_connection_v2_interface_t_type_p struct_telnet_connection_v2_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_telnet_connection_v2_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a telnet_connection_v2_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct telnet_connection_v2_interface *, $source);
} default_value { (const telnet_connection_v2_interface_t){ .listening = 0 } };

/* generated */
%typemap (python, struct-out) struct telnet_connection_v2_interface {
#define struct_telnet_connection_v2_interface_t_type_p struct_telnet_connection_v2_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_telnet_connection_v2_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct telnet_connection_v2_interface {
#define struct_telnet_connection_v2_interface_t_type_p struct_telnet_connection_v2_interface_t_type_p
	struct telnet_connection_v2_interface *src = MM_MALLOC(1, struct telnet_connection_v2_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_telnet_connection_v2_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct telnet_connection_v2_interface * {
#define struct_telnet_connection_v2_interface_t_type_p struct_telnet_connection_v2_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_telnet_connection_v2_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct telnet_connection_v2_interface * {
#define struct_telnet_connection_v2_interface_t_type_p struct_telnet_connection_v2_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_telnet_connection_v2_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct telemetry_manager_interface {
#define struct_telemetry_manager_interface_t_type_p struct_telemetry_manager_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_telemetry_manager_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a telemetry_manager_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct telemetry_manager_interface *, $source);
} default_value { (const telemetry_manager_interface_t){ .add_data = 0 } };

/* generated */
%typemap (python, struct-out) struct telemetry_manager_interface {
#define struct_telemetry_manager_interface_t_type_p struct_telemetry_manager_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_telemetry_manager_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct telemetry_manager_interface {
#define struct_telemetry_manager_interface_t_type_p struct_telemetry_manager_interface_t_type_p
	struct telemetry_manager_interface *src = MM_MALLOC(1, struct telemetry_manager_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_telemetry_manager_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct telemetry_manager_interface * {
#define struct_telemetry_manager_interface_t_type_p struct_telemetry_manager_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_telemetry_manager_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct telemetry_manager_interface * {
#define struct_telemetry_manager_interface_t_type_p struct_telemetry_manager_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_telemetry_manager_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct telemetry_interface {
#define struct_telemetry_interface_t_type_p struct_telemetry_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_telemetry_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a telemetry_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct telemetry_interface *, $source);
} default_value { (const telemetry_interface_t){ .get_telemetry_class_id = 0 } };

/* generated */
%typemap (python, struct-out) struct telemetry_interface {
#define struct_telemetry_interface_t_type_p struct_telemetry_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_telemetry_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct telemetry_interface {
#define struct_telemetry_interface_t_type_p struct_telemetry_interface_t_type_p
	struct telemetry_interface *src = MM_MALLOC(1, struct telemetry_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_telemetry_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct telemetry_interface * {
#define struct_telemetry_interface_t_type_p struct_telemetry_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_telemetry_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct telemetry_interface * {
#define struct_telemetry_interface_t_type_p struct_telemetry_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_telemetry_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct tcp_connection_info_interface {
#define struct_tcp_connection_info_interface_t_type_p struct_tcp_connection_info_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_tcp_connection_info_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a tcp_connection_info_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct tcp_connection_info_interface *, $source);
} default_value { (const tcp_connection_info_interface_t){ .remote_port = 0 } };

/* generated */
%typemap (python, struct-out) struct tcp_connection_info_interface {
#define struct_tcp_connection_info_interface_t_type_p struct_tcp_connection_info_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_tcp_connection_info_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct tcp_connection_info_interface {
#define struct_tcp_connection_info_interface_t_type_p struct_tcp_connection_info_interface_t_type_p
	struct tcp_connection_info_interface *src = MM_MALLOC(1, struct tcp_connection_info_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_tcp_connection_info_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct tcp_connection_info_interface * {
#define struct_tcp_connection_info_interface_t_type_p struct_tcp_connection_info_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_tcp_connection_info_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct tcp_connection_info_interface * {
#define struct_tcp_connection_info_interface_t_type_p struct_tcp_connection_info_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_tcp_connection_info_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct tcf_channel_interface {
#define struct_tcf_channel_interface_t_type_p struct_tcf_channel_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_tcf_channel_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a tcf_channel_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct tcf_channel_interface *, $source);
} default_value { (const tcf_channel_interface_t){ .start_channel = 0 } };

/* generated */
%typemap (python, struct-out) struct tcf_channel_interface {
#define struct_tcf_channel_interface_t_type_p struct_tcf_channel_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_tcf_channel_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct tcf_channel_interface {
#define struct_tcf_channel_interface_t_type_p struct_tcf_channel_interface_t_type_p
	struct tcf_channel_interface *src = MM_MALLOC(1, struct tcf_channel_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_tcf_channel_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct tcf_channel_interface * {
#define struct_tcf_channel_interface_t_type_p struct_tcf_channel_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_tcf_channel_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct tcf_channel_interface * {
#define struct_tcf_channel_interface_t_type_p struct_tcf_channel_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_tcf_channel_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct table_interface {
#define struct_table_interface_t_type_p struct_table_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_table_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a table_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct table_interface *, $source);
} default_value { (const table_interface_t){ .data = 0 } };

/* generated */
%typemap (python, struct-out) struct table_interface {
#define struct_table_interface_t_type_p struct_table_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_table_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct table_interface {
#define struct_table_interface_t_type_p struct_table_interface_t_type_p
	struct table_interface *src = MM_MALLOC(1, struct table_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_table_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct table_interface * {
#define struct_table_interface_t_type_p struct_table_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_table_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct table_interface * {
#define struct_table_interface_t_type_p struct_table_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_table_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct synchronous_mode_interface {
#define struct_synchronous_mode_interface_t_type_p struct_synchronous_mode_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_synchronous_mode_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a synchronous_mode_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct synchronous_mode_interface *, $source);
} default_value { (const synchronous_mode_interface_t){ .enter = 0 } };

/* generated */
%typemap (python, struct-out) struct synchronous_mode_interface {
#define struct_synchronous_mode_interface_t_type_p struct_synchronous_mode_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_synchronous_mode_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct synchronous_mode_interface {
#define struct_synchronous_mode_interface_t_type_p struct_synchronous_mode_interface_t_type_p
	struct synchronous_mode_interface *src = MM_MALLOC(1, struct synchronous_mode_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_synchronous_mode_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct synchronous_mode_interface * {
#define struct_synchronous_mode_interface_t_type_p struct_synchronous_mode_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_synchronous_mode_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct synchronous_mode_interface * {
#define struct_synchronous_mode_interface_t_type_p struct_synchronous_mode_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_synchronous_mode_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct sync_initial_time_interface {
#define struct_sync_initial_time_interface_t_type_p struct_sync_initial_time_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_sync_initial_time_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a sync_initial_time_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct sync_initial_time_interface *, $source);
} default_value { (const sync_initial_time_interface_t){ .get_initial_global_time = 0 } };

/* generated */
%typemap (python, struct-out) struct sync_initial_time_interface {
#define struct_sync_initial_time_interface_t_type_p struct_sync_initial_time_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_sync_initial_time_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct sync_initial_time_interface {
#define struct_sync_initial_time_interface_t_type_p struct_sync_initial_time_interface_t_type_p
	struct sync_initial_time_interface *src = MM_MALLOC(1, struct sync_initial_time_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_sync_initial_time_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct sync_initial_time_interface * {
#define struct_sync_initial_time_interface_t_type_p struct_sync_initial_time_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_sync_initial_time_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct sync_initial_time_interface * {
#define struct_sync_initial_time_interface_t_type_p struct_sync_initial_time_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_sync_initial_time_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct __anonymous93 {
#define struct_symbol_t_type_p struct_symbol_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_symbol_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a symbol_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(symbol_t *, $source);
} default_value { (const symbol_t){ .name = NULL } };

/* generated */
%typemap (python, struct-out) struct __anonymous93 {
#define struct_symbol_t_type_p struct_symbol_t_type_p
$target = new_opaque_object(&$source, false, false, struct_symbol_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous93 {
#define struct_symbol_t_type_p struct_symbol_t_type_p
	symbol_t *src = MM_MALLOC(1, symbol_t);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_symbol_t_type_p, NULL);
};

/* generated */
%typemap (python, out) symbol_t * {
#define struct_symbol_t_type_p struct_symbol_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_symbol_t_type_p, NULL);
};

/* generated */
%typemap (python, in) symbol_t * {
#define struct_symbol_t_type_p struct_symbol_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_symbol_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct symbols_interface {
#define struct_symbols_interface_t_type_p struct_symbols_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_symbols_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a symbols_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct symbols_interface *, $source);
} default_value { (const symbols_interface_t){ .load_symbols = 0 } };

/* generated */
%typemap (python, struct-out) struct symbols_interface {
#define struct_symbols_interface_t_type_p struct_symbols_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_symbols_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct symbols_interface {
#define struct_symbols_interface_t_type_p struct_symbols_interface_t_type_p
	struct symbols_interface *src = MM_MALLOC(1, struct symbols_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_symbols_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct symbols_interface * {
#define struct_symbols_interface_t_type_p struct_symbols_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_symbols_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct symbols_interface * {
#define struct_symbols_interface_t_type_p struct_symbols_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_symbols_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct step_info_interface {
#define struct_step_info_interface_t_type_p struct_step_info_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_step_info_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a step_info_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct step_info_interface *, $source);
} default_value { (const step_info_interface_t){ .get_halt_steps = 0 } };

/* generated */
%typemap (python, struct-out) struct step_info_interface {
#define struct_step_info_interface_t_type_p struct_step_info_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_step_info_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct step_info_interface {
#define struct_step_info_interface_t_type_p struct_step_info_interface_t_type_p
	struct step_info_interface *src = MM_MALLOC(1, struct step_info_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_step_info_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct step_info_interface * {
#define struct_step_info_interface_t_type_p struct_step_info_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_step_info_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct step_info_interface * {
#define struct_step_info_interface_t_type_p struct_step_info_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_step_info_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct step_instrumentation {
#define struct_step_handle_t_type_p struct_step_handle_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_step_handle_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a step_handle_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct step_instrumentation *, $source);
};

/* generated */
%typemap (python, struct-out) struct step_instrumentation {
#define struct_step_handle_t_type_p struct_step_handle_t_type_p
$target = new_opaque_object(&$source, false, false, struct_step_handle_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct step_instrumentation {
#define struct_step_handle_t_type_p struct_step_handle_t_type_p
	struct step_instrumentation *src = MM_MALLOC(1, struct step_instrumentation);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_step_handle_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct step_instrumentation * {
#define struct_step_handle_t_type_p struct_step_handle_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_step_handle_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct step_instrumentation * {
#define struct_step_handle_t_type_p struct_step_handle_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_step_handle_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct step_event_instrumentation_interface {
#define struct_step_event_instrumentation_interface_t_type_p struct_step_event_instrumentation_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_step_event_instrumentation_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a step_event_instrumentation_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct step_event_instrumentation_interface *, $source);
} default_value { (const step_event_instrumentation_interface_t){ .register_step_event_cb = 0 } };

/* generated */
%typemap (python, struct-out) struct step_event_instrumentation_interface {
#define struct_step_event_instrumentation_interface_t_type_p struct_step_event_instrumentation_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_step_event_instrumentation_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct step_event_instrumentation_interface {
#define struct_step_event_instrumentation_interface_t_type_p struct_step_event_instrumentation_interface_t_type_p
	struct step_event_instrumentation_interface *src = MM_MALLOC(1, struct step_event_instrumentation_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_step_event_instrumentation_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct step_event_instrumentation_interface * {
#define struct_step_event_instrumentation_interface_t_type_p struct_step_event_instrumentation_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_step_event_instrumentation_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct step_event_instrumentation_interface * {
#define struct_step_event_instrumentation_interface_t_type_p struct_step_event_instrumentation_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_step_event_instrumentation_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct __anonymous95 {
#define struct_step_cycle_ratio_t_type_p struct_step_cycle_ratio_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_step_cycle_ratio_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a step_cycle_ratio_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(step_cycle_ratio_t *, $source);
} default_value { (const step_cycle_ratio_t){ .steps = 0 } };

/* generated */
%typemap (python, struct-out) struct __anonymous95 {
#define struct_step_cycle_ratio_t_type_p struct_step_cycle_ratio_t_type_p
$target = new_opaque_object(&$source, false, false, struct_step_cycle_ratio_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous95 {
#define struct_step_cycle_ratio_t_type_p struct_step_cycle_ratio_t_type_p
	step_cycle_ratio_t *src = MM_MALLOC(1, step_cycle_ratio_t);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_step_cycle_ratio_t_type_p, NULL);
};

/* generated */
%typemap (python, out) step_cycle_ratio_t * {
#define struct_step_cycle_ratio_t_type_p struct_step_cycle_ratio_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_step_cycle_ratio_t_type_p, NULL);
};

/* generated */
%typemap (python, in) step_cycle_ratio_t * {
#define struct_step_cycle_ratio_t_type_p struct_step_cycle_ratio_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_step_cycle_ratio_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct step_cycle_ratio_interface {
#define struct_step_cycle_ratio_interface_t_type_p struct_step_cycle_ratio_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_step_cycle_ratio_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a step_cycle_ratio_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct step_cycle_ratio_interface *, $source);
} default_value { (const step_cycle_ratio_interface_t){ .get_ratio = 0 } };

/* generated */
%typemap (python, struct-out) struct step_cycle_ratio_interface {
#define struct_step_cycle_ratio_interface_t_type_p struct_step_cycle_ratio_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_step_cycle_ratio_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct step_cycle_ratio_interface {
#define struct_step_cycle_ratio_interface_t_type_p struct_step_cycle_ratio_interface_t_type_p
	struct step_cycle_ratio_interface *src = MM_MALLOC(1, struct step_cycle_ratio_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_step_cycle_ratio_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct step_cycle_ratio_interface * {
#define struct_step_cycle_ratio_interface_t_type_p struct_step_cycle_ratio_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_step_cycle_ratio_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct step_cycle_ratio_interface * {
#define struct_step_cycle_ratio_interface_t_type_p struct_step_cycle_ratio_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_step_cycle_ratio_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct step_interface {
#define struct_step_interface_t_type_p struct_step_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_step_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a step_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct step_interface *, $source);
} default_value { (const step_interface_t){ .get_step_count = 0 } };

/* generated */
%typemap (python, struct-out) struct step_interface {
#define struct_step_interface_t_type_p struct_step_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_step_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct step_interface {
#define struct_step_interface_t_type_p struct_step_interface_t_type_p
	struct step_interface *src = MM_MALLOC(1, struct step_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_step_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct step_interface * {
#define struct_step_interface_t_type_p struct_step_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_step_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct step_interface * {
#define struct_step_interface_t_type_p struct_step_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_step_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct page {
#define struct_page_t_type_p struct_page_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_page_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a page_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct page *, $source);
};

/* generated */
%typemap (python, struct-out) struct page {
#define struct_page_t_type_p struct_page_t_type_p
$target = new_opaque_object(&$source, false, false, struct_page_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct page {
#define struct_page_t_type_p struct_page_t_type_p
	struct page *src = MM_MALLOC(1, struct page);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_page_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct page * {
#define struct_page_t_type_p struct_page_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_page_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct page * {
#define struct_page_t_type_p struct_page_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_page_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct stc_interface {
#define struct_stc_interface_t_type_p struct_stc_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_stc_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a stc_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct stc_interface *, $source);
} default_value { (const stc_interface_t){ .flush_STC_inv = 0 } };

/* generated */
%typemap (python, struct-out) struct stc_interface {
#define struct_stc_interface_t_type_p struct_stc_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_stc_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct stc_interface {
#define struct_stc_interface_t_type_p struct_stc_interface_t_type_p
	struct stc_interface *src = MM_MALLOC(1, struct stc_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_stc_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct stc_interface * {
#define struct_stc_interface_t_type_p struct_stc_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_stc_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct stc_interface * {
#define struct_stc_interface_t_type_p struct_stc_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_stc_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct stall_interface {
#define struct_stall_interface_t_type_p struct_stall_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_stall_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a stall_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct stall_interface *, $source);
} default_value { (const stall_interface_t){ .get_stall_cycles = 0 } };

/* generated */
%typemap (python, struct-out) struct stall_interface {
#define struct_stall_interface_t_type_p struct_stall_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_stall_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct stall_interface {
#define struct_stall_interface_t_type_p struct_stall_interface_t_type_p
	struct stall_interface *src = MM_MALLOC(1, struct stall_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_stall_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct stall_interface * {
#define struct_stall_interface_t_type_p struct_stall_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_stall_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct stall_interface * {
#define struct_stall_interface_t_type_p struct_stall_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_stall_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct spr_interface {
#define struct_spr_interface_t_type_p struct_spr_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_spr_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a spr_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct spr_interface *, $source);
} default_value { (const spr_interface_t){ .register_user_handlers = 0 } };

/* generated */
%typemap (python, struct-out) struct spr_interface {
#define struct_spr_interface_t_type_p struct_spr_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_spr_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct spr_interface {
#define struct_spr_interface_t_type_p struct_spr_interface_t_type_p
	struct spr_interface *src = MM_MALLOC(1, struct spr_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_spr_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct spr_interface * {
#define struct_spr_interface_t_type_p struct_spr_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_spr_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct spr_interface * {
#define struct_spr_interface_t_type_p struct_spr_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_spr_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct sparc_v8_ecc_fault_injection_interface {
#define struct_sparc_v8_ecc_fault_injection_interface_t_type_p struct_sparc_v8_ecc_fault_injection_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_sparc_v8_ecc_fault_injection_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a sparc_v8_ecc_fault_injection_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct sparc_v8_ecc_fault_injection_interface *, $source);
} default_value { (const sparc_v8_ecc_fault_injection_interface_t){ .inject_instr_access_exception = 0 } };

/* generated */
%typemap (python, struct-out) struct sparc_v8_ecc_fault_injection_interface {
#define struct_sparc_v8_ecc_fault_injection_interface_t_type_p struct_sparc_v8_ecc_fault_injection_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_sparc_v8_ecc_fault_injection_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct sparc_v8_ecc_fault_injection_interface {
#define struct_sparc_v8_ecc_fault_injection_interface_t_type_p struct_sparc_v8_ecc_fault_injection_interface_t_type_p
	struct sparc_v8_ecc_fault_injection_interface *src = MM_MALLOC(1, struct sparc_v8_ecc_fault_injection_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_sparc_v8_ecc_fault_injection_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct sparc_v8_ecc_fault_injection_interface * {
#define struct_sparc_v8_ecc_fault_injection_interface_t_type_p struct_sparc_v8_ecc_fault_injection_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_sparc_v8_ecc_fault_injection_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct sparc_v8_ecc_fault_injection_interface * {
#define struct_sparc_v8_ecc_fault_injection_interface_t_type_p struct_sparc_v8_ecc_fault_injection_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_sparc_v8_ecc_fault_injection_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct sparc_v8_interface {
#define struct_sparc_v8_interface_t_type_p struct_sparc_v8_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_sparc_v8_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a sparc_v8_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct sparc_v8_interface *, $source);
} default_value { (const sparc_v8_interface_t){ .read_window_register = 0 } };

/* generated */
%typemap (python, struct-out) struct sparc_v8_interface {
#define struct_sparc_v8_interface_t_type_p struct_sparc_v8_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_sparc_v8_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct sparc_v8_interface {
#define struct_sparc_v8_interface_t_type_p struct_sparc_v8_interface_t_type_p
	struct sparc_v8_interface *src = MM_MALLOC(1, struct sparc_v8_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_sparc_v8_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct sparc_v8_interface * {
#define struct_sparc_v8_interface_t_type_p struct_sparc_v8_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_sparc_v8_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct sparc_v8_interface * {
#define struct_sparc_v8_interface_t_type_p struct_sparc_v8_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_sparc_v8_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct snoop_memory_interface {
#define struct_snoop_memory_interface_t_type_p struct_snoop_memory_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_snoop_memory_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a snoop_memory_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct snoop_memory_interface *, $source);
} default_value { (const snoop_memory_interface_t){ .operate = 0 } };

/* generated */
%typemap (python, struct-out) struct snoop_memory_interface {
#define struct_snoop_memory_interface_t_type_p struct_snoop_memory_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_snoop_memory_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct snoop_memory_interface {
#define struct_snoop_memory_interface_t_type_p struct_snoop_memory_interface_t_type_p
	struct snoop_memory_interface *src = MM_MALLOC(1, struct snoop_memory_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_snoop_memory_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct snoop_memory_interface * {
#define struct_snoop_memory_interface_t_type_p struct_snoop_memory_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_snoop_memory_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct snoop_memory_interface * {
#define struct_snoop_memory_interface_t_type_p struct_snoop_memory_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_snoop_memory_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct smm_instrumentation_subscribe_interface {
#define struct_smm_instrumentation_subscribe_interface_t_type_p struct_smm_instrumentation_subscribe_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_smm_instrumentation_subscribe_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a smm_instrumentation_subscribe_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct smm_instrumentation_subscribe_interface *, $source);
} default_value { (const smm_instrumentation_subscribe_interface_t){ .register_smm_enter_before_cb = 0 } };

/* generated */
%typemap (python, struct-out) struct smm_instrumentation_subscribe_interface {
#define struct_smm_instrumentation_subscribe_interface_t_type_p struct_smm_instrumentation_subscribe_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_smm_instrumentation_subscribe_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct smm_instrumentation_subscribe_interface {
#define struct_smm_instrumentation_subscribe_interface_t_type_p struct_smm_instrumentation_subscribe_interface_t_type_p
	struct smm_instrumentation_subscribe_interface *src = MM_MALLOC(1, struct smm_instrumentation_subscribe_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_smm_instrumentation_subscribe_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct smm_instrumentation_subscribe_interface * {
#define struct_smm_instrumentation_subscribe_interface_t_type_p struct_smm_instrumentation_subscribe_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_smm_instrumentation_subscribe_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct smm_instrumentation_subscribe_interface * {
#define struct_smm_instrumentation_subscribe_interface_t_type_p struct_smm_instrumentation_subscribe_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_smm_instrumentation_subscribe_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct slaver_message_interface {
#define struct_slaver_message_interface_t_type_p struct_slaver_message_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_slaver_message_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a slaver_message_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct slaver_message_interface *, $source);
} default_value { (const slaver_message_interface_t){ .send = 0 } };

/* generated */
%typemap (python, struct-out) struct slaver_message_interface {
#define struct_slaver_message_interface_t_type_p struct_slaver_message_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_slaver_message_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct slaver_message_interface {
#define struct_slaver_message_interface_t_type_p struct_slaver_message_interface_t_type_p
	struct slaver_message_interface *src = MM_MALLOC(1, struct slaver_message_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_slaver_message_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct slaver_message_interface * {
#define struct_slaver_message_interface_t_type_p struct_slaver_message_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_slaver_message_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct slaver_message_interface * {
#define struct_slaver_message_interface_t_type_p struct_slaver_message_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_slaver_message_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct slave_agent_interface {
#define struct_slave_agent_interface_t_type_p struct_slave_agent_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_slave_agent_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a slave_agent_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct slave_agent_interface *, $source);
} default_value { (const slave_agent_interface_t){ .accept = 0 } };

/* generated */
%typemap (python, struct-out) struct slave_agent_interface {
#define struct_slave_agent_interface_t_type_p struct_slave_agent_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_slave_agent_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct slave_agent_interface {
#define struct_slave_agent_interface_t_type_p struct_slave_agent_interface_t_type_p
	struct slave_agent_interface *src = MM_MALLOC(1, struct slave_agent_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_slave_agent_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct slave_agent_interface * {
#define struct_slave_agent_interface_t_type_p struct_slave_agent_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_slave_agent_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct slave_agent_interface * {
#define struct_slave_agent_interface_t_type_p struct_slave_agent_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_slave_agent_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct simulator_cache_interface {
#define struct_simulator_cache_interface_t_type_p struct_simulator_cache_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_simulator_cache_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a simulator_cache_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct simulator_cache_interface *, $source);
} default_value { (const simulator_cache_interface_t){ .flush = 0 } };

/* generated */
%typemap (python, struct-out) struct simulator_cache_interface {
#define struct_simulator_cache_interface_t_type_p struct_simulator_cache_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_simulator_cache_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct simulator_cache_interface {
#define struct_simulator_cache_interface_t_type_p struct_simulator_cache_interface_t_type_p
	struct simulator_cache_interface *src = MM_MALLOC(1, struct simulator_cache_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_simulator_cache_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct simulator_cache_interface * {
#define struct_simulator_cache_interface_t_type_p struct_simulator_cache_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_simulator_cache_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct simulator_cache_interface * {
#define struct_simulator_cache_interface_t_type_p struct_simulator_cache_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_simulator_cache_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct simple_interrupt_interface {
#define struct_simple_interrupt_interface_t_type_p struct_simple_interrupt_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_simple_interrupt_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a simple_interrupt_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct simple_interrupt_interface *, $source);
} default_value { (const simple_interrupt_interface_t){ .interrupt = 0 } };

/* generated */
%typemap (python, struct-out) struct simple_interrupt_interface {
#define struct_simple_interrupt_interface_t_type_p struct_simple_interrupt_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_simple_interrupt_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct simple_interrupt_interface {
#define struct_simple_interrupt_interface_t_type_p struct_simple_interrupt_interface_t_type_p
	struct simple_interrupt_interface *src = MM_MALLOC(1, struct simple_interrupt_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_simple_interrupt_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct simple_interrupt_interface * {
#define struct_simple_interrupt_interface_t_type_p struct_simple_interrupt_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_simple_interrupt_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct simple_interrupt_interface * {
#define struct_simple_interrupt_interface_t_type_p struct_simple_interrupt_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_simple_interrupt_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct simple_dispatcher_interface {
#define struct_simple_dispatcher_interface_t_type_p struct_simple_dispatcher_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_simple_dispatcher_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a simple_dispatcher_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct simple_dispatcher_interface *, $source);
} default_value { (const simple_dispatcher_interface_t){ .subscribe = 0 } };

/* generated */
%typemap (python, struct-out) struct simple_dispatcher_interface {
#define struct_simple_dispatcher_interface_t_type_p struct_simple_dispatcher_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_simple_dispatcher_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct simple_dispatcher_interface {
#define struct_simple_dispatcher_interface_t_type_p struct_simple_dispatcher_interface_t_type_p
	struct simple_dispatcher_interface *src = MM_MALLOC(1, struct simple_dispatcher_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_simple_dispatcher_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct simple_dispatcher_interface * {
#define struct_simple_dispatcher_interface_t_type_p struct_simple_dispatcher_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_simple_dispatcher_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct simple_dispatcher_interface * {
#define struct_simple_dispatcher_interface_t_type_p struct_simple_dispatcher_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_simple_dispatcher_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct signal_interface {
#define struct_signal_interface_t_type_p struct_signal_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_signal_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a signal_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct signal_interface *, $source);
} default_value { (const signal_interface_t){ .signal_raise = 0 } };

/* generated */
%typemap (python, struct-out) struct signal_interface {
#define struct_signal_interface_t_type_p struct_signal_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_signal_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct signal_interface {
#define struct_signal_interface_t_type_p struct_signal_interface_t_type_p
	struct signal_interface *src = MM_MALLOC(1, struct signal_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_signal_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct signal_interface * {
#define struct_signal_interface_t_type_p struct_signal_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_signal_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct signal_interface * {
#define struct_signal_interface_t_type_p struct_signal_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_signal_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct sh_interrupt_interface {
#define struct_sh_interrupt_interface_t_type_p struct_sh_interrupt_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_sh_interrupt_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a sh_interrupt_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct sh_interrupt_interface *, $source);
} default_value { (const sh_interrupt_interface_t){ .change_pending = 0 } };

/* generated */
%typemap (python, struct-out) struct sh_interrupt_interface {
#define struct_sh_interrupt_interface_t_type_p struct_sh_interrupt_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_sh_interrupt_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct sh_interrupt_interface {
#define struct_sh_interrupt_interface_t_type_p struct_sh_interrupt_interface_t_type_p
	struct sh_interrupt_interface *src = MM_MALLOC(1, struct sh_interrupt_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_sh_interrupt_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct sh_interrupt_interface * {
#define struct_sh_interrupt_interface_t_type_p struct_sh_interrupt_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_sh_interrupt_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct sh_interrupt_interface * {
#define struct_sh_interrupt_interface_t_type_p struct_sh_interrupt_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_sh_interrupt_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct serial_peripheral_interface_slave_interface {
#define struct_serial_peripheral_interface_slave_interface_t_type_p struct_serial_peripheral_interface_slave_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_serial_peripheral_interface_slave_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a serial_peripheral_interface_slave_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct serial_peripheral_interface_slave_interface *, $source);
} default_value { (const serial_peripheral_interface_slave_interface_t){ .spi_request = 0 } };

/* generated */
%typemap (python, struct-out) struct serial_peripheral_interface_slave_interface {
#define struct_serial_peripheral_interface_slave_interface_t_type_p struct_serial_peripheral_interface_slave_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_serial_peripheral_interface_slave_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct serial_peripheral_interface_slave_interface {
#define struct_serial_peripheral_interface_slave_interface_t_type_p struct_serial_peripheral_interface_slave_interface_t_type_p
	struct serial_peripheral_interface_slave_interface *src = MM_MALLOC(1, struct serial_peripheral_interface_slave_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_serial_peripheral_interface_slave_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct serial_peripheral_interface_slave_interface * {
#define struct_serial_peripheral_interface_slave_interface_t_type_p struct_serial_peripheral_interface_slave_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_serial_peripheral_interface_slave_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct serial_peripheral_interface_slave_interface * {
#define struct_serial_peripheral_interface_slave_interface_t_type_p struct_serial_peripheral_interface_slave_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_serial_peripheral_interface_slave_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct serial_peripheral_interface_master_interface {
#define struct_serial_peripheral_interface_master_interface_t_type_p struct_serial_peripheral_interface_master_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_serial_peripheral_interface_master_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a serial_peripheral_interface_master_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct serial_peripheral_interface_master_interface *, $source);
} default_value { (const serial_peripheral_interface_master_interface_t){ .spi_response = 0 } };

/* generated */
%typemap (python, struct-out) struct serial_peripheral_interface_master_interface {
#define struct_serial_peripheral_interface_master_interface_t_type_p struct_serial_peripheral_interface_master_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_serial_peripheral_interface_master_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct serial_peripheral_interface_master_interface {
#define struct_serial_peripheral_interface_master_interface_t_type_p struct_serial_peripheral_interface_master_interface_t_type_p
	struct serial_peripheral_interface_master_interface *src = MM_MALLOC(1, struct serial_peripheral_interface_master_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_serial_peripheral_interface_master_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct serial_peripheral_interface_master_interface * {
#define struct_serial_peripheral_interface_master_interface_t_type_p struct_serial_peripheral_interface_master_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_serial_peripheral_interface_master_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct serial_peripheral_interface_master_interface * {
#define struct_serial_peripheral_interface_master_interface_t_type_p struct_serial_peripheral_interface_master_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_serial_peripheral_interface_master_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct serial_device_interface {
#define struct_serial_device_interface_t_type_p struct_serial_device_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_serial_device_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a serial_device_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct serial_device_interface *, $source);
} default_value { (const serial_device_interface_t){ .write = 0 } };

/* generated */
%typemap (python, struct-out) struct serial_device_interface {
#define struct_serial_device_interface_t_type_p struct_serial_device_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_serial_device_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct serial_device_interface {
#define struct_serial_device_interface_t_type_p struct_serial_device_interface_t_type_p
	struct serial_device_interface *src = MM_MALLOC(1, struct serial_device_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_serial_device_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct serial_device_interface * {
#define struct_serial_device_interface_t_type_p struct_serial_device_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_serial_device_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct serial_device_interface * {
#define struct_serial_device_interface_t_type_p struct_serial_device_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_serial_device_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct serial_console_frontend_interface {
#define struct_serial_console_frontend_interface_t_type_p struct_serial_console_frontend_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_serial_console_frontend_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a serial_console_frontend_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct serial_console_frontend_interface *, $source);
} default_value { (const serial_console_frontend_interface_t){ .write = 0 } };

/* generated */
%typemap (python, struct-out) struct serial_console_frontend_interface {
#define struct_serial_console_frontend_interface_t_type_p struct_serial_console_frontend_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_serial_console_frontend_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct serial_console_frontend_interface {
#define struct_serial_console_frontend_interface_t_type_p struct_serial_console_frontend_interface_t_type_p
	struct serial_console_frontend_interface *src = MM_MALLOC(1, struct serial_console_frontend_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_serial_console_frontend_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct serial_console_frontend_interface * {
#define struct_serial_console_frontend_interface_t_type_p struct_serial_console_frontend_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_serial_console_frontend_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct serial_console_frontend_interface * {
#define struct_serial_console_frontend_interface_t_type_p struct_serial_console_frontend_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_serial_console_frontend_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct screenshot_interface {
#define struct_screenshot_interface_t_type_p struct_screenshot_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_screenshot_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a screenshot_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct screenshot_interface *, $source);
} default_value { (const screenshot_interface_t){ .save_png = 0 } };

/* generated */
%typemap (python, struct-out) struct screenshot_interface {
#define struct_screenshot_interface_t_type_p struct_screenshot_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_screenshot_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct screenshot_interface {
#define struct_screenshot_interface_t_type_p struct_screenshot_interface_t_type_p
	struct screenshot_interface *src = MM_MALLOC(1, struct screenshot_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_screenshot_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct screenshot_interface * {
#define struct_screenshot_interface_t_type_p struct_screenshot_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_screenshot_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct screenshot_interface * {
#define struct_screenshot_interface_t_type_p struct_screenshot_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_screenshot_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct scale_factor_listener_interface {
#define struct_scale_factor_listener_interface_t_type_p struct_scale_factor_listener_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_scale_factor_listener_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a scale_factor_listener_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct scale_factor_listener_interface *, $source);
} default_value { (const scale_factor_listener_interface_t){ .set = 0 } };

/* generated */
%typemap (python, struct-out) struct scale_factor_listener_interface {
#define struct_scale_factor_listener_interface_t_type_p struct_scale_factor_listener_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_scale_factor_listener_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct scale_factor_listener_interface {
#define struct_scale_factor_listener_interface_t_type_p struct_scale_factor_listener_interface_t_type_p
	struct scale_factor_listener_interface *src = MM_MALLOC(1, struct scale_factor_listener_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_scale_factor_listener_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct scale_factor_listener_interface * {
#define struct_scale_factor_listener_interface_t_type_p struct_scale_factor_listener_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_scale_factor_listener_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct scale_factor_listener_interface * {
#define struct_scale_factor_listener_interface_t_type_p struct_scale_factor_listener_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_scale_factor_listener_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct scalar_time_interface {
#define struct_scalar_time_interface_t_type_p struct_scalar_time_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_scalar_time_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a scalar_time_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct scalar_time_interface *, $source);
} default_value { (const scalar_time_interface_t){ .add_consumer = 0 } };

/* generated */
%typemap (python, struct-out) struct scalar_time_interface {
#define struct_scalar_time_interface_t_type_p struct_scalar_time_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_scalar_time_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct scalar_time_interface {
#define struct_scalar_time_interface_t_type_p struct_scalar_time_interface_t_type_p
	struct scalar_time_interface *src = MM_MALLOC(1, struct scalar_time_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_scalar_time_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct scalar_time_interface * {
#define struct_scalar_time_interface_t_type_p struct_scalar_time_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_scalar_time_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct scalar_time_interface * {
#define struct_scalar_time_interface_t_type_p struct_scalar_time_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_scalar_time_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct sata_interface {
#define struct_sata_interface_t_type_p struct_sata_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_sata_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a sata_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct sata_interface *, $source);
} default_value { (const sata_interface_t){ .receive_fis = 0 } };

/* generated */
%typemap (python, struct-out) struct sata_interface {
#define struct_sata_interface_t_type_p struct_sata_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_sata_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct sata_interface {
#define struct_sata_interface_t_type_p struct_sata_interface_t_type_p
	struct sata_interface *src = MM_MALLOC(1, struct sata_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_sata_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct sata_interface * {
#define struct_sata_interface_t_type_p struct_sata_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_sata_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct sata_interface * {
#define struct_sata_interface_t_type_p struct_sata_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_sata_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct rs232_device_interface {
#define struct_rs232_device_interface_t_type_p struct_rs232_device_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_rs232_device_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a rs232_device_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct rs232_device_interface *, $source);
} default_value { (const rs232_device_interface_t){ .cts = 0 } };

/* generated */
%typemap (python, struct-out) struct rs232_device_interface {
#define struct_rs232_device_interface_t_type_p struct_rs232_device_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_rs232_device_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct rs232_device_interface {
#define struct_rs232_device_interface_t_type_p struct_rs232_device_interface_t_type_p
	struct rs232_device_interface *src = MM_MALLOC(1, struct rs232_device_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_rs232_device_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct rs232_device_interface * {
#define struct_rs232_device_interface_t_type_p struct_rs232_device_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_rs232_device_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct rs232_device_interface * {
#define struct_rs232_device_interface_t_type_p struct_rs232_device_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_rs232_device_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct rs232_console_interface {
#define struct_rs232_console_interface_t_type_p struct_rs232_console_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_rs232_console_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a rs232_console_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct rs232_console_interface *, $source);
} default_value { (const rs232_console_interface_t){ .set_baudrate = 0 } };

/* generated */
%typemap (python, struct-out) struct rs232_console_interface {
#define struct_rs232_console_interface_t_type_p struct_rs232_console_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_rs232_console_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct rs232_console_interface {
#define struct_rs232_console_interface_t_type_p struct_rs232_console_interface_t_type_p
	struct rs232_console_interface *src = MM_MALLOC(1, struct rs232_console_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_rs232_console_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct rs232_console_interface * {
#define struct_rs232_console_interface_t_type_p struct_rs232_console_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_rs232_console_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct rs232_console_interface * {
#define struct_rs232_console_interface_t_type_p struct_rs232_console_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_rs232_console_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct riscv_signal_sgeip_interface {
#define struct_riscv_signal_sgeip_interface_t_type_p struct_riscv_signal_sgeip_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_riscv_signal_sgeip_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a riscv_signal_sgeip_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct riscv_signal_sgeip_interface *, $source);
} default_value { (const riscv_signal_sgeip_interface_t){ .signal_raise = 0 } };

/* generated */
%typemap (python, struct-out) struct riscv_signal_sgeip_interface {
#define struct_riscv_signal_sgeip_interface_t_type_p struct_riscv_signal_sgeip_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_riscv_signal_sgeip_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct riscv_signal_sgeip_interface {
#define struct_riscv_signal_sgeip_interface_t_type_p struct_riscv_signal_sgeip_interface_t_type_p
	struct riscv_signal_sgeip_interface *src = MM_MALLOC(1, struct riscv_signal_sgeip_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_riscv_signal_sgeip_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct riscv_signal_sgeip_interface * {
#define struct_riscv_signal_sgeip_interface_t_type_p struct_riscv_signal_sgeip_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_riscv_signal_sgeip_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct riscv_signal_sgeip_interface * {
#define struct_riscv_signal_sgeip_interface_t_type_p struct_riscv_signal_sgeip_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_riscv_signal_sgeip_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct riscv_instruction_action_interface {
#define struct_riscv_instruction_action_interface_t_type_p struct_riscv_instruction_action_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_riscv_instruction_action_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a riscv_instruction_action_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct riscv_instruction_action_interface *, $source);
} default_value { (const riscv_instruction_action_interface_t){ .read_x_register = 0 } };

/* generated */
%typemap (python, struct-out) struct riscv_instruction_action_interface {
#define struct_riscv_instruction_action_interface_t_type_p struct_riscv_instruction_action_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_riscv_instruction_action_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct riscv_instruction_action_interface {
#define struct_riscv_instruction_action_interface_t_type_p struct_riscv_instruction_action_interface_t_type_p
	struct riscv_instruction_action_interface *src = MM_MALLOC(1, struct riscv_instruction_action_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_riscv_instruction_action_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct riscv_instruction_action_interface * {
#define struct_riscv_instruction_action_interface_t_type_p struct_riscv_instruction_action_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_riscv_instruction_action_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct riscv_instruction_action_interface * {
#define struct_riscv_instruction_action_interface_t_type_p struct_riscv_instruction_action_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_riscv_instruction_action_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct riscv_imsic_interface {
#define struct_riscv_imsic_interface_t_type_p struct_riscv_imsic_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_riscv_imsic_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a riscv_imsic_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct riscv_imsic_interface *, $source);
} default_value { (const riscv_imsic_interface_t){ .num_guest_files = 0 } };

/* generated */
%typemap (python, struct-out) struct riscv_imsic_interface {
#define struct_riscv_imsic_interface_t_type_p struct_riscv_imsic_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_riscv_imsic_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct riscv_imsic_interface {
#define struct_riscv_imsic_interface_t_type_p struct_riscv_imsic_interface_t_type_p
	struct riscv_imsic_interface *src = MM_MALLOC(1, struct riscv_imsic_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_riscv_imsic_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct riscv_imsic_interface * {
#define struct_riscv_imsic_interface_t_type_p struct_riscv_imsic_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_riscv_imsic_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct riscv_imsic_interface * {
#define struct_riscv_imsic_interface_t_type_p struct_riscv_imsic_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_riscv_imsic_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct riscv_custom_csr_interface {
#define struct_riscv_custom_csr_interface_t_type_p struct_riscv_custom_csr_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_riscv_custom_csr_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a riscv_custom_csr_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct riscv_custom_csr_interface *, $source);
} default_value { (const riscv_custom_csr_interface_t){ .register_csr = 0 } };

/* generated */
%typemap (python, struct-out) struct riscv_custom_csr_interface {
#define struct_riscv_custom_csr_interface_t_type_p struct_riscv_custom_csr_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_riscv_custom_csr_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct riscv_custom_csr_interface {
#define struct_riscv_custom_csr_interface_t_type_p struct_riscv_custom_csr_interface_t_type_p
	struct riscv_custom_csr_interface *src = MM_MALLOC(1, struct riscv_custom_csr_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_riscv_custom_csr_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct riscv_custom_csr_interface * {
#define struct_riscv_custom_csr_interface_t_type_p struct_riscv_custom_csr_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_riscv_custom_csr_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct riscv_custom_csr_interface * {
#define struct_riscv_custom_csr_interface_t_type_p struct_riscv_custom_csr_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_riscv_custom_csr_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct riscv_coprocessor_interface {
#define struct_riscv_coprocessor_interface_t_type_p struct_riscv_coprocessor_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_riscv_coprocessor_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a riscv_coprocessor_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct riscv_coprocessor_interface *, $source);
} default_value { (const riscv_coprocessor_interface_t){ .read_register = 0 } };

/* generated */
%typemap (python, struct-out) struct riscv_coprocessor_interface {
#define struct_riscv_coprocessor_interface_t_type_p struct_riscv_coprocessor_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_riscv_coprocessor_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct riscv_coprocessor_interface {
#define struct_riscv_coprocessor_interface_t_type_p struct_riscv_coprocessor_interface_t_type_p
	struct riscv_coprocessor_interface *src = MM_MALLOC(1, struct riscv_coprocessor_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_riscv_coprocessor_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct riscv_coprocessor_interface * {
#define struct_riscv_coprocessor_interface_t_type_p struct_riscv_coprocessor_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_riscv_coprocessor_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct riscv_coprocessor_interface * {
#define struct_riscv_coprocessor_interface_t_type_p struct_riscv_coprocessor_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_riscv_coprocessor_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct riscv_clic_interrupt_interface {
#define struct_riscv_clic_interrupt_interface_t_type_p struct_riscv_clic_interrupt_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_riscv_clic_interrupt_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a riscv_clic_interrupt_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct riscv_clic_interrupt_interface *, $source);
} default_value { (const riscv_clic_interrupt_interface_t){ .set_active_interrupt = 0 } };

/* generated */
%typemap (python, struct-out) struct riscv_clic_interrupt_interface {
#define struct_riscv_clic_interrupt_interface_t_type_p struct_riscv_clic_interrupt_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_riscv_clic_interrupt_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct riscv_clic_interrupt_interface {
#define struct_riscv_clic_interrupt_interface_t_type_p struct_riscv_clic_interrupt_interface_t_type_p
	struct riscv_clic_interrupt_interface *src = MM_MALLOC(1, struct riscv_clic_interrupt_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_riscv_clic_interrupt_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct riscv_clic_interrupt_interface * {
#define struct_riscv_clic_interrupt_interface_t_type_p struct_riscv_clic_interrupt_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_riscv_clic_interrupt_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct riscv_clic_interrupt_interface * {
#define struct_riscv_clic_interrupt_interface_t_type_p struct_riscv_clic_interrupt_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_riscv_clic_interrupt_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct riscv_clic_interface {
#define struct_riscv_clic_interface_t_type_p struct_riscv_clic_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_riscv_clic_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a riscv_clic_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct riscv_clic_interface *, $source);
} default_value { (const riscv_clic_interface_t){ .acknowledge_interrupt = 0 } };

/* generated */
%typemap (python, struct-out) struct riscv_clic_interface {
#define struct_riscv_clic_interface_t_type_p struct_riscv_clic_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_riscv_clic_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct riscv_clic_interface {
#define struct_riscv_clic_interface_t_type_p struct_riscv_clic_interface_t_type_p
	struct riscv_clic_interface *src = MM_MALLOC(1, struct riscv_clic_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_riscv_clic_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct riscv_clic_interface * {
#define struct_riscv_clic_interface_t_type_p struct_riscv_clic_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_riscv_clic_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct riscv_clic_interface * {
#define struct_riscv_clic_interface_t_type_p struct_riscv_clic_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_riscv_clic_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct register_view_read_only_interface {
#define struct_register_view_read_only_interface_t_type_p struct_register_view_read_only_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_register_view_read_only_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a register_view_read_only_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct register_view_read_only_interface *, $source);
} default_value { (const register_view_read_only_interface_t){ .is_read_only = 0 } };

/* generated */
%typemap (python, struct-out) struct register_view_read_only_interface {
#define struct_register_view_read_only_interface_t_type_p struct_register_view_read_only_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_register_view_read_only_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct register_view_read_only_interface {
#define struct_register_view_read_only_interface_t_type_p struct_register_view_read_only_interface_t_type_p
	struct register_view_read_only_interface *src = MM_MALLOC(1, struct register_view_read_only_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_register_view_read_only_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct register_view_read_only_interface * {
#define struct_register_view_read_only_interface_t_type_p struct_register_view_read_only_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_register_view_read_only_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct register_view_read_only_interface * {
#define struct_register_view_read_only_interface_t_type_p struct_register_view_read_only_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_register_view_read_only_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct register_view_catalog_interface {
#define struct_register_view_catalog_interface_t_type_p struct_register_view_catalog_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_register_view_catalog_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a register_view_catalog_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct register_view_catalog_interface *, $source);
} default_value { (const register_view_catalog_interface_t){ .register_names = 0 } };

/* generated */
%typemap (python, struct-out) struct register_view_catalog_interface {
#define struct_register_view_catalog_interface_t_type_p struct_register_view_catalog_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_register_view_catalog_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct register_view_catalog_interface {
#define struct_register_view_catalog_interface_t_type_p struct_register_view_catalog_interface_t_type_p
	struct register_view_catalog_interface *src = MM_MALLOC(1, struct register_view_catalog_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_register_view_catalog_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct register_view_catalog_interface * {
#define struct_register_view_catalog_interface_t_type_p struct_register_view_catalog_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_register_view_catalog_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct register_view_catalog_interface * {
#define struct_register_view_catalog_interface_t_type_p struct_register_view_catalog_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_register_view_catalog_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct register_view_interface {
#define struct_register_view_interface_t_type_p struct_register_view_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_register_view_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a register_view_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct register_view_interface *, $source);
} default_value { (const register_view_interface_t){ .description = 0 } };

/* generated */
%typemap (python, struct-out) struct register_view_interface {
#define struct_register_view_interface_t_type_p struct_register_view_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_register_view_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct register_view_interface {
#define struct_register_view_interface_t_type_p struct_register_view_interface_t_type_p
	struct register_view_interface *src = MM_MALLOC(1, struct register_view_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_register_view_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct register_view_interface * {
#define struct_register_view_interface_t_type_p struct_register_view_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_register_view_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct register_view_interface * {
#define struct_register_view_interface_t_type_p struct_register_view_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_register_view_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct register_breakpoint_interface {
#define struct_register_breakpoint_interface_t_type_p struct_register_breakpoint_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_register_breakpoint_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a register_breakpoint_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct register_breakpoint_interface *, $source);
} default_value { (const register_breakpoint_interface_t){ .add_breakpoint = 0 } };

/* generated */
%typemap (python, struct-out) struct register_breakpoint_interface {
#define struct_register_breakpoint_interface_t_type_p struct_register_breakpoint_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_register_breakpoint_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct register_breakpoint_interface {
#define struct_register_breakpoint_interface_t_type_p struct_register_breakpoint_interface_t_type_p
	struct register_breakpoint_interface *src = MM_MALLOC(1, struct register_breakpoint_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_register_breakpoint_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct register_breakpoint_interface * {
#define struct_register_breakpoint_interface_t_type_p struct_register_breakpoint_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_register_breakpoint_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct register_breakpoint_interface * {
#define struct_register_breakpoint_interface_t_type_p struct_register_breakpoint_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_register_breakpoint_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct recorder_v2_interface {
#define struct_recorder_v2_interface_t_type_p struct_recorder_v2_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_recorder_v2_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a recorder_v2_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct recorder_v2_interface *, $source);
} default_value { (const recorder_v2_interface_t){ .record = 0 } };

/* generated */
%typemap (python, struct-out) struct recorder_v2_interface {
#define struct_recorder_v2_interface_t_type_p struct_recorder_v2_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_recorder_v2_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct recorder_v2_interface {
#define struct_recorder_v2_interface_t_type_p struct_recorder_v2_interface_t_type_p
	struct recorder_v2_interface *src = MM_MALLOC(1, struct recorder_v2_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_recorder_v2_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct recorder_v2_interface * {
#define struct_recorder_v2_interface_t_type_p struct_recorder_v2_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_recorder_v2_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct recorder_v2_interface * {
#define struct_recorder_v2_interface_t_type_p struct_recorder_v2_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_recorder_v2_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct recorder {
#define struct_recorder_t_type_p struct_recorder_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_recorder_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a recorder_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct recorder *, $source);
};

/* generated */
%typemap (python, struct-out) struct recorder {
#define struct_recorder_t_type_p struct_recorder_t_type_p
$target = new_opaque_object(&$source, false, false, struct_recorder_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct recorder {
#define struct_recorder_t_type_p struct_recorder_t_type_p
	struct recorder *src = MM_MALLOC(1, struct recorder);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_recorder_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct recorder * {
#define struct_recorder_t_type_p struct_recorder_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_recorder_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct recorder * {
#define struct_recorder_t_type_p struct_recorder_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_recorder_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct recorder_interface {
#define struct_recorder_interface_t_type_p struct_recorder_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_recorder_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a recorder_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct recorder_interface *, $source);
} default_value { (const recorder_interface_t){ .attach = 0 } };

/* generated */
%typemap (python, struct-out) struct recorder_interface {
#define struct_recorder_interface_t_type_p struct_recorder_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_recorder_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct recorder_interface {
#define struct_recorder_interface_t_type_p struct_recorder_interface_t_type_p
	struct recorder_interface *src = MM_MALLOC(1, struct recorder_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_recorder_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct recorder_interface * {
#define struct_recorder_interface_t_type_p struct_recorder_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_recorder_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct recorder_interface * {
#define struct_recorder_interface_t_type_p struct_recorder_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_recorder_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct recorded_interface {
#define struct_recorded_interface_t_type_p struct_recorded_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_recorded_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a recorded_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct recorded_interface *, $source);
} default_value { (const recorded_interface_t){ .input = 0 } };

/* generated */
%typemap (python, struct-out) struct recorded_interface {
#define struct_recorded_interface_t_type_p struct_recorded_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_recorded_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct recorded_interface {
#define struct_recorded_interface_t_type_p struct_recorded_interface_t_type_p
	struct recorded_interface *src = MM_MALLOC(1, struct recorded_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_recorded_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct recorded_interface * {
#define struct_recorded_interface_t_type_p struct_recorded_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_recorded_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct recorded_interface * {
#define struct_recorded_interface_t_type_p struct_recorded_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_recorded_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct ram_callback_entry {
#define struct_ram_cb_handle_t_type_p struct_ram_cb_handle_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_ram_cb_handle_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a ram_cb_handle_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct ram_callback_entry *, $source);
};

/* generated */
%typemap (python, struct-out) struct ram_callback_entry {
#define struct_ram_cb_handle_t_type_p struct_ram_cb_handle_t_type_p
$target = new_opaque_object(&$source, false, false, struct_ram_cb_handle_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct ram_callback_entry {
#define struct_ram_cb_handle_t_type_p struct_ram_cb_handle_t_type_p
	struct ram_callback_entry *src = MM_MALLOC(1, struct ram_callback_entry);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_ram_cb_handle_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct ram_callback_entry * {
#define struct_ram_cb_handle_t_type_p struct_ram_cb_handle_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_ram_cb_handle_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct ram_callback_entry * {
#define struct_ram_cb_handle_t_type_p struct_ram_cb_handle_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_ram_cb_handle_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct ram_access_subscribe_interface {
#define struct_ram_access_subscribe_interface_t_type_p struct_ram_access_subscribe_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_ram_access_subscribe_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a ram_access_subscribe_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct ram_access_subscribe_interface *, $source);
} default_value { (const ram_access_subscribe_interface_t){ .remove_callback = 0 } };

/* generated */
%typemap (python, struct-out) struct ram_access_subscribe_interface {
#define struct_ram_access_subscribe_interface_t_type_p struct_ram_access_subscribe_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_ram_access_subscribe_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct ram_access_subscribe_interface {
#define struct_ram_access_subscribe_interface_t_type_p struct_ram_access_subscribe_interface_t_type_p
	struct ram_access_subscribe_interface *src = MM_MALLOC(1, struct ram_access_subscribe_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_ram_access_subscribe_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct ram_access_subscribe_interface * {
#define struct_ram_access_subscribe_interface_t_type_p struct_ram_access_subscribe_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_ram_access_subscribe_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct ram_access_subscribe_interface * {
#define struct_ram_access_subscribe_interface_t_type_p struct_ram_access_subscribe_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_ram_access_subscribe_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct ram_interface {
#define struct_ram_interface_t_type_p struct_ram_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_ram_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a ram_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct ram_interface *, $source);
} default_value { (const ram_interface_t){ .fill = 0 } };

/* generated */
%typemap (python, struct-out) struct ram_interface {
#define struct_ram_interface_t_type_p struct_ram_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_ram_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct ram_interface {
#define struct_ram_interface_t_type_p struct_ram_interface_t_type_p
	struct ram_interface *src = MM_MALLOC(1, struct ram_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_ram_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct ram_interface * {
#define struct_ram_interface_t_type_p struct_ram_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_ram_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct ram_interface * {
#define struct_ram_interface_t_type_p struct_ram_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_ram_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct pulse_interface {
#define struct_pulse_interface_t_type_p struct_pulse_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_pulse_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a pulse_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct pulse_interface *, $source);
} default_value { (const pulse_interface_t){ .pulse = 0 } };

/* generated */
%typemap (python, struct-out) struct pulse_interface {
#define struct_pulse_interface_t_type_p struct_pulse_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_pulse_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct pulse_interface {
#define struct_pulse_interface_t_type_p struct_pulse_interface_t_type_p
	struct pulse_interface *src = MM_MALLOC(1, struct pulse_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_pulse_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct pulse_interface * {
#define struct_pulse_interface_t_type_p struct_pulse_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_pulse_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct pulse_interface * {
#define struct_pulse_interface_t_type_p struct_pulse_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_pulse_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct processor_info_v2_interface {
#define struct_processor_info_v2_interface_t_type_p struct_processor_info_v2_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_processor_info_v2_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a processor_info_v2_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct processor_info_v2_interface *, $source);
} default_value { (const processor_info_v2_interface_t){ .disassemble = 0 } };

/* generated */
%typemap (python, struct-out) struct processor_info_v2_interface {
#define struct_processor_info_v2_interface_t_type_p struct_processor_info_v2_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_processor_info_v2_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct processor_info_v2_interface {
#define struct_processor_info_v2_interface_t_type_p struct_processor_info_v2_interface_t_type_p
	struct processor_info_v2_interface *src = MM_MALLOC(1, struct processor_info_v2_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_processor_info_v2_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct processor_info_v2_interface * {
#define struct_processor_info_v2_interface_t_type_p struct_processor_info_v2_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_processor_info_v2_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct processor_info_v2_interface * {
#define struct_processor_info_v2_interface_t_type_p struct_processor_info_v2_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_processor_info_v2_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct processor_info_interface {
#define struct_processor_info_interface_t_type_p struct_processor_info_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_processor_info_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a processor_info_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct processor_info_interface *, $source);
} default_value { (const processor_info_interface_t){ .disassemble = 0 } };

/* generated */
%typemap (python, struct-out) struct processor_info_interface {
#define struct_processor_info_interface_t_type_p struct_processor_info_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_processor_info_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct processor_info_interface {
#define struct_processor_info_interface_t_type_p struct_processor_info_interface_t_type_p
	struct processor_info_interface *src = MM_MALLOC(1, struct processor_info_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_processor_info_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct processor_info_interface * {
#define struct_processor_info_interface_t_type_p struct_processor_info_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_processor_info_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct processor_info_interface * {
#define struct_processor_info_interface_t_type_p struct_processor_info_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_processor_info_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct processor_gui_interface {
#define struct_processor_gui_interface_t_type_p struct_processor_gui_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_processor_gui_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a processor_gui_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct processor_gui_interface *, $source);
} default_value { (const processor_gui_interface_t){ .dummy = 0 } };

/* generated */
%typemap (python, struct-out) struct processor_gui_interface {
#define struct_processor_gui_interface_t_type_p struct_processor_gui_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_processor_gui_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct processor_gui_interface {
#define struct_processor_gui_interface_t_type_p struct_processor_gui_interface_t_type_p
	struct processor_gui_interface *src = MM_MALLOC(1, struct processor_gui_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_processor_gui_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct processor_gui_interface * {
#define struct_processor_gui_interface_t_type_p struct_processor_gui_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_processor_gui_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct processor_gui_interface * {
#define struct_processor_gui_interface_t_type_p struct_processor_gui_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_processor_gui_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct processor_endian_interface {
#define struct_processor_endian_interface_t_type_p struct_processor_endian_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_processor_endian_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a processor_endian_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct processor_endian_interface *, $source);
} default_value { (const processor_endian_interface_t){ .get_instruction_endian = 0 } };

/* generated */
%typemap (python, struct-out) struct processor_endian_interface {
#define struct_processor_endian_interface_t_type_p struct_processor_endian_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_processor_endian_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct processor_endian_interface {
#define struct_processor_endian_interface_t_type_p struct_processor_endian_interface_t_type_p
	struct processor_endian_interface *src = MM_MALLOC(1, struct processor_endian_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_processor_endian_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct processor_endian_interface * {
#define struct_processor_endian_interface_t_type_p struct_processor_endian_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_processor_endian_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct processor_endian_interface * {
#define struct_processor_endian_interface_t_type_p struct_processor_endian_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_processor_endian_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct __anonymous106 {
#define struct_physical_block_t_type_p struct_physical_block_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_physical_block_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a physical_block_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(physical_block_t *, $source);
} default_value { (const physical_block_t){ .valid = 0 } };

/* generated */
%typemap (python, struct-out) struct __anonymous106 {
#define struct_physical_block_t_type_p struct_physical_block_t_type_p
$target = new_opaque_object(&$source, false, false, struct_physical_block_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous106 {
#define struct_physical_block_t_type_p struct_physical_block_t_type_p
	physical_block_t *src = MM_MALLOC(1, physical_block_t);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_physical_block_t_type_p, NULL);
};

/* generated */
%typemap (python, out) physical_block_t * {
#define struct_physical_block_t_type_p struct_physical_block_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_physical_block_t_type_p, NULL);
};

/* generated */
%typemap (python, in) physical_block_t * {
#define struct_physical_block_t_type_p struct_physical_block_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_physical_block_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct processor_cli_interface {
#define struct_processor_cli_interface_t_type_p struct_processor_cli_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_processor_cli_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a processor_cli_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct processor_cli_interface *, $source);
} default_value { (const processor_cli_interface_t){ .get_disassembly = 0 } };

/* generated */
%typemap (python, struct-out) struct processor_cli_interface {
#define struct_processor_cli_interface_t_type_p struct_processor_cli_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_processor_cli_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct processor_cli_interface {
#define struct_processor_cli_interface_t_type_p struct_processor_cli_interface_t_type_p
	struct processor_cli_interface *src = MM_MALLOC(1, struct processor_cli_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_processor_cli_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct processor_cli_interface * {
#define struct_processor_cli_interface_t_type_p struct_processor_cli_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_processor_cli_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct processor_cli_interface * {
#define struct_processor_cli_interface_t_type_p struct_processor_cli_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_processor_cli_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct probe_subscribe_interface {
#define struct_probe_subscribe_interface_t_type_p struct_probe_subscribe_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_probe_subscribe_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a probe_subscribe_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct probe_subscribe_interface *, $source);
} default_value { (const probe_subscribe_interface_t){ .subscribe = 0 } };

/* generated */
%typemap (python, struct-out) struct probe_subscribe_interface {
#define struct_probe_subscribe_interface_t_type_p struct_probe_subscribe_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_probe_subscribe_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct probe_subscribe_interface {
#define struct_probe_subscribe_interface_t_type_p struct_probe_subscribe_interface_t_type_p
	struct probe_subscribe_interface *src = MM_MALLOC(1, struct probe_subscribe_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_probe_subscribe_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct probe_subscribe_interface * {
#define struct_probe_subscribe_interface_t_type_p struct_probe_subscribe_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_probe_subscribe_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct probe_subscribe_interface * {
#define struct_probe_subscribe_interface_t_type_p struct_probe_subscribe_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_probe_subscribe_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct probe_sampler_cache_interface {
#define struct_probe_sampler_cache_interface_t_type_p struct_probe_sampler_cache_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_probe_sampler_cache_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a probe_sampler_cache_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct probe_sampler_cache_interface *, $source);
} default_value { (const probe_sampler_cache_interface_t){ .enable = 0 } };

/* generated */
%typemap (python, struct-out) struct probe_sampler_cache_interface {
#define struct_probe_sampler_cache_interface_t_type_p struct_probe_sampler_cache_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_probe_sampler_cache_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct probe_sampler_cache_interface {
#define struct_probe_sampler_cache_interface_t_type_p struct_probe_sampler_cache_interface_t_type_p
	struct probe_sampler_cache_interface *src = MM_MALLOC(1, struct probe_sampler_cache_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_probe_sampler_cache_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct probe_sampler_cache_interface * {
#define struct_probe_sampler_cache_interface_t_type_p struct_probe_sampler_cache_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_probe_sampler_cache_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct probe_sampler_cache_interface * {
#define struct_probe_sampler_cache_interface_t_type_p struct_probe_sampler_cache_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_probe_sampler_cache_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct probe_index_interface {
#define struct_probe_index_interface_t_type_p struct_probe_index_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_probe_index_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a probe_index_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct probe_index_interface *, $source);
} default_value { (const probe_index_interface_t){ .num_indices = 0 } };

/* generated */
%typemap (python, struct-out) struct probe_index_interface {
#define struct_probe_index_interface_t_type_p struct_probe_index_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_probe_index_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct probe_index_interface {
#define struct_probe_index_interface_t_type_p struct_probe_index_interface_t_type_p
	struct probe_index_interface *src = MM_MALLOC(1, struct probe_index_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_probe_index_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct probe_index_interface * {
#define struct_probe_index_interface_t_type_p struct_probe_index_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_probe_index_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct probe_index_interface * {
#define struct_probe_index_interface_t_type_p struct_probe_index_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_probe_index_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct probe_array_interface {
#define struct_probe_array_interface_t_type_p struct_probe_array_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_probe_array_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a probe_array_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct probe_array_interface *, $source);
} default_value { (const probe_array_interface_t){ .num_indices = 0 } };

/* generated */
%typemap (python, struct-out) struct probe_array_interface {
#define struct_probe_array_interface_t_type_p struct_probe_array_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_probe_array_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct probe_array_interface {
#define struct_probe_array_interface_t_type_p struct_probe_array_interface_t_type_p
	struct probe_array_interface *src = MM_MALLOC(1, struct probe_array_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_probe_array_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct probe_array_interface * {
#define struct_probe_array_interface_t_type_p struct_probe_array_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_probe_array_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct probe_array_interface * {
#define struct_probe_array_interface_t_type_p struct_probe_array_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_probe_array_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct probe_interface {
#define struct_probe_interface_t_type_p struct_probe_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_probe_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a probe_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct probe_interface *, $source);
} default_value { (const probe_interface_t){ .value = 0 } };

/* generated */
%typemap (python, struct-out) struct probe_interface {
#define struct_probe_interface_t_type_p struct_probe_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_probe_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct probe_interface {
#define struct_probe_interface_t_type_p struct_probe_interface_t_type_p
	struct probe_interface *src = MM_MALLOC(1, struct probe_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_probe_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct probe_interface * {
#define struct_probe_interface_t_type_p struct_probe_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_probe_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct probe_interface * {
#define struct_probe_interface_t_type_p struct_probe_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_probe_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct preference_interface {
#define struct_preference_interface_t_type_p struct_preference_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_preference_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a preference_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct preference_interface *, $source);
} default_value { (const preference_interface_t){ .get_preference_for_module_key = 0 } };

/* generated */
%typemap (python, struct-out) struct preference_interface {
#define struct_preference_interface_t_type_p struct_preference_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_preference_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct preference_interface {
#define struct_preference_interface_t_type_p struct_preference_interface_t_type_p
	struct preference_interface *src = MM_MALLOC(1, struct preference_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_preference_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct preference_interface * {
#define struct_preference_interface_t_type_p struct_preference_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_preference_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct preference_interface * {
#define struct_preference_interface_t_type_p struct_preference_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_preference_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct ppc_interface {
#define struct_ppc_interface_t_type_p struct_ppc_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_ppc_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a ppc_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct ppc_interface *, $source);
} default_value { (const ppc_interface_t){ .clear_atomic_reservation_bit = 0 } };

/* generated */
%typemap (python, struct-out) struct ppc_interface {
#define struct_ppc_interface_t_type_p struct_ppc_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_ppc_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct ppc_interface {
#define struct_ppc_interface_t_type_p struct_ppc_interface_t_type_p
	struct ppc_interface *src = MM_MALLOC(1, struct ppc_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_ppc_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct ppc_interface * {
#define struct_ppc_interface_t_type_p struct_ppc_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_ppc_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct ppc_interface * {
#define struct_ppc_interface_t_type_p struct_ppc_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_ppc_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct port_space_interface {
#define struct_port_space_interface_t_type_p struct_port_space_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_port_space_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a port_space_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct port_space_interface *, $source);
} default_value { (const port_space_interface_t){ .port_operation = 0 } };

/* generated */
%typemap (python, struct-out) struct port_space_interface {
#define struct_port_space_interface_t_type_p struct_port_space_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_port_space_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct port_space_interface {
#define struct_port_space_interface_t_type_p struct_port_space_interface_t_type_p
	struct port_space_interface *src = MM_MALLOC(1, struct port_space_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_port_space_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct port_space_interface * {
#define struct_port_space_interface_t_type_p struct_port_space_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_port_space_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct port_space_interface * {
#define struct_port_space_interface_t_type_p struct_port_space_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_port_space_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct __anonymous109 {
#define struct_ip_port_t_type_p struct_ip_port_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_ip_port_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an ip_port_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(ip_port_t *, $source);
} default_value { (const ip_port_t){ .ip_addr = 0 } };

/* generated */
%typemap (python, struct-out) struct __anonymous109 {
#define struct_ip_port_t_type_p struct_ip_port_t_type_p
$target = new_opaque_object(&$source, false, false, struct_ip_port_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous109 {
#define struct_ip_port_t_type_p struct_ip_port_t_type_p
	ip_port_t *src = MM_MALLOC(1, ip_port_t);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_ip_port_t_type_p, NULL);
};

/* generated */
%typemap (python, out) ip_port_t * {
#define struct_ip_port_t_type_p struct_ip_port_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_ip_port_t_type_p, NULL);
};

/* generated */
%typemap (python, in) ip_port_t * {
#define struct_ip_port_t_type_p struct_ip_port_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_ip_port_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct port_forward_interface {
#define struct_port_forward_interface_t_type_p struct_port_forward_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_port_forward_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a port_forward_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct port_forward_interface *, $source);
} default_value { (const port_forward_interface_t){ .port_unlisten = 0 } };

/* generated */
%typemap (python, struct-out) struct port_forward_interface {
#define struct_port_forward_interface_t_type_p struct_port_forward_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_port_forward_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct port_forward_interface {
#define struct_port_forward_interface_t_type_p struct_port_forward_interface_t_type_p
	struct port_forward_interface *src = MM_MALLOC(1, struct port_forward_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_port_forward_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct port_forward_interface * {
#define struct_port_forward_interface_t_type_p struct_port_forward_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_port_forward_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct port_forward_interface * {
#define struct_port_forward_interface_t_type_p struct_port_forward_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_port_forward_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct pool_protect_interface {
#define struct_pool_protect_interface_t_type_p struct_pool_protect_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_pool_protect_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a pool_protect_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct pool_protect_interface *, $source);
} default_value { (const pool_protect_interface_t){ .create_group = 0 } };

/* generated */
%typemap (python, struct-out) struct pool_protect_interface {
#define struct_pool_protect_interface_t_type_p struct_pool_protect_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_pool_protect_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct pool_protect_interface {
#define struct_pool_protect_interface_t_type_p struct_pool_protect_interface_t_type_p
	struct pool_protect_interface *src = MM_MALLOC(1, struct pool_protect_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_pool_protect_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct pool_protect_interface * {
#define struct_pool_protect_interface_t_type_p struct_pool_protect_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_pool_protect_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct pool_protect_interface * {
#define struct_pool_protect_interface_t_type_p struct_pool_protect_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_pool_protect_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct pmr_interface {
#define struct_pmr_interface_t_type_p struct_pmr_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_pmr_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a pmr_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct pmr_interface *, $source);
} default_value { (const pmr_interface_t){ .get = 0 } };

/* generated */
%typemap (python, struct-out) struct pmr_interface {
#define struct_pmr_interface_t_type_p struct_pmr_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_pmr_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct pmr_interface {
#define struct_pmr_interface_t_type_p struct_pmr_interface_t_type_p
	struct pmr_interface *src = MM_MALLOC(1, struct pmr_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_pmr_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct pmr_interface * {
#define struct_pmr_interface_t_type_p struct_pmr_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_pmr_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct pmr_interface * {
#define struct_pmr_interface_t_type_p struct_pmr_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_pmr_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct pcie_port_control_interface {
#define struct_pcie_port_control_interface_t_type_p struct_pcie_port_control_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_pcie_port_control_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a pcie_port_control_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct pcie_port_control_interface *, $source);
} default_value { (const pcie_port_control_interface_t){ .set_secondary_bus_number = 0 } };

/* generated */
%typemap (python, struct-out) struct pcie_port_control_interface {
#define struct_pcie_port_control_interface_t_type_p struct_pcie_port_control_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_pcie_port_control_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct pcie_port_control_interface {
#define struct_pcie_port_control_interface_t_type_p struct_pcie_port_control_interface_t_type_p
	struct pcie_port_control_interface *src = MM_MALLOC(1, struct pcie_port_control_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_pcie_port_control_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct pcie_port_control_interface * {
#define struct_pcie_port_control_interface_t_type_p struct_pcie_port_control_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_pcie_port_control_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct pcie_port_control_interface * {
#define struct_pcie_port_control_interface_t_type_p struct_pcie_port_control_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_pcie_port_control_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct pcie_map_interface {
#define struct_pcie_map_interface_t_type_p struct_pcie_map_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_pcie_map_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a pcie_map_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct pcie_map_interface *, $source);
} default_value { (const pcie_map_interface_t){ .add_map = 0 } };

/* generated */
%typemap (python, struct-out) struct pcie_map_interface {
#define struct_pcie_map_interface_t_type_p struct_pcie_map_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_pcie_map_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct pcie_map_interface {
#define struct_pcie_map_interface_t_type_p struct_pcie_map_interface_t_type_p
	struct pcie_map_interface *src = MM_MALLOC(1, struct pcie_map_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_pcie_map_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct pcie_map_interface * {
#define struct_pcie_map_interface_t_type_p struct_pcie_map_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_pcie_map_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct pcie_map_interface * {
#define struct_pcie_map_interface_t_type_p struct_pcie_map_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_pcie_map_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct pcie_link_training_interface {
#define struct_pcie_link_training_interface_t_type_p struct_pcie_link_training_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_pcie_link_training_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a pcie_link_training_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct pcie_link_training_interface *, $source);
} default_value { (const pcie_link_training_interface_t){ .trigger = 0 } };

/* generated */
%typemap (python, struct-out) struct pcie_link_training_interface {
#define struct_pcie_link_training_interface_t_type_p struct_pcie_link_training_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_pcie_link_training_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct pcie_link_training_interface {
#define struct_pcie_link_training_interface_t_type_p struct_pcie_link_training_interface_t_type_p
	struct pcie_link_training_interface *src = MM_MALLOC(1, struct pcie_link_training_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_pcie_link_training_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct pcie_link_training_interface * {
#define struct_pcie_link_training_interface_t_type_p struct_pcie_link_training_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_pcie_link_training_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct pcie_link_training_interface * {
#define struct_pcie_link_training_interface_t_type_p struct_pcie_link_training_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_pcie_link_training_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct pcie_hotplug_events_interface {
#define struct_pcie_hotplug_events_interface_t_type_p struct_pcie_hotplug_events_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_pcie_hotplug_events_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a pcie_hotplug_events_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct pcie_hotplug_events_interface *, $source);
} default_value { (const pcie_hotplug_events_interface_t){ .presence_change = 0 } };

/* generated */
%typemap (python, struct-out) struct pcie_hotplug_events_interface {
#define struct_pcie_hotplug_events_interface_t_type_p struct_pcie_hotplug_events_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_pcie_hotplug_events_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct pcie_hotplug_events_interface {
#define struct_pcie_hotplug_events_interface_t_type_p struct_pcie_hotplug_events_interface_t_type_p
	struct pcie_hotplug_events_interface *src = MM_MALLOC(1, struct pcie_hotplug_events_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_pcie_hotplug_events_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct pcie_hotplug_events_interface * {
#define struct_pcie_hotplug_events_interface_t_type_p struct_pcie_hotplug_events_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_pcie_hotplug_events_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct pcie_hotplug_events_interface * {
#define struct_pcie_hotplug_events_interface_t_type_p struct_pcie_hotplug_events_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_pcie_hotplug_events_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct pcie_device_interface {
#define struct_pcie_device_interface_t_type_p struct_pcie_device_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_pcie_device_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a pcie_device_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct pcie_device_interface *, $source);
} default_value { (const pcie_device_interface_t){ .connected = 0 } };

/* generated */
%typemap (python, struct-out) struct pcie_device_interface {
#define struct_pcie_device_interface_t_type_p struct_pcie_device_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_pcie_device_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct pcie_device_interface {
#define struct_pcie_device_interface_t_type_p struct_pcie_device_interface_t_type_p
	struct pcie_device_interface *src = MM_MALLOC(1, struct pcie_device_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_pcie_device_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct pcie_device_interface * {
#define struct_pcie_device_interface_t_type_p struct_pcie_device_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_pcie_device_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct pcie_device_interface * {
#define struct_pcie_device_interface_t_type_p struct_pcie_device_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_pcie_device_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct pcie_adapter_compat_interface {
#define struct_pcie_adapter_compat_interface_t_type_p struct_pcie_adapter_compat_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_pcie_adapter_compat_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a pcie_adapter_compat_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct pcie_adapter_compat_interface *, $source);
} default_value { (const pcie_adapter_compat_interface_t){ .set_secondary_bus_number = 0 } };

/* generated */
%typemap (python, struct-out) struct pcie_adapter_compat_interface {
#define struct_pcie_adapter_compat_interface_t_type_p struct_pcie_adapter_compat_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_pcie_adapter_compat_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct pcie_adapter_compat_interface {
#define struct_pcie_adapter_compat_interface_t_type_p struct_pcie_adapter_compat_interface_t_type_p
	struct pcie_adapter_compat_interface *src = MM_MALLOC(1, struct pcie_adapter_compat_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_pcie_adapter_compat_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct pcie_adapter_compat_interface * {
#define struct_pcie_adapter_compat_interface_t_type_p struct_pcie_adapter_compat_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_pcie_adapter_compat_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct pcie_adapter_compat_interface * {
#define struct_pcie_adapter_compat_interface_t_type_p struct_pcie_adapter_compat_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_pcie_adapter_compat_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct pci_upstream_operation_interface {
#define struct_pci_upstream_operation_interface_t_type_p struct_pci_upstream_operation_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_pci_upstream_operation_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a pci_upstream_operation_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct pci_upstream_operation_interface *, $source);
} default_value { (const pci_upstream_operation_interface_t){ .read = 0 } };

/* generated */
%typemap (python, struct-out) struct pci_upstream_operation_interface {
#define struct_pci_upstream_operation_interface_t_type_p struct_pci_upstream_operation_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_pci_upstream_operation_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct pci_upstream_operation_interface {
#define struct_pci_upstream_operation_interface_t_type_p struct_pci_upstream_operation_interface_t_type_p
	struct pci_upstream_operation_interface *src = MM_MALLOC(1, struct pci_upstream_operation_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_pci_upstream_operation_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct pci_upstream_operation_interface * {
#define struct_pci_upstream_operation_interface_t_type_p struct_pci_upstream_operation_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_pci_upstream_operation_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct pci_upstream_operation_interface * {
#define struct_pci_upstream_operation_interface_t_type_p struct_pci_upstream_operation_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_pci_upstream_operation_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct pci_upstream_interface {
#define struct_pci_upstream_interface_t_type_p struct_pci_upstream_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_pci_upstream_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a pci_upstream_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct pci_upstream_interface *, $source);
} default_value { (const pci_upstream_interface_t){ .operation = 0 } };

/* generated */
%typemap (python, struct-out) struct pci_upstream_interface {
#define struct_pci_upstream_interface_t_type_p struct_pci_upstream_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_pci_upstream_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct pci_upstream_interface {
#define struct_pci_upstream_interface_t_type_p struct_pci_upstream_interface_t_type_p
	struct pci_upstream_interface *src = MM_MALLOC(1, struct pci_upstream_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_pci_upstream_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct pci_upstream_interface * {
#define struct_pci_upstream_interface_t_type_p struct_pci_upstream_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_pci_upstream_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct pci_upstream_interface * {
#define struct_pci_upstream_interface_t_type_p struct_pci_upstream_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_pci_upstream_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct pci_multi_function_device_interface {
#define struct_pci_multi_function_device_interface_t_type_p struct_pci_multi_function_device_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_pci_multi_function_device_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a pci_multi_function_device_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct pci_multi_function_device_interface *, $source);
} default_value { (const pci_multi_function_device_interface_t){ .supported_functions = 0 } };

/* generated */
%typemap (python, struct-out) struct pci_multi_function_device_interface {
#define struct_pci_multi_function_device_interface_t_type_p struct_pci_multi_function_device_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_pci_multi_function_device_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct pci_multi_function_device_interface {
#define struct_pci_multi_function_device_interface_t_type_p struct_pci_multi_function_device_interface_t_type_p
	struct pci_multi_function_device_interface *src = MM_MALLOC(1, struct pci_multi_function_device_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_pci_multi_function_device_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct pci_multi_function_device_interface * {
#define struct_pci_multi_function_device_interface_t_type_p struct_pci_multi_function_device_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_pci_multi_function_device_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct pci_multi_function_device_interface * {
#define struct_pci_multi_function_device_interface_t_type_p struct_pci_multi_function_device_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_pci_multi_function_device_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct pci_interrupt_interface {
#define struct_pci_interrupt_interface_t_type_p struct_pci_interrupt_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_pci_interrupt_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a pci_interrupt_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct pci_interrupt_interface *, $source);
} default_value { (const pci_interrupt_interface_t){ .raise_interrupt = 0 } };

/* generated */
%typemap (python, struct-out) struct pci_interrupt_interface {
#define struct_pci_interrupt_interface_t_type_p struct_pci_interrupt_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_pci_interrupt_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct pci_interrupt_interface {
#define struct_pci_interrupt_interface_t_type_p struct_pci_interrupt_interface_t_type_p
	struct pci_interrupt_interface *src = MM_MALLOC(1, struct pci_interrupt_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_pci_interrupt_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct pci_interrupt_interface * {
#define struct_pci_interrupt_interface_t_type_p struct_pci_interrupt_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_pci_interrupt_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct pci_interrupt_interface * {
#define struct_pci_interrupt_interface_t_type_p struct_pci_interrupt_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_pci_interrupt_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct pci_express_hotplug_interface {
#define struct_pci_express_hotplug_interface_t_type_p struct_pci_express_hotplug_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_pci_express_hotplug_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a pci_express_hotplug_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct pci_express_hotplug_interface *, $source);
} default_value { (const pci_express_hotplug_interface_t){ .presence_change = 0 } };

/* generated */
%typemap (python, struct-out) struct pci_express_hotplug_interface {
#define struct_pci_express_hotplug_interface_t_type_p struct_pci_express_hotplug_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_pci_express_hotplug_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct pci_express_hotplug_interface {
#define struct_pci_express_hotplug_interface_t_type_p struct_pci_express_hotplug_interface_t_type_p
	struct pci_express_hotplug_interface *src = MM_MALLOC(1, struct pci_express_hotplug_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_pci_express_hotplug_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct pci_express_hotplug_interface * {
#define struct_pci_express_hotplug_interface_t_type_p struct_pci_express_hotplug_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_pci_express_hotplug_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct pci_express_hotplug_interface * {
#define struct_pci_express_hotplug_interface_t_type_p struct_pci_express_hotplug_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_pci_express_hotplug_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct pci_express_interface {
#define struct_pci_express_interface_t_type_p struct_pci_express_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_pci_express_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a pci_express_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct pci_express_interface *, $source);
} default_value { (const pci_express_interface_t){ .send_message = 0 } };

/* generated */
%typemap (python, struct-out) struct pci_express_interface {
#define struct_pci_express_interface_t_type_p struct_pci_express_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_pci_express_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct pci_express_interface {
#define struct_pci_express_interface_t_type_p struct_pci_express_interface_t_type_p
	struct pci_express_interface *src = MM_MALLOC(1, struct pci_express_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_pci_express_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct pci_express_interface * {
#define struct_pci_express_interface_t_type_p struct_pci_express_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_pci_express_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct pci_express_interface * {
#define struct_pci_express_interface_t_type_p struct_pci_express_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_pci_express_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct pci_downstream_interface {
#define struct_pci_downstream_interface_t_type_p struct_pci_downstream_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_pci_downstream_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a pci_downstream_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct pci_downstream_interface *, $source);
} default_value { (const pci_downstream_interface_t){ .operation = 0 } };

/* generated */
%typemap (python, struct-out) struct pci_downstream_interface {
#define struct_pci_downstream_interface_t_type_p struct_pci_downstream_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_pci_downstream_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct pci_downstream_interface {
#define struct_pci_downstream_interface_t_type_p struct_pci_downstream_interface_t_type_p
	struct pci_downstream_interface *src = MM_MALLOC(1, struct pci_downstream_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_pci_downstream_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct pci_downstream_interface * {
#define struct_pci_downstream_interface_t_type_p struct_pci_downstream_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_pci_downstream_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct pci_downstream_interface * {
#define struct_pci_downstream_interface_t_type_p struct_pci_downstream_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_pci_downstream_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct pci_device_interface {
#define struct_pci_device_interface_t_type_p struct_pci_device_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_pci_device_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a pci_device_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct pci_device_interface *, $source);
} default_value { (const pci_device_interface_t){ .bus_reset = 0 } };

/* generated */
%typemap (python, struct-out) struct pci_device_interface {
#define struct_pci_device_interface_t_type_p struct_pci_device_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_pci_device_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct pci_device_interface {
#define struct_pci_device_interface_t_type_p struct_pci_device_interface_t_type_p
	struct pci_device_interface *src = MM_MALLOC(1, struct pci_device_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_pci_device_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct pci_device_interface * {
#define struct_pci_device_interface_t_type_p struct_pci_device_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_pci_device_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct pci_device_interface * {
#define struct_pci_device_interface_t_type_p struct_pci_device_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_pci_device_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct pci_bus_interface {
#define struct_pci_bus_interface_t_type_p struct_pci_bus_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_pci_bus_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a pci_bus_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct pci_bus_interface *, $source);
} default_value { (const pci_bus_interface_t){ .memory_access = 0 } };

/* generated */
%typemap (python, struct-out) struct pci_bus_interface {
#define struct_pci_bus_interface_t_type_p struct_pci_bus_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_pci_bus_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct pci_bus_interface {
#define struct_pci_bus_interface_t_type_p struct_pci_bus_interface_t_type_p
	struct pci_bus_interface *src = MM_MALLOC(1, struct pci_bus_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_pci_bus_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct pci_bus_interface * {
#define struct_pci_bus_interface_t_type_p struct_pci_bus_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_pci_bus_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct pci_bus_interface * {
#define struct_pci_bus_interface_t_type_p struct_pci_bus_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_pci_bus_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct pci_bridge_interface {
#define struct_pci_bridge_interface_t_type_p struct_pci_bridge_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_pci_bridge_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a pci_bridge_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct pci_bridge_interface *, $source);
} default_value { (const pci_bridge_interface_t){ .system_error = 0 } };

/* generated */
%typemap (python, struct-out) struct pci_bridge_interface {
#define struct_pci_bridge_interface_t_type_p struct_pci_bridge_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_pci_bridge_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct pci_bridge_interface {
#define struct_pci_bridge_interface_t_type_p struct_pci_bridge_interface_t_type_p
	struct pci_bridge_interface *src = MM_MALLOC(1, struct pci_bridge_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_pci_bridge_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct pci_bridge_interface * {
#define struct_pci_bridge_interface_t_type_p struct_pci_bridge_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_pci_bridge_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct pci_bridge_interface * {
#define struct_pci_bridge_interface_t_type_p struct_pci_bridge_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_pci_bridge_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct page_bank_client_interface {
#define struct_page_bank_client_interface_t_type_p struct_page_bank_client_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_page_bank_client_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a page_bank_client_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct page_bank_client_interface *, $source);
} default_value { (const page_bank_client_interface_t){ .init_page = 0 } };

/* generated */
%typemap (python, struct-out) struct page_bank_client_interface {
#define struct_page_bank_client_interface_t_type_p struct_page_bank_client_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_page_bank_client_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct page_bank_client_interface {
#define struct_page_bank_client_interface_t_type_p struct_page_bank_client_interface_t_type_p
	struct page_bank_client_interface *src = MM_MALLOC(1, struct page_bank_client_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_page_bank_client_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct page_bank_client_interface * {
#define struct_page_bank_client_interface_t_type_p struct_page_bank_client_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_page_bank_client_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct page_bank_client_interface * {
#define struct_page_bank_client_interface_t_type_p struct_page_bank_client_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_page_bank_client_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct __anonymous114 {
#define struct_page_bank_grant_t_type_p struct_page_bank_grant_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_page_bank_grant_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a page_bank_grant_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(page_bank_grant_t *, $source);
} default_value { (const page_bank_grant_t){ .pbb = 0 } };

/* generated */
%typemap (python, struct-out) struct __anonymous114 {
#define struct_page_bank_grant_t_type_p struct_page_bank_grant_t_type_p
$target = new_opaque_object(&$source, false, false, struct_page_bank_grant_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous114 {
#define struct_page_bank_grant_t_type_p struct_page_bank_grant_t_type_p
	page_bank_grant_t *src = MM_MALLOC(1, page_bank_grant_t);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_page_bank_grant_t_type_p, NULL);
};

/* generated */
%typemap (python, out) page_bank_grant_t * {
#define struct_page_bank_grant_t_type_p struct_page_bank_grant_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_page_bank_grant_t_type_p, NULL);
};

/* generated */
%typemap (python, in) page_bank_grant_t * {
#define struct_page_bank_grant_t_type_p struct_page_bank_grant_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_page_bank_grant_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct pb_page {
#define struct_pb_page_t_type_p struct_pb_page_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_pb_page_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a pb_page_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct pb_page *NOTNULL, $source);
};

/* generated */
%typemap (python, struct-out) struct pb_page {
#define struct_pb_page_t_type_p struct_pb_page_t_type_p
$target = new_opaque_object(&$source, false, false, struct_pb_page_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct pb_page {
#define struct_pb_page_t_type_p struct_pb_page_t_type_p
	struct pb_page *NOTNULL src = MM_MALLOC(1, struct pb_page);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_pb_page_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct pb_page *NOTNULL {
#define struct_pb_page_t_type_p struct_pb_page_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_pb_page_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct pb_page *NOTNULL {
#define struct_pb_page_t_type_p struct_pb_page_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_pb_page_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct page_bank_interface {
#define struct_page_bank_interface_t_type_p struct_page_bank_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_page_bank_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a page_bank_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct page_bank_interface *, $source);
} default_value { (const page_bank_interface_t){ .attach = 0 } };

/* generated */
%typemap (python, struct-out) struct page_bank_interface {
#define struct_page_bank_interface_t_type_p struct_page_bank_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_page_bank_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct page_bank_interface {
#define struct_page_bank_interface_t_type_p struct_page_bank_interface_t_type_p
	struct page_bank_interface *src = MM_MALLOC(1, struct page_bank_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_page_bank_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct page_bank_interface * {
#define struct_page_bank_interface_t_type_p struct_page_bank_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_page_bank_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct page_bank_interface * {
#define struct_page_bank_interface_t_type_p struct_page_bank_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_page_bank_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct packet_interface {
#define struct_packet_interface_t_type_p struct_packet_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_packet_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a packet_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct packet_interface *, $source);
} default_value { (const packet_interface_t){ .transfer = 0 } };

/* generated */
%typemap (python, struct-out) struct packet_interface {
#define struct_packet_interface_t_type_p struct_packet_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_packet_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct packet_interface {
#define struct_packet_interface_t_type_p struct_packet_interface_t_type_p
	struct packet_interface *src = MM_MALLOC(1, struct packet_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_packet_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct packet_interface * {
#define struct_packet_interface_t_type_p struct_packet_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_packet_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct packet_interface * {
#define struct_packet_interface_t_type_p struct_packet_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_packet_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct osa_tracker_state_query_interface {
#define struct_osa_tracker_state_query_interface_t_type_p struct_osa_tracker_state_query_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_osa_tracker_state_query_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an osa_tracker_state_query_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct osa_tracker_state_query_interface *, $source);
} default_value { (const osa_tracker_state_query_interface_t){ ._deprecated = 0 } };

/* generated */
%typemap (python, struct-out) struct osa_tracker_state_query_interface {
#define struct_osa_tracker_state_query_interface_t_type_p struct_osa_tracker_state_query_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_osa_tracker_state_query_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct osa_tracker_state_query_interface {
#define struct_osa_tracker_state_query_interface_t_type_p struct_osa_tracker_state_query_interface_t_type_p
	struct osa_tracker_state_query_interface *src = MM_MALLOC(1, struct osa_tracker_state_query_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_osa_tracker_state_query_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct osa_tracker_state_query_interface * {
#define struct_osa_tracker_state_query_interface_t_type_p struct_osa_tracker_state_query_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_osa_tracker_state_query_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct osa_tracker_state_query_interface * {
#define struct_osa_tracker_state_query_interface_t_type_p struct_osa_tracker_state_query_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_osa_tracker_state_query_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct osa_tracker_state_notification_interface {
#define struct_osa_tracker_state_notification_interface_t_type_p struct_osa_tracker_state_notification_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_osa_tracker_state_notification_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an osa_tracker_state_notification_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct osa_tracker_state_notification_interface *, $source);
} default_value { (const osa_tracker_state_notification_interface_t){ .subscribe_tracker = 0 } };

/* generated */
%typemap (python, struct-out) struct osa_tracker_state_notification_interface {
#define struct_osa_tracker_state_notification_interface_t_type_p struct_osa_tracker_state_notification_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_osa_tracker_state_notification_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct osa_tracker_state_notification_interface {
#define struct_osa_tracker_state_notification_interface_t_type_p struct_osa_tracker_state_notification_interface_t_type_p
	struct osa_tracker_state_notification_interface *src = MM_MALLOC(1, struct osa_tracker_state_notification_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_osa_tracker_state_notification_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct osa_tracker_state_notification_interface * {
#define struct_osa_tracker_state_notification_interface_t_type_p struct_osa_tracker_state_notification_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_osa_tracker_state_notification_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct osa_tracker_state_notification_interface * {
#define struct_osa_tracker_state_notification_interface_t_type_p struct_osa_tracker_state_notification_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_osa_tracker_state_notification_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct osa_tracker_state_admin_interface {
#define struct_osa_tracker_state_admin_interface_t_type_p struct_osa_tracker_state_admin_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_osa_tracker_state_admin_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an osa_tracker_state_admin_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct osa_tracker_state_admin_interface *, $source);
} default_value { (const osa_tracker_state_admin_interface_t){ .begin = 0 } };

/* generated */
%typemap (python, struct-out) struct osa_tracker_state_admin_interface {
#define struct_osa_tracker_state_admin_interface_t_type_p struct_osa_tracker_state_admin_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_osa_tracker_state_admin_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct osa_tracker_state_admin_interface {
#define struct_osa_tracker_state_admin_interface_t_type_p struct_osa_tracker_state_admin_interface_t_type_p
	struct osa_tracker_state_admin_interface *src = MM_MALLOC(1, struct osa_tracker_state_admin_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_osa_tracker_state_admin_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct osa_tracker_state_admin_interface * {
#define struct_osa_tracker_state_admin_interface_t_type_p struct_osa_tracker_state_admin_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_osa_tracker_state_admin_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct osa_tracker_state_admin_interface * {
#define struct_osa_tracker_state_admin_interface_t_type_p struct_osa_tracker_state_admin_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_osa_tracker_state_admin_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct osa_tracker_control_interface {
#define struct_osa_tracker_control_interface_t_type_p struct_osa_tracker_control_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_osa_tracker_control_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an osa_tracker_control_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct osa_tracker_control_interface *, $source);
} default_value { (const osa_tracker_control_interface_t){ .disable = 0 } };

/* generated */
%typemap (python, struct-out) struct osa_tracker_control_interface {
#define struct_osa_tracker_control_interface_t_type_p struct_osa_tracker_control_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_osa_tracker_control_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct osa_tracker_control_interface {
#define struct_osa_tracker_control_interface_t_type_p struct_osa_tracker_control_interface_t_type_p
	struct osa_tracker_control_interface *src = MM_MALLOC(1, struct osa_tracker_control_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_osa_tracker_control_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct osa_tracker_control_interface * {
#define struct_osa_tracker_control_interface_t_type_p struct_osa_tracker_control_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_osa_tracker_control_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct osa_tracker_control_interface * {
#define struct_osa_tracker_control_interface_t_type_p struct_osa_tracker_control_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_osa_tracker_control_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct osa_tracker_component_interface {
#define struct_osa_tracker_component_interface_t_type_p struct_osa_tracker_component_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_osa_tracker_component_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an osa_tracker_component_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct osa_tracker_component_interface *, $source);
} default_value { (const osa_tracker_component_interface_t){ .get_tracker = 0 } };

/* generated */
%typemap (python, struct-out) struct osa_tracker_component_interface {
#define struct_osa_tracker_component_interface_t_type_p struct_osa_tracker_component_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_osa_tracker_component_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct osa_tracker_component_interface {
#define struct_osa_tracker_component_interface_t_type_p struct_osa_tracker_component_interface_t_type_p
	struct osa_tracker_component_interface *src = MM_MALLOC(1, struct osa_tracker_component_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_osa_tracker_component_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct osa_tracker_component_interface * {
#define struct_osa_tracker_component_interface_t_type_p struct_osa_tracker_component_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_osa_tracker_component_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct osa_tracker_component_interface * {
#define struct_osa_tracker_component_interface_t_type_p struct_osa_tracker_component_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_osa_tracker_component_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct osa_parameters_interface {
#define struct_osa_parameters_interface_t_type_p struct_osa_parameters_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_osa_parameters_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an osa_parameters_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct osa_parameters_interface *, $source);
} default_value { (const osa_parameters_interface_t){ .get_parameters = 0 } };

/* generated */
%typemap (python, struct-out) struct osa_parameters_interface {
#define struct_osa_parameters_interface_t_type_p struct_osa_parameters_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_osa_parameters_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct osa_parameters_interface {
#define struct_osa_parameters_interface_t_type_p struct_osa_parameters_interface_t_type_p
	struct osa_parameters_interface *src = MM_MALLOC(1, struct osa_parameters_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_osa_parameters_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct osa_parameters_interface * {
#define struct_osa_parameters_interface_t_type_p struct_osa_parameters_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_osa_parameters_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct osa_parameters_interface * {
#define struct_osa_parameters_interface_t_type_p struct_osa_parameters_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_osa_parameters_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct osa_node_tree_query_interface {
#define struct_osa_node_tree_query_interface_t_type_p struct_osa_node_tree_query_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_osa_node_tree_query_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an osa_node_tree_query_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct osa_node_tree_query_interface *, $source);
} default_value { (const osa_node_tree_query_interface_t){ .get_root_nodes = 0 } };

/* generated */
%typemap (python, struct-out) struct osa_node_tree_query_interface {
#define struct_osa_node_tree_query_interface_t_type_p struct_osa_node_tree_query_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_osa_node_tree_query_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct osa_node_tree_query_interface {
#define struct_osa_node_tree_query_interface_t_type_p struct_osa_node_tree_query_interface_t_type_p
	struct osa_node_tree_query_interface *src = MM_MALLOC(1, struct osa_node_tree_query_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_osa_node_tree_query_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct osa_node_tree_query_interface * {
#define struct_osa_node_tree_query_interface_t_type_p struct_osa_node_tree_query_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_osa_node_tree_query_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct osa_node_tree_query_interface * {
#define struct_osa_node_tree_query_interface_t_type_p struct_osa_node_tree_query_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_osa_node_tree_query_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct osa_node_tree_notification_interface {
#define struct_osa_node_tree_notification_interface_t_type_p struct_osa_node_tree_notification_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_osa_node_tree_notification_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an osa_node_tree_notification_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct osa_node_tree_notification_interface *, $source);
} default_value { (const osa_node_tree_notification_interface_t){ .notify_create = 0 } };

/* generated */
%typemap (python, struct-out) struct osa_node_tree_notification_interface {
#define struct_osa_node_tree_notification_interface_t_type_p struct_osa_node_tree_notification_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_osa_node_tree_notification_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct osa_node_tree_notification_interface {
#define struct_osa_node_tree_notification_interface_t_type_p struct_osa_node_tree_notification_interface_t_type_p
	struct osa_node_tree_notification_interface *src = MM_MALLOC(1, struct osa_node_tree_notification_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_osa_node_tree_notification_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct osa_node_tree_notification_interface * {
#define struct_osa_node_tree_notification_interface_t_type_p struct_osa_node_tree_notification_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_osa_node_tree_notification_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct osa_node_tree_notification_interface * {
#define struct_osa_node_tree_notification_interface_t_type_p struct_osa_node_tree_notification_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_osa_node_tree_notification_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct osa_node_tree_admin_interface {
#define struct_osa_node_tree_admin_interface_t_type_p struct_osa_node_tree_admin_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_osa_node_tree_admin_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an osa_node_tree_admin_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct osa_node_tree_admin_interface *, $source);
} default_value { (const osa_node_tree_admin_interface_t){ .begin = 0 } };

/* generated */
%typemap (python, struct-out) struct osa_node_tree_admin_interface {
#define struct_osa_node_tree_admin_interface_t_type_p struct_osa_node_tree_admin_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_osa_node_tree_admin_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct osa_node_tree_admin_interface {
#define struct_osa_node_tree_admin_interface_t_type_p struct_osa_node_tree_admin_interface_t_type_p
	struct osa_node_tree_admin_interface *src = MM_MALLOC(1, struct osa_node_tree_admin_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_osa_node_tree_admin_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct osa_node_tree_admin_interface * {
#define struct_osa_node_tree_admin_interface_t_type_p struct_osa_node_tree_admin_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_osa_node_tree_admin_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct osa_node_tree_admin_interface * {
#define struct_osa_node_tree_admin_interface_t_type_p struct_osa_node_tree_admin_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_osa_node_tree_admin_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct osa_node_path_interface {
#define struct_osa_node_path_interface_t_type_p struct_osa_node_path_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_osa_node_path_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an osa_node_path_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct osa_node_path_interface *, $source);
} default_value { (const osa_node_path_interface_t){ .matching_nodes = 0 } };

/* generated */
%typemap (python, struct-out) struct osa_node_path_interface {
#define struct_osa_node_path_interface_t_type_p struct_osa_node_path_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_osa_node_path_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct osa_node_path_interface {
#define struct_osa_node_path_interface_t_type_p struct_osa_node_path_interface_t_type_p
	struct osa_node_path_interface *src = MM_MALLOC(1, struct osa_node_path_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_osa_node_path_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct osa_node_path_interface * {
#define struct_osa_node_path_interface_t_type_p struct_osa_node_path_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_osa_node_path_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct osa_node_path_interface * {
#define struct_osa_node_path_interface_t_type_p struct_osa_node_path_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_osa_node_path_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct osa_micro_checkpoint_interface {
#define struct_osa_micro_checkpoint_interface_t_type_p struct_osa_micro_checkpoint_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_osa_micro_checkpoint_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an osa_micro_checkpoint_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct osa_micro_checkpoint_interface *, $source);
} default_value { (const osa_micro_checkpoint_interface_t){ .started = 0 } };

/* generated */
%typemap (python, struct-out) struct osa_micro_checkpoint_interface {
#define struct_osa_micro_checkpoint_interface_t_type_p struct_osa_micro_checkpoint_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_osa_micro_checkpoint_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct osa_micro_checkpoint_interface {
#define struct_osa_micro_checkpoint_interface_t_type_p struct_osa_micro_checkpoint_interface_t_type_p
	struct osa_micro_checkpoint_interface *src = MM_MALLOC(1, struct osa_micro_checkpoint_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_osa_micro_checkpoint_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct osa_micro_checkpoint_interface * {
#define struct_osa_micro_checkpoint_interface_t_type_p struct_osa_micro_checkpoint_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_osa_micro_checkpoint_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct osa_micro_checkpoint_interface * {
#define struct_osa_micro_checkpoint_interface_t_type_p struct_osa_micro_checkpoint_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_osa_micro_checkpoint_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct osa_mapper_query_interface {
#define struct_osa_mapper_query_interface_t_type_p struct_osa_mapper_query_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_osa_mapper_query_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an osa_mapper_query_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct osa_mapper_query_interface *, $source);
} default_value { (const osa_mapper_query_interface_t){ .get_process_list = 0 } };

/* generated */
%typemap (python, struct-out) struct osa_mapper_query_interface {
#define struct_osa_mapper_query_interface_t_type_p struct_osa_mapper_query_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_osa_mapper_query_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct osa_mapper_query_interface {
#define struct_osa_mapper_query_interface_t_type_p struct_osa_mapper_query_interface_t_type_p
	struct osa_mapper_query_interface *src = MM_MALLOC(1, struct osa_mapper_query_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_osa_mapper_query_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct osa_mapper_query_interface * {
#define struct_osa_mapper_query_interface_t_type_p struct_osa_mapper_query_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_osa_mapper_query_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct osa_mapper_query_interface * {
#define struct_osa_mapper_query_interface_t_type_p struct_osa_mapper_query_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_osa_mapper_query_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct osa_mapper_control_interface {
#define struct_osa_mapper_control_interface_t_type_p struct_osa_mapper_control_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_osa_mapper_control_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an osa_mapper_control_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct osa_mapper_control_interface *, $source);
} default_value { (const osa_mapper_control_interface_t){ .disable = 0 } };

/* generated */
%typemap (python, struct-out) struct osa_mapper_control_interface {
#define struct_osa_mapper_control_interface_t_type_p struct_osa_mapper_control_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_osa_mapper_control_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct osa_mapper_control_interface {
#define struct_osa_mapper_control_interface_t_type_p struct_osa_mapper_control_interface_t_type_p
	struct osa_mapper_control_interface *src = MM_MALLOC(1, struct osa_mapper_control_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_osa_mapper_control_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct osa_mapper_control_interface * {
#define struct_osa_mapper_control_interface_t_type_p struct_osa_mapper_control_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_osa_mapper_control_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct osa_mapper_control_interface * {
#define struct_osa_mapper_control_interface_t_type_p struct_osa_mapper_control_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_osa_mapper_control_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct osa_mapper_admin_interface {
#define struct_osa_mapper_admin_interface_t_type_p struct_osa_mapper_admin_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_osa_mapper_admin_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an osa_mapper_admin_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct osa_mapper_admin_interface *, $source);
} default_value { (const osa_mapper_admin_interface_t){ .tracker_updated = 0 } };

/* generated */
%typemap (python, struct-out) struct osa_mapper_admin_interface {
#define struct_osa_mapper_admin_interface_t_type_p struct_osa_mapper_admin_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_osa_mapper_admin_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct osa_mapper_admin_interface {
#define struct_osa_mapper_admin_interface_t_type_p struct_osa_mapper_admin_interface_t_type_p
	struct osa_mapper_admin_interface *src = MM_MALLOC(1, struct osa_mapper_admin_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_osa_mapper_admin_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct osa_mapper_admin_interface * {
#define struct_osa_mapper_admin_interface_t_type_p struct_osa_mapper_admin_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_osa_mapper_admin_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct osa_mapper_admin_interface * {
#define struct_osa_mapper_admin_interface_t_type_p struct_osa_mapper_admin_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_osa_mapper_admin_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct osa_machine_query_interface {
#define struct_osa_machine_query_interface_t_type_p struct_osa_machine_query_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_osa_machine_query_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an osa_machine_query_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct osa_machine_query_interface *, $source);
} default_value { (const osa_machine_query_interface_t){ .read_register = 0 } };

/* generated */
%typemap (python, struct-out) struct osa_machine_query_interface {
#define struct_osa_machine_query_interface_t_type_p struct_osa_machine_query_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_osa_machine_query_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct osa_machine_query_interface {
#define struct_osa_machine_query_interface_t_type_p struct_osa_machine_query_interface_t_type_p
	struct osa_machine_query_interface *src = MM_MALLOC(1, struct osa_machine_query_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_osa_machine_query_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct osa_machine_query_interface * {
#define struct_osa_machine_query_interface_t_type_p struct_osa_machine_query_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_osa_machine_query_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct osa_machine_query_interface * {
#define struct_osa_machine_query_interface_t_type_p struct_osa_machine_query_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_osa_machine_query_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct osa_machine_notification_interface {
#define struct_osa_machine_notification_interface_t_type_p struct_osa_machine_notification_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_osa_machine_notification_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an osa_machine_notification_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct osa_machine_notification_interface *, $source);
} default_value { (const osa_machine_notification_interface_t){ .notify_mode_change = 0 } };

/* generated */
%typemap (python, struct-out) struct osa_machine_notification_interface {
#define struct_osa_machine_notification_interface_t_type_p struct_osa_machine_notification_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_osa_machine_notification_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct osa_machine_notification_interface {
#define struct_osa_machine_notification_interface_t_type_p struct_osa_machine_notification_interface_t_type_p
	struct osa_machine_notification_interface *src = MM_MALLOC(1, struct osa_machine_notification_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_osa_machine_notification_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct osa_machine_notification_interface * {
#define struct_osa_machine_notification_interface_t_type_p struct_osa_machine_notification_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_osa_machine_notification_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct osa_machine_notification_interface * {
#define struct_osa_machine_notification_interface_t_type_p struct_osa_machine_notification_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_osa_machine_notification_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct osa_control_v2_interface {
#define struct_osa_control_v2_interface_t_type_p struct_osa_control_v2_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_osa_control_v2_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an osa_control_v2_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct osa_control_v2_interface *, $source);
} default_value { (const osa_control_v2_interface_t){ .request = 0 } };

/* generated */
%typemap (python, struct-out) struct osa_control_v2_interface {
#define struct_osa_control_v2_interface_t_type_p struct_osa_control_v2_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_osa_control_v2_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct osa_control_v2_interface {
#define struct_osa_control_v2_interface_t_type_p struct_osa_control_v2_interface_t_type_p
	struct osa_control_v2_interface *src = MM_MALLOC(1, struct osa_control_v2_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_osa_control_v2_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct osa_control_v2_interface * {
#define struct_osa_control_v2_interface_t_type_p struct_osa_control_v2_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_osa_control_v2_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct osa_control_v2_interface * {
#define struct_osa_control_v2_interface_t_type_p struct_osa_control_v2_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_osa_control_v2_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct osa_control_interface {
#define struct_osa_control_interface_t_type_p struct_osa_control_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_osa_control_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an osa_control_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct osa_control_interface *, $source);
} default_value { (const osa_control_interface_t){ .request = 0 } };

/* generated */
%typemap (python, struct-out) struct osa_control_interface {
#define struct_osa_control_interface_t_type_p struct_osa_control_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_osa_control_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct osa_control_interface {
#define struct_osa_control_interface_t_type_p struct_osa_control_interface_t_type_p
	struct osa_control_interface *src = MM_MALLOC(1, struct osa_control_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_osa_control_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct osa_control_interface * {
#define struct_osa_control_interface_t_type_p struct_osa_control_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_osa_control_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct osa_control_interface * {
#define struct_osa_control_interface_t_type_p struct_osa_control_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_osa_control_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct maybe_node_id {
#define struct_maybe_node_id_t_type_p struct_maybe_node_id_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_maybe_node_id_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a maybe_node_id_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(maybe_node_id_t *, $source);
} default_value { (const maybe_node_id_t){ .valid = 0 } };

/* generated */
%typemap (python, struct-out) struct maybe_node_id {
#define struct_maybe_node_id_t_type_p struct_maybe_node_id_t_type_p
$target = new_opaque_object(&$source, false, false, struct_maybe_node_id_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct maybe_node_id {
#define struct_maybe_node_id_t_type_p struct_maybe_node_id_t_type_p
	maybe_node_id_t *src = MM_MALLOC(1, struct maybe_node_id);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_maybe_node_id_t_type_p, NULL);
};

/* generated */
%typemap (python, out) maybe_node_id_t * {
#define struct_maybe_node_id_t_type_p struct_maybe_node_id_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_maybe_node_id_t_type_p, NULL);
};

/* generated */
%typemap (python, in) maybe_node_id_t * {
#define struct_maybe_node_id_t_type_p struct_maybe_node_id_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_maybe_node_id_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct osa_component_interface {
#define struct_osa_component_interface_t_type_p struct_osa_component_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_osa_component_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an osa_component_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct osa_component_interface *, $source);
} default_value { (const osa_component_interface_t){ .get_admin = 0 } };

/* generated */
%typemap (python, struct-out) struct osa_component_interface {
#define struct_osa_component_interface_t_type_p struct_osa_component_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_osa_component_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct osa_component_interface {
#define struct_osa_component_interface_t_type_p struct_osa_component_interface_t_type_p
	struct osa_component_interface *src = MM_MALLOC(1, struct osa_component_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_osa_component_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct osa_component_interface * {
#define struct_osa_component_interface_t_type_p struct_osa_component_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_osa_component_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct osa_component_interface * {
#define struct_osa_component_interface_t_type_p struct_osa_component_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_osa_component_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct __anonymous118 {
#define struct_opcode_length_info_t_type_p struct_opcode_length_info_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_opcode_length_info_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an opcode_length_info_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(opcode_length_info_t *, $source);
} default_value { (const opcode_length_info_t){ .min_alignment = 0 } };

/* generated */
%typemap (python, struct-out) struct __anonymous118 {
#define struct_opcode_length_info_t_type_p struct_opcode_length_info_t_type_p
$target = new_opaque_object(&$source, false, false, struct_opcode_length_info_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous118 {
#define struct_opcode_length_info_t_type_p struct_opcode_length_info_t_type_p
	opcode_length_info_t *src = MM_MALLOC(1, opcode_length_info_t);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_opcode_length_info_t_type_p, NULL);
};

/* generated */
%typemap (python, out) opcode_length_info_t * {
#define struct_opcode_length_info_t_type_p struct_opcode_length_info_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_opcode_length_info_t_type_p, NULL);
};

/* generated */
%typemap (python, in) opcode_length_info_t * {
#define struct_opcode_length_info_t_type_p struct_opcode_length_info_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_opcode_length_info_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct opcode_info_interface {
#define struct_opcode_info_interface_t_type_p struct_opcode_info_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_opcode_info_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an opcode_info_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct opcode_info_interface *, $source);
} default_value { (const opcode_info_interface_t){ .get_opcode_length_info = 0 } };

/* generated */
%typemap (python, struct-out) struct opcode_info_interface {
#define struct_opcode_info_interface_t_type_p struct_opcode_info_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_opcode_info_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct opcode_info_interface {
#define struct_opcode_info_interface_t_type_p struct_opcode_info_interface_t_type_p
	struct opcode_info_interface *src = MM_MALLOC(1, struct opcode_info_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_opcode_info_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct opcode_info_interface * {
#define struct_opcode_info_interface_t_type_p struct_opcode_info_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_opcode_info_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct opcode_info_interface * {
#define struct_opcode_info_interface_t_type_p struct_opcode_info_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_opcode_info_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct nios_eic_interface {
#define struct_nios_eic_interface_t_type_p struct_nios_eic_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_nios_eic_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a nios_eic_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct nios_eic_interface *, $source);
} default_value { (const nios_eic_interface_t){ .handler = 0 } };

/* generated */
%typemap (python, struct-out) struct nios_eic_interface {
#define struct_nios_eic_interface_t_type_p struct_nios_eic_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_nios_eic_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct nios_eic_interface {
#define struct_nios_eic_interface_t_type_p struct_nios_eic_interface_t_type_p
	struct nios_eic_interface *src = MM_MALLOC(1, struct nios_eic_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_nios_eic_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct nios_eic_interface * {
#define struct_nios_eic_interface_t_type_p struct_nios_eic_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_nios_eic_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct nios_eic_interface * {
#define struct_nios_eic_interface_t_type_p struct_nios_eic_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_nios_eic_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct nios_custom_interface {
#define struct_nios_custom_interface_t_type_p struct_nios_custom_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_nios_custom_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a nios_custom_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct nios_custom_interface *, $source);
} default_value { (const nios_custom_interface_t){ .custom = 0 } };

/* generated */
%typemap (python, struct-out) struct nios_custom_interface {
#define struct_nios_custom_interface_t_type_p struct_nios_custom_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_nios_custom_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct nios_custom_interface {
#define struct_nios_custom_interface_t_type_p struct_nios_custom_interface_t_type_p
	struct nios_custom_interface *src = MM_MALLOC(1, struct nios_custom_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_nios_custom_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct nios_custom_interface * {
#define struct_nios_custom_interface_t_type_p struct_nios_custom_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_nios_custom_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct nios_custom_interface * {
#define struct_nios_custom_interface_t_type_p struct_nios_custom_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_nios_custom_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct nios_cache_interface {
#define struct_nios_cache_interface_t_type_p struct_nios_cache_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_nios_cache_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a nios_cache_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct nios_cache_interface *, $source);
} default_value { (const nios_cache_interface_t){ .flushd = 0 } };

/* generated */
%typemap (python, struct-out) struct nios_cache_interface {
#define struct_nios_cache_interface_t_type_p struct_nios_cache_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_nios_cache_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct nios_cache_interface {
#define struct_nios_cache_interface_t_type_p struct_nios_cache_interface_t_type_p
	struct nios_cache_interface *src = MM_MALLOC(1, struct nios_cache_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_nios_cache_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct nios_cache_interface * {
#define struct_nios_cache_interface_t_type_p struct_nios_cache_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_nios_cache_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct nios_cache_interface * {
#define struct_nios_cache_interface_t_type_p struct_nios_cache_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_nios_cache_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct nios_interface {
#define struct_nios_interface_t_type_p struct_nios_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_nios_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a nios_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct nios_interface *, $source);
};

/* generated */
%typemap (python, struct-out) struct nios_interface {
#define struct_nios_interface_t_type_p struct_nios_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_nios_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct nios_interface {
#define struct_nios_interface_t_type_p struct_nios_interface_t_type_p
	struct nios_interface *src = MM_MALLOC(1, struct nios_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_nios_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct nios_interface * {
#define struct_nios_interface_t_type_p struct_nios_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_nios_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct nios_interface * {
#define struct_nios_interface_t_type_p struct_nios_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_nios_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct network_breakpoint_interface {
#define struct_network_breakpoint_interface_t_type_p struct_network_breakpoint_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_network_breakpoint_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a network_breakpoint_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct network_breakpoint_interface *, $source);
} default_value { (const network_breakpoint_interface_t){ .add = 0 } };

/* generated */
%typemap (python, struct-out) struct network_breakpoint_interface {
#define struct_network_breakpoint_interface_t_type_p struct_network_breakpoint_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_network_breakpoint_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct network_breakpoint_interface {
#define struct_network_breakpoint_interface_t_type_p struct_network_breakpoint_interface_t_type_p
	struct network_breakpoint_interface *src = MM_MALLOC(1, struct network_breakpoint_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_network_breakpoint_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct network_breakpoint_interface * {
#define struct_network_breakpoint_interface_t_type_p struct_network_breakpoint_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_network_breakpoint_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct network_breakpoint_interface * {
#define struct_network_breakpoint_interface_t_type_p struct_network_breakpoint_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_network_breakpoint_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct nand_flash_interface {
#define struct_nand_flash_interface_t_type_p struct_nand_flash_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_nand_flash_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a nand_flash_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct nand_flash_interface *, $source);
} default_value { (const nand_flash_interface_t){ .read_access = 0 } };

/* generated */
%typemap (python, struct-out) struct nand_flash_interface {
#define struct_nand_flash_interface_t_type_p struct_nand_flash_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_nand_flash_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct nand_flash_interface {
#define struct_nand_flash_interface_t_type_p struct_nand_flash_interface_t_type_p
	struct nand_flash_interface *src = MM_MALLOC(1, struct nand_flash_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_nand_flash_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct nand_flash_interface * {
#define struct_nand_flash_interface_t_type_p struct_nand_flash_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_nand_flash_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct nand_flash_interface * {
#define struct_nand_flash_interface_t_type_p struct_nand_flash_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_nand_flash_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct multi_level_signal_interface {
#define struct_multi_level_signal_interface_t_type_p struct_multi_level_signal_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_multi_level_signal_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a multi_level_signal_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct multi_level_signal_interface *, $source);
} default_value { (const multi_level_signal_interface_t){ .signal_level_change = 0 } };

/* generated */
%typemap (python, struct-out) struct multi_level_signal_interface {
#define struct_multi_level_signal_interface_t_type_p struct_multi_level_signal_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_multi_level_signal_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct multi_level_signal_interface {
#define struct_multi_level_signal_interface_t_type_p struct_multi_level_signal_interface_t_type_p
	struct multi_level_signal_interface *src = MM_MALLOC(1, struct multi_level_signal_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_multi_level_signal_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct multi_level_signal_interface * {
#define struct_multi_level_signal_interface_t_type_p struct_multi_level_signal_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_multi_level_signal_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct multi_level_signal_interface * {
#define struct_multi_level_signal_interface_t_type_p struct_multi_level_signal_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_multi_level_signal_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct mouse_interface {
#define struct_mouse_interface_t_type_p struct_mouse_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_mouse_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a mouse_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct mouse_interface *, $source);
} default_value { (const mouse_interface_t){ .mouse_event = 0 } };

/* generated */
%typemap (python, struct-out) struct mouse_interface {
#define struct_mouse_interface_t_type_p struct_mouse_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_mouse_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct mouse_interface {
#define struct_mouse_interface_t_type_p struct_mouse_interface_t_type_p
	struct mouse_interface *src = MM_MALLOC(1, struct mouse_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_mouse_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct mouse_interface * {
#define struct_mouse_interface_t_type_p struct_mouse_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_mouse_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct mouse_interface * {
#define struct_mouse_interface_t_type_p struct_mouse_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_mouse_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct mmc_interface {
#define struct_mmc_interface_t_type_p struct_mmc_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_mmc_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a mmc_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct mmc_interface *, $source);
} default_value { (const mmc_interface_t){ .write_data = 0 } };

/* generated */
%typemap (python, struct-out) struct mmc_interface {
#define struct_mmc_interface_t_type_p struct_mmc_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_mmc_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct mmc_interface {
#define struct_mmc_interface_t_type_p struct_mmc_interface_t_type_p
	struct mmc_interface *src = MM_MALLOC(1, struct mmc_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_mmc_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct mmc_interface * {
#define struct_mmc_interface_t_type_p struct_mmc_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_mmc_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct mmc_interface * {
#define struct_mmc_interface_t_type_p struct_mmc_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_mmc_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct mips_mt_ase_policy_interface {
#define struct_mips_mt_ase_policy_interface_t_type_p struct_mips_mt_ase_policy_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_mips_mt_ase_policy_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a mips_mt_ase_policy_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct mips_mt_ase_policy_interface *, $source);
} default_value { (const mips_mt_ase_policy_interface_t){ .read_grp_prio = 0 } };

/* generated */
%typemap (python, struct-out) struct mips_mt_ase_policy_interface {
#define struct_mips_mt_ase_policy_interface_t_type_p struct_mips_mt_ase_policy_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_mips_mt_ase_policy_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct mips_mt_ase_policy_interface {
#define struct_mips_mt_ase_policy_interface_t_type_p struct_mips_mt_ase_policy_interface_t_type_p
	struct mips_mt_ase_policy_interface *src = MM_MALLOC(1, struct mips_mt_ase_policy_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_mips_mt_ase_policy_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct mips_mt_ase_policy_interface * {
#define struct_mips_mt_ase_policy_interface_t_type_p struct_mips_mt_ase_policy_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_mips_mt_ase_policy_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct mips_mt_ase_policy_interface * {
#define struct_mips_mt_ase_policy_interface_t_type_p struct_mips_mt_ase_policy_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_mips_mt_ase_policy_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct mips_mt_ase_interface {
#define struct_mips_mt_ase_interface_t_type_p struct_mips_mt_ase_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_mips_mt_ase_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a mips_mt_ase_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct mips_mt_ase_interface *, $source);
} default_value { (const mips_mt_ase_interface_t){ .vpe_id = 0 } };

/* generated */
%typemap (python, struct-out) struct mips_mt_ase_interface {
#define struct_mips_mt_ase_interface_t_type_p struct_mips_mt_ase_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_mips_mt_ase_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct mips_mt_ase_interface {
#define struct_mips_mt_ase_interface_t_type_p struct_mips_mt_ase_interface_t_type_p
	struct mips_mt_ase_interface *src = MM_MALLOC(1, struct mips_mt_ase_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_mips_mt_ase_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct mips_mt_ase_interface * {
#define struct_mips_mt_ase_interface_t_type_p struct_mips_mt_ase_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_mips_mt_ase_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct mips_mt_ase_interface * {
#define struct_mips_mt_ase_interface_t_type_p struct_mips_mt_ase_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_mips_mt_ase_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct mips_ite_interface {
#define struct_mips_ite_interface_t_type_p struct_mips_ite_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_mips_ite_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a mips_ite_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct mips_ite_interface *, $source);
} default_value { (const mips_ite_interface_t){ .set_dtag_lo = 0 } };

/* generated */
%typemap (python, struct-out) struct mips_ite_interface {
#define struct_mips_ite_interface_t_type_p struct_mips_ite_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_mips_ite_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct mips_ite_interface {
#define struct_mips_ite_interface_t_type_p struct_mips_ite_interface_t_type_p
	struct mips_ite_interface *src = MM_MALLOC(1, struct mips_ite_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_mips_ite_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct mips_ite_interface * {
#define struct_mips_ite_interface_t_type_p struct_mips_ite_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_mips_ite_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct mips_ite_interface * {
#define struct_mips_ite_interface_t_type_p struct_mips_ite_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_mips_ite_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct mips_exception_query_interface {
#define struct_mips_exception_query_interface_t_type_p struct_mips_exception_query_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_mips_exception_query_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a mips_exception_query_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct mips_exception_query_interface *, $source);
} default_value { (const mips_exception_query_interface_t){ .return_pc = 0 } };

/* generated */
%typemap (python, struct-out) struct mips_exception_query_interface {
#define struct_mips_exception_query_interface_t_type_p struct_mips_exception_query_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_mips_exception_query_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct mips_exception_query_interface {
#define struct_mips_exception_query_interface_t_type_p struct_mips_exception_query_interface_t_type_p
	struct mips_exception_query_interface *src = MM_MALLOC(1, struct mips_exception_query_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_mips_exception_query_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct mips_exception_query_interface * {
#define struct_mips_exception_query_interface_t_type_p struct_mips_exception_query_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_mips_exception_query_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct mips_exception_query_interface * {
#define struct_mips_exception_query_interface_t_type_p struct_mips_exception_query_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_mips_exception_query_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct mips_eic_interface {
#define struct_mips_eic_interface_t_type_p struct_mips_eic_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_mips_eic_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a mips_eic_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct mips_eic_interface *, $source);
} default_value { (const mips_eic_interface_t){ .cpu_pending_irqs = 0 } };

/* generated */
%typemap (python, struct-out) struct mips_eic_interface {
#define struct_mips_eic_interface_t_type_p struct_mips_eic_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_mips_eic_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct mips_eic_interface {
#define struct_mips_eic_interface_t_type_p struct_mips_eic_interface_t_type_p
	struct mips_eic_interface *src = MM_MALLOC(1, struct mips_eic_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_mips_eic_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct mips_eic_interface * {
#define struct_mips_eic_interface_t_type_p struct_mips_eic_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_mips_eic_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct mips_eic_interface * {
#define struct_mips_eic_interface_t_type_p struct_mips_eic_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_mips_eic_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct mips_coprocessor_interface {
#define struct_mips_coprocessor_interface_t_type_p struct_mips_coprocessor_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_mips_coprocessor_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a mips_coprocessor_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct mips_coprocessor_interface *, $source);
} default_value { (const mips_coprocessor_interface_t){ .read_register = 0 } };

/* generated */
%typemap (python, struct-out) struct mips_coprocessor_interface {
#define struct_mips_coprocessor_interface_t_type_p struct_mips_coprocessor_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_mips_coprocessor_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct mips_coprocessor_interface {
#define struct_mips_coprocessor_interface_t_type_p struct_mips_coprocessor_interface_t_type_p
	struct mips_coprocessor_interface *src = MM_MALLOC(1, struct mips_coprocessor_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_mips_coprocessor_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct mips_coprocessor_interface * {
#define struct_mips_coprocessor_interface_t_type_p struct_mips_coprocessor_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_mips_coprocessor_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct mips_coprocessor_interface * {
#define struct_mips_coprocessor_interface_t_type_p struct_mips_coprocessor_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_mips_coprocessor_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct mips_cache_instruction_interface {
#define struct_mips_cache_instruction_interface_t_type_p struct_mips_cache_instruction_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_mips_cache_instruction_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a mips_cache_instruction_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct mips_cache_instruction_interface *, $source);
} default_value { (const mips_cache_instruction_interface_t){ .cache_instruction = 0 } };

/* generated */
%typemap (python, struct-out) struct mips_cache_instruction_interface {
#define struct_mips_cache_instruction_interface_t_type_p struct_mips_cache_instruction_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_mips_cache_instruction_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct mips_cache_instruction_interface {
#define struct_mips_cache_instruction_interface_t_type_p struct_mips_cache_instruction_interface_t_type_p
	struct mips_cache_instruction_interface *src = MM_MALLOC(1, struct mips_cache_instruction_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_mips_cache_instruction_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct mips_cache_instruction_interface * {
#define struct_mips_cache_instruction_interface_t_type_p struct_mips_cache_instruction_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_mips_cache_instruction_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct mips_cache_instruction_interface * {
#define struct_mips_cache_instruction_interface_t_type_p struct_mips_cache_instruction_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_mips_cache_instruction_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct mips_interface {
#define struct_mips_interface_t_type_p struct_mips_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_mips_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a mips_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct mips_interface *, $source);
};

/* generated */
%typemap (python, struct-out) struct mips_interface {
#define struct_mips_interface_t_type_p struct_mips_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_mips_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct mips_interface {
#define struct_mips_interface_t_type_p struct_mips_interface_t_type_p
	struct mips_interface *src = MM_MALLOC(1, struct mips_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_mips_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct mips_interface * {
#define struct_mips_interface_t_type_p struct_mips_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_mips_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct mips_interface * {
#define struct_mips_interface_t_type_p struct_mips_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_mips_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct mii_management_interface {
#define struct_mii_management_interface_t_type_p struct_mii_management_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_mii_management_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a mii_management_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct mii_management_interface *, $source);
} default_value { (const mii_management_interface_t){ .serial_access = 0 } };

/* generated */
%typemap (python, struct-out) struct mii_management_interface {
#define struct_mii_management_interface_t_type_p struct_mii_management_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_mii_management_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct mii_management_interface {
#define struct_mii_management_interface_t_type_p struct_mii_management_interface_t_type_p
	struct mii_management_interface *src = MM_MALLOC(1, struct mii_management_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_mii_management_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct mii_management_interface * {
#define struct_mii_management_interface_t_type_p struct_mii_management_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_mii_management_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct mii_management_interface * {
#define struct_mii_management_interface_t_type_p struct_mii_management_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_mii_management_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct mii_interface {
#define struct_mii_interface_t_type_p struct_mii_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_mii_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a mii_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct mii_interface *, $source);
} default_value { (const mii_interface_t){ .serial_access = 0 } };

/* generated */
%typemap (python, struct-out) struct mii_interface {
#define struct_mii_interface_t_type_p struct_mii_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_mii_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct mii_interface {
#define struct_mii_interface_t_type_p struct_mii_interface_t_type_p
	struct mii_interface *src = MM_MALLOC(1, struct mii_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_mii_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct mii_interface * {
#define struct_mii_interface_t_type_p struct_mii_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_mii_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct mii_interface * {
#define struct_mii_interface_t_type_p struct_mii_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_mii_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct microwire_interface {
#define struct_microwire_interface_t_type_p struct_microwire_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_microwire_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a microwire_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct microwire_interface *, $source);
} default_value { (const microwire_interface_t){ .set_cs = 0 } };

/* generated */
%typemap (python, struct-out) struct microwire_interface {
#define struct_microwire_interface_t_type_p struct_microwire_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_microwire_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct microwire_interface {
#define struct_microwire_interface_t_type_p struct_microwire_interface_t_type_p
	struct microwire_interface *src = MM_MALLOC(1, struct microwire_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_microwire_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct microwire_interface * {
#define struct_microwire_interface_t_type_p struct_microwire_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_microwire_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct microwire_interface * {
#define struct_microwire_interface_t_type_p struct_microwire_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_microwire_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct map_list {
#define struct_map_list_t_type_p struct_map_list_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_map_list_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a map_list_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct map_list *, $source);
} default_value { (const map_list_t){ .map_type = Sim_Map_Ram } };

/* generated */
%typemap (python, struct-out) struct map_list {
#define struct_map_list_t_type_p struct_map_list_t_type_p
$target = new_opaque_object(&$source, false, false, struct_map_list_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct map_list {
#define struct_map_list_t_type_p struct_map_list_t_type_p
	struct map_list *src = MM_MALLOC(1, struct map_list);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_map_list_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct map_list * {
#define struct_map_list_t_type_p struct_map_list_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_map_list_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct map_list * {
#define struct_map_list_t_type_p struct_map_list_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_map_list_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct memory_space_interface {
#define struct_memory_space_interface_t_type_p struct_memory_space_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_memory_space_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a memory_space_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct memory_space_interface *, $source);
} default_value { (const memory_space_interface_t){ .space_lookup = 0 } };

/* generated */
%typemap (python, struct-out) struct memory_space_interface {
#define struct_memory_space_interface_t_type_p struct_memory_space_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_memory_space_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct memory_space_interface {
#define struct_memory_space_interface_t_type_p struct_memory_space_interface_t_type_p
	struct memory_space_interface *src = MM_MALLOC(1, struct memory_space_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_memory_space_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct memory_space_interface * {
#define struct_memory_space_interface_t_type_p struct_memory_space_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_memory_space_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct memory_space_interface * {
#define struct_memory_space_interface_t_type_p struct_memory_space_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_memory_space_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct memory_profiler_interface {
#define struct_memory_profiler_interface_t_type_p struct_memory_profiler_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_memory_profiler_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a memory_profiler_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct memory_profiler_interface *, $source);
} default_value { (const memory_profiler_interface_t){ .get = 0 } };

/* generated */
%typemap (python, struct-out) struct memory_profiler_interface {
#define struct_memory_profiler_interface_t_type_p struct_memory_profiler_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_memory_profiler_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct memory_profiler_interface {
#define struct_memory_profiler_interface_t_type_p struct_memory_profiler_interface_t_type_p
	struct memory_profiler_interface *src = MM_MALLOC(1, struct memory_profiler_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_memory_profiler_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct memory_profiler_interface * {
#define struct_memory_profiler_interface_t_type_p struct_memory_profiler_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_memory_profiler_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct memory_profiler_interface * {
#define struct_memory_profiler_interface_t_type_p struct_memory_profiler_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_memory_profiler_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct mdio45_phy_interface {
#define struct_mdio45_phy_interface_t_type_p struct_mdio45_phy_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_mdio45_phy_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a mdio45_phy_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct mdio45_phy_interface *, $source);
} default_value { (const mdio45_phy_interface_t){ .read_register = 0 } };

/* generated */
%typemap (python, struct-out) struct mdio45_phy_interface {
#define struct_mdio45_phy_interface_t_type_p struct_mdio45_phy_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_mdio45_phy_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct mdio45_phy_interface {
#define struct_mdio45_phy_interface_t_type_p struct_mdio45_phy_interface_t_type_p
	struct mdio45_phy_interface *src = MM_MALLOC(1, struct mdio45_phy_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_mdio45_phy_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct mdio45_phy_interface * {
#define struct_mdio45_phy_interface_t_type_p struct_mdio45_phy_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_mdio45_phy_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct mdio45_phy_interface * {
#define struct_mdio45_phy_interface_t_type_p struct_mdio45_phy_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_mdio45_phy_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct mdio45_bus_interface {
#define struct_mdio45_bus_interface_t_type_p struct_mdio45_bus_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_mdio45_bus_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a mdio45_bus_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct mdio45_bus_interface *, $source);
} default_value { (const mdio45_bus_interface_t){ .read_register = 0 } };

/* generated */
%typemap (python, struct-out) struct mdio45_bus_interface {
#define struct_mdio45_bus_interface_t_type_p struct_mdio45_bus_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_mdio45_bus_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct mdio45_bus_interface {
#define struct_mdio45_bus_interface_t_type_p struct_mdio45_bus_interface_t_type_p
	struct mdio45_bus_interface *src = MM_MALLOC(1, struct mdio45_bus_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_mdio45_bus_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct mdio45_bus_interface * {
#define struct_mdio45_bus_interface_t_type_p struct_mdio45_bus_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_mdio45_bus_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct mdio45_bus_interface * {
#define struct_mdio45_bus_interface_t_type_p struct_mdio45_bus_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_mdio45_bus_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct map_demap_interface {
#define struct_map_demap_interface_t_type_p struct_map_demap_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_map_demap_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a map_demap_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct map_demap_interface *, $source);
} default_value { (const map_demap_interface_t){ .add_map = 0 } };

/* generated */
%typemap (python, struct-out) struct map_demap_interface {
#define struct_map_demap_interface_t_type_p struct_map_demap_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_map_demap_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct map_demap_interface {
#define struct_map_demap_interface_t_type_p struct_map_demap_interface_t_type_p
	struct map_demap_interface *src = MM_MALLOC(1, struct map_demap_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_map_demap_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct map_demap_interface * {
#define struct_map_demap_interface_t_type_p struct_map_demap_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_map_demap_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct map_demap_interface * {
#define struct_map_demap_interface_t_type_p struct_map_demap_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_map_demap_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct log_object_interface {
#define struct_log_object_interface_t_type_p struct_log_object_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_log_object_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a log_object_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct log_object_interface *, $source);
};

/* generated */
%typemap (python, struct-out) struct log_object_interface {
#define struct_log_object_interface_t_type_p struct_log_object_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_log_object_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct log_object_interface {
#define struct_log_object_interface_t_type_p struct_log_object_interface_t_type_p
	struct log_object_interface *src = MM_MALLOC(1, struct log_object_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_log_object_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct log_object_interface * {
#define struct_log_object_interface_t_type_p struct_log_object_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_log_object_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct log_object_interface * {
#define struct_log_object_interface_t_type_p struct_log_object_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_log_object_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct link_endpoint_v2_interface {
#define struct_link_endpoint_v2_interface_t_type_p struct_link_endpoint_v2_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_link_endpoint_v2_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a link_endpoint_v2_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct link_endpoint_v2_interface *, $source);
} default_value { (const link_endpoint_v2_interface_t){ .delivery_time = 0 } };

/* generated */
%typemap (python, struct-out) struct link_endpoint_v2_interface {
#define struct_link_endpoint_v2_interface_t_type_p struct_link_endpoint_v2_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_link_endpoint_v2_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct link_endpoint_v2_interface {
#define struct_link_endpoint_v2_interface_t_type_p struct_link_endpoint_v2_interface_t_type_p
	struct link_endpoint_v2_interface *src = MM_MALLOC(1, struct link_endpoint_v2_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_link_endpoint_v2_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct link_endpoint_v2_interface * {
#define struct_link_endpoint_v2_interface_t_type_p struct_link_endpoint_v2_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_link_endpoint_v2_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct link_endpoint_v2_interface * {
#define struct_link_endpoint_v2_interface_t_type_p struct_link_endpoint_v2_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_link_endpoint_v2_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct __anonymous120 {
#define struct_slave_time_t_type_p struct_slave_time_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_slave_time_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a slave_time_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(slave_time_t *, $source);
} default_value { (const slave_time_t){ .val = { .lo = 0 } } };

/* generated */
%typemap (python, struct-out) struct __anonymous120 {
#define struct_slave_time_t_type_p struct_slave_time_t_type_p
$target = new_opaque_object(&$source, false, false, struct_slave_time_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous120 {
#define struct_slave_time_t_type_p struct_slave_time_t_type_p
	slave_time_t *src = MM_MALLOC(1, slave_time_t);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_slave_time_t_type_p, NULL);
};

/* generated */
%typemap (python, out) slave_time_t * {
#define struct_slave_time_t_type_p struct_slave_time_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_slave_time_t_type_p, NULL);
};

/* generated */
%typemap (python, in) slave_time_t * {
#define struct_slave_time_t_type_p struct_slave_time_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_slave_time_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct link_endpoint_interface {
#define struct_link_endpoint_interface_t_type_p struct_link_endpoint_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_link_endpoint_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a link_endpoint_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct link_endpoint_interface *, $source);
} default_value { (const link_endpoint_interface_t){ .delivery_time = 0 } };

/* generated */
%typemap (python, struct-out) struct link_endpoint_interface {
#define struct_link_endpoint_interface_t_type_p struct_link_endpoint_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_link_endpoint_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct link_endpoint_interface {
#define struct_link_endpoint_interface_t_type_p struct_link_endpoint_interface_t_type_p
	struct link_endpoint_interface *src = MM_MALLOC(1, struct link_endpoint_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_link_endpoint_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct link_endpoint_interface * {
#define struct_link_endpoint_interface_t_type_p struct_link_endpoint_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_link_endpoint_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct link_endpoint_interface * {
#define struct_link_endpoint_interface_t_type_p struct_link_endpoint_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_link_endpoint_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct linear_image_interface {
#define struct_linear_image_interface_t_type_p struct_linear_image_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_linear_image_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a linear_image_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct linear_image_interface *, $source);
} default_value { (const linear_image_interface_t){ .prepare_range = 0 } };

/* generated */
%typemap (python, struct-out) struct linear_image_interface {
#define struct_linear_image_interface_t_type_p struct_linear_image_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_linear_image_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct linear_image_interface {
#define struct_linear_image_interface_t_type_p struct_linear_image_interface_t_type_p
	struct linear_image_interface *src = MM_MALLOC(1, struct linear_image_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_linear_image_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct linear_image_interface * {
#define struct_linear_image_interface_t_type_p struct_linear_image_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_linear_image_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct linear_image_interface * {
#define struct_linear_image_interface_t_type_p struct_linear_image_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_linear_image_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct leader_message_interface {
#define struct_leader_message_interface_t_type_p struct_leader_message_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_leader_message_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a leader_message_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct leader_message_interface *, $source);
} default_value { (const leader_message_interface_t){ .send = 0 } };

/* generated */
%typemap (python, struct-out) struct leader_message_interface {
#define struct_leader_message_interface_t_type_p struct_leader_message_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_leader_message_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct leader_message_interface {
#define struct_leader_message_interface_t_type_p struct_leader_message_interface_t_type_p
	struct leader_message_interface *src = MM_MALLOC(1, struct leader_message_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_leader_message_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct leader_message_interface * {
#define struct_leader_message_interface_t_type_p struct_leader_message_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_leader_message_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct leader_message_interface * {
#define struct_leader_message_interface_t_type_p struct_leader_message_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_leader_message_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct keyboard_console_interface {
#define struct_keyboard_console_interface_t_type_p struct_keyboard_console_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_keyboard_console_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a keyboard_console_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct keyboard_console_interface *, $source);
} default_value { (const keyboard_console_interface_t){ .keyboard_ready = 0 } };

/* generated */
%typemap (python, struct-out) struct keyboard_console_interface {
#define struct_keyboard_console_interface_t_type_p struct_keyboard_console_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_keyboard_console_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct keyboard_console_interface {
#define struct_keyboard_console_interface_t_type_p struct_keyboard_console_interface_t_type_p
	struct keyboard_console_interface *src = MM_MALLOC(1, struct keyboard_console_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_keyboard_console_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct keyboard_console_interface * {
#define struct_keyboard_console_interface_t_type_p struct_keyboard_console_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_keyboard_console_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct keyboard_console_interface * {
#define struct_keyboard_console_interface_t_type_p struct_keyboard_console_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_keyboard_console_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct keyboard_interface {
#define struct_keyboard_interface_t_type_p struct_keyboard_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_keyboard_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a keyboard_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct keyboard_interface *, $source);
} default_value { (const keyboard_interface_t){ .keyboard_event = 0 } };

/* generated */
%typemap (python, struct-out) struct keyboard_interface {
#define struct_keyboard_interface_t_type_p struct_keyboard_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_keyboard_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct keyboard_interface {
#define struct_keyboard_interface_t_type_p struct_keyboard_interface_t_type_p
	struct keyboard_interface *src = MM_MALLOC(1, struct keyboard_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_keyboard_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct keyboard_interface * {
#define struct_keyboard_interface_t_type_p struct_keyboard_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_keyboard_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct keyboard_interface * {
#define struct_keyboard_interface_t_type_p struct_keyboard_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_keyboard_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct jit_control_interface {
#define struct_jit_control_interface_t_type_p struct_jit_control_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_jit_control_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a jit_control_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct jit_control_interface *, $source);
} default_value { (const jit_control_interface_t){ .set_compile_enable = 0 } };

/* generated */
%typemap (python, struct-out) struct jit_control_interface {
#define struct_jit_control_interface_t_type_p struct_jit_control_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_jit_control_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct jit_control_interface {
#define struct_jit_control_interface_t_type_p struct_jit_control_interface_t_type_p
	struct jit_control_interface *src = MM_MALLOC(1, struct jit_control_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_jit_control_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct jit_control_interface * {
#define struct_jit_control_interface_t_type_p struct_jit_control_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_jit_control_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct jit_control_interface * {
#define struct_jit_control_interface_t_type_p struct_jit_control_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_jit_control_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct io_memory_interface {
#define struct_io_memory_interface_t_type_p struct_io_memory_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_io_memory_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an io_memory_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct io_memory_interface *, $source);
} default_value { (const io_memory_interface_t){ ._deprecated_map = 0 } };

/* generated */
%typemap (python, struct-out) struct io_memory_interface {
#define struct_io_memory_interface_t_type_p struct_io_memory_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_io_memory_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct io_memory_interface {
#define struct_io_memory_interface_t_type_p struct_io_memory_interface_t_type_p
	struct io_memory_interface *src = MM_MALLOC(1, struct io_memory_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_io_memory_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct io_memory_interface * {
#define struct_io_memory_interface_t_type_p struct_io_memory_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_io_memory_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct io_memory_interface * {
#define struct_io_memory_interface_t_type_p struct_io_memory_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_io_memory_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct interrupt_subscriber_interface {
#define struct_interrupt_subscriber_interface_t_type_p struct_interrupt_subscriber_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_interrupt_subscriber_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an interrupt_subscriber_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct interrupt_subscriber_interface *, $source);
} default_value { (const interrupt_subscriber_interface_t){ .notify = 0 } };

/* generated */
%typemap (python, struct-out) struct interrupt_subscriber_interface {
#define struct_interrupt_subscriber_interface_t_type_p struct_interrupt_subscriber_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_interrupt_subscriber_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct interrupt_subscriber_interface {
#define struct_interrupt_subscriber_interface_t_type_p struct_interrupt_subscriber_interface_t_type_p
	struct interrupt_subscriber_interface *src = MM_MALLOC(1, struct interrupt_subscriber_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_interrupt_subscriber_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct interrupt_subscriber_interface * {
#define struct_interrupt_subscriber_interface_t_type_p struct_interrupt_subscriber_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_interrupt_subscriber_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct interrupt_subscriber_interface * {
#define struct_interrupt_subscriber_interface_t_type_p struct_interrupt_subscriber_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_interrupt_subscriber_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct interrupt_cpu_interface {
#define struct_interrupt_cpu_interface_t_type_p struct_interrupt_cpu_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_interrupt_cpu_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an interrupt_cpu_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct interrupt_cpu_interface *, $source);
} default_value { (const interrupt_cpu_interface_t){ .ack = 0 } };

/* generated */
%typemap (python, struct-out) struct interrupt_cpu_interface {
#define struct_interrupt_cpu_interface_t_type_p struct_interrupt_cpu_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_interrupt_cpu_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct interrupt_cpu_interface {
#define struct_interrupt_cpu_interface_t_type_p struct_interrupt_cpu_interface_t_type_p
	struct interrupt_cpu_interface *src = MM_MALLOC(1, struct interrupt_cpu_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_interrupt_cpu_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct interrupt_cpu_interface * {
#define struct_interrupt_cpu_interface_t_type_p struct_interrupt_cpu_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_interrupt_cpu_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct interrupt_cpu_interface * {
#define struct_interrupt_cpu_interface_t_type_p struct_interrupt_cpu_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_interrupt_cpu_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct interrupt_ack_interface {
#define struct_interrupt_ack_interface_t_type_p struct_interrupt_ack_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_interrupt_ack_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an interrupt_ack_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct interrupt_ack_interface *, $source);
} default_value { (const interrupt_ack_interface_t){ .raise_interrupt = 0 } };

/* generated */
%typemap (python, struct-out) struct interrupt_ack_interface {
#define struct_interrupt_ack_interface_t_type_p struct_interrupt_ack_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_interrupt_ack_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct interrupt_ack_interface {
#define struct_interrupt_ack_interface_t_type_p struct_interrupt_ack_interface_t_type_p
	struct interrupt_ack_interface *src = MM_MALLOC(1, struct interrupt_ack_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_interrupt_ack_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct interrupt_ack_interface * {
#define struct_interrupt_ack_interface_t_type_p struct_interrupt_ack_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_interrupt_ack_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct interrupt_ack_interface * {
#define struct_interrupt_ack_interface_t_type_p struct_interrupt_ack_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_interrupt_ack_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct internal_class_testing_interface {
#define struct_internal_class_testing_interface_t_type_p struct_internal_class_testing_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_internal_class_testing_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an internal_class_testing_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct internal_class_testing_interface *, $source);
} default_value { (const internal_class_testing_interface_t){ .test = 0 } };

/* generated */
%typemap (python, struct-out) struct internal_class_testing_interface {
#define struct_internal_class_testing_interface_t_type_p struct_internal_class_testing_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_internal_class_testing_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct internal_class_testing_interface {
#define struct_internal_class_testing_interface_t_type_p struct_internal_class_testing_interface_t_type_p
	struct internal_class_testing_interface *src = MM_MALLOC(1, struct internal_class_testing_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_internal_class_testing_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct internal_class_testing_interface * {
#define struct_internal_class_testing_interface_t_type_p struct_internal_class_testing_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_internal_class_testing_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct internal_class_testing_interface * {
#define struct_internal_class_testing_interface_t_type_p struct_internal_class_testing_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_internal_class_testing_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct int_register_interface {
#define struct_int_register_interface_t_type_p struct_int_register_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_int_register_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an int_register_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct int_register_interface *, $source);
} default_value { (const int_register_interface_t){ .get_number = 0 } };

/* generated */
%typemap (python, struct-out) struct int_register_interface {
#define struct_int_register_interface_t_type_p struct_int_register_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_int_register_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct int_register_interface {
#define struct_int_register_interface_t_type_p struct_int_register_interface_t_type_p
	struct int_register_interface *src = MM_MALLOC(1, struct int_register_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_int_register_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct int_register_interface * {
#define struct_int_register_interface_t_type_p struct_int_register_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_int_register_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct int_register_interface * {
#define struct_int_register_interface_t_type_p struct_int_register_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_int_register_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct instrumentation_tool_interface {
#define struct_instrumentation_tool_interface_t_type_p struct_instrumentation_tool_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_instrumentation_tool_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an instrumentation_tool_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct instrumentation_tool_interface *, $source);
} default_value { (const instrumentation_tool_interface_t){ .connect = 0 } };

/* generated */
%typemap (python, struct-out) struct instrumentation_tool_interface {
#define struct_instrumentation_tool_interface_t_type_p struct_instrumentation_tool_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_instrumentation_tool_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct instrumentation_tool_interface {
#define struct_instrumentation_tool_interface_t_type_p struct_instrumentation_tool_interface_t_type_p
	struct instrumentation_tool_interface *src = MM_MALLOC(1, struct instrumentation_tool_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_instrumentation_tool_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct instrumentation_tool_interface * {
#define struct_instrumentation_tool_interface_t_type_p struct_instrumentation_tool_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_instrumentation_tool_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct instrumentation_tool_interface * {
#define struct_instrumentation_tool_interface_t_type_p struct_instrumentation_tool_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_instrumentation_tool_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct instrumentation_order_interface {
#define struct_instrumentation_order_interface_t_type_p struct_instrumentation_order_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_instrumentation_order_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an instrumentation_order_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct instrumentation_order_interface *, $source);
} default_value { (const instrumentation_order_interface_t){ .get_connections = 0 } };

/* generated */
%typemap (python, struct-out) struct instrumentation_order_interface {
#define struct_instrumentation_order_interface_t_type_p struct_instrumentation_order_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_instrumentation_order_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct instrumentation_order_interface {
#define struct_instrumentation_order_interface_t_type_p struct_instrumentation_order_interface_t_type_p
	struct instrumentation_order_interface *src = MM_MALLOC(1, struct instrumentation_order_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_instrumentation_order_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct instrumentation_order_interface * {
#define struct_instrumentation_order_interface_t_type_p struct_instrumentation_order_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_instrumentation_order_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct instrumentation_order_interface * {
#define struct_instrumentation_order_interface_t_type_p struct_instrumentation_order_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_instrumentation_order_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct instrumentation_filter_status_interface {
#define struct_instrumentation_filter_status_interface_t_type_p struct_instrumentation_filter_status_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_instrumentation_filter_status_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an instrumentation_filter_status_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct instrumentation_filter_status_interface *, $source);
} default_value { (const instrumentation_filter_status_interface_t){ .get_disabled_sources = 0 } };

/* generated */
%typemap (python, struct-out) struct instrumentation_filter_status_interface {
#define struct_instrumentation_filter_status_interface_t_type_p struct_instrumentation_filter_status_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_instrumentation_filter_status_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct instrumentation_filter_status_interface {
#define struct_instrumentation_filter_status_interface_t_type_p struct_instrumentation_filter_status_interface_t_type_p
	struct instrumentation_filter_status_interface *src = MM_MALLOC(1, struct instrumentation_filter_status_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_instrumentation_filter_status_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct instrumentation_filter_status_interface * {
#define struct_instrumentation_filter_status_interface_t_type_p struct_instrumentation_filter_status_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_instrumentation_filter_status_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct instrumentation_filter_status_interface * {
#define struct_instrumentation_filter_status_interface_t_type_p struct_instrumentation_filter_status_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_instrumentation_filter_status_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct instrumentation_filter_slave_interface {
#define struct_instrumentation_filter_slave_interface_t_type_p struct_instrumentation_filter_slave_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_instrumentation_filter_slave_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an instrumentation_filter_slave_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct instrumentation_filter_slave_interface *, $source);
} default_value { (const instrumentation_filter_slave_interface_t){ .disable = 0 } };

/* generated */
%typemap (python, struct-out) struct instrumentation_filter_slave_interface {
#define struct_instrumentation_filter_slave_interface_t_type_p struct_instrumentation_filter_slave_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_instrumentation_filter_slave_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct instrumentation_filter_slave_interface {
#define struct_instrumentation_filter_slave_interface_t_type_p struct_instrumentation_filter_slave_interface_t_type_p
	struct instrumentation_filter_slave_interface *src = MM_MALLOC(1, struct instrumentation_filter_slave_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_instrumentation_filter_slave_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct instrumentation_filter_slave_interface * {
#define struct_instrumentation_filter_slave_interface_t_type_p struct_instrumentation_filter_slave_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_instrumentation_filter_slave_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct instrumentation_filter_slave_interface * {
#define struct_instrumentation_filter_slave_interface_t_type_p struct_instrumentation_filter_slave_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_instrumentation_filter_slave_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct instrumentation_filter_master_interface {
#define struct_instrumentation_filter_master_interface_t_type_p struct_instrumentation_filter_master_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_instrumentation_filter_master_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an instrumentation_filter_master_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct instrumentation_filter_master_interface *, $source);
} default_value { (const instrumentation_filter_master_interface_t){ .set_source_id = 0 } };

/* generated */
%typemap (python, struct-out) struct instrumentation_filter_master_interface {
#define struct_instrumentation_filter_master_interface_t_type_p struct_instrumentation_filter_master_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_instrumentation_filter_master_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct instrumentation_filter_master_interface {
#define struct_instrumentation_filter_master_interface_t_type_p struct_instrumentation_filter_master_interface_t_type_p
	struct instrumentation_filter_master_interface *src = MM_MALLOC(1, struct instrumentation_filter_master_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_instrumentation_filter_master_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct instrumentation_filter_master_interface * {
#define struct_instrumentation_filter_master_interface_t_type_p struct_instrumentation_filter_master_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_instrumentation_filter_master_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct instrumentation_filter_master_interface * {
#define struct_instrumentation_filter_master_interface_t_type_p struct_instrumentation_filter_master_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_instrumentation_filter_master_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct instrumentation_connection_interface {
#define struct_instrumentation_connection_interface_t_type_p struct_instrumentation_connection_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_instrumentation_connection_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an instrumentation_connection_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct instrumentation_connection_interface *, $source);
} default_value { (const instrumentation_connection_interface_t){ .enable = 0 } };

/* generated */
%typemap (python, struct-out) struct instrumentation_connection_interface {
#define struct_instrumentation_connection_interface_t_type_p struct_instrumentation_connection_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_instrumentation_connection_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct instrumentation_connection_interface {
#define struct_instrumentation_connection_interface_t_type_p struct_instrumentation_connection_interface_t_type_p
	struct instrumentation_connection_interface *src = MM_MALLOC(1, struct instrumentation_connection_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_instrumentation_connection_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct instrumentation_connection_interface * {
#define struct_instrumentation_connection_interface_t_type_p struct_instrumentation_connection_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_instrumentation_connection_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct instrumentation_connection_interface * {
#define struct_instrumentation_connection_interface_t_type_p struct_instrumentation_connection_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_instrumentation_connection_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct instrum_registration_interface {
#define struct_instrum_registration_interface_t_type_p struct_instrum_registration_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_instrum_registration_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an instrum_registration_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct instrum_registration_interface *, $source);
} default_value { (const instrum_registration_interface_t){ .pre_register_provider_contract = 0 } };

/* generated */
%typemap (python, struct-out) struct instrum_registration_interface {
#define struct_instrum_registration_interface_t_type_p struct_instrum_registration_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_instrum_registration_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct instrum_registration_interface {
#define struct_instrum_registration_interface_t_type_p struct_instrum_registration_interface_t_type_p
	struct instrum_registration_interface *src = MM_MALLOC(1, struct instrum_registration_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_instrum_registration_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct instrum_registration_interface * {
#define struct_instrum_registration_interface_t_type_p struct_instrum_registration_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_instrum_registration_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct instrum_registration_interface * {
#define struct_instrum_registration_interface_t_type_p struct_instrum_registration_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_instrum_registration_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct instruction_fetch_interface {
#define struct_instruction_fetch_interface_t_type_p struct_instruction_fetch_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_instruction_fetch_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an instruction_fetch_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct instruction_fetch_interface *, $source);
} default_value { (const instruction_fetch_interface_t){ .get_mode = 0 } };

/* generated */
%typemap (python, struct-out) struct instruction_fetch_interface {
#define struct_instruction_fetch_interface_t_type_p struct_instruction_fetch_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_instruction_fetch_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct instruction_fetch_interface {
#define struct_instruction_fetch_interface_t_type_p struct_instruction_fetch_interface_t_type_p
	struct instruction_fetch_interface *src = MM_MALLOC(1, struct instruction_fetch_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_instruction_fetch_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct instruction_fetch_interface * {
#define struct_instruction_fetch_interface_t_type_p struct_instruction_fetch_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_instruction_fetch_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct instruction_fetch_interface * {
#define struct_instruction_fetch_interface_t_type_p struct_instruction_fetch_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_instruction_fetch_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct image_interface {
#define struct_image_interface_t_type_p struct_image_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_image_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an image_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct image_interface *, $source);
} default_value { (const image_interface_t){ .set_persistent = 0 } };

/* generated */
%typemap (python, struct-out) struct image_interface {
#define struct_image_interface_t_type_p struct_image_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_image_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct image_interface {
#define struct_image_interface_t_type_p struct_image_interface_t_type_p
	struct image_interface *src = MM_MALLOC(1, struct image_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_image_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct image_interface * {
#define struct_image_interface_t_type_p struct_image_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_image_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct image_interface * {
#define struct_image_interface_t_type_p struct_image_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_image_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct ieee_802_3_phy_v3_interface {
#define struct_ieee_802_3_phy_v3_interface_t_type_p struct_ieee_802_3_phy_v3_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_ieee_802_3_phy_v3_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an ieee_802_3_phy_v3_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct ieee_802_3_phy_v3_interface *, $source);
} default_value { (const ieee_802_3_phy_v3_interface_t){ .send_frame = 0 } };

/* generated */
%typemap (python, struct-out) struct ieee_802_3_phy_v3_interface {
#define struct_ieee_802_3_phy_v3_interface_t_type_p struct_ieee_802_3_phy_v3_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_ieee_802_3_phy_v3_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct ieee_802_3_phy_v3_interface {
#define struct_ieee_802_3_phy_v3_interface_t_type_p struct_ieee_802_3_phy_v3_interface_t_type_p
	struct ieee_802_3_phy_v3_interface *src = MM_MALLOC(1, struct ieee_802_3_phy_v3_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_ieee_802_3_phy_v3_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct ieee_802_3_phy_v3_interface * {
#define struct_ieee_802_3_phy_v3_interface_t_type_p struct_ieee_802_3_phy_v3_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_ieee_802_3_phy_v3_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct ieee_802_3_phy_v3_interface * {
#define struct_ieee_802_3_phy_v3_interface_t_type_p struct_ieee_802_3_phy_v3_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_ieee_802_3_phy_v3_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct ieee_802_3_phy_v2_interface {
#define struct_ieee_802_3_phy_v2_interface_t_type_p struct_ieee_802_3_phy_v2_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_ieee_802_3_phy_v2_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an ieee_802_3_phy_v2_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct ieee_802_3_phy_v2_interface *, $source);
} default_value { (const ieee_802_3_phy_v2_interface_t){ .send_frame = 0 } };

/* generated */
%typemap (python, struct-out) struct ieee_802_3_phy_v2_interface {
#define struct_ieee_802_3_phy_v2_interface_t_type_p struct_ieee_802_3_phy_v2_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_ieee_802_3_phy_v2_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct ieee_802_3_phy_v2_interface {
#define struct_ieee_802_3_phy_v2_interface_t_type_p struct_ieee_802_3_phy_v2_interface_t_type_p
	struct ieee_802_3_phy_v2_interface *src = MM_MALLOC(1, struct ieee_802_3_phy_v2_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_ieee_802_3_phy_v2_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct ieee_802_3_phy_v2_interface * {
#define struct_ieee_802_3_phy_v2_interface_t_type_p struct_ieee_802_3_phy_v2_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_ieee_802_3_phy_v2_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct ieee_802_3_phy_v2_interface * {
#define struct_ieee_802_3_phy_v2_interface_t_type_p struct_ieee_802_3_phy_v2_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_ieee_802_3_phy_v2_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct ieee_802_3_phy_interface {
#define struct_ieee_802_3_phy_interface_t_type_p struct_ieee_802_3_phy_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_ieee_802_3_phy_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an ieee_802_3_phy_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct ieee_802_3_phy_interface *, $source);
} default_value { (const ieee_802_3_phy_interface_t){ .send_frame = 0 } };

/* generated */
%typemap (python, struct-out) struct ieee_802_3_phy_interface {
#define struct_ieee_802_3_phy_interface_t_type_p struct_ieee_802_3_phy_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_ieee_802_3_phy_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct ieee_802_3_phy_interface {
#define struct_ieee_802_3_phy_interface_t_type_p struct_ieee_802_3_phy_interface_t_type_p
	struct ieee_802_3_phy_interface *src = MM_MALLOC(1, struct ieee_802_3_phy_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_ieee_802_3_phy_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct ieee_802_3_phy_interface * {
#define struct_ieee_802_3_phy_interface_t_type_p struct_ieee_802_3_phy_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_ieee_802_3_phy_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct ieee_802_3_phy_interface * {
#define struct_ieee_802_3_phy_interface_t_type_p struct_ieee_802_3_phy_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_ieee_802_3_phy_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct ieee_802_3_mac_v3_interface {
#define struct_ieee_802_3_mac_v3_interface_t_type_p struct_ieee_802_3_mac_v3_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_ieee_802_3_mac_v3_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an ieee_802_3_mac_v3_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct ieee_802_3_mac_v3_interface *, $source);
} default_value { (const ieee_802_3_mac_v3_interface_t){ .receive_frame = 0 } };

/* generated */
%typemap (python, struct-out) struct ieee_802_3_mac_v3_interface {
#define struct_ieee_802_3_mac_v3_interface_t_type_p struct_ieee_802_3_mac_v3_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_ieee_802_3_mac_v3_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct ieee_802_3_mac_v3_interface {
#define struct_ieee_802_3_mac_v3_interface_t_type_p struct_ieee_802_3_mac_v3_interface_t_type_p
	struct ieee_802_3_mac_v3_interface *src = MM_MALLOC(1, struct ieee_802_3_mac_v3_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_ieee_802_3_mac_v3_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct ieee_802_3_mac_v3_interface * {
#define struct_ieee_802_3_mac_v3_interface_t_type_p struct_ieee_802_3_mac_v3_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_ieee_802_3_mac_v3_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct ieee_802_3_mac_v3_interface * {
#define struct_ieee_802_3_mac_v3_interface_t_type_p struct_ieee_802_3_mac_v3_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_ieee_802_3_mac_v3_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct ieee_802_3_mac_interface {
#define struct_ieee_802_3_mac_interface_t_type_p struct_ieee_802_3_mac_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_ieee_802_3_mac_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an ieee_802_3_mac_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct ieee_802_3_mac_interface *, $source);
} default_value { (const ieee_802_3_mac_interface_t){ .receive_frame = 0 } };

/* generated */
%typemap (python, struct-out) struct ieee_802_3_mac_interface {
#define struct_ieee_802_3_mac_interface_t_type_p struct_ieee_802_3_mac_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_ieee_802_3_mac_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct ieee_802_3_mac_interface {
#define struct_ieee_802_3_mac_interface_t_type_p struct_ieee_802_3_mac_interface_t_type_p
	struct ieee_802_3_mac_interface *src = MM_MALLOC(1, struct ieee_802_3_mac_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_ieee_802_3_mac_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct ieee_802_3_mac_interface * {
#define struct_ieee_802_3_mac_interface_t_type_p struct_ieee_802_3_mac_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_ieee_802_3_mac_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct ieee_802_3_mac_interface * {
#define struct_ieee_802_3_mac_interface_t_type_p struct_ieee_802_3_mac_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_ieee_802_3_mac_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct icode_interface {
#define struct_icode_interface_t_type_p struct_icode_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_icode_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an icode_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct icode_interface *, $source);
} default_value { (const icode_interface_t){ .release_all_icode = 0 } };

/* generated */
%typemap (python, struct-out) struct icode_interface {
#define struct_icode_interface_t_type_p struct_icode_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_icode_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct icode_interface {
#define struct_icode_interface_t_type_p struct_icode_interface_t_type_p
	struct icode_interface *src = MM_MALLOC(1, struct icode_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_icode_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct icode_interface * {
#define struct_icode_interface_t_type_p struct_icode_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_icode_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct icode_interface * {
#define struct_icode_interface_t_type_p struct_icode_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_icode_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct i8051_timer_interface {
#define struct_i8051_timer_interface_t_type_p struct_i8051_timer_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_i8051_timer_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an i8051_timer_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct i8051_timer_interface *, $source);
} default_value { (const i8051_timer_interface_t){ .change_mode = 0 } };

/* generated */
%typemap (python, struct-out) struct i8051_timer_interface {
#define struct_i8051_timer_interface_t_type_p struct_i8051_timer_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_i8051_timer_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct i8051_timer_interface {
#define struct_i8051_timer_interface_t_type_p struct_i8051_timer_interface_t_type_p
	struct i8051_timer_interface *src = MM_MALLOC(1, struct i8051_timer_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_i8051_timer_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct i8051_timer_interface * {
#define struct_i8051_timer_interface_t_type_p struct_i8051_timer_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_i8051_timer_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct i8051_timer_interface * {
#define struct_i8051_timer_interface_t_type_p struct_i8051_timer_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_i8051_timer_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct i8051_interrupt_interface {
#define struct_i8051_interrupt_interface_t_type_p struct_i8051_interrupt_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_i8051_interrupt_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an i8051_interrupt_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct i8051_interrupt_interface *, $source);
} default_value { (const i8051_interrupt_interface_t){ .active_interrupt = 0 } };

/* generated */
%typemap (python, struct-out) struct i8051_interrupt_interface {
#define struct_i8051_interrupt_interface_t_type_p struct_i8051_interrupt_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_i8051_interrupt_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct i8051_interrupt_interface {
#define struct_i8051_interrupt_interface_t_type_p struct_i8051_interrupt_interface_t_type_p
	struct i8051_interrupt_interface *src = MM_MALLOC(1, struct i8051_interrupt_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_i8051_interrupt_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct i8051_interrupt_interface * {
#define struct_i8051_interrupt_interface_t_type_p struct_i8051_interrupt_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_i8051_interrupt_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct i8051_interrupt_interface * {
#define struct_i8051_interrupt_interface_t_type_p struct_i8051_interrupt_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_i8051_interrupt_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct i3c_slave_interface {
#define struct_i3c_slave_interface_t_type_p struct_i3c_slave_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_i3c_slave_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an i3c_slave_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct i3c_slave_interface *, $source);
} default_value { (const i3c_slave_interface_t){ .start = 0 } };

/* generated */
%typemap (python, struct-out) struct i3c_slave_interface {
#define struct_i3c_slave_interface_t_type_p struct_i3c_slave_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_i3c_slave_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct i3c_slave_interface {
#define struct_i3c_slave_interface_t_type_p struct_i3c_slave_interface_t_type_p
	struct i3c_slave_interface *src = MM_MALLOC(1, struct i3c_slave_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_i3c_slave_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct i3c_slave_interface * {
#define struct_i3c_slave_interface_t_type_p struct_i3c_slave_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_i3c_slave_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct i3c_slave_interface * {
#define struct_i3c_slave_interface_t_type_p struct_i3c_slave_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_i3c_slave_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct i3c_master_interface {
#define struct_i3c_master_interface_t_type_p struct_i3c_master_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_i3c_master_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an i3c_master_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct i3c_master_interface *, $source);
} default_value { (const i3c_master_interface_t){ .acknowledge = 0 } };

/* generated */
%typemap (python, struct-out) struct i3c_master_interface {
#define struct_i3c_master_interface_t_type_p struct_i3c_master_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_i3c_master_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct i3c_master_interface {
#define struct_i3c_master_interface_t_type_p struct_i3c_master_interface_t_type_p
	struct i3c_master_interface *src = MM_MALLOC(1, struct i3c_master_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_i3c_master_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct i3c_master_interface * {
#define struct_i3c_master_interface_t_type_p struct_i3c_master_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_i3c_master_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct i3c_master_interface * {
#define struct_i3c_master_interface_t_type_p struct_i3c_master_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_i3c_master_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct i3c_hdr_slave_interface {
#define struct_i3c_hdr_slave_interface_t_type_p struct_i3c_hdr_slave_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_i3c_hdr_slave_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an i3c_hdr_slave_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct i3c_hdr_slave_interface *, $source);
} default_value { (const i3c_hdr_slave_interface_t){ .hdr_write = 0 } };

/* generated */
%typemap (python, struct-out) struct i3c_hdr_slave_interface {
#define struct_i3c_hdr_slave_interface_t_type_p struct_i3c_hdr_slave_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_i3c_hdr_slave_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct i3c_hdr_slave_interface {
#define struct_i3c_hdr_slave_interface_t_type_p struct_i3c_hdr_slave_interface_t_type_p
	struct i3c_hdr_slave_interface *src = MM_MALLOC(1, struct i3c_hdr_slave_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_i3c_hdr_slave_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct i3c_hdr_slave_interface * {
#define struct_i3c_hdr_slave_interface_t_type_p struct_i3c_hdr_slave_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_i3c_hdr_slave_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct i3c_hdr_slave_interface * {
#define struct_i3c_hdr_slave_interface_t_type_p struct_i3c_hdr_slave_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_i3c_hdr_slave_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct i3c_hdr_master_interface {
#define struct_i3c_hdr_master_interface_t_type_p struct_i3c_hdr_master_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_i3c_hdr_master_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an i3c_hdr_master_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct i3c_hdr_master_interface *, $source);
} default_value { (const i3c_hdr_master_interface_t){ .hdr_read_response = 0 } };

/* generated */
%typemap (python, struct-out) struct i3c_hdr_master_interface {
#define struct_i3c_hdr_master_interface_t_type_p struct_i3c_hdr_master_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_i3c_hdr_master_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct i3c_hdr_master_interface {
#define struct_i3c_hdr_master_interface_t_type_p struct_i3c_hdr_master_interface_t_type_p
	struct i3c_hdr_master_interface *src = MM_MALLOC(1, struct i3c_hdr_master_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_i3c_hdr_master_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct i3c_hdr_master_interface * {
#define struct_i3c_hdr_master_interface_t_type_p struct_i3c_hdr_master_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_i3c_hdr_master_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct i3c_hdr_master_interface * {
#define struct_i3c_hdr_master_interface_t_type_p struct_i3c_hdr_master_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_i3c_hdr_master_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct i3c_daa_snoop_interface {
#define struct_i3c_daa_snoop_interface_t_type_p struct_i3c_daa_snoop_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_i3c_daa_snoop_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an i3c_daa_snoop_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct i3c_daa_snoop_interface *, $source);
} default_value { (const i3c_daa_snoop_interface_t){ .assigned_address = 0 } };

/* generated */
%typemap (python, struct-out) struct i3c_daa_snoop_interface {
#define struct_i3c_daa_snoop_interface_t_type_p struct_i3c_daa_snoop_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_i3c_daa_snoop_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct i3c_daa_snoop_interface {
#define struct_i3c_daa_snoop_interface_t_type_p struct_i3c_daa_snoop_interface_t_type_p
	struct i3c_daa_snoop_interface *src = MM_MALLOC(1, struct i3c_daa_snoop_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_i3c_daa_snoop_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct i3c_daa_snoop_interface * {
#define struct_i3c_daa_snoop_interface_t_type_p struct_i3c_daa_snoop_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_i3c_daa_snoop_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct i3c_daa_snoop_interface * {
#define struct_i3c_daa_snoop_interface_t_type_p struct_i3c_daa_snoop_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_i3c_daa_snoop_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct i2c_slave_v2_interface {
#define struct_i2c_slave_v2_interface_t_type_p struct_i2c_slave_v2_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_i2c_slave_v2_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an i2c_slave_v2_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct i2c_slave_v2_interface *, $source);
} default_value { (const i2c_slave_v2_interface_t){ .start = 0 } };

/* generated */
%typemap (python, struct-out) struct i2c_slave_v2_interface {
#define struct_i2c_slave_v2_interface_t_type_p struct_i2c_slave_v2_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_i2c_slave_v2_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct i2c_slave_v2_interface {
#define struct_i2c_slave_v2_interface_t_type_p struct_i2c_slave_v2_interface_t_type_p
	struct i2c_slave_v2_interface *src = MM_MALLOC(1, struct i2c_slave_v2_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_i2c_slave_v2_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct i2c_slave_v2_interface * {
#define struct_i2c_slave_v2_interface_t_type_p struct_i2c_slave_v2_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_i2c_slave_v2_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct i2c_slave_v2_interface * {
#define struct_i2c_slave_v2_interface_t_type_p struct_i2c_slave_v2_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_i2c_slave_v2_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct i2c_slave_interface {
#define struct_i2c_slave_interface_t_type_p struct_i2c_slave_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_i2c_slave_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an i2c_slave_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct i2c_slave_interface *, $source);
} default_value { (const i2c_slave_interface_t){ .start_request = 0 } };

/* generated */
%typemap (python, struct-out) struct i2c_slave_interface {
#define struct_i2c_slave_interface_t_type_p struct_i2c_slave_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_i2c_slave_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct i2c_slave_interface {
#define struct_i2c_slave_interface_t_type_p struct_i2c_slave_interface_t_type_p
	struct i2c_slave_interface *src = MM_MALLOC(1, struct i2c_slave_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_i2c_slave_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct i2c_slave_interface * {
#define struct_i2c_slave_interface_t_type_p struct_i2c_slave_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_i2c_slave_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct i2c_slave_interface * {
#define struct_i2c_slave_interface_t_type_p struct_i2c_slave_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_i2c_slave_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct i2c_master_v2_interface {
#define struct_i2c_master_v2_interface_t_type_p struct_i2c_master_v2_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_i2c_master_v2_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an i2c_master_v2_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct i2c_master_v2_interface *, $source);
} default_value { (const i2c_master_v2_interface_t){ .acknowledge = 0 } };

/* generated */
%typemap (python, struct-out) struct i2c_master_v2_interface {
#define struct_i2c_master_v2_interface_t_type_p struct_i2c_master_v2_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_i2c_master_v2_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct i2c_master_v2_interface {
#define struct_i2c_master_v2_interface_t_type_p struct_i2c_master_v2_interface_t_type_p
	struct i2c_master_v2_interface *src = MM_MALLOC(1, struct i2c_master_v2_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_i2c_master_v2_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct i2c_master_v2_interface * {
#define struct_i2c_master_v2_interface_t_type_p struct_i2c_master_v2_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_i2c_master_v2_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct i2c_master_v2_interface * {
#define struct_i2c_master_v2_interface_t_type_p struct_i2c_master_v2_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_i2c_master_v2_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct i2c_master_interface {
#define struct_i2c_master_interface_t_type_p struct_i2c_master_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_i2c_master_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an i2c_master_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct i2c_master_interface *, $source);
} default_value { (const i2c_master_interface_t){ .bus_freed = 0 } };

/* generated */
%typemap (python, struct-out) struct i2c_master_interface {
#define struct_i2c_master_interface_t_type_p struct_i2c_master_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_i2c_master_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct i2c_master_interface {
#define struct_i2c_master_interface_t_type_p struct_i2c_master_interface_t_type_p
	struct i2c_master_interface *src = MM_MALLOC(1, struct i2c_master_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_i2c_master_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct i2c_master_interface * {
#define struct_i2c_master_interface_t_type_p struct_i2c_master_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_i2c_master_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct i2c_master_interface * {
#define struct_i2c_master_interface_t_type_p struct_i2c_master_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_i2c_master_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct i2c_link_interface {
#define struct_i2c_link_interface_t_type_p struct_i2c_link_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_i2c_link_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an i2c_link_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct i2c_link_interface *, $source);
} default_value { (const i2c_link_interface_t){ .register_slave_address = 0 } };

/* generated */
%typemap (python, struct-out) struct i2c_link_interface {
#define struct_i2c_link_interface_t_type_p struct_i2c_link_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_i2c_link_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct i2c_link_interface {
#define struct_i2c_link_interface_t_type_p struct_i2c_link_interface_t_type_p
	struct i2c_link_interface *src = MM_MALLOC(1, struct i2c_link_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_i2c_link_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct i2c_link_interface * {
#define struct_i2c_link_interface_t_type_p struct_i2c_link_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_i2c_link_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct i2c_link_interface * {
#define struct_i2c_link_interface_t_type_p struct_i2c_link_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_i2c_link_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct i2c_device_interface {
#define struct_i2c_device_interface_t_type_p struct_i2c_device_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_i2c_device_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an i2c_device_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct i2c_device_interface *, $source);
} default_value { (const i2c_device_interface_t){ .set_state = 0 } };

/* generated */
%typemap (python, struct-out) struct i2c_device_interface {
#define struct_i2c_device_interface_t_type_p struct_i2c_device_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_i2c_device_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct i2c_device_interface {
#define struct_i2c_device_interface_t_type_p struct_i2c_device_interface_t_type_p
	struct i2c_device_interface *src = MM_MALLOC(1, struct i2c_device_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_i2c_device_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct i2c_device_interface * {
#define struct_i2c_device_interface_t_type_p struct_i2c_device_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_i2c_device_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct i2c_device_interface * {
#define struct_i2c_device_interface_t_type_p struct_i2c_device_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_i2c_device_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct i2c_bus_interface {
#define struct_i2c_bus_interface_t_type_p struct_i2c_bus_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_i2c_bus_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an i2c_bus_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct i2c_bus_interface *, $source);
} default_value { (const i2c_bus_interface_t){ .start = 0 } };

/* generated */
%typemap (python, struct-out) struct i2c_bus_interface {
#define struct_i2c_bus_interface_t_type_p struct_i2c_bus_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_i2c_bus_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct i2c_bus_interface {
#define struct_i2c_bus_interface_t_type_p struct_i2c_bus_interface_t_type_p
	struct i2c_bus_interface *src = MM_MALLOC(1, struct i2c_bus_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_i2c_bus_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct i2c_bus_interface * {
#define struct_i2c_bus_interface_t_type_p struct_i2c_bus_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_i2c_bus_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct i2c_bus_interface * {
#define struct_i2c_bus_interface_t_type_p struct_i2c_bus_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_i2c_bus_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct i2c_bridge_interface {
#define struct_i2c_bridge_interface_t_type_p struct_i2c_bridge_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_i2c_bridge_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an i2c_bridge_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct i2c_bridge_interface *, $source);
} default_value { (const i2c_bridge_interface_t){ .address_added = 0 } };

/* generated */
%typemap (python, struct-out) struct i2c_bridge_interface {
#define struct_i2c_bridge_interface_t_type_p struct_i2c_bridge_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_i2c_bridge_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct i2c_bridge_interface {
#define struct_i2c_bridge_interface_t_type_p struct_i2c_bridge_interface_t_type_p
	struct i2c_bridge_interface *src = MM_MALLOC(1, struct i2c_bridge_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_i2c_bridge_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct i2c_bridge_interface * {
#define struct_i2c_bridge_interface_t_type_p struct_i2c_bridge_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_i2c_bridge_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct i2c_bridge_interface * {
#define struct_i2c_bridge_interface_t_type_p struct_i2c_bridge_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_i2c_bridge_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct host_serial_interface {
#define struct_host_serial_interface_t_type_p struct_host_serial_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_host_serial_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a host_serial_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct host_serial_interface *, $source);
} default_value { (const host_serial_interface_t){ .setup = 0 } };

/* generated */
%typemap (python, struct-out) struct host_serial_interface {
#define struct_host_serial_interface_t_type_p struct_host_serial_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_host_serial_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct host_serial_interface {
#define struct_host_serial_interface_t_type_p struct_host_serial_interface_t_type_p
	struct host_serial_interface *src = MM_MALLOC(1, struct host_serial_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_host_serial_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct host_serial_interface * {
#define struct_host_serial_interface_t_type_p struct_host_serial_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_host_serial_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct host_serial_interface * {
#define struct_host_serial_interface_t_type_p struct_host_serial_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_host_serial_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct gui_console_backend_interface {
#define struct_gui_console_backend_interface_t_type_p struct_gui_console_backend_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_gui_console_backend_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a gui_console_backend_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct gui_console_backend_interface *, $source);
} default_value { (const gui_console_backend_interface_t){ .start = 0 } };

/* generated */
%typemap (python, struct-out) struct gui_console_backend_interface {
#define struct_gui_console_backend_interface_t_type_p struct_gui_console_backend_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_gui_console_backend_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct gui_console_backend_interface {
#define struct_gui_console_backend_interface_t_type_p struct_gui_console_backend_interface_t_type_p
	struct gui_console_backend_interface *src = MM_MALLOC(1, struct gui_console_backend_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_gui_console_backend_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct gui_console_backend_interface * {
#define struct_gui_console_backend_interface_t_type_p struct_gui_console_backend_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_gui_console_backend_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct gui_console_backend_interface * {
#define struct_gui_console_backend_interface_t_type_p struct_gui_console_backend_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_gui_console_backend_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct global_time_interface {
#define struct_global_time_interface_t_type_p struct_global_time_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_global_time_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a global_time_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct global_time_interface *, $source);
} default_value { (const global_time_interface_t){ .set = 0 } };

/* generated */
%typemap (python, struct-out) struct global_time_interface {
#define struct_global_time_interface_t_type_p struct_global_time_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_global_time_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct global_time_interface {
#define struct_global_time_interface_t_type_p struct_global_time_interface_t_type_p
	struct global_time_interface *src = MM_MALLOC(1, struct global_time_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_global_time_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct global_time_interface * {
#define struct_global_time_interface_t_type_p struct_global_time_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_global_time_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct global_time_interface * {
#define struct_global_time_interface_t_type_p struct_global_time_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_global_time_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct gfx_console_frontend_interface {
#define struct_gfx_console_frontend_interface_t_type_p struct_gfx_console_frontend_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_gfx_console_frontend_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a gfx_console_frontend_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct gfx_console_frontend_interface *, $source);
} default_value { (const gfx_console_frontend_interface_t){ .start = 0 } };

/* generated */
%typemap (python, struct-out) struct gfx_console_frontend_interface {
#define struct_gfx_console_frontend_interface_t_type_p struct_gfx_console_frontend_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_gfx_console_frontend_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct gfx_console_frontend_interface {
#define struct_gfx_console_frontend_interface_t_type_p struct_gfx_console_frontend_interface_t_type_p
	struct gfx_console_frontend_interface *src = MM_MALLOC(1, struct gfx_console_frontend_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_gfx_console_frontend_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct gfx_console_frontend_interface * {
#define struct_gfx_console_frontend_interface_t_type_p struct_gfx_console_frontend_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_gfx_console_frontend_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct gfx_console_frontend_interface * {
#define struct_gfx_console_frontend_interface_t_type_p struct_gfx_console_frontend_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_gfx_console_frontend_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct gfx_console_backend_interface {
#define struct_gfx_console_backend_interface_t_type_p struct_gfx_console_backend_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_gfx_console_backend_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a gfx_console_backend_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct gfx_console_backend_interface *, $source);
} default_value { (const gfx_console_backend_interface_t){ .kbd_event = 0 } };

/* generated */
%typemap (python, struct-out) struct gfx_console_backend_interface {
#define struct_gfx_console_backend_interface_t_type_p struct_gfx_console_backend_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_gfx_console_backend_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct gfx_console_backend_interface {
#define struct_gfx_console_backend_interface_t_type_p struct_gfx_console_backend_interface_t_type_p
	struct gfx_console_backend_interface *src = MM_MALLOC(1, struct gfx_console_backend_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_gfx_console_backend_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct gfx_console_backend_interface * {
#define struct_gfx_console_backend_interface_t_type_p struct_gfx_console_backend_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_gfx_console_backend_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct gfx_console_backend_interface * {
#define struct_gfx_console_backend_interface_t_type_p struct_gfx_console_backend_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_gfx_console_backend_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct gfx_con_interface {
#define struct_gfx_con_interface_t_type_p struct_gfx_con_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_gfx_con_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a gfx_con_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct gfx_con_interface *, $source);
} default_value { (const gfx_con_interface_t){ .set_color = 0 } };

/* generated */
%typemap (python, struct-out) struct gfx_con_interface {
#define struct_gfx_con_interface_t_type_p struct_gfx_con_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_gfx_con_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct gfx_con_interface {
#define struct_gfx_con_interface_t_type_p struct_gfx_con_interface_t_type_p
	struct gfx_con_interface *src = MM_MALLOC(1, struct gfx_con_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_gfx_con_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct gfx_con_interface * {
#define struct_gfx_con_interface_t_type_p struct_gfx_con_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_gfx_con_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct gfx_con_interface * {
#define struct_gfx_con_interface_t_type_p struct_gfx_con_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_gfx_con_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct gfx_break_interface {
#define struct_gfx_break_interface_t_type_p struct_gfx_break_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_gfx_break_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a gfx_break_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct gfx_break_interface *, $source);
} default_value { (const gfx_break_interface_t){ .store = 0 } };

/* generated */
%typemap (python, struct-out) struct gfx_break_interface {
#define struct_gfx_break_interface_t_type_p struct_gfx_break_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_gfx_break_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct gfx_break_interface {
#define struct_gfx_break_interface_t_type_p struct_gfx_break_interface_t_type_p
	struct gfx_break_interface *src = MM_MALLOC(1, struct gfx_break_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_gfx_break_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct gfx_break_interface * {
#define struct_gfx_break_interface_t_type_p struct_gfx_break_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_gfx_break_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct gfx_break_interface * {
#define struct_gfx_break_interface_t_type_p struct_gfx_break_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_gfx_break_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct gbic_transceiver_interface {
#define struct_gbic_transceiver_interface_t_type_p struct_gbic_transceiver_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_gbic_transceiver_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a gbic_transceiver_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct gbic_transceiver_interface *, $source);
} default_value { (const gbic_transceiver_interface_t){ .read_mod_def = 0 } };

/* generated */
%typemap (python, struct-out) struct gbic_transceiver_interface {
#define struct_gbic_transceiver_interface_t_type_p struct_gbic_transceiver_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_gbic_transceiver_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct gbic_transceiver_interface {
#define struct_gbic_transceiver_interface_t_type_p struct_gbic_transceiver_interface_t_type_p
	struct gbic_transceiver_interface *src = MM_MALLOC(1, struct gbic_transceiver_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_gbic_transceiver_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct gbic_transceiver_interface * {
#define struct_gbic_transceiver_interface_t_type_p struct_gbic_transceiver_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_gbic_transceiver_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct gbic_transceiver_interface * {
#define struct_gbic_transceiver_interface_t_type_p struct_gbic_transceiver_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_gbic_transceiver_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct frequency_listener_interface {
#define struct_frequency_listener_interface_t_type_p struct_frequency_listener_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_frequency_listener_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a frequency_listener_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct frequency_listener_interface *, $source);
} default_value { (const frequency_listener_interface_t){ .set = 0 } };

/* generated */
%typemap (python, struct-out) struct frequency_listener_interface {
#define struct_frequency_listener_interface_t_type_p struct_frequency_listener_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_frequency_listener_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct frequency_listener_interface {
#define struct_frequency_listener_interface_t_type_p struct_frequency_listener_interface_t_type_p
	struct frequency_listener_interface *src = MM_MALLOC(1, struct frequency_listener_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_frequency_listener_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct frequency_listener_interface * {
#define struct_frequency_listener_interface_t_type_p struct_frequency_listener_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_frequency_listener_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct frequency_listener_interface * {
#define struct_frequency_listener_interface_t_type_p struct_frequency_listener_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_frequency_listener_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct frequency_interface {
#define struct_frequency_interface_t_type_p struct_frequency_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_frequency_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a frequency_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct frequency_interface *, $source);
} default_value { (const frequency_interface_t){ .get = 0 } };

/* generated */
%typemap (python, struct-out) struct frequency_interface {
#define struct_frequency_interface_t_type_p struct_frequency_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_frequency_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct frequency_interface {
#define struct_frequency_interface_t_type_p struct_frequency_interface_t_type_p
	struct frequency_interface *src = MM_MALLOC(1, struct frequency_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_frequency_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct frequency_interface * {
#define struct_frequency_interface_t_type_p struct_frequency_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_frequency_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct frequency_interface * {
#define struct_frequency_interface_t_type_p struct_frequency_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_frequency_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct freerun_interface {
#define struct_freerun_interface_t_type_p struct_freerun_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_freerun_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a freerun_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct freerun_interface *, $source);
} default_value { (const freerun_interface_t){ .enabled = 0 } };

/* generated */
%typemap (python, struct-out) struct freerun_interface {
#define struct_freerun_interface_t_type_p struct_freerun_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_freerun_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct freerun_interface {
#define struct_freerun_interface_t_type_p struct_freerun_interface_t_type_p
	struct freerun_interface *src = MM_MALLOC(1, struct freerun_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_freerun_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct freerun_interface * {
#define struct_freerun_interface_t_type_p struct_freerun_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_freerun_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct freerun_interface * {
#define struct_freerun_interface_t_type_p struct_freerun_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_freerun_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct forward_rewrite_endpoint {
#define struct_forward_rewrite_endpoint_t_type_p struct_forward_rewrite_endpoint_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_forward_rewrite_endpoint_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a forward_rewrite_endpoint_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(forward_rewrite_endpoint_t *, $source);
} default_value { (const forward_rewrite_endpoint_t){ .ip_addr = { 0 } } };

/* generated */
%typemap (python, struct-out) struct forward_rewrite_endpoint {
#define struct_forward_rewrite_endpoint_t_type_p struct_forward_rewrite_endpoint_t_type_p
$target = new_opaque_object(&$source, false, false, struct_forward_rewrite_endpoint_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct forward_rewrite_endpoint {
#define struct_forward_rewrite_endpoint_t_type_p struct_forward_rewrite_endpoint_t_type_p
	forward_rewrite_endpoint_t *src = MM_MALLOC(1, struct forward_rewrite_endpoint);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_forward_rewrite_endpoint_t_type_p, NULL);
};

/* generated */
%typemap (python, out) forward_rewrite_endpoint_t * {
#define struct_forward_rewrite_endpoint_t_type_p struct_forward_rewrite_endpoint_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_forward_rewrite_endpoint_t_type_p, NULL);
};

/* generated */
%typemap (python, in) forward_rewrite_endpoint_t * {
#define struct_forward_rewrite_endpoint_t_type_p struct_forward_rewrite_endpoint_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_forward_rewrite_endpoint_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct forward_destination_rewriter_interface {
#define struct_forward_destination_rewriter_interface_t_type_p struct_forward_destination_rewriter_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_forward_destination_rewriter_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a forward_destination_rewriter_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct forward_destination_rewriter_interface *, $source);
} default_value { (const forward_destination_rewriter_interface_t){ .get_new_destination = 0 } };

/* generated */
%typemap (python, struct-out) struct forward_destination_rewriter_interface {
#define struct_forward_destination_rewriter_interface_t_type_p struct_forward_destination_rewriter_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_forward_destination_rewriter_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct forward_destination_rewriter_interface {
#define struct_forward_destination_rewriter_interface_t_type_p struct_forward_destination_rewriter_interface_t_type_p
	struct forward_destination_rewriter_interface *src = MM_MALLOC(1, struct forward_destination_rewriter_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_forward_destination_rewriter_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct forward_destination_rewriter_interface * {
#define struct_forward_destination_rewriter_interface_t_type_p struct_forward_destination_rewriter_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_forward_destination_rewriter_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct forward_destination_rewriter_interface * {
#define struct_forward_destination_rewriter_interface_t_type_p struct_forward_destination_rewriter_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_forward_destination_rewriter_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct follower_agent_interface {
#define struct_follower_agent_interface_t_type_p struct_follower_agent_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_follower_agent_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a follower_agent_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct follower_agent_interface *, $source);
} default_value { (const follower_agent_interface_t){ .accept = 0 } };

/* generated */
%typemap (python, struct-out) struct follower_agent_interface {
#define struct_follower_agent_interface_t_type_p struct_follower_agent_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_follower_agent_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct follower_agent_interface {
#define struct_follower_agent_interface_t_type_p struct_follower_agent_interface_t_type_p
	struct follower_agent_interface *src = MM_MALLOC(1, struct follower_agent_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_follower_agent_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct follower_agent_interface * {
#define struct_follower_agent_interface_t_type_p struct_follower_agent_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_follower_agent_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct follower_agent_interface * {
#define struct_follower_agent_interface_t_type_p struct_follower_agent_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_follower_agent_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct fmn_station_control_v2_interface {
#define struct_fmn_station_control_v2_interface_t_type_p struct_fmn_station_control_v2_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_fmn_station_control_v2_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a fmn_station_control_v2_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct fmn_station_control_v2_interface *, $source);
} default_value { (const fmn_station_control_v2_interface_t){ .send_message = 0 } };

/* generated */
%typemap (python, struct-out) struct fmn_station_control_v2_interface {
#define struct_fmn_station_control_v2_interface_t_type_p struct_fmn_station_control_v2_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_fmn_station_control_v2_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct fmn_station_control_v2_interface {
#define struct_fmn_station_control_v2_interface_t_type_p struct_fmn_station_control_v2_interface_t_type_p
	struct fmn_station_control_v2_interface *src = MM_MALLOC(1, struct fmn_station_control_v2_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_fmn_station_control_v2_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct fmn_station_control_v2_interface * {
#define struct_fmn_station_control_v2_interface_t_type_p struct_fmn_station_control_v2_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_fmn_station_control_v2_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct fmn_station_control_v2_interface * {
#define struct_fmn_station_control_v2_interface_t_type_p struct_fmn_station_control_v2_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_fmn_station_control_v2_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct fmn_station_control_interface {
#define struct_fmn_station_control_interface_t_type_p struct_fmn_station_control_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_fmn_station_control_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a fmn_station_control_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct fmn_station_control_interface *, $source);
} default_value { (const fmn_station_control_interface_t){ .send_message = 0 } };

/* generated */
%typemap (python, struct-out) struct fmn_station_control_interface {
#define struct_fmn_station_control_interface_t_type_p struct_fmn_station_control_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_fmn_station_control_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct fmn_station_control_interface {
#define struct_fmn_station_control_interface_t_type_p struct_fmn_station_control_interface_t_type_p
	struct fmn_station_control_interface *src = MM_MALLOC(1, struct fmn_station_control_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_fmn_station_control_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct fmn_station_control_interface * {
#define struct_fmn_station_control_interface_t_type_p struct_fmn_station_control_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_fmn_station_control_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct fmn_station_control_interface * {
#define struct_fmn_station_control_interface_t_type_p struct_fmn_station_control_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_fmn_station_control_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct firewire_device_interface {
#define struct_firewire_device_interface_t_type_p struct_firewire_device_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_firewire_device_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a firewire_device_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct firewire_device_interface *, $source);
} default_value { (const firewire_device_interface_t){ .transfer = 0 } };

/* generated */
%typemap (python, struct-out) struct firewire_device_interface {
#define struct_firewire_device_interface_t_type_p struct_firewire_device_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_firewire_device_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct firewire_device_interface {
#define struct_firewire_device_interface_t_type_p struct_firewire_device_interface_t_type_p
	struct firewire_device_interface *src = MM_MALLOC(1, struct firewire_device_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_firewire_device_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct firewire_device_interface * {
#define struct_firewire_device_interface_t_type_p struct_firewire_device_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_firewire_device_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct firewire_device_interface * {
#define struct_firewire_device_interface_t_type_p struct_firewire_device_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_firewire_device_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct firewire_bus_interface {
#define struct_firewire_bus_interface_t_type_p struct_firewire_bus_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_firewire_bus_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a firewire_bus_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct firewire_bus_interface *, $source);
} default_value { (const firewire_bus_interface_t){ .connect_device = 0 } };

/* generated */
%typemap (python, struct-out) struct firewire_bus_interface {
#define struct_firewire_bus_interface_t_type_p struct_firewire_bus_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_firewire_bus_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct firewire_bus_interface {
#define struct_firewire_bus_interface_t_type_p struct_firewire_bus_interface_t_type_p
	struct firewire_bus_interface *src = MM_MALLOC(1, struct firewire_bus_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_firewire_bus_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct firewire_bus_interface * {
#define struct_firewire_bus_interface_t_type_p struct_firewire_bus_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_firewire_bus_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct firewire_bus_interface * {
#define struct_firewire_bus_interface_t_type_p struct_firewire_bus_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_firewire_bus_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct external_connection_events_interface {
#define struct_external_connection_events_interface_t_type_p struct_external_connection_events_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_external_connection_events_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an external_connection_events_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct external_connection_events_interface *, $source);
} default_value { (const external_connection_events_interface_t){ .on_accept = 0 } };

/* generated */
%typemap (python, struct-out) struct external_connection_events_interface {
#define struct_external_connection_events_interface_t_type_p struct_external_connection_events_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_external_connection_events_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct external_connection_events_interface {
#define struct_external_connection_events_interface_t_type_p struct_external_connection_events_interface_t_type_p
	struct external_connection_events_interface *src = MM_MALLOC(1, struct external_connection_events_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_external_connection_events_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct external_connection_events_interface * {
#define struct_external_connection_events_interface_t_type_p struct_external_connection_events_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_external_connection_events_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct external_connection_events_interface * {
#define struct_external_connection_events_interface_t_type_p struct_external_connection_events_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_external_connection_events_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct external_connection_ctl_interface {
#define struct_external_connection_ctl_interface_t_type_p struct_external_connection_ctl_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_external_connection_ctl_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an external_connection_ctl_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct external_connection_ctl_interface *, $source);
} default_value { (const external_connection_ctl_interface_t){ .accept = 0 } };

/* generated */
%typemap (python, struct-out) struct external_connection_ctl_interface {
#define struct_external_connection_ctl_interface_t_type_p struct_external_connection_ctl_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_external_connection_ctl_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct external_connection_ctl_interface {
#define struct_external_connection_ctl_interface_t_type_p struct_external_connection_ctl_interface_t_type_p
	struct external_connection_ctl_interface *src = MM_MALLOC(1, struct external_connection_ctl_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_external_connection_ctl_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct external_connection_ctl_interface * {
#define struct_external_connection_ctl_interface_t_type_p struct_external_connection_ctl_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_external_connection_ctl_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct external_connection_ctl_interface * {
#define struct_external_connection_ctl_interface_t_type_p struct_external_connection_ctl_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_external_connection_ctl_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct extended_serial_interface {
#define struct_extended_serial_interface_t_type_p struct_extended_serial_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_extended_serial_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an extended_serial_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct extended_serial_interface *, $source);
} default_value { (const extended_serial_interface_t){ .write_at = 0 } };

/* generated */
%typemap (python, struct-out) struct extended_serial_interface {
#define struct_extended_serial_interface_t_type_p struct_extended_serial_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_extended_serial_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct extended_serial_interface {
#define struct_extended_serial_interface_t_type_p struct_extended_serial_interface_t_type_p
	struct extended_serial_interface *src = MM_MALLOC(1, struct extended_serial_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_extended_serial_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct extended_serial_interface * {
#define struct_extended_serial_interface_t_type_p struct_extended_serial_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_extended_serial_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct extended_serial_interface * {
#define struct_extended_serial_interface_t_type_p struct_extended_serial_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_extended_serial_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct execute_control_interface {
#define struct_execute_control_interface_t_type_p struct_execute_control_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_execute_control_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an execute_control_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct execute_control_interface *, $source);
} default_value { (const execute_control_interface_t){ .message_pending = 0 } };

/* generated */
%typemap (python, struct-out) struct execute_control_interface {
#define struct_execute_control_interface_t_type_p struct_execute_control_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_execute_control_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct execute_control_interface {
#define struct_execute_control_interface_t_type_p struct_execute_control_interface_t_type_p
	struct execute_control_interface *src = MM_MALLOC(1, struct execute_control_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_execute_control_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct execute_control_interface * {
#define struct_execute_control_interface_t_type_p struct_execute_control_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_execute_control_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct execute_control_interface * {
#define struct_execute_control_interface_t_type_p struct_execute_control_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_execute_control_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct execute_interface {
#define struct_execute_interface_t_type_p struct_execute_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_execute_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an execute_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct execute_interface *, $source);
} default_value { (const execute_interface_t){ .run = 0 } };

/* generated */
%typemap (python, struct-out) struct execute_interface {
#define struct_execute_interface_t_type_p struct_execute_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_execute_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct execute_interface {
#define struct_execute_interface_t_type_p struct_execute_interface_t_type_p
	struct execute_interface *src = MM_MALLOC(1, struct execute_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_execute_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct execute_interface * {
#define struct_execute_interface_t_type_p struct_execute_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_execute_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct execute_interface * {
#define struct_execute_interface_t_type_p struct_execute_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_execute_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct exec_trace_interface {
#define struct_exec_trace_interface_t_type_p struct_exec_trace_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_exec_trace_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an exec_trace_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct exec_trace_interface *, $source);
} default_value { (const exec_trace_interface_t){ .register_tracer = 0 } };

/* generated */
%typemap (python, struct-out) struct exec_trace_interface {
#define struct_exec_trace_interface_t_type_p struct_exec_trace_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_exec_trace_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct exec_trace_interface {
#define struct_exec_trace_interface_t_type_p struct_exec_trace_interface_t_type_p
	struct exec_trace_interface *src = MM_MALLOC(1, struct exec_trace_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_exec_trace_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct exec_trace_interface * {
#define struct_exec_trace_interface_t_type_p struct_exec_trace_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_exec_trace_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct exec_trace_interface * {
#define struct_exec_trace_interface_t_type_p struct_exec_trace_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_exec_trace_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct exception_interface {
#define struct_exception_interface_t_type_p struct_exception_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_exception_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an exception_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct exception_interface *, $source);
} default_value { (const exception_interface_t){ .get_number = 0 } };

/* generated */
%typemap (python, struct-out) struct exception_interface {
#define struct_exception_interface_t_type_p struct_exception_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_exception_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct exception_interface {
#define struct_exception_interface_t_type_p struct_exception_interface_t_type_p
	struct exception_interface *src = MM_MALLOC(1, struct exception_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_exception_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct exception_interface * {
#define struct_exception_interface_t_type_p struct_exception_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_exception_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct exception_interface * {
#define struct_exception_interface_t_type_p struct_exception_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_exception_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct event_handler_interface {
#define struct_event_handler_interface_t_type_p struct_event_handler_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_event_handler_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an event_handler_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct event_handler_interface *, $source);
} default_value { (const event_handler_interface_t){ .handle_event = 0 } };

/* generated */
%typemap (python, struct-out) struct event_handler_interface {
#define struct_event_handler_interface_t_type_p struct_event_handler_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_event_handler_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct event_handler_interface {
#define struct_event_handler_interface_t_type_p struct_event_handler_interface_t_type_p
	struct event_handler_interface *src = MM_MALLOC(1, struct event_handler_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_event_handler_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct event_handler_interface * {
#define struct_event_handler_interface_t_type_p struct_event_handler_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_event_handler_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct event_handler_interface * {
#define struct_event_handler_interface_t_type_p struct_event_handler_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_event_handler_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct event_delta_interface {
#define struct_event_delta_interface_t_type_p struct_event_delta_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_event_delta_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an event_delta_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct event_delta_interface *, $source);
} default_value { (const event_delta_interface_t){ .set_delta = 0 } };

/* generated */
%typemap (python, struct-out) struct event_delta_interface {
#define struct_event_delta_interface_t_type_p struct_event_delta_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_event_delta_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct event_delta_interface {
#define struct_event_delta_interface_t_type_p struct_event_delta_interface_t_type_p
	struct event_delta_interface *src = MM_MALLOC(1, struct event_delta_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_event_delta_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct event_delta_interface * {
#define struct_event_delta_interface_t_type_p struct_event_delta_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_event_delta_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct event_delta_interface * {
#define struct_event_delta_interface_t_type_p struct_event_delta_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_event_delta_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct ethernet_vlan_snoop_interface {
#define struct_ethernet_vlan_snoop_interface_t_type_p struct_ethernet_vlan_snoop_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_ethernet_vlan_snoop_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an ethernet_vlan_snoop_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct ethernet_vlan_snoop_interface *, $source);
} default_value { (const ethernet_vlan_snoop_interface_t){ .attach = 0 } };

/* generated */
%typemap (python, struct-out) struct ethernet_vlan_snoop_interface {
#define struct_ethernet_vlan_snoop_interface_t_type_p struct_ethernet_vlan_snoop_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_ethernet_vlan_snoop_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct ethernet_vlan_snoop_interface {
#define struct_ethernet_vlan_snoop_interface_t_type_p struct_ethernet_vlan_snoop_interface_t_type_p
	struct ethernet_vlan_snoop_interface *src = MM_MALLOC(1, struct ethernet_vlan_snoop_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_ethernet_vlan_snoop_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct ethernet_vlan_snoop_interface * {
#define struct_ethernet_vlan_snoop_interface_t_type_p struct_ethernet_vlan_snoop_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_ethernet_vlan_snoop_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct ethernet_vlan_snoop_interface * {
#define struct_ethernet_vlan_snoop_interface_t_type_p struct_ethernet_vlan_snoop_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_ethernet_vlan_snoop_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct ethernet_snoop_interface {
#define struct_ethernet_snoop_interface_t_type_p struct_ethernet_snoop_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_ethernet_snoop_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an ethernet_snoop_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct ethernet_snoop_interface *, $source);
} default_value { (const ethernet_snoop_interface_t){ .attach = 0 } };

/* generated */
%typemap (python, struct-out) struct ethernet_snoop_interface {
#define struct_ethernet_snoop_interface_t_type_p struct_ethernet_snoop_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_ethernet_snoop_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct ethernet_snoop_interface {
#define struct_ethernet_snoop_interface_t_type_p struct_ethernet_snoop_interface_t_type_p
	struct ethernet_snoop_interface *src = MM_MALLOC(1, struct ethernet_snoop_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_ethernet_snoop_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct ethernet_snoop_interface * {
#define struct_ethernet_snoop_interface_t_type_p struct_ethernet_snoop_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_ethernet_snoop_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct ethernet_snoop_interface * {
#define struct_ethernet_snoop_interface_t_type_p struct_ethernet_snoop_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_ethernet_snoop_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct ethernet_probe_interface {
#define struct_ethernet_probe_interface_t_type_p struct_ethernet_probe_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_ethernet_probe_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an ethernet_probe_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct ethernet_probe_interface *, $source);
} default_value { (const ethernet_probe_interface_t){ .attach_snooper = 0 } };

/* generated */
%typemap (python, struct-out) struct ethernet_probe_interface {
#define struct_ethernet_probe_interface_t_type_p struct_ethernet_probe_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_ethernet_probe_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct ethernet_probe_interface {
#define struct_ethernet_probe_interface_t_type_p struct_ethernet_probe_interface_t_type_p
	struct ethernet_probe_interface *src = MM_MALLOC(1, struct ethernet_probe_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_ethernet_probe_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct ethernet_probe_interface * {
#define struct_ethernet_probe_interface_t_type_p struct_ethernet_probe_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_ethernet_probe_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct ethernet_probe_interface * {
#define struct_ethernet_probe_interface_t_type_p struct_ethernet_probe_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_ethernet_probe_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct ethernet_common_interface {
#define struct_ethernet_common_interface_t_type_p struct_ethernet_common_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_ethernet_common_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an ethernet_common_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct ethernet_common_interface *, $source);
} default_value { (const ethernet_common_interface_t){ .frame = 0 } };

/* generated */
%typemap (python, struct-out) struct ethernet_common_interface {
#define struct_ethernet_common_interface_t_type_p struct_ethernet_common_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_ethernet_common_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct ethernet_common_interface {
#define struct_ethernet_common_interface_t_type_p struct_ethernet_common_interface_t_type_p
	struct ethernet_common_interface *src = MM_MALLOC(1, struct ethernet_common_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_ethernet_common_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct ethernet_common_interface * {
#define struct_ethernet_common_interface_t_type_p struct_ethernet_common_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_ethernet_common_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct ethernet_common_interface * {
#define struct_ethernet_common_interface_t_type_p struct_ethernet_common_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_ethernet_common_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct ethernet_cable_interface {
#define struct_ethernet_cable_interface_t_type_p struct_ethernet_cable_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_ethernet_cable_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an ethernet_cable_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct ethernet_cable_interface *, $source);
} default_value { (const ethernet_cable_interface_t){ .link_status = 0 } };

/* generated */
%typemap (python, struct-out) struct ethernet_cable_interface {
#define struct_ethernet_cable_interface_t_type_p struct_ethernet_cable_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_ethernet_cable_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct ethernet_cable_interface {
#define struct_ethernet_cable_interface_t_type_p struct_ethernet_cable_interface_t_type_p
	struct ethernet_cable_interface *src = MM_MALLOC(1, struct ethernet_cable_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_ethernet_cable_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct ethernet_cable_interface * {
#define struct_ethernet_cable_interface_t_type_p struct_ethernet_cable_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_ethernet_cable_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct ethernet_cable_interface * {
#define struct_ethernet_cable_interface_t_type_p struct_ethernet_cable_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_ethernet_cable_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct dummy_memory_page_interface {
#define struct_dummy_memory_page_interface_t_type_p struct_dummy_memory_page_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_dummy_memory_page_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a dummy_memory_page_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct dummy_memory_page_interface *, $source);
} default_value { (const dummy_memory_page_interface_t){ .dummy = 0 } };

/* generated */
%typemap (python, struct-out) struct dummy_memory_page_interface {
#define struct_dummy_memory_page_interface_t_type_p struct_dummy_memory_page_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_dummy_memory_page_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct dummy_memory_page_interface {
#define struct_dummy_memory_page_interface_t_type_p struct_dummy_memory_page_interface_t_type_p
	struct dummy_memory_page_interface *src = MM_MALLOC(1, struct dummy_memory_page_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_dummy_memory_page_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct dummy_memory_page_interface * {
#define struct_dummy_memory_page_interface_t_type_p struct_dummy_memory_page_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_dummy_memory_page_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct dummy_memory_page_interface * {
#define struct_dummy_memory_page_interface_t_type_p struct_dummy_memory_page_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_dummy_memory_page_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct dist_control_node_interface {
#define struct_dist_control_node_interface_t_type_p struct_dist_control_node_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_dist_control_node_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a dist_control_node_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct dist_control_node_interface *, $source);
} default_value { (const dist_control_node_interface_t){ .flush_async_channel_down = 0 } };

/* generated */
%typemap (python, struct-out) struct dist_control_node_interface {
#define struct_dist_control_node_interface_t_type_p struct_dist_control_node_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_dist_control_node_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct dist_control_node_interface {
#define struct_dist_control_node_interface_t_type_p struct_dist_control_node_interface_t_type_p
	struct dist_control_node_interface *src = MM_MALLOC(1, struct dist_control_node_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_dist_control_node_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct dist_control_node_interface * {
#define struct_dist_control_node_interface_t_type_p struct_dist_control_node_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_dist_control_node_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct dist_control_node_interface * {
#define struct_dist_control_node_interface_t_type_p struct_dist_control_node_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_dist_control_node_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct dist_control_interface {
#define struct_dist_control_interface_t_type_p struct_dist_control_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_dist_control_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a dist_control_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct dist_control_interface *, $source);
} default_value { (const dist_control_interface_t){ .initiate_async_flush = 0 } };

/* generated */
%typemap (python, struct-out) struct dist_control_interface {
#define struct_dist_control_interface_t_type_p struct_dist_control_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_dist_control_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct dist_control_interface {
#define struct_dist_control_interface_t_type_p struct_dist_control_interface_t_type_p
	struct dist_control_interface *src = MM_MALLOC(1, struct dist_control_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_dist_control_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct dist_control_interface * {
#define struct_dist_control_interface_t_type_p struct_dist_control_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_dist_control_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct dist_control_interface * {
#define struct_dist_control_interface_t_type_p struct_dist_control_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_dist_control_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct disk_component_interface {
#define struct_disk_component_interface_t_type_p struct_disk_component_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_disk_component_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a disk_component_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct disk_component_interface *, $source);
} default_value { (const disk_component_interface_t){ .size = 0 } };

/* generated */
%typemap (python, struct-out) struct disk_component_interface {
#define struct_disk_component_interface_t_type_p struct_disk_component_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_disk_component_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct disk_component_interface {
#define struct_disk_component_interface_t_type_p struct_disk_component_interface_t_type_p
	struct disk_component_interface *src = MM_MALLOC(1, struct disk_component_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_disk_component_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct disk_component_interface * {
#define struct_disk_component_interface_t_type_p struct_disk_component_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_disk_component_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct disk_component_interface * {
#define struct_disk_component_interface_t_type_p struct_disk_component_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_disk_component_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct direct_memory_update_interface {
#define struct_direct_memory_update_interface_t_type_p struct_direct_memory_update_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_direct_memory_update_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a direct_memory_update_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct direct_memory_update_interface *, $source);
} default_value { (const direct_memory_update_interface_t){ .release = 0 } };

/* generated */
%typemap (python, struct-out) struct direct_memory_update_interface {
#define struct_direct_memory_update_interface_t_type_p struct_direct_memory_update_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_direct_memory_update_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct direct_memory_update_interface {
#define struct_direct_memory_update_interface_t_type_p struct_direct_memory_update_interface_t_type_p
	struct direct_memory_update_interface *src = MM_MALLOC(1, struct direct_memory_update_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_direct_memory_update_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct direct_memory_update_interface * {
#define struct_direct_memory_update_interface_t_type_p struct_direct_memory_update_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_direct_memory_update_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct direct_memory_update_interface * {
#define struct_direct_memory_update_interface_t_type_p struct_direct_memory_update_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_direct_memory_update_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct __anonymous141 {
#define struct_direct_memory_tags_t_type_p struct_direct_memory_tags_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_direct_memory_tags_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a direct_memory_tags_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(direct_memory_tags_t *, $source);
} default_value { (const direct_memory_tags_t){ .len = 0 } };

/* generated */
%typemap (python, struct-out) struct __anonymous141 {
#define struct_direct_memory_tags_t_type_p struct_direct_memory_tags_t_type_p
$target = new_opaque_object(&$source, false, false, struct_direct_memory_tags_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous141 {
#define struct_direct_memory_tags_t_type_p struct_direct_memory_tags_t_type_p
	direct_memory_tags_t *src = MM_MALLOC(1, direct_memory_tags_t);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_direct_memory_tags_t_type_p, NULL);
};

/* generated */
%typemap (python, out) direct_memory_tags_t * {
#define struct_direct_memory_tags_t_type_p struct_direct_memory_tags_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_direct_memory_tags_t_type_p, NULL);
};

/* generated */
%typemap (python, in) direct_memory_tags_t * {
#define struct_direct_memory_tags_t_type_p struct_direct_memory_tags_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_direct_memory_tags_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct direct_memory_tags_interface {
#define struct_direct_memory_tags_interface_t_type_p struct_direct_memory_tags_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_direct_memory_tags_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a direct_memory_tags_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct direct_memory_tags_interface *, $source);
} default_value { (const direct_memory_tags_interface_t){ .get_tags_data = 0 } };

/* generated */
%typemap (python, struct-out) struct direct_memory_tags_interface {
#define struct_direct_memory_tags_interface_t_type_p struct_direct_memory_tags_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_direct_memory_tags_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct direct_memory_tags_interface {
#define struct_direct_memory_tags_interface_t_type_p struct_direct_memory_tags_interface_t_type_p
	struct direct_memory_tags_interface *src = MM_MALLOC(1, struct direct_memory_tags_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_direct_memory_tags_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct direct_memory_tags_interface * {
#define struct_direct_memory_tags_interface_t_type_p struct_direct_memory_tags_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_direct_memory_tags_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct direct_memory_tags_interface * {
#define struct_direct_memory_tags_interface_t_type_p struct_direct_memory_tags_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_direct_memory_tags_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct direct_memory_lookup_v2_interface {
#define struct_direct_memory_lookup_v2_interface_t_type_p struct_direct_memory_lookup_v2_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_direct_memory_lookup_v2_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a direct_memory_lookup_v2_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct direct_memory_lookup_v2_interface *, $source);
} default_value { (const direct_memory_lookup_v2_interface_t){ .lookup = 0 } };

/* generated */
%typemap (python, struct-out) struct direct_memory_lookup_v2_interface {
#define struct_direct_memory_lookup_v2_interface_t_type_p struct_direct_memory_lookup_v2_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_direct_memory_lookup_v2_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct direct_memory_lookup_v2_interface {
#define struct_direct_memory_lookup_v2_interface_t_type_p struct_direct_memory_lookup_v2_interface_t_type_p
	struct direct_memory_lookup_v2_interface *src = MM_MALLOC(1, struct direct_memory_lookup_v2_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_direct_memory_lookup_v2_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct direct_memory_lookup_v2_interface * {
#define struct_direct_memory_lookup_v2_interface_t_type_p struct_direct_memory_lookup_v2_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_direct_memory_lookup_v2_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct direct_memory_lookup_v2_interface * {
#define struct_direct_memory_lookup_v2_interface_t_type_p struct_direct_memory_lookup_v2_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_direct_memory_lookup_v2_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct __anonymous142 {
#define struct_direct_memory_lookup_t_type_p struct_direct_memory_lookup_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_direct_memory_lookup_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a direct_memory_lookup_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(direct_memory_lookup_t *, $source);
} default_value { (const direct_memory_lookup_t){ .target = NULL } };

/* generated */
%typemap (python, struct-out) struct __anonymous142 {
#define struct_direct_memory_lookup_t_type_p struct_direct_memory_lookup_t_type_p
$target = new_opaque_object(&$source, false, false, struct_direct_memory_lookup_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous142 {
#define struct_direct_memory_lookup_t_type_p struct_direct_memory_lookup_t_type_p
	direct_memory_lookup_t *src = MM_MALLOC(1, direct_memory_lookup_t);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_direct_memory_lookup_t_type_p, NULL);
};

/* generated */
%typemap (python, out) direct_memory_lookup_t * {
#define struct_direct_memory_lookup_t_type_p struct_direct_memory_lookup_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_direct_memory_lookup_t_type_p, NULL);
};

/* generated */
%typemap (python, in) direct_memory_lookup_t * {
#define struct_direct_memory_lookup_t_type_p struct_direct_memory_lookup_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_direct_memory_lookup_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct direct_memory_lookup_interface {
#define struct_direct_memory_lookup_interface_t_type_p struct_direct_memory_lookup_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_direct_memory_lookup_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a direct_memory_lookup_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct direct_memory_lookup_interface *, $source);
} default_value { (const direct_memory_lookup_interface_t){ .lookup = 0 } };

/* generated */
%typemap (python, struct-out) struct direct_memory_lookup_interface {
#define struct_direct_memory_lookup_interface_t_type_p struct_direct_memory_lookup_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_direct_memory_lookup_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct direct_memory_lookup_interface {
#define struct_direct_memory_lookup_interface_t_type_p struct_direct_memory_lookup_interface_t_type_p
	struct direct_memory_lookup_interface *src = MM_MALLOC(1, struct direct_memory_lookup_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_direct_memory_lookup_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct direct_memory_lookup_interface * {
#define struct_direct_memory_lookup_interface_t_type_p struct_direct_memory_lookup_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_direct_memory_lookup_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct direct_memory_lookup_interface * {
#define struct_direct_memory_lookup_interface_t_type_p struct_direct_memory_lookup_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_direct_memory_lookup_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct direct_memory_flush_interface {
#define struct_direct_memory_flush_interface_t_type_p struct_direct_memory_flush_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_direct_memory_flush_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a direct_memory_flush_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct direct_memory_flush_interface *, $source);
} default_value { (const direct_memory_flush_interface_t){ .revoke = 0 } };

/* generated */
%typemap (python, struct-out) struct direct_memory_flush_interface {
#define struct_direct_memory_flush_interface_t_type_p struct_direct_memory_flush_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_direct_memory_flush_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct direct_memory_flush_interface {
#define struct_direct_memory_flush_interface_t_type_p struct_direct_memory_flush_interface_t_type_p
	struct direct_memory_flush_interface *src = MM_MALLOC(1, struct direct_memory_flush_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_direct_memory_flush_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct direct_memory_flush_interface * {
#define struct_direct_memory_flush_interface_t_type_p struct_direct_memory_flush_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_direct_memory_flush_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct direct_memory_flush_interface * {
#define struct_direct_memory_flush_interface_t_type_p struct_direct_memory_flush_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_direct_memory_flush_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct __anonymous143 {
#define struct_direct_memory_t_type_p struct_direct_memory_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_direct_memory_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a direct_memory_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(direct_memory_t *, $source);
} default_value { (const direct_memory_t){ .permission = 0 } };

/* generated */
%typemap (python, struct-out) struct __anonymous143 {
#define struct_direct_memory_t_type_p struct_direct_memory_t_type_p
$target = new_opaque_object(&$source, false, false, struct_direct_memory_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous143 {
#define struct_direct_memory_t_type_p struct_direct_memory_t_type_p
	direct_memory_t *src = MM_MALLOC(1, direct_memory_t);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_direct_memory_t_type_p, NULL);
};

/* generated */
%typemap (python, out) direct_memory_t * {
#define struct_direct_memory_t_type_p struct_direct_memory_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_direct_memory_t_type_p, NULL);
};

/* generated */
%typemap (python, in) direct_memory_t * {
#define struct_direct_memory_t_type_p struct_direct_memory_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_direct_memory_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct granted_mem {
#define struct_granted_mem_t_type_p struct_granted_mem_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_granted_mem_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a granted_mem_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct granted_mem *, $source);
};

/* generated */
%typemap (python, struct-out) struct granted_mem {
#define struct_granted_mem_t_type_p struct_granted_mem_t_type_p
$target = new_opaque_object(&$source, false, false, struct_granted_mem_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct granted_mem {
#define struct_granted_mem_t_type_p struct_granted_mem_t_type_p
	struct granted_mem *src = MM_MALLOC(1, struct granted_mem);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_granted_mem_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct granted_mem * {
#define struct_granted_mem_t_type_p struct_granted_mem_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_granted_mem_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct granted_mem * {
#define struct_granted_mem_t_type_p struct_granted_mem_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_granted_mem_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct direct_memory_interface {
#define struct_direct_memory_interface_t_type_p struct_direct_memory_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_direct_memory_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a direct_memory_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct direct_memory_interface *, $source);
} default_value { (const direct_memory_interface_t){ .get_handle = 0 } };

/* generated */
%typemap (python, struct-out) struct direct_memory_interface {
#define struct_direct_memory_interface_t_type_p struct_direct_memory_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_direct_memory_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct direct_memory_interface {
#define struct_direct_memory_interface_t_type_p struct_direct_memory_interface_t_type_p
	struct direct_memory_interface *src = MM_MALLOC(1, struct direct_memory_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_direct_memory_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct direct_memory_interface * {
#define struct_direct_memory_interface_t_type_p struct_direct_memory_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_direct_memory_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct direct_memory_interface * {
#define struct_direct_memory_interface_t_type_p struct_direct_memory_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_direct_memory_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct device_identification_interface {
#define struct_device_identification_interface_t_type_p struct_device_identification_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_device_identification_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a device_identification_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct device_identification_interface *, $source);
} default_value { (const device_identification_interface_t){ .get_id = 0 } };

/* generated */
%typemap (python, struct-out) struct device_identification_interface {
#define struct_device_identification_interface_t_type_p struct_device_identification_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_device_identification_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct device_identification_interface {
#define struct_device_identification_interface_t_type_p struct_device_identification_interface_t_type_p
	struct device_identification_interface *src = MM_MALLOC(1, struct device_identification_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_device_identification_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct device_identification_interface * {
#define struct_device_identification_interface_t_type_p struct_device_identification_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_device_identification_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct device_identification_interface * {
#define struct_device_identification_interface_t_type_p struct_device_identification_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_device_identification_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct debug_symbol_file_interface {
#define struct_debug_symbol_file_interface_t_type_p struct_debug_symbol_file_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_debug_symbol_file_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a debug_symbol_file_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct debug_symbol_file_interface *, $source);
} default_value { (const debug_symbol_file_interface_t){ .open_symbol_file = 0 } };

/* generated */
%typemap (python, struct-out) struct debug_symbol_file_interface {
#define struct_debug_symbol_file_interface_t_type_p struct_debug_symbol_file_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_debug_symbol_file_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct debug_symbol_file_interface {
#define struct_debug_symbol_file_interface_t_type_p struct_debug_symbol_file_interface_t_type_p
	struct debug_symbol_file_interface *src = MM_MALLOC(1, struct debug_symbol_file_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_debug_symbol_file_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct debug_symbol_file_interface * {
#define struct_debug_symbol_file_interface_t_type_p struct_debug_symbol_file_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_debug_symbol_file_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct debug_symbol_file_interface * {
#define struct_debug_symbol_file_interface_t_type_p struct_debug_symbol_file_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_debug_symbol_file_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct debug_symbol_interface {
#define struct_debug_symbol_interface_t_type_p struct_debug_symbol_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_debug_symbol_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a debug_symbol_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct debug_symbol_interface *, $source);
} default_value { (const debug_symbol_interface_t){ .address_source = 0 } };

/* generated */
%typemap (python, struct-out) struct debug_symbol_interface {
#define struct_debug_symbol_interface_t_type_p struct_debug_symbol_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_debug_symbol_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct debug_symbol_interface {
#define struct_debug_symbol_interface_t_type_p struct_debug_symbol_interface_t_type_p
	struct debug_symbol_interface *src = MM_MALLOC(1, struct debug_symbol_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_debug_symbol_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct debug_symbol_interface * {
#define struct_debug_symbol_interface_t_type_p struct_debug_symbol_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_debug_symbol_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct debug_symbol_interface * {
#define struct_debug_symbol_interface_t_type_p struct_debug_symbol_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_debug_symbol_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct debug_step_interface {
#define struct_debug_step_interface_t_type_p struct_debug_step_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_debug_step_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a debug_step_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct debug_step_interface *, $source);
} default_value { (const debug_step_interface_t){ .instruction_into = 0 } };

/* generated */
%typemap (python, struct-out) struct debug_step_interface {
#define struct_debug_step_interface_t_type_p struct_debug_step_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_debug_step_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct debug_step_interface {
#define struct_debug_step_interface_t_type_p struct_debug_step_interface_t_type_p
	struct debug_step_interface *src = MM_MALLOC(1, struct debug_step_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_debug_step_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct debug_step_interface * {
#define struct_debug_step_interface_t_type_p struct_debug_step_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_debug_step_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct debug_step_interface * {
#define struct_debug_step_interface_t_type_p struct_debug_step_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_debug_step_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct debug_setup_interface {
#define struct_debug_setup_interface_t_type_p struct_debug_setup_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_debug_setup_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a debug_setup_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct debug_setup_interface *, $source);
} default_value { (const debug_setup_interface_t){ .add_symbol_file = 0 } };

/* generated */
%typemap (python, struct-out) struct debug_setup_interface {
#define struct_debug_setup_interface_t_type_p struct_debug_setup_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_debug_setup_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct debug_setup_interface {
#define struct_debug_setup_interface_t_type_p struct_debug_setup_interface_t_type_p
	struct debug_setup_interface *src = MM_MALLOC(1, struct debug_setup_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_debug_setup_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct debug_setup_interface * {
#define struct_debug_setup_interface_t_type_p struct_debug_setup_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_debug_setup_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct debug_setup_interface * {
#define struct_debug_setup_interface_t_type_p struct_debug_setup_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_debug_setup_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct debug_query_interface {
#define struct_debug_query_interface_t_type_p struct_debug_query_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_debug_query_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a debug_query_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct debug_query_interface *, $source);
} default_value { (const debug_query_interface_t){ .matching_contexts = 0 } };

/* generated */
%typemap (python, struct-out) struct debug_query_interface {
#define struct_debug_query_interface_t_type_p struct_debug_query_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_debug_query_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct debug_query_interface {
#define struct_debug_query_interface_t_type_p struct_debug_query_interface_t_type_p
	struct debug_query_interface *src = MM_MALLOC(1, struct debug_query_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_debug_query_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct debug_query_interface * {
#define struct_debug_query_interface_t_type_p struct_debug_query_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_debug_query_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct debug_query_interface * {
#define struct_debug_query_interface_t_type_p struct_debug_query_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_debug_query_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct debug_notification_interface {
#define struct_debug_notification_interface_t_type_p struct_debug_notification_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_debug_notification_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a debug_notification_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct debug_notification_interface *, $source);
} default_value { (const debug_notification_interface_t){ .notify_context_creation = 0 } };

/* generated */
%typemap (python, struct-out) struct debug_notification_interface {
#define struct_debug_notification_interface_t_type_p struct_debug_notification_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_debug_notification_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct debug_notification_interface {
#define struct_debug_notification_interface_t_type_p struct_debug_notification_interface_t_type_p
	struct debug_notification_interface *src = MM_MALLOC(1, struct debug_notification_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_debug_notification_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct debug_notification_interface * {
#define struct_debug_notification_interface_t_type_p struct_debug_notification_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_debug_notification_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct debug_notification_interface * {
#define struct_debug_notification_interface_t_type_p struct_debug_notification_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_debug_notification_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct datagram_link_interface {
#define struct_datagram_link_interface_t_type_p struct_datagram_link_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_datagram_link_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a datagram_link_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct datagram_link_interface *, $source);
} default_value { (const datagram_link_interface_t){ .receive = 0 } };

/* generated */
%typemap (python, struct-out) struct datagram_link_interface {
#define struct_datagram_link_interface_t_type_p struct_datagram_link_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_datagram_link_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct datagram_link_interface {
#define struct_datagram_link_interface_t_type_p struct_datagram_link_interface_t_type_p
	struct datagram_link_interface *src = MM_MALLOC(1, struct datagram_link_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_datagram_link_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct datagram_link_interface * {
#define struct_datagram_link_interface_t_type_p struct_datagram_link_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_datagram_link_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct datagram_link_interface * {
#define struct_datagram_link_interface_t_type_p struct_datagram_link_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_datagram_link_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct data_profiler_interface {
#define struct_data_profiler_interface_t_type_p struct_data_profiler_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_data_profiler_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a data_profiler_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct data_profiler_interface *, $source);
} default_value { (const data_profiler_interface_t){ .accum_in_range = 0 } };

/* generated */
%typemap (python, struct-out) struct data_profiler_interface {
#define struct_data_profiler_interface_t_type_p struct_data_profiler_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_data_profiler_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct data_profiler_interface {
#define struct_data_profiler_interface_t_type_p struct_data_profiler_interface_t_type_p
	struct data_profiler_interface *src = MM_MALLOC(1, struct data_profiler_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_data_profiler_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct data_profiler_interface * {
#define struct_data_profiler_interface_t_type_p struct_data_profiler_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_data_profiler_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct data_profiler_interface * {
#define struct_data_profiler_interface_t_type_p struct_data_profiler_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_data_profiler_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct clock_instrumentation {
#define struct_cycle_handle_t_type_p struct_cycle_handle_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_cycle_handle_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a cycle_handle_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct clock_instrumentation *, $source);
};

/* generated */
%typemap (python, struct-out) struct clock_instrumentation {
#define struct_cycle_handle_t_type_p struct_cycle_handle_t_type_p
$target = new_opaque_object(&$source, false, false, struct_cycle_handle_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct clock_instrumentation {
#define struct_cycle_handle_t_type_p struct_cycle_handle_t_type_p
	struct clock_instrumentation *src = MM_MALLOC(1, struct clock_instrumentation);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_cycle_handle_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct clock_instrumentation * {
#define struct_cycle_handle_t_type_p struct_cycle_handle_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_cycle_handle_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct clock_instrumentation * {
#define struct_cycle_handle_t_type_p struct_cycle_handle_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_cycle_handle_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct cycle_event_instrumentation_interface {
#define struct_cycle_event_instrumentation_interface_t_type_p struct_cycle_event_instrumentation_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_cycle_event_instrumentation_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a cycle_event_instrumentation_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct cycle_event_instrumentation_interface *, $source);
} default_value { (const cycle_event_instrumentation_interface_t){ .register_cycle_event_cb = 0 } };

/* generated */
%typemap (python, struct-out) struct cycle_event_instrumentation_interface {
#define struct_cycle_event_instrumentation_interface_t_type_p struct_cycle_event_instrumentation_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_cycle_event_instrumentation_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct cycle_event_instrumentation_interface {
#define struct_cycle_event_instrumentation_interface_t_type_p struct_cycle_event_instrumentation_interface_t_type_p
	struct cycle_event_instrumentation_interface *src = MM_MALLOC(1, struct cycle_event_instrumentation_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_cycle_event_instrumentation_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct cycle_event_instrumentation_interface * {
#define struct_cycle_event_instrumentation_interface_t_type_p struct_cycle_event_instrumentation_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_cycle_event_instrumentation_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct cycle_event_instrumentation_interface * {
#define struct_cycle_event_instrumentation_interface_t_type_p struct_cycle_event_instrumentation_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_cycle_event_instrumentation_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct cycle_event_interface {
#define struct_cycle_event_interface_t_type_p struct_cycle_event_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_cycle_event_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a cycle_event_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct cycle_event_interface *, $source);
} default_value { (const cycle_event_interface_t){ .cycles = 0 } };

/* generated */
%typemap (python, struct-out) struct cycle_event_interface {
#define struct_cycle_event_interface_t_type_p struct_cycle_event_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_cycle_event_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct cycle_event_interface {
#define struct_cycle_event_interface_t_type_p struct_cycle_event_interface_t_type_p
	struct cycle_event_interface *src = MM_MALLOC(1, struct cycle_event_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_cycle_event_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct cycle_event_interface * {
#define struct_cycle_event_interface_t_type_p struct_cycle_event_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_cycle_event_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct cycle_event_interface * {
#define struct_cycle_event_interface_t_type_p struct_cycle_event_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_cycle_event_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct cycle_control_interface {
#define struct_cycle_control_interface_t_type_p struct_cycle_control_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_cycle_control_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a cycle_control_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct cycle_control_interface *, $source);
} default_value { (const cycle_control_interface_t){ .stop = 0 } };

/* generated */
%typemap (python, struct-out) struct cycle_control_interface {
#define struct_cycle_control_interface_t_type_p struct_cycle_control_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_cycle_control_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct cycle_control_interface {
#define struct_cycle_control_interface_t_type_p struct_cycle_control_interface_t_type_p
	struct cycle_control_interface *src = MM_MALLOC(1, struct cycle_control_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_cycle_control_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct cycle_control_interface * {
#define struct_cycle_control_interface_t_type_p struct_cycle_control_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_cycle_control_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct cycle_control_interface * {
#define struct_cycle_control_interface_t_type_p struct_cycle_control_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_cycle_control_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct __anonymous144 {
#define struct_duration_t_type_p struct_duration_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_duration_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a duration_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(duration_t *, $source);
} default_value { (const duration_t){ .t = 0 } };

/* generated */
%typemap (python, struct-out) struct __anonymous144 {
#define struct_duration_t_type_p struct_duration_t_type_p
$target = new_opaque_object(&$source, false, false, struct_duration_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous144 {
#define struct_duration_t_type_p struct_duration_t_type_p
	duration_t *src = MM_MALLOC(1, duration_t);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_duration_t_type_p, NULL);
};

/* generated */
%typemap (python, out) duration_t * {
#define struct_duration_t_type_p struct_duration_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_duration_t_type_p, NULL);
};

/* generated */
%typemap (python, in) duration_t * {
#define struct_duration_t_type_p struct_duration_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_duration_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct __anonymous145 {
#define struct_local_time_t_type_p struct_local_time_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_local_time_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a local_time_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(local_time_t *, $source);
} default_value { (const local_time_t){ .clock = NULL } };

/* generated */
%typemap (python, struct-out) struct __anonymous145 {
#define struct_local_time_t_type_p struct_local_time_t_type_p
$target = new_opaque_object(&$source, false, false, struct_local_time_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous145 {
#define struct_local_time_t_type_p struct_local_time_t_type_p
	local_time_t *src = MM_MALLOC(1, local_time_t);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_local_time_t_type_p, NULL);
};

/* generated */
%typemap (python, out) local_time_t * {
#define struct_local_time_t_type_p struct_local_time_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_local_time_t_type_p, NULL);
};

/* generated */
%typemap (python, in) local_time_t * {
#define struct_local_time_t_type_p struct_local_time_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_local_time_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct event_class {
#define struct_event_class_t_type_p struct_event_class_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_event_class_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an event_class_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct event_class *NOTNULL, $source);
};

/* generated */
%typemap (python, struct-out) struct event_class {
#define struct_event_class_t_type_p struct_event_class_t_type_p
$target = new_opaque_object(&$source, false, false, struct_event_class_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct event_class {
#define struct_event_class_t_type_p struct_event_class_t_type_p
	struct event_class *NOTNULL src = MM_MALLOC(1, struct event_class);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_event_class_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct event_class *NOTNULL {
#define struct_event_class_t_type_p struct_event_class_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_event_class_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct event_class *NOTNULL {
#define struct_event_class_t_type_p struct_event_class_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_event_class_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct cycle_interface {
#define struct_cycle_interface_t_type_p struct_cycle_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_cycle_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a cycle_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct cycle_interface *, $source);
} default_value { (const cycle_interface_t){ .get_cycle_count = 0 } };

/* generated */
%typemap (python, struct-out) struct cycle_interface {
#define struct_cycle_interface_t_type_p struct_cycle_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_cycle_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct cycle_interface {
#define struct_cycle_interface_t_type_p struct_cycle_interface_t_type_p
	struct cycle_interface *src = MM_MALLOC(1, struct cycle_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_cycle_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct cycle_interface * {
#define struct_cycle_interface_t_type_p struct_cycle_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_cycle_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct cycle_interface * {
#define struct_cycle_interface_t_type_p struct_cycle_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_cycle_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct cxl_non_device_decoder_handling_interface {
#define struct_cxl_non_device_decoder_handling_interface_t_type_p struct_cxl_non_device_decoder_handling_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_cxl_non_device_decoder_handling_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a cxl_non_device_decoder_handling_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct cxl_non_device_decoder_handling_interface *, $source);
} default_value { (const cxl_non_device_decoder_handling_interface_t){ .enable_decoder = 0 } };

/* generated */
%typemap (python, struct-out) struct cxl_non_device_decoder_handling_interface {
#define struct_cxl_non_device_decoder_handling_interface_t_type_p struct_cxl_non_device_decoder_handling_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_cxl_non_device_decoder_handling_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct cxl_non_device_decoder_handling_interface {
#define struct_cxl_non_device_decoder_handling_interface_t_type_p struct_cxl_non_device_decoder_handling_interface_t_type_p
	struct cxl_non_device_decoder_handling_interface *src = MM_MALLOC(1, struct cxl_non_device_decoder_handling_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_cxl_non_device_decoder_handling_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct cxl_non_device_decoder_handling_interface * {
#define struct_cxl_non_device_decoder_handling_interface_t_type_p struct_cxl_non_device_decoder_handling_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_cxl_non_device_decoder_handling_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct cxl_non_device_decoder_handling_interface * {
#define struct_cxl_non_device_decoder_handling_interface_t_type_p struct_cxl_non_device_decoder_handling_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_cxl_non_device_decoder_handling_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct cxl_mem_downstream_port_managing_interface {
#define struct_cxl_mem_downstream_port_managing_interface_t_type_p struct_cxl_mem_downstream_port_managing_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_cxl_mem_downstream_port_managing_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a cxl_mem_downstream_port_managing_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct cxl_mem_downstream_port_managing_interface *, $source);
} default_value { (const cxl_mem_downstream_port_managing_interface_t){ .register_port_mem_obj = 0 } };

/* generated */
%typemap (python, struct-out) struct cxl_mem_downstream_port_managing_interface {
#define struct_cxl_mem_downstream_port_managing_interface_t_type_p struct_cxl_mem_downstream_port_managing_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_cxl_mem_downstream_port_managing_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct cxl_mem_downstream_port_managing_interface {
#define struct_cxl_mem_downstream_port_managing_interface_t_type_p struct_cxl_mem_downstream_port_managing_interface_t_type_p
	struct cxl_mem_downstream_port_managing_interface *src = MM_MALLOC(1, struct cxl_mem_downstream_port_managing_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_cxl_mem_downstream_port_managing_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct cxl_mem_downstream_port_managing_interface * {
#define struct_cxl_mem_downstream_port_managing_interface_t_type_p struct_cxl_mem_downstream_port_managing_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_cxl_mem_downstream_port_managing_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct cxl_mem_downstream_port_managing_interface * {
#define struct_cxl_mem_downstream_port_managing_interface_t_type_p struct_cxl_mem_downstream_port_managing_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_cxl_mem_downstream_port_managing_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct cxl_map_interface {
#define struct_cxl_map_interface_t_type_p struct_cxl_map_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_cxl_map_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a cxl_map_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct cxl_map_interface *, $source);
} default_value { (const cxl_map_interface_t){ .add_map = 0 } };

/* generated */
%typemap (python, struct-out) struct cxl_map_interface {
#define struct_cxl_map_interface_t_type_p struct_cxl_map_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_cxl_map_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct cxl_map_interface {
#define struct_cxl_map_interface_t_type_p struct_cxl_map_interface_t_type_p
	struct cxl_map_interface *src = MM_MALLOC(1, struct cxl_map_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_cxl_map_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct cxl_map_interface * {
#define struct_cxl_map_interface_t_type_p struct_cxl_map_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_cxl_map_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct cxl_map_interface * {
#define struct_cxl_map_interface_t_type_p struct_cxl_map_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_cxl_map_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct cpu_memory_query_interface {
#define struct_cpu_memory_query_interface_t_type_p struct_cpu_memory_query_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_cpu_memory_query_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a cpu_memory_query_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct cpu_memory_query_interface *, $source);
} default_value { (const cpu_memory_query_interface_t){ .logical_address = 0 } };

/* generated */
%typemap (python, struct-out) struct cpu_memory_query_interface {
#define struct_cpu_memory_query_interface_t_type_p struct_cpu_memory_query_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_cpu_memory_query_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct cpu_memory_query_interface {
#define struct_cpu_memory_query_interface_t_type_p struct_cpu_memory_query_interface_t_type_p
	struct cpu_memory_query_interface *src = MM_MALLOC(1, struct cpu_memory_query_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_cpu_memory_query_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct cpu_memory_query_interface * {
#define struct_cpu_memory_query_interface_t_type_p struct_cpu_memory_query_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_cpu_memory_query_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct cpu_memory_query_interface * {
#define struct_cpu_memory_query_interface_t_type_p struct_cpu_memory_query_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_cpu_memory_query_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct cpu_exception_return_info {
#define struct_exception_return_handle_t_type_p struct_exception_return_handle_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_exception_return_handle_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an exception_return_handle_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct cpu_exception_return_info *, $source);
};

/* generated */
%typemap (python, struct-out) struct cpu_exception_return_info {
#define struct_exception_return_handle_t_type_p struct_exception_return_handle_t_type_p
$target = new_opaque_object(&$source, false, false, struct_exception_return_handle_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct cpu_exception_return_info {
#define struct_exception_return_handle_t_type_p struct_exception_return_handle_t_type_p
	struct cpu_exception_return_info *src = MM_MALLOC(1, struct cpu_exception_return_info);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_exception_return_handle_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct cpu_exception_return_info * {
#define struct_exception_return_handle_t_type_p struct_exception_return_handle_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_exception_return_handle_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct cpu_exception_return_info * {
#define struct_exception_return_handle_t_type_p struct_exception_return_handle_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_exception_return_handle_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct cpu_address_info {
#define struct_address_handle_t_type_p struct_address_handle_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_address_handle_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an address_handle_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct cpu_address_info *, $source);
};

/* generated */
%typemap (python, struct-out) struct cpu_address_info {
#define struct_address_handle_t_type_p struct_address_handle_t_type_p
$target = new_opaque_object(&$source, false, false, struct_address_handle_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct cpu_address_info {
#define struct_address_handle_t_type_p struct_address_handle_t_type_p
	struct cpu_address_info *src = MM_MALLOC(1, struct cpu_address_info);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_address_handle_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct cpu_address_info * {
#define struct_address_handle_t_type_p struct_address_handle_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_address_handle_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct cpu_address_info * {
#define struct_address_handle_t_type_p struct_address_handle_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_address_handle_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct instrumentation_entry {
#define struct__cpu_instrumentation_entry_t_type_p struct__cpu_instrumentation_entry_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct__cpu_instrumentation_entry_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a _cpu_instrumentation_entry_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct instrumentation_entry *, $source);
};

/* generated */
%typemap (python, struct-out) struct instrumentation_entry {
#define struct__cpu_instrumentation_entry_t_type_p struct__cpu_instrumentation_entry_t_type_p
$target = new_opaque_object(&$source, false, false, struct__cpu_instrumentation_entry_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct instrumentation_entry {
#define struct__cpu_instrumentation_entry_t_type_p struct__cpu_instrumentation_entry_t_type_p
	struct instrumentation_entry *src = MM_MALLOC(1, struct instrumentation_entry);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct__cpu_instrumentation_entry_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct instrumentation_entry * {
#define struct__cpu_instrumentation_entry_t_type_p struct__cpu_instrumentation_entry_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct__cpu_instrumentation_entry_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct instrumentation_entry * {
#define struct__cpu_instrumentation_entry_t_type_p struct__cpu_instrumentation_entry_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct__cpu_instrumentation_entry_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct cpu_instrumentation_subscribe_interface {
#define struct_cpu_instrumentation_subscribe_interface_t_type_p struct_cpu_instrumentation_subscribe_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_cpu_instrumentation_subscribe_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a cpu_instrumentation_subscribe_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct cpu_instrumentation_subscribe_interface *, $source);
} default_value { (const cpu_instrumentation_subscribe_interface_t){ .remove_callback = 0 } };

/* generated */
%typemap (python, struct-out) struct cpu_instrumentation_subscribe_interface {
#define struct_cpu_instrumentation_subscribe_interface_t_type_p struct_cpu_instrumentation_subscribe_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_cpu_instrumentation_subscribe_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct cpu_instrumentation_subscribe_interface {
#define struct_cpu_instrumentation_subscribe_interface_t_type_p struct_cpu_instrumentation_subscribe_interface_t_type_p
	struct cpu_instrumentation_subscribe_interface *src = MM_MALLOC(1, struct cpu_instrumentation_subscribe_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_cpu_instrumentation_subscribe_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct cpu_instrumentation_subscribe_interface * {
#define struct_cpu_instrumentation_subscribe_interface_t_type_p struct_cpu_instrumentation_subscribe_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_cpu_instrumentation_subscribe_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct cpu_instrumentation_subscribe_interface * {
#define struct_cpu_instrumentation_subscribe_interface_t_type_p struct_cpu_instrumentation_subscribe_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_cpu_instrumentation_subscribe_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct cpu_instruction_query_interface {
#define struct_cpu_instruction_query_interface_t_type_p struct_cpu_instruction_query_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_cpu_instruction_query_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a cpu_instruction_query_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct cpu_instruction_query_interface *, $source);
} default_value { (const cpu_instruction_query_interface_t){ .logical_address = 0 } };

/* generated */
%typemap (python, struct-out) struct cpu_instruction_query_interface {
#define struct_cpu_instruction_query_interface_t_type_p struct_cpu_instruction_query_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_cpu_instruction_query_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct cpu_instruction_query_interface {
#define struct_cpu_instruction_query_interface_t_type_p struct_cpu_instruction_query_interface_t_type_p
	struct cpu_instruction_query_interface *src = MM_MALLOC(1, struct cpu_instruction_query_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_cpu_instruction_query_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct cpu_instruction_query_interface * {
#define struct_cpu_instruction_query_interface_t_type_p struct_cpu_instruction_query_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_cpu_instruction_query_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct cpu_instruction_query_interface * {
#define struct_cpu_instruction_query_interface_t_type_p struct_cpu_instruction_query_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_cpu_instruction_query_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct cpu_replace_info {
#define struct_decoder_handle_t_type_p struct_decoder_handle_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_decoder_handle_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a decoder_handle_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct cpu_replace_info *, $source);
};

/* generated */
%typemap (python, struct-out) struct cpu_replace_info {
#define struct_decoder_handle_t_type_p struct_decoder_handle_t_type_p
$target = new_opaque_object(&$source, false, false, struct_decoder_handle_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct cpu_replace_info {
#define struct_decoder_handle_t_type_p struct_decoder_handle_t_type_p
	struct cpu_replace_info *src = MM_MALLOC(1, struct cpu_replace_info);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_decoder_handle_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct cpu_replace_info * {
#define struct_decoder_handle_t_type_p struct_decoder_handle_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_decoder_handle_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct cpu_replace_info * {
#define struct_decoder_handle_t_type_p struct_decoder_handle_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_decoder_handle_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct cpu_instruction_decoder_interface {
#define struct_cpu_instruction_decoder_interface_t_type_p struct_cpu_instruction_decoder_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_cpu_instruction_decoder_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a cpu_instruction_decoder_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct cpu_instruction_decoder_interface *, $source);
} default_value { (const cpu_instruction_decoder_interface_t){ .register_emulation_cb = 0 } };

/* generated */
%typemap (python, struct-out) struct cpu_instruction_decoder_interface {
#define struct_cpu_instruction_decoder_interface_t_type_p struct_cpu_instruction_decoder_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_cpu_instruction_decoder_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct cpu_instruction_decoder_interface {
#define struct_cpu_instruction_decoder_interface_t_type_p struct_cpu_instruction_decoder_interface_t_type_p
	struct cpu_instruction_decoder_interface *src = MM_MALLOC(1, struct cpu_instruction_decoder_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_cpu_instruction_decoder_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct cpu_instruction_decoder_interface * {
#define struct_cpu_instruction_decoder_interface_t_type_p struct_cpu_instruction_decoder_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_cpu_instruction_decoder_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct cpu_instruction_decoder_interface * {
#define struct_cpu_instruction_decoder_interface_t_type_p struct_cpu_instruction_decoder_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_cpu_instruction_decoder_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct cpu_exception_info {
#define struct_exception_handle_t_type_p struct_exception_handle_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_exception_handle_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an exception_handle_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct cpu_exception_info *, $source);
};

/* generated */
%typemap (python, struct-out) struct cpu_exception_info {
#define struct_exception_handle_t_type_p struct_exception_handle_t_type_p
$target = new_opaque_object(&$source, false, false, struct_exception_handle_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct cpu_exception_info {
#define struct_exception_handle_t_type_p struct_exception_handle_t_type_p
	struct cpu_exception_info *src = MM_MALLOC(1, struct cpu_exception_info);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_exception_handle_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct cpu_exception_info * {
#define struct_exception_handle_t_type_p struct_exception_handle_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_exception_handle_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct cpu_exception_info * {
#define struct_exception_handle_t_type_p struct_exception_handle_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_exception_handle_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct cpu_exception_query_interface {
#define struct_cpu_exception_query_interface_t_type_p struct_cpu_exception_query_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_cpu_exception_query_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a cpu_exception_query_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct cpu_exception_query_interface *, $source);
} default_value { (const cpu_exception_query_interface_t){ .exception_number = 0 } };

/* generated */
%typemap (python, struct-out) struct cpu_exception_query_interface {
#define struct_cpu_exception_query_interface_t_type_p struct_cpu_exception_query_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_cpu_exception_query_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct cpu_exception_query_interface {
#define struct_cpu_exception_query_interface_t_type_p struct_cpu_exception_query_interface_t_type_p
	struct cpu_exception_query_interface *src = MM_MALLOC(1, struct cpu_exception_query_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_cpu_exception_query_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct cpu_exception_query_interface * {
#define struct_cpu_exception_query_interface_t_type_p struct_cpu_exception_query_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_cpu_exception_query_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct cpu_exception_query_interface * {
#define struct_cpu_exception_query_interface_t_type_p struct_cpu_exception_query_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_cpu_exception_query_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct cpu_cached_instruction_once_interface {
#define struct_cpu_cached_instruction_once_interface_t_type_p struct_cpu_cached_instruction_once_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_cpu_cached_instruction_once_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a cpu_cached_instruction_once_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct cpu_cached_instruction_once_interface *, $source);
} default_value { (const cpu_cached_instruction_once_interface_t){ .register_instruction_before_once_cb = 0 } };

/* generated */
%typemap (python, struct-out) struct cpu_cached_instruction_once_interface {
#define struct_cpu_cached_instruction_once_interface_t_type_p struct_cpu_cached_instruction_once_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_cpu_cached_instruction_once_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct cpu_cached_instruction_once_interface {
#define struct_cpu_cached_instruction_once_interface_t_type_p struct_cpu_cached_instruction_once_interface_t_type_p
	struct cpu_cached_instruction_once_interface *src = MM_MALLOC(1, struct cpu_cached_instruction_once_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_cpu_cached_instruction_once_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct cpu_cached_instruction_once_interface * {
#define struct_cpu_cached_instruction_once_interface_t_type_p struct_cpu_cached_instruction_once_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_cpu_cached_instruction_once_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct cpu_cached_instruction_once_interface * {
#define struct_cpu_cached_instruction_once_interface_t_type_p struct_cpu_cached_instruction_once_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_cpu_cached_instruction_once_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct cpu_memory_info {
#define struct_memory_handle_t_type_p struct_memory_handle_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_memory_handle_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a memory_handle_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct cpu_memory_info *, $source);
};

/* generated */
%typemap (python, struct-out) struct cpu_memory_info {
#define struct_memory_handle_t_type_p struct_memory_handle_t_type_p
$target = new_opaque_object(&$source, false, false, struct_memory_handle_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct cpu_memory_info {
#define struct_memory_handle_t_type_p struct_memory_handle_t_type_p
	struct cpu_memory_info *src = MM_MALLOC(1, struct cpu_memory_info);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_memory_handle_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct cpu_memory_info * {
#define struct_memory_handle_t_type_p struct_memory_handle_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_memory_handle_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct cpu_memory_info * {
#define struct_memory_handle_t_type_p struct_memory_handle_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_memory_handle_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct cpu_instrumentation_info {
#define struct_instruction_handle_t_type_p struct_instruction_handle_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_instruction_handle_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an instruction_handle_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct cpu_instrumentation_info *, $source);
};

/* generated */
%typemap (python, struct-out) struct cpu_instrumentation_info {
#define struct_instruction_handle_t_type_p struct_instruction_handle_t_type_p
$target = new_opaque_object(&$source, false, false, struct_instruction_handle_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct cpu_instrumentation_info {
#define struct_instruction_handle_t_type_p struct_instruction_handle_t_type_p
	struct cpu_instrumentation_info *src = MM_MALLOC(1, struct cpu_instrumentation_info);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_instruction_handle_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct cpu_instrumentation_info * {
#define struct_instruction_handle_t_type_p struct_instruction_handle_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_instruction_handle_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct cpu_instrumentation_info * {
#define struct_instruction_handle_t_type_p struct_instruction_handle_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_instruction_handle_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct cached_instruction_data {
#define struct_cached_instruction_handle_t_type_p struct_cached_instruction_handle_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_cached_instruction_handle_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a cached_instruction_handle_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct cached_instruction_data *, $source);
};

/* generated */
%typemap (python, struct-out) struct cached_instruction_data {
#define struct_cached_instruction_handle_t_type_p struct_cached_instruction_handle_t_type_p
$target = new_opaque_object(&$source, false, false, struct_cached_instruction_handle_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct cached_instruction_data {
#define struct_cached_instruction_handle_t_type_p struct_cached_instruction_handle_t_type_p
	struct cached_instruction_data *src = MM_MALLOC(1, struct cached_instruction_data);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_cached_instruction_handle_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct cached_instruction_data * {
#define struct_cached_instruction_handle_t_type_p struct_cached_instruction_handle_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_cached_instruction_handle_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct cached_instruction_data * {
#define struct_cached_instruction_handle_t_type_p struct_cached_instruction_handle_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_cached_instruction_handle_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct cpu_cached_instruction_interface {
#define struct_cpu_cached_instruction_interface_t_type_p struct_cpu_cached_instruction_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_cpu_cached_instruction_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a cpu_cached_instruction_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct cpu_cached_instruction_interface *, $source);
} default_value { (const cpu_cached_instruction_interface_t){ .register_instruction_before_cb = 0 } };

/* generated */
%typemap (python, struct-out) struct cpu_cached_instruction_interface {
#define struct_cpu_cached_instruction_interface_t_type_p struct_cpu_cached_instruction_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_cpu_cached_instruction_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct cpu_cached_instruction_interface {
#define struct_cpu_cached_instruction_interface_t_type_p struct_cpu_cached_instruction_interface_t_type_p
	struct cpu_cached_instruction_interface *src = MM_MALLOC(1, struct cpu_cached_instruction_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_cpu_cached_instruction_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct cpu_cached_instruction_interface * {
#define struct_cpu_cached_instruction_interface_t_type_p struct_cpu_cached_instruction_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_cpu_cached_instruction_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct cpu_cached_instruction_interface * {
#define struct_cpu_cached_instruction_interface_t_type_p struct_cpu_cached_instruction_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_cpu_cached_instruction_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct __anonymous152 {
#define struct_coreint_reply_t_type_p struct_coreint_reply_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_coreint_reply_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a coreint_reply_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(coreint_reply_t *, $source);
} default_value { (const coreint_reply_t){ .enabled = 0 } };

/* generated */
%typemap (python, struct-out) struct __anonymous152 {
#define struct_coreint_reply_t_type_p struct_coreint_reply_t_type_p
$target = new_opaque_object(&$source, false, false, struct_coreint_reply_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous152 {
#define struct_coreint_reply_t_type_p struct_coreint_reply_t_type_p
	coreint_reply_t *src = MM_MALLOC(1, coreint_reply_t);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_coreint_reply_t_type_p, NULL);
};

/* generated */
%typemap (python, out) coreint_reply_t * {
#define struct_coreint_reply_t_type_p struct_coreint_reply_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_coreint_reply_t_type_p, NULL);
};

/* generated */
%typemap (python, in) coreint_reply_t * {
#define struct_coreint_reply_t_type_p struct_coreint_reply_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_coreint_reply_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct coreint_interface {
#define struct_coreint_interface_t_type_p struct_coreint_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_coreint_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a coreint_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct coreint_interface *, $source);
} default_value { (const coreint_interface_t){ .acknowledge = 0 } };

/* generated */
%typemap (python, struct-out) struct coreint_interface {
#define struct_coreint_interface_t_type_p struct_coreint_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_coreint_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct coreint_interface {
#define struct_coreint_interface_t_type_p struct_coreint_interface_t_type_p
	struct coreint_interface *src = MM_MALLOC(1, struct coreint_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_coreint_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct coreint_interface * {
#define struct_coreint_interface_t_type_p struct_coreint_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_coreint_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct coreint_interface * {
#define struct_coreint_interface_t_type_p struct_coreint_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_coreint_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct context_handler_interface {
#define struct_context_handler_interface_t_type_p struct_context_handler_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_context_handler_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a context_handler_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct context_handler_interface *, $source);
} default_value { (const context_handler_interface_t){ .get_current_context = 0 } };

/* generated */
%typemap (python, struct-out) struct context_handler_interface {
#define struct_context_handler_interface_t_type_p struct_context_handler_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_context_handler_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct context_handler_interface {
#define struct_context_handler_interface_t_type_p struct_context_handler_interface_t_type_p
	struct context_handler_interface *src = MM_MALLOC(1, struct context_handler_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_context_handler_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct context_handler_interface * {
#define struct_context_handler_interface_t_type_p struct_context_handler_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_context_handler_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct context_handler_interface * {
#define struct_context_handler_interface_t_type_p struct_context_handler_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_context_handler_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct connector_interface {
#define struct_connector_interface_t_type_p struct_connector_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_connector_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a connector_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct connector_interface *, $source);
} default_value { (const connector_interface_t){ .type = 0 } };

/* generated */
%typemap (python, struct-out) struct connector_interface {
#define struct_connector_interface_t_type_p struct_connector_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_connector_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct connector_interface {
#define struct_connector_interface_t_type_p struct_connector_interface_t_type_p
	struct connector_interface *src = MM_MALLOC(1, struct connector_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_connector_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct connector_interface * {
#define struct_connector_interface_t_type_p struct_connector_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_connector_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct connector_interface * {
#define struct_connector_interface_t_type_p struct_connector_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_connector_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct concurrency_mode_interface {
#define struct_concurrency_mode_interface_t_type_p struct_concurrency_mode_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_concurrency_mode_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a concurrency_mode_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct concurrency_mode_interface *, $source);
} default_value { (const concurrency_mode_interface_t){ .supported_modes = 0 } };

/* generated */
%typemap (python, struct-out) struct concurrency_mode_interface {
#define struct_concurrency_mode_interface_t_type_p struct_concurrency_mode_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_concurrency_mode_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct concurrency_mode_interface {
#define struct_concurrency_mode_interface_t_type_p struct_concurrency_mode_interface_t_type_p
	struct concurrency_mode_interface *src = MM_MALLOC(1, struct concurrency_mode_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_concurrency_mode_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct concurrency_mode_interface * {
#define struct_concurrency_mode_interface_t_type_p struct_concurrency_mode_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_concurrency_mode_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct concurrency_mode_interface * {
#define struct_concurrency_mode_interface_t_type_p struct_concurrency_mode_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_concurrency_mode_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct concurrency_group_interface {
#define struct_concurrency_group_interface_t_type_p struct_concurrency_group_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_concurrency_group_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a concurrency_group_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct concurrency_group_interface *, $source);
} default_value { (const concurrency_group_interface_t){ .serialized_memory_group = 0 } };

/* generated */
%typemap (python, struct-out) struct concurrency_group_interface {
#define struct_concurrency_group_interface_t_type_p struct_concurrency_group_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_concurrency_group_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct concurrency_group_interface {
#define struct_concurrency_group_interface_t_type_p struct_concurrency_group_interface_t_type_p
	struct concurrency_group_interface *src = MM_MALLOC(1, struct concurrency_group_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_concurrency_group_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct concurrency_group_interface * {
#define struct_concurrency_group_interface_t_type_p struct_concurrency_group_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_concurrency_group_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct concurrency_group_interface * {
#define struct_concurrency_group_interface_t_type_p struct_concurrency_group_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_concurrency_group_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct con_input_code_interface {
#define struct_con_input_code_interface_t_type_p struct_con_input_code_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_con_input_code_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a con_input_code_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct con_input_code_interface *, $source);
} default_value { (const con_input_code_interface_t){ .input = 0 } };

/* generated */
%typemap (python, struct-out) struct con_input_code_interface {
#define struct_con_input_code_interface_t_type_p struct_con_input_code_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_con_input_code_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct con_input_code_interface {
#define struct_con_input_code_interface_t_type_p struct_con_input_code_interface_t_type_p
	struct con_input_code_interface *src = MM_MALLOC(1, struct con_input_code_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_con_input_code_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct con_input_code_interface * {
#define struct_con_input_code_interface_t_type_p struct_con_input_code_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_con_input_code_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct con_input_code_interface * {
#define struct_con_input_code_interface_t_type_p struct_con_input_code_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_con_input_code_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct con_input_interface {
#define struct_con_input_interface_t_type_p struct_con_input_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_con_input_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a con_input_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct con_input_interface *, $source);
} default_value { (const con_input_interface_t){ .input_str = 0 } };

/* generated */
%typemap (python, struct-out) struct con_input_interface {
#define struct_con_input_interface_t_type_p struct_con_input_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_con_input_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct con_input_interface {
#define struct_con_input_interface_t_type_p struct_con_input_interface_t_type_p
	struct con_input_interface *src = MM_MALLOC(1, struct con_input_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_con_input_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct con_input_interface * {
#define struct_con_input_interface_t_type_p struct_con_input_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_con_input_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct con_input_interface * {
#define struct_con_input_interface_t_type_p struct_con_input_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_con_input_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct component_connector_interface {
#define struct_component_connector_interface_t_type_p struct_component_connector_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_component_connector_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a component_connector_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct component_connector_interface *, $source);
} default_value { (const component_connector_interface_t){ .get_check_data = 0 } };

/* generated */
%typemap (python, struct-out) struct component_connector_interface {
#define struct_component_connector_interface_t_type_p struct_component_connector_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_component_connector_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct component_connector_interface {
#define struct_component_connector_interface_t_type_p struct_component_connector_interface_t_type_p
	struct component_connector_interface *src = MM_MALLOC(1, struct component_connector_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_component_connector_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct component_connector_interface * {
#define struct_component_connector_interface_t_type_p struct_component_connector_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_component_connector_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct component_connector_interface * {
#define struct_component_connector_interface_t_type_p struct_component_connector_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_component_connector_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct component_interface {
#define struct_component_interface_t_type_p struct_component_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_component_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a component_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct component_interface *, $source);
} default_value { (const component_interface_t){ .pre_instantiate = 0 } };

/* generated */
%typemap (python, struct-out) struct component_interface {
#define struct_component_interface_t_type_p struct_component_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_component_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct component_interface {
#define struct_component_interface_t_type_p struct_component_interface_t_type_p
	struct component_interface *src = MM_MALLOC(1, struct component_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_component_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct component_interface * {
#define struct_component_interface_t_type_p struct_component_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_component_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct component_interface * {
#define struct_component_interface_t_type_p struct_component_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_component_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct co_execute_interface {
#define struct_co_execute_interface_t_type_p struct_co_execute_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_co_execute_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a co_execute_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct co_execute_interface *, $source);
} default_value { (const co_execute_interface_t){ .start_thread = 0 } };

/* generated */
%typemap (python, struct-out) struct co_execute_interface {
#define struct_co_execute_interface_t_type_p struct_co_execute_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_co_execute_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct co_execute_interface {
#define struct_co_execute_interface_t_type_p struct_co_execute_interface_t_type_p
	struct co_execute_interface *src = MM_MALLOC(1, struct co_execute_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_co_execute_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct co_execute_interface * {
#define struct_co_execute_interface_t_type_p struct_co_execute_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_co_execute_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct co_execute_interface * {
#define struct_co_execute_interface_t_type_p struct_co_execute_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_co_execute_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct cmd_line_selection_interface {
#define struct_cmd_line_selection_interface_t_type_p struct_cmd_line_selection_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_cmd_line_selection_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a cmd_line_selection_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct cmd_line_selection_interface *, $source);
} default_value { (const cmd_line_selection_interface_t){ .new_selection = 0 } };

/* generated */
%typemap (python, struct-out) struct cmd_line_selection_interface {
#define struct_cmd_line_selection_interface_t_type_p struct_cmd_line_selection_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_cmd_line_selection_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct cmd_line_selection_interface {
#define struct_cmd_line_selection_interface_t_type_p struct_cmd_line_selection_interface_t_type_p
	struct cmd_line_selection_interface *src = MM_MALLOC(1, struct cmd_line_selection_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_cmd_line_selection_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct cmd_line_selection_interface * {
#define struct_cmd_line_selection_interface_t_type_p struct_cmd_line_selection_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_cmd_line_selection_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct cmd_line_selection_interface * {
#define struct_cmd_line_selection_interface_t_type_p struct_cmd_line_selection_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_cmd_line_selection_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct cmd_line_frontend_interface {
#define struct_cmd_line_frontend_interface_t_type_p struct_cmd_line_frontend_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_cmd_line_frontend_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a cmd_line_frontend_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct cmd_line_frontend_interface *, $source);
} default_value { (const cmd_line_frontend_interface_t){ .write = 0 } };

/* generated */
%typemap (python, struct-out) struct cmd_line_frontend_interface {
#define struct_cmd_line_frontend_interface_t_type_p struct_cmd_line_frontend_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_cmd_line_frontend_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct cmd_line_frontend_interface {
#define struct_cmd_line_frontend_interface_t_type_p struct_cmd_line_frontend_interface_t_type_p
	struct cmd_line_frontend_interface *src = MM_MALLOC(1, struct cmd_line_frontend_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_cmd_line_frontend_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct cmd_line_frontend_interface * {
#define struct_cmd_line_frontend_interface_t_type_p struct_cmd_line_frontend_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_cmd_line_frontend_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct cmd_line_frontend_interface * {
#define struct_cmd_line_frontend_interface_t_type_p struct_cmd_line_frontend_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_cmd_line_frontend_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct __anonymous157 {
#define struct_disassembly_mode_info_t_type_p struct_disassembly_mode_info_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_disassembly_mode_info_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a disassembly_mode_info_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(disassembly_mode_info_t *, $source);
} default_value { (const disassembly_mode_info_t){ .mode = 0 } };

/* generated */
%typemap (python, struct-out) struct __anonymous157 {
#define struct_disassembly_mode_info_t_type_p struct_disassembly_mode_info_t_type_p
$target = new_opaque_object(&$source, false, false, struct_disassembly_mode_info_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous157 {
#define struct_disassembly_mode_info_t_type_p struct_disassembly_mode_info_t_type_p
	disassembly_mode_info_t *src = MM_MALLOC(1, disassembly_mode_info_t);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_disassembly_mode_info_t_type_p, NULL);
};

/* generated */
%typemap (python, out) disassembly_mode_info_t * {
#define struct_disassembly_mode_info_t_type_p struct_disassembly_mode_info_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_disassembly_mode_info_t_type_p, NULL);
};

/* generated */
%typemap (python, in) disassembly_mode_info_t * {
#define struct_disassembly_mode_info_t_type_p struct_disassembly_mode_info_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_disassembly_mode_info_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct __anonymous159 {
#define struct_disassembly_instruction_t_type_p struct_disassembly_instruction_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_disassembly_instruction_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a disassembly_instruction_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(disassembly_instruction_t *, $source);
} default_value { (const disassembly_instruction_t){ .address = 0 } };

/* generated */
%typemap (python, struct-out) struct __anonymous159 {
#define struct_disassembly_instruction_t_type_p struct_disassembly_instruction_t_type_p
$target = new_opaque_object(&$source, false, false, struct_disassembly_instruction_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous159 {
#define struct_disassembly_instruction_t_type_p struct_disassembly_instruction_t_type_p
	disassembly_instruction_t *src = MM_MALLOC(1, disassembly_instruction_t);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_disassembly_instruction_t_type_p, NULL);
};

/* generated */
%typemap (python, out) disassembly_instruction_t * {
#define struct_disassembly_instruction_t_type_p struct_disassembly_instruction_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_disassembly_instruction_t_type_p, NULL);
};

/* generated */
%typemap (python, in) disassembly_instruction_t * {
#define struct_disassembly_instruction_t_type_p struct_disassembly_instruction_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_disassembly_instruction_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct class_disassembly_interface {
#define struct_class_disassembly_interface_t_type_p struct_class_disassembly_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_class_disassembly_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a class_disassembly_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct class_disassembly_interface *, $source);
} default_value { (const class_disassembly_interface_t){ .disassemble_buffer = 0 } };

/* generated */
%typemap (python, struct-out) struct class_disassembly_interface {
#define struct_class_disassembly_interface_t_type_p struct_class_disassembly_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_class_disassembly_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct class_disassembly_interface {
#define struct_class_disassembly_interface_t_type_p struct_class_disassembly_interface_t_type_p
	struct class_disassembly_interface *src = MM_MALLOC(1, struct class_disassembly_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_class_disassembly_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct class_disassembly_interface * {
#define struct_class_disassembly_interface_t_type_p struct_class_disassembly_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_class_disassembly_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct class_disassembly_interface * {
#define struct_class_disassembly_interface_t_type_p struct_class_disassembly_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_class_disassembly_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct checkpoint_interface {
#define struct_checkpoint_interface_t_type_p struct_checkpoint_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_checkpoint_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a checkpoint_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct checkpoint_interface *, $source);
} default_value { (const checkpoint_interface_t){ .save = 0 } };

/* generated */
%typemap (python, struct-out) struct checkpoint_interface {
#define struct_checkpoint_interface_t_type_p struct_checkpoint_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_checkpoint_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct checkpoint_interface {
#define struct_checkpoint_interface_t_type_p struct_checkpoint_interface_t_type_p
	struct checkpoint_interface *src = MM_MALLOC(1, struct checkpoint_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_checkpoint_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct checkpoint_interface * {
#define struct_checkpoint_interface_t_type_p struct_checkpoint_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_checkpoint_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct checkpoint_interface * {
#define struct_checkpoint_interface_t_type_p struct_checkpoint_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_checkpoint_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct cell_inspection_interface {
#define struct_cell_inspection_interface_t_type_p struct_cell_inspection_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_cell_inspection_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a cell_inspection_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct cell_inspection_interface *, $source);
} default_value { (const cell_inspection_interface_t){ .set_current_processor_obj = 0 } };

/* generated */
%typemap (python, struct-out) struct cell_inspection_interface {
#define struct_cell_inspection_interface_t_type_p struct_cell_inspection_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_cell_inspection_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct cell_inspection_interface {
#define struct_cell_inspection_interface_t_type_p struct_cell_inspection_interface_t_type_p
	struct cell_inspection_interface *src = MM_MALLOC(1, struct cell_inspection_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_cell_inspection_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct cell_inspection_interface * {
#define struct_cell_inspection_interface_t_type_p struct_cell_inspection_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_cell_inspection_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct cell_inspection_interface * {
#define struct_cell_inspection_interface_t_type_p struct_cell_inspection_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_cell_inspection_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct cdrom_media_interface {
#define struct_cdrom_media_interface_t_type_p struct_cdrom_media_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_cdrom_media_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a cdrom_media_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct cdrom_media_interface *, $source);
} default_value { (const cdrom_media_interface_t){ .capacity = 0 } };

/* generated */
%typemap (python, struct-out) struct cdrom_media_interface {
#define struct_cdrom_media_interface_t_type_p struct_cdrom_media_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_cdrom_media_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct cdrom_media_interface {
#define struct_cdrom_media_interface_t_type_p struct_cdrom_media_interface_t_type_p
	struct cdrom_media_interface *src = MM_MALLOC(1, struct cdrom_media_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_cdrom_media_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct cdrom_media_interface * {
#define struct_cdrom_media_interface_t_type_p struct_cdrom_media_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_cdrom_media_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct cdrom_media_interface * {
#define struct_cdrom_media_interface_t_type_p struct_cdrom_media_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_cdrom_media_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct callback_info_interface {
#define struct_callback_info_interface_t_type_p struct_callback_info_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_callback_info_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a callback_info_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct callback_info_interface *, $source);
} default_value { (const callback_info_interface_t){ .get_callbacks = 0 } };

/* generated */
%typemap (python, struct-out) struct callback_info_interface {
#define struct_callback_info_interface_t_type_p struct_callback_info_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_callback_info_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct callback_info_interface {
#define struct_callback_info_interface_t_type_p struct_callback_info_interface_t_type_p
	struct callback_info_interface *src = MM_MALLOC(1, struct callback_info_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_callback_info_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct callback_info_interface * {
#define struct_callback_info_interface_t_type_p struct_callback_info_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_callback_info_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct callback_info_interface * {
#define struct_callback_info_interface_t_type_p struct_callback_info_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_callback_info_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct cache_control_interface {
#define struct_cache_control_interface_t_type_p struct_cache_control_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_cache_control_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a cache_control_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct cache_control_interface *, $source);
} default_value { (const cache_control_interface_t){ .cache_control = 0 } };

/* generated */
%typemap (python, struct-out) struct cache_control_interface {
#define struct_cache_control_interface_t_type_p struct_cache_control_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_cache_control_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct cache_control_interface {
#define struct_cache_control_interface_t_type_p struct_cache_control_interface_t_type_p
	struct cache_control_interface *src = MM_MALLOC(1, struct cache_control_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_cache_control_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct cache_control_interface * {
#define struct_cache_control_interface_t_type_p struct_cache_control_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_cache_control_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct cache_control_interface * {
#define struct_cache_control_interface_t_type_p struct_cache_control_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_cache_control_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct map_info {
#define struct_map_info_t_type_p struct_map_info_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_map_info_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a map_info_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(map_info_t *, $source);
} default_value { (const map_info_t){ .base = 0 } };

/* generated */
%typemap (python, struct-out) struct map_info {
#define struct_map_info_t_type_p struct_map_info_t_type_p
$target = new_opaque_object(&$source, false, false, struct_map_info_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct map_info {
#define struct_map_info_t_type_p struct_map_info_t_type_p
	map_info_t *src = MM_MALLOC(1, struct map_info);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_map_info_t_type_p, NULL);
};

/* generated */
%typemap (python, out) map_info_t * {
#define struct_map_info_t_type_p struct_map_info_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_map_info_t_type_p, NULL);
};

/* generated */
%typemap (python, in) map_info_t * {
#define struct_map_info_t_type_p struct_map_info_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_map_info_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct bridge_interface {
#define struct_bridge_interface_t_type_p struct_bridge_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_bridge_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a bridge_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct bridge_interface *, $source);
} default_value { (const bridge_interface_t){ .not_taken = 0 } };

/* generated */
%typemap (python, struct-out) struct bridge_interface {
#define struct_bridge_interface_t_type_p struct_bridge_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_bridge_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct bridge_interface {
#define struct_bridge_interface_t_type_p struct_bridge_interface_t_type_p
	struct bridge_interface *src = MM_MALLOC(1, struct bridge_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_bridge_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct bridge_interface * {
#define struct_bridge_interface_t_type_p struct_bridge_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_bridge_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct bridge_interface * {
#define struct_bridge_interface_t_type_p struct_bridge_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_bridge_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct breakpoint_type_provider_interface {
#define struct_breakpoint_type_provider_interface_t_type_p struct_breakpoint_type_provider_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_breakpoint_type_provider_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a breakpoint_type_provider_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct breakpoint_type_provider_interface *, $source);
} default_value { (const breakpoint_type_provider_interface_t){ .register_bp = 0 } };

/* generated */
%typemap (python, struct-out) struct breakpoint_type_provider_interface {
#define struct_breakpoint_type_provider_interface_t_type_p struct_breakpoint_type_provider_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_breakpoint_type_provider_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct breakpoint_type_provider_interface {
#define struct_breakpoint_type_provider_interface_t_type_p struct_breakpoint_type_provider_interface_t_type_p
	struct breakpoint_type_provider_interface *src = MM_MALLOC(1, struct breakpoint_type_provider_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_breakpoint_type_provider_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct breakpoint_type_provider_interface * {
#define struct_breakpoint_type_provider_interface_t_type_p struct_breakpoint_type_provider_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_breakpoint_type_provider_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct breakpoint_type_provider_interface * {
#define struct_breakpoint_type_provider_interface_t_type_p struct_breakpoint_type_provider_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_breakpoint_type_provider_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct breakpoint_type_interface {
#define struct_breakpoint_type_interface_t_type_p struct_breakpoint_type_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_breakpoint_type_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a breakpoint_type_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct breakpoint_type_interface *, $source);
} default_value { (const breakpoint_type_interface_t){ .register_type = 0 } };

/* generated */
%typemap (python, struct-out) struct breakpoint_type_interface {
#define struct_breakpoint_type_interface_t_type_p struct_breakpoint_type_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_breakpoint_type_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct breakpoint_type_interface {
#define struct_breakpoint_type_interface_t_type_p struct_breakpoint_type_interface_t_type_p
	struct breakpoint_type_interface *src = MM_MALLOC(1, struct breakpoint_type_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_breakpoint_type_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct breakpoint_type_interface * {
#define struct_breakpoint_type_interface_t_type_p struct_breakpoint_type_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_breakpoint_type_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct breakpoint_type_interface * {
#define struct_breakpoint_type_interface_t_type_p struct_breakpoint_type_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_breakpoint_type_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct breakpoint_registration_interface {
#define struct_breakpoint_registration_interface_t_type_p struct_breakpoint_registration_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_breakpoint_registration_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a breakpoint_registration_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct breakpoint_registration_interface *, $source);
} default_value { (const breakpoint_registration_interface_t){ .register_breakpoint = 0 } };

/* generated */
%typemap (python, struct-out) struct breakpoint_registration_interface {
#define struct_breakpoint_registration_interface_t_type_p struct_breakpoint_registration_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_breakpoint_registration_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct breakpoint_registration_interface {
#define struct_breakpoint_registration_interface_t_type_p struct_breakpoint_registration_interface_t_type_p
	struct breakpoint_registration_interface *src = MM_MALLOC(1, struct breakpoint_registration_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_breakpoint_registration_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct breakpoint_registration_interface * {
#define struct_breakpoint_registration_interface_t_type_p struct_breakpoint_registration_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_breakpoint_registration_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct breakpoint_registration_interface * {
#define struct_breakpoint_registration_interface_t_type_p struct_breakpoint_registration_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_breakpoint_registration_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct breakpoint_query_v2_interface {
#define struct_breakpoint_query_v2_interface_t_type_p struct_breakpoint_query_v2_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_breakpoint_query_v2_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a breakpoint_query_v2_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct breakpoint_query_v2_interface *, $source);
} default_value { (const breakpoint_query_v2_interface_t){ .get_breakpoints = 0 } };

/* generated */
%typemap (python, struct-out) struct breakpoint_query_v2_interface {
#define struct_breakpoint_query_v2_interface_t_type_p struct_breakpoint_query_v2_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_breakpoint_query_v2_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct breakpoint_query_v2_interface {
#define struct_breakpoint_query_v2_interface_t_type_p struct_breakpoint_query_v2_interface_t_type_p
	struct breakpoint_query_v2_interface *src = MM_MALLOC(1, struct breakpoint_query_v2_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_breakpoint_query_v2_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct breakpoint_query_v2_interface * {
#define struct_breakpoint_query_v2_interface_t_type_p struct_breakpoint_query_v2_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_breakpoint_query_v2_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct breakpoint_query_v2_interface * {
#define struct_breakpoint_query_v2_interface_t_type_p struct_breakpoint_query_v2_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_breakpoint_query_v2_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct breakpoint_query_interface {
#define struct_breakpoint_query_interface_t_type_p struct_breakpoint_query_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_breakpoint_query_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a breakpoint_query_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct breakpoint_query_interface *, $source);
} default_value { (const breakpoint_query_interface_t){ .get_breakpoints = 0 } };

/* generated */
%typemap (python, struct-out) struct breakpoint_query_interface {
#define struct_breakpoint_query_interface_t_type_p struct_breakpoint_query_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_breakpoint_query_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct breakpoint_query_interface {
#define struct_breakpoint_query_interface_t_type_p struct_breakpoint_query_interface_t_type_p
	struct breakpoint_query_interface *src = MM_MALLOC(1, struct breakpoint_query_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_breakpoint_query_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct breakpoint_query_interface * {
#define struct_breakpoint_query_interface_t_type_p struct_breakpoint_query_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_breakpoint_query_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct breakpoint_query_interface * {
#define struct_breakpoint_query_interface_t_type_p struct_breakpoint_query_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_breakpoint_query_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct breakpoint_manager_interface {
#define struct_breakpoint_manager_interface_t_type_p struct_breakpoint_manager_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_breakpoint_manager_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a breakpoint_manager_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct breakpoint_manager_interface *, $source);
} default_value { (const breakpoint_manager_interface_t){ .list_breakpoints = 0 } };

/* generated */
%typemap (python, struct-out) struct breakpoint_manager_interface {
#define struct_breakpoint_manager_interface_t_type_p struct_breakpoint_manager_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_breakpoint_manager_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct breakpoint_manager_interface {
#define struct_breakpoint_manager_interface_t_type_p struct_breakpoint_manager_interface_t_type_p
	struct breakpoint_manager_interface *src = MM_MALLOC(1, struct breakpoint_manager_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_breakpoint_manager_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct breakpoint_manager_interface * {
#define struct_breakpoint_manager_interface_t_type_p struct_breakpoint_manager_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_breakpoint_manager_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct breakpoint_manager_interface * {
#define struct_breakpoint_manager_interface_t_type_p struct_breakpoint_manager_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_breakpoint_manager_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct breakpoint_change_interface {
#define struct_breakpoint_change_interface_t_type_p struct_breakpoint_change_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_breakpoint_change_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a breakpoint_change_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct breakpoint_change_interface *, $source);
} default_value { (const breakpoint_change_interface_t){ .breakpoint_added = 0 } };

/* generated */
%typemap (python, struct-out) struct breakpoint_change_interface {
#define struct_breakpoint_change_interface_t_type_p struct_breakpoint_change_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_breakpoint_change_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct breakpoint_change_interface {
#define struct_breakpoint_change_interface_t_type_p struct_breakpoint_change_interface_t_type_p
	struct breakpoint_change_interface *src = MM_MALLOC(1, struct breakpoint_change_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_breakpoint_change_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct breakpoint_change_interface * {
#define struct_breakpoint_change_interface_t_type_p struct_breakpoint_change_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_breakpoint_change_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct breakpoint_change_interface * {
#define struct_breakpoint_change_interface_t_type_p struct_breakpoint_change_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_breakpoint_change_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct __anonymous162 {
#define struct_breakpoint_info_t_type_p struct_breakpoint_info_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_breakpoint_info_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a breakpoint_info_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(breakpoint_info_t *, $source);
} default_value { (const breakpoint_info_t){ .handle = 0 } };

/* generated */
%typemap (python, struct-out) struct __anonymous162 {
#define struct_breakpoint_info_t_type_p struct_breakpoint_info_t_type_p
$target = new_opaque_object(&$source, false, false, struct_breakpoint_info_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous162 {
#define struct_breakpoint_info_t_type_p struct_breakpoint_info_t_type_p
	breakpoint_info_t *src = MM_MALLOC(1, breakpoint_info_t);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_breakpoint_info_t_type_p, NULL);
};

/* generated */
%typemap (python, out) breakpoint_info_t * {
#define struct_breakpoint_info_t_type_p struct_breakpoint_info_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_breakpoint_info_t_type_p, NULL);
};

/* generated */
%typemap (python, in) breakpoint_info_t * {
#define struct_breakpoint_info_t_type_p struct_breakpoint_info_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_breakpoint_info_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct breakpoint_interface {
#define struct_breakpoint_interface_t_type_p struct_breakpoint_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_breakpoint_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a breakpoint_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct breakpoint_interface *, $source);
} default_value { (const breakpoint_interface_t){ .insert_breakpoint = 0 } };

/* generated */
%typemap (python, struct-out) struct breakpoint_interface {
#define struct_breakpoint_interface_t_type_p struct_breakpoint_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_breakpoint_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct breakpoint_interface {
#define struct_breakpoint_interface_t_type_p struct_breakpoint_interface_t_type_p
	struct breakpoint_interface *src = MM_MALLOC(1, struct breakpoint_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_breakpoint_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct breakpoint_interface * {
#define struct_breakpoint_interface_t_type_p struct_breakpoint_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_breakpoint_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct breakpoint_interface * {
#define struct_breakpoint_interface_t_type_p struct_breakpoint_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_breakpoint_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct break_strings_v2_interface {
#define struct_break_strings_v2_interface_t_type_p struct_break_strings_v2_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_break_strings_v2_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a break_strings_v2_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct break_strings_v2_interface *, $source);
} default_value { (const break_strings_v2_interface_t){ .add = 0 } };

/* generated */
%typemap (python, struct-out) struct break_strings_v2_interface {
#define struct_break_strings_v2_interface_t_type_p struct_break_strings_v2_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_break_strings_v2_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct break_strings_v2_interface {
#define struct_break_strings_v2_interface_t_type_p struct_break_strings_v2_interface_t_type_p
	struct break_strings_v2_interface *src = MM_MALLOC(1, struct break_strings_v2_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_break_strings_v2_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct break_strings_v2_interface * {
#define struct_break_strings_v2_interface_t_type_p struct_break_strings_v2_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_break_strings_v2_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct break_strings_v2_interface * {
#define struct_break_strings_v2_interface_t_type_p struct_break_strings_v2_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_break_strings_v2_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct break_strings_interface {
#define struct_break_strings_interface_t_type_p struct_break_strings_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_break_strings_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a break_strings_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct break_strings_interface *, $source);
} default_value { (const break_strings_interface_t){ .add = 0 } };

/* generated */
%typemap (python, struct-out) struct break_strings_interface {
#define struct_break_strings_interface_t_type_p struct_break_strings_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_break_strings_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct break_strings_interface {
#define struct_break_strings_interface_t_type_p struct_break_strings_interface_t_type_p
	struct break_strings_interface *src = MM_MALLOC(1, struct break_strings_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_break_strings_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct break_strings_interface * {
#define struct_break_strings_interface_t_type_p struct_break_strings_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_break_strings_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct break_strings_interface * {
#define struct_break_strings_interface_t_type_p struct_break_strings_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_break_strings_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct branch_recorder_handler_interface {
#define struct_branch_recorder_handler_interface_t_type_p struct_branch_recorder_handler_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_branch_recorder_handler_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a branch_recorder_handler_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct branch_recorder_handler_interface *, $source);
} default_value { (const branch_recorder_handler_interface_t){ .attach_branch_recorder = 0 } };

/* generated */
%typemap (python, struct-out) struct branch_recorder_handler_interface {
#define struct_branch_recorder_handler_interface_t_type_p struct_branch_recorder_handler_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_branch_recorder_handler_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct branch_recorder_handler_interface {
#define struct_branch_recorder_handler_interface_t_type_p struct_branch_recorder_handler_interface_t_type_p
	struct branch_recorder_handler_interface *src = MM_MALLOC(1, struct branch_recorder_handler_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_branch_recorder_handler_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct branch_recorder_handler_interface * {
#define struct_branch_recorder_handler_interface_t_type_p struct_branch_recorder_handler_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_branch_recorder_handler_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct branch_recorder_handler_interface * {
#define struct_branch_recorder_handler_interface_t_type_p struct_branch_recorder_handler_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_branch_recorder_handler_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct __anonymous164 {
#define struct_branch_arc_t_type_p struct_branch_arc_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_branch_arc_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a branch_arc_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(branch_arc_t *, $source);
} default_value { (const branch_arc_t){ .addr_from = 0 } };

/* generated */
%typemap (python, struct-out) struct __anonymous164 {
#define struct_branch_arc_t_type_p struct_branch_arc_t_type_p
$target = new_opaque_object(&$source, false, false, struct_branch_arc_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous164 {
#define struct_branch_arc_t_type_p struct_branch_arc_t_type_p
	branch_arc_t *src = MM_MALLOC(1, branch_arc_t);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_branch_arc_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous164 * {
#define struct_branch_arc_t_type_p struct_branch_arc_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_branch_arc_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct __anonymous164 * {
#define struct_branch_arc_t_type_p struct_branch_arc_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_branch_arc_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct branch_arc_iter {
#define struct_branch_arc_iter_t_type_p struct_branch_arc_iter_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_branch_arc_iter_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a branch_arc_iter_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct branch_arc_iter *, $source);
} default_value { (const branch_arc_iter_t){ .next = 0 } };

/* generated */
%typemap (python, struct-out) struct branch_arc_iter {
#define struct_branch_arc_iter_t_type_p struct_branch_arc_iter_t_type_p
$target = new_opaque_object(&$source, false, false, struct_branch_arc_iter_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct branch_arc_iter {
#define struct_branch_arc_iter_t_type_p struct_branch_arc_iter_t_type_p
	struct branch_arc_iter *src = MM_MALLOC(1, struct branch_arc_iter);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_branch_arc_iter_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct branch_arc_iter * {
#define struct_branch_arc_iter_t_type_p struct_branch_arc_iter_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_branch_arc_iter_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct branch_arc_iter * {
#define struct_branch_arc_iter_t_type_p struct_branch_arc_iter_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_branch_arc_iter_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct branch_arc_interface {
#define struct_branch_arc_interface_t_type_p struct_branch_arc_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_branch_arc_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a branch_arc_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct branch_arc_interface *, $source);
} default_value { (const branch_arc_interface_t){ .iter = 0 } };

/* generated */
%typemap (python, struct-out) struct branch_arc_interface {
#define struct_branch_arc_interface_t_type_p struct_branch_arc_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_branch_arc_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct branch_arc_interface {
#define struct_branch_arc_interface_t_type_p struct_branch_arc_interface_t_type_p
	struct branch_arc_interface *src = MM_MALLOC(1, struct branch_arc_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_branch_arc_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct branch_arc_interface * {
#define struct_branch_arc_interface_t_type_p struct_branch_arc_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_branch_arc_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct branch_arc_interface * {
#define struct_branch_arc_interface_t_type_p struct_branch_arc_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_branch_arc_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct bp_manager_interface {
#define struct_bp_manager_interface_t_type_p struct_bp_manager_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_bp_manager_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a bp_manager_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct bp_manager_interface *, $source);
} default_value { (const bp_manager_interface_t){ .list_breakpoints = 0 } };

/* generated */
%typemap (python, struct-out) struct bp_manager_interface {
#define struct_bp_manager_interface_t_type_p struct_bp_manager_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_bp_manager_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct bp_manager_interface {
#define struct_bp_manager_interface_t_type_p struct_bp_manager_interface_t_type_p
	struct bp_manager_interface *src = MM_MALLOC(1, struct bp_manager_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_bp_manager_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct bp_manager_interface * {
#define struct_bp_manager_interface_t_type_p struct_bp_manager_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_bp_manager_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct bp_manager_interface * {
#define struct_bp_manager_interface_t_type_p struct_bp_manager_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_bp_manager_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct bank_instrumentation_subscribe_interface {
#define struct_bank_instrumentation_subscribe_interface_t_type_p struct_bank_instrumentation_subscribe_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_bank_instrumentation_subscribe_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a bank_instrumentation_subscribe_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct bank_instrumentation_subscribe_interface *, $source);
} default_value { (const bank_instrumentation_subscribe_interface_t){ .register_before_read = 0 } };

/* generated */
%typemap (python, struct-out) struct bank_instrumentation_subscribe_interface {
#define struct_bank_instrumentation_subscribe_interface_t_type_p struct_bank_instrumentation_subscribe_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_bank_instrumentation_subscribe_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct bank_instrumentation_subscribe_interface {
#define struct_bank_instrumentation_subscribe_interface_t_type_p struct_bank_instrumentation_subscribe_interface_t_type_p
	struct bank_instrumentation_subscribe_interface *src = MM_MALLOC(1, struct bank_instrumentation_subscribe_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_bank_instrumentation_subscribe_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct bank_instrumentation_subscribe_interface * {
#define struct_bank_instrumentation_subscribe_interface_t_type_p struct_bank_instrumentation_subscribe_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_bank_instrumentation_subscribe_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct bank_instrumentation_subscribe_interface * {
#define struct_bank_instrumentation_subscribe_interface_t_type_p struct_bank_instrumentation_subscribe_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_bank_instrumentation_subscribe_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct bank_before_write_interface {
#define struct_bank_before_write_interface_t_type_p struct_bank_before_write_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_bank_before_write_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a bank_before_write_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct bank_before_write_interface *, $source);
} default_value { (const bank_before_write_interface_t){ .offset = 0 } };

/* generated */
%typemap (python, struct-out) struct bank_before_write_interface {
#define struct_bank_before_write_interface_t_type_p struct_bank_before_write_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_bank_before_write_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct bank_before_write_interface {
#define struct_bank_before_write_interface_t_type_p struct_bank_before_write_interface_t_type_p
	struct bank_before_write_interface *src = MM_MALLOC(1, struct bank_before_write_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_bank_before_write_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct bank_before_write_interface * {
#define struct_bank_before_write_interface_t_type_p struct_bank_before_write_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_bank_before_write_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct bank_before_write_interface * {
#define struct_bank_before_write_interface_t_type_p struct_bank_before_write_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_bank_before_write_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct bank_before_read_interface {
#define struct_bank_before_read_interface_t_type_p struct_bank_before_read_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_bank_before_read_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a bank_before_read_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct bank_before_read_interface *, $source);
} default_value { (const bank_before_read_interface_t){ .offset = 0 } };

/* generated */
%typemap (python, struct-out) struct bank_before_read_interface {
#define struct_bank_before_read_interface_t_type_p struct_bank_before_read_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_bank_before_read_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct bank_before_read_interface {
#define struct_bank_before_read_interface_t_type_p struct_bank_before_read_interface_t_type_p
	struct bank_before_read_interface *src = MM_MALLOC(1, struct bank_before_read_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_bank_before_read_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct bank_before_read_interface * {
#define struct_bank_before_read_interface_t_type_p struct_bank_before_read_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_bank_before_read_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct bank_before_read_interface * {
#define struct_bank_before_read_interface_t_type_p struct_bank_before_read_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_bank_before_read_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct bank_after_write_interface {
#define struct_bank_after_write_interface_t_type_p struct_bank_after_write_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_bank_after_write_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a bank_after_write_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct bank_after_write_interface *, $source);
} default_value { (const bank_after_write_interface_t){ .offset = 0 } };

/* generated */
%typemap (python, struct-out) struct bank_after_write_interface {
#define struct_bank_after_write_interface_t_type_p struct_bank_after_write_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_bank_after_write_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct bank_after_write_interface {
#define struct_bank_after_write_interface_t_type_p struct_bank_after_write_interface_t_type_p
	struct bank_after_write_interface *src = MM_MALLOC(1, struct bank_after_write_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_bank_after_write_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct bank_after_write_interface * {
#define struct_bank_after_write_interface_t_type_p struct_bank_after_write_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_bank_after_write_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct bank_after_write_interface * {
#define struct_bank_after_write_interface_t_type_p struct_bank_after_write_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_bank_after_write_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct bank_access {
#define struct_bank_access_t_type_p struct_bank_access_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_bank_access_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a bank_access_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct bank_access *, $source);
};

/* generated */
%typemap (python, struct-out) struct bank_access {
#define struct_bank_access_t_type_p struct_bank_access_t_type_p
$target = new_opaque_object(&$source, false, false, struct_bank_access_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct bank_access {
#define struct_bank_access_t_type_p struct_bank_access_t_type_p
	struct bank_access *src = MM_MALLOC(1, struct bank_access);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_bank_access_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct bank_access * {
#define struct_bank_access_t_type_p struct_bank_access_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_bank_access_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct bank_access * {
#define struct_bank_access_t_type_p struct_bank_access_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_bank_access_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct bank_after_read_interface {
#define struct_bank_after_read_interface_t_type_p struct_bank_after_read_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_bank_after_read_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a bank_after_read_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct bank_after_read_interface *, $source);
} default_value { (const bank_after_read_interface_t){ .offset = 0 } };

/* generated */
%typemap (python, struct-out) struct bank_after_read_interface {
#define struct_bank_after_read_interface_t_type_p struct_bank_after_read_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_bank_after_read_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct bank_after_read_interface {
#define struct_bank_after_read_interface_t_type_p struct_bank_after_read_interface_t_type_p
	struct bank_after_read_interface *src = MM_MALLOC(1, struct bank_after_read_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_bank_after_read_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct bank_after_read_interface * {
#define struct_bank_after_read_interface_t_type_p struct_bank_after_read_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_bank_after_read_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct bank_after_read_interface * {
#define struct_bank_after_read_interface_t_type_p struct_bank_after_read_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_bank_after_read_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct arm_trustzone_interface {
#define struct_arm_trustzone_interface_t_type_p struct_arm_trustzone_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_arm_trustzone_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an arm_trustzone_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct arm_trustzone_interface *, $source);
} default_value { (const arm_trustzone_interface_t){ .get_security_mode = 0 } };

/* generated */
%typemap (python, struct-out) struct arm_trustzone_interface {
#define struct_arm_trustzone_interface_t_type_p struct_arm_trustzone_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_arm_trustzone_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct arm_trustzone_interface {
#define struct_arm_trustzone_interface_t_type_p struct_arm_trustzone_interface_t_type_p
	struct arm_trustzone_interface *src = MM_MALLOC(1, struct arm_trustzone_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_arm_trustzone_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct arm_trustzone_interface * {
#define struct_arm_trustzone_interface_t_type_p struct_arm_trustzone_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_arm_trustzone_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct arm_trustzone_interface * {
#define struct_arm_trustzone_interface_t_type_p struct_arm_trustzone_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_arm_trustzone_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct arm_gic_cpu_state_interface {
#define struct_arm_gic_cpu_state_interface_t_type_p struct_arm_gic_cpu_state_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_arm_gic_cpu_state_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an arm_gic_cpu_state_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct arm_gic_cpu_state_interface *, $source);
} default_value { (const arm_gic_cpu_state_interface_t){ .get_cpu_state = 0 } };

/* generated */
%typemap (python, struct-out) struct arm_gic_cpu_state_interface {
#define struct_arm_gic_cpu_state_interface_t_type_p struct_arm_gic_cpu_state_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_arm_gic_cpu_state_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct arm_gic_cpu_state_interface {
#define struct_arm_gic_cpu_state_interface_t_type_p struct_arm_gic_cpu_state_interface_t_type_p
	struct arm_gic_cpu_state_interface *src = MM_MALLOC(1, struct arm_gic_cpu_state_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_arm_gic_cpu_state_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct arm_gic_cpu_state_interface * {
#define struct_arm_gic_cpu_state_interface_t_type_p struct_arm_gic_cpu_state_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_arm_gic_cpu_state_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct arm_gic_cpu_state_interface * {
#define struct_arm_gic_cpu_state_interface_t_type_p struct_arm_gic_cpu_state_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_arm_gic_cpu_state_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct gic_reg_info {
#define struct_gic_reg_info_t_type_p struct_gic_reg_info_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_gic_reg_info_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not a gic_reg_info_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(gic_reg_info_t *, $source);
} default_value { (const gic_reg_info_t){ .op1 = 0 } };

/* generated */
%typemap (python, struct-out) struct gic_reg_info {
#define struct_gic_reg_info_t_type_p struct_gic_reg_info_t_type_p
$target = new_opaque_object(&$source, false, false, struct_gic_reg_info_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct gic_reg_info {
#define struct_gic_reg_info_t_type_p struct_gic_reg_info_t_type_p
	gic_reg_info_t *src = MM_MALLOC(1, struct gic_reg_info);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_gic_reg_info_t_type_p, NULL);
};

/* generated */
%typemap (python, out) gic_reg_info_t * {
#define struct_gic_reg_info_t_type_p struct_gic_reg_info_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_gic_reg_info_t_type_p, NULL);
};

/* generated */
%typemap (python, in) gic_reg_info_t * {
#define struct_gic_reg_info_t_type_p struct_gic_reg_info_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_gic_reg_info_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct arm_cpu_state {
#define struct_arm_cpu_state_t_type_p struct_arm_cpu_state_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_arm_cpu_state_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an arm_cpu_state_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(arm_cpu_state_t *, $source);
} default_value { (const arm_cpu_state_t){ .fmo_set = 0 } };

/* generated */
%typemap (python, struct-out) struct arm_cpu_state {
#define struct_arm_cpu_state_t_type_p struct_arm_cpu_state_t_type_p
$target = new_opaque_object(&$source, false, false, struct_arm_cpu_state_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct arm_cpu_state {
#define struct_arm_cpu_state_t_type_p struct_arm_cpu_state_t_type_p
	arm_cpu_state_t *src = MM_MALLOC(1, struct arm_cpu_state);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_arm_cpu_state_t_type_p, NULL);
};

/* generated */
%typemap (python, out) arm_cpu_state_t * {
#define struct_arm_cpu_state_t_type_p struct_arm_cpu_state_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_arm_cpu_state_t_type_p, NULL);
};

/* generated */
%typemap (python, in) arm_cpu_state_t * {
#define struct_arm_cpu_state_t_type_p struct_arm_cpu_state_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_arm_cpu_state_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct arm_gic_interface {
#define struct_arm_gic_interface_t_type_p struct_arm_gic_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_arm_gic_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an arm_gic_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct arm_gic_interface *, $source);
} default_value { (const arm_gic_interface_t){ .read_register = 0 } };

/* generated */
%typemap (python, struct-out) struct arm_gic_interface {
#define struct_arm_gic_interface_t_type_p struct_arm_gic_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_arm_gic_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct arm_gic_interface {
#define struct_arm_gic_interface_t_type_p struct_arm_gic_interface_t_type_p
	struct arm_gic_interface *src = MM_MALLOC(1, struct arm_gic_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_arm_gic_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct arm_gic_interface * {
#define struct_arm_gic_interface_t_type_p struct_arm_gic_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_arm_gic_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct arm_gic_interface * {
#define struct_arm_gic_interface_t_type_p struct_arm_gic_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_arm_gic_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct arm_external_debug_interface {
#define struct_arm_external_debug_interface_t_type_p struct_arm_external_debug_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_arm_external_debug_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an arm_external_debug_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct arm_external_debug_interface *, $source);
} default_value { (const arm_external_debug_interface_t){ .handle_semihosting = 0 } };

/* generated */
%typemap (python, struct-out) struct arm_external_debug_interface {
#define struct_arm_external_debug_interface_t_type_p struct_arm_external_debug_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_arm_external_debug_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct arm_external_debug_interface {
#define struct_arm_external_debug_interface_t_type_p struct_arm_external_debug_interface_t_type_p
	struct arm_external_debug_interface *src = MM_MALLOC(1, struct arm_external_debug_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_arm_external_debug_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct arm_external_debug_interface * {
#define struct_arm_external_debug_interface_t_type_p struct_arm_external_debug_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_arm_external_debug_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct arm_external_debug_interface * {
#define struct_arm_external_debug_interface_t_type_p struct_arm_external_debug_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_arm_external_debug_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct arm_cpu_group_tlb_interface {
#define struct_arm_cpu_group_tlb_interface_t_type_p struct_arm_cpu_group_tlb_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_arm_cpu_group_tlb_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an arm_cpu_group_tlb_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct arm_cpu_group_tlb_interface *, $source);
} default_value { (const arm_cpu_group_tlb_interface_t){ .invalidate_tlb = 0 } };

/* generated */
%typemap (python, struct-out) struct arm_cpu_group_tlb_interface {
#define struct_arm_cpu_group_tlb_interface_t_type_p struct_arm_cpu_group_tlb_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_arm_cpu_group_tlb_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct arm_cpu_group_tlb_interface {
#define struct_arm_cpu_group_tlb_interface_t_type_p struct_arm_cpu_group_tlb_interface_t_type_p
	struct arm_cpu_group_tlb_interface *src = MM_MALLOC(1, struct arm_cpu_group_tlb_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_arm_cpu_group_tlb_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct arm_cpu_group_tlb_interface * {
#define struct_arm_cpu_group_tlb_interface_t_type_p struct_arm_cpu_group_tlb_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_arm_cpu_group_tlb_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct arm_cpu_group_tlb_interface * {
#define struct_arm_cpu_group_tlb_interface_t_type_p struct_arm_cpu_group_tlb_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_arm_cpu_group_tlb_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct arm_cpu_group_exclusive_interface {
#define struct_arm_cpu_group_exclusive_interface_t_type_p struct_arm_cpu_group_exclusive_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_arm_cpu_group_exclusive_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an arm_cpu_group_exclusive_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct arm_cpu_group_exclusive_interface *, $source);
} default_value { (const arm_cpu_group_exclusive_interface_t){ .mark_exclusive = 0 } };

/* generated */
%typemap (python, struct-out) struct arm_cpu_group_exclusive_interface {
#define struct_arm_cpu_group_exclusive_interface_t_type_p struct_arm_cpu_group_exclusive_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_arm_cpu_group_exclusive_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct arm_cpu_group_exclusive_interface {
#define struct_arm_cpu_group_exclusive_interface_t_type_p struct_arm_cpu_group_exclusive_interface_t_type_p
	struct arm_cpu_group_exclusive_interface *src = MM_MALLOC(1, struct arm_cpu_group_exclusive_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_arm_cpu_group_exclusive_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct arm_cpu_group_exclusive_interface * {
#define struct_arm_cpu_group_exclusive_interface_t_type_p struct_arm_cpu_group_exclusive_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_arm_cpu_group_exclusive_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct arm_cpu_group_exclusive_interface * {
#define struct_arm_cpu_group_exclusive_interface_t_type_p struct_arm_cpu_group_exclusive_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_arm_cpu_group_exclusive_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct arm_cpu_group_event_interface {
#define struct_arm_cpu_group_event_interface_t_type_p struct_arm_cpu_group_event_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_arm_cpu_group_event_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an arm_cpu_group_event_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct arm_cpu_group_event_interface *, $source);
} default_value { (const arm_cpu_group_event_interface_t){ .signal_event = 0 } };

/* generated */
%typemap (python, struct-out) struct arm_cpu_group_event_interface {
#define struct_arm_cpu_group_event_interface_t_type_p struct_arm_cpu_group_event_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_arm_cpu_group_event_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct arm_cpu_group_event_interface {
#define struct_arm_cpu_group_event_interface_t_type_p struct_arm_cpu_group_event_interface_t_type_p
	struct arm_cpu_group_event_interface *src = MM_MALLOC(1, struct arm_cpu_group_event_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_arm_cpu_group_event_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct arm_cpu_group_event_interface * {
#define struct_arm_cpu_group_event_interface_t_type_p struct_arm_cpu_group_event_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_arm_cpu_group_event_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct arm_cpu_group_event_interface * {
#define struct_arm_cpu_group_event_interface_t_type_p struct_arm_cpu_group_event_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_arm_cpu_group_event_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct arm_coprocessor_interface {
#define struct_arm_coprocessor_interface_t_type_p struct_arm_coprocessor_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_arm_coprocessor_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an arm_coprocessor_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct arm_coprocessor_interface *, $source);
} default_value { (const arm_coprocessor_interface_t){ .process_data = 0 } };

/* generated */
%typemap (python, struct-out) struct arm_coprocessor_interface {
#define struct_arm_coprocessor_interface_t_type_p struct_arm_coprocessor_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_arm_coprocessor_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct arm_coprocessor_interface {
#define struct_arm_coprocessor_interface_t_type_p struct_arm_coprocessor_interface_t_type_p
	struct arm_coprocessor_interface *src = MM_MALLOC(1, struct arm_coprocessor_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_arm_coprocessor_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct arm_coprocessor_interface * {
#define struct_arm_coprocessor_interface_t_type_p struct_arm_coprocessor_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_arm_coprocessor_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct arm_coprocessor_interface * {
#define struct_arm_coprocessor_interface_t_type_p struct_arm_coprocessor_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_arm_coprocessor_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct arm_avic {
#define struct_arm_avic_t_type_p struct_arm_avic_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_arm_avic_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an arm_avic_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(arm_avic_t *, $source);
} default_value { (const arm_avic_t){ .valid = 0 } };

/* generated */
%typemap (python, struct-out) struct arm_avic {
#define struct_arm_avic_t_type_p struct_arm_avic_t_type_p
$target = new_opaque_object(&$source, false, false, struct_arm_avic_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct arm_avic {
#define struct_arm_avic_t_type_p struct_arm_avic_t_type_p
	arm_avic_t *src = MM_MALLOC(1, struct arm_avic);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_arm_avic_t_type_p, NULL);
};

/* generated */
%typemap (python, out) arm_avic_t * {
#define struct_arm_avic_t_type_p struct_arm_avic_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_arm_avic_t_type_p, NULL);
};

/* generated */
%typemap (python, in) arm_avic_t * {
#define struct_arm_avic_t_type_p struct_arm_avic_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_arm_avic_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct arm_avic_interface {
#define struct_arm_avic_interface_t_type_p struct_arm_avic_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_arm_avic_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an arm_avic_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct arm_avic_interface *, $source);
} default_value { (const arm_avic_interface_t){ .get_interrupt_address = 0 } };

/* generated */
%typemap (python, struct-out) struct arm_avic_interface {
#define struct_arm_avic_interface_t_type_p struct_arm_avic_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_arm_avic_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct arm_avic_interface {
#define struct_arm_avic_interface_t_type_p struct_arm_avic_interface_t_type_p
	struct arm_avic_interface *src = MM_MALLOC(1, struct arm_avic_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_arm_avic_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct arm_avic_interface * {
#define struct_arm_avic_interface_t_type_p struct_arm_avic_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_arm_avic_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct arm_avic_interface * {
#define struct_arm_avic_interface_t_type_p struct_arm_avic_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_arm_avic_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct arm_interface {
#define struct_arm_interface_t_type_p struct_arm_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_arm_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an arm_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct arm_interface *, $source);
} default_value { (const arm_interface_t){ .read_register_mode = 0 } };

/* generated */
%typemap (python, struct-out) struct arm_interface {
#define struct_arm_interface_t_type_p struct_arm_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_arm_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct arm_interface {
#define struct_arm_interface_t_type_p struct_arm_interface_t_type_p
	struct arm_interface *src = MM_MALLOC(1, struct arm_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_arm_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct arm_interface * {
#define struct_arm_interface_t_type_p struct_arm_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_arm_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct arm_interface * {
#define struct_arm_interface_t_type_p struct_arm_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_arm_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct arinc429_receiver_interface {
#define struct_arinc429_receiver_interface_t_type_p struct_arinc429_receiver_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_arinc429_receiver_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an arinc429_receiver_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct arinc429_receiver_interface *, $source);
} default_value { (const arinc429_receiver_interface_t){ .receive_word = 0 } };

/* generated */
%typemap (python, struct-out) struct arinc429_receiver_interface {
#define struct_arinc429_receiver_interface_t_type_p struct_arinc429_receiver_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_arinc429_receiver_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct arinc429_receiver_interface {
#define struct_arinc429_receiver_interface_t_type_p struct_arinc429_receiver_interface_t_type_p
	struct arinc429_receiver_interface *src = MM_MALLOC(1, struct arinc429_receiver_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_arinc429_receiver_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct arinc429_receiver_interface * {
#define struct_arinc429_receiver_interface_t_type_p struct_arinc429_receiver_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_arinc429_receiver_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct arinc429_receiver_interface * {
#define struct_arinc429_receiver_interface_t_type_p struct_arinc429_receiver_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_arinc429_receiver_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct arinc429_bus_interface {
#define struct_arinc429_bus_interface_t_type_p struct_arinc429_bus_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_arinc429_bus_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an arinc429_bus_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct arinc429_bus_interface *, $source);
} default_value { (const arinc429_bus_interface_t){ .send_word = 0 } };

/* generated */
%typemap (python, struct-out) struct arinc429_bus_interface {
#define struct_arinc429_bus_interface_t_type_p struct_arinc429_bus_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_arinc429_bus_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct arinc429_bus_interface {
#define struct_arinc429_bus_interface_t_type_p struct_arinc429_bus_interface_t_type_p
	struct arinc429_bus_interface *src = MM_MALLOC(1, struct arinc429_bus_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_arinc429_bus_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct arinc429_bus_interface * {
#define struct_arinc429_bus_interface_t_type_p struct_arinc429_bus_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_arinc429_bus_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct arinc429_bus_interface * {
#define struct_arinc429_bus_interface_t_type_p struct_arinc429_bus_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_arinc429_bus_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct apic_cpu_interface {
#define struct_apic_cpu_interface_t_type_p struct_apic_cpu_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_apic_cpu_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an apic_cpu_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct apic_cpu_interface *, $source);
} default_value { (const apic_cpu_interface_t){ .tpr_r = 0 } };

/* generated */
%typemap (python, struct-out) struct apic_cpu_interface {
#define struct_apic_cpu_interface_t_type_p struct_apic_cpu_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_apic_cpu_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct apic_cpu_interface {
#define struct_apic_cpu_interface_t_type_p struct_apic_cpu_interface_t_type_p
	struct apic_cpu_interface *src = MM_MALLOC(1, struct apic_cpu_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_apic_cpu_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct apic_cpu_interface * {
#define struct_apic_cpu_interface_t_type_p struct_apic_cpu_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_apic_cpu_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct apic_cpu_interface * {
#define struct_apic_cpu_interface_t_type_p struct_apic_cpu_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_apic_cpu_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct apic_bus_interface {
#define struct_apic_bus_interface_t_type_p struct_apic_bus_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_apic_bus_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an apic_bus_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct apic_bus_interface *, $source);
} default_value { (const apic_bus_interface_t){ .interrupt = 0 } };

/* generated */
%typemap (python, struct-out) struct apic_bus_interface {
#define struct_apic_bus_interface_t_type_p struct_apic_bus_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_apic_bus_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct apic_bus_interface {
#define struct_apic_bus_interface_t_type_p struct_apic_bus_interface_t_type_p
	struct apic_bus_interface *src = MM_MALLOC(1, struct apic_bus_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_apic_bus_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct apic_bus_interface * {
#define struct_apic_bus_interface_t_type_p struct_apic_bus_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_apic_bus_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct apic_bus_interface * {
#define struct_apic_bus_interface_t_type_p struct_apic_bus_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_apic_bus_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct alg_connection {
#define struct_alg_connection_t_type_p struct_alg_connection_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_alg_connection_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an alg_connection_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct alg_connection *, $source);
};

/* generated */
%typemap (python, struct-out) struct alg_connection {
#define struct_alg_connection_t_type_p struct_alg_connection_t_type_p
$target = new_opaque_object(&$source, false, false, struct_alg_connection_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct alg_connection {
#define struct_alg_connection_t_type_p struct_alg_connection_t_type_p
	struct alg_connection *src = MM_MALLOC(1, struct alg_connection);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_alg_connection_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct alg_connection * {
#define struct_alg_connection_t_type_p struct_alg_connection_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_alg_connection_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct alg_connection * {
#define struct_alg_connection_t_type_p struct_alg_connection_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_alg_connection_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct alg_interface {
#define struct_alg_interface_t_type_p struct_alg_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_alg_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an alg_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct alg_interface *, $source);
} default_value { (const alg_interface_t){ .close_connection = 0 } };

/* generated */
%typemap (python, struct-out) struct alg_interface {
#define struct_alg_interface_t_type_p struct_alg_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_alg_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct alg_interface {
#define struct_alg_interface_t_type_p struct_alg_interface_t_type_p
	struct alg_interface *src = MM_MALLOC(1, struct alg_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_alg_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct alg_interface * {
#define struct_alg_interface_t_type_p struct_alg_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_alg_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct alg_interface * {
#define struct_alg_interface_t_type_p struct_alg_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_alg_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct addr_prof_iter {
#define struct_addr_prof_iter_t_type_p struct_addr_prof_iter_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_addr_prof_iter_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an addr_prof_iter_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct addr_prof_iter *, $source);
} default_value { (const addr_prof_iter_t){ .next = 0 } };

/* generated */
%typemap (python, struct-out) struct addr_prof_iter {
#define struct_addr_prof_iter_t_type_p struct_addr_prof_iter_t_type_p
$target = new_opaque_object(&$source, false, false, struct_addr_prof_iter_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct addr_prof_iter {
#define struct_addr_prof_iter_t_type_p struct_addr_prof_iter_t_type_p
	struct addr_prof_iter *src = MM_MALLOC(1, struct addr_prof_iter);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_addr_prof_iter_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct addr_prof_iter * {
#define struct_addr_prof_iter_t_type_p struct_addr_prof_iter_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_addr_prof_iter_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct addr_prof_iter * {
#define struct_addr_prof_iter_t_type_p struct_addr_prof_iter_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_addr_prof_iter_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct address_profiler_interface {
#define struct_address_profiler_interface_t_type_p struct_address_profiler_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_address_profiler_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an address_profiler_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct address_profiler_interface *, $source);
} default_value { (const address_profiler_interface_t){ .iter = 0 } };

/* generated */
%typemap (python, struct-out) struct address_profiler_interface {
#define struct_address_profiler_interface_t_type_p struct_address_profiler_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_address_profiler_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct address_profiler_interface {
#define struct_address_profiler_interface_t_type_p struct_address_profiler_interface_t_type_p
	struct address_profiler_interface *src = MM_MALLOC(1, struct address_profiler_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_address_profiler_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct address_profiler_interface * {
#define struct_address_profiler_interface_t_type_p struct_address_profiler_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_address_profiler_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct address_profiler_interface * {
#define struct_address_profiler_interface_t_type_p struct_address_profiler_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_address_profiler_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct abs_pointer_activate_interface {
#define struct_abs_pointer_activate_interface_t_type_p struct_abs_pointer_activate_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_abs_pointer_activate_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an abs_pointer_activate_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct abs_pointer_activate_interface *, $source);
} default_value { (const abs_pointer_activate_interface_t){ .enable = 0 } };

/* generated */
%typemap (python, struct-out) struct abs_pointer_activate_interface {
#define struct_abs_pointer_activate_interface_t_type_p struct_abs_pointer_activate_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_abs_pointer_activate_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct abs_pointer_activate_interface {
#define struct_abs_pointer_activate_interface_t_type_p struct_abs_pointer_activate_interface_t_type_p
	struct abs_pointer_activate_interface *src = MM_MALLOC(1, struct abs_pointer_activate_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_abs_pointer_activate_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct abs_pointer_activate_interface * {
#define struct_abs_pointer_activate_interface_t_type_p struct_abs_pointer_activate_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_abs_pointer_activate_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct abs_pointer_activate_interface * {
#define struct_abs_pointer_activate_interface_t_type_p struct_abs_pointer_activate_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_abs_pointer_activate_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct __anonymous175 {
#define struct_abs_pointer_state_t_type_p struct_abs_pointer_state_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_abs_pointer_state_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an abs_pointer_state_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(abs_pointer_state_t *, $source);
} default_value { (const abs_pointer_state_t){ .buttons = 0 } };

/* generated */
%typemap (python, struct-out) struct __anonymous175 {
#define struct_abs_pointer_state_t_type_p struct_abs_pointer_state_t_type_p
$target = new_opaque_object(&$source, false, false, struct_abs_pointer_state_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct __anonymous175 {
#define struct_abs_pointer_state_t_type_p struct_abs_pointer_state_t_type_p
	abs_pointer_state_t *src = MM_MALLOC(1, abs_pointer_state_t);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_abs_pointer_state_t_type_p, NULL);
};

/* generated */
%typemap (python, out) abs_pointer_state_t * {
#define struct_abs_pointer_state_t_type_p struct_abs_pointer_state_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_abs_pointer_state_t_type_p, NULL);
};

/* generated */
%typemap (python, in) abs_pointer_state_t * {
#define struct_abs_pointer_state_t_type_p struct_abs_pointer_state_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_abs_pointer_state_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct abs_pointer_interface {
#define struct_abs_pointer_interface_t_type_p struct_abs_pointer_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_abs_pointer_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an abs_pointer_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct abs_pointer_interface *, $source);
} default_value { (const abs_pointer_interface_t){ .set_state = 0 } };

/* generated */
%typemap (python, struct-out) struct abs_pointer_interface {
#define struct_abs_pointer_interface_t_type_p struct_abs_pointer_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_abs_pointer_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct abs_pointer_interface {
#define struct_abs_pointer_interface_t_type_p struct_abs_pointer_interface_t_type_p
	struct abs_pointer_interface *src = MM_MALLOC(1, struct abs_pointer_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_abs_pointer_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct abs_pointer_interface * {
#define struct_abs_pointer_interface_t_type_p struct_abs_pointer_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_abs_pointer_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct abs_pointer_interface * {
#define struct_abs_pointer_interface_t_type_p struct_abs_pointer_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_abs_pointer_interface_t_type_p)))
		$error;
	$target = tgt;
};

/* generated */
%typemap (python, in) struct a20_interface {
#define struct_a20_interface_t_type_p struct_a20_interface_t_type_p
	if (Py_TYPE($source) != VT_get_opaque_python_type(
		struct_a20_interface_t_type_p)) {
		PyErr_SetString(PyExc_TypeError, "not an a20_interface_t");
		$error;
	}
	$target = *PY_OPAQUE_DATA(struct a20_interface *, $source);
} default_value { (const a20_interface_t){ .set_a20_line = 0 } };

/* generated */
%typemap (python, struct-out) struct a20_interface {
#define struct_a20_interface_t_type_p struct_a20_interface_t_type_p
$target = new_opaque_object(&$source, false, false, struct_a20_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct a20_interface {
#define struct_a20_interface_t_type_p struct_a20_interface_t_type_p
	struct a20_interface *src = MM_MALLOC(1, struct a20_interface);
	*src = $source;
	$target = new_opaque_object(src, true, false, struct_a20_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, out) struct a20_interface * {
#define struct_a20_interface_t_type_p struct_a20_interface_t_type_p
$target = new_opaque_object((void *)$source, false, false, struct_a20_interface_t_type_p, NULL);
};

/* generated */
%typemap (python, in) struct a20_interface * {
#define struct_a20_interface_t_type_p struct_a20_interface_t_type_p
	void *tgt = NULL;
	if (!get_py_opaque(&tgt, $source,
	            VT_get_opaque_python_type(struct_a20_interface_t_type_p)))
		$error;
	$target = tgt;
};

%body (python) {
static UNUSED bool
get_set_error_t(PyObject *from, set_error_t *to)
{
        int i;
        if (!get_int(from, &i))
                return false;
        if (i < 0 || i >= Sim_Set_Error_Types) {
                PyErr_SetString(PyExc_OverflowError,
                                "out of range for set_error_t");
                return false;
        }
        *to = i;
        return true;
}
};

#endif /* PYWRAPGEN_TYPEMAPS */
