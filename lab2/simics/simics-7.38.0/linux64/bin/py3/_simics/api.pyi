def CORE_absolutify_path(path, base):
    '''
    char *CORE_absolutify_path(const char *NOTNULL path, const char *NOTNULL base)
    '''
def CORE_acquire_cell(obj, function_name, source_location):
    '''
    domain_lock_t *CORE_acquire_cell(conf_object_t *NOTNULL obj, const char *NOTNULL function_name, const char *NOTNULL source_location)
    '''
def CORE_acquire_object(obj, function_name, source_location):
    '''
    domain_lock_t *CORE_acquire_object(conf_object_t *NOTNULL obj, const char *NOTNULL function_name, const char *NOTNULL source_location)
    '''
def CORE_acquire_target(obj, function_name, source_location):
    '''
    domain_lock_t *CORE_acquire_target(conf_object_t *NOTNULL obj, const char *NOTNULL function_name, const char *NOTNULL source_location)
    '''
def CORE_attribute_error(revex_ignore, msg):
    '''
    void CORE_attribute_error(bool revex_ignore, const char *NOTNULL msg)
    '''
def CORE_clear_interrupt_script():
    '''
    unsigned CORE_clear_interrupt_script(void)
    '''
def CORE_clear_object_lock_stats():
    '''
    void CORE_clear_object_lock_stats(void)
    '''
def CORE_clear_stopped_on_error():
    '''
    bool CORE_clear_stopped_on_error(void)
    '''
def CORE_create_script_branch(branch, desc, cli_branch, caller, filename, line):
    '''
    int CORE_create_script_branch(SIM_PYOBJECT *branch, const char *desc, bool cli_branch, const char *caller, const char *filename, int line)
    '''
def CORE_customize_prompt(prompt):
    '''
    char *CORE_customize_prompt(const char *NOTNULL prompt)
    '''
def CORE_disable_object_lock_stats():
    '''
    void CORE_disable_object_lock_stats(void)
    '''
def CORE_enable_object_lock_stats():
    '''
    void CORE_enable_object_lock_stats(void)
    '''
def CORE_error_interrupt(msg):
    '''
    void CORE_error_interrupt(const char *NOTNULL msg)
    '''
def CORE_forget_module_class(class_name):
    '''
    void CORE_forget_module_class(const char *class_name)
    '''
def CORE_generate_modcache(distfile_list):
    '''
    int CORE_generate_modcache(const char *distfile_list)
    '''
def CORE_generate_modcache_from_module_list(dest, files):
    '''
    bool CORE_generate_modcache_from_module_list(const char *dest, attr_value_t files)
    '''
def CORE_get_all_class_aliases():
    '''
    attr_value_t CORE_get_all_class_aliases(void)
    '''
def CORE_get_all_known_module_classes():
    '''
    attr_value_t CORE_get_all_known_module_classes(void)
    '''
def CORE_get_class_notifiers(cls, include_all):
    '''
    attr_value_t CORE_get_class_notifiers(conf_class_t *NOTNULL cls, bool include_all)
    '''
def CORE_get_current_loading_module():
    '''
    const char *CORE_get_current_loading_module(void)
    '''
def CORE_get_deferred_transactions_info():
    '''
    deferred_transaction_info_vect_t CORE_get_deferred_transactions_info(void)
    '''
def CORE_get_extra_module_info(name):
    '''
    attr_value_t CORE_get_extra_module_info(const char *NOTNULL name)
    '''
def CORE_get_global_notifier_subscribers():
    '''
    attr_value_t CORE_get_global_notifier_subscribers(void)
    '''
def CORE_get_implemented_haps(m):
    '''
    attr_value_t CORE_get_implemented_haps(const char *NOTNULL m)
    '''
def CORE_get_last_break_object():
    '''
    conf_object_t *CORE_get_last_break_object(void)
    '''
def CORE_get_last_loaded_module():
    '''
    const char *CORE_get_last_loaded_module(void)
    '''
def CORE_get_module_path(module_name):
    '''
    const char *CORE_get_module_path(const char *NOTNULL module_name)
    '''
def CORE_get_notifier_subscribers(obj):
    '''
    attr_value_t CORE_get_notifier_subscribers(conf_object_t *NOTNULL obj)
    '''
def CORE_get_object_lock_stats(obj):
    '''
    attr_value_t CORE_get_object_lock_stats(conf_object_t *NOTNULL obj)
    '''
def CORE_get_port_description(cls, portname):
    '''
    const char *CORE_get_port_description(const conf_class_t *NOTNULL cls, const char *NOTNULL portname)
    '''
def CORE_get_py_class_data(cls):
    '''
    SIM_PYOBJECT *CORE_get_py_class_data(conf_class_t *NOTNULL cls)
    '''
def CORE_get_realtime_queue():
    '''
    attr_value_t CORE_get_realtime_queue(void)
    '''
def CORE_get_screen_width():
    '''
    int CORE_get_screen_width(void)
    '''
def CORE_get_script_branch_id():
    '''
    int CORE_get_script_branch_id(void)
    '''
def CORE_get_settings_dir():
    '''
    const char *CORE_get_settings_dir(void)
    '''
def CORE_global_notify(type):
    '''
    void CORE_global_notify(global_notifier_type_t type)
    '''
def CORE_host_cpuid_brand_string():
    '''
    const char *CORE_host_cpuid_brand_string(void)
    '''
def CORE_host_cpuid_fms():
    '''
    uint32 CORE_host_cpuid_fms(void)
    '''
def CORE_host_hypervisor_info():
    '''
    host_hypervisor_info_t CORE_host_hypervisor_info(void)
    '''
def CORE_image_page_dirty(obj, ofs):
    '''
    bool CORE_image_page_dirty(conf_object_t *NOTNULL obj, uint64 ofs)
    '''
def CORE_image_page_exists(obj, ofs):
    '''
    bool CORE_image_page_exists(conf_object_t *NOTNULL obj, uint64 ofs)
    '''
def CORE_init_from_python():
    '''
    void CORE_init_from_python(void)
    '''
def CORE_inspect_transaction_terminus(mt, t, addr, max_depth):
    '''
    terminus_cb_data_t CORE_inspect_transaction_terminus(const map_target_t *NOTNULL mt, transaction_t *NOTNULL t, uint64 addr, unsigned int max_depth)
    '''
def CORE_interrupt_script_branch(id):
    '''
    int CORE_interrupt_script_branch(int id)
    '''
def CORE_is_active_frame(frame):
    '''
    bool CORE_is_active_frame(lang_void *frame)
    '''
def CORE_is_cli_script_branch():
    '''
    bool CORE_is_cli_script_branch(void)
    '''
def CORE_is_intel_lic():
    '''
    bool CORE_is_intel_lic(void)
    '''
def CORE_is_permanent_object_name(objname):
    '''
    bool CORE_is_permanent_object_name(const char *NOTNULL objname)
    '''
def CORE_load_binary(obj, file, offset, use_pa, verbose, no_clear):
    '''
    physical_address_t CORE_load_binary(conf_object_t *NOTNULL obj, const char *NOTNULL file, physical_address_t offset, bool use_pa, bool verbose, bool no_clear)
    '''
def CORE_load_intel_obj_file(obj, file):
    '''
    attr_value_t CORE_load_intel_obj_file(conf_object_t *NOTNULL obj, const char *NOTNULL file)
    '''
def CORE_magic_break_query():
    '''
    bool CORE_magic_break_query(void)
    '''
def CORE_make_script_branch_deferrable_begin():
    '''
    void CORE_make_script_branch_deferrable_begin(void)
    '''
def CORE_make_script_branch_deferrable_end():
    '''
    void CORE_make_script_branch_deferrable_end(void)
    '''
def CORE_module_has_commands(module, report_global_commands):
    '''
    bool CORE_module_has_commands(const char *NOTNULL module, bool report_global_commands)
    '''
def CORE_num_execution_threads():
    '''
    int CORE_num_execution_threads(void)
    '''
def CORE_num_sim_obj():
    '''
    unsigned CORE_num_sim_obj(void)
    '''
def CORE_object_iterator(obj):
    '''
    SIM_PYOBJECT *CORE_object_iterator(conf_object_t *obj)
    '''
def CORE_object_iterator_for_class(cls):
    '''
    SIM_PYOBJECT *CORE_object_iterator_for_class(conf_class_t *NOTNULL cls)
    '''
def CORE_object_iterator_for_interface(ifaces):
    '''
    SIM_PYOBJECT *CORE_object_iterator_for_interface(attr_value_t ifaces)
    '''
def CORE_object_lock_stats_enabled():
    '''
    bool CORE_object_lock_stats_enabled(void)
    '''
def CORE_pop_current_loading_module():
    '''
    void CORE_pop_current_loading_module(void)
    '''
def CORE_print_telemetry_notice():
    '''
    void CORE_print_telemetry_notice(void)
    '''
def CORE_probe_translator(initiator, address, target, port, map_base, map_offset, map_length, map_func, map_prio, map_align, map_swap):
    '''
    probe_result_t CORE_probe_translator(conf_object_t *initiator, uint64 address, conf_object_t *NOTNULL target, const char *port, uint64 map_base, uint64 map_offset, uint64 map_length, int map_func, int16 map_prio, int map_align, swap_mode_t map_swap)
    '''
def CORE_process_top_domain():
    '''
    conf_object_t *CORE_process_top_domain(void)
    '''
def CORE_push_current_loading_module(m):
    '''
    void CORE_push_current_loading_module(const char *NOTNULL m)
    '''
def CORE_python_flush():
    '''
    void CORE_python_flush(void)
    '''
def CORE_python_free_map_target(obj):
    '''
    void CORE_python_free_map_target(SIM_PYOBJECT *NOTNULL obj)
    '''
def CORE_python_run_code(code, file_path):
    '''
    void CORE_python_run_code(const char *NOTNULL code, const char *file_path)
    '''
def CORE_python_write(src):
    '''
    void CORE_python_write(const byte_string_t src)
    '''
def CORE_read_phys_memory_tags_mask(mem_space, paddr, ntags):
    '''
    uint64 CORE_read_phys_memory_tags_mask(conf_object_t *NOTNULL mem_space, physical_address_t paddr, unsigned ntags)
    '''
def CORE_restart_simics(argv_list):
    '''
    void CORE_restart_simics(attr_value_t *argv_list)
    '''
def CORE_restore_global_checkpoint_paths():
    '''
    void CORE_restore_global_checkpoint_paths(void)
    '''
def CORE_run_target(target, ns, presets, preset_yml, cmdline_args, local):
    '''
    attr_value_t CORE_run_target(const char *NOTNULL target, attr_value_t ns, attr_value_t presets, attr_value_t preset_yml, attr_value_t cmdline_args, bool local)
    '''
def CORE_save_global_checkpoint_paths():
    '''
    void CORE_save_global_checkpoint_paths(void)
    '''
def CORE_save_preferences():
    '''
    void CORE_save_preferences(void)
    '''
def CORE_set_atom_tracing(enable):
    '''
    bool CORE_set_atom_tracing(bool enable)
    '''
def CORE_set_module_searchpath(paths, num_user):
    '''
    void CORE_set_module_searchpath(attr_value_t paths, unsigned num_user)
    '''
def CORE_set_py_class_data(cls, data):
    '''
    void CORE_set_py_class_data(conf_class_t *NOTNULL cls, SIM_PYOBJECT *data)
    '''
def CORE_set_settings_dir(val):
    '''
    void CORE_set_settings_dir(const char *NOTNULL val)
    '''
def CORE_set_thread_name(name):
    '''
    void CORE_set_thread_name(const char *name)
    '''
def CORE_shallow_object_iterator(obj, expand_arrays):
    '''
    SIM_PYOBJECT *CORE_shallow_object_iterator(conf_object_t *obj, bool expand_arrays)
    '''
def CORE_wake_script_branch(wait_id):
    '''
    bool CORE_wake_script_branch(int wait_id)
    '''
def CORE_write_configuration_objects(file, obj_list, save_queue, flags):
    '''
    int CORE_write_configuration_objects(const char *NOTNULL file, attr_value_t obj_list, bool save_queue, save_flags_t flags)
    '''
def CORE_write_pre_conf_objects(file, pobjs, flags):
    '''
    void CORE_write_pre_conf_objects(const char *file, pre_conf_object_set_t *pobjs, save_flags_t flags)
    '''
def DBG_check_typing_system(type, val):
    '''
    int DBG_check_typing_system(const char *type, attr_value_t *NOTNULL val)
    '''
def DBG_control_c_handler():
    '''
    void DBG_control_c_handler(void)
    '''
def DBG_crash(use_assert):
    '''
    void DBG_crash(bool use_assert)
    '''
def DBG_delete_object_by_name(name):
    '''
    void DBG_delete_object_by_name(const char *NOTNULL name)
    '''
def DBG_deliver_messages(h):
    '''
    void DBG_deliver_messages(thread_domain_holder_t *NOTNULL h)
    '''
def DBG_domain_multiplicity(d):
    '''
    int DBG_domain_multiplicity(thread_domain_t *NOTNULL d)
    '''
def DBG_get_dm_tag(t, tag_index):
    '''
    bool DBG_get_dm_tag(direct_memory_tags_t t, unsigned tag_index)
    '''
def DBG_get_holder_elevation_count(h):
    '''
    unsigned DBG_get_holder_elevation_count(thread_domain_holder_t *NOTNULL h)
    '''
def DBG_get_holder_nesting_count():
    '''
    unsigned DBG_get_holder_nesting_count(void)
    '''
def DBG_get_holder_retain_count(h):
    '''
    unsigned DBG_get_holder_retain_count(thread_domain_holder_t *NOTNULL h)
    '''
def DBG_get_io_cache(obj):
    '''
    attr_value_t DBG_get_io_cache(conf_object_t *NOTNULL obj)
    '''
def DBG_get_io_cache_counters(obj):
    '''
    attr_value_t DBG_get_io_cache_counters(conf_object_t *NOTNULL obj)
    '''
def DBG_get_pending_exception():
    '''
    sim_exception_t DBG_get_pending_exception(void)
    '''
def DBG_infinite_loop():
    '''
    void DBG_infinite_loop(void)
    '''
def DBG_license_files():
    '''
    void DBG_license_files(void)
    '''
def DBG_magic_instruction(arg):
    '''
    void DBG_magic_instruction(int arg)
    '''
def DBG_make_class_deletable(cls):
    '''
    bool DBG_make_class_deletable(conf_class_t *NOTNULL cls)
    '''
def DBG_make_page_bank_grant(page_size):
    '''
    page_bank_grant_t DBG_make_page_bank_grant(size_t page_size)
    '''
def DBG_make_pb_page(coalloc_size):
    '''
    pb_page_t *DBG_make_pb_page(unsigned coalloc_size)
    '''
def DBG_mm_get_sites():
    '''
    attr_value_t DBG_mm_get_sites(void)
    '''
def DBG_page_bank_get_page_data(g, len):
    '''
    bytes_t DBG_page_bank_get_page_data(page_bank_grant_t g, int len)
    '''
def DBG_page_bank_get_user_data(page, len):
    '''
    bytes_t DBG_page_bank_get_user_data(pb_page_t *NOTNULL page, int len)
    '''
def DBG_page_bank_set_page_data(g, data):
    '''
    void DBG_page_bank_set_page_data(page_bank_grant_t g, bytes_t data)
    '''
def DBG_page_bank_set_user_data(page, data):
    '''
    void DBG_page_bank_set_user_data(pb_page_t *NOTNULL page, bytes_t data)
    '''
def DBG_print_stack():
    '''
    const char *DBG_print_stack(void)
    '''
def DBG_send_thread_domain_message(obj, h, func, data):
    '''
    void DBG_send_thread_domain_message(conf_object_t *NOTNULL obj, thread_domain_holder_t *NOTNULL h, void (*func)(lang_void *data), lang_void *data)
    '''
def DBG_set_dm_tag(t, tag_index, val):
    '''
    void DBG_set_dm_tag(direct_memory_tags_t t, unsigned tag_index, bool val)
    '''
def DBG_test_function_pointer(func):
    '''
    void (*DBG_test_function_pointer(void (*func)(void)))(void)
    '''
def DBG_thread_domain_locked(d):
    '''
    bool DBG_thread_domain_locked(thread_domain_t *NOTNULL d)
    '''
def DBG_uncatch_signal(sig):
    '''
    int DBG_uncatch_signal(int sig)
    '''
def OLD_copyright():
    '''
    void OLD_copyright(void)
    '''
def SIM_STC_flush_cache(cpu):
    '''
    void SIM_STC_flush_cache(conf_object_t *NOTNULL cpu)
    '''
def SIM_acquire_cell(obj, function_name, source_location):
    '''
    domain_lock_t *SIM_acquire_cell(conf_object_t *NOTNULL obj, const char *NOTNULL function_name, const char *NOTNULL source_location)
    '''
def SIM_acquire_object(obj, function_name, source_location):
    '''
    domain_lock_t *SIM_acquire_object(conf_object_t *NOTNULL obj, const char *NOTNULL function_name, const char *NOTNULL source_location)
    '''
def SIM_acquire_object_for_execution(obj):
    '''
    domain_lock_t *SIM_acquire_object_for_execution(conf_object_t *NOTNULL obj)
    '''
def SIM_acquire_target(obj, function_name, source_location):
    '''
    domain_lock_t *SIM_acquire_target(conf_object_t *NOTNULL obj, const char *NOTNULL function_name, const char *NOTNULL source_location)
    '''
def SIM_add_configuration(object_list, file):
    '''
    void SIM_add_configuration(pre_conf_object_set_t *NOTNULL object_list, const char *file)
    '''
def SIM_add_directory(directory, prepend):
    '''
    void SIM_add_directory(const char *NOTNULL directory, bool prepend)
    '''
def SIM_add_global_notifier(what, subscriber, callback, data):
    '''
    global_notifier_callback_t *SIM_add_global_notifier(global_notifier_type_t what, conf_object_t *subscriber, void (*NOTNULL callback)(conf_object_t *subscriber, lang_void *data), lang_void *data)
    '''
def SIM_add_global_notifier_once(what, subscriber, callback, data):
    '''
    global_notifier_callback_t *SIM_add_global_notifier_once(global_notifier_type_t what, conf_object_t *subscriber, void (*NOTNULL callback)(conf_object_t *subscriber, lang_void *data), lang_void *data)
    '''
def SIM_add_module_dir(path):
    '''
    void SIM_add_module_dir(const char *NOTNULL path)
    '''
def SIM_add_notifier(obj, what, subscriber, callback, data):
    '''
    notifier_handle_t *SIM_add_notifier(conf_object_t *NOTNULL obj, notifier_type_t what, conf_object_t *subscriber, void (*callback)(conf_object_t *, conf_object_t *NOTNULL, lang_void *), lang_void *data)
    '''
def SIM_add_output_handler(func, user_data):
    '''
    void SIM_add_output_handler(NOTNULL output_handler_t func, lang_void *user_data)
    '''
def SIM_add_profiling_area(name, start, end):
    '''
    profile_area_t *SIM_add_profiling_area(const char *NOTNULL name, uintptr_t start, uintptr_t end)
    '''
def SIM_arm_mem_trans_from_generic(mop):
    '''
    arm_memory_transaction_t *SIM_arm_mem_trans_from_generic(generic_transaction_t *NOTNULL mop)
    '''
def SIM_attribute_error(msg):
    '''
    void SIM_attribute_error(const char *NOTNULL msg)
    '''
def SIM_break_cycle(obj, cycles):
    '''
    void SIM_break_cycle(conf_object_t *NOTNULL obj, int64 cycles)
    '''
def SIM_break_message(msg):
    '''
    void SIM_break_message(const char *msg)
    '''
def SIM_break_simulation(msg):
    '''
    void SIM_break_simulation(const char *msg)
    '''
def SIM_break_step(obj, steps):
    '''
    void SIM_break_step(conf_object_t *NOTNULL obj, int64 steps)
    '''
def SIM_breakpoint(obj, type, access, address, length, flags):
    '''
    breakpoint_id_t SIM_breakpoint(conf_object_t *NOTNULL obj, breakpoint_kind_t type, access_t access, uint64 address, uint64 length, breakpoint_flag_t flags)
    '''
