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

#ifndef __CRYPTO_INTERFACE_H__
#define __CRYPTO_INTERFACE_H__

#include <simics/base/types.h>
#include <simics/base/conf-object.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/*
 * Context usage (general for all interfaces):
 * 1. Call context_size()
 * 2. Allocate a buffer_t of at least the size from context_size()
 * 3. Call init() to initialize the context (ctx).
 * 4. Call update() until all data has been processed.
 * 5. Call finalize(), do not reuse the context after this.
 *
 * The context should always be checkpointable, and it's up to the low-level
 * implementation to make sure that the context does not contain any pointers
 * etc. to ensure checkpointability.
 *
 * The user should not make any assumptions of the format of the context, this
 * to ensure that a backend can be replaced.
 */

/*
 * XMSS algorithm interface:
 */
typedef enum {
        Xmss_Cipher_Error_None = 0,
        Xmss_Cipher_Error_Verify,
} xmss_cipher_error_t;

SIM_INTERFACE(lms_cipher) {
    xmss_cipher_error_t (*lms_verify)(
        conf_object_t *obj, bytes_t public_key,
            bytes_t message,bytes_t signature);
};
#define LMS_CIPHER_INTERFACE "lms_cipher"
/*
 * Generic stream cipher interface, used for instance for:
 *   - ARC4
 */
typedef enum {
        Stream_Cipher_Error_None = 0,
        Stream_Cipher_Error_Ctx_Size,
        Stream_Cipher_Error_Key_Size,
        Stream_Cipher_Error_Unknown = -1
} stream_cipher_error_t;

typedef struct {
    size_t (*context_size)(conf_object_t *obj);

    stream_cipher_error_t (*encrypt_init)(
        conf_object_t *obj, buffer_t ctx, const bytes_t key);
    stream_cipher_error_t (*encrypt_update)(
        conf_object_t *obj, buffer_t ctx,
        const bytes_t in_data, buffer_t *out_data);
    stream_cipher_error_t (*encrypt_final)(
        conf_object_t *obj, buffer_t ctx, buffer_t *out_data);
    stream_cipher_error_t (*encrypt)(
        conf_object_t *obj, const bytes_t key,
        const bytes_t in_data, buffer_t *out_data);

    stream_cipher_error_t (*decrypt_init)(
        conf_object_t *obj, buffer_t ctx, const bytes_t key);
    stream_cipher_error_t (*decrypt_update)(
        conf_object_t *obj, buffer_t ctx,
        const bytes_t in_data, buffer_t *out_data);
    stream_cipher_error_t (*decrypt_final)(
        conf_object_t *obj, buffer_t ctx, buffer_t *out_data);
    stream_cipher_error_t (*decrypt)(
        conf_object_t *obj, const bytes_t key,
        const bytes_t in_data, buffer_t *out_data);
} stream_cipher_interface_t;
#define STREAM_CIPHER_INTERFACE "stream_cipher"

/*
 * Generic block cipher interface, used for instance for:
 * - DES-ECB/CBC/OFB/CFB
 * - AES-ECB/CBC/OFB/CFB
 */

typedef enum {
        Block_Cipher_Mode_None = 0,
        Block_Cipher_Mode_ECB,
        Block_Cipher_Mode_CBC,
        Block_Cipher_Mode_OFB,
        Block_Cipher_Mode_CFB,
} block_cipher_mode_t;

typedef enum {
        Block_Cipher_Error_None = 0,
        Block_Cipher_Error_Ctx_Size,
        Block_Cipher_Error_Mode,
        Block_Cipher_Error_Key_Size,
        Block_Cipher_Error_Data_Size,
        Block_Cipher_Error_Unknown = -1
} block_cipher_error_t;

typedef struct {
    size_t (*context_size)(conf_object_t *obj);

    /* Is this method needed? */
    int (*block_size)(conf_object_t *obj, block_cipher_mode_t mode);

    /* Encrypt init/update/final. */
    block_cipher_error_t (*encrypt_init)(
        conf_object_t *obj, buffer_t ctx, block_cipher_mode_t mode,
        bool is_fast_key, const bytes_t key, buffer_t *fast_key,
        const bytes_t iv);
    block_cipher_error_t (*encrypt_update)(
        conf_object_t *obj, buffer_t ctx,
        const bytes_t in_data, buffer_t *out_data, buffer_t *out_iv);
    block_cipher_error_t (*encrypt_final)(
        conf_object_t *obj, buffer_t ctx,
        buffer_t *out_data, buffer_t *out_iv);

    /* Encrypt one-shot */
    block_cipher_error_t (*encrypt)(
        conf_object_t *obj, block_cipher_mode_t mode,
        bool is_fast_key, const bytes_t key, buffer_t *fast_key,
        const bytes_t in_iv, const bytes_t in_data,
        buffer_t *out_iv, buffer_t *out_data);

    /* Decrypt init/update/final. */
    block_cipher_error_t (*decrypt_init)(
        conf_object_t *obj, buffer_t ctx, block_cipher_mode_t mode,
        bool is_fast_key, const bytes_t key, buffer_t *fast_key,
        const bytes_t iv);
    block_cipher_error_t (*decrypt_update)(
        conf_object_t *obj, buffer_t ctx,
        const bytes_t in_data, buffer_t *out_data, buffer_t *out_iv);
    block_cipher_error_t (*decrypt_final)(
        conf_object_t *obj, buffer_t ctx,
        buffer_t *out_data, buffer_t *out_iv);

    /* Decrypt one-shot */
    block_cipher_error_t (*decrypt)(
        conf_object_t *obj, block_cipher_mode_t mode,
        bool is_fast_key, const bytes_t key, buffer_t *fast_key,
        const bytes_t in_iv, const bytes_t in_data,
        buffer_t *out_iv, buffer_t *out_data);

    block_cipher_error_t (*decrypt_key_expanded)(
        conf_object_t *obj, block_cipher_mode_t mode, bool is_fast_key,
        const bytes_t key, buffer_t *fast_key,
        const bytes_t in_iv, const bytes_t in_data,
        buffer_t *out_iv, buffer_t *out_data);

    /* Specialized method for expanding a key implemented in AES only*/
    void (*expand_key)(conf_object_t *obj, const bytes_t *key,
                       buffer_t *expanded,bool is_big_end);

} block_cipher_interface_t;
#define BLOCK_CIPHER_INTERFACE "block_cipher"

/*
 * Counter mode interface, used for instance for:
 * - AES-CTR
 */
typedef enum {
        Ctr_Cipher_Error_None = 0,
        Ctr_Cipher_Error_Key_Size,
        Ctr_Cipher_Error_Data_Size,
        Ctr_Cipher_Error_Unknown = -1,
} ctr_cipher_error_t;

typedef struct {
    ctr_cipher_error_t (*encrypt)(
        conf_object_t *obj,
        const bytes_t key, uint64 *counter_u, uint64 *counter_l,
        uint32 mod_exp, const bytes_t in_data, buffer_t *out_data);

    ctr_cipher_error_t (*decrypt)(
        conf_object_t *obj,
        const bytes_t key, uint64 *counter_u, uint64 *counter_l,
        uint32 mod_exp, const bytes_t in_data, buffer_t *out_data);
} ctr_cipher_interface_t;
#define CTR_CIPHER_INTERFACE "ctr_cipher"

/*
 * For hard-disk encryption, used for instance for:
 * - AES-XTS
 */
typedef enum {
        Xts_Cipher_Error_None = 0,
        Xts_Cipher_Error_Key_Size,
        Xts_Cipher_Error_Unknown = -1,
} xts_cipher_error_t;

