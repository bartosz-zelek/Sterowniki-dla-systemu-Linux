# © 2010 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


import cli

for cha in ['des', '3des', 'aes', 'rc4', 'kasumi', 'snow', 'zuc', 'pka',
            'md5', 'sha160', 'sha224', 'sha256', 'sha384', 'sha512',
            'sha3_256', 'sha3_512', 'sha3_224', 'sha3_384',
            'sha512_256', 'sha512_224', 'lms',
            'crc', 'sum', 'xor', 'rsa', 'kdf', 'dh', 'dsa', 'ecdsa']:
    cli.new_info_command('crypto_engine_' + cha, lambda obj: [])
    cli.new_status_command('crypto_engine_' + cha, lambda obj: [])