def SIM_breakpoint_remove(id, access, address, length):
    '''
    void SIM_breakpoint_remove(breakpoint_id_t id, access_t access, generic_address_t address, generic_address_t length)
    '''
def SIM_c_get_class_interface(cls, name):
    '''
    const class_interface_t *SIM_c_get_class_interface(const conf_class_t *NOTNULL cls, const char *NOTNULL name)
    '''
def SIM_c_get_class_port_interface(cls, name, portname):
    '''
    const class_interface_t *SIM_c_get_class_port_interface(const conf_class_t *NOTNULL cls, const char *NOTNULL name, const char *portname)
    '''
def SIM_c_get_interface(obj, name):
    '''
    const interface_t *SIM_c_get_interface(const conf_object_t *NOTNULL obj, const char *NOTNULL name)
    '''
def SIM_c_get_port_interface(obj, name, portname):
    '''
    const interface_t *SIM_c_get_port_interface(const conf_object_t *NOTNULL obj, const char *NOTNULL name, const char *portname)
    '''
def SIM_call_python_function(func, args):
    '''
    attr_value_t SIM_call_python_function(const char *NOTNULL func, attr_value_t *NOTNULL args)
    '''
def SIM_cancel_realtime_event(id):
    '''
    int SIM_cancel_realtime_event(int64 id)
    '''
def SIM_class_has_attribute(cls, attr):
    '''
    bool SIM_class_has_attribute(conf_class_t *NOTNULL cls, const char *NOTNULL attr)
    '''
def SIM_class_has_notifier(cls, what):
    '''
    bool SIM_class_has_notifier(conf_class_t *NOTNULL cls, notifier_type_t what)
    '''
def SIM_class_port(cls, portname):
    '''
    conf_class_t *SIM_class_port(const conf_class_t *NOTNULL cls, const char *NOTNULL portname)
    '''
def SIM_clear_directories():
    '''
    void SIM_clear_directories(void)
    '''
def SIM_complete_transaction(t, ex):
    '''
    void SIM_complete_transaction(transaction_t *NOTNULL t, exception_type_t ex)
    '''
def SIM_continue(steps):
    '''
    pc_step_t SIM_continue(int64 steps)
    '''
def SIM_copy_class(name, src_cls, desc):
    '''
    conf_class_t *SIM_copy_class(const char *NOTNULL name, const conf_class_t *NOTNULL src_cls, const char *desc)
    '''
def SIM_copyright():
    '''
    char *SIM_copyright(void)
    '''
def SIM_create_class(name, class_info):
    '''
    conf_class_t *SIM_create_class(const char *NOTNULL name, const class_info_t *NOTNULL class_info)
    '''
def SIM_create_object(cls, name, attrs):
    '''
    conf_object_t *SIM_create_object(conf_class_t *NOTNULL cls, const char *name, attr_value_t attrs)
    '''
def SIM_current_checkpoint_dir():
    '''
    char *SIM_current_checkpoint_dir(void)
    '''
def SIM_current_clock():
    '''
    conf_object_t *SIM_current_clock(void)
    '''
def SIM_cycle_count(obj):
    '''
    cycles_t SIM_cycle_count(conf_object_t *NOTNULL obj)
    '''
def SIM_defer_owned_transaction(t):
    '''
    transaction_t *SIM_defer_owned_transaction(transaction_t *NOTNULL t)
    '''
def SIM_defer_transaction(obj, t):
    '''
    transaction_t *SIM_defer_transaction(conf_object_t *obj, transaction_t *t)
    '''
def SIM_delete_breakpoint(id):
    '''
    void SIM_delete_breakpoint(breakpoint_id_t id)
    '''
def SIM_delete_global_notifier(handle):
    '''
    void SIM_delete_global_notifier(global_notifier_callback_t *NOTNULL handle)
    '''
def SIM_delete_notifier(obj, handle):
    '''
    void SIM_delete_notifier(conf_object_t *NOTNULL obj, notifier_handle_t *handle)
    '''
def SIM_delete_object(obj):
    '''
    int SIM_delete_object(conf_object_t *NOTNULL obj)
    '''
def SIM_delete_objects(object_list):
    '''
    int SIM_delete_objects(attr_value_t object_list)
    '''
def SIM_delete_snapshot(name):
    '''
    snapshot_error_t SIM_delete_snapshot(const char *NOTNULL name)
    '''
def SIM_describe_notifier(type, generic_desc):
    '''
    void SIM_describe_notifier(notifier_type_t type, const char *NOTNULL generic_desc)
    '''
def SIM_describe_pseudo_exception(ex):
    '''
    const char *SIM_describe_pseudo_exception(exception_type_t ex)
    '''
def SIM_disable_breakpoint(id):
    '''
    void SIM_disable_breakpoint(breakpoint_id_t id)
    '''
def SIM_disassemble_address(cpu, address, logical, sub):
    '''
    tuple_int_string_t SIM_disassemble_address(conf_object_t *NOTNULL cpu, generic_address_t address, int logical, int sub)
    '''
def SIM_drop_thread_domains():
    '''
    domain_lock_t *SIM_drop_thread_domains(void)
    '''
def SIM_enable_breakpoint(id):
    '''
    void SIM_enable_breakpoint(breakpoint_id_t id)
    '''
def SIM_ensure_partial_attr_order(cls, attr1, attr2):
    '''
    void SIM_ensure_partial_attr_order(conf_class_t *NOTNULL cls, const char *NOTNULL attr1, const char *NOTNULL attr2)
    '''
def SIM_event_cancel_step(clock, evclass, obj, pred, match_data):
    '''
    void SIM_event_cancel_step(conf_object_t *NOTNULL clock, event_class_t *NOTNULL evclass, conf_object_t *NOTNULL obj, int (*pred)(lang_void *data, lang_void *match_data), lang_void *match_data)
    '''
def SIM_event_cancel_time(clock, evclass, obj, pred, match_data):
    '''
    void SIM_event_cancel_time(conf_object_t *NOTNULL clock, event_class_t *NOTNULL evclass, conf_object_t *NOTNULL obj, int (*pred)(lang_void *data, lang_void *match_data), lang_void *match_data)
    '''
def SIM_event_class_flags(ec):
    '''
    event_class_flag_t SIM_event_class_flags(event_class_t *NOTNULL ec)
    '''
def SIM_event_find_next_cycle(clock, evclass, obj, pred, match_data):
    '''
    cycles_t SIM_event_find_next_cycle(conf_object_t *NOTNULL clock, event_class_t *NOTNULL evclass, conf_object_t *NOTNULL obj, int (*pred)(lang_void *data, lang_void *match_data), lang_void *match_data)
    '''
def SIM_event_find_next_step(clock, evclass, obj, pred, match_data):
    '''
    pc_step_t SIM_event_find_next_step(conf_object_t *NOTNULL clock, event_class_t *NOTNULL evclass, conf_object_t *NOTNULL obj, int (*pred)(lang_void *data, lang_void *match_data), lang_void *match_data)
    '''
def SIM_event_find_next_time(clock, evclass, obj, pred, match_data):
    '''
    double SIM_event_find_next_time(conf_object_t *NOTNULL clock, event_class_t *NOTNULL evclass, conf_object_t *NOTNULL obj, int (*pred)(lang_void *data, lang_void *match_data), lang_void *match_data)
    '''
def SIM_event_post_cycle(clock, evclass, obj, cycles, user_data):
    '''
    void SIM_event_post_cycle(conf_object_t *NOTNULL clock, event_class_t *NOTNULL evclass, conf_object_t *NOTNULL obj, cycles_t cycles, lang_void *user_data)
    '''
def SIM_event_post_step(clock, evclass, obj, steps, user_data):
    '''
    void SIM_event_post_step(conf_object_t *NOTNULL clock, event_class_t *NOTNULL evclass, conf_object_t *NOTNULL obj, pc_step_t steps, lang_void *user_data)
    '''
def SIM_event_post_time(clock, evclass, obj, seconds, user_data):
    '''
    void SIM_event_post_time(conf_object_t *NOTNULL clock, event_class_t *NOTNULL evclass, conf_object_t *NOTNULL obj, double seconds, lang_void *user_data)
    '''
def SIM_extend_class(cls, ext):
    '''
    void SIM_extend_class(conf_class_t *NOTNULL cls, conf_class_t *NOTNULL ext)
    '''
def SIM_flush_D_STC_logical(cpu, vaddr, length):
    '''
    void SIM_flush_D_STC_logical(conf_object_t *NOTNULL cpu, logical_address_t vaddr, logical_address_t length)
    '''
def SIM_flush_D_STC_physical(cpu, paddr, length, read_or_write):
    '''
    void SIM_flush_D_STC_physical(conf_object_t *NOTNULL cpu, physical_address_t paddr, physical_address_t length, read_or_write_t read_or_write)
    '''
def SIM_flush_I_STC_logical(cpu, vaddr, length):
    '''
    void SIM_flush_I_STC_logical(conf_object_t *NOTNULL cpu, logical_address_t vaddr, logical_address_t length)
    '''
def SIM_flush_I_STC_physical(cpu, paddr, length):
    '''
    void SIM_flush_I_STC_physical(conf_object_t *NOTNULL cpu, physical_address_t paddr, physical_address_t length)
    '''
def SIM_flush_all_caches():
    '''
    void SIM_flush_all_caches(void)
    '''
def SIM_flush_cell_caches(obj):
    '''
    void SIM_flush_cell_caches(conf_object_t *NOTNULL obj)
    '''
def SIM_free_map_target(mt):
    '''
    void SIM_free_map_target(map_target_t *mt)
    '''
def SIM_get_all_classes():
    '''
    attr_value_t SIM_get_all_classes(void)
    '''
def SIM_get_all_failed_modules():
    '''
    attr_value_t SIM_get_all_failed_modules(void)
    '''
def SIM_get_all_hap_types():
    '''
    attr_value_t SIM_get_all_hap_types(void)
    '''
def SIM_get_all_modules():
    '''
    attr_value_t SIM_get_all_modules(void)
    '''
def SIM_get_all_objects():
    '''
    attr_value_t SIM_get_all_objects(void)
    '''
def SIM_get_all_processors():
    '''
    attr_value_t SIM_get_all_processors(void)
    '''
def SIM_get_attribute(obj, name):
    '''
    attr_value_t SIM_get_attribute(conf_object_t *NOTNULL obj, const char *NOTNULL name)
    '''
def SIM_get_attribute_attributes(cls, attr):
    '''
    attr_attr_t SIM_get_attribute_attributes(conf_class_t *NOTNULL cls, const char *NOTNULL attr)
    '''
def SIM_get_attribute_idx(obj, name, index):
    '''
    attr_value_t SIM_get_attribute_idx(conf_object_t *NOTNULL obj, const char *NOTNULL name, attr_value_t *NOTNULL index)
    '''
def SIM_get_batch_mode():
    '''
    bool SIM_get_batch_mode(void)
    '''
def SIM_get_class(name):
    '''
    conf_class_t *SIM_get_class(const char *NOTNULL name)
    '''
def SIM_get_class_attribute(cls, name):
    '''
    attr_value_t SIM_get_class_attribute(conf_class_t *NOTNULL cls, const char *NOTNULL name)
    '''
def SIM_get_class_attribute_idx(cls, name, index):
    '''
    attr_value_t SIM_get_class_attribute_idx(conf_class_t *NOTNULL cls, const char *NOTNULL name, attr_value_t *NOTNULL index)
    '''
def SIM_get_class_interface(cls, name):
    '''
    const class_interface_t *SIM_get_class_interface(const conf_class_t *NOTNULL cls, const char *NOTNULL name)
    '''
def SIM_get_class_name(class_data):
    '''
    const char *SIM_get_class_name(const conf_class_t *NOTNULL class_data)
    '''
def SIM_get_class_port_interface(cls, name, portname):
    '''
    const class_interface_t *SIM_get_class_port_interface(const conf_class_t *NOTNULL cls, const char *NOTNULL name, const char *portname)
    '''
def SIM_get_debugger():
    '''
    conf_object_t *SIM_get_debugger(void)
    '''
def SIM_get_directories():
    '''
    attr_value_t SIM_get_directories(void)
    '''
def SIM_get_event_class(cl, name):
    '''
    event_class_t *SIM_get_event_class(conf_class_t *NOTNULL cl, const char *NOTNULL name)
    '''
def SIM_get_global_message(ref):
    '''
    const char *SIM_get_global_message(lang_void *NOTNULL ref)
    '''
def SIM_get_init_arg_boolean(name, default_value):
    '''
    bool SIM_get_init_arg_boolean(const char *NOTNULL name, bool default_value)
    '''
def SIM_get_init_arg_string(name, default_value):
    '''
    const char *SIM_get_init_arg_string(const char *NOTNULL name, const char *default_value)
    '''
def SIM_get_interface(obj, name):
    '''
    const interface_t *SIM_get_interface(const conf_object_t *NOTNULL obj, const char *NOTNULL name)
    '''
def SIM_get_mem_op_exception(mop):
    '''
    static inline exception_type_t SIM_get_mem_op_exception(const generic_transaction_t *NOTNULL mop)
    '''
def SIM_get_mem_op_ini_type(mop):
    '''
    static inline ini_type_t SIM_get_mem_op_ini_type(const generic_transaction_t *NOTNULL mop)
    '''
def SIM_get_mem_op_initiator(mop):
    '''
    static inline conf_object_t *SIM_get_mem_op_initiator(const generic_transaction_t *NOTNULL mop)
    '''
def SIM_get_mem_op_inquiry(mop):
    '''
    static inline bool SIM_get_mem_op_inquiry(const generic_transaction_t *NOTNULL mop)
    '''
def SIM_get_mem_op_page_cross(mop):
    '''
    static inline unsigned SIM_get_mem_op_page_cross(const generic_transaction_t *NOTNULL mop)
    '''
def SIM_get_mem_op_physical_address(mop):
    '''
    static inline physical_address_t SIM_get_mem_op_physical_address(const generic_transaction_t *NOTNULL mop)
    '''
def SIM_get_mem_op_size(mop):
    '''
    static inline unsigned SIM_get_mem_op_size(const generic_transaction_t *NOTNULL mop)
    '''
def SIM_get_mem_op_type(mop):
    '''
    static inline mem_op_type_t SIM_get_mem_op_type(const generic_transaction_t *NOTNULL mop)
    '''
def SIM_get_mem_op_type_name(type):
    '''
    const char *SIM_get_mem_op_type_name(mem_op_type_t type)
    '''
def SIM_get_mem_op_value_be(mop):
    '''
    uint64 SIM_get_mem_op_value_be(const generic_transaction_t *NOTNULL mop)
    '''
def SIM_get_mem_op_value_buf(mop):
    '''
    attr_value_t SIM_get_mem_op_value_buf(const generic_transaction_t *NOTNULL mop)
    '''
def SIM_get_mem_op_value_cpu(mop):
    '''
    uint64 SIM_get_mem_op_value_cpu(const generic_transaction_t *NOTNULL mop)
    '''
def SIM_get_mem_op_value_le(mop):
    '''
    uint64 SIM_get_mem_op_value_le(const generic_transaction_t *NOTNULL mop)
    '''
def SIM_get_mem_op_virtual_address(mop):
    '''
    static inline logical_address_t SIM_get_mem_op_virtual_address(const generic_transaction_t *NOTNULL mop)
    '''
def SIM_get_object(name):
    '''
    conf_object_t *SIM_get_object(const char *NOTNULL name)
    '''
def SIM_get_port_interface(obj, name, portname):
    '''
    const interface_t *SIM_get_port_interface(const conf_object_t *NOTNULL obj, const char *NOTNULL name, const char *portname)
    '''
def SIM_get_processor(proc_no):
    '''
    conf_object_t *SIM_get_processor(int proc_no)
    '''
def SIM_get_processor_number(cpu):
    '''
    int SIM_get_processor_number(const conf_object_t *NOTNULL cpu)
    '''
def SIM_get_python_interface_type(name):
    '''
    SIM_PYOBJECT *SIM_get_python_interface_type(const char *NOTNULL name)
    '''
def SIM_get_quiet():
    '''
    bool SIM_get_quiet(void)
    '''
def SIM_get_snapshot_info(name):
    '''
    attr_value_t SIM_get_snapshot_info(const char *NOTNULL name)
    '''
def SIM_get_transaction_bytes(t, buf):
    '''
    void SIM_get_transaction_bytes(const transaction_t *NOTNULL t, buffer_t buf)
    '''
def SIM_get_transaction_bytes_offs(t, offs, buf, zerofill_holes):
    '''
    void SIM_get_transaction_bytes_offs(const transaction_t *NOTNULL t, unsigned offs, buffer_t buf, bool zerofill_holes)
    '''
def SIM_get_transaction_id(t):
    '''
    int64 SIM_get_transaction_id(transaction_t *t)
    '''
def SIM_get_transaction_value_be(t):
    '''
    uint64 SIM_get_transaction_value_be(const transaction_t *NOTNULL t)
    '''
def SIM_get_transaction_value_le(t):
    '''
    uint64 SIM_get_transaction_value_le(const transaction_t *NOTNULL t)
    '''
def SIM_get_verbose():
    '''
    bool SIM_get_verbose(void)
    '''
def SIM_hap_add_callback(hap, func, data):
    '''
    hap_handle_t SIM_hap_add_callback(const char *NOTNULL hap, NOTNULL obj_hap_func_t func, lang_void *data)
    '''
def SIM_hap_add_callback_index(hap, func, data, index):
    '''
    hap_handle_t SIM_hap_add_callback_index(const char *NOTNULL hap, NOTNULL obj_hap_func_t func, lang_void *data, int64 index)
    '''
def SIM_hap_add_callback_obj(hap, obj, flags, func, data):
    '''
    hap_handle_t SIM_hap_add_callback_obj(const char *NOTNULL hap, conf_object_t *NOTNULL obj, int flags, NOTNULL obj_hap_func_t func, lang_void *data)
    '''
def SIM_hap_add_callback_obj_index(hap, obj, flags, func, data, index):
    '''
    hap_handle_t SIM_hap_add_callback_obj_index(const char *NOTNULL hap, conf_object_t *NOTNULL obj, int flags, NOTNULL obj_hap_func_t func, lang_void *data, int64 index)
    '''
def SIM_hap_add_callback_obj_range(hap, obj, flags, func, data, start, end):
    '''
    hap_handle_t SIM_hap_add_callback_obj_range(const char *NOTNULL hap, conf_object_t *NOTNULL obj, int flags, NOTNULL obj_hap_func_t func, lang_void *data, int64 start, int64 end)
    '''
def SIM_hap_add_callback_range(hap, func, data, start, end):
    '''
    hap_handle_t SIM_hap_add_callback_range(const char *NOTNULL hap, NOTNULL obj_hap_func_t func, lang_void *data, int64 start, int64 end)
    '''
def SIM_hap_add_type(hap, params, param_desc, index, desc, unused):
    '''
    hap_type_t SIM_hap_add_type(const char *NOTNULL hap, const char *NOTNULL params, const char *param_desc, const char *index, const char *desc, int unused)
    '''
def SIM_hap_delete_callback(hap, func, data):
    '''
    void SIM_hap_delete_callback(const char *NOTNULL hap, NOTNULL obj_hap_func_t func, lang_void *data)
    '''
def SIM_hap_delete_callback_id(hap, handle):
    '''
    void SIM_hap_delete_callback_id(const char *NOTNULL hap, hap_handle_t handle)
    '''
def SIM_hap_delete_callback_obj(hap, obj, func, data):
    '''
    void SIM_hap_delete_callback_obj(const char *NOTNULL hap, conf_object_t *NOTNULL obj, NOTNULL obj_hap_func_t func, lang_void *data)
    '''
def SIM_hap_delete_callback_obj_id(hap, obj, handle):
    '''
    void SIM_hap_delete_callback_obj_id(const char *NOTNULL hap, conf_object_t *NOTNULL obj, hap_handle_t handle)
    '''