typedef struct {
    size_t (*context_size)(conf_object_t *obj, size_t sector_size);

    xts_cipher_error_t (*encrypt_init)(
        conf_object_t *obj, buffer_t ctx,
        const bytes_t key1, const bytes_t key2,
        uint64 sector_idx, size_t sector_size);
    xts_cipher_error_t (*encrypt_update)(
        conf_object_t *obj, buffer_t ctx,
        const bytes_t in_data, buffer_t *out_data);
    xts_cipher_error_t (*encrypt_final)(
        conf_object_t *obj, buffer_t ctx, buffer_t *out_data);
    xts_cipher_error_t (*encrypt)(
        conf_object_t *obj,
        const bytes_t key1, const bytes_t key2,
        uint64 sector_idx, size_t sector_size,
        const bytes_t in_data, buffer_t *out_data);

    xts_cipher_error_t (*decrypt_init)(
        conf_object_t *obj, buffer_t ctx,
        const bytes_t key1, const bytes_t key2,
        uint64 sector_idx, size_t sector_size);
    xts_cipher_error_t (*decrypt_update)(
        conf_object_t *obj, buffer_t ctx,
        const bytes_t in_data, buffer_t *out_data);
    xts_cipher_error_t (*decrypt_final)(
        conf_object_t *obj, buffer_t ctx, buffer_t *out_data);
    xts_cipher_error_t (*decrypt)(
        conf_object_t *obj,
        const bytes_t key1, const bytes_t key2,
        uint64 sector_idx, size_t sector_size,
        const bytes_t in_data, buffer_t *out_data);
} xts_cipher_interface_t;
#define XTS_CIPHER_INTERFACE "xts_cipher"

typedef struct {
    xts_cipher_error_t err;
    bytes_t data;
} xts_output_t;

SIM_INTERFACE(xts_ext_cipher) {
    size_t (*context_size)(conf_object_t *obj, size_t sector_size);
    xts_cipher_error_t (*encrypt_init)(
        conf_object_t *obj, buffer_t ctx,
        bytes_t key1, bytes_t key2,
        bytes_t iv, size_t sector_size);
    xts_output_t (*encrypt_update)(
        conf_object_t *obj, buffer_t ctx, bytes_t in_data);
    xts_output_t (*encrypt_final)(conf_object_t *obj, buffer_t ctx);
    xts_output_t (*encrypt)(
        conf_object_t *obj,
        bytes_t key1, bytes_t key2,
        bytes_t iv, size_t sector_size,
        bytes_t in_data);
    xts_cipher_error_t (*decrypt_init)(
        conf_object_t *obj, buffer_t ctx,
        bytes_t key1, bytes_t key2,
        bytes_t iv, size_t sector_size);
    xts_output_t (*decrypt_update)(
        conf_object_t *obj, buffer_t ctx, bytes_t in_data);
    xts_output_t (*decrypt_final)(conf_object_t *obj, buffer_t ctx);
    xts_output_t (*decrypt)(
        conf_object_t *obj,
        bytes_t key1, bytes_t key2,
        bytes_t iv, size_t sector_size,
        bytes_t in_data);
};
#define XTS_EXT_CIPHER_INTERFACE "xts_ext_cipher"

/*
 * CCM mode encryption interface, used by:
 * - AES-CCM
 */
typedef enum {
        Ccm_Cipher_Error_None = 0,
        Ccm_Cipher_Error_Key_Size,
        Ccm_Cipher_Error_Data_Size,
        Ccm_Cipher_Error_Tag_Size,
        Ccm_Cipher_Error_Tag_Check_Fail,
        Ccm_Cipher_Error_Unknown = -1,
} ccm_cipher_error_t;

typedef struct {
    /* Encrypt with preformatted B0/CTR blocks. MAC output is separate from
       data output. Both the unencrypted and encrypted MACs are outputted */
    ccm_cipher_error_t (*encrypt_lowlevel)(
        conf_object_t *obj,
        const bytes_t key, const bytes_t aad,
        const bytes_t B0, uint64 counter_u, uint64 counter_l,
        const bytes_t in_data, buffer_t *out_data,
        buffer_t *calculated_mac, buffer_t *encrypted_mac);

    /* Encrypt with nonce. MAC is appended to the output data */
    ccm_cipher_error_t (*encrypt)(
        conf_object_t *obj,
        const bytes_t key, const bytes_t iv,
        const bytes_t aad, int tag_len,
        const bytes_t in_data, buffer_t *out_data);

    /* Decrypt with preformatted B0/CTR blocks. MAC input must be separated from
       the input data. The input MAC will be decrypted and placed in
       decrypted_mac. The calculated MAC (unencrypted) is placed in
       calculated_mac. No ICV checking is done. You can do the ICV check by
       comparing the calculated MAC to the decrypted MAC. */
    ccm_cipher_error_t (*decrypt_lowlevel)(
        conf_object_t *obj,
        const bytes_t key, const bytes_t aad,
        const bytes_t B0, uint64 counter_u, uint64 counter_l,
        const bytes_t in_data, const bytes_t mac,
        buffer_t *out_data,
        buffer_t *calculated_mac, buffer_t *decrypted_mac);

    /* Decrypt with nonce. Input MAC is assumed to be appended to the input
       data. Note that the output data will _not_ contain the decrypted MAC.
       This function will perform an automatic ICV check and returns
       Gcm_Cipher_Error_Tag_Check_Fail if there is a mismatch. */
    ccm_cipher_error_t (*decrypt)(
        conf_object_t *obj,
        const bytes_t key, const bytes_t iv,
        const bytes_t aad, int tag_len,
        const bytes_t in_data, buffer_t *out_data);
} ccm_cipher_interface_t;
#define CCM_CIPHER_INTERFACE "ccm_cipher"

/*
 * GCM mode encryption interface, used by:
 * - AES-GCM
 */
typedef enum {
        Gcm_Cipher_Error_None = 0,
        Gcm_Cipher_Error_Key_Size,
        Gcm_Cipher_Error_Unknown = -1,
} gcm_cipher_error_t;

typedef struct {
    size_t (*context_size)(conf_object_t *obj);

    /* Encrypt init/update/final methods. Note that streaming IV is not
       supported (i.e. all IV must be provided in the encrypt_init() call).
       It is however possible to stream AAD and user data. To do this, call
       encrypt_update() with non-empty AAD and empty in_data buffers. When all
       AAD has been streamed, continue calling encrypt_update() with empty
       AAD and non-empty in_data buffers. Note that the final AAD call can also
       contain input data. */
    gcm_cipher_error_t (*encrypt_init)(
        conf_object_t *obj, buffer_t ctx,
        const bytes_t key, const bytes_t iv);
    gcm_cipher_error_t (*encrypt_update)(
        conf_object_t *obj, buffer_t ctx,
        const bytes_t aad, const bytes_t in_data,
        buffer_t *out_data);
    gcm_cipher_error_t (*encrypt_final)(
        conf_object_t *obj, buffer_t ctx,
        buffer_t *out_data, buffer_t *mac);

    /* Encrypt one-shot version */
    gcm_cipher_error_t (*encrypt)(
        conf_object_t *obj,
        const bytes_t key, const bytes_t iv,
        const bytes_t aad,
        const bytes_t in_data, buffer_t *out_data, buffer_t *mac);

    /* Decrypt init/update/final methods. See encrypt_init/update/final for
       details on streaming AAD and input data. */
    gcm_cipher_error_t (*decrypt_init)(
        conf_object_t *obj, buffer_t ctx,
        const bytes_t key, const bytes_t iv);
    gcm_cipher_error_t (*decrypt_update)(
        conf_object_t *obj, buffer_t ctx,
        const bytes_t aad, const bytes_t in_data,
        buffer_t *out_data);
    gcm_cipher_error_t (*decrypt_final)(
        conf_object_t *obj, buffer_t ctx,
        buffer_t *out_data, buffer_t *mac);

    /* Decrypt one-shot version */
    gcm_cipher_error_t (*decrypt)(
        conf_object_t *obj,
        const bytes_t key, const bytes_t iv, const bytes_t aad,
        const bytes_t in_data, buffer_t *out_data, buffer_t *mac);
} gcm_cipher_interface_t;
#define GCM_CIPHER_INTERFACE "gcm_cipher"

