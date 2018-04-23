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

#include <jaeger-struct/compiler/Struct.h>

#include <unordered_set>

#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/printer.h>

#include <jaeger-struct/compiler/Strings.h>

namespace jaeger_struct {
namespace compiler {
namespace {

std::vector<Field>
determineFields(const google::protobuf::Descriptor& descriptor,
                const TypeRegistry& registry)
{
    std::unordered_set<const google::protobuf::FieldDescriptor*> unionFields;
    for (auto i = 0, len = descriptor.oneof_decl_count(); i < len; i++) {
        auto&& oneOf = *descriptor.oneof_decl(i);
        for (auto j = 0, oneOfLen = oneOf.field_count(); j < oneOfLen; j++) {
            unionFields.insert(oneOf.field(j));
        }
    }

    std::unordered_set<const google::protobuf::FieldDescriptor*> fields;
    for (auto i = 0, len = descriptor.field_count(); i < len; i++) {
        auto&& field = *descriptor.field(i);
        fields.insert(&field);
    }

    for (auto&& unionField : unionFields) {
        auto itr = fields.find(unionField);
        assert(itr != std::end(fields));
        fields.erase(itr);
    }

    std::vector<const google::protobuf::FieldDescriptor*> sortedFields(
        fields.cbegin(), fields.cend());
    std::sort(std::begin(sortedFields),
              std::end(sortedFields),
              [](const google::protobuf::FieldDescriptor* lhs,
                 const google::protobuf::FieldDescriptor* rhs) {
                  return lhs->number() < rhs->number();
              });

    std::vector<Field> result;
    result.reserve(sortedFields.size());
    std::transform(
        std::begin(sortedFields),
        std::end(sortedFields),
        std::back_inserter(result),
        [&registry](const google::protobuf::FieldDescriptor* descriptor) {
            return Field(*descriptor, registry);
        });
    return result;
}

}  // anonymous namespace

Struct::Struct(const google::protobuf::Descriptor& descriptor,
               const TypeRegistry& registry)
    : ComplexType(snakeCase(descriptor.full_name()),
                  determineFields(descriptor, registry))
{
}

}  // namespace compiler
}  // namespace jaeger_struct
