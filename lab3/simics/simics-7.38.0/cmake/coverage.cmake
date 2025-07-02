# © 2023 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

# LCOV coverage convenience target if LCOV is present
find_program(LCOV lcov)
if(LCOV)
  # See cmake/README.md for more details on how to run Coverage
  get_filename_component(LCOV_PATH ${LCOV} DIRECTORY)
  find_program(GENHTML genhtml HINTS ${LCOV_PATH})
  find_program(GCOV gcov)
  set(LCOV_BASE lcov-base.info)
  set(LCOV_TEST lcov-test.info)
  set(LCOV_TOTAL lcov.info)
  set(LCOV_LOG lcov.log)
  set(LCOV_ERR lcov.err)
  add_custom_target(init-coverage
    COMMENT "Collecting initial coverage"
    COMMAND ${LCOV} -c -i -d ${CMAKE_CURRENT_BINARY_DIR} --gcov-tool ${GCOV}
            -o ${LCOV_BASE} 2>${LCOV_ERR} >${LCOV_LOG})
  add_dependencies(init-coverage reset-coverage)

  add_custom_target(reset-coverage
    COMMENT "Reset all coverage counters to zero"
    COMMAND ${LCOV} -q -z -d ${CMAKE_CURRENT_BINARY_DIR} --gcov-tool ${GCOV}
            -o ${LCOV_BASE}
    COMMAND ${LCOV} -q -z -d ${CMAKE_CURRENT_BINARY_DIR} --gcov-tool ${GCOV}
            -o ${LCOV_TEST}
    COMMAND ${LCOV} -q -z -d ${CMAKE_CURRENT_BINARY_DIR} --gcov-tool ${GCOV}
            -o ${LCOV_TOTAL})

  add_custom_target(capture-coverage
    COMMENT "Capture coverage data"
    DEPENDS ${LCOV_BASE}  # add to bail early if LCOV_BASE is not created yet
    COMMAND ${LCOV} -c -d ${CMAKE_CURRENT_BINARY_DIR} -o ${LCOV_TEST}
            --gcov-tool ${GCOV} 2>${LCOV_ERR} >${LCOV_LOG}
    COMMAND ${LCOV} -a ${LCOV_BASE} -a ${LCOV_TEST} -o ${LCOV_TOTAL}
            >>${LCOV_LOG})

  add_custom_target(filter-coverage
    COMMENT "Filter coverage data; excluding build-deps and generated files"
    # Ignore Simics build-deps
    COMMAND ${LCOV} -r ${LCOV_TOTAL} -o ${LCOV_TOTAL} "'*conan_build_deps*'"
            >>${LCOV_LOG}
    # Ignore system headers
    COMMAND ${LCOV} -r ${LCOV_TOTAL} -o ${LCOV_TOTAL} "'/usr/include*'"
            >>${LCOV_LOG}
    # Ignore generated files
    COMMAND ${LCOV} -r ${LCOV_TOTAL} -o ${LCOV_TOTAL}
            "'${CMAKE_CURRENT_BINARY_DIR}/*'" >>${LCOV_LOG})
  add_dependencies(filter-coverage capture-coverage)

  add_custom_target(generate-coverage-report-internal
    # This target is for Simics-internal use
    COMMENT "Generate coverage using LCOV/GCOV; with Simics Base libs"
    COMMAND ${GENHTML} ${LCOV_TOTAL} --legend --prefix ${SIMICS_PROJECT_DIR}
            -o coverage >>${LCOV_LOG})
  add_dependencies(generate-coverage-report-internal filter-coverage)

  add_custom_target(generate-coverage-report
    COMMENT "Generate coverage using LCOV/GCOV"
    # Ignore everything coming from Simics Base
    COMMAND ${LCOV} -r ${LCOV_TOTAL} -o ${LCOV_TOTAL}
            "'${SIMICS_BASE_DIR}/*'" >>${LCOV_LOG}
    # Ignore build artifacts
    COMMAND ${LCOV} -r ${LCOV_TOTAL} -o ${LCOV_TOTAL}
            "'${SIMICS_PROJECT_DIR}/linux64/*'" >>${LCOV_LOG}
    # Doubtful that anyone will run this on win64; but exclude is fast
    COMMAND ${LCOV} -r ${LCOV_TOTAL} -o ${LCOV_TOTAL}
            "'${SIMICS_PROJECT_DIR}/win64/*'" >>${LCOV_LOG}
    COMMAND ${GENHTML} ${LCOV_TOTAL} --legend --prefix
            ${SIMICS_PROJECT_DIR} -o coverage >>${LCOV_LOG})
  add_dependencies(generate-coverage-report filter-coverage)

endif()
