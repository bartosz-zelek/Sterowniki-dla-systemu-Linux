/*
  © 2010 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef __KDF_INTERFACE_H__
#define __KDF_INTERFACE_H__

#include <simics/base/types.h>
#include <simics/base/conf-object.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* Errors */
typedef enum {
    Kdf_Error_None = 0,
    Kdf_Error_Unknown,
    Kdf_Error_Hash_Alg
} kdf_error_t;

/* Hash methods */
typedef enum {
    Kdf_Hash_Alg_Sha160 = 0,
    Kdf_Hash_Alg_Sha256,
    Kdf_Hash_Alg_Sha384,
    Kdf_Hash_Alg_Sha512
} kdf_hash_alg_t;

typedef struct {
    kdf_error_t (*kdf_counter_be32_hmac)(conf_object_t  *obj,
                                         kdf_hash_alg_t  hash_alg,
                                         bytes_t         key,
                                         const char     *label,
                                         bytes_t         context,
                                         size_t          size_in_bits,
                                         buffer_t        key_stream,
                                         uint32         *counter_in_out);
} kdf_interface_t;
#define KDF_INTERFACE "kdf"

#if defined(__cplusplus)
}
#endif

#endif // __KDF_INTERFACE_H__
