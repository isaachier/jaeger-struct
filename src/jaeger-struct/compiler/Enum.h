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

#ifndef JAEGER_STRUCT_COMPILER_ENUM_H
#define JAEGER_STRUCT_COMPILER_ENUM_H

#include <set>
#include <string>

#include <jaeger-struct/compiler/Type.h>

namespace google {
namespace protobuf {
namespace io {

class Printer;

}  // namespace io

class EnumDescriptor;

}  // namespace protobuf
}  // namespace google

namespace jaeger_struct {
namespace compiler {

class Enum : public Type {
  public:
    struct Value {
      public:
        Value(const std::string& name, int value)
            : _name(name)
            , _value(value)
        {
        }

        friend bool operator<(const Value& lhs, const Value& rhs)
        {
            return lhs._value < rhs._value;
        }

      private:
        std::string _name;
        int _value;
    };

    explicit Enum(const google::protobuf::EnumDescriptor& descriptor);

    std::string name() const override { return _name; }

  private:
    std::string _name;
    std::set<Value> _values;
};

}  // namespace compiler
}  // namespace jaeger_struct

#endif  // JAEGER_STRUCT_COMPILER_ENUM_H
