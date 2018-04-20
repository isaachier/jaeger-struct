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

#ifndef JAEGER_STRUCT_COMPILER_GENERATOR
#define JAEGER_STRUCT_COMPILER_GENERATOR

#include <google/protobuf/compiler/code_generator.h>

namespace jaeger_struct {
namespace compiler {

class Generator : public google::protobuf::compiler::CodeGenerator {
  public:
    bool Generate(const google::protobuf::FileDescriptor* file,
                  const std::string& parameter,
                  google::protobuf::compiler::GeneratorContext* context,
                  std::string* error) const override;
};

}  // namespace compiler
}  // namespace jaeger_struct

#endif  // JAEGER_STRUCT_COMPILER_GENERATOR