typedef struct {
    gcm_cipher_error_t err;
    bytes_t data;
    bytes_t tag;
} gcm_output_t;

SIM_INTERFACE(gcm_ext_cipher) {
    size_t (*context_size)(conf_object_t *obj);

    /* Encrypt init/update/final methods. Note that streaming IV is not
       supported (i.e. all IV must be provided in the encrypt_init() call).
       J0 can be provided instead of IV by setting the flag.
       It is however possible to stream AAD and user data. To do this, call
       encrypt_update() with non-empty AAD and empty in_data buffers. When all
       AAD has been streamed, continue calling encrypt_update() with empty
       AAD and non-empty in_data buffers. Note that the final AAD call can also
       contain input data. */
    gcm_cipher_error_t (*encrypt_init)(
        conf_object_t *obj, buffer_t ctx,
        bytes_t key, bytes_t iv, bool iv_is_j0);
    gcm_output_t (*encrypt_update)(
        conf_object_t *obj, buffer_t ctx,
        bytes_t aad, bytes_t in_data);
    gcm_output_t (*encrypt_final)(conf_object_t *obj, buffer_t ctx);

    /* Encrypt one-shot version */
    gcm_output_t (*encrypt)(conf_object_t *obj,
        bytes_t key, bytes_t iv, bool iv_is_j0, bytes_t aad, bytes_t in_data);

    /* Decrypt init/update/final methods. See encrypt_init/update/final for
       details on streaming AAD and input data. */
    gcm_cipher_error_t (*decrypt_init)(
        conf_object_t *obj, buffer_t ctx,
        bytes_t key, bytes_t iv, bool iv_is_j0);
    gcm_output_t (*decrypt_update)(
        conf_object_t *obj, buffer_t ctx, bytes_t aad, bytes_t in_data);
    gcm_output_t (*decrypt_final)(conf_object_t *obj, buffer_t ctx);

    /* Decrypt one-shot version */
    gcm_output_t (*decrypt)(conf_object_t *obj,
        bytes_t key, bytes_t iv, bool iv_is_j0, bytes_t aad, bytes_t in_data);
};
#define GCM_EXT_CIPHER_INTERFACE "gcm_ext_cipher"

/*
 * Telecom cipher, used for instance by:
 * - kasumi (gsm)
 * - snow-3g (3g!)
 */
typedef enum {
        F8_Error_None = 0,
        F8_Error_Key_Size,
        F8_Error_Unknown = -1,
} f8_error_t;

typedef struct {
    size_t (*context_size)(conf_object_t *obj);

    /* Init/update/final versions */
    f8_error_t (*init)(
        conf_object_t *obj, buffer_t ctx,
        bool is_fast_key, const bytes_t key, buffer_t *fast_key,
        uint32 count, uint8 bearer, uint8 direction, uint16 ca, uint16 ce);
    f8_error_t (*update)(
        conf_object_t *obj, buffer_t ctx,
        const bytes_t in_data, int bits, buffer_t *out_data);
    f8_error_t (*final)(
        conf_object_t *obj, buffer_t ctx, buffer_t *out_data);

    /* One-shot version */
    f8_error_t (*f8)(
        conf_object_t *obj,
        bool is_fast_key, const bytes_t key, buffer_t *fast_key,
        uint32 count, uint8 bearer, uint8 direction, uint16 ca, uint16 ce,
        const bytes_t in_data, int bits, buffer_t *out_data);
} f8_interface_t;
#define F8_INTERFACE "f8"

SIM_INTERFACE(f8_ex) {
     size_t (*context_size)(conf_object_t *obj);
    /* Init/update/final versions */
    f8_error_t (*init)(
        conf_object_t *obj, buffer_t ctx,
        const bytes_t key,
        uint32 count, uint8 bearer, uint8 direction, uint16 ca, uint16 ce);
    f8_error_t (*init_raw)(
        conf_object_t *obj, buffer_t ctx, const bytes_t key, const bytes_t iv);
    f8_error_t (*update)(
        conf_object_t *obj, buffer_t ctx,
        const bytes_t in_data, int bits, buffer_t out_data);
    f8_error_t (*final)(
        conf_object_t *obj, buffer_t ctx, buffer_t out_data);

    /* One-shot version */
    f8_error_t (*f8)(
        conf_object_t *obj,
        const bytes_t key,
        uint32 count, uint8 bearer, uint8 direction, uint16 ca, uint16 ce,
        const bytes_t in_data, int bits, buffer_t out_data);
    f8_error_t (*f8_raw)(
        conf_object_t *obj,
        const bytes_t key, const bytes_t iv,
        const bytes_t in_data, int bits, buffer_t out_data);
};
#define F8_EX_INTERFACE "f8_ex"

/*
 * Telecom mac, used for instance by:
 * - kasumi (gsm)
 * - snow-3g (3g!)
 */

typedef enum {
        F9_Error_None = 0,
        F9_Error_Key_Size,
        F9_Error_Unknown = -1,
} f9_error_t;

typedef struct {
    size_t (*context_size)(conf_object_t *obj);

    /* F9 init/update/final */
    f9_error_t (*init)(
        conf_object_t *obj, buffer_t ctx,
        const bytes_t key, uint32 count, uint32 fresh, uint8 direction);
    f9_error_t (*update)(
        conf_object_t *obj, buffer_t ctx, const bytes_t in_data, int bits);
    f9_error_t (*final)(
        conf_object_t *obj, buffer_t ctx, buffer_t *mac);

    /* F9 one-shot */
    f9_error_t (*f9)(
        conf_object_t *obj,
        const bytes_t key, uint32 count, uint32 fresh, uint8 direction,
        const bytes_t in_data, int bits, buffer_t *mac);
} f9_interface_t;
#define F9_INTERFACE "f9"

SIM_INTERFACE(f9_ex) {
    size_t (*context_size)(conf_object_t *obj);

    /* F9 init/update/final */
    f9_error_t (*init)(
        conf_object_t *obj, buffer_t ctx,
        const bytes_t key, uint32 count, uint32 fresh, uint8 direction);
    f9_error_t (*init_raw)(
        conf_object_t *obj, buffer_t ctx,
        const bytes_t key, const bytes_t iv);
    f9_error_t (*update)(
        conf_object_t *obj, buffer_t ctx, const bytes_t in_data, int bits);
    f9_error_t (*final)(
        conf_object_t *obj, buffer_t ctx, buffer_t mac);

    /* F9 one-shot */
    f9_error_t (*f9)(
        conf_object_t *obj,
        const bytes_t key, uint32 count, uint32 fresh, uint8 direction,
        const bytes_t in_data, int bits, buffer_t mac);
    f9_error_t (*f9_raw)(
        conf_object_t *obj,
        const bytes_t key, const bytes_t iv,
        const bytes_t in_data, int bits, buffer_t mac);
};
#define F9_EX_INTERFACE "f9_ex"

typedef enum {
        ZUC_Error_None = 0,
        ZUC_Error_Size,
        ZUC_Error_Unknown = -1,
} zuc_error_t;

typedef struct {
        size_t length;
        zuc_error_t err;
} zuc_output_t;

