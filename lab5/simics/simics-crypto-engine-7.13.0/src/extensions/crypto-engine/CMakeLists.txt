# © 2021 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

# skip-cmake-conversion

if(SIMICS_REPO_ROOT)

include(openssl.cmake)

simics_add_module(crypto-engine
    CLASSES crypto_engine_3des
        crypto_engine_aes
        crypto_engine_crc
        crypto_engine_des
        crypto_engine_dh
        crypto_engine_dsa
        crypto_engine_ecdsa
        crypto_engine_kasumi
        crypto_engine_kdf
        crypto_engine_lms
        crypto_engine_md5
        crypto_engine_pka
        crypto_engine_rc4
        crypto_engine_rsa
        crypto_engine_sha160
        crypto_engine_sha224
        crypto_engine_sha256
        crypto_engine_sha384
        crypto_engine_sha3_224
        crypto_engine_sha3_256
        crypto_engine_sha3_384
        crypto_engine_sha3_512
        crypto_engine_sha512
        crypto_engine_sha512_224
        crypto_engine_sha512_256
        crypto_engine_snow
        crypto_engine_sum
        crypto_engine_xor
        crypto_engine_zuc
        crypto_engine_openssl
        crypto_engine_vnc_des
    SOURCES crypto-engine-3des.dml
        crypto-engine-aes.dml
        crypto-engine-crc.dml
        crypto-engine-des.dml
        crypto-engine-dh.dml
        crypto-engine-dsa.dml
        crypto-engine-ecdsa.dml
        crypto-engine-kasumi.dml
        crypto-engine-kdf.dml
        crypto-engine-lms.dml
        crypto-engine-md5.dml
        crypto-engine-pka.dml
        crypto-engine-rc4.dml
        crypto-engine-rsa.dml
        crypto-engine-sha160.dml
        crypto-engine-sha224.dml
        crypto-engine-sha256.dml
        crypto-engine-sha384.dml
        crypto-engine-sha3_224.dml
        crypto-engine-sha3_256.dml
        crypto-engine-sha3_384.dml
        crypto-engine-sha3_512.dml
        crypto-engine-sha512.dml
        crypto-engine-sha512_224.dml
        crypto-engine-sha512_256.dml
        crypto-engine-snow.dml
        crypto-engine-sum.dml
        crypto-engine-xor.dml
        crypto-engine-zuc.dml
        crypto-engine-openssl.dml
        crypto-engine-vnc_des.dml
        crypto-engine.c
        module_load.py
    PYWRAP crypto-interface.h
        kdf-interface.h
        pka-interface.h
        crypto-engine.h
    INTERFACES
        dsa_v2
        evp_aead
        evp_cipher
        evp_hash
        evp_hmac
        f8_ex
        f9_ex
        gcm_ext_cipher
        rsa_ex
        rsa_v2
        vnc_des
        xts_ext_cipher
        zuc
        ecdsa_v2
    SIMICS_API latest
    INIT_LOCAL
)

# Copy libs to sys/lib
if(WIN32)
  # Use locally deployed build-deps ; or require user to provide the path
  set(LIBCRYPTO_FILES libcrypto-3-x64.dll legacy.dll)
  set(LIBCRYPTO_IN_PYTHON_PKG ${SIMICS_PYTHON_DIR}/${SIMICS_HOST_TYPE}/bin)
else()
  set(LIBCRYPTO_FILES libcrypto.so.3 legacy.so)
  set(LIBCRYPTO_IN_PYTHON_PKG ${SIMICS_PYTHON_DIR}/${SIMICS_HOST_TYPE}/sys/lib)
endif()

set(LIBCRYPTO_TARGETS)
foreach(FILE ${LIBCRYPTO_FILES})
  set(DEST ${SIMICS_PROJECT_DIR}/${SIMICS_HOST_TYPE}/sys/lib/${FILE})
  simics_copy_file(${LIBCRYPTO_IN_PYTHON_PKG}/${FILE} TO ${DEST})
  list(APPEND LIBCRYPTO_TARGETS ${DEST})
endforeach()

add_custom_target(copy-libcrypto DEPENDS ${LIBCRYPTO_TARGETS})

add_dependencies(crypto-engine copy-libcrypto)

target_link_libraries(crypto-engine
    PRIVATE crypto-engine::backends dml-lib
)

simics_add_external_build_dependency(crypto-engine OpenSSL)
# Old code with new lib requires compatibility mode to squash warnings
add_compile_definitions("OPENSSL_API_COMPAT=0x10101000")

add_subdirectory(backends)

else()

add_library(crypto-engine-includes INTERFACE)
target_include_directories(crypto-engine-includes INTERFACE .)
add_library(crypto-engine::includes ALIAS crypto-engine-includes)
add_library(crypto-engine::imports ALIAS crypto-engine-includes)

endif()
