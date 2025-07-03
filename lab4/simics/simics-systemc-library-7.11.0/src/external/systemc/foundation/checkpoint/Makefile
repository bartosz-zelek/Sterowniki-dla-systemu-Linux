BOOST_INC_DIR:=
BOOST_LIB_DIR:=
KERNEL_DIR:=
KERNEL_LIB:=

AR:=ar
CXX:=g++
MAKE:=make
PDFLATEX:=pdflatex
PYTHON:=python

DOC_DIR:=$(CURDIR)/doc
LIB_DIR:=$(CURDIR)/lib
OBJ_DIR:=$(CURDIR)/obj

ifeq ($(BOOST_INC_DIR),)
  $(error Please set BOOST_INC_DIR to the Boost include directory)
endif
ifeq ($(BOOST_LIB_DIR),)
  $(error Please set BOOST_LIB_DIR to the Boost library directory)
endif

ifeq ($(KERNEL_DIR),)
  $(error Please set KERNEL_DIR to the SystemC Kernel source directory)
endif
ifeq ($(KERNEL_LIB),)
  $(error Please set KERNEL_LIB to the SystemC Kernel library)
endif

CHECKPOINT_LIB:=$(LIB_DIR)/libsystemc-checkpoint.a
MANUAL:=$(DOC_DIR)/systemc-checkpoint.pdf
SERIALIZATION_LIB:=$(LIB_DIR)/libcci_serialization.a

ACCELLERA_LT_DIR:=$(CURDIR)/accellera-lt-chkpt
CHECKPOINT_DIR:=$(CURDIR)/systemc-checkpoint
MANUAL_DEVICE_DIR:=$(CURDIR)/manual-device
MANUAL_DIR:=$(CURDIR)/manual
SERIALIZATION_DIR:=$(CURDIR)/serialization

.PHONY: all accellera-lt manual manual-device serialization systemc-checkpoint
all: manual manual-device serialization systemc-checkpoint
manual: $(MANUAL)
serialization: $(SERIALIZATION_LIB)
systemc-checkpoint: $(CHECKPOINT_LIB)

accellera-lt: $(CHECKPOINT_LIB)
	$(MAKE)  -C $(ACCELLERA_LT_DIR)

$(CHECKPOINT_LIB): $(SERIALIZATION_LIB) | $(OBJ_DIR) $(LIB_DIR)
	$(MAKE) -C $(CHECKPOINT_DIR)

$(MANUAL): | $(DOC_DIR) $(OBJ_DIR)
	$(MAKE) -C $(MANUAL_DIR)

manual-device: $(CHECKPOINT_LIB)
	$(MAKE) -C $(MANUAL_DEVICE_DIR)

$(SERIALIZATION_LIB): | $(OBJ_DIR) $(LIB_DIR)
	$(MAKE) -C $(SERIALIZATION_DIR)

$(DOC_DIR) $(LIB_DIR) $(OBJ_DIR):
	mkdir -p $@

export AR
export BOOST_INC_DIR
export BOOST_LIB_DIR
export CHECKPOINT_DIR
export CHECKPOINT_LIB
export CXX
export DOC_DIR
export KERNEL_DIR
export KERNEL_LIB
export KERNEL_VERSION_2_3_1
export LIB_DIR
export MANUAL_DEVICE_DIR
export OBJ_DIR
export PDFLATEX
export PYTHON
export SERIALIZATION_DIR
export SERIALIZATION_LIB

clean-accellera-lt:
	$(MAKE) -C $(ACCELLERA_LT_DIR) clean
clean-manual:
	$(MAKE) -C $(MANUAL_DIR) clean
clean-manual-device:
	$(MAKE) -C $(MANUAL_DEVICE_DIR) clean
clean-serialization:
	$(MAKE) -C $(SERIALIZATION_DIR) clean
clean-systemc-checkpoint:
	$(MAKE) -C $(CHECKPOINT_DIR) clean

.PHONY: clean clean-accellera-lt clean-manual clean-manual-device \
        clean-serialization clean-systemc-checkpoint
clean: clean-manual clean-manual-device clean-serialization \
       clean-systemc-checkpoint
