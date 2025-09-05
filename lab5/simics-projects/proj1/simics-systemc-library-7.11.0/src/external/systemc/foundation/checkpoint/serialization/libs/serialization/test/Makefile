# This test suite compile and run most unit tests for Boost Serialization.
# Because we don't produce the wide char library, no tests for
# the text_warchive or xml_warchive archives are run. Neither are
# related encoding tests.
# Since we always link statically, we don't run any dll or symbol
# visibility tests.

FOR_EACH:= \
  test_array \
  test_binary \
  test_bitset \
  test_boost_array \
  test_class_info_load \
  test_class_info_save \
  test_complex \
  test_contained_class \
  test_cyclic_ptrs \
  test_delete_pointer \
  test_deque \
  test_derived \
  test_derived_class \
  test_derived_class_ptr \
  test_diamond \
  test_diamond_complex \
  test_exported \
  test_forward_list \
  test_forward_list_ptrs \
  test_helper_support \
  test_interrupts \
  test_list \
  test_list_ptrs \
  test_map \
  test_map_boost_unordered \
  test_map_hashed \
  test_map_unordered \
  test_mi \
  test_multiple_inheritance \
  test_multiple_ptrs \
  test_native_array \
  test_new_operator \
  test_no_rtti \
  test_non_default_ctor \
  test_non_default_ctor2 \
  test_non_intrusive \
  test_null_ptr \
  test_nvp \
  test_object \
  test_optional \
  test_primitive \
  test_priority_queue \
  test_private_base2 \
  test_queue \
  test_recursion \
  test_registered \
  test_set \
  test_set_boost_unordered \
  test_set_hashed \
  test_set_unordered \
  test_shared_ptr \
  test_shared_ptr_132 \
  test_shared_ptr_multi_base \
  test_simple_class \
  test_simple_class_ptr \
  test_slist \
  test_slist_ptrs \
  test_split \
  test_stack \
  test_tracking \
  test_unique_ptr \
  test_unregistered \
  test_valarray \
  test_variant \
  test_vector \
  test_z

POLYMORPHIC:= \
  $(OBJ_DIR)/polymorphic_text_archive.dir/test_polymorphic \
  $(OBJ_DIR)/polymorphic_text_archive.dir/test_polymorphic2 \
  $(OBJ_DIR)/polymorphic_text_archive.dir/test_polymorphic_helper

REGULAR:= \
  $(OBJ_DIR)/test_private_ctor \
  $(OBJ_DIR)/test_reset_object_address \
  $(OBJ_DIR)/test_void_cast \
  $(OBJ_DIR)/test_mult_archive_types \
  $(OBJ_DIR)/test_iterators \
  $(OBJ_DIR)/test_iterators_base64 \
  $(OBJ_DIR)/test_inclusion \
  $(OBJ_DIR)/test_smart_cast

COMPILE:= \
  $(OBJ_DIR)/test_const_pass.o \
  $(OBJ_DIR)/test_strong_typedef.o \
  $(OBJ_DIR)/test_traits_pass.o

COMPILE_WITH_WARNINGS:= \
  $(OBJ_DIR)/compile-with-warnings.dir/test_static_warning.o \
  $(OBJ_DIR)/compile-with-warnings.dir/test_const_save_warn1.o \
  $(OBJ_DIR)/compile-with-warnings.dir/test_const_save_warn2.o \
  $(OBJ_DIR)/compile-with-warnings.dir/test_const_save_warn3.o

COMPILE_WITH_ERRORS:= \
  $(OBJ_DIR)/compile-with-errors.dir/test_not_serializable.o \
  $(OBJ_DIR)/compile-with-errors.dir/test_traits_fail.o \
  $(OBJ_DIR)/compile-with-errors.dir/test_const_load_fail1.o \
  $(OBJ_DIR)/compile-with-errors.dir/test_const_load_fail2.o \
  $(OBJ_DIR)/compile-with-errors.dir/test_const_load_fail3.o \
  $(OBJ_DIR)/compile-with-errors.dir/test_const_load_fail1_nvp.o \
  $(OBJ_DIR)/compile-with-errors.dir/test_const_load_fail2_nvp.o \
  $(OBJ_DIR)/compile-with-errors.dir/test_const_load_fail3_nvp.o \
  $(OBJ_DIR)/compile-with-errors.dir/test_check.o

