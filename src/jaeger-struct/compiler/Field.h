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

#ifndef JAEGER_STRUCT_COMPILER_FIELD_H
#define JAEGER_STRUCT_COMPILER_FIELD_H

#include <memory>
#include <string>

#include <jaeger-struct/compiler/Type.h>

namespace google {
namespace protobuf {
namespace io {

class Printer;

}  // namespace io

class FieldDescriptor;

}  // namespace protobuf
}  // namespace google

namespace jaeger_struct {
namespace compiler {

class TypeRegistry;

class Field {
  public:
    Field(const google::protobuf::FieldDescriptor& descriptor,
          const TypeRegistry& registry);

    const std::string& name() const { return _name; }

    void writeDefinition(google::protobuf::io::Printer& printer) const;

  private:
    std::shared_ptr<const Type> _type;
    int _repetition;
    std::string _name;
};

}  // namespace compiler
}  // namespace jaeger_struct

#endif  // JAEGER_STRUCT_COMPILER_FIELD_H