def SIM_hap_get_name(hap):
    '''
    const char *SIM_hap_get_name(hap_type_t hap)
    '''
def SIM_hap_get_number(hap):
    '''
    hap_type_t SIM_hap_get_number(const char *NOTNULL hap)
    '''
def SIM_hap_is_active(hap):
    '''
    bool SIM_hap_is_active(hap_type_t hap)
    '''
def SIM_hap_is_active_obj(hap, obj):
    '''
    bool SIM_hap_is_active_obj(hap_type_t hap, conf_object_t *NOTNULL obj)
    '''
def SIM_hap_is_active_obj_idx(hap, obj, index):
    '''
    bool SIM_hap_is_active_obj_idx(hap_type_t hap, conf_object_t *NOTNULL obj, int64 index)
    '''
def SIM_hap_occurred(hap, obj, value, list):
    '''
    int SIM_hap_occurred(hap_type_t hap, conf_object_t *obj, int64 value, attr_value_t *NOTNULL list)
    '''
def SIM_hap_occurred_always(hap, obj, value, list):
    '''
    int SIM_hap_occurred_always(hap_type_t hap, conf_object_t *obj, int64 value, attr_value_t *NOTNULL list)
    '''
def SIM_hap_remove_type(hap):
    '''
    void SIM_hap_remove_type(const char *NOTNULL hap)
    '''
def SIM_has_notifier(obj, what):
    '''
    bool SIM_has_notifier(conf_object_t *NOTNULL obj, notifier_type_t what)
    '''
def SIM_init_command_line():
    '''
    void SIM_init_command_line(void)
    '''
def SIM_inspect_address_routing(mt, t, addr, callback, data):
    '''
    bool SIM_inspect_address_routing(const map_target_t *NOTNULL mt, transaction_t *NOTNULL t, uint64 addr, bool (*NOTNULL callback)(const map_target_t *, const transaction_t *, uint64, uint64, uint64, uint64, access_t, translation_flags_t, lang_void *), lang_void *data)
    '''
def SIM_inspect_breakpoints(mt, t, start, end, callback, data):
    '''
    bool SIM_inspect_breakpoints(const map_target_t *NOTNULL mt, transaction_t *NOTNULL t, uint64 start, uint64 end, bool (*NOTNULL callback)(conf_object_t *, breakpoint_set_t, const transaction_t *, uint64, uint64, lang_void *), lang_void *data)
    '''
def SIM_is_restoring_snapshot():
    '''
    bool SIM_is_restoring_snapshot(void)
    '''
def SIM_is_restoring_state(obj):
    '''
    bool SIM_is_restoring_state(conf_object_t *obj)
    '''
def SIM_issue_transaction(mt, t, addr):
    '''
    exception_type_t SIM_issue_transaction(const map_target_t *NOTNULL mt, transaction_t *NOTNULL t, uint64 addr)
    '''
def SIM_last_error():
    '''
    const char *SIM_last_error(void)
    '''
def SIM_license():
    '''
    void SIM_license(void)
    '''
def SIM_license_file(format):
    '''
    char *SIM_license_file(const char *format)
    '''
def SIM_list_snapshots():
    '''
    attr_value_t SIM_list_snapshots(void)
    '''
def SIM_load_binary(obj, file, offset, use_pa, verbose):
    '''
    physical_address_t SIM_load_binary(conf_object_t *NOTNULL obj, const char *NOTNULL file, physical_address_t offset, bool use_pa, bool verbose)
    '''
def SIM_load_file(obj, file, paddr, verbose):
    '''
    void SIM_load_file(conf_object_t *NOTNULL obj, const char *NOTNULL file, physical_address_t paddr, bool verbose)
    '''
def SIM_load_module(module):
    '''
    void SIM_load_module(const char *NOTNULL module)
    '''
def SIM_load_target(target, ns, presets, cmdline_args):
    '''
    attr_value_t SIM_load_target(const char *NOTNULL target, const char *ns, attr_value_t presets, attr_value_t cmdline_args)
    '''
def SIM_log_level(obj):
    '''
    unsigned SIM_log_level(const conf_object_t *NOTNULL obj)
    '''
def SIM_log_message(obj, level, group_ids, log_type, message):
    '''
    void SIM_log_message(conf_object_t *NOTNULL obj, int level, uint64 group_ids, log_type_t log_type, const char *NOTNULL message)
    '''
def SIM_log_register_groups(cls, names):
    '''
    void SIM_log_register_groups(conf_class_t *NOTNULL cls, const char *const *NOTNULL names)
    '''
def SIM_lookup_file(file):
    '''
    char *SIM_lookup_file(const char *file)
    '''
def SIM_main_loop():
    '''
    void SIM_main_loop(void)
    '''
def SIM_map_target_flush(mt, base, size, access):
    '''
    bool SIM_map_target_flush(const map_target_t *NOTNULL mt, uint64 base, uint64 size, access_t access)
    '''
def SIM_map_target_object(mt):
    '''
    conf_object_t *SIM_map_target_object(const map_target_t *NOTNULL mt)
    '''
def SIM_map_target_port(mt):
    '''
    const char *SIM_map_target_port(const map_target_t *NOTNULL mt)
    '''
def SIM_map_target_target(mt):
    '''
    const map_target_t *SIM_map_target_target(const map_target_t *NOTNULL mt)
    '''
def SIM_marked_for_deletion(obj):
    '''
    bool SIM_marked_for_deletion(const conf_object_t *NOTNULL obj)
    '''
def SIM_mem_op_ensure_future_visibility(mop):
    '''
    static inline void SIM_mem_op_ensure_future_visibility(generic_transaction_t *NOTNULL mop)
    '''
def SIM_mem_op_is_atomic(mop):
    '''
    static inline bool SIM_mem_op_is_atomic(const generic_transaction_t *NOTNULL mop)
    '''
def SIM_mem_op_is_control(mop):
    '''
    static inline bool SIM_mem_op_is_control(const generic_transaction_t *NOTNULL mop)
    '''
def SIM_mem_op_is_data(mop):
    '''
    static inline bool SIM_mem_op_is_data(const generic_transaction_t *NOTNULL mop)
    '''
def SIM_mem_op_is_from_cache(mop):
    '''
    static inline bool SIM_mem_op_is_from_cache(const generic_transaction_t *NOTNULL mop)
    '''
def SIM_mem_op_is_from_cpu(mop):
    '''
    static inline bool SIM_mem_op_is_from_cpu(const generic_transaction_t *NOTNULL mop)
    '''
def SIM_mem_op_is_from_cpu_arch(mop, arch):
    '''
    static inline bool SIM_mem_op_is_from_cpu_arch(const generic_transaction_t *NOTNULL mop, ini_type_t arch)
    '''
def SIM_mem_op_is_from_device(mop):
    '''
    static inline bool SIM_mem_op_is_from_device(const generic_transaction_t *NOTNULL mop)
    '''
def SIM_mem_op_is_instruction(mop):
    '''
    static inline bool SIM_mem_op_is_instruction(const generic_transaction_t *NOTNULL mop)
    '''
def SIM_mem_op_is_prefetch(mop):
    '''
    static inline bool SIM_mem_op_is_prefetch(const generic_transaction_t *NOTNULL mop)
    '''
def SIM_mem_op_is_read(mop):
    '''
    static inline bool SIM_mem_op_is_read(const generic_transaction_t *NOTNULL mop)
    '''
def SIM_mem_op_is_write(mop):
    '''
    static inline bool SIM_mem_op_is_write(const generic_transaction_t *NOTNULL mop)
    '''
def SIM_mem_op_may_stall(mop):
    '''
    static inline bool SIM_mem_op_may_stall(const generic_transaction_t *NOTNULL mop)
    '''
def SIM_mips_mem_trans_from_generic(mop):
    '''
    mips_memory_transaction_t *SIM_mips_mem_trans_from_generic(generic_transaction_t *NOTNULL mop)
    '''
def SIM_module_list_refresh():
    '''
    void SIM_module_list_refresh(void)
    '''
def SIM_monitor_chained_transaction(t, ex):
    '''
    exception_type_t SIM_monitor_chained_transaction(transaction_t *NOTNULL t, exception_type_t ex)
    '''
def SIM_monitor_transaction(t, ex):
    '''
    exception_type_t SIM_monitor_transaction(transaction_t *NOTNULL t, exception_type_t ex)
    '''
def SIM_native_path(path):
    '''
    char *SIM_native_path(const char *NOTNULL path)
    '''
def SIM_new_map_target(obj, port, chained_target):
    '''
    map_target_t *SIM_new_map_target(conf_object_t *NOTNULL obj, const char *port, const map_target_t *chained_target)
    '''
def SIM_notifier_description(type):
    '''
    const char *SIM_notifier_description(notifier_type_t type)
    '''
def SIM_notifier_type(type):
    '''
    notifier_type_t SIM_notifier_type(const char *NOTNULL type)
    '''
def SIM_notify(obj, type):
    '''
    void SIM_notify(conf_object_t *NOTNULL obj, notifier_type_t type)
    '''
def SIM_notify_on_descriptor(fd, mode, run_in_thread, callback, data):
    '''
    void SIM_notify_on_descriptor(int fd, notify_mode_t mode, int run_in_thread, void (*callback)(lang_void *data), lang_void *data)
    '''
def SIM_notify_on_socket(sock, mode, run_in_thread, callback, data):
    '''
    void SIM_notify_on_socket(socket_t sock, notify_mode_t mode, int run_in_thread, void (*callback)(lang_void *data), lang_void *data)
    '''
def SIM_number_processors():
    '''
    int SIM_number_processors(void)
    '''
def SIM_object_class(obj):
    '''
    static inline conf_class_t *SIM_object_class(const conf_object_t *NOTNULL obj)
    '''
def SIM_object_clock(obj):
    '''
    conf_object_t *SIM_object_clock(const conf_object_t *NOTNULL obj)
    '''
def SIM_object_descendant(obj, relname):
    '''
    conf_object_t *SIM_object_descendant(conf_object_t *obj, const char *NOTNULL relname)
    '''
def SIM_object_id(obj):
    '''
    const char *SIM_object_id(const conf_object_t *NOTNULL obj)
    '''
def SIM_object_is_configured(obj):
    '''
    bool SIM_object_is_configured(const conf_object_t *NOTNULL obj)
    '''
def SIM_object_is_processor(obj):
    '''
    bool SIM_object_is_processor(conf_object_t *NOTNULL obj)
    '''
def SIM_object_name(obj):
    '''
    const char *SIM_object_name(const conf_object_t *NOTNULL obj)
    '''
def SIM_object_parent(obj):
    '''
    conf_object_t *SIM_object_parent(conf_object_t *NOTNULL obj)
    '''
def SIM_pci_mem_trans_from_generic(mop):
    '''
    pci_memory_transaction_t *SIM_pci_mem_trans_from_generic(generic_transaction_t *NOTNULL mop)
    '''
def SIM_picosecond_clock(obj):
    '''
    conf_object_t *SIM_picosecond_clock(conf_object_t *NOTNULL obj)
    '''
def SIM_poll_transaction(t):
    '''
    exception_type_t SIM_poll_transaction(transaction_t *NOTNULL t)
    '''
def SIM_port_object_parent(obj):
    '''
    conf_object_t *SIM_port_object_parent(conf_object_t *NOTNULL obj)
    '''
def SIM_ppc_mem_trans_from_generic(mop):
    '''
    ppc_memory_transaction_t *SIM_ppc_mem_trans_from_generic(generic_transaction_t *NOTNULL mop)
    '''
def SIM_process_pending_work():
    '''
    int SIM_process_pending_work(void)
    '''
def SIM_process_work(done, done_data):
    '''
    int SIM_process_work(int (*NOTNULL done)(lang_void *), lang_void *done_data)
    '''
def SIM_processor_privilege_level(cpu):
    '''
    int SIM_processor_privilege_level(conf_object_t *NOTNULL cpu)
    '''
def SIM_quit(exit_code):
    '''
    void SIM_quit(int exit_code)
    '''
def SIM_reacquire_thread_domains(lock):
    '''
    void SIM_reacquire_thread_domains(domain_lock_t *NOTNULL lock)
    '''
def SIM_read_byte(obj, paddr):
    '''
    uint8 SIM_read_byte(conf_object_t *NOTNULL obj, generic_address_t paddr)
    '''
def SIM_read_configuration(file):
    '''
    void SIM_read_configuration(const char *NOTNULL file)
    '''
def SIM_read_phys_memory(cpu, paddr, length):
    '''
    uint64 SIM_read_phys_memory(conf_object_t *NOTNULL cpu, physical_address_t paddr, int length)
    '''
def SIM_read_phys_memory_tags(mem_space, paddr, ntags):
    '''
    uint64 SIM_read_phys_memory_tags(conf_object_t *NOTNULL mem_space, physical_address_t paddr, unsigned ntags)
    '''
def SIM_realtime_event(delay_ms, callback, data, run_in_thread, desc):
    '''
    int64 SIM_realtime_event(unsigned delay_ms, void (*NOTNULL callback)(lang_void *data), lang_void *data, int run_in_thread, const char *desc)
    '''
def SIM_reconnect_transaction(t, id):
    '''
    void SIM_reconnect_transaction(transaction_t *t, int64 id)
    '''
def SIM_register_attribute(cls, name, get_attr, set_attr, attr, type, desc):
    '''
    void SIM_register_attribute(conf_class_t *NOTNULL cls, const char *NOTNULL name, attr_value_t (*get_attr)(conf_object_t *), set_error_t (*set_attr)(conf_object_t *, attr_value_t *), attr_attr_t attr, const char *NOTNULL type, const char *desc)
    '''
def SIM_register_attribute_with_user_data(cls, name, get_attr, user_data_get, set_attr, user_data_set, attr, type, desc):
    '''
    void SIM_register_attribute_with_user_data(conf_class_t *NOTNULL cls, const char *NOTNULL name, attr_value_t (*get_attr)(conf_object_t *, lang_void *), lang_void *user_data_get, set_error_t (*set_attr)(conf_object_t *, attr_value_t *, lang_void *), lang_void *user_data_set, attr_attr_t attr, const char *NOTNULL type, const char *desc)
    '''
def SIM_register_class(name, class_data):
    '''
    conf_class_t *SIM_register_class(const char *NOTNULL name, const class_data_t *NOTNULL class_data)
    '''
def SIM_register_class_alias(alias, name):
    '''
    void SIM_register_class_alias(const char *NOTNULL alias, const char *NOTNULL name)
    '''
def SIM_register_class_attribute(cls, name, get_attr, set_attr, attr, type, desc):
    '''
    void SIM_register_class_attribute(conf_class_t *NOTNULL cls, const char *NOTNULL name, attr_value_t (*get_attr)(conf_class_t *), set_error_t (*set_attr)(conf_class_t *, attr_value_t *), attr_attr_t attr, const char *NOTNULL type, const char *desc)
    '''
def SIM_register_class_attribute_with_user_data(cls, name, get_attr, user_data_get, set_attr, user_data_set, attr, type, desc):
    '''
    void SIM_register_class_attribute_with_user_data(conf_class_t *NOTNULL cls, const char *NOTNULL name, attr_value_t (*get_attr)(conf_class_t *, lang_void *), lang_void *user_data_get, set_error_t (*set_attr)(conf_class_t *, attr_value_t *, lang_void *), lang_void *user_data_set, attr_attr_t attr, const char *NOTNULL type, const char *desc)
    '''
def SIM_register_clock(cls, iface):
    '''
    int SIM_register_clock(conf_class_t *NOTNULL cls, const cycle_interface_t *NOTNULL iface)
    '''
def SIM_register_compatible_interfaces(cls, name):
    '''
    void SIM_register_compatible_interfaces(conf_class_t *NOTNULL cls, const char *NOTNULL name)
    '''
def SIM_register_context_handler(cls, iface):
    '''
    int SIM_register_context_handler(conf_class_t *NOTNULL cls, const context_handler_interface_t *NOTNULL iface)
    '''
def SIM_register_event(name, cl, flags, callback, destroy, get_value, set_value, describe):
    '''
    event_class_t *SIM_register_event(const char *NOTNULL name, conf_class_t *cl, event_class_flag_t flags, void (*NOTNULL callback)(conf_object_t *, lang_void *), void (*destroy)(conf_object_t *obj, lang_void *data), attr_value_t (*get_value)(conf_object_t *obj, lang_void *data), lang_void *(*set_value)(conf_object_t *, attr_value_t), char *(*describe)(conf_object_t *, lang_void *))
    '''
def SIM_register_interface(cls, name, iface):
    '''
    int SIM_register_interface(conf_class_t *NOTNULL cls, const char *NOTNULL name, const interface_t *NOTNULL iface)
    '''
def SIM_register_notifier(cls, what, desc):
    '''
    void SIM_register_notifier(conf_class_t *NOTNULL cls, notifier_type_t what, const char *desc)
    '''
def SIM_register_port(cls, name, port_cls, desc):
    '''
    void SIM_register_port(conf_class_t *NOTNULL cls, const char *NOTNULL name, conf_class_t *NOTNULL port_cls, const char *desc)
    '''
def SIM_register_port_interface(cls, name, iface, portname, desc):
    '''
    int SIM_register_port_interface(conf_class_t *NOTNULL cls, const char *NOTNULL name, const interface_t *NOTNULL iface, const char *NOTNULL portname, const char *desc)
    '''
def SIM_register_python_atom_type(name):
    '''
    void SIM_register_python_atom_type(const char *NOTNULL name)
    '''
def SIM_register_simple_port(cls, name, desc):
    '''
    conf_class_t *SIM_register_simple_port(conf_class_t *NOTNULL cls, const char *NOTNULL name, const char *desc)
    '''
def SIM_register_tracked_notifier(cls, what, desc, subscribed_changed):
    '''
    void SIM_register_tracked_notifier(conf_class_t *NOTNULL cls, notifier_type_t what, const char *desc, void (*subscribed_changed)(conf_object_t *, notifier_type_t, bool))
    '''
def SIM_register_typed_attribute(cls, name, get_attr, user_data_get, set_attr, user_data_set, attr, type, idx_type, desc):
    '''
    int SIM_register_typed_attribute(conf_class_t *NOTNULL cls, const char *NOTNULL name, get_attr_t get_attr, lang_void *user_data_get, set_attr_t set_attr, lang_void *user_data_set, attr_attr_t attr, const char *type, const char *idx_type, const char *desc)
    '''
def SIM_register_typed_class_attribute(cls, name, get_attr, user_data_get, set_attr, user_data_set, attr, type, idx_type, desc):
    '''
    int SIM_register_typed_class_attribute(conf_class_t *NOTNULL cls, const char *NOTNULL name, get_class_attr_t get_attr, lang_void *user_data_get, set_class_attr_t set_attr, lang_void *user_data_set, attr_attr_t attr, const char *type, const char *idx_type, const char *desc)
    '''
def SIM_register_work(f, data):
    '''
    void SIM_register_work(void (*NOTNULL f)(lang_void *data), lang_void *data)
    '''
def SIM_release_cell(obj, lock):
    '''
    void SIM_release_cell(conf_object_t *NOTNULL obj, domain_lock_t *lock)
    '''
def SIM_release_object(obj, lock):
    '''
    void SIM_release_object(conf_object_t *NOTNULL obj, domain_lock_t *lock)
    '''
def SIM_release_target(obj, lock):
    '''
    void SIM_release_target(conf_object_t *NOTNULL obj, domain_lock_t *lock)
    '''
def SIM_remove_output_handler(func, user_data):
    '''
    void SIM_remove_output_handler(NOTNULL output_handler_t func, lang_void *user_data)
    '''
def SIM_remove_profiling_area(handle):
    '''
    void SIM_remove_profiling_area(profile_area_t *NOTNULL handle)
    '''
def SIM_replace_transaction(t_old, t_new):
    '''
    void SIM_replace_transaction(transaction_t *NOTNULL t_old, transaction_t *t_new)
    '''
def SIM_require_object(obj):
    '''
    void SIM_require_object(conf_object_t *NOTNULL obj)
    '''
def SIM_reset_processor(cpu, hard_reset):
    '''
    void SIM_reset_processor(conf_object_t *NOTNULL cpu, int hard_reset)
    '''