SIM_INTERFACE(zuc) {
    /* Required size of input context buffer_t.
       key_size should be key size in bytes. */
    size_t (*context_size)(conf_object_t *obj, int key_size);

    zuc_error_t (*enc_init)(conf_object_t *obj, buffer_t ctx,
                            bytes_t key, bytes_t iv);
    /* length is the number of bits of input to be processed.
       The returned length indicates number of bytes processed.
       output should have same size as input. */
    zuc_output_t (*enc_update)(conf_object_t *obj, buffer_t ctx,
                               bytes_t input, size_t length, buffer_t output);
    /* The returned length indicates number of bytes processed. */
    zuc_output_t (*enc_final)(conf_object_t *obj,
                              buffer_t ctx, buffer_t output);

    /* One-shot encryption */
    zuc_output_t (*encrypt)(conf_object_t *obj, bytes_t key, bytes_t iv,
                            bytes_t input, size_t length, buffer_t output);

    /* mac_size is MAC size in bytes */
    zuc_error_t (*mac_init)(conf_object_t *obj, buffer_t ctx,
                            int mac_size, bytes_t key, bytes_t iv);
    /* length is the number of bits of input to be processed. */
    zuc_error_t (*mac_update)(conf_object_t *obj, buffer_t ctx,
                              bytes_t input, size_t length);
    /* output should have the MAC size */
    zuc_error_t (*mac_final)(conf_object_t *obj, buffer_t ctx, buffer_t output);

    /* One-shot MAC */
    zuc_error_t (*mac)(conf_object_t *obj, int mac_size,
                       bytes_t key, bytes_t iv,
                       bytes_t input, size_t length, buffer_t output);
};
#define ZUC_INTERFACE "zuc"

/* 
 * Generic OpenSSL EVP_CIPHER interface
 *
 * NOTES: no implicit padding, user must pad data to cipher block size,
 *  otherwise *_final will fail
 */
SIM_INTERFACE(evp_cipher) {
    /* EVP_CIPHER_CTX_new -- create generic cipher context */
    uintptr_t (*context_new)(conf_object_t *obj);
    /* EVP_CIPHER_CTX_free -- clear and free cipher context */
    void (*context_free)(conf_object_t *obj, uintptr_t ctx);
    /* EVP_CIPHER_block_size */
    int (*block_size)(conf_object_t *obj);

    /* EVP_EncryptInit -- initialize cipher context */
    bool (*encrypt_init)(conf_object_t *obj, uintptr_t ctx, bytes_t key, bytes_t iv);
    /* EVP_DecryptInit -- initialize cipher context */
    bool (*decrypt_init)(conf_object_t *obj, uintptr_t ctx, bytes_t key, bytes_t iv);
    /* EVP_EncryptUpdate */
    int (*encrypt_update)(conf_object_t *obj, uintptr_t ctx, bytes_t in, buffer_t out);
    /* EVP_DecryptUpdate */
    int (*decrypt_update)(conf_object_t *obj, uintptr_t ctx, bytes_t in, buffer_t out);
    /* EVP_EncryptFinal */
    bool (*encrypt_final)(conf_object_t *obj, uintptr_t ctx);
    /* EVP_DectuptFinal */
    bool (*decrypt_final)(conf_object_t *obj, uintptr_t ctx);

    /* One-shot encrypt */
    int (*encrypt)(conf_object_t *obj, bytes_t key, bytes_t iv,
                   bytes_t in, buffer_t out);
    /* One-shot decrypt */
    int (*decrypt)(conf_object_t *obj, bytes_t key, bytes_t iv,
                   bytes_t in, buffer_t out);
};
#define EVP_CIPHER_INTERFACE "evp_cipher"

/*
 * OpenSSL EVP_CIPHER with AEAD controls
 *
 * NOTES:
 *  - no implicit padding, user must pad data to block size
 *  - call set_tag before decrypt_final to validate input
 */
SIM_INTERFACE(evp_aead) {
    /* EVP_CTRL_AEAD_GET_TAG */
    bool (*get_tag)(conf_object_t *obj, uintptr_t ctx, buffer_t tag);
    /* EVP_CTRL_AEAD_SET_TAG */
    bool (*set_tag)(conf_object_t *obj, uintptr_t ctx, bytes_t tag);

    /* One-shot AEAD */
    int (*encrypt)(conf_object_t *obj, bytes_t key, bytes_t iv,
                   bytes_t aad, bytes_t in, buffer_t out, buffer_t tag);
    int (*decrypt)(conf_object_t *obj, bytes_t key, bytes_t iv,
                   bytes_t aad, bytes_t in, bytes_t tag, buffer_t out);
};
#define EVP_AEAD_INTERFACE "evp_aead"

SIM_INTERFACE(evp_hash) {
    size_t (*hash_size)(conf_object_t *obj);

    uintptr_t (*hash_context_new)(conf_object_t *obj);

    void (*hash_context_free)(conf_object_t *obj, uintptr_t ctx);

    bool (*hash_init)(conf_object_t *obj, uintptr_t ctx);

    bool (*hash_update)(conf_object_t *obj, uintptr_t ctx, bytes_t input);

    bool (*hash_final)(conf_object_t *obj, uintptr_t ctx, buffer_t output);

    bool (*hash)(conf_object_t *obj, bytes_t input, buffer_t output);
};
#define EVP_HASH_INTERFACE "evp_hash"

SIM_INTERFACE(evp_hmac) {
    size_t (*mac_size)(conf_object_t *obj);

    uintptr_t (*mac_context_new)(conf_object_t *obj);

    void (*mac_context_free)(conf_object_t *obj, uintptr_t ctx);

    bool (*mac_init)(conf_object_t *obj, uintptr_t ctx, bytes_t key);

    bool (*mac_update)(conf_object_t *obj, uintptr_t ctx, bytes_t input);

    bool (*mac_final)(conf_object_t *obj, uintptr_t ctx, buffer_t output);

    bool (*mac)(conf_object_t *obj, bytes_t key,
                bytes_t input, buffer_t output);
};
#define EVP_HMAC_INTERFACE "evp_hmac"

/*
 * Generic SHA3 interface, used for instance by:
 * - SHA3-*
 */
typedef enum {
        SHA3_Error_None = 0,
        SHA3_Error_Unknown = -1,
} sha3_error_t;

typedef struct {
    size_t (*context_size)(conf_object_t *obj);

    /* SHA3 init/update/final */
    sha3_error_t (*init)(
        conf_object_t *obj, buffer_t ctx, uint8 pad_prefix, uint8 pad_postfix);
    sha3_error_t (*update)(
        conf_object_t *obj, buffer_t ctx, const bytes_t in_data);
    sha3_error_t (*final)(
        conf_object_t *obj, buffer_t ctx, buffer_t *out_data, bool pad_is_needed);

    /* SHA3 one-shot */
    sha3_error_t (*sha3)(
        conf_object_t *obj, const bytes_t in_data, buffer_t *out_data, 
        uint8 pad_prefix, uint8 pad_postfix, bool pad_is_needed);
} sha3_interface_t;
#define SHA3_INTERFACE = "sha3";

/*
 * Generic SHA3 interface, used for instance by:
 * - SHA3 HMAC-*
 */
typedef struct {
    size_t (*context_size)(conf_object_t *obj);

    /* SHA3 HMAC init/update/final */
    sha3_error_t (*init)(
        conf_object_t *obj, buffer_t ctx, const bytes_t key,
        uint8 pad_prefix, uint8 pad_postfix);
    sha3_error_t (*update)(
        conf_object_t *obj, buffer_t ctx, const bytes_t in_data);
    sha3_error_t (*final)(
        conf_object_t *obj, buffer_t ctx, buffer_t *out_data, bool pad_is_needed);

    /* SHA3 HMAC one-shot */
    sha3_error_t (*mac)(
        conf_object_t *obj, const bytes_t key,
        const bytes_t in_data, buffer_t *out_data,
        uint8 pad_prefix, uint8 pad_postfix, bool pad_is_needed);
} sha3_hmac_interface_t;
#define SHA3_HMAC_INTERFACE = "sha3_hmac";

