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

#ifndef JAEGER_STRUCT_COMPILER_STRUCT_H
#define JAEGER_STRUCT_COMPILER_STRUCT_H

#include <vector>

#include <jaeger-struct/compiler/ComplexType.h>

namespace google {
namespace protobuf {

class Descriptor;

}  // namespace protobuf
}  // namespace google

namespace jaeger_struct {
namespace compiler {

class TypeRegistry;

class Struct : public ComplexType {
  public:
    Struct(const google::protobuf::Descriptor& descriptor,
           const TypeRegistry& registry);

    void writeDefinition(google::protobuf::io::Printer& printer) const override;
};

}  // namespace compiler
}  // namespace jaeger_struct

#endif  // JAEGER_STRUCT_COMPILER_STRUCT_H