def SIM_restore_snapshot(name):
    '''
    snapshot_error_t SIM_restore_snapshot(const char *NOTNULL name)
    '''
def SIM_run_alone(f, data):
    '''
    void SIM_run_alone(void (*NOTNULL f)(lang_void *data), lang_void *data)
    '''
def SIM_run_async_work(async_call, async_ready, arg):
    '''
    void SIM_run_async_work(lang_void *(*NOTNULL async_call)(lang_void *), void (*async_ready)(lang_void *, lang_void *), lang_void *arg)
    '''
def SIM_run_command(line):
    '''
    attr_value_t SIM_run_command(const char *NOTNULL line)
    '''
def SIM_run_command_file(file, local):
    '''
    void SIM_run_command_file(const char *NOTNULL file, bool local)
    '''
def SIM_run_command_file_params(file, local, params):
    '''
    void SIM_run_command_file_params(const char *NOTNULL file, bool local, attr_value_t params)
    '''
def SIM_run_in_thread(f, arg):
    '''
    void SIM_run_in_thread(void (*NOTNULL f)(lang_void *arg), lang_void *arg)
    '''
def SIM_run_python(line):
    '''
    attr_value_t SIM_run_python(const char *NOTNULL line)
    '''
def SIM_run_unrestricted(obj, func, user_data):
    '''
    void SIM_run_unrestricted(conf_object_t *NOTNULL obj, void (*NOTNULL func)(conf_object_t *obj, lang_void *param), lang_void *user_data)
    '''
def SIM_set_attribute(obj, name, value):
    '''
    set_error_t SIM_set_attribute(conf_object_t *NOTNULL obj, const char *NOTNULL name, attr_value_t *NOTNULL value)
    '''
def SIM_set_attribute_default(obj, name, value):
    '''
    set_error_t SIM_set_attribute_default(conf_object_t *NOTNULL obj, const char *NOTNULL name, attr_value_t value)
    '''
def SIM_set_attribute_idx(obj, name, index, value):
    '''
    set_error_t SIM_set_attribute_idx(conf_object_t *NOTNULL obj, const char *NOTNULL name, attr_value_t *NOTNULL index, attr_value_t *NOTNULL value)
    '''
def SIM_set_class_attribute(cls, name, value):
    '''
    set_error_t SIM_set_class_attribute(conf_class_t *NOTNULL cls, const char *NOTNULL name, attr_value_t *NOTNULL value)
    '''
def SIM_set_class_attribute_idx(cls, name, index, value):
    '''
    set_error_t SIM_set_class_attribute_idx(conf_class_t *NOTNULL cls, const char *NOTNULL name, attr_value_t *NOTNULL index, attr_value_t *NOTNULL value)
    '''
def SIM_set_configuration(conf):
    '''
    void SIM_set_configuration(attr_value_t conf)
    '''
def SIM_set_log_level(obj, level):
    '''
    void SIM_set_log_level(conf_object_t *NOTNULL obj, unsigned level)
    '''
def SIM_set_mem_op_exception(mop, exc):
    '''
    static inline void SIM_set_mem_op_exception(generic_transaction_t *NOTNULL mop, exception_type_t exc)
    '''
def SIM_set_mem_op_initiator(mop, type, obj):
    '''
    static inline void SIM_set_mem_op_initiator(generic_transaction_t *NOTNULL mop, ini_type_t type, conf_object_t *obj)
    '''
def SIM_set_mem_op_inquiry(mop, inquiry):
    '''
    static inline void SIM_set_mem_op_inquiry(generic_transaction_t *NOTNULL mop, bool inquiry)
    '''
def SIM_set_mem_op_physical_address(mop, pa):
    '''
    static inline void SIM_set_mem_op_physical_address(generic_transaction_t *NOTNULL mop, physical_address_t pa)
    '''
def SIM_set_mem_op_reissue(mop):
    '''
    static inline void SIM_set_mem_op_reissue(generic_transaction_t *NOTNULL mop)
    '''
def SIM_set_mem_op_type(mop, type):
    '''
    static inline void SIM_set_mem_op_type(generic_transaction_t *NOTNULL mop, mem_op_type_t type)
    '''
def SIM_set_mem_op_value_be(mop, value):
    '''
    void SIM_set_mem_op_value_be(generic_transaction_t *NOTNULL mop, uint64 value)
    '''
def SIM_set_mem_op_value_buf(mop, value):
    '''
    void SIM_set_mem_op_value_buf(generic_transaction_t *NOTNULL mop, attr_value_t value)
    '''
def SIM_set_mem_op_value_cpu(mop, value):
    '''
    void SIM_set_mem_op_value_cpu(generic_transaction_t *NOTNULL mop, uint64 value)
    '''
def SIM_set_mem_op_value_le(mop, value):
    '''
    void SIM_set_mem_op_value_le(generic_transaction_t *NOTNULL mop, uint64 value)
    '''
def SIM_set_mem_op_virtual_address(mop, va):
    '''
    static inline void SIM_set_mem_op_virtual_address(generic_transaction_t *NOTNULL mop, logical_address_t va)
    '''
def SIM_set_object_configured(obj):
    '''
    void SIM_set_object_configured(conf_object_t *NOTNULL obj)
    '''
def SIM_set_quiet(quiet):
    '''
    void SIM_set_quiet(bool quiet)
    '''
def SIM_set_transaction_bytes(t, bytes):
    '''
    void SIM_set_transaction_bytes(const transaction_t *NOTNULL t, bytes_t bytes)
    '''
def SIM_set_transaction_bytes_constant(t, value):
    '''
    void SIM_set_transaction_bytes_constant(const transaction_t *NOTNULL t, uint8 value)
    '''
def SIM_set_transaction_bytes_offs(t, offs, bytes):
    '''
    void SIM_set_transaction_bytes_offs(const transaction_t *NOTNULL t, unsigned offs, bytes_t bytes)
    '''
def SIM_set_transaction_value_be(t, val):
    '''
    void SIM_set_transaction_value_be(const transaction_t *NOTNULL t, uint64 val)
    '''
def SIM_set_transaction_value_le(t, val):
    '''
    void SIM_set_transaction_value_le(const transaction_t *NOTNULL t, uint64 val)
    '''
def SIM_set_verbose(verbose):
    '''
    void SIM_set_verbose(bool verbose)
    '''
def SIM_shutdown():
    '''
    void SIM_shutdown(void)
    '''
def SIM_simics_is_running():
    '''
    bool SIM_simics_is_running(void)
    '''
def SIM_source_python(file):
    '''
    void SIM_source_python(const char *NOTNULL file)
    '''
def SIM_source_python_in_module(file, module):
    '''
    void SIM_source_python_in_module(const char *NOTNULL file, const char *NOTNULL module)
    '''
def SIM_stall(obj, seconds):
    '''
    void SIM_stall(conf_object_t *NOTNULL obj, double seconds)
    '''
def SIM_stall_count(obj):
    '''
    cycles_t SIM_stall_count(conf_object_t *NOTNULL obj)
    '''
def SIM_stall_cycle(obj, cycles):
    '''
    void SIM_stall_cycle(conf_object_t *NOTNULL obj, cycles_t cycles)
    '''
def SIM_stalled_until(obj):
    '''
    cycles_t SIM_stalled_until(conf_object_t *NOTNULL obj)
    '''
def SIM_step_count(obj):
    '''
    pc_step_t SIM_step_count(conf_object_t *NOTNULL obj)
    '''
def SIM_take_snapshot(name):
    '''
    snapshot_error_t SIM_take_snapshot(const char *NOTNULL name)
    '''
def SIM_thread_safe_callback(f, data):
    '''
    void SIM_thread_safe_callback(void (*NOTNULL f)(lang_void *data), lang_void *data)
    '''
def SIM_time(obj):
    '''
    double SIM_time(conf_object_t *NOTNULL obj)
    '''
def SIM_transaction_flags(t):
    '''
    transaction_flags_t SIM_transaction_flags(const transaction_t *NOTNULL t)
    '''
def SIM_transaction_initiator(t):
    '''
    conf_object_t *SIM_transaction_initiator(const transaction_t *NOTNULL t)
    '''
def SIM_transaction_is_deferrable(t):
    '''
    bool SIM_transaction_is_deferrable(const transaction_t *NOTNULL t)
    '''
def SIM_transaction_is_fetch(t):
    '''
    bool SIM_transaction_is_fetch(const transaction_t *NOTNULL t)
    '''
def SIM_transaction_is_inquiry(t):
    '''
    bool SIM_transaction_is_inquiry(const transaction_t *NOTNULL t)
    '''
def SIM_transaction_is_read(t):
    '''
    bool SIM_transaction_is_read(const transaction_t *NOTNULL t)
    '''
def SIM_transaction_is_write(t):
    '''
    bool SIM_transaction_is_write(const transaction_t *NOTNULL t)
    '''
def SIM_transaction_size(t):
    '''
    unsigned SIM_transaction_size(const transaction_t *NOTNULL t)
    '''
def SIM_transaction_wait(t, ex):
    '''
    exception_type_t SIM_transaction_wait(transaction_t *NOTNULL t, exception_type_t ex)
    '''
def SIM_translation_changed(obj):
    '''
    void SIM_translation_changed(conf_object_t *NOTNULL obj)
    '''
def SIM_trigger_global_message(msg, ref):
    '''
    void SIM_trigger_global_message(const char *NOTNULL msg, lang_void *ref)
    '''
def SIM_version():
    '''
    const char *SIM_version(void)
    '''
def SIM_version_base():
    '''
    const char *SIM_version_base(void)
    '''
def SIM_version_major():
    '''
    const char *SIM_version_major(void)
    '''
def SIM_vmxmon_version():
    '''
    char *SIM_vmxmon_version(void)
    '''
def SIM_write_byte(obj, paddr, value):
    '''
    void SIM_write_byte(conf_object_t *NOTNULL obj, generic_address_t paddr, uint8 value)
    '''
def SIM_write_configuration_to_file(file, flags):
    '''
    void SIM_write_configuration_to_file(const char *NOTNULL file, save_flags_t flags)
    '''
def SIM_write_persistent_state(file, root, flags):
    '''
    void SIM_write_persistent_state(const char *NOTNULL file, conf_object_t *root, save_flags_t flags)
    '''
def SIM_write_phys_memory(cpu, paddr, value, length):
    '''
    void SIM_write_phys_memory(conf_object_t *NOTNULL cpu, physical_address_t paddr, uint64 value, int length)
    '''
def SIM_write_phys_memory_tags(mem_space, paddr, tag_bits, ntags):
    '''
    void SIM_write_phys_memory_tags(conf_object_t *NOTNULL mem_space, physical_address_t paddr, uint64 tag_bits, unsigned ntags)
    '''
def SIM_x86_mem_trans_from_generic(mop):
    '''
    x86_memory_transaction_t *SIM_x86_mem_trans_from_generic(generic_transaction_t *NOTNULL mop)
    '''
def SIM_yield_thread_domains():
    '''
    void SIM_yield_thread_domains(void)
    '''
def VT_abort_error(obj, msg):
    '''
    void VT_abort_error(conf_object_t *obj, const char *NOTNULL msg)
    '''
def VT_abort_execution_fiber():
    '''
    void VT_abort_execution_fiber(void)
    '''
def VT_abort_user(msg):
    '''
    void VT_abort_user(const char *msg)
    '''
def VT_activate_executor(ec, obj):
    '''
    void VT_activate_executor(execute_environ_t *NOTNULL ec, conf_object_t *obj)
    '''
def VT_add_objects(set):
    '''
    attr_value_t VT_add_objects(pre_conf_object_set_t *set)
    '''
def VT_add_permanent_object(obj):
    '''
    void VT_add_permanent_object(conf_object_t *NOTNULL obj)
    '''
def VT_add_telemetry_data(group, key, value):
    '''
    void VT_add_telemetry_data(const char *NOTNULL group, const char *NOTNULL key, attr_value_t *NOTNULL value)
    '''
def VT_add_telemetry_data_bool(group, key, value):
    '''
    void VT_add_telemetry_data_bool(const char *NOTNULL group, const char *NOTNULL key, bool value)
    '''
def VT_add_telemetry_data_int(group, key, value):
    '''
    void VT_add_telemetry_data_int(const char *NOTNULL group, const char *NOTNULL key, uint64 value)
    '''
def VT_add_telemetry_data_str(group, key, value):
    '''
    void VT_add_telemetry_data_str(const char *NOTNULL group, const char *NOTNULL key, const char *NOTNULL value)
    '''
def VT_assert_cell_context(obj, file, func):
    '''
    void VT_assert_cell_context(conf_object_t *NOTNULL obj, const char *NOTNULL file, const char *NOTNULL func)
    '''
def VT_assert_object_lock(obj, func, line):
    '''
    void VT_assert_object_lock(conf_object_t *NOTNULL obj, const char *NOTNULL func, const char *NOTNULL line)
    '''
def VT_assert_outside_execution_context(func, file, line):
    '''
    void VT_assert_outside_execution_context(const char *func, const char *file, int line)
    '''
def VT_async_events_pending():
    '''
    bool VT_async_events_pending(void)
    '''
def VT_async_events_pending_in_cell(cell):
    '''
    bool VT_async_events_pending_in_cell(conf_object_t *NOTNULL cell)
    '''
def VT_async_stall_usecs(usecs):
    '''
    void VT_async_stall_usecs(uint64 usecs)
    '''
def VT_attr_values_equal(a1, a2):
    '''
    bool VT_attr_values_equal(attr_value_t a1, attr_value_t a2)
    '''
def VT_call_python_module_function(module, func, args):
    '''
    attr_value_t VT_call_python_module_function(const char *module, const char *func, attr_value_t *args)
    '''
def VT_cell_global_time(cell):
    '''
    global_time_t VT_cell_global_time(conf_object_t *NOTNULL cell)
    '''
def VT_cell_threading_enabled(obj):
    '''
    bool VT_cell_threading_enabled(conf_object_t *obj)
    '''
def VT_check_async_events():
    '''
    bool VT_check_async_events(void)
    '''
def VT_check_async_events_from_cell(cell):
    '''
    bool VT_check_async_events_from_cell(conf_object_t *NOTNULL cell)
    '''
def VT_check_package_updates():
    '''
    void VT_check_package_updates(void)
    '''
def VT_clock_frequency_about_to_change(obj):
    '''
    void VT_clock_frequency_about_to_change(conf_object_t *NOTNULL obj)
    '''
def VT_clock_frequency_change(obj, cycles_per_second):
    '''
    void VT_clock_frequency_change(conf_object_t *NOTNULL obj, uint64 cycles_per_second)
    '''
def VT_command_line_create(obj, interactive, primary):
    '''
    cmd_line_id_t VT_command_line_create(conf_object_t *obj, bool interactive, bool primary)
    '''
def VT_command_line_delete(id):
    '''
    void VT_command_line_delete(cmd_line_id_t id)
    '''
def VT_command_line_key(id, mod, key):
    '''
    void VT_command_line_key(cmd_line_id_t id, cmd_line_mod_t mod, cmd_line_key_t key)
    '''
def VT_command_line_new_position(id, pos):
    '''
    void VT_command_line_new_position(cmd_line_id_t id, int pos)
    '''
def VT_command_line_new_selection(id, left, right):
    '''
    void VT_command_line_new_selection(cmd_line_id_t id, int left, int right)
    '''
def VT_command_line_set_size(cmd_line_id_t, cols, rows):
    '''
    void VT_command_line_set_size(cmd_line_id_t, int cols, int rows)
    '''
def VT_command_line_to_clipboard(id, str):
    '''
    void VT_command_line_to_clipboard(cmd_line_id_t id, const char *str)
    '''
def VT_cpuload_ns(obj):
    '''
    int64 VT_cpuload_ns(conf_object_t *NOTNULL obj)
    '''
def VT_create_execute_environ():
    '''
    execute_environ_t *VT_create_execute_environ(void)
    '''
def VT_critical_error(short_msg, long_msg):
    '''
    void VT_critical_error(const char *NOTNULL short_msg, const char *NOTNULL long_msg)
    '''
def VT_cross_cell_call(f, obj, arg):
    '''
    void VT_cross_cell_call(void (*NOTNULL f)(conf_object_t *obj, lang_void *arg), conf_object_t *NOTNULL obj, lang_void *arg)
    '''
def VT_ctx_remove_from_context_handler(obj, c_handler):
    '''
    void VT_ctx_remove_from_context_handler(conf_object_t *NOTNULL obj, conf_object_t *c_handler)
    '''
def VT_ctx_set_on_context_handler(obj, c_handler, bp_flags):
    '''
    void VT_ctx_set_on_context_handler(conf_object_t *NOTNULL obj, conf_object_t *c_handler, uint32 bp_flags)
    '''
def VT_cycles_to_quantum_end(obj):
    '''
    cycles_t VT_cycles_to_quantum_end(conf_object_t *NOTNULL obj)
    '''
def VT_deprecate(depr_build_id, warn_msg, ref_msg):
    '''
    void VT_deprecate(int depr_build_id, const char *NOTNULL warn_msg, const char *NOTNULL ref_msg)
    '''
def VT_disable_cpuload_stats():
    '''
    int VT_disable_cpuload_stats(void)
    '''
def VT_dispatch_async_events():
    '''
    void VT_dispatch_async_events(void)
    '''
def VT_domain_event_at(domain, ec, obj, clock, when, param):
    '''
    void VT_domain_event_at(conf_object_t *NOTNULL domain, event_class_t *NOTNULL ec, conf_object_t *NOTNULL obj, conf_object_t *NOTNULL clock, double when, lang_void *param)
    '''
def VT_domain_event_soon(domain, ec, obj, param):
    '''
    void VT_domain_event_soon(conf_object_t *NOTNULL domain, event_class_t *NOTNULL ec, conf_object_t *NOTNULL obj, lang_void *param)
    '''
def VT_dont_use_license_feature(feature_name):
    '''
    bool VT_dont_use_license_feature(const char *NOTNULL feature_name)
    '''
def VT_dump_snapshot(name):
    '''
    attr_value_t VT_dump_snapshot(const char *NOTNULL name)
    '''
def VT_effective_log_level(obj):
    '''
    unsigned VT_effective_log_level(const conf_object_t *NOTNULL obj)
    '''
def VT_enable_cpuload_stats():
    '''
    int VT_enable_cpuload_stats(void)
    '''
def VT_enter_running_mode():
    '''
    void VT_enter_running_mode(void)
    '''
def VT_execute(ec):
    '''
    bool VT_execute(execute_environ_t *NOTNULL ec)
    '''
def VT_exit_running_mode():
    '''
    void VT_exit_running_mode(void)
    '''
def VT_feature_in_use(arg, only_once):
    '''
    void VT_feature_in_use(const char *NOTNULL arg, bool only_once)
    '''
def VT_first_clock():
    '''
    conf_object_t *VT_first_clock(void)
    '''
def VT_free_all_icode():
    '''
    void VT_free_all_icode(void)
    '''
def VT_free_completion(c):
    '''
    void VT_free_completion(completion_t *c)
    '''
def VT_free_execute_environ(env):
    '''
    void VT_free_execute_environ(execute_environ_t *env)
    '''
def VT_free_machine_icode(requester):
    '''
    void VT_free_machine_icode(conf_object_t *NOTNULL requester)
    '''
def VT_free_thread_domain_group(g):
    '''
    void VT_free_thread_domain_group(thread_domain_group_t *NOTNULL g)
    '''
def VT_free_thread_domain_holder(h):
    '''
    void VT_free_thread_domain_holder(thread_domain_holder_t *NOTNULL h)
    '''
def VT_frontend_exception(exc_type, str):
    '''
    void VT_frontend_exception(sim_exception_t exc_type, const char *NOTNULL str)
    '''
def VT_generate_object_name():
    '''
    char *VT_generate_object_name(void)
    '''
def VT_get_all_attributes(class_name):
    '''
    attr_value_t VT_get_all_attributes(const char *NOTNULL class_name)
    '''
def VT_get_all_implementing_modules(class_name):
    '''
    attr_value_t VT_get_all_implementing_modules(const char *NOTNULL class_name)
    '''
