SERIALIZATION_LIB:=$(LIB_DIR)/libcci_serialization.a

INC_DIR:=$(CURDIR)
SRC_DIR:=$(CURDIR)/libs/serialization/src
UNT_DIR:=$(CURDIR)/libs/serialization/test
CXXFLAGS += \
  -DEXPECTED_BOOST_VERSION=106501 \
  -isystem $(BOOST_INC_DIR) \
  -I$(INC_DIR) \
  -O2 \
  -Wall \
  -Werror \
  -Wformat-nonliteral \
  -Wno-address \
  -Wno-deprecated \
  -Wno-undef \
  -Wno-unused-local-typedefs \
  -Wpointer-arith \
  -Wwrite-strings \
  -g \
  -mtune=core2
ifneq ($(WINDOWS),yes)  # -fPIC is implicit with MinGW
CXXFLAGS += -fPIC
endif

ifeq ($(SKIP_TESTS),)
  UNITTEST:=unittest
endif
.PHONY: all clean unittest clean-unittest
all: $(SERIALIZATION_LIB) $(UNITTEST)

clean: clean-unittest
	rm -f $(OBJS) $(DEPS) $(SERIALIZATION_LIB)

# NOTE: wide-char support has been intentionally left out
SRCS:= \
  archive_exception.cpp \
  assert_boost_version.cpp \
  basic_archive.cpp \
  basic_iarchive.cpp \
  basic_iserializer.cpp \
  basic_oarchive.cpp \
  basic_oserializer.cpp \
  basic_pointer_iserializer.cpp \
  basic_pointer_oserializer.cpp \
  basic_serializer_map.cpp \
  basic_text_iprimitive.cpp \
  basic_text_oprimitive.cpp \
  basic_xml_archive.cpp \
  binary_iarchive.cpp \
  binary_oarchive.cpp \
  codecvt_null.cpp \
  extended_type_info.cpp \
  extended_type_info_no_rtti.cpp \
  extended_type_info_typeid.cpp \
  polymorphic_iarchive.cpp \
  polymorphic_oarchive.cpp \
  singleton.cpp \
  stl_port.cpp \
  text_iarchive.cpp \
  text_oarchive.cpp \
  utf8_codecvt_facet.cpp \
  void_cast.cpp \
  xml_archive_exception.cpp \
  xml_grammar.cpp \
  xml_iarchive.cpp \
  xml_oarchive.cpp

OBJS:=$(addprefix $(OBJ_DIR)/,$(SRCS:cpp=o))
DEPS:=$(addprefix $(OBJ_DIR)/,$(SRCS:cpp=d))

$(OBJ_DIR) $(LIB_DIR):
	mkdir -p $@

$(OBJ_DIR)/%.d: | $(OBJ_DIR) ;
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(OBJ_DIR)/%.d | $(OBJ_DIR)
	$(info Compiling $@)
	$(CXX) -MT $@ -MD -MF $(OBJ_DIR)/$*.d $(CXXFLAGS) -c -o $@ $<

-include $(DEPS)

$(SERIALIZATION_LIB): $(OBJS) | $(LIB_DIR)
	$(info Linking $@)
	$(AR) rcs $@ $^

export BOOST_INC_DIR
export BOOST_LIB_DIR
export CXX
export CXXFLAGS
export INC_DIR
export LDFLAGS
export OBJ_DIR
export SERIALIZATION_LIB
export UNT_DIR

# Because of an apperent limitation in GNU Make 3.80 that causes
# order-only prerequisites to be rebuilt despite already existing, we
# circumvent the underlying Makefile should no changes have been made
# to the library. When GNU Make is updated, this should turn into a
# .PHONY target so that GNU Make itself may decide if any unit tests
# should be rebuilt.
unittest: $(SERIALIZATION_LIB)
	$(MAKE) -f $(CURDIR)/libs/serialization/test/Makefile

clean-unittest:
	$(MAKE) -f $(CURDIR)/libs/serialization/test/Makefile clean
