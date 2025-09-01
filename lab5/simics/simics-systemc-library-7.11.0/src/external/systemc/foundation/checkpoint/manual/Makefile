all: $(DOC_DIR)/systemc-checkpoint.pdf

DOC_MODULE_STENCILS:= \
  $(OBJ_DIR)/base.stencil \
  $(OBJ_DIR)/heap.stencil \
  $(OBJ_DIR)/non-module.stencil \
  $(OBJ_DIR)/payload.stencil \
  $(OBJ_DIR)/static.stencil \
  $(OBJ_DIR)/thread.stencil \
  $(OBJ_DIR)/user-module-serializer.stencil \
  $(OBJ_DIR)/user-module.stencil

$(DOC_MODULE_STENCILS): $(MANUAL_DEVICE_DIR)/doc-systemc-checkpoint.h

CHECKPOINT_STENCILS:= \
  $(OBJ_DIR)/in-memory-state-interface.stencil \
  $(OBJ_DIR)/on-disk-state-interface.stencil \
  $(OBJ_DIR)/payload-update.stencil \
  $(OBJ_DIR)/serialize-callback-interface.stencil \
  $(OBJ_DIR)/state-keeper-interface.stencil

$(OBJ_DIR)/in-memory-state-interface.stencil: \
  $(CHECKPOINT_DIR)/systemc-checkpoint/in_memory_state_interface.h
$(OBJ_DIR)/on-disk-state-interface.stencil: \
  $(CHECKPOINT_DIR)/systemc-checkpoint/on_disk_state_interface.h
$(OBJ_DIR)/payload-update.stencil: \
  $(CHECKPOINT_DIR)/systemc-checkpoint/payload_update.h
$(OBJ_DIR)/serialize-callback-interface.stencil: \
  $(CHECKPOINT_DIR)/systemc-checkpoint/serialize_callback_interface.h
$(OBJ_DIR)/state-keeper-interface.stencil: \
  $(CHECKPOINT_DIR)/systemc-checkpoint/state_keeper_interface.h

$(OBJ_DIR)/%.stencil:
	$(PYTHON) $(CURDIR)/stencil.py --in $< --out $@ --tag $*

# MP treats sequences of *** as errors, but pdflatex outputs asterisks
# liberally, so we truncate any occurrences
FOR_EACH:=1 2 3
SQUEEZE_ASTERISKS:=tr -s \*
GENERATE_PDF = $(foreach _,$(FOR_EACH),$(PDFLATEX) $< | $(SQUEEZE_ASTERISKS);)
$(OBJ_DIR)/%.pdf: $(CURDIR)/%.tex $(DOC_MODULE_STENCILS) $(CHECKPOINT_STENCILS)
	cd $(OBJ_DIR) && $(GENERATE_PDF)

$(DOC_DIR)/systemc-checkpoint.pdf: $(OBJ_DIR)/systemc-checkpoint.pdf
	cp $< $@

.PHONY: clean
clean:
	rm -f $(DOC_DIR)/systemc-checkpoint.pdf \
              $(OBJ_DIR)/*.aux \
              $(OBJ_DIR)/*.log \
              $(OBJ_DIR)/*.out \
              $(OBJ_DIR)/*.pdf \
              $(OBJ_DIR)/*.stencil \
              $(OBJ_DIR)/*.toc