def VT_get_all_instances(cls):
    '''
    attr_value_t VT_get_all_instances(conf_class_t *NOTNULL cls)
    '''
def VT_get_all_objects_impl(ifaces):
    '''
    attr_value_t VT_get_all_objects_impl(attr_value_t ifaces)
    '''
def VT_get_atom_class_id(name):
    '''
    atom_id_t VT_get_atom_class_id(const char *NOTNULL name)
    '''
def VT_get_attribute_type(cls, attr):
    '''
    const char *VT_get_attribute_type(conf_class_t *NOTNULL cls, const char *NOTNULL attr)
    '''
def VT_get_attributes(cls):
    '''
    attr_value_t VT_get_attributes(conf_class_t *NOTNULL cls)
    '''
def VT_get_break_message():
    '''
    const char *VT_get_break_message(void)
    '''
def VT_get_cell_execution_time():
    '''
    uint64 VT_get_cell_execution_time(void)
    '''
def VT_get_class_description(cls):
    '''
    const char *VT_get_class_description(const conf_class_t *NOTNULL cls)
    '''
def VT_get_class_extensions(cls):
    '''
    attr_value_t VT_get_class_extensions(conf_class_t *NOTNULL cls)
    '''
def VT_get_class_info(class_name):
    '''
    attr_value_t VT_get_class_info(const char *NOTNULL class_name)
    '''
def VT_get_class_kind(cls):
    '''
    class_kind_t VT_get_class_kind(const conf_class_t *NOTNULL cls)
    '''
def VT_get_class_short_desc(cls):
    '''
    const char *VT_get_class_short_desc(const conf_class_t *NOTNULL cls)
    '''
def VT_get_configuration(file):
    '''
    pre_conf_object_set_t *VT_get_configuration(const char *NOTNULL file)
    '''
def VT_get_current_cell():
    '''
    conf_object_t *VT_get_current_cell(void)
    '''
def VT_get_current_processor():
    '''
    conf_object_t *VT_get_current_processor(void)
    '''
def VT_get_current_processor_old():
    '''
    conf_object_t *VT_get_current_processor_old(void)
    '''
def VT_get_event_class(cl, name):
    '''
    event_class_t *VT_get_event_class(conf_class_t *NOTNULL cl, const char *NOTNULL name)
    '''
def VT_get_hide_consoles_flag():
    '''
    int VT_get_hide_consoles_flag(void)
    '''
def VT_get_interfaces(cls):
    '''
    attr_value_t VT_get_interfaces(conf_class_t *NOTNULL cls)
    '''
def VT_get_map_generation(obj):
    '''
    uint32 VT_get_map_generation(conf_object_t *NOTNULL obj)
    '''
def VT_get_object_by_name(name):
    '''
    conf_object_t *VT_get_object_by_name(const char *NOTNULL name)
    '''
def VT_get_pci_mem_op_requester_id(mop):
    '''
    uint16 VT_get_pci_mem_op_requester_id(const pci_memory_transaction_t *NOTNULL mop)
    '''
def VT_get_pci_mem_op_tlp_prefix(mop):
    '''
    uint32 VT_get_pci_mem_op_tlp_prefix(const pci_memory_transaction_t *NOTNULL mop)
    '''
def VT_get_port_classes(cls):
    '''
    attr_value_t VT_get_port_classes(conf_class_t *NOTNULL cls)
    '''
def VT_get_port_interfaces(cls):
    '''
    attr_value_t VT_get_port_interfaces(conf_class_t *NOTNULL cls)
    '''
def VT_get_port_obj_desc(cls, name):
    '''
    const char *VT_get_port_obj_desc(conf_class_t *NOTNULL cls, const char *NOTNULL name)
    '''
def VT_get_product_name(prod):
    '''
    char *VT_get_product_name(license_product_t prod)
    '''
def VT_get_saved_cwd():
    '''
    const char *VT_get_saved_cwd(void)
    '''
def VT_get_simulation_time():
    '''
    uint64 VT_get_simulation_time(void)
    '''
def VT_get_stop_reasons():
    '''
    attr_value_t VT_get_stop_reasons(void)
    '''
def VT_get_stop_type():
    '''
    stop_type_t VT_get_stop_type(void)
    '''
def VT_get_thread_domain(obj):
    '''
    thread_domain_t *VT_get_thread_domain(conf_object_t *NOTNULL obj)
    '''
def VT_get_transaction(id):
    '''
    transaction_t *VT_get_transaction(int64 id)
    '''
def VT_global_async_events_pending():
    '''
    bool VT_global_async_events_pending(void)
    '''
def VT_hap_global_callback_ranges(hap):
    '''
    attr_value_t VT_hap_global_callback_ranges(hap_type_t hap)
    '''
def VT_in_main_branch():
    '''
    bool VT_in_main_branch(void)
    '''
def VT_inside_instruction():
    '''
    bool VT_inside_instruction(void)
    '''
def VT_interrupt_script(user):
    '''
    void VT_interrupt_script(bool user)
    '''
def VT_io_memory_operation(obj, iface, memop, data, info):
    '''
    bytes_t VT_io_memory_operation(conf_object_t *NOTNULL obj, io_memory_interface_t *NOTNULL iface, generic_transaction_t *NOTNULL memop, bytes_t data, map_info_t info)
    '''
def VT_is_oec_thread():
    '''
    bool VT_is_oec_thread(void)
    '''
def VT_is_running_at_intel():
    '''
    bool VT_is_running_at_intel(void)
    '''
def VT_is_saving_persistent_data():
    '''
    bool VT_is_saving_persistent_data(void)
    '''
def VT_list_registered_atoms():
    '''
    attr_value_t VT_list_registered_atoms(void)
    '''
def VT_load_module_file(file):
    '''
    void VT_load_module_file(const char *NOTNULL file)
    '''
def VT_load_target_preset_yml(target, ns, presets, preset_yml):
    '''
    void VT_load_target_preset_yml(const char *NOTNULL target, const char *ns, attr_value_t presets, const char *preset_yml)
    '''
def VT_local_async_events_pending(obj):
    '''
    bool VT_local_async_events_pending(conf_object_t *NOTNULL obj)
    '''
def VT_log_always_count():
    '''
    int VT_log_always_count(void)
    '''
def VT_log_message(obj, level, group_ids, log_type, message):
    '''
    void VT_log_message(conf_object_t *NOTNULL obj, int level, int group_ids, log_type_t log_type, const char *NOTNULL message)
    '''
def VT_log_message64(obj, level, group_ids, log_type, message):
    '''
    void VT_log_message64(conf_object_t *NOTNULL obj, int level, uint64 group_ids, log_type_t log_type, const char *NOTNULL message)
    '''
def VT_logical_file_size(filename):
    '''
    int64 VT_logical_file_size(const char *NOTNULL filename)
    '''
def VT_logit(str):
    '''
    void VT_logit(const char *NOTNULL str)
    '''
def VT_lookup_atom_class_id(name):
    '''
    atom_id_t VT_lookup_atom_class_id(const char *NOTNULL name)
    '''
def VT_lookup_transaction_atom(t, id):
    '''
    const atom_t *VT_lookup_transaction_atom(const transaction_t *NOTNULL t, atom_id_t id)
    '''
def VT_magic_break_enable(enable):
    '''
    int VT_magic_break_enable(bool enable)
    '''
def VT_map_target_access(mt, mop):
    '''
    exception_type_t VT_map_target_access(const map_target_t *NOTNULL mt, generic_transaction_t *mop)
    '''
def VT_map_target_dm_lookup(mt, requester, addr, size, access):
    '''
    direct_memory_lookup_t VT_map_target_dm_lookup(const map_target_t *NOTNULL mt, conf_object_t *requester, physical_address_t addr, uint64 size, access_t access)
    '''
def VT_multicore_accelerator_enabled(obj):
    '''
    bool VT_multicore_accelerator_enabled(conf_object_t *obj)
    '''
def VT_new_completion():
    '''
    completion_t *VT_new_completion(void)
    '''
def VT_new_thread_domain():
    '''
    thread_domain_t *VT_new_thread_domain(void)
    '''
def VT_new_thread_domain_group():
    '''
    thread_domain_group_t *VT_new_thread_domain_group(void)
    '''
def VT_new_thread_domain_holder():
    '''
    thread_domain_holder_t *VT_new_thread_domain_holder(void)
    '''
def VT_object_cell(obj):
    '''
    conf_object_t *VT_object_cell(conf_object_t *NOTNULL obj)
    '''
def VT_object_checkpointable(obj):
    '''
    bool VT_object_checkpointable(conf_object_t *NOTNULL obj)
    '''
def VT_object_ps_clock(obj):
    '''
    conf_object_t *VT_object_ps_clock(conf_object_t *NOTNULL obj)
    '''
def VT_outside_execution_context_violation():
    '''
    bool VT_outside_execution_context_violation(void)
    '''
def VT_post_local_work(cell, f, data):
    '''
    void VT_post_local_work(conf_object_t *NOTNULL cell, void (*NOTNULL f)(lang_void *data), lang_void *data)
    '''
def VT_process_cross_cell_calls():
    '''
    void VT_process_cross_cell_calls(void)
    '''
def VT_python_decref(o):
    '''
    void VT_python_decref(SIM_PYOBJECT *NOTNULL o)
    '''
def VT_python_extension_data(obj, ext_cls):
    '''
    lang_void *VT_python_extension_data(conf_object_t *NOTNULL obj, conf_class_t *NOTNULL ext_cls)
    '''
def VT_real_network_warnings():
    '''
    void VT_real_network_warnings(void)
    '''
def VT_register_async_notifier(obj, wakeup):
    '''
    void VT_register_async_notifier(conf_object_t *obj, void (*wakeup)(conf_object_t *))
    '''
def VT_register_atom_class(name, pywrap_atom_from_type, pywrap_type_from_atom):
    '''
    void VT_register_atom_class(const char *NOTNULL name, const char *pywrap_atom_from_type, const char *pywrap_type_from_atom)
    '''
def VT_register_oec_thread():
    '''
    void VT_register_oec_thread(void)
    '''
def VT_register_passive_clock(cls, iface):
    '''
    int VT_register_passive_clock(conf_class_t *NOTNULL cls, const cycle_interface_t *NOTNULL iface)
    '''
def VT_register_port_array_interface(cls, name, iface_list, portname, desc):
    '''
    int VT_register_port_array_interface(conf_class_t *NOTNULL cls, const char *NOTNULL name, const interface_array_t *NOTNULL iface_list, const char *NOTNULL portname, const char *desc)
    '''
def VT_register_thread():
    '''
    void VT_register_thread(void)
    '''
def VT_rename_object(obj, newname):
    '''
    void VT_rename_object(conf_object_t *NOTNULL obj, const char *NOTNULL newname)
    '''
def VT_report_error(title, string):
    '''
    void VT_report_error(const char *NOTNULL title, const char *NOTNULL string)
    '''
def VT_run_outside_cell(f, arg):
    '''
    void VT_run_outside_cell(void (*NOTNULL f)(lang_void *arg), lang_void *arg)
    '''
def VT_set_completion(c, status):
    '''
    void VT_set_completion(completion_t *NOTNULL c, int status)
    '''
def VT_set_constructor_data(cls, data):
    '''
    void VT_set_constructor_data(conf_class_t *NOTNULL cls, lang_void *data)
    '''
def VT_set_delete_protection(obj, on):
    '''
    void VT_set_delete_protection(conf_object_t *NOTNULL obj, bool on)
    '''
def VT_set_execute_environ(obj, env):
    '''
    void VT_set_execute_environ(conf_object_t *NOTNULL obj, execute_environ_t *env)
    '''
def VT_set_object_checkpointable(obj, checkpointable):
    '''
    void VT_set_object_checkpointable(conf_object_t *NOTNULL obj, bool checkpointable)
    '''
def VT_set_object_clock(obj, clock):
    '''
    void VT_set_object_clock(conf_object_t *NOTNULL obj, conf_object_t *clock)
    '''
def VT_set_pci_mem_op_tlp_prefix(mop, tlp_prefix):
    '''
    void VT_set_pci_mem_op_tlp_prefix(pci_memory_transaction_t *NOTNULL mop, uint32 tlp_prefix)
    '''
def VT_set_prompt_customizer(cb):
    '''
    void VT_set_prompt_customizer(NOTNULL prompt_customizer_t cb)
    '''
def VT_set_restoring_state(state):
    '''
    void VT_set_restoring_state(bool state)
    '''
def VT_set_thread_domain(obj, td):
    '''
    void VT_set_thread_domain(conf_object_t *NOTNULL obj, thread_domain_t *td)
    '''
def VT_set_wait_handler(h):
    '''
    wait_handler_t *VT_set_wait_handler(wait_handler_t *h)
    '''
def VT_sign_module(filename):
    '''
    int VT_sign_module(const char *NOTNULL filename)
    '''
def VT_simulation_break_requested():
    '''
    bool VT_simulation_break_requested(void)
    '''
def VT_snapshot_size_used():
    '''
    attr_value_t VT_snapshot_size_used(void)
    '''
def VT_snapshots_ignore_class(class_name):
    '''
    void VT_snapshots_ignore_class(const char *NOTNULL class_name)
    '''
def VT_snapshots_skip_attr_restore(cls, attr_name):
    '''
    void VT_snapshots_skip_attr_restore(conf_class_t *NOTNULL cls, const char *NOTNULL attr_name)
    '''
def VT_snapshots_skip_class_restore(cls):
    '''
    void VT_snapshots_skip_class_restore(conf_class_t *NOTNULL cls)
    '''
def VT_stacked_post(obj, func, user_data):
    '''
    void VT_stacked_post(conf_object_t *NOTNULL obj, void (*NOTNULL func)(conf_object_t *obj, lang_void *param), lang_void *user_data)
    '''
def VT_start_execution_fiber(obj, func, param):
    '''
    void VT_start_execution_fiber(conf_object_t *NOTNULL obj, void (*func)(conf_object_t *obj, lang_void *param), lang_void *param)
    '''
def VT_step_stamp(step_obj):
    '''
    int128 VT_step_stamp(conf_object_t *NOTNULL step_obj)
    '''
def VT_stop_error(obj, msg):
    '''
    void VT_stop_error(conf_object_t *obj, const char *NOTNULL msg)
    '''
def VT_stop_event_processing(obj):
    '''
    void VT_stop_event_processing(conf_object_t *NOTNULL obj)
    '''
def VT_stop_execution(env):
    '''
    void VT_stop_execution(execute_environ_t *env)
    '''
def VT_stop_finished(msg):
    '''
    void VT_stop_finished(const char *msg)
    '''
def VT_stop_message(obj, msg):
    '''
    void VT_stop_message(conf_object_t *obj, const char *msg)
    '''
def VT_stop_user(msg):
    '''
    void VT_stop_user(const char *msg)
    '''
def VT_test_symbol_versioning():
    '''
    const char *VT_test_symbol_versioning(void)
    '''
def VT_thread_cell_association_begin(obj):
    '''
    void VT_thread_cell_association_begin(conf_object_t *NOTNULL obj)
    '''
def VT_thread_cell_association_end(obj):
    '''
    void VT_thread_cell_association_end(conf_object_t *NOTNULL obj)
    '''
def VT_thread_domain_call(obj, h, f, arg):
    '''
    void VT_thread_domain_call(conf_object_t *NOTNULL obj, thread_domain_holder_t *NOTNULL h, void (*NOTNULL f)(conf_object_t *obj, lang_void *arg), lang_void *arg)
    '''
def VT_thread_domain_decref(d):
    '''
    void VT_thread_domain_decref(thread_domain_t *NOTNULL d)
    '''
def VT_thread_domain_dispatch_messages(h):
    '''
    void VT_thread_domain_dispatch_messages(thread_domain_holder_t *NOTNULL h)
    '''
def VT_thread_domain_enter(obj, h):
    '''
    void VT_thread_domain_enter(conf_object_t *NOTNULL obj, thread_domain_holder_t *NOTNULL h)
    '''
def VT_thread_domain_enter_cell(obj, h):
    '''
    void VT_thread_domain_enter_cell(conf_object_t *NOTNULL obj, thread_domain_holder_t *NOTNULL h)
    '''
def VT_thread_domain_enter_execute(obj, h):
    '''
    void VT_thread_domain_enter_execute(conf_object_t *NOTNULL obj, thread_domain_holder_t *NOTNULL h)
    '''
def VT_thread_domain_enter_group(h, g):
    '''
    void VT_thread_domain_enter_group(thread_domain_holder_t *NOTNULL h, thread_domain_group_t *NOTNULL g)
    '''
def VT_thread_domain_group_add(g, obj):
    '''
    void VT_thread_domain_group_add(thread_domain_group_t *NOTNULL g, conf_object_t *NOTNULL obj)
    '''
def VT_thread_domain_group_remove(g, obj):
    '''
    void VT_thread_domain_group_remove(thread_domain_group_t *NOTNULL g, conf_object_t *NOTNULL obj)
    '''
def VT_thread_domain_incref(d):
    '''
    void VT_thread_domain_incref(thread_domain_t *NOTNULL d)
    '''
def VT_thread_domain_leave(obj, h):
    '''
    void VT_thread_domain_leave(conf_object_t *NOTNULL obj, thread_domain_holder_t *NOTNULL h)
    '''
def VT_thread_domain_leave_cell(obj, h):
    '''
    void VT_thread_domain_leave_cell(conf_object_t *NOTNULL obj, thread_domain_holder_t *NOTNULL h)
    '''
def VT_thread_domain_leave_execute(obj, h):
    '''
    void VT_thread_domain_leave_execute(conf_object_t *NOTNULL obj, thread_domain_holder_t *NOTNULL h)
    '''
def VT_thread_domain_leave_group(h, g):
    '''
    void VT_thread_domain_leave_group(thread_domain_holder_t *NOTNULL h, thread_domain_group_t *NOTNULL g)
    '''
def VT_unregister_async_notifier(obj, wakeup):
    '''
    void VT_unregister_async_notifier(conf_object_t *obj, void (*wakeup)(conf_object_t *obj))
    '''
def VT_unregister_thread():
    '''
    void VT_unregister_thread(void)
    '''
def VT_unrestricted_sync_point(obj):
    '''
    void VT_unrestricted_sync_point(conf_object_t *NOTNULL obj)
    '''
def VT_update_thread_domain_assignments():
    '''
    void VT_update_thread_domain_assignments(void)
    '''
def VT_use_ipv4():
    '''
    bool VT_use_ipv4(void)
    '''
def VT_user_interrupt(msg, break_script):
    '''
    void VT_user_interrupt(const char *msg, int break_script)
    '''
def VT_wait_for_completion(c):
    '''
    int VT_wait_for_completion(completion_t *NOTNULL c)
    '''
def VT_wait_for_simulator_init():
    '''
    void VT_wait_for_simulator_init(void)
    '''
def VT_wait_in_script_branch(command, wait_id, reverse, always, use_obj, caller, filename, line, wait_data):
    '''
    void VT_wait_in_script_branch(const char *command, int wait_id, bool reverse, bool always, bool use_obj, const char *caller, const char *filename, int line, const char *wait_data)
    '''
def VT_wr_product():
    '''
    void VT_wr_product(void)
    '''
def VT_yield_thread_domains(h):
    '''
    void VT_yield_thread_domains(thread_domain_holder_t *NOTNULL h)
    '''
class a20_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        a20_interface_t(set_a20_line=None, get_a20_line=None)
    '''


class abs_pointer_activate_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        abs_pointer_activate_interface_t(enable=None, disable=None)
    '''


class abs_pointer_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        abs_pointer_interface_t(set_state=None)
    '''


class abs_pointer_state_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        abs_pointer_state_t(buttons=0, x=0, y=0, z=0)
    '''


class addr_prof_iter_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class address_handle_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class address_profiler_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        address_profiler_interface_t(iter=None, sum=None, max=None,
            granularity_log2=None, address_bits=None, physical_addresses=None,
            description=None, num_views=None)
    '''


class alg_connection_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class alg_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        alg_interface_t(close_connection=None, data_from_real=None,
            data_from_simulated=None, connection_from_real=None,
            connection_from_simulated=None)
    '''


