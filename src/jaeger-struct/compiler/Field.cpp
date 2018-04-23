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

#include <jaeger-struct/compiler/Field.h>

#include <sstream>
#include <stdexcept>

#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/printer.h>

#include <jaeger-struct/compiler/Strings.h>
#include <jaeger-struct/compiler/TypeRegistry.h>

namespace jaeger_struct {
namespace compiler {
namespace {

std::shared_ptr<const Type>
determineType(const google::protobuf::FieldDescriptor& field,
              const TypeRegistry& typeRegistry)
{
    std::shared_ptr<const Type> type;

#define TYPE_MAPPING(enumName, typeID)                                         \
    case google::protobuf::FieldDescriptor::CPPTYPE_##enumName:                \
        type = typeRegistry.findType(#typeID);                                 \
        break

    switch (field.cpp_type()) {
    case google::protobuf::FieldDescriptor::CPPTYPE_ENUM:
        type = typeRegistry.findType(
            snakeCase(makeIdentifier(field.enum_type()->full_name())));
        break;
    case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
        type = typeRegistry.findType(
            snakeCase(makeIdentifier(field.message_type()->full_name())));
        break;
    default:
        break;
    }

    return type;

#undef TYPE_MAPPING
}

}  // anonymous namespace

Field::Field(const google::protobuf::FieldDescriptor& descriptor,
             const TypeRegistry& registry)
    : _type(determineType(descriptor, registry))
    , _name(snakeCase(descriptor.name()))
{
}

}  // namespace compiler
}  // namespace jaeger_struct