/*
 * Generic hash interface, used for instance by:
 * - MD5
 * - SHA*
 */
typedef enum {
        Hash_Error_None = 0,
        Hash_Error_Unknown = -1,
} hash_error_t;

#define HASH_SHA1      64
#define HASH_SHA256    672
#define HASH_SHA384    673
#define HASH_SHA512    674

typedef enum {
        Hash_Sha1 = HASH_SHA1,
        Hash_Sha256 = HASH_SHA256,
        Hash_Sha384 = HASH_SHA384,
        Hash_Sha512 = HASH_SHA512,
} hash_type_t;

typedef struct {
    size_t (*context_size)(conf_object_t *obj);

    /* Hash init/update/final */
    hash_error_t (*init)(
        conf_object_t *obj, buffer_t ctx);
    hash_error_t (*update)(
        conf_object_t *obj, buffer_t ctx, const bytes_t in_data);
    hash_error_t (*final)(
        conf_object_t *obj, buffer_t ctx, buffer_t *out_data);

    /* Hash one-shot. No need to initialize out_data; out_data.data should
       be freed once it is not needed. */
    hash_error_t (*hash)(
        conf_object_t *obj, const bytes_t in_data, buffer_t *out_data);

    hash_error_t (*hash_ex)(
        conf_object_t *obj, const bytes_t snapshot, const bytes_t in_data,
        buffer_t *out_data);

    /* Alternative way of using hash_ex */
    hash_error_t (*init_ex)(
        conf_object_t *obj, buffer_t ctx, const bytes_t snapshot,
        size_t message_length);
    hash_error_t (*snapshot)(
        conf_object_t *obj, buffer_t ctx, buffer_t *out_data);

} hash_interface_t;
#define HASH_INTERFACE "hash"

/*
 * Generic mac interface, used for instance by:
 * - MD5-HMAC/SMAC
 * - SHA*-HMAC
 */
typedef enum {
        Mac_Error_None = 0,
        Mac_Error_Key_Size,
        Mac_Error_Unknown = -1,
} mac_error_t;

typedef struct {
    size_t (*context_size)(conf_object_t *obj);

    /* MAC init/update/final methods */
    mac_error_t (*init)(
        conf_object_t *obj, buffer_t ctx, const bytes_t key);
    mac_error_t (*update)(
        conf_object_t *obj, buffer_t ctx, const bytes_t in_data);
    mac_error_t (*final)(
        conf_object_t *obj, buffer_t ctx, buffer_t *out_data);

    /* MAC one-shot method */
    mac_error_t (*mac)(
        conf_object_t *obj,
        const bytes_t key, const bytes_t in_data,
        buffer_t *out_data);
} mac_interface_t;
#define MAC_INTERFACE "mac"

/*
 * Generic hmac interface (mac interface with optimized handling for
 * ipad/opad), used for instance by:
 * - MD5-HMAC
 * - SHA*-HMAC
 */
typedef enum {
        Hmac_Error_None = 0,
        Hmac_Error_Key_Size,
        Hmac_Error_Unknown = -1,
} hmac_error_t;

typedef struct {
    size_t (*context_size)(conf_object_t *obj);

    /* The ipad_opad key contains the xor:ed result of the raw key and the
       ipad/opad blocks as defined in the HMAC specification. Thus the
       ipad/opad key (key or ipad_opad, depending on generating or using)
       contains:
          xor(key,ipad)||xor(key,opad) (|| means concatenate).

       If "key_is_ipad_opad" is false; then key is the raw key and you get the
       xor:ed result in ipad_opad. If true; key contains the xor:ed value and
       ipad_opad should be ignored. */

    /* HMAC init/update/final methods */
    hmac_error_t (*init)(
        conf_object_t *obj, buffer_t ctx, const bytes_t key,
        bool key_is_ipad_opad, buffer_t *ipad_opad);
    hmac_error_t (*update)(
        conf_object_t *obj, buffer_t ctx, const bytes_t in_data);
    hmac_error_t (*final)(
        conf_object_t *obj, buffer_t ctx, buffer_t *out_data);

    /* MAC one-shot method */
    hmac_error_t (*mac)(
        conf_object_t *obj,
        const bytes_t key, bool key_is_ipad_opad, const bytes_t in_data,
        buffer_t *ipad_opad, buffer_t *out_data);
} hmac_interface_t;
#define HMAC_INTERFACE "hmac"

/*
 * Generic xcbc interface, used by AES_XCBC_MAC
*/
typedef enum {
        Xcbc_Precompute_Error_None = 0,
        Xcbc_Precompute_Error_Key_Size,
        Xcbc_Precompute_Error_E_Size,
        Xcbc_Precompute_Error_Unaligned_Data,
        Xcbc_Precompute_Error_Unknown = -1
} xcbc_precompute_error_t;

typedef struct {
    size_t (*context_size)(conf_object_t *obj);

    /* XCBC init/update/final methods */
    xcbc_precompute_error_t (*init)(
        conf_object_t *obj, buffer_t ctx, const bytes_t k1,
        const bytes_t k2, const bytes_t k3, const bytes_t e);
    xcbc_precompute_error_t (*update)(
        conf_object_t *obj, buffer_t ctx, const bytes_t in_data);
    xcbc_precompute_error_t (*final)(
        conf_object_t *obj, buffer_t ctx, buffer_t *out_data);

    /* XCBC precompute one-shot method */
    xcbc_precompute_error_t (*mac)(
        conf_object_t *obj,
        const bytes_t k1, const bytes_t k2, const bytes_t k3,
        const bytes_t e, const bytes_t in_data, buffer_t *out_data);
} xcbc_precompute_interface_t;
#define XCBC_PRECOMPUTE_INTERFACE = "xcbc_precompute";


/*
 * Generic CRC interface, used for instance by:
 * - CRC-32-IEEE 802.3
 */
typedef struct {
    uint64 (*crc)(
        conf_object_t *obj,
        uint64 poly, int width, uint64 initial_crc,
        bool reverse_initial_crc, bool reverse_input,
        bool reverse_output, bool swap_output, bool complement_output,
        bytes_t in_data);
} crc_interface_t;
#define CRC_INTERFACE "crc"

#define RSA_PKCS1_PADDING      1
#define RSA_SSLV23_PADDING     2
#define RSA_NO_PADDING         3
#define RSA_PKCS1_OAEP_PADDING 4
#define RSA_X931_PADDING       5