class apic_bus_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        apic_bus_interface_t(interrupt=None)
    '''


class apic_cpu_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        apic_cpu_interface_t(tpr_r=None, tpr_w=None, local_int=None,
            power_on=None, init=None, enabled_r=None)
    '''


class arinc429_bus_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        arinc429_bus_interface_t(send_word=None)
    '''


class arinc429_receiver_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        arinc429_receiver_interface_t(receive_word=None)
    '''


class arm_avic_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        arm_avic_interface_t(get_interrupt_address=None)
    '''


class arm_avic_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        arm_avic_t(valid=0, address=0)
    '''


class arm_coprocessor_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        arm_coprocessor_interface_t(process_data=None, load_coprocessor=None,
            read_register=None, write_register=None, read_register_64_bit=None,
            write_register_64_bit=None, store_coprocessor=None, reset=None)
    '''


class arm_cpu_group_event_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        arm_cpu_group_event_interface_t(signal_event=None)
    '''


class arm_cpu_group_exclusive_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        arm_cpu_group_exclusive_interface_t(mark_exclusive=None,
            clear_and_probe_exclusive=None)
    '''


class arm_cpu_group_tlb_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        arm_cpu_group_tlb_interface_t(invalidate_tlb=None)
    '''


class arm_cpu_state_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        arm_cpu_state_t(fmo_set=False, imo_set=False, irq_set=False,
            fiq_set=False, ns=False, el=0, mpidr=0)
    '''


class arm_external_debug_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        arm_external_debug_interface_t(handle_semihosting=None,
            read_reg=None, write_reg=None)
    '''


class arm_gic_cpu_state_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        arm_gic_cpu_state_interface_t(get_cpu_state=None)
    '''


class arm_gic_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        arm_gic_interface_t(read_register=None, write_register=None,
            cpu_state_changed=None)
    '''


class arm_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        arm_interface_t(read_register_mode=None, write_register_mode=None)
    '''


class arm_memory_transaction_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        arm_memory_transaction_t(s = generic_transaction_t(),
            mode = Sim_CPU_Mode_User, instr_origin = Normal_Load_Store,
            rotate = 0)
    '''


class arm_trustzone_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        arm_trustzone_interface_t(get_security_mode=None,
            mem_op_security_mode=None)
    '''


class atom_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class bank_access_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class bank_after_read_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        bank_after_read_interface_t(offset=None, size=None, missed=None,
            value=None, set_missed=None, set_value=None, initiator=None)
    '''


class bank_after_write_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        bank_after_write_interface_t(offset=None, size=None, missed=None,
            set_missed=None, initiator=None)
    '''


class bank_before_read_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        bank_before_read_interface_t(offset=None, size=None, set_offset=None,
            inquire=None, initiator=None)
    '''


class bank_before_write_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        bank_before_write_interface_t(offset=None, size=None, value=None,
            suppress=None, set_offset=None, set_value=None, initiator=None)
    '''


class bank_instrumentation_subscribe_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        bank_instrumentation_subscribe_interface_t(register_before_read=None,
            register_after_read=None, register_before_write=None,
            register_after_write=None, remove_callback=None,
            remove_connection_callbacks=None, enable_connection_callbacks=None,
            disable_connection_callbacks=None)
    '''


class bp_manager_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        bp_manager_interface_t(list_breakpoints=None, delete_breakpoint=None,
            get_properties=None, set_enabled=None, set_temporary=None,
            set_ignore_count=None, list_breakpoint_types=None,
            get_provider=None)
    '''


class branch_arc_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        branch_arc_interface_t(iter=None)
    '''


class branch_arc_iter_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class branch_arc_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class branch_recorder_handler_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        branch_recorder_handler_interface_t(attach_branch_recorder=None,
            detach_branch_recorder=None)
    '''


class break_strings_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        break_strings_interface_t(add=None, add_single=None, remove=None)
    '''


class break_strings_v2_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        break_strings_v2_interface_t(add=None, add_single=None,
            add_regexp=None, remove=None)
    '''


class breakpoint_change_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        breakpoint_change_interface_t(breakpoint_added=None,
            breakpoint_removed=None)
    '''


class breakpoint_info_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        breakpoint_info_t(handle=0, read_write_execute=0, start=0, end=0)
    '''


class breakpoint_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        breakpoint_interface_t(insert_breakpoint=None,
            remove_breakpoint=None, get_breakpoint=None)
    '''


class breakpoint_manager_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        breakpoint_manager_interface_t(list_breakpoints=None,
            delete_breakpoint=None, get_properties=None, set_enabled=None,
            set_temporary=None, set_ignore_count=None)
    '''


class breakpoint_query_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        breakpoint_query_interface_t(get_breakpoints=None)
    '''


class breakpoint_query_v2_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        breakpoint_query_v2_interface_t(get_breakpoints=None)
    '''


class breakpoint_registration_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        breakpoint_registration_interface_t(register_breakpoint=None,
            deleted=None)
    '''


class breakpoint_type_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        breakpoint_type_interface_t(register_type=None, trigger=None,
            get_break_id=None, get_manager_id=None)
    '''


class breakpoint_type_provider_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        breakpoint_type_provider_interface_t(register_bp=None, add_bp=None,
            remove_bp=None, trace_msg=None, break_msg=None, wait_msg=None,
            break_data=None, values=None, trace=None)
    '''


class bridge_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        bridge_interface_t(not_taken=None)
    '''


class cache_control_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        cache_control_interface_t(cache_control=None)
    '''


class cached_instruction_handle_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class callback_info_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        callback_info_interface_t(get_callbacks=None)
    '''


class cdrom_media_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        cdrom_media_interface_t(capacity=None, insert=None, eject=None)
    '''


class cell_inspection_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        cell_inspection_interface_t(set_current_processor_obj=None,
            set_current_step_obj=None)
    '''


class checkpoint_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        checkpoint_interface_t(save=None, finish=None,
            has_persistent_data=None, save_v2=None)
    '''


class class_data_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        class_data_t(alloc_object=None, init_object=None,
            finalize_instance=None, pre_delete_instance=None,
            delete_instance=None, description=None, class_desc=None,
            kind=Sim_Class_Kind_Vanilla)
    '''


class class_disassembly_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        class_disassembly_interface_t(disassemble_buffer=None,
            get_modes=None)
    '''


class class_info_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        class_info_t(alloc=None, init=None, finalize=None,
            objects_finalized=None, deinit=None, dealloc=None, description=None,
            short_desc=None, kind=Sim_Class_Kind_Vanilla)
    '''


class cmd_line_frontend_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        cmd_line_frontend_interface_t(write=None, delete_line=None,
            disconnect=None, cursor_left=None, cursor_right=None,
            clear_screen=None, prompt_end=None, bell=None)
    '''


class cmd_line_selection_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        cmd_line_selection_interface_t(new_selection=None, to_clipboard=None)
    '''


class co_execute_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        co_execute_interface_t(start_thread=None, yield=None)
    '''


class completion_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class component_connector_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        component_connector_interface_t(get_check_data=None,
            get_connect_data=None, check=None, connect=None, disconnect=None)
    '''


class component_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        component_interface_t(pre_instantiate=None, post_instantiate=None,
            create_cell=None, get_slots=None, get_slot_objects=None,
            get_slot_value=None, set_slot_value=None, has_slot=None,
            add_slot=None, del_slot=None)
    '''


class con_input_code_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        con_input_code_interface_t(input=None)
    '''


class con_input_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        con_input_interface_t(input_str=None, input_data=None)
    '''


class concurrency_group_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        concurrency_group_interface_t(serialized_memory_group=None,
            execution_group=None)
    '''


class concurrency_mode_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        concurrency_mode_interface_t(supported_modes=None, current_mode=None,
            switch_mode=None)
    '''


class conf_class_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class conf_object_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class connector_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        connector_interface_t(type=None, hotpluggable=None, required=None,
            multi=None, direction=None, add_destination=None,
            remove_destination=None, destination=None, update=None, check=None,
            connect=None, disconnect=None, deletion_requested=None)
    '''


class context_handler_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        context_handler_interface_t(get_current_context=None,
            set_current_context=None)
    '''


class coreint_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        coreint_interface_t(acknowledge=None)
    '''


class coreint_reply_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        coreint_reply_t(enabled=False, vector=0)
    '''


class cpu_cached_instruction_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        cpu_cached_instruction_interface_t(register_instruction_before_cb=None,
            register_instruction_after_cb=None, register_read_before_cb=None,
            register_read_after_cb=None, register_write_before_cb=None,
            register_write_after_cb=None)
    '''


class cpu_cached_instruction_once_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        cpu_cached_instruction_once_interface_t(register_instruction_before_once_cb=None,
            register_instruction_after_once_cb=None)
    '''


class cpu_exception_query_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        cpu_exception_query_interface_t(exception_number=None, fault_pc=None)
    '''


class cpu_instruction_decoder_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        cpu_instruction_decoder_interface_t(register_emulation_cb=None)
    '''


class cpu_instruction_query_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        cpu_instruction_query_interface_t(logical_address=None,
            physical_address=None, get_instruction_bytes=None)
    '''


class cpu_instrumentation_subscribe_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        cpu_instrumentation_subscribe_interface_t(remove_callback=None,
            enable_callback=None, disable_callback=None,
            remove_connection_callbacks=None, enable_connection_callbacks=None,
            disable_connection_callbacks=None,
            register_instruction_before_cb=None,
            register_instruction_after_cb=None, register_read_before_cb=None,
            register_read_after_cb=None, register_write_before_cb=None,
            register_write_after_cb=None, register_address_before_cb=None,
            register_cached_instruction_cb=None,
            register_instruction_decoder_cb=None,
            register_exception_before_cb=None, register_exception_after_cb=None,
            register_exception_return_before_cb=None,
            register_exception_return_after_cb=None,
            register_mode_change_cb=None,
            register_control_register_read_before_cb=None,
            register_control_register_write_before_cb=None)
    '''


class cpu_memory_query_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        cpu_memory_query_interface_t(logical_address=None,
            physical_address=None, get_bytes=None, set_bytes=None, atomic=None,
            arch=None, get_page_crossing_info=None, get_surrounding_bytes=None)
    '''


class cpuid_ret_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        cpuid_ret_t(taken=0, out_a=0, out_b=0, out_c=0, out_d=0)
    '''


class cpuid_value_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        cpuid_value_t(a=0, b=0, c=0, d=0)
    '''


class cxl_map_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        cxl_map_interface_t(add_map=None, del_map=None)
    '''


class cxl_mem_downstream_port_managing_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        cxl_mem_downstream_port_managing_interface_t(register_port_mem_obj=None,
            unregister_port_mem_obj=None)
    '''


class cxl_non_device_decoder_handling_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        cxl_non_device_decoder_handling_interface_t(enable_decoder=None,
            disable_decoder=None)
    '''


class cycle_control_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        cycle_control_interface_t(stop=None, start=None,
            set_cycle_count=None)
    '''


class cycle_event_instrumentation_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        cycle_event_instrumentation_interface_t(register_cycle_event_cb=None,
            remove_cycle_event_cb=None)
    '''


class cycle_event_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        cycle_event_interface_t(cycles=None, post=None, cancel=None,
            lookup=None, events=None)
    '''


class cycle_handle_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class cycle_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        cycle_interface_t(get_cycle_count=None, get_time=None,
            cycles_delta=None, get_frequency=None, post_cycle=None,
            post_time=None, cancel=None, find_next_cycle=None,
            find_next_time=None, events=None, get_time_in_ps=None,
            cycles_delta_from_ps=None, post_time_in_ps=None,
            find_next_time_in_ps=None)
    '''


class data_profiler_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        data_profiler_interface_t(accum_in_range=None, save=None, load=None,
            clear=None)
    '''


class datagram_link_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        datagram_link_interface_t(receive=None)
    '''


class debug_notification_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        debug_notification_interface_t(notify_context_creation=None,
            notify_context_destruction=None, notify_location=None,
            notify_address=None, notify_line=None, notify_activated=None,
            notify_deactivated=None, notify_callbacks_done=None, cancel=None)
    '''


class debug_query_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        debug_query_interface_t(matching_contexts=None,
            get_context_group=None, get_context_parent=None,
            get_context_children=None, query_for_context_group=None,
            query_for_context_id=None, query_for_context_tree=None,
            context_name=None, context_id_for_object=None,
            object_for_context=None, context_has_state=None,
            get_active_processor=None)
    '''


class debug_setup_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        debug_setup_interface_t(add_symbol_file=None,
            add_symbol_segment=None, add_symbol_section=None,
            remove_symbol_file=None, clear_symbol_files=None, symbol_files=None,
            symbol_files_for_ctx=None, list_all_mappings=None,
            add_path_map_entry=None, remove_path_map_entry=None,
            clear_path_map_entries=None, path_map_entries=None,
            path_map_entries_for_ctx=None, apply_path_map=None)
    '''


class debug_step_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        debug_step_interface_t(instruction_into=None, into=None,
            instruction_over=None, over=None, out=None,
            reverse_instruction_into=None, reverse_into=None,
            reverse_instruction_over=None, reverse_over=None, reverse_out=None)
    '''


class debug_symbol_file_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        debug_symbol_file_interface_t(open_symbol_file=None,
            close_symbol_file=None, symbol_file_info=None, sections_info=None,
            segments_info=None, open_symbol_section=None)
    '''


class debug_symbol_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        debug_symbol_interface_t(address_source=None, source_address=None,
            address_symbol=None, stack_depth=None, stack_frames=None,
            local_variables=None, local_arguments=None, expression_value=None,
            expression_type=None, type_info=None, type_to_string=None,
            symbol_address=None, address_string=None, lvalue_write=None,
            address_write=None, address_read=None, struct_members=None,
            struct_field=None, list_functions=None, list_global_variables=None,
            list_source_files=None)
    '''


class decoder_handle_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class deferred_transaction_info_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class device_identification_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        device_identification_interface_t(get_id=None, get_key=None)
    '''


class direct_memory_flush_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        direct_memory_flush_interface_t(revoke=None, set_access_bits=None)
    '''


class direct_memory_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        direct_memory_interface_t(get_handle=None, request=None, revoke=None,
            release=None, ack=None)
    '''


class direct_memory_lookup_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        direct_memory_lookup_interface_t(lookup=None)
    '''


class direct_memory_lookup_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class direct_memory_lookup_v2_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        direct_memory_lookup_v2_interface_t(lookup=None)
    '''


class direct_memory_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class direct_memory_tags_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        direct_memory_tags_interface_t(get_tags_data=None)
    '''


class direct_memory_tags_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class direct_memory_update_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        direct_memory_update_interface_t(release=None,
            update_permission=None, conflicting_access=None)
    '''


class disassembly_instruction_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class disassembly_mode_info_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class disk_component_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        disk_component_interface_t(size=None)
    '''


class dist_control_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        dist_control_interface_t(initiate_async_flush=None,
            flush_async_channel_up=None)
    '''


class dist_control_node_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        dist_control_node_interface_t(flush_async_channel_down=None)
    '''


class domain_lock_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class dummy_memory_page_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        dummy_memory_page_interface_t(dummy=None)
    '''


class duration_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class ethernet_cable_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        ethernet_cable_interface_t(link_status=None)
    '''


class ethernet_common_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        ethernet_common_interface_t(frame=None)
    '''


class ethernet_probe_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        ethernet_probe_interface_t(attach_snooper=None, attach_probe=None,
            detach=None, send_frame=None)
    '''


class ethernet_snoop_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        ethernet_snoop_interface_t(attach=None)
    '''


class ethernet_vlan_snoop_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        ethernet_vlan_snoop_interface_t(attach=None)
    '''


class event_class_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class event_delta_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        event_delta_interface_t(set_delta=None, get_delta=None)
    '''


class event_handler_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        event_handler_interface_t(handle_event=None, stop=None)
    '''


class exception_handle_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class exception_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        exception_interface_t(get_number=None, get_name=None,
            get_source=None, all_exceptions=None)
    '''


class exception_return_handle_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class exec_trace_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        exec_trace_interface_t(register_tracer=None, unregister_tracer=None)
    '''


class execute_control_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        execute_control_interface_t(message_pending=None, yield_request=None)
    '''


class execute_environ_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class execute_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        execute_interface_t(run=None, stop=None, switch_in=None,
            switch_out=None)
    '''


class extended_serial_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        extended_serial_interface_t(write_at=None, graphics_mode=None)
    '''


class external_connection_ctl_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        external_connection_ctl_interface_t(accept=None, read=None,
            write=None, write_async=None, has_error=None, notify=None,
            close=None)
    '''


class external_connection_events_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        external_connection_events_interface_t(on_accept=None, on_input=None,
            can_write=None)
    '''


class firewire_bus_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        firewire_bus_interface_t(connect_device=None, disconnect_device=None,
            set_device_bus_id=None, set_id_mask=None, transfer=None,
            register_channel=None, unregister_channel=None, reset=None)
    '''


class firewire_device_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        firewire_device_interface_t(transfer=None, reset=None,
            get_self_id_template=None, get_rhb=None, get_port_count=None,
            get_port_mask=None)
    '''


class fmn_station_control_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        fmn_station_control_interface_t(send_message=None, load_message=None,
            wait=None, sync=None)
    '''


class fmn_station_control_v2_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        fmn_station_control_v2_interface_t(send_message=None,
            load_message=None, wait=None, sync=None)
    '''


class follower_agent_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        follower_agent_interface_t(accept=None, accept_async=None)
    '''


class forward_destination_rewriter_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        forward_destination_rewriter_interface_t(get_new_destination=None)
    '''


class forward_rewrite_endpoint_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        forward_rewrite_endpoint_t(ip_addr=..., ip_addr_len=0, port=0,
            initiated=False)
    '''


class freerun_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        freerun_interface_t(enabled=None, advance_clock=None,
            start_clock=None, stop_clock=None, current_itime=None)
    '''


class frequency_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        frequency_interface_t(get=None)
    '''


class frequency_listener_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        frequency_listener_interface_t(set=None)
    '''


class gbic_transceiver_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        gbic_transceiver_interface_t(read_mod_def=None, write_mod_def=None,
            loopback=None)
    '''


class generic_transaction_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        generic_transaction_t(logical_address = 0, physical_address = 0,
            size = 0, type = Sim_Trans_Load, atomic = 0, inquiry = 0,
            non_coherent = 0, may_stall = 0, reissue = 0,
            block_STC = 0, use_page_cache = 0, inverse_endian = 0,
            page_cross = 0, mem_hier_on_fetch = 0, block_flags = 0,
            ini_type = Sim_Initiator_Illegal, ini_ptr = None, exception = 0,
            transaction = None)
    '''


def get_compile_info(int):
    '''
    int get_compile_info(int)
    '''
class gfx_break_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        gfx_break_interface_t(store=None, add=None, remove=None, match=None,
            info=None, export_png=None, add_bytes=None)
    '''


class gfx_con_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        gfx_con_interface_t(set_color=None, set_size=None, put_pixel=None,
            put_pixel_rgb=None, redraw=None, update_keyboard_leds=None,
            put_pixel_col=None, put_block=None)
    '''


class gfx_console_backend_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        gfx_console_backend_interface_t(kbd_event=None, mouse_event=None,
            request_refresh=None, set_visible=None, text_data=None,
            got_grab_keys=None)
    '''


class gfx_console_frontend_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        gfx_console_frontend_interface_t(start=None, stop=None,
            set_title=None, set_size=None, set_visible=None,
            signal_text_update=None, set_grab_mode=None, set_mouse_pos=None,
            set_keyboard_leds=None, set_grab_modifier=None,
            set_grab_button=None)
    '''


class gic_reg_info_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        gic_reg_info_t(op1=0, crn=0, crm=0, op2=0,
            cpu_state=arm_cpu_state_t())
    '''


class global_notifier_callback_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class global_time_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        global_time_interface_t(set=None, get=None, get_low=None, clear=None,
            valid=None)
    '''


class granted_mem_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class gui_console_backend_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        gui_console_backend_interface_t(start=None, stop=None)
    '''


