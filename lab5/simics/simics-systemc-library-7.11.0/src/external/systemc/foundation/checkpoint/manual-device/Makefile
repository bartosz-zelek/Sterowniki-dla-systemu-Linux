.PHONY: all clean run-sc-main
all: run-sc-main

CXXFLAGS += \
  -I$(BOOST_INC_DIR) \
  -I$(KERNEL_DIR)/src \
  -I$(SERIALIZATION_DIR) \
  -I$(CHECKPOINT_DIR) \
  -I$(CURDIR) \
  -DSC_INCLUDE_DYNAMIC_PROCESSES \
  -Wno-unused-function \
  -Wno-unused-local-typedefs \
  -Wno-maybe-uninitialized \
  -Wno-undef \
  -Wno-strict-overflow \
  -fPIC

LDFLAGS += \
  -L$(BOOST_LIB_DIR) \
  -l$(CHECKPOINT_LIB) \
  -l$(KERNEL_LIB) \
  -l$(SERIALIZATION_LIB) \
  -lboost_filesystem \
  -lboost_system \
  -lboost_iostreams \
  -lpthread

$(OBJ_DIR)/sc-main: $(CURDIR)/sc-main.cc
	$(CXX) $(CXXFLAGS) $< $(LDFLAGS) -o $@

ifneq ($(LD_LIBRARY_PATH),)
  SET_LIBRARY_PATH=LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(BOOST_LIB_DIR)
else
  SET_LIBRARY_PATH=LD_LIBRARY_PATH=$(BOOST_LIB_DIR)
endif

run-sc-main: $(OBJ_DIR)/sc-main
	cd $(OBJ_DIR) && $(SET_LIBRARY_PATH) $(OBJ_DIR)/sc-main

clean:
	rm -rf $(OBJ_DIR)/scratch $(OBJ_DIR)/sc-main