typedef struct {
    int32 (*generate_key)(conf_object_t *obj, uint8 *pubkey, uint32 *publen,
                          uint8 *privkey, uint32 *privlen,
                          uint8 *P, uint32 *plen,
                          uint8 *Q, uint32 *qlen);

    int32 (*private_decrypt)(conf_object_t *obj, uint8 *pubkey, uint32 publen,
                           uint8 *privkey, uint32 privlen,
                           int32 flen, uint8 *from, uint8 *to, int32 padding);

    int32 (*public_encrypt)(conf_object_t *obj, uint8 *pubkey, uint32 publen,
                          uint8 *privkey, uint32 privlen,
                          int32 flen, uint8 *from, uint8 *to, int32 padding);

    int32 (*sign)(conf_object_t *obj, uint8 *pubkey, uint32 publen,
                 uint8 *privkey, uint32 privlen, int32 md_type, uint8 *dgst,
                 uint32 dlen, uint8 *sig, uint32 *siglen);

    int32 (*verify_sig)(conf_object_t *obj, uint8 *pubkey, uint32 publen,
                 uint8 *privkey, uint32 privlen, int32 md_type, uint8 *dgst,
                 uint32 dlen, uint8 *sig, uint32 siglen);

    int32 (*mgf1)(conf_object_t *obj, uint8 *mask, int32 len,
                        const uint8 *seed, int32 seedlen);

    int32 (*add_oaep_padding)(conf_object_t *obj, uint8 *to, int32 tlen,
                        uint8 *f, int32 fl, uint8 *p, int32 pl);

    int32 (*check_oaep_padding)(conf_object_t *obj, uint8 *to, int32 tlen,
                        uint8 *f, int32 fl, int32 rsa_len, uint8 *p, int32 pl);

    int32 (*private_from_primes)(conf_object_t *obj, uint8 *pubkey,
                                 uint32 publen, uint8 *privkey, uint32 *privlen,
                                 uint8 *pprime, uint32 plen,
                                 uint8 *qprime, uint32 qlen);
    int (*generate_key_full)(conf_object_t *obj, const uint8 *P,
                             const uint8 *Q, const uint8 *E, uint8 *N,
                             uint8 *D, uint8 *dP, uint8 *dQ, uint8 *qInv,
                             uint32 bitLength);
    int (*private2_decrypt)(conf_object_t *obj, const uint8 *C, 
                            const uint8 *P, const uint8 *Q,
                            const uint8 *dP, const uint8 *dQ,
                            const uint8 *qInv, uint8 *M, uint32 bitLength);
} rsa_interface_t;
#define RSA_INTERFACE "rsa"

typedef enum {
        RSA_PKCS1_Padding = RSA_PKCS1_PADDING,
        RSA_SSLV23_Padding = RSA_SSLV23_PADDING,
        RSA_No_Padding = RSA_NO_PADDING,
        RSA_PKCS1_OAEP_Padding = RSA_PKCS1_OAEP_PADDING,
        RSA_X931_Padding = RSA_X931_PADDING,
} rsa_padding_t;

SIM_INTERFACE(rsa_v2) {
        /* We use the usual RSA notation: (n, e) is the public key with modulus
           and public exponent, d is the private key/exponent and p, q are the
           prime numbers such that n = p*q. */

        /* Generate random key of size of 'n', for public exponent 'e',
           a thin wrapper around OpenSSL RSA_generate_key_ex.
           The 'p', 'q' and 'd' buffers must have the same size as 'n'.
           The calculated numbers are stored in little-endian format,
           padded with 0.*/
        void (*generate_key)(conf_object_t *obj, uint64 e,
                             buffer_t n, buffer_t p,
                             buffer_t q, buffer_t d);

        /* Calculates private key using given public key and primes.
           The 'n', 'p' and 'q' numbers must be in little-endian format,
           such as those returned by generate_key.
           The 'd' buffer must have the same length as 'n'.
           Returns actual length of 'd'. */
        size_t (*key_from_primes)(
                conf_object_t *obj,
                bytes_t n, uint64 e, bytes_t p, bytes_t q, buffer_t d);

        /* Minimum output buffer size for encrypt/decrypt/sign. */
        size_t (*buffer_size)(conf_object_t *obj, bytes_t n, uint64 e);

        /* Encrypt using public key. Minimum output buffer size is given by
           buffer_size(n, e). Input buffer size is at most
           buffer_size(n, e) and must equal that if no padding is selected.
           n is in little-endian format.
           from and to are in big-endian format.
           Returns encrypted data length.
           See OpenSSL RSA_public_encrypt for more details. */
        size_t (*encrypt)(conf_object_t *obj,
                          bytes_t n, uint64 e,
                          bytes_t from, buffer_t to, rsa_padding_t padding);

        /* Decrypt using private key. The buffer sizes must be as defined in
           the encrypt method (with 'from'/'to' reversed).
           Returns decrypted data length.
           See OpenSSL RSA_private_decrypt for more details. */
        size_t (*decrypt)(conf_object_t *obj,
                          bytes_t n, uint64 e, bytes_t d,
                          bytes_t from, buffer_t to, rsa_padding_t padding);

        /* Calculates hash/digest of the given data.
           type is one of the Hash_ShaN values, and the output buffer
           must have size N/8. */
        void (*mgf1)(conf_object_t *obj,
                     bytes_t from, buffer_t out, hash_type_t type);

        /* Produce a digital signature of 'digest' (which typically is a
           message digest, such as the output from the mgf1 method) and store
           the result in 'sig'.
           Minimum output buffer size is given by the buffer_size method.
           'type' should be the hash type used to produce 'digest', whose
           length must be the output size of that hash.
           Returns signature data length.
           See OpenSSL RSA_sign function for more information. */
        size_t (*sign)(conf_object_t *obj,
                       bytes_t n, uint64 e, bytes_t d,
                       hash_type_t type, bytes_t digest, buffer_t sig);

        /* Verify that 'sig' is a digital signature of 'digest'.
           'type' should be the hash type used to produce 'digest', whose
           length must be the output size of that hash.
           See OpenSSL RSA_verify function for more information. */
        bool (*verify)(conf_object_t *obj,
                       bytes_t n, uint64 e,
                       hash_type_t type, bytes_t digest, bytes_t sig);

        /* Add padding to given data.
           Uses the length of 'to' to determine padding size.
           Output buffer must not be shorter than input.
           See OpenSSL RSA_padding_add_PKCS1_OAEP for more information. */
        void (*add_oaep_padding)(conf_object_t *obj,
                                 bytes_t from, bytes_t params, buffer_t to);

        /* Check padding and decodes given data (output of add_oaep_padding).
           See OpenSSL RSA_padding_check_PKCS1_OAEP for more information.
           Returns length of recovered data. */
        size_t (*check_oaep_padding)(conf_object_t *obj,
                                     bytes_t from, int rsa_len, bytes_t params,
                                     buffer_t to);

        /* Generate a private key using 孫子算經's Chinese-Remainder-Theorem (CRT)
           represented by its CRT parameters: n, d, dP, dQ and qInv.
           The 'p', 'q' and 'e' bytes must be in little-endian format.
           The calculated numbers are stored in little-endian format.
           This method may only be called when using package 1030 version 6.0.6 or higher.
          */
        void (*generate_crt_key)(conf_object_t *obj,
                                 bytes_t p, bytes_t q, bytes_t e,
                                 buffer_t n, buffer_t d, buffer_t dP,
                                 buffer_t dQ, buffer_t qInv);

        /* Generates private key (pkcs#1 v2.1 section 3.2 first form)
           using reduced lambda to satisfy requirement d < λ(n).
           Input parameters (little-endian):
                p, q - first two prime factors of the RSA modulus n
                e - RSA public exponent
           Output parameters (little-endian):
                n - RSA modulus, a positive integer
                d - RSA private exponent, a positive integer */
        void (*generate_private_key)(conf_object_t *obj, bytes_t p, bytes_t q,
                                     bytes_t e, buffer_t n, buffer_t d);

        /* Encrypt using public key and big exponent.
           Minimum output buffer size is given by buffer_size(n, e).
           Input buffer size is at most buffer_size(n, e)
           and must equal that if no padding is selected.
           n and e are in little-endian format.
           from and to are in big-endian format.
           Returns encrypted data length.
           See OpenSSL RSA_public_encrypt for more details. */
        size_t (*encrypt_ex)(conf_object_t *obj, bytes_t n, bytes_t e,
                             bytes_t from, buffer_t to, rsa_padding_t padding);

        /* Generate a private key using 孫子算經's Chinese-Remainder-Theorem (CRT) with reduced lambda
           represented by its CRT parameters: n, d, dP, dQ and qInv.
           The 'p', 'q' and 'e' bytes must be in little-endian format.
           The calculated numbers are stored in little-endian format.
          */
        void (*generate_key_form2_reduced_lambda)(conf_object_t *obj,
                                                  bytes_t p, bytes_t q, bytes_t e,
                                                  buffer_t n, buffer_t d, buffer_t dP,
                                                  buffer_t dQ, buffer_t qInv);
};
#define RSA_V2_INTERFACE "rsa_v2"