class host_hypervisor_info_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class host_serial_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        host_serial_interface_t(setup=None, name=None, shutdown=None)
    '''


class i2c_bridge_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        i2c_bridge_interface_t(address_added=None, address_removed=None)
    '''


class i2c_bus_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        i2c_bus_interface_t(start=None, stop=None, read_data=None,
            write_data=None, register_device=None, unregister_device=None)
    '''


class i2c_device_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        i2c_device_interface_t(set_state=None, read_data=None,
            write_data=None)
    '''


class i2c_link_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        i2c_link_interface_t(register_slave_address=None,
            unregister_slave_address=None, register_bridge=None,
            disconnect_device=None, start_request=None, read_request=None,
            ack_read_request=None, write_request=None, read_response=None,
            ack_read_response=None, write_response=None, start_response=None,
            stop=None)
    '''


class i2c_master_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        i2c_master_interface_t(bus_freed=None, read_response=None,
            ack_read_response=None, write_response=None, start_response=None)
    '''


class i2c_master_v2_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        i2c_master_v2_interface_t(acknowledge=None, read_response=None)
    '''


class i2c_slave_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        i2c_slave_interface_t(start_request=None, read_request=None,
            ack_read_request=None, write_request=None, stop=None)
    '''


class i2c_slave_v2_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        i2c_slave_v2_interface_t(start=None, read=None, write=None,
            stop=None, addresses=None)
    '''


class i3c_daa_snoop_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        i3c_daa_snoop_interface_t(assigned_address=None)
    '''


class i3c_hdr_master_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        i3c_hdr_master_interface_t(hdr_read_response=None,
            hdr_acknowledge=None)
    '''


class i3c_hdr_slave_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        i3c_hdr_slave_interface_t(hdr_write=None, hdr_read=None,
            hdr_restart=None, hdr_exit=None)
    '''


class i3c_master_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        i3c_master_interface_t(acknowledge=None, read_response=None,
            daa_response=None, ibi_request=None, ibi_address=None)
    '''


class i3c_slave_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        i3c_slave_interface_t(start=None, write=None, sdr_write=None,
            read=None, daa_read=None, stop=None, ibi_start=None,
            ibi_acknowledge=None)
    '''


class i8051_interrupt_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        i8051_interrupt_interface_t(active_interrupt=None, reti=None,
            suppress_irq=None)
    '''


class i8051_timer_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        i8051_timer_interface_t(change_mode=None, switch_timer=None)
    '''


class icode_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        icode_interface_t(release_all_icode=None)
    '''


class ieee_802_3_mac_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        ieee_802_3_mac_interface_t(receive_frame=None,
            tx_bandwidth_available=None, link_status_changed=None)
    '''


class ieee_802_3_mac_v3_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        ieee_802_3_mac_v3_interface_t(receive_frame=None,
            tx_bandwidth_available=None, link_status_changed=None)
    '''


class ieee_802_3_phy_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        ieee_802_3_phy_interface_t(send_frame=None, check_tx_bandwidth=None,
            set_promiscous_mode=None)
    '''


class ieee_802_3_phy_v2_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        ieee_802_3_phy_v2_interface_t(send_frame=None,
            check_tx_bandwidth=None, add_mac=None, del_mac=None,
            add_mac_mask=None, del_mac_mask=None, set_promiscous_mode=None)
    '''


class ieee_802_3_phy_v3_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        ieee_802_3_phy_v3_interface_t(send_frame=None,
            check_tx_bandwidth=None)
    '''


class image_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        image_interface_t(set_persistent=None, save_to_file=None,
            save_diff=None, clear_range=None, fill=None, size=None, set=None,
            get=None, flush_writable=None)
    '''


class instruction_fetch_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        instruction_fetch_interface_t(get_mode=None, set_mode=None,
            get_line_size=None, set_line_size=None)
    '''


class instruction_handle_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class instrum_registration_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        instrum_registration_interface_t(pre_register_provider_contract=None,
            register_provider_contract=None, pre_register_tool_contract=None,
            register_tool_contract=None)
    '''


class instrumentation_connection_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        instrumentation_connection_interface_t(enable=None, disable=None)
    '''


class instrumentation_filter_master_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        instrumentation_filter_master_interface_t(set_source_id=None,
            add_slave=None, remove_slave=None, short_filter_config=None)
    '''


class instrumentation_filter_slave_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        instrumentation_filter_slave_interface_t(disable=None, enable=None)
    '''


class instrumentation_filter_status_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        instrumentation_filter_status_interface_t(get_disabled_sources=None)
    '''


class instrumentation_order_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        instrumentation_order_interface_t(get_connections=None,
            move_before=None)
    '''


class instrumentation_tool_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        instrumentation_tool_interface_t(connect=None, disconnect=None)
    '''


class int_register_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        int_register_interface_t(get_number=None, get_name=None, read=None,
            write=None, all_registers=None, register_info=None)
    '''


class internal_class_testing_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        internal_class_testing_interface_t(test=None)
    '''


class interrupt_ack_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        interrupt_ack_interface_t(raise_interrupt=None, lower_interrupt=None)
    '''


class interrupt_cpu_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        interrupt_cpu_interface_t(ack=None)
    '''


class interrupt_subscriber_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        interrupt_subscriber_interface_t(notify=None, reset=None)
    '''


class io_memory_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        io_memory_interface_t(_deprecated_map=None, operation=None)
    '''


class ip_port_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class jit_control_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        jit_control_interface_t(set_compile_enable=None)
    '''


class keyboard_console_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        keyboard_console_interface_t(keyboard_ready=None)
    '''


class keyboard_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        keyboard_interface_t(keyboard_event=None)
    '''


class leader_message_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        leader_message_interface_t(send=None, send_async=None)
    '''


class linear_image_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        linear_image_interface_t(prepare_range=None)
    '''


class link_endpoint_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        link_endpoint_interface_t(delivery_time=None, delivery_skey=None)
    '''


class link_endpoint_v2_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        link_endpoint_v2_interface_t(delivery_time=None, delivery_skey=None)
    '''


class local_time_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        local_time_t(clock=None, t=bigtime_t())
    '''


class log_object_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        log_object_interface_t()
    '''


class map_demap_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        map_demap_interface_t(add_map=None, remove_map=None,
            add_default=None, remove_default=None, map_simple=None,
            map_bridge=None, unmap=None, unmap_address=None)
    '''


class map_info_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        map_info_t(base=0, start=0, length=0, function=0, priority=0,
            align_size=0, reverse_endian=Sim_Swap_None)
    '''


class map_list_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class map_target_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        map_target_t()
    '''


class maybe_node_id_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        maybe_node_id_t(valid=False, id=0)
    '''


class mdio45_bus_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        mdio45_bus_interface_t(read_register=None, write_register=None)
    '''


class mdio45_phy_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        mdio45_phy_interface_t(read_register=None, write_register=None)
    '''


class memory_handle_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class memory_profiler_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        memory_profiler_interface_t(get=None, set=None,
            get_granularity_log2=None)
    '''


class memory_space_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        memory_space_interface_t(space_lookup=None, access=None, read=None,
            write=None, timing_model_operate=None, fill=None)
    '''


class microwire_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        microwire_interface_t(set_cs=None, set_sk=None, set_di=None,
            get_do=None, read_word=None, write_word=None)
    '''


class mii_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        mii_interface_t(serial_access=None, read_register=None,
            write_register=None)
    '''


class mii_management_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        mii_management_interface_t(serial_access=None, read_register=None,
            write_register=None)
    '''


class mips_cache_instruction_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        mips_cache_instruction_interface_t(cache_instruction=None)
    '''


class mips_coprocessor_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        mips_coprocessor_interface_t(read_register=None, write_register=None)
    '''


class mips_eic_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        mips_eic_interface_t(cpu_pending_irqs=None, requested_ipl=None,
            requested_offset=None, requested_vect_num=None, reg_set=None,
            handled=None)
    '''


class mips_exception_query_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        mips_exception_query_interface_t(return_pc=None)
    '''


class mips_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        mips_interface_t()
    '''


class mips_ite_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        mips_ite_interface_t(set_dtag_lo=None, get_dtag_lo=None,
            get_errctl=None, block_tc=None, gated_exception=None,
            current_tc_num=None, unblock_tc=None, is_big_endian=None)
    '''


class mips_memory_transaction_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        mips_memory_transaction_t(s = generic_transaction_t(),
            cache_coherency = 0)
    '''


class mips_mt_ase_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        mips_mt_ase_interface_t(vpe_id=None, current_thread_num=None,
            cycles_per_tc=None, last_schedule_cycle=None, primary_vpe=None,
            vpe=None, tc_gprs=None, tc_lo=None, tc_hi=None,
            tc_reschedule_conditions=None)
    '''


class mips_mt_ase_policy_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        mips_mt_ase_policy_interface_t(read_grp_prio=None, scheduler=None)
    '''


class mmc_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        mmc_interface_t(write_data=None)
    '''


class mouse_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        mouse_interface_t(mouse_event=None)
    '''


class multi_level_signal_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        multi_level_signal_interface_t(signal_level_change=None,
            signal_current_level=None)
    '''


class nand_flash_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        nand_flash_interface_t(read_access=None, write_access=None,
            set_command_latch_enable=None, set_address_latch_enable=None,
            set_write_protect=None, set_spare_area_enable=None)
    '''


class network_breakpoint_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        network_breakpoint_interface_t(add=None, remove=None)
    '''


class next_map_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        next_map_t(mt=None, ex=0, addr=0)
    '''


class nios_cache_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        nios_cache_interface_t(flushd=None, flushda=None, flushi=None,
            flushp=None, initd=None, initda=None, initi=None, sync=None)
    '''


class nios_custom_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        nios_custom_interface_t(custom=None)
    '''


class nios_eic_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        nios_eic_interface_t(handler=None, level=None, reg_set=None,
            nmi=None, handled=None)
    '''


class nios_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        nios_interface_t()
    '''


class notifier_handle_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class opcode_info_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        opcode_info_interface_t(get_opcode_length_info=None)
    '''


class opcode_length_info_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        opcode_length_info_t(min_alignment=0, max_length=0, avg_length=0)
    '''


class osa_component_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        osa_component_interface_t(get_admin=None, get_root_node=None,
            notify_tracker=None, cancel_notify=None, has_tracker=None,
            get_processors=None)
    '''


class osa_control_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        osa_control_interface_t(request=None, release=None)
    '''


class osa_control_v2_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        osa_control_v2_interface_t(request=None, release=None,
            clear_state=None)
    '''


class osa_machine_notification_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        osa_machine_notification_interface_t(notify_mode_change=None,
            notify_exception=None, notify_control_reg=None,
            notify_exec_breakpoint=None, notify_read_breakpoint=None,
            notify_write_breakpoint=None, cancel=None,
            notify_control_reg_read=None)
    '''


class osa_machine_query_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        osa_machine_query_interface_t(read_register=None,
            get_register_number=None, read_phys_memory=None,
            read_phys_bytes=None, virtual_to_physical=None, cpu_mode=None,
            get_all_processors=None, get_exception_number=None)
    '''


class osa_mapper_admin_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        osa_mapper_admin_interface_t(tracker_updated=None)
    '''


class osa_mapper_control_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        osa_mapper_control_interface_t(disable=None, enable=None,
            clear_state=None)
    '''


class osa_mapper_query_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        osa_mapper_query_interface_t(get_process_list=None, get_mapper=None)
    '''


class osa_micro_checkpoint_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        osa_micro_checkpoint_interface_t(started=None, finished=None)
    '''


class osa_node_path_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        osa_node_path_interface_t(matching_nodes=None, node_path=None)
    '''


class osa_node_tree_admin_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        osa_node_tree_admin_interface_t(begin=None, end=None, create=None,
            add=None, update=None, remove=None, event=None, activate=None,
            deactivate=None, register_formatter=None, unregister_formatter=None,
            reset=None, set_property=None)
    '''


class osa_node_tree_notification_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        osa_node_tree_notification_interface_t(notify_create=None,
            notify_destroy=None, notify_property_change=None,
            notify_cpu_move_from=None, notify_cpu_move_to=None,
            notify_event=None, notify_enable=None, notify_disable=None,
            cancel_notify=None, notify_callbacks_done=None)
    '''


class osa_node_tree_query_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        osa_node_tree_query_interface_t(get_root_nodes=None, get_node=None,
            get_current_nodes=None, get_current_processors=None,
            get_all_processors=None, get_mapper=None, get_parent=None,
            get_children=None, get_formatted_properties=None)
    '''


class osa_parameters_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        osa_parameters_interface_t(get_parameters=None, set_parameters=None,
            is_kind_supported=None)
    '''


class osa_tracker_component_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        osa_tracker_component_interface_t(get_tracker=None, get_mapper=None)
    '''


class osa_tracker_control_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        osa_tracker_control_interface_t(disable=None, enable=None,
            clear_state=None, add_processor=None, remove_processor=None)
    '''


class osa_tracker_state_admin_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        osa_tracker_state_admin_interface_t(begin=None, end=None, add=None,
            remove=None, remove_all=None, set_attribute=None, update=None,
            event=None)
    '''


class osa_tracker_state_notification_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        osa_tracker_state_notification_interface_t(subscribe_tracker=None,
            unsubscribe_tracker=None)
    '''


class osa_tracker_state_query_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        osa_tracker_state_query_interface_t(_deprecated=None,
            get_entities=None, get_entity=None)
    '''


class packet_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        packet_interface_t(transfer=None)
    '''


class page_bank_client_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        page_bank_client_interface_t(init_page=None, release_page=None,
            update_permission=None, conflicting_access=None)
    '''


class page_bank_grant_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class page_bank_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        page_bank_interface_t(attach=None, detach=None, lookup=None,
            request=None, drop=None, query=None, iterate=None, revoke=None,
            page_fill=None, page_size=None, size=None)
    '''


class page_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class pb_page_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class pci_bridge_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        pci_bridge_interface_t(system_error=None, raise_interrupt=None,
            lower_interrupt=None)
    '''


class pci_bus_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        pci_bus_interface_t(memory_access=None, raise_interrupt=None,
            lower_interrupt=None, interrupt_acknowledge=None, add_map=None,
            remove_map=None, set_bus_number=None, set_sub_bus_number=None,
            add_default=None, remove_default=None, bus_reset=None,
            special_cycle=None, system_error=None, get_bus_address=None,
            set_device_status=None, configuration_space=None, io_space=None,
            memory_space=None)
    '''


class pci_device_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        pci_device_interface_t(bus_reset=None,
            _deprecated_interrupt_acknowledge=None,
            _deprecated_special_cycle=None, system_error=None,
            interrupt_raised=None, interrupt_lowered=None)
    '''


class pci_downstream_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        pci_downstream_interface_t(operation=None)
    '''


class pci_express_hotplug_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        pci_express_hotplug_interface_t(presence_change=None,
            inject_power_fault=None, press_attention_button=None,
            set_mrl_state=None, get_mrl_state=None)
    '''


class pci_express_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        pci_express_interface_t(send_message=None)
    '''


class pci_interrupt_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        pci_interrupt_interface_t(raise_interrupt=None, lower_interrupt=None)
    '''


class pci_memory_transaction_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        pci_memory_transaction_t(_internal_s = generic_transaction_t(),
            _internal_original_size = 0, _internal_bus_address = 0,
            _internal_bus_number = 0, _internal_device_number = 0,
            _internal_function_number = 0, _internal_tlp_prefix = 0)
    '''


class pci_multi_function_device_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        pci_multi_function_device_interface_t(supported_functions=None)
    '''


class pci_upstream_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        pci_upstream_interface_t(operation=None)
    '''


class pci_upstream_operation_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        pci_upstream_operation_interface_t(read=None, write=None)
    '''


class pcie_adapter_compat_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        pcie_adapter_compat_interface_t(set_secondary_bus_number=None)
    '''


class pcie_byte_count_ret_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        pcie_byte_count_ret_t(val=0)
    '''


class pcie_device_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        pcie_device_interface_t(connected=None, disconnected=None,
            hot_reset=None)
    '''


class pcie_error_ret_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        pcie_error_ret_t(val=PCIE_Error_Not_Set)
    '''


class pcie_hotplug_events_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        pcie_hotplug_events_interface_t(presence_change=None,
            power_fault=None, attention_button_pressed=None, mrl_sensor=None,
            data_link_layer=None)
    '''


class pcie_ide_secured_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        pcie_ide_secured_t(t=False, k=False, m=False, p=False, sub_stream=0,
            stream_id=0, pr_sent_counter=0)
    '''


class pcie_link_negotiation_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        pcie_link_negotiation_t(maximum_link_speed=PCIE_Link_Speed_Undefined,
            maximum_link_width=PCIE_Link_Width_Undefined,
            negotiated_link_speed=PCIE_Link_Speed_Undefined,
            negotiated_link_width=PCIE_Link_Width_Undefined)
    '''


class pcie_link_training_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        pcie_link_training_interface_t(trigger=None)
    '''


class pcie_map_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        pcie_map_interface_t(add_map=None, del_map=None, add_function=None,
            del_function=None, enable_function=None, disable_function=None,
            get_device_id=None)
    '''


class pcie_port_control_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        pcie_port_control_interface_t(set_secondary_bus_number=None,
            hot_reset=None)
    '''


class physical_block_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        physical_block_t(valid=0, address=0, block_start=0, block_end=0)
    '''


class pmr_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        pmr_interface_t(get=None, set=None)
    '''


class pool_protect_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        pool_protect_interface_t(create_group=None, protect=None)
    '''


class port_forward_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        port_forward_interface_t(port_unlisten=None, portmap=None,
            get_simulated_ip_port=None, get_real_ip_port=None,
            get_host_ip_port=None, get_service_node_ip_port=None,
            data_to_real=None, data_to_simulated=None,
            set_destination_rewriter=None)
    '''


class port_space_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        port_space_interface_t(port_operation=None, read=None, write=None)
    '''


class ppc_decoration_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class ppc_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        ppc_interface_t(clear_atomic_reservation_bit=None,
            raise_machine_check_exception=None, get_timebase_enabled=None,
            set_timebase_enabled=None, get_sleep_state=None,
            set_sleep_state=None)
    '''


class ppc_memory_transaction_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        ppc_memory_transaction_t(s = generic_transaction_t(),
            mode = Sim_CPU_Mode_User, instr_origin = Normal_Load_Store,
            ea_origin = 0, wimg = 0, alignment = 0, inhibit_exception = 0,
            external_pid = 0, decoration = ppc_decoration_t())
    '''


class preference_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        preference_interface_t(get_preference_for_module_key=None,
            set_preference_for_module_key=None)
    '''


class probe_array_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        probe_array_interface_t(num_indices=None, value=None,
            all_values=None, properties=None)
    '''


class probe_index_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        probe_index_interface_t(num_indices=None, value=None,
            properties=None)
    '''


class probe_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        probe_interface_t(value=None, properties=None)
    '''


class probe_result_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class probe_sampler_cache_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        probe_sampler_cache_interface_t(enable=None, disable=None,
            get_generation=None)
    '''


class probe_subscribe_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        probe_subscribe_interface_t(subscribe=None, unsubscribe=None,
            num_subscribers=None)
    '''


class processor_cli_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        processor_cli_interface_t(get_disassembly=None, get_pregs=None,
            get_diff_regs=None, get_pending_exception_string=None,
            get_address_prefix=None, translate_to_physical=None)
    '''


class processor_endian_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        processor_endian_interface_t(get_instruction_endian=None,
            get_data_endian=None)
    '''


class processor_gui_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        processor_gui_interface_t(dummy=None)
    '''


class processor_info_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        processor_info_interface_t(disassemble=None,
            set_program_counter=None, get_program_counter=None,
            logical_to_physical=None, enable_processor=None,
            disable_processor=None, get_enabled=None, get_endian=None,
            get_physical_memory=None, get_logical_address_width=None,
            get_physical_address_width=None, architecture=None)
    '''


