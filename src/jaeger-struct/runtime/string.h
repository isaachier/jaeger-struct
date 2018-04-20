/*
 * Copyright (c) 2018 Uber Technologies, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef JAEGER_STRUCT_RUNTIME_STRING_H
#define JAEGER_STRUCT_RUNTIME_STRING_H

#include <jaeger-struct/runtime/common.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct jaeger_string {
    size_t len;
    char buffer[];
} jaeger_string;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* JAEGER_STRUCT_RUNTIME_STRING_H */
