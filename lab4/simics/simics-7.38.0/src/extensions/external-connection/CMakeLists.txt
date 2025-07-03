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

# Host specific configuration
if(WIN32)
  set(HOST_SRC named-pipe.c)
  set(HOST_CLS named-pipe-server)
else()
  set(HOST_SRC unix-socket.c)
  set(HOST_CLS unix-socket-server)
endif()

simics_add_module(external-connection
  CLASSES tcp-server ${HOST_CLS}
  SOURCES init.c tcp.c module_load.py ${HOST_SRC}
  INIT_LOCAL
  SIMICS_API latest
)

if(WIN32)
  # TODO(ah): setup these things from root config instead
  set(LIBWIN32 -lgdi32 -lcomdlg32 -lnetapi32 -lole32 -lrpcrt4)
  set(LIBSOCKET -lws2_32)
  target_compile_definitions(external-connection
    PRIVATE _WIN32_WINNT=0x0603 WIN32_LEAN_AND_MEAN)
  target_link_libraries(external-connection
    PRIVATE ${LIBWIN32} ${LIBSOCKET} -lpthread)
endif()