class processor_info_v2_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        processor_info_v2_interface_t(disassemble=None,
            set_program_counter=None, get_program_counter=None,
            logical_to_physical=None, get_processor_mode=None,
            enable_processor=None, disable_processor=None, get_enabled=None,
            get_endian=None, get_physical_memory=None,
            get_logical_address_width=None, get_physical_address_width=None,
            architecture=None)
    '''


class profile_area_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class pulse_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        pulse_interface_t(pulse=None)
    '''


class ram_access_subscribe_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        ram_access_subscribe_interface_t(remove_callback=None,
            enable_callback=None, disable_callback=None,
            register_access_before_cb=None, register_access_after_cb=None,
            register_access_filter_cb=None)
    '''


class ram_cb_handle_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class ram_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        ram_interface_t(fill=None, read=None, write=None, touch=None,
            size=None)
    '''


class recorded_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        recorded_interface_t(input=None)
    '''


class recorder_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        recorder_interface_t(attach=None, detach=None, input=None,
            playback=None)
    '''


class recorder_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class recorder_v2_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        recorder_v2_interface_t(record=None, playback=None)
    '''


class register_breakpoint_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        register_breakpoint_interface_t(add_breakpoint=None,
            remove_breakpoint=None, get_breakpoints=None)
    '''


class register_view_catalog_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        register_view_catalog_interface_t(register_names=None,
            register_offsets=None)
    '''


class register_view_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        register_view_interface_t(description=None, big_endian_bitorder=None,
            number_of_registers=None, register_info=None,
            get_register_value=None, set_register_value=None)
    '''


class register_view_read_only_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        register_view_read_only_interface_t(is_read_only=None)
    '''


class riscv_clic_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        riscv_clic_interface_t(acknowledge_interrupt=None)
    '''


class riscv_clic_interrupt_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        riscv_clic_interrupt_interface_t(set_active_interrupt=None,
            clear_interrupt=None)
    '''


class riscv_coprocessor_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        riscv_coprocessor_interface_t(read_register=None,
            write_register=None)
    '''


class riscv_custom_csr_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        riscv_custom_csr_interface_t(register_csr=None, register_reset=None)
    '''


class riscv_imsic_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        riscv_imsic_interface_t(num_guest_files=None, read_irq_file=None,
            read_and_write_irq_file=None, read_xtopei=None,
            read_and_write_xtopei=None)
    '''


class riscv_instruction_action_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        riscv_instruction_action_interface_t(read_x_register=None,
            write_x_register=None, name_x_register=None, read_csr=None,
            write_csr=None, read_memory=None, write_memory=None,
            load_memory_buf=None, store_memory_buf=None,
            get_current_cpu_mode=None, raise_exception=None)
    '''


class riscv_io_error_ret_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class riscv_signal_sgeip_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        riscv_signal_sgeip_interface_t(signal_raise=None, signal_lower=None)
    '''


class rs232_console_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        rs232_console_interface_t(set_baudrate=None, set_data_bits=None,
            set_stop_bits=None, set_parity_mode=None, set_dtr=None,
            set_rts=None, set_break=None)
    '''


class rs232_device_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        rs232_device_interface_t(cts=None, dsr=None, ring=None, carrier=None,
            got_break=None, got_frame_error=None)
    '''


class sata_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        sata_interface_t(receive_fis=None)
    '''


class scalar_time_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        scalar_time_interface_t(add_consumer=None, remove_consumer=None,
            poll=None)
    '''


class scale_factor_listener_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        scale_factor_listener_interface_t(set=None)
    '''


class screenshot_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        screenshot_interface_t(save_png=None, save_bmp=None)
    '''


class serial_console_frontend_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        serial_console_frontend_interface_t(write=None)
    '''


class serial_device_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        serial_device_interface_t(write=None, receive_ready=None)
    '''


class serial_peripheral_interface_master_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        serial_peripheral_interface_master_interface_t(spi_response=None)
    '''


class serial_peripheral_interface_slave_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        serial_peripheral_interface_slave_interface_t(spi_request=None,
            connect_master=None, disconnect_master=None)
    '''


class sh_interrupt_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        sh_interrupt_interface_t(change_pending=None)
    '''


class signal_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        signal_interface_t(signal_raise=None, signal_lower=None)
    '''


class simple_dispatcher_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        simple_dispatcher_interface_t(subscribe=None, unsubscribe=None)
    '''


class simple_interrupt_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        simple_interrupt_interface_t(interrupt=None, interrupt_clear=None)
    '''


class simulator_cache_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        simulator_cache_interface_t(flush=None)
    '''


class slave_agent_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        slave_agent_interface_t(accept=None, accept_async=None)
    '''


class slave_time_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class slaver_message_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        slaver_message_interface_t(send=None, send_async=None)
    '''


class smm_instrumentation_subscribe_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        smm_instrumentation_subscribe_interface_t(register_smm_enter_before_cb=None,
            register_smm_enter_after_cb=None, register_smm_leave_before_cb=None,
            register_smm_leave_after_cb=None)
    '''


class smm_reg_state_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class snoop_memory_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        snoop_memory_interface_t(operate=None)
    '''


class sparc_v8_ecc_fault_injection_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        sparc_v8_ecc_fault_injection_interface_t(inject_instr_access_exception=None,
            inject_data_access_exception=None, inject_reg_access_error=None)
    '''


class sparc_v8_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        sparc_v8_interface_t(read_window_register=None,
            write_window_register=None, power_down=None)
    '''


class spr_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        spr_interface_t(register_user_handlers=None,
            unregister_user_handlers=None, set_target_value=None,
            stash_value=None, fetch_value=None, default_getter=None,
            default_setter=None, get_name=None, get_number=None)
    '''


class stall_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        stall_interface_t(get_stall_cycles=None, set_stall_cycles=None,
            get_total_stall_cycles=None)
    '''


class stc_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        stc_interface_t(flush_STC_inv=None, flush_DSTC_logical_all=None,
            flush_DSTC_from_page=None, flush_DSTC_physical=None, dump_dstc=None,
            check_dstc=None, flush_ISTC=None, flush_ISTC_logical=None,
            flush_ISTC_physical=None, dump_turbo_page_mappings=None,
            get_physical_breakpoint_block_size=None)
    '''


class step_cycle_ratio_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        step_cycle_ratio_interface_t(get_ratio=None, set_ratio=None)
    '''


class step_cycle_ratio_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class step_event_instrumentation_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        step_event_instrumentation_interface_t(register_step_event_cb=None,
            remove_step_event_cb=None)
    '''


class step_handle_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class step_info_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        step_info_interface_t(get_halt_steps=None, set_halt_steps=None,
            get_ffwd_steps=None, set_ffwd_steps=None, get_ma_steps=None,
            set_ma_steps=None)
    '''


class step_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        step_interface_t(get_step_count=None, post_step=None,
            cancel_step=None, find_next_step=None, events=None, advance=None)
    '''


class symbol_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class symbols_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        symbols_interface_t(load_symbols=None, symbol_files=None,
            symbol_file_info=None, symbols_at=None, source_at=None,
            source_lines=None)
    '''


class sync_initial_time_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        sync_initial_time_interface_t(get_initial_global_time=None)
    '''


class synchronous_mode_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        synchronous_mode_interface_t(enter=None, leave=None)
    '''


class table_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        table_interface_t(data=None, properties=None)
    '''


class tagged_linear_address_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        tagged_linear_address_t(valid=False, addr=0)
    '''


class tagged_physical_address_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class tcf_channel_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        tcf_channel_interface_t(start_channel=None)
    '''


class tcp_connection_info_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        tcp_connection_info_interface_t(remote_port=None, remote_ip=None)
    '''


class telemetry_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        telemetry_interface_t(get_telemetry_class_id=None,
            get_telemetry_class_name=None, get_telemetry_class_description=None,
            get_telemetry_id=None, get_telemetry_name=None,
            get_telemetry_description=None, get_value=None)
    '''


class telemetry_manager_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        telemetry_manager_interface_t(add_data=None)
    '''


class telnet_connection_v2_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        telnet_connection_v2_interface_t(listening=None, connected=None,
            disconnect=None)
    '''


class temporal_state_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        temporal_state_interface_t(save=None, merge=None,
            prepare_restore=None, finish_restore=None)
    '''


class terminal_client_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        terminal_client_interface_t(write=None, disconnect=None)
    '''


class terminal_server_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        terminal_server_interface_t(write=None, set_size=None,
            disconnect=None)
    '''


class terminus_cb_data_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class text_console_backend_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        text_console_backend_interface_t(input=None, request_refresh=None,
            set_visible=None, line_length=None, line_wrap=None, set_size=None,
            set_default_colours=None)
    '''


class text_console_frontend_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        text_console_frontend_interface_t(start=None, stop=None,
            set_title=None, set_size=None, set_max_scrollback_size=None,
            set_default_colours=None, set_visible=None)
    '''


class thread_domain_group_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class thread_domain_holder_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class thread_domain_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class timing_model_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        timing_model_interface_t(operate=None)
    '''


class transaction_cb_handle_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class transaction_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        transaction_interface_t(issue=None)
    '''


class transaction_subscribe_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        transaction_subscribe_interface_t(remove_callback=None,
            enable_callback=None, disable_callback=None, register_issue_cb=None)
    '''


class transaction_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        transaction_t(prev = None)
    '''


class transaction_target_type_ret_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        transaction_target_type_ret_t(val=Sim_Transaction_Target_Unset)
    '''


class transaction_trace_atom_access_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        transaction_trace_atom_access_t(callback=None, cb_data=None)
    '''


class transaction_translator_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        transaction_translator_interface_t(translate=None)
    '''


class translate_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        translate_interface_t(translate=None)
    '''


class translation_flush_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        translation_flush_interface_t(flush_range=None)
    '''


class translation_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        translation_t(target=None, base=0, start=0, size=0, flags=0)
    '''


class translator_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        translator_interface_t(translate=None)
    '''


class uint64_state_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        uint64_state_interface_t(set=None)
    '''


class usb_device_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        usb_device_interface_t(submit_transfer=None, abort_transfer=None,
            reset=None)
    '''


class usb_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        usb_interface_t(connect_device=None, disconnect_device=None,
            complete_transfer=None)
    '''


class usb_transfer_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class vectored_interrupt_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        vectored_interrupt_interface_t(set_level=None)
    '''


class vectored_interrupt_source_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        vectored_interrupt_source_interface_t(acknowledge=None)
    '''


class vga_text_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        vga_text_interface_t(add_string_notification=None)
    '''


class vga_text_update_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        vga_text_update_interface_t(write=None)
    '''


class vga_update_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        vga_update_interface_t(refresh=None, refresh_all=None)
    '''


class virtual_data_breakpoint_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        virtual_data_breakpoint_interface_t(add_read=None, add_write=None,
            remove=None)
    '''


class virtual_instruction_breakpoint_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        virtual_instruction_breakpoint_interface_t(add=None, remove=None)
    '''


class vmp_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        vmp_interface_t(class_has_support=None, host_support=None,
            compatible_config=None, enable=None, disable=None, enabled=None,
            set_threshold=None, get_feature=None, set_feature=None,
            get_info=None)
    '''


class vmx_instrumentation_subscribe_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        vmx_instrumentation_subscribe_interface_t(register_vmx_mode_leave_cb=None,
            register_vmx_mode_enter_cb=None)
    '''


class vnc_server_v2_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        vnc_server_v2_interface_t(listening=None, num_clients=None,
            disconnect=None)
    '''


class wait_handler_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class winsome_console_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        winsome_console_interface_t(gfx=None, resize=None, refresh=None,
            append=None, text=None, activity=None)
    '''


class x86_access_type_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        x86_access_type_interface_t(get_enum_name=None, get_short_name=None,
            get_description=None, implicit=None)
    '''


class x86_address_query_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        x86_address_query_interface_t(segment=None,
            get_page_crossing_info=None)
    '''


class x86_cache_flush_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        x86_cache_flush_interface_t(flush=None)
    '''


class x86_cpuid_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        x86_cpuid_interface_t(cpuid=None)
    '''


class x86_cpuid_query_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        x86_cpuid_query_interface_t(cpuid_query=None)
    '''


class x86_cstate_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        x86_cstate_interface_t(get_cstate=None, set_cstate=None)
    '''


class x86_cstate_notification_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        x86_cstate_notification_interface_t(notification=None)
    '''


class x86_cstate_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        x86_cstate_t(state=0, sub_state=0)
    '''


class x86_ept_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        x86_ept_interface_t(guest_physical_to_physical=None,
            is_ept_active=None)
    '''


class x86_exception_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        x86_exception_interface_t(DE_fault=None, BR_fault=None,
            UD_fault=None, NM_fault=None, DF_abort=None, TS_fault=None,
            NP_fault=None, SS_fault=None, GP_fault=None, PF_fault=None,
            MF_fault=None, AC_fault=None, XM_fault=None)
    '''


class x86_exception_query_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        x86_exception_query_interface_t(vector=None, source=None,
            error_code=None)
    '''


class x86_fpu_env_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        x86_fpu_env_t(cw=0, sw=0, tag=0, opc=0, last_instr_ptr=0,
            last_operand_ptr=0, last_instr_sel=0, last_operand_sel=0, pad=0)
    '''


class x86_fpu_reg_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        x86_fpu_reg_t(low=0, high=0)
    '''


class x86_instruction_query_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        x86_instruction_query_interface_t(linear_address=None)
    '''


class x86_instrumentation_subscribe_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        x86_instrumentation_subscribe_interface_t(register_mode_switch_cb=None,
            register_illegal_instruction_cb=None)
    '''


class x86_instrumentation_subscribe_v2_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        x86_instrumentation_subscribe_v2_interface_t(register_mode_enter_cb=None,
            register_mode_leave_cb=None, register_illegal_instruction_cb=None)
    '''


class x86_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        x86_interface_t(set_pin_status=None, start_up=None, interrupt=None,
            uninterrupt=None, has_pending_interrupt=None,
            has_waiting_interrupt=None, logical_to_linear=None,
            linear_to_physical=None, enter_acpi_c2_state=None)
    '''


class x86_memory_access_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        x86_memory_access_interface_t(read_logical=None, write_logical=None)
    '''


class x86_memory_query_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        x86_memory_query_interface_t(linear_address=None, segment=None,
            access_type=None, memory_type=None)
    '''


class x86_memory_transaction_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        x86_memory_transaction_t(s = generic_transaction_t(),
            linear_address = 0, guest_physical_address = 0, segnum = 0,
            access_linear = 0, io = 0, fault_as_if_write = 0,
            guest_phys_valid = 0, mode = Sim_CPU_Mode_User,
            access_type = X86_Other, pat_type = X86_None, mtrr_type = X86_None,
            effective_type = X86_None, sequence_number = 0)
    '''


class x86_monitor_info_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        x86_monitor_info_t(armed=False, address=0, extensions=0, hints=0)
    '''


class x86_msr_getter_ret_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        x86_msr_getter_ret_t(status=0, value=0)
    '''


class x86_msr_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        x86_msr_interface_t(register_handlers=None, unregister_handlers=None,
            get=None, set=None, get_name=None, get_number=None,
            get_all_valid_numbers=None)
    '''


class x86_mwait_info_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        x86_mwait_info_t(extensions=0, hints=0)
    '''


class x86_pending_debug_exc_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        x86_pending_debug_exc_t(pending=False, pending_dr6=0)
    '''


class x86_pkg_cstate_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        x86_pkg_cstate_interface_t(get_pkg_cstate=None, set_pkg_cstate=None,
            pkg_cstate_update=None)
    '''


class x86_reg_access_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        x86_reg_access_interface_t(get_gpr=None, set_gpr=None, get_rip=None,
            set_rip=None, get_rflags=None, set_rflags=None,
            set_status_flags=None, get_seg=None, set_seg=None,
            get_system_seg=None, set_system_seg=None, get_cr=None, set_cr=None,
            get_efer=None, set_efer=None, get_xcr=None, set_xcr=None,
            get_freg=None, set_freg=None, get_fpu_env=None, set_fpu_env=None,
            get_xmm=None, set_xmm=None, get_ymm=None, set_ymm=None,
            get_mxcsr=None, set_mxcsr=None, get_dr=None, set_dr=None,
            get_in_smm=None, set_in_smm=None, get_smm_base=None,
            set_smm_base=None, get_monitor_info=None, set_monitor_info=None,
            get_mwait_info=None, set_mwait_info=None, get_activity_state=None,
            set_activity_state=None, get_interruptibility_state=None,
            set_interruptibility_state=None, get_pending_debug_exc=None,
            set_pending_debug_exc=None, get_xmode_info=None, get_exec_mode=None,
            get_mxcsr_mask=None, get_ext_state_dirty_bits=None,
            or_ext_state_dirty_bits=None, and_ext_state_dirty_bits=None,
            get_pdpte=None, set_pdpte=None, get_vmx_mode=None)
    '''


class x86_seg_reg_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        x86_seg_reg_t(valid=False, sel=0, base=0, attr=0, limit=0)
    '''


class x86_smm_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        x86_smm_interface_t(save_state=None, restore_state=None)
    '''


class x86_smm_state_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        x86_smm_state_interface_t(get_state=None, set_state=None,
            smram_read=None, smram_write=None)
    '''


class x86_system_seg_reg_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        x86_system_seg_reg_t(base=0, limit=0)
    '''


class x86_tlb_attrs_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class x86_tlb_attrs_v3_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        x86_tlb_attrs_v3_t(pte_attrs=0, pat_type=X86_None,
            mtrr_type=X86_None, page_size_k=0)
    '''


class x86_tlb_entry_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class x86_tlb_entry_v3_t:
    '''
    Python representation of C data structure.
    Cannot be instantiated from Python.
    '''


class x86_tlb_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        x86_tlb_interface_t(flush_all=None, flush_page=None, lookup=None,
            add=None, itlb_lookup=None, set_pae_mode=None)
    '''


class x86_tlb_v2_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        x86_tlb_v2_interface_t(flush_all=None, flush_page=None, lookup=None,
            add=None, itlb_lookup=None)
    '''


class x86_tlb_v3_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        x86_tlb_v3_interface_t(add=None, lookup=None, itlb_lookup=None,
            invalidate_page=None, invalidate=None)
    '''


class x86_vmp_control_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        x86_vmp_control_interface_t(get_block_count=None,
            set_block_count=None)
    '''


class x86_xmode_info_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        x86_xmode_info_t(efer_lma=False, cs_l=False, cs_d=False, ss_b=False)
    '''


class xed_access_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        xed_access_interface_t(get_last=None, decode=None, to_string=None)
    '''


class xmm_reg_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        xmm_reg_t(lo64=0, hi64=0)
    '''


class xtensa_export_state_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        xtensa_export_state_interface_t(register_export_state=None)
    '''


class xtensa_import_wire_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        xtensa_import_wire_interface_t(register_import_wire=None)
    '''


class xtensa_input_queue_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        xtensa_input_queue_interface_t(register_input_queue=None)
    '''


class xtensa_internal_memories_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        xtensa_internal_memories_interface_t(iram_mappings=None,
            irom_mappings=None, dram_mappings=None, drom_mappings=None)
    '''


class xtensa_lookup_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        xtensa_lookup_interface_t(register_lookup=None)
    '''


class xtensa_mpu_lookup_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        xtensa_mpu_lookup_interface_t(mpu_region=None)
    '''


class xtensa_mpu_lookup_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        xtensa_mpu_lookup_t(background=False, region=0, start=0, end=0,
            sr=False, sw=False, sx=False, ur=False, uw=False, ux=False)
    '''


class xtensa_output_queue_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        xtensa_output_queue_interface_t(register_output_queue=None)
    '''


class xtensa_wwdt_config_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        xtensa_wwdt_config_interface_t(has_wwdt=None, reset_val=None,
            hb_reset_val=None, ikey=None, bkey=None, rkey=None, kkey=None,
            ekey=None, c1key=None, c2key=None, t1key=None, t2key=None)
    '''


class xtensa_wwdt_faultinfo_interface_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        xtensa_wwdt_faultinfo_interface_t(status=None)
    '''


class ymm_reg_t:
    '''
    Python representation of C data structure.
    Constructor synopsis:
        ymm_reg_t(llo64=0, lhi64=0, hlo64=0, hhi64=0)
    '''


