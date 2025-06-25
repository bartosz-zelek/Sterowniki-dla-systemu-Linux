.PHONY: all clean run-accellera-lt
all: $(OBJ_DIR)/accellera-lt

CXXFLAGS += \
  -I$(BOOST_INC_DIR) \
  -I$(CHECKPOINT_DIR) \
  -I$(CURDIR) \
  -I$(KERNEL_DIR)/examples/tlm \
  -I$(KERNEL_DIR)/examples/tlm/common/include \
  -I$(KERNEL_DIR)/examples/tlm/common/src \
  -I$(KERNEL_DIR)/examples/tlm/lt/include \
  -I$(KERNEL_DIR)/examples/tlm/lt/src \
  -I$(KERNEL_DIR)/src \
  -I$(SERIALIZATION_DIR) \
  -DSC_INCLUDE_DYNAMIC_PROCESSES \
  -Wno-unused-function \
  -Wno-unused-local-typedefs \
  -Wno-maybe-uninitialized \
  -Wno-undef \
  -Wno-strict-overflow \
  -fPIC

# INTC_CHKPT enables the patched version of the accellera-lt device
CXXFLAGS += -DINTC_CHKPT

LDFLAGS += \
  -L$(BOOST_LIB_DIR) \
  -l$(CHECKPOINT_LIB) \
  -l$(KERNEL_LIB) \
  -l$(SERIALIZATION_LIB) \
  -lboost_filesystem \
  -lboost_system \
  -lboost_iostreams \
  -lpthread

SRCS:= \
  at_target_1_phase.cpp \
  initiator_top.cpp \
  lt_initiator.cpp \
  lt_target.cpp \
  lt_top.cpp \
  memory.cpp \
  report.cpp \
  sc-main.cpp \
  serializer_at_target_1_phase.cpp \
  serializer_initiator_top.cpp \
  serializer_lt_initiator.cpp \
  serializer_lt_target.cpp \
  serializer_traffic_generator.cpp \
  traffic_generator.cpp

VPATH = $(KERNEL_DIR)/examples/tlm/common/src $(KERNEL_DIR)/examples/tlm/lt/src
$(OBJ_DIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

OBJS:=$(addprefix $(OBJ_DIR)/,$(SRCS:cpp=o))
$(OBJ_DIR)/accellera-lt: $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

clean:
	rm -rf $(OBJS) $(OBJ_DIR)/accellera-lt
