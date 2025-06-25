ifneq ($(LD_LIBRARY_PATH),)
  LIBRARY_PATHS:=$(LD_LIBRARY_PATH):$(LIBRARY_PATHS)
endif
SET_LIBRARY_PATH=LD_LIBRARY_PATH=$(LIBRARY_PATHS)

LDFLAGS += \
  -ldl \
  -lm \
  -lrt \
  -pthread \
  -lstdc++fs

.PHONY: all clean
all: $(OBJ_DIR)/unittest

OBJS:= \
  $(OBJ_DIR)/scratch_directory.o \
  $(OBJ_DIR)/test_all_sc_objects.o \
  $(OBJ_DIR)/test_array.o \
  $(OBJ_DIR)/test_bitset.o \
  $(OBJ_DIR)/test_deque.o \
  $(OBJ_DIR)/test_export.o \
  $(OBJ_DIR)/test_hierarchical_name.o \
  $(OBJ_DIR)/test_iarchive.o \
  $(OBJ_DIR)/test_in_memory_state.o \
  $(OBJ_DIR)/test_json_ptree_parser.o \
  $(OBJ_DIR)/test_kernel.o \
  $(OBJ_DIR)/test_list.o \
  $(OBJ_DIR)/test_map.o \
  $(OBJ_DIR)/test_module.o \
  $(OBJ_DIR)/test_module_order.o \
  $(OBJ_DIR)/test_multiset.o \
  $(OBJ_DIR)/test_multimap.o \
  $(OBJ_DIR)/test_oarchive.o \
  $(OBJ_DIR)/test_on_disk_systemc_state.o \
  $(OBJ_DIR)/test_order.o \
  $(OBJ_DIR)/test_pair.o \
  $(OBJ_DIR)/test_payload_update.o \
  $(OBJ_DIR)/test_payload.o \
  $(OBJ_DIR)/test_peq_with_get.o \
  $(OBJ_DIR)/test_priority_queue.o \
  $(OBJ_DIR)/test_queue.o \
  $(OBJ_DIR)/test_sc_bigint.o \
  $(OBJ_DIR)/test_sc_biguint.o \
  $(OBJ_DIR)/test_sc_buffer.o \
  $(OBJ_DIR)/test_sc_bv.o \
  $(OBJ_DIR)/test_sc_clock.o \
  $(OBJ_DIR)/test_sc_event_queue.o \
  $(OBJ_DIR)/test_sc_fifo.o \
  $(OBJ_DIR)/test_sc_fix.o \
  $(OBJ_DIR)/test_sc_fix_fast.o \
  $(OBJ_DIR)/test_sc_fixed.o \
  $(OBJ_DIR)/test_sc_fixed_fast.o \
  $(OBJ_DIR)/test_sc_fxval.o \
  $(OBJ_DIR)/test_sc_fxval_fast.o \
  $(OBJ_DIR)/test_sc_int.o \
  $(OBJ_DIR)/test_sc_logic.o \
  $(OBJ_DIR)/test_sc_lv.o \
  $(OBJ_DIR)/test_sc_mutex.o \
  $(OBJ_DIR)/test_sc_ppq.o \
  $(OBJ_DIR)/test_sc_semaphore.o \
  $(OBJ_DIR)/test_sc_signal.o \
  $(OBJ_DIR)/test_sc_signal_resolved.o \
  $(OBJ_DIR)/test_sc_signal_rv.o \
  $(OBJ_DIR)/test_sc_signed.o \
  $(OBJ_DIR)/test_sc_time.o \
  $(OBJ_DIR)/test_sc_ufix.o \
  $(OBJ_DIR)/test_sc_ufix_fast.o \
  $(OBJ_DIR)/test_sc_ufixed.o \
  $(OBJ_DIR)/test_sc_ufixed_fast.o \
  $(OBJ_DIR)/test_sc_uint.o \
  $(OBJ_DIR)/test_sc_unsigned.o \
  $(OBJ_DIR)/test_serializable_meta_data.o \
  $(OBJ_DIR)/test_serialize_callback.o \
  $(OBJ_DIR)/test_serializer.o \
  $(OBJ_DIR)/test_set.o \
  $(OBJ_DIR)/test_stack.o \
  $(OBJ_DIR)/test_state_keeper.o \
  $(OBJ_DIR)/test_state_keeper_checkpoints.o \
  $(OBJ_DIR)/test_state_tree.o \
  $(OBJ_DIR)/test_subdirectories.o \
  $(OBJ_DIR)/test_tlm_quantumkeeper.o \
  $(OBJ_DIR)/test_tracking.o \
  $(OBJ_DIR)/test_valarray.o \
  $(OBJ_DIR)/test_vector.o \
  $(OBJ_DIR)/main.o

$(OBJ_DIR)/%.o: $(SRC_DIR)/unittest/%.cc
	@echo "Compiling $@"
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/unittest: $(OBJS) $(CHECKPOINT_LIB) $(SERIALIZATION_LIB) $(KERNEL_LIB)
	@echo "Linking $@"
	$(CXX) -o $@ $^ $(LDFLAGS) $(TEST_LIBS)
	@echo "Running $@"
	cd $(OBJ_DIR) && $(SET_LIBRARY_PATH) $@

clean:
	rm -f $(OBJ_DIR)/unittest $(OBJS)
