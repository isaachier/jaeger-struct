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

#ifndef JAEGER_STRUCT_COMPILER_COMPLEX_TYPE_H
#define JAEGER_STRUCT_COMPILER_COMPLEX_TYPE_H

#include <vector>

#include <jaeger-struct/compiler/Field.h>
#include <jaeger-struct/compiler/Type.h>

namespace google {
namespace protobuf {
namespace io {

class Printer;

}  // namespace io
}  // namespace protobuf
}  // namespace google

namespace jaeger_struct {
namespace compiler {

class ComplexType : public Type {
  public:
    ComplexType(const std::string& name, const std::vector<Field>& fields)
        : _name(name)
        , _fields(fields)
    {
    }

    virtual ~ComplexType() = default;

    std::string name() const override { return _name; }

    const std::vector<Field>& fields() const { return _fields; }

    virtual void
    writeDefinition(google::protobuf::io::Printer& printer) const = 0;

  protected:
    void writeBracedDefinition(google::protobuf::io::Printer& printer) const;

  private:
    std::string _name;
    std::vector<Field> _fields;
};

}  // namespace compiler
}  // namespace jaeger_struct

#endif  // JAEGER_STRUCT_COMPILER_COMPLEX_TYPE_H