# We don't produce the wide char library, so no need to run its tests
# here (text_warchive, xml_warchive).
ARCHIVES:=text_archive \
          binary_archive \
          xml_archive
TESTS:=$(foreach ARCHIVE,$(ARCHIVES),$(addprefix $(OBJ_DIR)/,$(addprefix $(ARCHIVE).dir/,$(FOR_EACH))))
TESTS += $(POLYMORPHIC)
TESTS += $(REGULAR)
TESTS += $(COMPILE)
TESTS += $(COMPILE_WITH_WARNINGS)
TESTS += $(COMPILE_WITH_ERRORS)

.PHONY: all clean
all: $(TESTS)
$(TESTS): $(SERIALIZATION_LIB)

DEPENDENCIES:= \
  $(OBJ_DIR)/A.o \
  $(OBJ_DIR)/polymorphic_base.o \
  $(OBJ_DIR)/polymorphic_derived1.o \
  $(OBJ_DIR)/test_polymorphic2imp.o \
  $(OBJ_DIR)/test_polymorphic_A.o
clean:
	rm -rf $(TESTS) $(DIRECTORIES) $(DEPENDENCIES)

TEST_DEFINES:= \
  -DBOOST_ALL_NO_LIB=1 \
  -DBOOST_LIB_DIAGNOSTIC=1

CXXFLAGS += \
  -I$(SRC_DIR) \
  -I$(UNT_DIR) \
  -Wno-deprecated \
  -Wno-unused-variable \
  $(TEST_DEFINES)

LDFLAGS += \
  $(SERIALIZATION_LIB) \
  -L$(BOOST_LIB_DIR) \
  -lboost_filesystem \
  -lboost_system

ifeq ($(LIBRARY_PATHS),)
  LIBRARY_PATHS:=$(BOOST_LIB_DIR)
else
  LIBRARY_PATHS:=$(LIBRARY_PATHS):$(BOOST_LIB_DIR)
endif
ifneq ($(LD_LIBRARY_PATH),)
  LIBRARY_PATHS:=$(LD_LIBRARY_PATH):$(LIBRARY_PATHS)
endif
SET_LIBRARY_PATH=LD_LIBRARY_PATH=$(LIBRARY_PATHS)

# GNU Make recognize the directories as intermediate files and
# attempts to delete them. Prevent this.
DIRECTORIES:= \
  $(addprefix $(OBJ_DIR)/,$(addsuffix .dir,$(ARCHIVES))) \
  $(OBJ_DIR)/polymorphic_text_archive.dir \
  $(OBJ_DIR)/compile-with-warnings.dir \
  $(OBJ_DIR)/compile-with-errors.dir
.SECONDARY: $(DIRECTORIES)

$(OBJ_DIR)/%.dir:
	$(info Creating $@)
	mkdir -p $@

