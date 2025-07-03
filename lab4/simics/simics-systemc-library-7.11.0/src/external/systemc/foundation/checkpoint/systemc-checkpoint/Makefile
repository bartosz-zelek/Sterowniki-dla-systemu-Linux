CHECKPOINT_LIB:=$(LIB_DIR)/libsystemc-checkpoint.a
SRC_DIR:=$(CURDIR)

CXXFLAGS += \
  -I$(BOOST_INC_DIR) \
  -I$(KERNEL_DIR)/src \
  -I$(SERIALIZATION_DIR) \
  -I$(SRC_DIR) \
  -DSC_INCLUDE_DYNAMIC_PROCESSES \
  -Wno-deprecated-declarations \
  -Wno-maybe-uninitialized \
  -Wno-strict-overflow \
  -Wno-undef \
  -Wno-unused-function \
  -Wno-unused-local-typedefs \
  -Wno-unused-variable
ifneq ($(WINDOWS),yes)  # -fPIC is implicit with MinGW
  CXXFLAGS += -fPIC
endif

OBJS:= \
  $(OBJ_DIR)/hierarchical_name.o \
  $(OBJ_DIR)/iarchive.o \
  $(OBJ_DIR)/in_memory_state.o \
  $(OBJ_DIR)/istate_tree.o \
  $(OBJ_DIR)/notify_callbacks.o \
  $(OBJ_DIR)/kernel.o \
  $(OBJ_DIR)/oarchive.o \
  $(OBJ_DIR)/on_disk_state.o \
  $(OBJ_DIR)/on_disk_systemc_state.o \
  $(OBJ_DIR)/ostate_tree.o \
  $(OBJ_DIR)/payload_update_registry.o \
  $(OBJ_DIR)/serializer_registry.o \
  $(OBJ_DIR)/sim_context_proxy_checkpoint.o \
  $(OBJ_DIR)/state_keeper.o \
  $(OBJ_DIR)/state_tree.o
DEPS:=$(OBJS:o=d)

ifeq ($(SKIP_TESTS),)
  UNITTEST:=unittest
endif
.PHONY: all clean unittest clean-unittest
all: $(CHECKPOINT_LIB) $(UNITTEST)

clean: clean-unittest
	rm -f $(OBJS) $(DEPS) $(CHECKPOINT_LIB)

$(OBJ_DIR) $(LIB_DIR):
	mkdir -p $@

$(OBJ_DIR)/%.d: | $(OBJ_DIR) ;
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cc $(OBJ_DIR)/%.d | $(OBJ_DIR)
	$(info Compiling $@)
	$(CXX) -MT $@ -MD -MF $(OBJ_DIR)/$*.d $(CXXFLAGS) -c -o $@ $<

-include $(DEPS)

$(CHECKPOINT_LIB): $(OBJS) | $(LIB_DIR)
	$(info Linking $@)
	$(AR) rcs $@ $^

export BOOST_INC_DIR
export BOOST_LIB_DIR
export CHECKPOINT_LIB
export CXX
export CXXFLAGS
export KERNEL_LIB
export LDFLAGS
export LIBRARY_PATHS
export LIB_DIR
export OBJ_DIR
export SERIALIZATION_LIB
export SRC_DIR

unittest: $(CHECKPOINT_LIB)
	$(MAKE) -C $(SRC_DIR)/unittest

clean-unittest:
	$(MAKE) -C $(SRC_DIR)/unittest clean
