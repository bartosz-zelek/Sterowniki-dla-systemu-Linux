# © 2019 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

# Generate header file containing static assert of compiler version
$(COMPILER_VERSION_CHECK_H): generate_compiler_version_check$(EXEEXT)
	$(INVOKE)$< > $@

generate_compiler_version_check$(EXEEXT): $(kernel_make_dir)/generate_compiler_version_check.cc
ifeq ($(MSVC),yes)
	set LINK= & $(VCVARSPREFIX) $(CXX) /EHsc $< /link /SUBSYSTEM:CONSOLE
else
	$(CXX) $(EXTRA_CXXFLAGS) $< -o $@
endif
