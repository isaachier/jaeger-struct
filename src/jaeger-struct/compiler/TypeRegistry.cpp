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

#include <jaeger-struct/compiler/TypeRegistry.h>

#include <jaeger-struct/compiler/FundamentalType.h>

namespace jaeger_struct {
namespace compiler {

#define FUNDAMENTAL_TYPE(typeID)                                               \
    {                                                                          \
#typeID, std::static_pointer_cast < Type >                             \
                     (std::make_shared <FundamentalType>(#typeID))             \
    }

TypeRegistry::TypeRegistry()
    : _typeRegistry({ FUNDAMENTAL_TYPE(bool),
                      FUNDAMENTAL_TYPE(float),
                      FUNDAMENTAL_TYPE(double),
                      FUNDAMENTAL_TYPE(int32_t),
                      FUNDAMENTAL_TYPE(int64_t),
                      FUNDAMENTAL_TYPE(uint32_t),
                      FUNDAMENTAL_TYPE(uint64_t),
                      FUNDAMENTAL_TYPE(jaeger_string) })
{
}

}  // namespace compiler
}  // namespace jaeger_struct
