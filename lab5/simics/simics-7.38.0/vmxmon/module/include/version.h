/* -*- coding: utf-8 -*-

  Copyright 2015-2022 Intel Corporation

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of Intel Corporation nor the names of its contributors
      may be used to endorse or promote products derived from this software
      without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef VERSION_H
#define VERSION_H

#define VMXMON_MAJOR_VERSION            1
#define VMXMON_API_VERSION              16      /* current API version */
#define VMXMON_RELEASE                  7

#define VMXMON_COMPAT_API_VERSION       15      /* API we are compatible with */

#define VMXMON_VERSION_CODE(a, b, c)    (((a) << 24) | ((b) << 16) | ((c) << 8))
#define VMXMON_VERSION                  VMXMON_VERSION_CODE(            \
                                                VMXMON_MAJOR_VERSION,   \
                                                VMXMON_API_VERSION,     \
                                                VMXMON_RELEASE)

#define VMXMON_STRINGIFY(A) #A
#define VMXMON_STRING(A) VMXMON_STRINGIFY(A)
#define VMXMON_VERSION_STRING VMXMON_STRING(VMXMON_MAJOR_VERSION) "." \
                              VMXMON_STRING(VMXMON_API_VERSION) "." \
                              VMXMON_STRING(VMXMON_RELEASE)

#endif  /* VERSION_H */
