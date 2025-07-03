// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*****************************************************************************

  Licensed to Accellera Systems Initiative Inc. (Accellera) under one or
  more contributor license agreements.  See the NOTICE file distributed
  with this work for additional information regarding copyright ownership.
  Accellera licenses this file to you under the Apache License, Version 2.0
  (the "License"); you may not use this file except in compliance with the
  License.  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
  implied.  See the License for the specific language governing
  permissions and limitations under the License.

 *****************************************************************************/

#ifndef SYSTEMC_CHECKPOINT_SERIALIZATION_SERIALIZER_MACROS_H
#define SYSTEMC_CHECKPOINT_SERIALIZATION_SERIALIZER_MACROS_H

#ifndef SC_INCLUDE_FX
#define SC_INCLUDE_FX
#endif

#include <systemc-checkpoint/serialization/smd.h>
#include <systemc-checkpoint/serialization/vector.h>

#include <vector>

#define LOAD_TYPE_FROM_BIT_VECTOR(archiver, type)                              \
do {                                                                           \
    /* TODO(dtschanzx): bug 24025 */                                           \
    std::vector<int> v;                                                        \
    int length = 0;                                                            \
    ar & SMD(length);                                                          \
    ar & SMD(v);                                                               \
    for (int i = 0; i < length; ++i)                                           \
        type[i] = v[i];                                                        \
} while (0)

#define SAVE_TYPE_TO_BIT_VECTOR_WITH_LENGTH(archiver, type, length)            \
do {                                                                           \
    /* TODO(dtschanzx): bug 24025 */                                           \
    std::vector<int> v(length);                                                \
    for (int i = 0; i < length; ++i)                                           \
        v[i] = type[i];                                                        \
                                                                               \
    ar & SMD(length);                                                          \
    ar & SMD(v);                                                               \
} while (0)

#define SAVE_TYPE_TO_BIT_VECTOR(archiver, type)                                \
do {                                                                           \
    int length = type.wl();                                                    \
    SAVE_TYPE_TO_BIT_VECTOR_WITH_LENGTH(archiver, type, length);               \
} while (0)

#endif