SIM_INTERFACE(rsa_ex) {
        /* Calculates key using given primes and public exponent.
           Given buffers must be allocated of relevant
           sizes, where 2 * max(p.len, q.len) is an upper bound.
           Used buffer sizes are returned. */
        bool (*generate_key)(
                conf_object_t *obj,
                bytes_t p, bytes_t q, bytes_t e, buffer_t n,
                buffer_t d, buffer_t dp, buffer_t dq, buffer_t q_inv);

        /* Output buffer size for decrypt. */
        size_t (*buffer_size)(conf_object_t *obj, bytes_t n, bytes_t e);

        /* Decrypt using private key and intermediate values
           from generate_key. Output buffer size is given by
           the buffer_size method. */
        bool (*decrypt)(
                conf_object_t *obj,
                bytes_t p, bytes_t q, bytes_t dp, bytes_t dq, bytes_t q_inv,
                bytes_t from, buffer_t to);
};
#define RSA_EX_INTERFACE "rsa_ex"

typedef struct {
    int32 (*oaep_encode)(conf_object_t *obj, uint8 *to, int32 tlen,
                         uint8 *f, int32 fl, uint8 *p, int32 pl, int32 hash);

    int32 (*oaep_decode)(conf_object_t *obj, uint8 *to, int32 tlen,
                         uint8 *f, int32 fl, int32 rsa_len, uint8 *p, int32 pl,
                         int32 hash);

    int32 (*pss_encode)(conf_object_t *obj, uint8 *pubkey, uint32 publen,
                        uint8 *digest, uint8 *emsg, int32 slen, int32 hash);

    int32 (*pss_decode)(conf_object_t *obj, uint8 *pubkey, uint32 publen,
                        uint8 *digest, uint8 *emsg, int32 slen, int32 hash);

    int32 (*ssa_encode)(conf_object_t *obj, uint32 tlen, uint8 *to, uint32 flen,
                        uint8 *from, int32 dlen, const uint8 *der);

    int32 (*ssa_decode)(conf_object_t *obj, uint32 dsize, uint8 *digest,
                        uint32 esize, uint8 *emsg, int32 dlen,
                        const uint8 *der);
} tpm_rsa_interface_t;
#define TPM_RSA_INTERFACE "tpm_rsa"

/*
 * Diffie-Hellman interface
 */
typedef enum {
        Dh_Error_None = 0,
        Dh_Error_Parameters_Size,
        Dh_Error_Unknown = -1
} dh_error_t;

typedef struct {
    dh_error_t (*dh)(conf_object_t *obj, const uint8 *base, uint32 baselen,
                     const uint8 *secret, uint32 secretlen,
                     const uint8 *modulus, uint32 moduluslen,
                     uint8 *res, uint32 *reslen);
} dh_interface_t;
#define DH_INTERFACE "dh"


typedef struct {
    int32 (*generate_g)(conf_object_t *obj, uint8 *P, uint8 *Q , uint8 *H,
                          uint8 *G, uint32 bitLength, uint32 signLength);

    int32 (*generate_y)(conf_object_t *obj, uint8 *P, uint8 *G , uint8 *X,
                          uint8 *Y, uint32 bitLength, uint32 signLength);

    int32 (*sign_rs)(conf_object_t *obj, uint8 *M, uint8 *K, uint8 *P,
                     uint8 *Q, uint8 *G, uint8 *X, uint8 *R, uint8 *S,
                     uint32 bitLength, uint32 signLength);

    int32 (*verify_sig)(conf_object_t *obj, uint8 *R, uint8 *S, uint8 *M,
                        uint8 *P, uint8 *Q, uint8 *G, uint8 *Y,
                        uint32 bitLength, uint32 signLength);
} dsa_interface_t;
#define DSA_INTERFACE "dsa"

SIM_INTERFACE(dsa_v2) {
    int32 (*generate_p)(conf_object_t *obj, bytes_t x, bytes_t q, buffer_t p);

    int32 (*generate_g)(conf_object_t *obj, bytes_t p, bytes_t q, bytes_t h,
                        buffer_t g);

    int32 (*generate_y)(conf_object_t *obj, bytes_t p, bytes_t g, bytes_t x,
                        buffer_t y);

    int32 (*sign_rs)(conf_object_t *obj, bytes_t m, bytes_t k, bytes_t p,
                        bytes_t q, bytes_t g, bytes_t x, buffer_t r,
                        buffer_t s);

    int32 (*sign_s)(conf_object_t *obj, bytes_t m, bytes_t k, bytes_t q,
                    bytes_t r, bytes_t x, buffer_t s);

    int32 (*sign_r)(conf_object_t *obj, bytes_t k, bytes_t p, bytes_t q,
                    bytes_t g, buffer_t r);

    int32 (*verify_sig)(conf_object_t *obj, bytes_t r, bytes_t s, bytes_t m,
                        bytes_t p, bytes_t q, bytes_t g, bytes_t y);
};
#define DSA_V2_INTERFACE "dsa_v2"

typedef struct {
    int32 (*mul_gfp)(conf_object_t *obj, uint8 *A, uint8 *B, uint8 *P,
                     uint8 *xG, uint8 *yG, uint8 *K,
                     uint8 *xR, uint8 *yR, int bitLength);

    int32 (*sign_r_gfp)(conf_object_t *obj, uint8 *A, uint8 *B, uint8 *P,
                        uint8 *N, uint8 *xG, uint8 *yG, uint8 *K,
                        uint8 *R, int bitLength);

    int32 (*sign_s_gfp)(conf_object_t *obj, uint8 *H, uint8 *D, uint8 *R,
                        uint8 *K, uint8 *N, uint8 *S, int bitLength);

    int32 (*sign_rs_gfp)(conf_object_t *obj, uint8 *A, uint8 *B, uint8 *P,
                         uint8 *N, uint8 *xG, uint8 *yG, uint8 *K,
                         uint8 *H, uint8 *D, uint8 *R, uint8 *S, int bitLength);

    int32 (*verify_gfp)(conf_object_t *obj, uint8 *A, uint8 *B, uint8 *P,
                        uint8 *N, uint8 *xG, uint8 *yG, uint8 *xQ, uint8 *yQ,
                        uint8 *E, uint8 *R, uint8 *S, int bitLength);
} ecdsa_interface_t;
#define ECDSA_INTERFACE "ecdsa";