$(OBJ_DIR)/%.o: $(UNT_DIR)/%.cpp
	$(info Compiling $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Unlike recent versions of GNU Make, in 3.80 the $^ variable denotes
# all prerequisites, including order-only prerequisites.
OO_WORKAROUND = $(filter-out $(DIRECTORIES),$1)

define ARCHIVE_PATTERN_RULE
$1/$2.dir/test_%: $(UNT_DIR)/test_%.cpp | $1/$2.dir
	$$(info Compiling $$@)
	$(CXX) -o $$@ $$(call OO_WORKAROUND,$$^) $$(CXXFLAGS) $(LDFLAGS)
	$$(info Running $$@)
	cd $1/$2.dir && $(SET_LIBRARY_PATH) $$@ > /dev/null

$1/$2.dir/test_%: CXXFLAGS += -DBOOST_ARCHIVE_TEST=$2.hpp

# Pattern targets may not have extra prerequisites appended at a later
# time, so they must be specified explicitly:
$1/$2.dir/test_class_info_load: | $1/$2.dir/test_class_info_save

$1/$2.dir/test_array: $1/A.o
$1/$2.dir/test_boost_array: $1/A.o
$1/$2.dir/test_contained_class: $1/A.o
$1/$2.dir/test_cyclic_ptrs: $1/A.o
$1/$2.dir/test_deque: $1/A.o
$1/$2.dir/test_derived_class: $1/A.o
$1/$2.dir/test_derived_class_ptr: $1/A.o
$1/$2.dir/test_exported: $1/polymorphic_base.o
$1/$2.dir/test_forward_list: $1/A.o
$1/$2.dir/test_forward_list_ptrs: $1/A.o
$1/$2.dir/test_list: $1/A.o
$1/$2.dir/test_list_ptrs: $1/A.o
$1/$2.dir/test_map: $1/A.o
$1/$2.dir/test_map_boost_unordered: $1/A.o
$1/$2.dir/test_map_hashed: $1/A.o
$1/$2.dir/test_map_unordered: $1/A.o
$1/$2.dir/test_multiple_ptrs: $1/A.o
$1/$2.dir/test_native_array: $1/A.o
$1/$2.dir/test_new_operator: $1/A.o
$1/$2.dir/test_no_rtti: $1/polymorphic_base.o $1/polymorphic_derived1.o
$1/$2.dir/test_nvp: $1/A.o
$1/$2.dir/test_priority_queue: $1/A.o
$1/$2.dir/test_queue: $1/A.o
$1/$2.dir/test_recursion: $1/A.o
$1/$2.dir/test_set: $1/A.o
$1/$2.dir/test_set_boost_unordered: $1/A.o
$1/$2.dir/test_set_hashed: $1/A.o
$1/$2.dir/test_set_unordered: $1/A.o
$1/$2.dir/test_simple_class: $1/A.o
$1/$2.dir/test_simple_class_ptr: $1/A.o
$1/$2.dir/test_slist: $1/A.o
$1/$2.dir/test_slist_ptrs: $1/A.o
$1/$2.dir/test_stack: $1/A.o
$1/$2.dir/test_variant: $1/A.o
$1/$2.dir/test_vector: $1/A.o

# This test triggers a static warning as part of the serialization of
# non-tracked pointers. This is expected; suppress the warning.
$1/$2.dir/test_tracking: CXXFLAGS += -Wno-sign-compare
endef

$(foreach ARCHIVE,$(ARCHIVES),$(eval $(call ARCHIVE_PATTERN_RULE,$(OBJ_DIR),$(ARCHIVE))))

$(eval $(call ARCHIVE_PATTERN_RULE,$(OBJ_DIR),polymorphic_text_archive))

$(OBJ_DIR)/polymorphic_text_archive.dir/test_polymorphic: $(OBJ_DIR)/test_polymorphic_A.o $(OBJ_DIR)/A.o
$(OBJ_DIR)/polymorphic_text_archive.dir/test_polymorphic2: $(OBJ_DIR)/test_polymorphic2imp.o

$(OBJ_DIR)/test_%: $(UNT_DIR)/test_%.cpp
	$(info Compiling $@)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)
	$(info Running $@)
	cd $(OBJ_DIR) && $(SET_LIBRARY_PATH) $@ > /dev/null

$(OBJ_DIR)/test_reset_object_address: $(OBJ_DIR)/A.o

# Units with expected warnings are compiled using a reduced set of
# flags which is lenient on warnings but doesn't suppress them.
LENIENT_CXXFLAGS:=-Wall \
                  -I$(INC_DIR) \
                  -I$(UNT_DIR) \
                  -I$(BOOST_INC_DIR) \
                  $(TEST_DEFINES)

$(OBJ_DIR)/compile-with-warnings.dir/%.o: $(UNT_DIR)/%.cpp | $(OBJ_DIR)/compile-with-warnings.dir
	$(info Compiling $@)
	$(CXX) $(LENIENT_CXXFLAGS) -c -o $@ $< &> /dev/null

# Unlike extra prerequisites for patterns, GNU Make supports
# pattern-specific variables
$(OBJ_DIR)/compile-with-warnings.dir/%: FLAGS:=$(LENIENT_CXXFLAGS) $(LDFLAGS)
$(OBJ_DIR)/compile-with-warnings.dir/%: $(UNT_DIR)/%.cpp | $(OBJ_DIR)/compile-with-warnings.dir
	$(info Compiling $@)
	$(CXX) -o $@ $(call OO_WORKAROUND,$^) $(FLAGS) &> /dev/null

$(OBJ_DIR)/compile-with-errors.dir/%.o: $(UNT_DIR)/%.cpp | $(OBJ_DIR)/compile-with-errors.dir
	$(info Compiling $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $< &> /dev/null && exit 1 || touch $@