SIM_INTERFACE(ecdsa_v2) {
    /* NOTE
       All in/out buffers - big numbers passing in full or shortened form as
       big endian integers. Shortened form means that leading (MSB) zero-valued
       bytes can be omitted. 
    */
    
    // gfp

    /* GFP Point Multiplication
       A - a equation coefficient
       B - b equation coefficient
       Q - modulus
       xG - x coordinate of curve point
       yG - y coordinate of curve point
       K - scalar multiplier
       xR - x coordinate of resultant EC point
       yR - y coordinate of resultant EC point
       bitLength - upper limit for input and output buffer size in bits
       return - status flag: 0 - resultant EC point coordinates are valid,
                otherwise -  resultant EC point coordinates are not valid.
    */
    int32 (*mul_gfp)(conf_object_t *obj, bytes_t A, bytes_t B, bytes_t P,
                     bytes_t xG, bytes_t yG, bytes_t K, buffer_t xR,
                     buffer_t yR, uint32 bitLength);

    /* ECDSA GFP Sign R
       A - a equation coefficient
       B - b equation coefficient
       P - modulus
       N - order of the base point of B/K-163 or B/K-233
       xG - x coordinate of base point G
       yG - y coordinate of base point G
       K - random value
       R - ECDSA signature
       bitLength - upper limit for input and output buffer size in bits
       return - status flag: 0 - signature is valid, otherwise - signature is
                not valid.
    */
    int32 (*sign_r_gfp)(conf_object_t *obj, bytes_t A, bytes_t B, bytes_t P,
                        bytes_t N, bytes_t xG, bytes_t yG, bytes_t K,
                        buffer_t R, uint32 bitLength);

    /* ECDSA GFP Sign S
       E - hash of message
       D - private key
       R - ECDSA r signature value
       K - random value
       N - order of the base point G
       S - ECDSA signature
       bitLength - upper limit for input and output buffer size in bits
       return - status flag: 0 - signature is valid, otherwise - signature is
                 not valid.
    */
    int32 (*sign_s_gfp)(conf_object_t *obj, bytes_t E, bytes_t D, bytes_t R,
                        bytes_t K, bytes_t N, buffer_t S, uint32 bitLength);

    /* ECDSA GFP Sign RS for curves B/K-163 and B/K-233
       A - a equation coefficient
       B - b equation coefficient
       P - modulus
       N - order of base point G
       xG - x coordinate of base point G
       yG - y coordinate of base point G
       K - random value
       E - digest of the message to be signed
       D - private key
       R - ECDSA signature r
       S - ECDSA signature s
       bitLength - upper limit for input and output buffer size in bits
       return - status flag: 0 - signatures are valid, otherwise - signatures
                are not valid.
    */
    int32 (*sign_rs_gfp)(conf_object_t *obj, bytes_t A, bytes_t B, bytes_t P,
                         bytes_t N, bytes_t xG, bytes_t yG, bytes_t K,
                         bytes_t E, bytes_t D, buffer_t R, buffer_t S,
                         uint32 bitLength);

    /* ECDSA GFP Verify
       A - a equation coefficient
       B - b equation coefficient
       P - modulus
       N - order of the base point G
       xG - x coordinate of base point G
       yG - y coordinate of base point G
       xQ - x coordinate of public key Q
       yQ - y coordinate of public key Q
       E - digest of the message to be signed
       R - ECDSA signature r
       S - ECDSA signature s
       bitLength - upper limit for input and output buffer size in bits
       return - status flag: 0 - verification succeeded, otherwise -
                verification failed.
    */
    int32 (*verify_gfp)(conf_object_t *obj, bytes_t A, bytes_t B, bytes_t P,
                        bytes_t N, bytes_t xG, bytes_t yG, bytes_t xQ,
                        bytes_t yQ, bytes_t E, buffer_t R, bytes_t S,
                        int bitLength);

    // gf2

    /* ECDSA Sign R for curves B/K-163 and B/K-233
       A - a equation coefficient of B/K-163 of B/K-233
       B - b equation coefficient of B/K-163 or B/K-233
       P - field polynomial of B/K-163 or B/K-233
       N - order of the base point of B/K-163 or B/K-233
       xG - x coordinate of base point G of B/K-163 of B/K-233
       yG - y coordinate of base point G of B/K-163 or B/K-233
       K - random value
       R - ECDSA signature
       bitLength - upper limit for input and output buffer size in bits
       return - status flag: 0 - signature is valid, otherwise - signature is
                not valid.
    */
    int32 (*sign_r_gf2)(conf_object_t *obj, bytes_t A, bytes_t B, bytes_t P,
                        bytes_t N, bytes_t xG, bytes_t yG, bytes_t K,
                        buffer_t R, uint32 bitLength);

    /* ECDSA Sign S for curves with n < 2^bitLength
       E - hash of message
       D - private key
       R - ECDSA r signature value
       K - random value
       N - order of the base point G
       S - ECDSA signature
       bitLength - upper limit for input and output buffer size in bits
       return - status flag: 0 - signature is valid, otherwise - signature is
                 not valid.
    */
    int32 (*sign_s_gf2)(conf_object_t *obj, bytes_t E, bytes_t D, bytes_t R,
                        bytes_t K, bytes_t N, buffer_t S, uint32 bitLength);

    /* ECDSA Sign RS for curves B/K-163 and B/K-233
       A - a equation coefficient of B/K-163 of B/K-233
       B - b equation coefficient of B/K-163 or B/K-233
       P - irreducible field polynomial of B/K-163 or B/K-233
       N - order of base point G
       xG - x coordinate of base point belonging to either B/K-163 or B/K-233
       yG - y coordinate of base point belonging to either B/K-163 or B/K-233
       K - random value
       E - integer derived from hash of message
       D - private key
       R - ECDSA signature r
       S - ECDSA signature s
       bitLength - upper limit for input and output buffer size in bits
       return - status flag: 0 - signatures are valid, otherwise - signatures
                are not valid.
    */
    int32 (*sign_rs_gf2)(conf_object_t *obj, bytes_t A, bytes_t B, bytes_t P,
                         bytes_t N, bytes_t xG, bytes_t yG, bytes_t K,
                         bytes_t E, bytes_t D, buffer_t R, buffer_t S,
                         uint32 bitLength);

    /* ECDSA Sign RS (ECC P256 and P384)
       K - random value
       E - digest of the message to be signed
       D - private key
       R - ECDSA signature r
       S - ECDSA signature s
       curve_type - 415 (NID_X9_62_prime256v1), 715 (NID_secp384r1)
       bitLength - upper limit for input and output buffer size in bits
       return - status flag: 0 - signatures are valid, otherwise - signatures
                are not valid.
    */
    int32 (*sign_rs_ecc)(conf_object_t *obj, bytes_t K, bytes_t E, bytes_t D,
                         buffer_t R, buffer_t S, int curve_type,
                         uint32 bitLength);

    /* ECDSA GF2 Verify for curves B/K-163 and B/K-233
       A - a equation coefficient of B/K-163 of B/K-233
       B - b equation coefficient of B/K-163 of B/K-233
       P -  field polynomial of B/K-163 or B/K-233
       N - order of the base point of B/K-163 or B/K-233
       xG - x coordinate of base point belonging to either B/K-163 or B/K-233
       yG - y coordinate of base point belonging to either B/K-163 or B/K-233
       xQ - x coordinate of public key Q
       yQ - y coordinate of public key Q
       E - message hash
       R - ECDSA signature r
       S - ECDSA signature s
       bitLength - upper limit for input and output buffer size in bits
       return - status flag: 0 - verification succeeded, otherwise -
                verification failed.
    */
    int32 (*verify_gf2)(conf_object_t *obj, bytes_t A, bytes_t B, bytes_t P,
                        bytes_t N, bytes_t xG, bytes_t yG, bytes_t xQ,
                        bytes_t yQ, bytes_t E, buffer_t R, bytes_t S,
                        uint32 bitLength);
};
#define ECDSA_V2_INTERFACE "ecdsa_v2"

SIM_INTERFACE(vnc_des) {
        bool (*set_key)(conf_object_t *obj, bytes_t key, bool decrypt);
        bool (*des)(conf_object_t *obj, buffer_t to, bytes_t from);
};
#define VNC_DES_INTERFACE "vnc_des"

#if defined(__cplusplus)
}
#endif

#endif  /* __CRYPTO_INTERFACE_H__ */
